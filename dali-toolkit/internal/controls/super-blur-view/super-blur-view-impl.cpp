/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL_INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

namespace //Unnamed namespace
{

using namespace Dali;

//Todo: make these properties instead of constants
const unsigned int GAUSSIAN_BLUR_DEFAULT_NUM_SAMPLES = 11;
const unsigned int GAUSSIAN_BLUR_NUM_SAMPLES_INCREMENTATION = 10;
const float GAUSSIAN_BLUR_BELL_CURVE_WIDTH = 4.5f;
const float GAUSSIAN_BLUR_BELL_CURVE_WIDTH_INCREMENTATION = 5.f;
const Pixel::Format GAUSSIAN_BLUR_RENDER_TARGET_PIXEL_FORMAT = Pixel::RGBA8888;
const float GAUSSIAN_BLUR_DOWNSAMPLE_WIDTH_SCALE = 0.5f;
const float GAUSSIAN_BLUR_DOWNSAMPLE_HEIGHT_SCALE = 0.5f;

const char* ALPHA_UNIFORM_NAME( "uAlpha" );
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp float uAlpha;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
    gl_FragColor.a *= uAlpha;
  }\n
);

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
    if(blurStrength < mRange.x)
    {
      current = 0.f;
    }
    else if(blurStrength > mRange.y)
    {
      current = 1.f;
    }
    else
    {
      current = ( blurStrength - mRange.x) / ( mRange.y - mRange.x );
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

DALI_PROPERTY_REGISTRATION( Toolkit, SuperBlurView, "image", MAP, IMAGE )

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

SuperBlurView::SuperBlurView( unsigned int blurLevels )
: Control( ControlBehaviour( DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mTargetSize( Vector2::ZERO ),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mBlurLevels( blurLevels ),
  mResourcesCleared( true )
{
  DALI_ASSERT_ALWAYS( mBlurLevels > 0 && " Minimal blur level is one, otherwise no blur is needed" );
  mGaussianBlurView.assign( blurLevels, Toolkit::GaussianBlurView() );
  mBlurredImage.assign( blurLevels, FrameBufferImage() );
  mVisuals.assign( blurLevels+1, Toolkit::Visual::Base() );
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
  mBlurStrengthPropertyIndex = Self().RegisterProperty( "blurStrength", 0.f );
}

void SuperBlurView::SetImage(Image inputImage)
{
  if( mTargetSize == Vector2::ZERO || mInputImage == inputImage)
  {
    return;
  }

  ClearBlurResource();

  mInputImage = inputImage;
  Actor self( Self() );

  mVisuals[0] = Toolkit::VisualFactory::Get().CreateVisual( mInputImage );
  RegisterVisual( 0, mVisuals[0] ); // Will clean up previously registered visuals for this index.
  mVisuals[0].SetDepthIndex(0);
  SetShaderEffect( mVisuals[0] );

  BlurImage( 0,  inputImage);
  for(unsigned int i=1; i<mBlurLevels;i++)
  {
    BlurImage( i, mBlurredImage[i-1]);
  }

  mResourcesCleared = false;
}

Image SuperBlurView::GetImage()
{
  return mInputImage;
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
  Stage::GetCurrent().Add( mGaussianBlurView[idx] );
  mGaussianBlurView[idx].SetUserImageAndOutputRenderTarget( image, mBlurredImage[idx] );
  mGaussianBlurView[idx].ActivateOnce();
  if( idx == mBlurLevels-1 )
  {
    mGaussianBlurView[idx].FinishedSignal().Connect( this, &SuperBlurView::OnBlurViewFinished );
  }
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
    }
    mResourcesCleared = true;
  }
}
void SuperBlurView::SetShaderEffect( Toolkit::Visual::Base& visual )
{
  Property::Map shaderMap;
  std::stringstream verterShaderString;
  shaderMap[ "fragmentShader" ] = FRAGMENT_SHADER;

  Internal::Visual::Base& rendererImpl = Toolkit::GetImplementation( visual );
  rendererImpl.SetCustomShader( shaderMap );
}

void SuperBlurView::OnSizeSet( const Vector3& targetSize )
{
  if( mTargetSize != Vector2(targetSize) )
  {
    mTargetSize = Vector2(targetSize);

    Actor self = Self();
    for( unsigned int i = 1; i <= mBlurLevels; i++ )
    {
      float exponent = static_cast<float>(i);
      mBlurredImage[i-1] = FrameBufferImage::New( mTargetSize.width/std::pow(2.f,exponent) , mTargetSize.height/std::pow(2.f,exponent),
                                                GAUSSIAN_BLUR_RENDER_TARGET_PIXEL_FORMAT );

      mVisuals[i] = Toolkit::VisualFactory::Get().CreateVisual( mBlurredImage[i - 1] );
      RegisterVisual( i, mVisuals[i] ); // Will clean up existing visual with same index.
      mVisuals[i].SetDepthIndex( i );
      SetShaderEffect( mVisuals[i] );
    }

    if( mInputImage )
    {
      SetImage( mInputImage );
    }
  }
}

void SuperBlurView::OnStageConnection( int depth )
{
  // Chaining up first ensures visuals have SetOnStage called to create their renderers
  Control::OnStageConnection( depth );

  if( mTargetSize == Vector2::ZERO )
  {
    return;
  }

  Actor self = Self();
  for(unsigned int i=1; i<=mBlurLevels;i++)
  {
    Renderer renderer = self.GetRendererAt( i );
    Property::Index index = renderer.RegisterProperty( ALPHA_UNIFORM_NAME, 0.f );
    Constraint constraint = Constraint::New<float>( renderer, index, ActorOpacityConstraint(mBlurLevels, i-1) );
    constraint.AddSource( Source( self, mBlurStrengthPropertyIndex ) );
    constraint.Apply();
  }
}

void SuperBlurView::OnStageDisconnection( )
{
  Control::OnStageDisconnection();
}

Vector3 SuperBlurView::GetNaturalSize()
{
  if( mInputImage )
  {
    return Vector3( mInputImage.GetWidth(), mInputImage.GetHeight(), 0.f );
  }
  return Vector3::ZERO;
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

  Toolkit::SuperBlurView blurView = Toolkit::SuperBlurView::DownCast( Dali::BaseHandle( object ) );

  if( blurView )
  {
    SuperBlurView& superBlurViewImpl( GetImpl( blurView ) );

    if( propertyIndex == Toolkit::SuperBlurView::Property::IMAGE )
    {
      Property::Map map;
      Image inputImage = superBlurViewImpl.GetImage();
      if( inputImage )
      {
        Scripting::CreatePropertyMap( inputImage, map );
      }
      value = Property::Value( map );
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
