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
#include "effects-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include "../../filters/blur-two-pass-filter.h"
#include "../../filters/emboss-filter.h"
#include "../../filters/spread-filter.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

Dali::BaseHandle Create()
{
  return Toolkit::EffectsView::New();
}

Dali::TypeRegistration mType( typeid(Dali::Toolkit::EffectsView), typeid(Dali::Toolkit::Control), Create );

const Pixel::Format EFFECTS_VIEW_DEFAULT_PIXEL_FORMAT = Pixel::RGBA8888;
const float         ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;
const Vector4       EFFECTS_VIEW_DEFAULT_BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 0.0 );
const bool          EFFECTS_VIEW_REFRESH_ON_DEMAND(false);
// custom properties
const float         EFFECT_SIZE_DEFAULT( 1.0f );
const std::string   EFFECT_SIZE_PROPERTY_NAME( "EffectSize" );
const float         EFFECT_STRENGTH_DEFAULT( 0.5f );
const std::string   EFFECT_STRENGTH_PROPERTY_NAME( "EffectStrength" );
const Vector3       EFFECT_OFFSET_DEFAULT( 0.0f, 0.0f, 0.0f );
const std::string   EFFECT_OFFSET_PROPERTY_NAME( "EffectOffset" );
const Vector4       EFFECT_COLOR_DEFAULT( Color::WHITE );
const std::string   EFFECT_COLOR_PROPERTY_NAME( "EffectColor" );

const char* const EFFECTS_VIEW_FRAGMENT_SOURCE =
    "void main()\n"
    "{\n"
    "  gl_FragColor = uColor;\n"
    "  gl_FragColor.a *= texture2D( sTexture, vTexCoord).a;\n"
    "}\n";

const float BLUR_KERNEL0[] = { 12.0f/16.0f,
                               2.0f/16.0f, 2.0f/16.0f };

const float BLUR_KERNEL1[] = { 8.0f/16.0f,
                               4.0f/16.0f, 4.0f/16.0f };

const float BLUR_KERNEL2[] = { 6.0f/16.0f,
                               2.5f/16.0f, 2.5f/16.0f,
                               1.5f/16.0f, 1.5f/16.0f,
                               1.0f/16.0f, 1.0f/16.0f };

const float BLUR_KERNEL3[] = { 4.0f/16.0f,
                               3.0f/16.0f, 2.0f/16.0f,
                               2.0f/16.0f, 2.0f/16.0f,
                               1.0f/16.0f, 1.0f/16.0f };

const float BLUR_KERNEL4[] = { 3.0f/16.0f,
                               2.5f/16.0f,  2.5f/16.0f,
                               1.75f/16.0f, 1.75f/16.0f,
                               1.25f/16.0f, 1.25f/16.0f,
                               1.0f/16.0f,  1.0f/16.0f };

} // namespace

Toolkit::EffectsView EffectsView::New()
{
  EffectsView* effectsView = new EffectsView;

  Toolkit::EffectsView handle = Toolkit::EffectsView( *effectsView );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  effectsView->Initialize();

  return handle;
}

EffectsView::EffectsView()
: Control( CONTROL_BEHAVIOUR_NONE ),
  mEffectType( Toolkit::EffectsView::INVALID_TYPE ),
  mPixelFormat( EFFECTS_VIEW_DEFAULT_PIXEL_FORMAT ),
  mSpread(0.0f),
  mBackgroundColor( EFFECTS_VIEW_DEFAULT_BACKGROUND_COLOR ),
  mTargetSize( Vector2::ZERO ),
  mLastSize( Vector2::ZERO ),
  mRefreshOnDemand(EFFECTS_VIEW_REFRESH_ON_DEMAND),
  mEffectSizePropertyIndex(Property::INVALID_INDEX),
  mEffectStrengthPropertyIndex(Property::INVALID_INDEX),
  mEffectOffsetPropertyIndex(Property::INVALID_INDEX),
  mEffectColorPropertyIndex(Property::INVALID_INDEX)
{
}

EffectsView::~EffectsView()
{
  RemoveFilters();
}

void EffectsView::SetType( Toolkit::EffectsView::EffectType type )
{
  if( mEffectType != type )
  {
    mEffectType = type;

    RemoveFilters();

    switch( mEffectType )
    {
      case Toolkit::EffectsView::DROP_SHADOW:
      {
        mFilters.push_back( new SpreadFilter );
        mFilters.push_back( new BlurTwoPassFilter );
        break;
      }
      case Toolkit::EffectsView::EMBOSS:
      {
        mFilters.push_back( new SpreadFilter );
        mFilters.push_back( new EmbossFilter );
        mFilters.push_back( new BlurTwoPassFilter );
        mActorPostFilter.RemoveShaderEffect();
        break;
      }
      default:
      {
        break;
      }
    }
  }
}

