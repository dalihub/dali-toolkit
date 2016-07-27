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
#include "effects-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/images/texture-set-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/filters/blur-two-pass-filter.h>
#include <dali-toolkit/internal/filters/emboss-filter.h>
#include <dali-toolkit/internal/filters/spread-filter.h>

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
  return EffectsView::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::EffectsView, Toolkit::Control, Create )
DALI_PROPERTY_REGISTRATION( Toolkit, EffectsView, "effectSize", INTEGER, EFFECT_SIZE )
DALI_ANIMATABLE_PROPERTY_REGISTRATION( Toolkit, EffectsView, "effectOffset", VECTOR3, EFFECT_OFFSET )
DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT( Toolkit, EffectsView, "effectColor", Color::WHITE, EFFECT_COLOR )
DALI_TYPE_REGISTRATION_END()

const Pixel::Format EFFECTS_VIEW_DEFAULT_PIXEL_FORMAT = Pixel::RGBA8888;
const float         ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;
const Vector4       EFFECTS_VIEW_DEFAULT_BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 0.0 );
const bool          EFFECTS_VIEW_REFRESH_ON_DEMAND(false);

#define DALI_COMPOSE_SHADER(STR) #STR

const char* EFFECTS_VIEW_VERTEX_SOURCE = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec3 effectOffset;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition.xyz += effectOffset;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = aPosition + vec2(0.5);\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* EFFECTS_VIEW_FRAGMENT_SOURCE = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 effectColor;\n
  \n
  void main()\n
  {\n
     gl_FragColor = effectColor;\n
     gl_FragColor.a *= texture2D( sTexture, vTexCoord).a;\n
  }\n
);

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
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ),
  mChildrenRoot(Actor::New()),
  mBackgroundColor( EFFECTS_VIEW_DEFAULT_BACKGROUND_COLOR ),
  mTargetSize( Vector2::ZERO ),
  mLastSize( Vector2::ZERO ),
  mEffectSize(0),
  mEffectType( Toolkit::EffectsView::INVALID_TYPE ),
  mPixelFormat( EFFECTS_VIEW_DEFAULT_PIXEL_FORMAT ),
  mEnabled( false ),
  mRefreshOnDemand(EFFECTS_VIEW_REFRESH_ON_DEMAND)
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
    RemoveFilters();

    Actor self = Self();
    Property::Map rendererMap;
    rendererMap.Insert( "rendererType", "IMAGE" );

    switch( type )
    {
      case Toolkit::EffectsView::DROP_SHADOW:
      {
        mFilters.PushBack( new SpreadFilter );
        mFilters.PushBack( new BlurTwoPassFilter );
        break;
      }
      case Toolkit::EffectsView::EMBOSS:
      {
        mFilters.PushBack( new SpreadFilter );
        mFilters.PushBack( new EmbossFilter );
        mFilters.PushBack( new BlurTwoPassFilter );
        break;
      }
      default:
      {
        break;
      }
    }

    Property::Map customShader;
    customShader[ "vertexShader" ] = EFFECTS_VIEW_VERTEX_SOURCE;
    customShader[ "fragmentShader" ] = EFFECTS_VIEW_FRAGMENT_SOURCE;
    rendererMap[ "shader" ] = customShader;
    InitializeVisual( self, mVisualPostFilter, rendererMap );

    mEffectType = type;
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
  mEnabled = true;
}

