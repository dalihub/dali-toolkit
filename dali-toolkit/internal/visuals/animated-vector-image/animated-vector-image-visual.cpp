/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/animated-vector-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const Dali::Vector4 FULL_TEXTURE_RECT( 0.f, 0.f, 1.f, 1.f );

// stop behavior
DALI_ENUM_TO_STRING_TABLE_BEGIN( STOP_BEHAVIOR )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::DevelImageVisual::StopBehavior, CURRENT_FRAME )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::DevelImageVisual::StopBehavior, FIRST_FRAME )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::DevelImageVisual::StopBehavior, LAST_FRAME )
DALI_ENUM_TO_STRING_TABLE_END( STOP_BEHAVIOR )

// looping mode
DALI_ENUM_TO_STRING_TABLE_BEGIN( LOOPING_MODE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::DevelImageVisual::LoopingMode, RESTART )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::DevelImageVisual::LoopingMode, AUTO_REVERSE )
DALI_ENUM_TO_STRING_TABLE_END( LOOPING_MODE )

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_VECTOR_ANIMATION" );
#endif

} // unnamed namespace

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl, const Property::Map& properties )
{
  AnimatedVectorImageVisualPtr visual( new AnimatedVectorImageVisual( factoryCache, shaderFactory, imageUrl ) );
  visual->SetProperties( properties );

  return visual;
}

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
{
  AnimatedVectorImageVisualPtr visual( new AnimatedVectorImageVisual( factoryCache, shaderFactory, imageUrl ) );

  return visual;
}

AnimatedVectorImageVisual::AnimatedVectorImageVisual( VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, const VisualUrl& imageUrl )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, static_cast<Toolkit::Visual::Type>( Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE ) ),
  mUrl( imageUrl ),
  mAnimationData(),
  mVectorAnimationTask( new VectorAnimationTask( factoryCache, imageUrl.GetUrl() ) ),
  mImageVisualShaderFactory( shaderFactory ),
  mVisualSize(),
  mVisualScale( Vector2::ONE ),
  mPlacementActor(),
  mPlayState( DevelImageVisual::PlayState::STOPPED ),
  mEventCallback( nullptr ),
  mRendererAdded( false ),
  mCoreShutdown(false),
  mRedrawInScalingDown(true)
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;

  mVectorAnimationTask->UploadCompletedSignal().Connect( this, &AnimatedVectorImageVisual::OnUploadCompleted );
  mVectorAnimationTask->SetAnimationFinishedCallback( new EventThreadCallback( MakeCallback( this, &AnimatedVectorImageVisual::OnAnimationFinished ) ) );

  auto& vectorAnimationManager = mFactoryCache.GetVectorAnimationManager();
  vectorAnimationManager.AddObserver(*this);
}

AnimatedVectorImageVisual::~AnimatedVectorImageVisual()
{
  if( ! mCoreShutdown )
  {
    auto& vectorAnimationManager = mFactoryCache.GetVectorAnimationManager();
    vectorAnimationManager.RemoveObserver(*this);

    if( mEventCallback )
    {
      mFactoryCache.GetVectorAnimationManager().UnregisterEventCallback( mEventCallback );
    }

    // Finalize animation task and disconnect the signal in the main thread
    mVectorAnimationTask->UploadCompletedSignal().Disconnect( this, &AnimatedVectorImageVisual::OnUploadCompleted );
    mVectorAnimationTask->Finalize();
  }
}

void AnimatedVectorImageVisual::VectorAnimationManagerDestroyed()
{
  // Core is shutting down. Don't talk to the plugin any more.
  mCoreShutdown = true;
}

void AnimatedVectorImageVisual::GetNaturalSize( Vector2& naturalSize )
{
  if( mVisualSize != Vector2::ZERO )
  {
    naturalSize = mVisualSize;
  }
  else
  {
    uint32_t width, height;
    mVectorAnimationTask->GetDefaultSize( width, height );
    naturalSize.x = width;
    naturalSize.y = height;
  }

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::GetNaturalSize: w = %f, h = %f [%p]\n", naturalSize.width, naturalSize.height, this );
}

void AnimatedVectorImageVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE );
  if( mUrl.IsValid() )
  {
    map.Insert( Toolkit::ImageVisual::Property::URL, mUrl.GetUrl() );
  }
  map.Insert( Toolkit::DevelImageVisual::Property::LOOP_COUNT, mAnimationData.loopCount );

  uint32_t startFrame, endFrame;
  mVectorAnimationTask->GetPlayRange( startFrame, endFrame );

  Property::Array playRange;
  playRange.PushBack( static_cast< int32_t >( startFrame ) );
  playRange.PushBack( static_cast< int32_t >( endFrame ) );
  map.Insert( Toolkit::DevelImageVisual::Property::PLAY_RANGE, playRange );

  map.Insert( Toolkit::DevelImageVisual::Property::PLAY_STATE, static_cast< int32_t >( mPlayState ) );
  map.Insert( Toolkit::DevelImageVisual::Property::CURRENT_FRAME_NUMBER, static_cast< int32_t >( mVectorAnimationTask->GetCurrentFrameNumber() ) );
  map.Insert( Toolkit::DevelImageVisual::Property::TOTAL_FRAME_NUMBER, static_cast< int32_t >( mVectorAnimationTask->GetTotalFrameNumber() ) );

  map.Insert( Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, mAnimationData.stopBehavior );
  map.Insert( Toolkit::DevelImageVisual::Property::LOOPING_MODE, mAnimationData.loopingMode );
  map.Insert( Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, mRedrawInScalingDown );

  Property::Map layerInfo;
  mVectorAnimationTask->GetLayerInfo( layerInfo );
  map.Insert( Toolkit::DevelImageVisual::Property::CONTENT_INFO, layerInfo );
}

void AnimatedVectorImageVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

void AnimatedVectorImageVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // url already passed in from constructor
  for( Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter )
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue( iter );
    if( keyValue.first.type == Property::Key::INDEX )
    {
      DoSetProperty( keyValue.first.indexKey, keyValue.second );
    }
    else
    {
       if( keyValue.first == LOOP_COUNT_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::LOOP_COUNT, keyValue.second );
       }
       else if( keyValue.first == PLAY_RANGE_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::PLAY_RANGE, keyValue.second );
       }
       else if( keyValue.first == STOP_BEHAVIOR_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR, keyValue.second );
       }
       else if( keyValue.first == LOOPING_MODE_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::LOOPING_MODE, keyValue.second );
       }
       else if( keyValue.first == REDRAW_IN_SCALING_DOWN_NAME )
       {
          DoSetProperty( Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, keyValue.second );
       }
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::DoSetProperty( Property::Index index, const Property::Value& value )
{
  switch(index)
  {
    case Toolkit::DevelImageVisual::Property::LOOP_COUNT:
    {
      int32_t loopCount;
      if( value.Get( loopCount ) )
      {
        mAnimationData.loopCount = loopCount;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_LOOP_COUNT;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::PLAY_RANGE:
    {
      const Property::Array* array = value.GetArray();
      if( array )
      {
        mAnimationData.playRange = *array;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_RANGE;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::STOP_BEHAVIOR:
    {
      int32_t stopBehavior = mAnimationData.stopBehavior;
      if( Scripting::GetEnumerationProperty( value, STOP_BEHAVIOR_TABLE, STOP_BEHAVIOR_TABLE_COUNT, stopBehavior ) )
      {
        mAnimationData.stopBehavior = DevelImageVisual::StopBehavior::Type( stopBehavior );
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_STOP_BEHAVIOR;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::LOOPING_MODE:
    {
      int32_t loopingMode = mAnimationData.loopingMode;
      if( Scripting::GetEnumerationProperty( value, LOOPING_MODE_TABLE, LOOPING_MODE_TABLE_COUNT, loopingMode ) )
      {
        mAnimationData.loopingMode = DevelImageVisual::LoopingMode::Type( loopingMode );
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_LOOPING_MODE;
      }
      break;
    }
    case Toolkit::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN:
    {
      bool redraw;
      if( value.Get( redraw ) )
      {
        mRedrawInScalingDown = redraw;
      }
      break;
    }
  }
}

void AnimatedVectorImageVisual::DoSetOnScene( Actor& actor )
{
  Shader shader;

  if( mImpl->mCustomShader )
  {
    shader = Shader::New( mImpl->mCustomShader->mVertexShader.empty() ? mImageVisualShaderFactory.GetVertexShaderSource() : mImpl->mCustomShader->mVertexShader,
                          mImpl->mCustomShader->mFragmentShader.empty() ? mImageVisualShaderFactory.GetFragmentShaderSource() : mImpl->mCustomShader->mFragmentShader,
                          mImpl->mCustomShader->mHints );

    shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
  }
  else
  {
    shader = mImageVisualShaderFactory.GetShader( mFactoryCache, false, true, false );
  }

  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  mImpl->mRenderer = Renderer::New( geometry, shader );

  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer.SetTextures( textureSet );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

  // Defer the rasterisation task until we get given a size (by Size Negotiation algorithm)

  // Hold the weak handle of the placement actor and delay the adding of renderer until the rasterization is finished.
  mPlacementActor = actor;

  mVectorAnimationTask->SetRenderer( mImpl->mRenderer );

  // Add property notification for scaling & size
  mScaleNotification = actor.AddPropertyNotification( Actor::Property::WORLD_SCALE, StepCondition( 0.1f, 1.0f ) );
  mScaleNotification.NotifySignal().Connect( this, &AnimatedVectorImageVisual::OnScaleNotification );

  mSizeNotification = actor.AddPropertyNotification( Actor::Property::SIZE, StepCondition( 3.0f ) );
  mSizeNotification.NotifySignal().Connect( this, &AnimatedVectorImageVisual::OnSizeNotification );

  DevelActor::VisibilityChangedSignal( actor ).Connect( this, &AnimatedVectorImageVisual::OnControlVisibilityChanged );

  Window window = DevelWindow::Get( actor );
  if( window )
  {
    DevelWindow::VisibilityChangedSignal( window ).Connect( this, &AnimatedVectorImageVisual::OnWindowVisibilityChanged );
  }

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::DoSetOnScene [%p]\n", this );
}

void AnimatedVectorImageVisual::DoSetOffScene( Actor& actor )
{
  StopAnimation();
  SendAnimationData();

  if( mImpl->mRenderer )
  {
    actor.RemoveRenderer( mImpl->mRenderer );
    mImpl->mRenderer.Reset();

    mRendererAdded = false;
  }

  // Remove property notification
  actor.RemovePropertyNotification( mScaleNotification );
  actor.RemovePropertyNotification( mSizeNotification );

  DevelActor::VisibilityChangedSignal( actor ).Disconnect( this, &AnimatedVectorImageVisual::OnControlVisibilityChanged );

  Window window = DevelWindow::Get( actor );
  if( window )
  {
    DevelWindow::VisibilityChangedSignal( window ).Disconnect( this, &AnimatedVectorImageVisual::OnWindowVisibilityChanged );
  }

  mPlacementActor.Reset();

  // Reset the visual size to zero so that when adding the actor back to stage the rasterization is forced
  mVisualSize = Vector2::ZERO;
  mVisualScale = Vector2::ONE;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::DoSetOffScene [%p]\n", this );
}

void AnimatedVectorImageVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );

  if( IsOnScene() && visualSize != mVisualSize )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnSetTransform: width = %f, height = %f [%p]\n", visualSize.width, visualSize.height, this );

    mVisualSize = visualSize;

    SetVectorImageSize();

    if( mPlayState == DevelImageVisual::PlayState::PLAYING && mAnimationData.playState != DevelImageVisual::PlayState::PLAYING )
    {
      mAnimationData.playState = DevelImageVisual::PlayState::PLAYING;
      mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
    }

    SendAnimationData();
  }
}

void AnimatedVectorImageVisual::OnDoAction( const Property::Index actionId, const Property::Value& attributes )
{
  // Check if action is valid for this visual type and perform action if possible
  switch( actionId )
  {
    case DevelAnimatedVectorImageVisual::Action::PLAY:
    {
      if( IsOnScene() && mVisualSize != Vector2::ZERO )
      {
        if( mAnimationData.playState != DevelImageVisual::PlayState::PLAYING )
        {
          mAnimationData.playState = DevelImageVisual::PlayState::PLAYING;
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
        }
      }
      mPlayState = DevelImageVisual::PlayState::PLAYING;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::PAUSE:
    {
      if( mAnimationData.playState == DevelImageVisual::PlayState::PLAYING )
      {
        mAnimationData.playState = DevelImageVisual::PlayState::PAUSED;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      mPlayState = DevelImageVisual::PlayState::PAUSED;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::STOP:
    {
      if( mAnimationData.playState != DevelImageVisual::PlayState::STOPPED )
      {
        mAnimationData.playState = DevelImageVisual::PlayState::STOPPED;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      mPlayState = DevelImageVisual::PlayState::STOPPED;
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::JUMP_TO:
    {
      int32_t frameNumber;
      if( attributes.Get( frameNumber ) )
      {
        mAnimationData.currentFrame = frameNumber;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_CURRENT_FRAME;
      }
      break;
    }
    case DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY:
    {
      const Property::Map* map = attributes.GetMap();
      if( map )
      {
        DoSetProperties( *map );
      }
      break;
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::OnUploadCompleted()
{
  // If weak handle is holding a placement actor, it is the time to add the renderer to actor.
  Actor actor = mPlacementActor.GetHandle();
  if( actor && !mRendererAdded )
  {
    actor.AddRenderer( mImpl->mRenderer );
    mRendererAdded = true;

    ResourceReady( Toolkit::Visual::ResourceStatus::READY );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnUploadCompleted: Renderer is added [%p]\n", this );
  }
}

void AnimatedVectorImageVisual::OnAnimationFinished()
{
  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnAnimationFinished: action state = %d [%p]\n", mPlayState, this );

  if( mPlayState != DevelImageVisual::PlayState::STOPPED )
  {
    mPlayState = DevelImageVisual::PlayState::STOPPED;

    mAnimationData.playState = DevelImageVisual::PlayState::STOPPED;

    if( mImpl->mEventObserver )
    {
      mImpl->mEventObserver->NotifyVisualEvent( *this, DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED );
    }
  }

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED );
  }
}

void AnimatedVectorImageVisual::SendAnimationData()
{
  if( mAnimationData.resendFlag )
  {
    mVectorAnimationTask->SetAnimationData( mAnimationData );

    if( mImpl->mRenderer )
    {
      if( mAnimationData.playState == DevelImageVisual::PlayState::PLAYING )
      {
        mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY );
      }
      else
      {
        mImpl->mRenderer.SetProperty( DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED );
      }
    }

    mAnimationData.resendFlag = 0;
  }
}

void AnimatedVectorImageVisual::SetVectorImageSize()
{
  uint32_t width = static_cast< uint32_t >( mVisualSize.width * mVisualScale.width );
  uint32_t height = static_cast< uint32_t >( mVisualSize.height * mVisualScale.height );

  mAnimationData.width = width;
  mAnimationData.height = height;
  mAnimationData.resendFlag |= VectorAnimationTask::RESEND_SIZE;
}

void AnimatedVectorImageVisual::StopAnimation()
{
  if( mAnimationData.playState != DevelImageVisual::PlayState::STOPPED )
  {
    mAnimationData.playState = DevelImageVisual::PlayState::STOPPED;
    mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;

    mPlayState = DevelImageVisual::PlayState::STOPPED;
  }
}

void AnimatedVectorImageVisual::TriggerVectorRasterization()
{
  if( !mEventCallback && !mCoreShutdown )
  {
    mEventCallback = MakeCallback( this, &AnimatedVectorImageVisual::OnProcessEvents );
    auto& vectorAnimationManager = mFactoryCache.GetVectorAnimationManager();
    vectorAnimationManager.RegisterEventCallback( mEventCallback );
    Stage::GetCurrent().KeepRendering( 0.0f );  // Trigger event processing
  }
}

void AnimatedVectorImageVisual::OnScaleNotification( PropertyNotification& source )
{
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    Vector3 scale = actor.GetProperty< Vector3 >( Actor::Property::WORLD_SCALE );

    if(mRedrawInScalingDown || scale.width >= 1.0f || scale.height >= 1.0f)
    {
      mVisualScale.width = scale.width;
      mVisualScale.height = scale.height;

      DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnScaleNotification: scale = %f, %f [%p]\n", mVisualScale.width, mVisualScale.height, this );

      SetVectorImageSize();
      SendAnimationData();

      Stage::GetCurrent().KeepRendering( 0.0f );  // Trigger event processing
    }
  }
}

void AnimatedVectorImageVisual::OnSizeNotification( PropertyNotification& source )
{
  Actor actor = mPlacementActor.GetHandle();
  if( actor )
  {
    Vector3 size = actor.GetCurrentProperty< Vector3 >( Actor::Property::SIZE );
    mVisualSize.width = size.width;
    mVisualSize.height = size.height;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnSizeNotification: size = %f, %f [%p]\n", mVisualSize.width, mVisualSize.height, this );

    SetVectorImageSize();
    SendAnimationData();

    Stage::GetCurrent().KeepRendering( 0.0f );  // Trigger event processing
  }
}

void AnimatedVectorImageVisual::OnControlVisibilityChanged( Actor actor, bool visible, DevelActor::VisibilityChange::Type type )
{
  if( !visible )
  {
    StopAnimation();
    TriggerVectorRasterization();

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnControlVisibilityChanged: invisibile. Pause animation [%p]\n", this );
  }
}

void AnimatedVectorImageVisual::OnWindowVisibilityChanged( Window window, bool visible )
{
  if( !visible )
  {
    StopAnimation();
    TriggerVectorRasterization();

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::OnWindowVisibilityChanged: invisibile. Pause animation [%p]\n", this );
  }
}

void AnimatedVectorImageVisual::OnProcessEvents()
{
  SendAnimationData();

  mEventCallback = nullptr;  // The callback will be deleted in the VectorAnimationManager
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