Toolkit::EffectsView::EffectType EffectsView::GetType() const
{
  return mEffectType;
}

void EffectsView::Enable()
{
  // make sure resources are allocated and start the render tasks processing
  AllocateResources();
  CreateRenderTasks();
}

void EffectsView::Disable()
{
  // stop render tasks processing
  // Note: render target resources are automatically freed since we set the Image::Unused flag
  RemoveRenderTasks();
}

void EffectsView::Refresh()
{
  RefreshRenderTasks();
}

void EffectsView::SetRefreshOnDemand( bool onDemand )
{
  mRefreshOnDemand = onDemand;

  RefreshRenderTasks();
}

void EffectsView::SetPixelFormat( Pixel::Format pixelFormat )
{
  mPixelFormat = pixelFormat;
}

void EffectsView::SetOutputImage( FrameBufferImage image )
{
  CustomActor self = Self();

  if( mImageForResult != image )
  {
    if( !image )
    {
      if( mImageForResult )
      {
        self.Remove( mActorForResult );
        mActorForResult.Reset();

        self.Add( mActorPostFilter );
        self.Add( mActorForChildren );
      }
    }
    else
    {
      if( mImageForResult )
      {
        self.Remove( mActorForResult );
      }
      mActorForResult = Actor::New();
      mActorForResult.SetParentOrigin( ParentOrigin::CENTER );
      mActorForResult.SetSize( mTargetSize );
      mActorForResult.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

      Self().Add( mActorForResult );
      mActorForResult.Add( mActorPostFilter );
      mActorForResult.Add( mActorForChildren );
    }
    mImageForResult = image;
  }
}

FrameBufferImage EffectsView::GetOutputImage()
{
  return mImageForResult;
}

Property::Index EffectsView::GetEffectSizePropertyIndex() const
{
  return mEffectSizePropertyIndex;
}

Property::Index EffectsView::GetEffectStrengthPropertyIndex() const
{
  return mEffectStrengthPropertyIndex;
}

Property::Index EffectsView::GetEffectOffsetPropertyIndex() const
{
  return mEffectOffsetPropertyIndex;
}

Property::Index EffectsView::GetEffectColorPropertyIndex() const
{
  return mEffectColorPropertyIndex;
}

void EffectsView::SetupProperties()
{
  CustomActor self = Self();

  // Register a property that the user can control the drop shadow offset
  mEffectSizePropertyIndex     = self.RegisterProperty(EFFECT_SIZE_PROPERTY_NAME, EFFECT_SIZE_DEFAULT, Property::READ_WRITE);
  mEffectStrengthPropertyIndex = self.RegisterProperty(EFFECT_STRENGTH_PROPERTY_NAME, EFFECT_STRENGTH_DEFAULT, Property::READ_WRITE);
  mEffectOffsetPropertyIndex   = self.RegisterProperty(EFFECT_OFFSET_PROPERTY_NAME, EFFECT_OFFSET_DEFAULT);
  mEffectColorPropertyIndex    = self.RegisterProperty(EFFECT_COLOR_PROPERTY_NAME, EFFECT_COLOR_DEFAULT);
  mActorPostFilter.ApplyConstraint( Constraint::New<Vector3>( Actor::Property::Position, Source( self, mEffectOffsetPropertyIndex ), EqualToConstraint() ) );
  mActorPostFilter.ApplyConstraint( Constraint::New<Vector4>( Actor::Property::Color, Source( self, mEffectColorPropertyIndex ), EqualToConstraint() ) );
}

void EffectsView::SetBackgroundColor( const Vector4& color )
{
  mBackgroundColor = color;
}

Vector4 EffectsView::GetBackgroundColor() const
{
  return mBackgroundColor;
}

// From Control
void EffectsView::OnInitialize()
{
  //////////////////////////////////////////////////////
  // Create cameras
  mCameraForChildren = CameraActor::New();
  mCameraForChildren.SetParentOrigin(ParentOrigin::CENTER);

  mActorForChildren = ImageActor::New();
  mActorForChildren.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
  mActorForChildren.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

  mActorPostFilter = ImageActor::New();
  mActorPostFilter.SetParentOrigin( ParentOrigin::CENTER );
  mActorPostFilter.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );
  mActorPostFilter.SetShaderEffect( ShaderEffect::New( "", EFFECTS_VIEW_FRAGMENT_SOURCE ) );

  // Connect to actor tree
  Self().Add( mActorPostFilter );
  Self().Add( mActorForChildren );
  Self().Add( mCameraForChildren );

  SetupProperties();
}

