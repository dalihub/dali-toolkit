/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include "video-view-impl.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/integration-api/adaptors/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::VideoView::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::VideoView, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "video", MAP, VIDEO )
DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "looping", BOOLEAN, LOOPING )
DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "muted", BOOLEAN, MUTED )
DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "volume", MAP, VOLUME )
DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "underlay", BOOLEAN, UNDERLAY )

DALI_SIGNAL_REGISTRATION( Toolkit, VideoView, "finished", FINISHED_SIGNAL )

DALI_ACTION_REGISTRATION( Toolkit, VideoView, "play", ACTION_VIDEOVIEW_PLAY )
DALI_ACTION_REGISTRATION( Toolkit, VideoView, "pause", ACTION_VIDEOVIEW_PAUSE )
DALI_ACTION_REGISTRATION( Toolkit, VideoView, "stop", ACTION_VIDEOVIEW_STOP )
DALI_ACTION_REGISTRATION( Toolkit, VideoView, "forward", ACTION_VIDEOVIEW_FORWARD )
DALI_ACTION_REGISTRATION( Toolkit, VideoView, "backward", ACTION_VIDEOVIEW_BACKWARD )

DALI_TYPE_REGISTRATION_END()

const char* const VOLUME_LEFT( "volumeLeft" );
const char* const VOLUME_RIGHT( "volumeRight" );

// 3.0 TC uses RENDERING_TARGET. It should be removed in next release
const char* const RENDERING_TARGET( "renderingTarget" );
const char* const WINDOW_SURFACE_TARGET( "windowSurfaceTarget" );
const char* const NATIVE_IMAGE_TARGET( "nativeImageTarget" );

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    gl_Position = uMvpMatrix * vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float opacity;\n
  \n
  void main()\n
  {\n
    gl_FragColor = vec4(mixColor, opacity)*uColor;\n
  }\n
);

} // anonymous namepsace

VideoView::VideoView()
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mCurrentVideoPlayPosition( 0 ),
  mIsPlay( false ),
  mIsUnderlay( true )
{
  mVideoPlayer = Dali::VideoPlayer::New();
}

VideoView::~VideoView()
{
}

Toolkit::VideoView VideoView::New()
{
  VideoView* impl = new VideoView();
  Toolkit::VideoView handle = Toolkit::VideoView( *impl );

  impl->Initialize();

  return handle;
}

void VideoView::OnInitialize()
{
  mVideoPlayer.FinishedSignal().Connect( this, &VideoView::EmitSignalFinish );
  SetWindowSurfaceTarget();
}

void VideoView::SetUrl( const std::string& url )
{
  if( mUrl != url || !mPropertyMap.Empty() )
  {
    mUrl = url;
    mPropertyMap.Clear();
  }

  if( !mIsUnderlay )
  {
    Actor self( Self() );
    Internal::InitializeVisual( self, mVisual, mNativeImage );

    if( self.OnStage() )
    {
      Toolkit::GetImplementation( mVisual ).SetOnStage( self );
    }
  }

  mVideoPlayer.SetUrl( mUrl );
}

void VideoView::SetPropertyMap( Property::Map map )
{
  mPropertyMap = map;

  Actor self( Self() );
  Internal::InitializeVisual( self, mVisual, mPropertyMap );

  Property::Value* widthValue = mPropertyMap.Find( "width" );
  if( widthValue )
  {
    int width;
    if( widthValue->Get( width ) )
    {
      mVideoSize = ImageDimensions( width, mVideoSize.GetHeight() );
    }
  }

  Property::Value* heightValue = mPropertyMap.Find( "height" );
  if( heightValue )
  {
    int height;
    if( heightValue->Get( height ) )
    {
      mVideoSize = ImageDimensions( mVideoSize.GetWidth(), height );
    }
  }

  Property::Value* target = map.Find( RENDERING_TARGET );
  std::string targetType;

  if( target && target->Get( targetType ) && targetType == WINDOW_SURFACE_TARGET )
  {
    mIsUnderlay = true;
    this->SetWindowSurfaceTarget();
  }
  else if( target && target->Get( targetType ) && targetType == NATIVE_IMAGE_TARGET )
  {
    mIsUnderlay = false;
    this->SetNativeImageTarget();
  }

  RelayoutRequest();
}

std::string VideoView::GetUrl()
{
  return mUrl;
}

