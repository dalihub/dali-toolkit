/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "super-blur-view-impl.h"

// EXTERNAL INCLUDES
#include <cmath>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

namespace //Unnamed namespace
{

using namespace Dali;

//Todo: make these properties instead of constants
const unsigned int GAUSSIAN_BLUR_DEFAULT_NUM_SAMPLES = 11;
const unsigned int GAUSSIAN_BLUR_NUM_SAMPLES_INCREMENTATION = 10;
const float GAUSSIAN_BLUR_BELL_CURVE_WIDTH = 4.5f;
const float GAUSSIAN_BLUR_BELL_CURVE_WIDTH_INCREMENTATION = 5.f;
const Pixel::Format GAUSSIAN_BLUR_RENDER_TARGET_PIXEL_FORMAT = Pixel::RGB888;
const float GAUSSIAN_BLUR_DOWNSAMPLE_WIDTH_SCALE = 0.5f;
const float GAUSSIAN_BLUR_DOWNSAMPLE_HEIGHT_SCALE = 0.5f;

/**
 * The constraint is used to blend the group of blurred images continuously with a unified blur strength property value which ranges from zero to one.
 */
struct ActorOpacityConstraint
{
  ActorOpacityConstraint(int totalImageNum, int currentImageIdx)
  {
    float rangeLength = 1.f / static_cast<float>( totalImageNum );
    float index = static_cast<float>( currentImageIdx );
    mRange = Vector2( index*rangeLength, (index+1.f)*rangeLength );
  }

  void operator()( float& current, const PropertyInputContainer& inputs )
  {
    float blurStrength = inputs[0]->GetFloat();
    if(blurStrength <= mRange.x)
    {
      current = 1.f;
    }
    else if(blurStrength > mRange.y)
    {
      current = 0.f;
    }
    else
    {
      current = ( mRange.y - blurStrength) / ( mRange.y - mRange.x );
    }
  }