void EffectsView::OnControlSizeSet(const Vector3& targetSize)
{
  mTargetSize = Vector2(targetSize);

  // if we are already on stage, need to update render target sizes now to reflect the new size of this actor
  if(Self().OnStage())
  {
    AllocateResources();
  }

  if( mActorForResult )
  {
    mActorForResult.SetSize( targetSize );
  }
  if( mActorForChildren )
  {
    mActorForChildren.SetSize( targetSize );
  }
  if( mActorPostFilter )
  {
    mActorPostFilter.SetSize( targetSize );
  }

  // Children render camera must move when EffectsView object is resized.
  // This is since we cannot change render target size - so we need to remap the child actors' rendering
  // accordingly so they still exactly fill the render target.
  // Note that this means the effective resolution of the child render changes as the EffectsView object
  // changes size, this is the trade off for not being able to modify render target size
  // Change camera z position based on EffectsView actor height
  if( mCameraForChildren )
  {
    const float cameraPosScale( 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f) );
    mCameraForChildren.SetZ( targetSize.height * cameraPosScale );
  }

  const size_t numFilters( mFilters.size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->SetSize( mTargetSize );
  }

}

void EffectsView::OnStageDisconnection()
{
  const size_t numFilters( mFilters.size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Disable();
  }
}

void EffectsView::SetupFilters()
{
  int effectSize = static_cast< int >( Self().GetProperty( mEffectSizePropertyIndex ).Get<float>() );
  switch( mEffectType )
  {
    case Toolkit::EffectsView::DROP_SHADOW:
    {
      SpreadFilter* spreadFilter = static_cast< SpreadFilter* >( mFilters[0] );
      spreadFilter->SetInputImage( mImageForChildren );
      spreadFilter->SetOutputImage( mImagePostFilter );
      spreadFilter->SetRootActor( Self() );
      spreadFilter->SetBackgroundColor( mBackgroundColor );
      spreadFilter->SetPixelFormat( mPixelFormat );
      spreadFilter->SetSize( mTargetSize );
      spreadFilter->SetSpread( effectSize );

      BlurTwoPassFilter* blurFilter = static_cast< BlurTwoPassFilter* >( mFilters[1] );
      blurFilter->SetInputImage( mImagePostFilter );
      blurFilter->SetOutputImage( mImagePostFilter );
      blurFilter->SetRootActor( Self() );
      blurFilter->SetBackgroundColor( mBackgroundColor );
      blurFilter->SetPixelFormat( mPixelFormat );
      blurFilter->SetSize( mTargetSize );

      const float* kernel(NULL);
      size_t kernelSize(0);
      switch( effectSize )
      {
        case 4:  {  kernel = BLUR_KERNEL4; kernelSize = sizeof(BLUR_KERNEL4)/sizeof(BLUR_KERNEL4[0]); break; }
        case 3:  {  kernel = BLUR_KERNEL3; kernelSize = sizeof(BLUR_KERNEL3)/sizeof(BLUR_KERNEL3[0]); break; }
        case 2:  {  kernel = BLUR_KERNEL2; kernelSize = sizeof(BLUR_KERNEL2)/sizeof(BLUR_KERNEL2[0]); break; }
        case 1:  {  kernel = BLUR_KERNEL1; kernelSize = sizeof(BLUR_KERNEL1)/sizeof(BLUR_KERNEL1[0]); break; }
        case 0:
        default: {  kernel = BLUR_KERNEL0; kernelSize = sizeof(BLUR_KERNEL0)/sizeof(BLUR_KERNEL0[0]); break; }
      }
      blurFilter->CreateKernel( kernel, kernelSize );
      break;
    }
    case Toolkit::EffectsView::EMBOSS:
    {
      SpreadFilter* spreadFilter = static_cast< SpreadFilter* >( mFilters[0] );
      spreadFilter->SetInputImage( mImageForChildren );
      spreadFilter->SetOutputImage( mImagePostFilter );
      spreadFilter->SetRootActor( Self() );
      spreadFilter->SetBackgroundColor( mBackgroundColor );
      spreadFilter->SetPixelFormat( Pixel::RGBA8888 );
      spreadFilter->SetSize( mTargetSize );
      spreadFilter->SetSpread( effectSize );

      EmbossFilter* embossFilter = static_cast< EmbossFilter* >( mFilters[1] );
      embossFilter->SetInputImage( mImagePostFilter );
      embossFilter->SetOutputImage( mImagePostFilter );
      embossFilter->SetRootActor( Self() );
      embossFilter->SetBackgroundColor( mBackgroundColor );
      embossFilter->SetPixelFormat( Pixel::RGBA8888 );
      embossFilter->SetSize( mTargetSize );

      BlurTwoPassFilter* blurFilter = static_cast< BlurTwoPassFilter* >( mFilters[2] );
      blurFilter->SetInputImage( mImagePostFilter );
      blurFilter->SetOutputImage( mImagePostFilter );
      blurFilter->SetRootActor( Self() );
      blurFilter->SetBackgroundColor( Vector4( 0.5f, 0.5f, 0.5f, 0.0 ) );
      blurFilter->SetPixelFormat( Pixel::RGBA8888 );
      blurFilter->SetSize( mTargetSize );
      blurFilter->CreateKernel( BLUR_KERNEL0, sizeof(BLUR_KERNEL0)/sizeof(BLUR_KERNEL0[0]) );

      break;
    }
    default:
    {
      break;
    }
  }
}
void EffectsView::AllocateResources()
{
  if(mTargetSize != mLastSize)
  {
    mLastSize = mTargetSize;

    SetupCameras();

    mImageForChildren = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Dali::Image::UNUSED );
    mActorForChildren.SetImage(mImageForChildren);

    mImagePostFilter = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Dali::Image::UNUSED );
    mActorPostFilter.SetImage(mImagePostFilter);

    SetupFilters();
  }
}