void EffectsView::Disable()
{
  // stop render tasks processing
  // Note: render target resources are automatically freed since we set the Image::Unused flag
  RemoveRenderTasks();
  mEnabled = false;
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

void EffectsView::SetBackgroundColor( const Vector4& color )
{
  mBackgroundColor = color;
}

Vector4 EffectsView::GetBackgroundColor() const
{
  return mBackgroundColor;
}

void EffectsView::SetEffectSize( int effectSize )
{
  mEffectSize = effectSize;

  if( mEnabled )
  {
    const size_t numFilters( mFilters.Size() );
    for( size_t i = 0; i < numFilters; ++i )
    {
      mFilters[i]->Disable();
    }

    SetupFilters();

    for( size_t i = 0; i < numFilters; ++i )
    {
      mFilters[i]->Enable();
    }
  }
}

int EffectsView::GetEffectSize()
{
  return mEffectSize;
}

// From Control
void EffectsView::OnInitialize()
{
  CustomActor self = Self();
  mChildrenRoot.SetParentOrigin( ParentOrigin::CENTER );
  self.Add( mChildrenRoot );
}

void EffectsView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet( targetSize );

  mTargetSize = Vector2(targetSize);

  // if we are already on stage, need to update render target sizes now to reflect the new size of this actor
  if(mEnabled)
  {
    if( mLastSize != Vector2::ZERO )
    {
      Disable();
    }
    Enable();
  }

  mChildrenRoot.SetSize( targetSize );
}

void EffectsView::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );

  Enable();

  Actor self = Self();
  if( mVisualPostFilter )
  {
    mVisualPostFilter.SetOnStage( self );
  }
  if( mVisualForChildren )
  {
    mVisualForChildren.SetOnStage( self );
  }
}

void EffectsView::OnStageDisconnection()
{
  Disable();

  const size_t numFilters( mFilters.Size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Disable();
  }

  Actor self = Self();
  if( mVisualPostFilter )
  {
    mVisualPostFilter.SetOffStage( self );
  }
  if( mVisualForChildren )
  {
    mVisualForChildren.SetOffStage( self );
  }

  Control::OnStageDisconnection();
}

void EffectsView::OnChildAdd( Actor& child )
{
  Control::OnChildAdd( child );

  if( child != mChildrenRoot && child != mCameraForChildren )
  {
    mChildrenRoot.Add( child );
  }
}

void EffectsView::OnChildRemove( Actor& child )
{
  mChildrenRoot.Remove( child );

  Control::OnChildRemove( child );
}

void EffectsView::SetupFilters()
{
  switch( mEffectType )
  {
    case Toolkit::EffectsView::DROP_SHADOW:
    {
      SpreadFilter* spreadFilter = static_cast< SpreadFilter* >( mFilters[0] );
      spreadFilter->SetInputImage( mImageForChildren );
      spreadFilter->SetOutputImage( mImagePostFilter );
      spreadFilter->SetRootActor( mChildrenRoot );
      spreadFilter->SetBackgroundColor( mBackgroundColor );
      spreadFilter->SetPixelFormat( mPixelFormat );
      spreadFilter->SetSize( mTargetSize );
      spreadFilter->SetSpread( mEffectSize );

      BlurTwoPassFilter* blurFilter = static_cast< BlurTwoPassFilter* >( mFilters[1] );
      blurFilter->SetInputImage( mImagePostFilter );
      blurFilter->SetOutputImage( mImagePostFilter );
      blurFilter->SetRootActor( mChildrenRoot );
      blurFilter->SetBackgroundColor( mBackgroundColor );
      blurFilter->SetPixelFormat( mPixelFormat );
      blurFilter->SetSize( mTargetSize );

      const float* kernel(NULL);
      size_t kernelSize(0);
      switch( mEffectSize )
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
      spreadFilter->SetRootActor( mChildrenRoot );
      spreadFilter->SetBackgroundColor( mBackgroundColor );
      spreadFilter->SetPixelFormat( Pixel::RGBA8888 );
      spreadFilter->SetSize( mTargetSize );
      spreadFilter->SetSpread( mEffectSize );

      EmbossFilter* embossFilter = static_cast< EmbossFilter* >( mFilters[1] );
      embossFilter->SetInputImage( mImagePostFilter );
      embossFilter->SetOutputImage( mImagePostFilter );
      embossFilter->SetRootActor( mChildrenRoot );
      embossFilter->SetBackgroundColor( mBackgroundColor );
      embossFilter->SetPixelFormat( Pixel::RGBA8888 );
      embossFilter->SetSize( mTargetSize );

      BlurTwoPassFilter* blurFilter = static_cast< BlurTwoPassFilter* >( mFilters[2] );
      blurFilter->SetInputImage( mImagePostFilter );
      blurFilter->SetOutputImage( mImagePostFilter );
      blurFilter->SetRootActor( mChildrenRoot );
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

    Actor self( Self() );

    mImageForChildren = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Dali::Image::UNUSED );
    InitializeVisual( self, mVisualForChildren, mImageForChildren );
    mVisualForChildren.SetDepthIndex( DepthIndex::CONTENT+1 );

    mImagePostFilter = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Dali::Image::UNUSED );
    TextureSet textureSet = TextureSet::New();
    TextureSetImage( textureSet, 0u,  mImagePostFilter );
    self.GetRendererAt( 0 ).SetTextures( textureSet );
    mVisualPostFilter.SetDepthIndex( DepthIndex::CONTENT );

    SetupFilters();
  }
}