void VideoView::SetLooping(bool looping)
{
  mVideoPlayer.SetLooping( looping );
}

bool VideoView::IsLooping()
{
  return mVideoPlayer.IsLooping();
}

void VideoView::Play()
{
  if( mIsUnderlay )
  {
    Self().AddRenderer( mRenderer );
  }

  mVideoPlayer.Play();
  mIsPlay = true;
}

void VideoView::Pause()
{
  mVideoPlayer.Pause();
  mIsPlay = false;
}

void VideoView::Stop()
{
  mVideoPlayer.Stop();
  mIsPlay = false;
}

void VideoView::Forward( int millisecond )
{
  int curPos = mVideoPlayer.GetPlayPosition();

  int nextPos = curPos + millisecond;

  mVideoPlayer.SetPlayPosition( nextPos );
}

void VideoView::Backward( int millisecond )
{
  int curPos = mVideoPlayer.GetPlayPosition();

  int nextPos = curPos - millisecond;
  nextPos = ( nextPos < 0 )? 0: nextPos;

  mVideoPlayer.SetPlayPosition( nextPos );
}

void VideoView::SetMute( bool mute )
{
  mVideoPlayer.SetMute( mute );
}

bool VideoView::IsMuted()
{
  return mVideoPlayer.IsMuted();
}

void VideoView::SetVolume( float left, float right )
{
  mVideoPlayer.SetVolume( left, right );
}

void VideoView::GetVolume( float& left, float& right )
{
  mVideoPlayer.GetVolume( left, right );
}

Dali::Toolkit::VideoView::VideoViewSignalType& VideoView::FinishedSignal()
{
  return mFinishedSignal;
}

void VideoView::EmitSignalFinish()
{
  if( mIsUnderlay )
  {
    Self().RemoveRenderer( mRenderer );
  }

  if ( !mFinishedSignal.Empty() )
  {
    Dali::Toolkit::VideoView handle( GetOwner() );
    mFinishedSignal.Emit( handle );
  }
}

bool VideoView::DoAction( BaseObject* object, const std::string& actionName, const Property::Map& attributes )
{
  bool ret = false;

  Dali::BaseHandle handle( object );
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast( handle );

  if( !videoView )
  {
    return ret;
  }

  VideoView& impl = GetImpl( videoView );

  if( strcmp( actionName.c_str(), ACTION_VIDEOVIEW_PLAY ) == 0 )
  {
    impl.Play();
    ret = true;
  }
  else if( strcmp( actionName.c_str(), ACTION_VIDEOVIEW_PAUSE ) == 0 )
  {
    impl.Pause();
    ret = true;
  }
  else if( strcmp( actionName.c_str(), ACTION_VIDEOVIEW_STOP ) == 0 )
  {
    impl.Stop();
    ret = true;
  }
  else if( strcmp( actionName.c_str(), ACTION_VIDEOVIEW_FORWARD ) == 0 )
  {
    int millisecond = 0;
    if( attributes["videoForward"].Get( millisecond ) )
    {
      impl.Forward( millisecond );
      ret = true;
    }
  }
  else if( strcmp( actionName.c_str(), ACTION_VIDEOVIEW_BACKWARD ) == 0 )
  {
    int millisecond = 0;
    if( attributes["videoBackward"].Get( millisecond ) )
    {
      impl.Backward( millisecond );
      ret = true;
    }
  }

  return ret;
}