  Vector2 mRange;
};

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const unsigned int DEFAULT_BLUR_LEVEL(5u); ///< The default blur level when creating SuperBlurView from the type registry

BaseHandle Create()
{
  return Toolkit::SuperBlurView::New( DEFAULT_BLUR_LEVEL );
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::SuperBlurView, Toolkit::Control, Create )

DALI_PROPERTY_REGISTRATION( SuperBlurView, "image", MAP, IMAGE )

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

SuperBlurView::SuperBlurView( unsigned int blurLevels )
: Control( CONTROL_BEHAVIOUR_NONE ),
  mBlurLevels( blurLevels ),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mResourcesCleared( true ),
  mTargetSize( Vector2::ZERO )
{
  DALI_ASSERT_ALWAYS( mBlurLevels > 0 && " Minimal blur level is one, otherwise no blur is needed" );
  mGaussianBlurView.assign( blurLevels, NULL );
  mBlurredImage.assign( blurLevels, FrameBufferImage() );
  mImageActors.assign( blurLevels + 1, ImageActor() );
}

SuperBlurView::~SuperBlurView()
{
}

Toolkit::SuperBlurView SuperBlurView::New( unsigned int blurLevels )
{
  //Create the implementation
  IntrusivePtr<SuperBlurView> superBlurView( new SuperBlurView( blurLevels ) );

  //Pass ownership to CustomActor via derived handle
  Toolkit::SuperBlurView handle( *superBlurView );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  superBlurView->Initialize();

  return handle;
}

void SuperBlurView::OnInitialize()
{
  mBlurStrengthPropertyIndex = Self().RegisterProperty( "blur-strength",0.f );

  DALI_ASSERT_ALWAYS( mImageActors.size() == mBlurLevels+1 && "must synchronize the ImageActor group if blur levels got changed " );
  for(unsigned int i=0; i<=mBlurLevels;i++)
  {
    mImageActors[i] = ImageActor::New(  );
    mImageActors[i].SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mImageActors[i].SetParentOrigin( ParentOrigin::CENTER );
    mImageActors[i].SetZ(-static_cast<float>(i)*0.01f);
    mImageActors[i].SetColorMode( USE_OWN_MULTIPLY_PARENT_ALPHA );
    Self().Add( mImageActors[i] );
  }

  for(unsigned int i=0; i < mBlurLevels; i++)
  {
    Constraint constraint = Constraint::New<float>( mImageActors[i], Actor::Property::COLOR_ALPHA, ActorOpacityConstraint(mBlurLevels, i) );
    constraint.AddSource( ParentSource( mBlurStrengthPropertyIndex ) );
    constraint.Apply();
  }

  Self().SetSize(Stage::GetCurrent().GetSize());
}

void SuperBlurView::SetImage(Image inputImage)
{
  DALI_ASSERT_ALWAYS( mImageActors.size() == mBlurLevels+1 && "must synchronize the ImageActor group if blur levels got changed " );
  DALI_ASSERT_ALWAYS( mBlurredImage.size() == mBlurLevels && "must synchronize the blurred image group if blur levels got changed " );

  ClearBlurResource();

  mImageActors[0].SetImage( inputImage );

  for(unsigned int i=1; i<=mBlurLevels;i++)
  {
    mImageActors[i].SetImage( mBlurredImage[i-1] );
  }

  BlurImage( 0,  inputImage);
  for(unsigned int i=1; i<mBlurLevels;i++)
  {
    BlurImage( i,  mBlurredImage[i-1]);
  }

  mResourcesCleared = false;
}

Property::Index SuperBlurView::GetBlurStrengthPropertyIndex() const
{
  return mBlurStrengthPropertyIndex;
}

void SuperBlurView::SetBlurStrength( float blurStrength )
{
  Self().SetProperty(mBlurStrengthPropertyIndex, blurStrength);
}

float SuperBlurView::GetCurrentBlurStrength() const
{
  float blurStrength;
  (Self().GetProperty( mBlurStrengthPropertyIndex )).Get(blurStrength);

  return blurStrength;
}

Toolkit::SuperBlurView::SuperBlurViewSignal& SuperBlurView::BlurFinishedSignal()
{
  return mBlurFinishedSignal;
}

Image SuperBlurView::GetBlurredImage( unsigned int level )
{
  DALI_ASSERT_ALWAYS( level>0 && level<=mBlurLevels );
  return mBlurredImage[level-1];
}

void SuperBlurView::BlurImage( unsigned int idx, Image image )
{
  DALI_ASSERT_ALWAYS( mGaussianBlurView.size()>idx );
  mGaussianBlurView[idx] = Toolkit::GaussianBlurView::New( GAUSSIAN_BLUR_DEFAULT_NUM_SAMPLES+GAUSSIAN_BLUR_NUM_SAMPLES_INCREMENTATION*idx,
                                                           GAUSSIAN_BLUR_BELL_CURVE_WIDTH + GAUSSIAN_BLUR_BELL_CURVE_WIDTH_INCREMENTATION*static_cast<float>(idx),
                                                           GAUSSIAN_BLUR_RENDER_TARGET_PIXEL_FORMAT,
                                                           GAUSSIAN_BLUR_DOWNSAMPLE_WIDTH_SCALE, GAUSSIAN_BLUR_DOWNSAMPLE_HEIGHT_SCALE, true );
  mGaussianBlurView[idx].SetParentOrigin(ParentOrigin::CENTER);
  mGaussianBlurView[idx].SetSize(mTargetSize);
  mGaussianBlurView[idx].SetUserImageAndOutputRenderTarget( image, mBlurredImage[idx] );
  if( idx == mBlurLevels-1 )
  {
    mGaussianBlurView[idx].FinishedSignal().Connect( this, &SuperBlurView::OnBlurViewFinished );
  }
  Stage::GetCurrent().Add( mGaussianBlurView[idx] );
  mGaussianBlurView[idx].ActivateOnce();
}

void SuperBlurView::OnBlurViewFinished( Toolkit::GaussianBlurView blurView )
{
  ClearBlurResource();
  Toolkit::SuperBlurView handle( GetOwner() );
  mBlurFinishedSignal.Emit( handle );
}

void SuperBlurView::ClearBlurResource()
{
  if( !mResourcesCleared )
  {
    DALI_ASSERT_ALWAYS( mGaussianBlurView.size() == mBlurLevels && "must synchronize the GaussianBlurView group if blur levels got changed " );
    for(unsigned int i=0; i<mBlurLevels;i++)
    {
      Stage::GetCurrent().Remove( mGaussianBlurView[i] );
      mGaussianBlurView[i].Deactivate();
      mGaussianBlurView[i].Reset();
    }
    mResourcesCleared = true;
  }
}

void SuperBlurView::OnControlSizeSet( const Vector3& targetSize )
{
  if( mTargetSize != Vector2(targetSize) )
  {
    mTargetSize = Vector2(targetSize);

    for(unsigned int i=0; i<mBlurLevels;i++)
    {
      float exponent = static_cast<float>(i+1);
      mBlurredImage[i] = FrameBufferImage::New( mTargetSize.width/std::pow(2.f,exponent) , mTargetSize.height/std::pow(2.f,exponent),
                                                GAUSSIAN_BLUR_RENDER_TARGET_PIXEL_FORMAT, Dali::Image::NEVER );
    }
  }
}

void SuperBlurView::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::SuperBlurView superBlurView = Toolkit::SuperBlurView::DownCast( Dali::BaseHandle( object ) );

  if( superBlurView )
  {
    SuperBlurView& superBlurViewImpl( GetImpl( superBlurView ) );

    if( propertyIndex == Toolkit::SuperBlurView::Property::IMAGE )
    {
      Dali::Image image = Scripting::NewImage( value );
      if ( image )
      {
        superBlurViewImpl.SetImage( image );
      }
      else
      {
        DALI_LOG_ERROR( "Cannot create image from property value\n" );
      }
    }
  }
}

Property::Value SuperBlurView::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::SuperBlurView pushButton = Toolkit::SuperBlurView::DownCast( Dali::BaseHandle( object ) );

  if( pushButton )
  {
    SuperBlurView& superBlurViewImpl( GetImpl( pushButton ) );

    if( propertyIndex == Toolkit::SuperBlurView::Property::IMAGE )
    {
      Property::Map map;
      if( !superBlurViewImpl.mImageActors.empty() && superBlurViewImpl.mImageActors[0] )
      {
        Scripting::CreatePropertyMap( superBlurViewImpl.mImageActors[0], map );
      }
      value = Property::Value( map );
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