void EffectsView::SetupCameras()
{
  if( !mCameraForChildren )
  {
    // Create a camera for the children render, corresponding to its render target size
    mCameraForChildren = CameraActor::New(mTargetSize);
    mCameraForChildren.SetParentOrigin(ParentOrigin::CENTER);
    mCameraForChildren.SetInvertYAxis( true );
    Self().Add( mCameraForChildren );
  }
  else
  {
    // place the camera for the children render, corresponding to its render target size
    const float cameraPosScale( 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f) );
    mCameraForChildren.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    mCameraForChildren.SetNearClippingPlane(1.0f);
    mCameraForChildren.SetAspectRatio(mTargetSize.width / mTargetSize.height);
    mCameraForChildren.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
    mCameraForChildren.SetPosition(0.0f, 0.0f, mTargetSize.height * cameraPosScale);
    mCameraForChildren.SetZ( mTargetSize.height * cameraPosScale );
  }
}

void EffectsView::CreateRenderTasks()
{
  if( mTargetSize == Vector2::ZERO )
  {
    return;
  }
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // create render task to render our child actors to offscreen buffer
  mRenderTaskForChildren = taskList.CreateTask();
  mRenderTaskForChildren.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForChildren.SetSourceActor( mChildrenRoot );
  mRenderTaskForChildren.SetExclusive(true);
  mRenderTaskForChildren.SetInputEnabled( false );
  mRenderTaskForChildren.SetClearColor( mBackgroundColor );
  mRenderTaskForChildren.SetClearEnabled( true );
  mRenderTaskForChildren.SetTargetFrameBuffer( mImageForChildren );
  mRenderTaskForChildren.SetCameraActor(mCameraForChildren); // use camera that covers render target exactly

  // Enable image filters
  const size_t numFilters( mFilters.Size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Enable();
  }
}

void EffectsView::RemoveRenderTasks()
{
  if( mTargetSize == Vector2::ZERO )
  {
    return;
  }

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  taskList.RemoveTask(mRenderTaskForChildren);

  const size_t numFilters( mFilters.Size() );
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

  const size_t numFilters( mFilters.Size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    mFilters[i]->Refresh();
  }
}

void EffectsView::RemoveFilters()
{
  const size_t numFilters( mFilters.Size() );
  for( size_t i = 0; i < numFilters; ++i )
  {
    delete mFilters[i];
  }
  mFilters.Release();
}

void EffectsView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::EffectsView effectsView = Toolkit::EffectsView::DownCast( Dali::BaseHandle( object ) );

  if ( effectsView )
  {
    switch ( index )
    {
      case Toolkit::EffectsView::Property::EFFECT_SIZE:
      {
        int effectSize;
        if( value.Get( effectSize ) )
        {
          GetImpl( effectsView ).SetEffectSize( effectSize );
        }
        break;
      }
      default:
      {
        break;
      }
    }
  }
}

Property::Value EffectsView::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::EffectsView imageview = Toolkit::EffectsView::DownCast( Dali::BaseHandle( object ) );

  if ( imageview )
  {
    EffectsView& impl = GetImpl( imageview );
    switch ( propertyIndex )
    {
      case Toolkit::EffectsView::Property::EFFECT_SIZE:
      {
         value = impl.GetEffectSize();
        break;
      }
      default:
      {
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