bool VideoView::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), FINISHED_SIGNAL ) )
  {
    videoView.FinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void VideoView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast( Dali::BaseHandle( object ) );

  if( videoView )
  {
    VideoView& impl = GetImpl( videoView );

    switch( index )
    {
      case Toolkit::VideoView::Property::VIDEO:
      {
        std::string videoUrl;
        Property::Map map;

        if( value.Get( videoUrl ) )
        {
          impl.SetUrl( videoUrl );
        }
        else if( value.Get( map ) )
        {
          Property::Value* shaderValue = map.Find( Toolkit::Visual::Property::SHADER, CUSTOM_SHADER );

          if( map.Count() > 1u || !shaderValue )
          {
            impl.SetPropertyMap( map );
          }
          else if( impl.mVisual && map.Count() == 1u && shaderValue )
          {
            Property::Map shaderMap;
            if( shaderValue->Get( shaderMap ) )
            {
              Internal::Visual::Base& visual = Toolkit::GetImplementation( impl.mVisual );
              visual.SetCustomShader( shaderMap );
              if( videoView.OnStage() )
              {
                visual.SetOffStage( videoView );
                visual.SetOnStage( videoView );
              }
            }
          }
        }
        break;
      }
      case Toolkit::VideoView::Property::LOOPING:
      {
        bool looping;
        if( value.Get( looping ) )
        {
          impl.SetLooping( looping );
        }
        break;
      }
      case Toolkit::VideoView::Property::MUTED:
      {
        bool mute;
        if( value.Get( mute ) )
        {
          impl.SetMute( mute );
        }
        break;
      }
      case Toolkit::VideoView::Property::VOLUME:
      {
        Property::Map map;
        float left, right;
        if( value.Get( map ) )
        {
          Property::Value* volumeLeft = map.Find( VOLUME_LEFT );
          Property::Value* volumeRight = map.Find( VOLUME_RIGHT );
          if( volumeLeft && volumeLeft->Get( left ) && volumeRight && volumeRight->Get( right ) )
          {
            impl.SetVolume( left, right );
          }
        }
        break;
      }
      case Toolkit::VideoView::Property::UNDERLAY:
      {
        bool underlay;
        if( value.Get( underlay ) )
        {
          impl.SetUnderlay( underlay );
        }
        break;
      }
    }
  }
}

Property::Value VideoView::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast( Dali::BaseHandle( object ) );

  if( videoView )
  {
    VideoView& impl = GetImpl( videoView );

    switch( propertyIndex )
    {
      case Toolkit::VideoView::Property::VIDEO:
      {
        if( !impl.mUrl.empty() )
        {
          value = impl.mUrl;
        }
        else if( !impl.mPropertyMap.Empty() )
        {
          value = impl.mPropertyMap;
        }
        break;
      }
      case Toolkit::VideoView::Property::LOOPING:
      {
        value = impl.IsLooping();
        break;
      }
      case Toolkit::VideoView::Property::MUTED:
      {
        value = impl.IsMuted();
        break;
      }
      case Toolkit::VideoView::Property::VOLUME:
      {
        Property::Map map;
        float left, right;

        impl.GetVolume( left, right );
        map.Insert( VOLUME_LEFT, left );
        map.Insert( VOLUME_RIGHT, right );
        value = map;
        break;
      }
      case Toolkit::VideoView::Property::UNDERLAY:
      {
        value = impl.IsUnderlay();
        break;
      }
    }
  }

  return value;
}

void VideoView::SetDepthIndex( int depthIndex )
{
  if( mVisual )
  {
    mVisual.SetDepthIndex( depthIndex );
  }
}

void VideoView::OnStageConnection( int depth )
{
  if( mVisual )
  {
    CustomActor self = Self();
    Toolkit::GetImplementation(mVisual).SetOnStage( self );
  }

  Control::OnStageConnection( depth );
}

void VideoView::OnStageDisconnection()
{
  if( mVisual )
  {
    CustomActor self = Self();
    Toolkit::GetImplementation(mVisual).SetOffStage( self );
  }

  Control::OnStageDisconnection();
}

Vector3 VideoView::GetNaturalSize()
{
  Vector3 size;
  size.x = mVideoSize.GetWidth();
  size.y = mVideoSize.GetHeight();

  if( size.x > 0 && size.y > 0 )
  {
    size.z = std::min( size.x, size.y );
    return size;
  }
  else
  {
    return Control::GetNaturalSize();
  }
}

float VideoView::GetHeightForWidth( float width )
{
  if( mVideoSize.GetWidth() > 0 && mVideoSize.GetHeight() > 0 )
  {
    return GetHeightForWidthBase( width );
  }
  else
  {
    return Control::GetHeightForWidthBase( width );
  }
}

float VideoView::GetWidthForHeight( float height )
{
  if( mVideoSize.GetWidth() > 0 && mVideoSize.GetHeight() > 0 )
  {
    return GetWidthForHeightBase( height );
  }
  else
  {
    return Control::GetWidthForHeightBase( height );
  }
}