void EffectsView::SetupCameras()
{
  const float cameraPosScale( 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f) );

  // Create and place a camera for the children render, corresponding to its render target size
  mCameraForChildren.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
  // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
  mCameraForChildren.SetNearClippingPlane(1.0f);
  mCameraForChildren.SetAspectRatio(mTargetSize.width / mTargetSize.height);
  mCameraForChildren.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
  mCameraForChildren.SetPosition(0.0f, 0.0f, mTargetSize.height * cameraPosScale);
  mCameraForChildren.SetRotation(Quaternion(M_PI, Vector3::YAXIS));
  mCameraForChildren.SetZ( mTargetSize.height * cameraPosScale );
}

void EffectsView::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // create render task to render our child actors to offscreen buffer
  mRenderTaskForChildren = taskList.CreateTask();
  mRenderTaskForChildren.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForChildren.SetSourceActor( Self() );
  mRenderTaskForChildren.SetExclusive(true);
  mRenderTaskForChildren.SetInputEnabled( false );
  mRenderTaskForChildren.SetClearColor( mBackgroundColor );
  mRenderTaskForChildren.SetClearEnabled( true );
  mRenderTaskForChildren.SetTargetFrameBuffer( mImageForChildren );
  mRenderTaskForChildren.SetCameraActor(mCameraForChildren); // use camera that covers render target exactly

  // Enable image filters
  const size_t numFilters( mFilters.size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Enable();
  }

  // create render task to render result of the image filters to the final offscreen
  if( mImageForResult )
  {
    mRenderTaskForResult = taskList.CreateTask();
    mRenderTaskForResult.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
    mRenderTaskForResult.SetSourceActor( mActorForResult );
    mRenderTaskForResult.SetExclusive(true);
    mRenderTaskForResult.SetInputEnabled( false );
    mRenderTaskForResult.SetClearColor( mBackgroundColor );
    mRenderTaskForResult.SetClearEnabled( true );
    mRenderTaskForResult.SetTargetFrameBuffer( mImageForResult );
    mRenderTaskForResult.SetCameraActor(mCameraForChildren); // use camera that covers render target exactly
  }
}

void EffectsView::RemoveRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  taskList.RemoveTask(mRenderTaskForChildren);
  taskList.RemoveTask(mRenderTaskForResult);

  const size_t numFilters( mFilters.size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Disable();
  }
}

void EffectsView::RefreshRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  if( mRenderTaskForChildren )
  {
    mRenderTaskForChildren.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }

  if( mRenderTaskForResult )
  {
    mRenderTaskForResult.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }

  const size_t numFilters( mFilters.size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Refresh();
  }
}

void EffectsView::RemoveFilters()
{
  const size_t numFilters( mFilters.size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    delete mFilters[i];
  }
  mFilters.clear();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