void VideoView::SetWindowSurfaceTarget()
{
  Actor self = Self();
  int curPos = mVideoPlayer.GetPlayPosition();

  if( mVisual )
  {
    Toolkit::GetImplementation(mVisual).SetOffStage(self);
    mVisual.Reset();
  }

  if( mIsPlay )
  {
    mVideoPlayer.Pause();
  }

  mPositionUpdateNotification = self.AddPropertyNotification( Actor::Property::WORLD_POSITION, StepCondition( 1.0f, 1.0f ) );
  mSizeUpdateNotification = self.AddPropertyNotification( Actor::Property::SIZE, StepCondition( 1.0f, 1.0f ) );
  mScaleUpdateNotification = self.AddPropertyNotification( Actor::Property::WORLD_SCALE, StepCondition( 0.1f, 1.0f ) );
  mPositionUpdateNotification.NotifySignal().Connect( this, &VideoView::UpdateDisplayArea );
  mSizeUpdateNotification.NotifySignal().Connect( this, &VideoView::UpdateDisplayArea );
  mScaleUpdateNotification.NotifySignal().Connect( this, &VideoView::UpdateDisplayArea );

  mVideoPlayer.SetRenderingTarget( Dali::Adaptor::Get().GetNativeWindowHandle() );
  mVideoPlayer.SetUrl( mUrl );

  if( !mRenderer )
  {
    // For underlay rendering mode, video display area have to be transparent.
    Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
    Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mRenderer = Renderer::New( geometry, shader );

    mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
    mRenderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::ONE );
    mRenderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ZERO );
    mRenderer.SetProperty( Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE );
    mRenderer.SetProperty( Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ZERO );
  }

  if( mIsPlay )
  {
    Play();
  }

  if( curPos > 0 )
  {
    mVideoPlayer.SetPlayPosition( curPos );
  }
}

void VideoView::SetNativeImageTarget()
{
  if( mVideoPlayer.IsVideoTextureSupported() == false )
  {
    DALI_LOG_ERROR( "Platform doesn't support decoded video frame images\n" );
    mIsUnderlay = true;
    return;
  }

  if( mIsPlay )
  {
    mVideoPlayer.Pause();
  }

  Actor self( Self() );
  self.RemoveRenderer( mRenderer );
  Dali::Stage::GetCurrent().KeepRendering( 0.0f );

  self.RemovePropertyNotification( mPositionUpdateNotification );
  self.RemovePropertyNotification( mSizeUpdateNotification );
  self.RemovePropertyNotification( mScaleUpdateNotification );

  int curPos = mVideoPlayer.GetPlayPosition();

  Any source;
  Dali::NativeImageSourcePtr nativeImageSourcePtr = Dali::NativeImageSource::New( source );
  mNativeImage = Dali::NativeImage::New( *nativeImageSourcePtr );

  mVideoPlayer.SetRenderingTarget( nativeImageSourcePtr );
  mVideoPlayer.SetUrl( mUrl );

  Internal::InitializeVisual( self, mVisual, mNativeImage );
  Self().RemoveRenderer( mRenderer );

  if( mIsPlay )
  {
    Play();
  }

  if( curPos > 0 )
  {
    mVideoPlayer.SetPlayPosition( curPos );
  }
}

void VideoView::UpdateDisplayArea( Dali::PropertyNotification& source )
{
  if( !mIsUnderlay )
  {
    return;
  }

  Actor self( Self() );

  bool positionUsesAnchorPoint = self.GetProperty( DevelActor::Property::POSITION_USES_ANCHOR_POINT ).Get< bool >();
  Vector3 actorSize = self.GetCurrentSize() * self.GetCurrentScale();
  Vector3 anchorPointOffSet = actorSize * ( positionUsesAnchorPoint ? self.GetCurrentAnchorPoint() : AnchorPoint::TOP_LEFT );

  Vector2 screenPosition = self.GetProperty( DevelActor::Property::SCREEN_POSITION ).Get< Vector2 >();

  mDisplayArea.x = screenPosition.x - anchorPointOffSet.x;
  mDisplayArea.y = screenPosition.y - anchorPointOffSet.y;
  mDisplayArea.width = actorSize.x;
  mDisplayArea.height = actorSize.y;

  mVideoPlayer.SetDisplayArea( mDisplayArea );
}

void VideoView::SetUnderlay( bool set )
{
  if( set != mIsUnderlay )
  {
    mIsUnderlay = set;

    if( mIsUnderlay )
    {
      SetWindowSurfaceTarget();
    }
    else
    {
      SetNativeImageTarget();
    }

    RelayoutRequest();
  }
}

bool VideoView::IsUnderlay()
{
  return mIsUnderlay;
}

} // namespace Internal

} // namespace toolkit

} // namespace Dali
