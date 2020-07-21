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
#include "video-view-impl.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

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
DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "playPosition", INTEGER, PLAY_POSITION )
DALI_PROPERTY_REGISTRATION( Toolkit, VideoView, "displayMode", INTEGER, DISPLAY_MODE )

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

const char* const CUSTOM_SHADER( "shader" );
const char* const CUSTOM_VERTEX_SHADER( "vertexShader" );
const char* const CUSTOM_FRAGMENT_SHADER( "fragmentShader" );
const char* const DEFAULT_SAMPLER_TYPE_NAME( "sampler2D" );
const char* const CUSTOM_SAMPLER_TYPE_NAME( "samplerExternalOES" );

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform highp   mat4 uMvpMatrix;\n
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
  \n
  void main()\n
  {\n
    gl_FragColor = vec4(0.0);\n
  }\n
);

const char* VERTEX_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform highp   mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  varying mediump vec2 sTexCoordRect;\n
  void main()\n
  {\n
    gl_Position = uMvpMatrix * vec4(aPosition * uSize.xy, 0.0, 1.0);\n
    vTexCoord = aPosition + vec2(0.5);\n
  }\n
);

const char* FRAGMENT_SHADER_TEXTURE = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  varying mediump vec2 vTexCoord;\n
  uniform samplerExternalOES sTexture;\n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
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
}

void VideoView::SetUrl( const std::string& url )
{
    mUrl = url;
    mPropertyMap.Clear();

  mVideoPlayer.SetUrl( mUrl );
}

void VideoView::SetPropertyMap( Property::Map map )
{
  mPropertyMap = map;

  Property::Value* target = map.Find( RENDERING_TARGET );
  std::string targetType;

  if( target && target->Get( targetType ) && targetType == WINDOW_SURFACE_TARGET )
  {
    mIsUnderlay = true;
    SetWindowSurfaceTarget();
  }
  else if( target && target->Get( targetType ) && targetType == NATIVE_IMAGE_TARGET )
  {
    mIsUnderlay = false;
    SetNativeImageTarget();
  }

  // Custom shader
  Property::Value* shaderValue;
  if( !map.Empty() )
  {
    shaderValue = map.Find( CUSTOM_SHADER );

    if( shaderValue )
    {
      Property::Map* shaderMap = shaderValue->GetMap();
      if( shaderMap )
      {
        mEffectPropertyMap = *shaderMap;
      }
    }
  }

  if( mTextureRenderer && !mEffectPropertyMap.Empty() )
  {
    Dali::Shader shader = CreateShader();
    mTextureRenderer.SetShader( shader );
  }

  RelayoutRequest();
}

std::string VideoView::GetUrl()
{
  return mUrl;
}

void VideoView::SetLooping( bool looping )
{
  mVideoPlayer.SetLooping( looping );
}

bool VideoView::IsLooping()
{
  return mVideoPlayer.IsLooping();
}

void VideoView::Play()
{
  if( mOverlayRenderer )
  {
    Self().AddRenderer( mOverlayRenderer );
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
  if( mOverlayRenderer )
  {
    Self().RemoveRenderer( mOverlayRenderer );
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

void VideoView::SetPropertyInternal( Property::Index index, const Property::Value& value )
{
  switch( index )
  {
    case Toolkit::VideoView::Property::VIDEO:
    {
      std::string videoUrl;
      Property::Map map;

      if( value.Get( videoUrl ) )
      {
        SetUrl( videoUrl );
      }
      else if( value.Get( map ) )
      {
        SetPropertyMap( map );
      }
      break;
    }
    case Toolkit::VideoView::Property::LOOPING:
    {
      bool looping;
      if( value.Get( looping ) )
      {
        SetLooping( looping );
      }
      break;
    }
    case Toolkit::VideoView::Property::MUTED:
    {
      bool mute;
      if( value.Get( mute ) )
      {
        SetMute( mute );
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
          SetVolume( left, right );
        }
      }
      break;
    }
    case Toolkit::VideoView::Property::UNDERLAY:
    {
      bool underlay;
      if( value.Get( underlay ) )
      {
        SetUnderlay( underlay );
      }
      break;
    }
    case Toolkit::VideoView::Property::PLAY_POSITION:
    {
      int pos;
      if( value.Get( pos ) )
      {
        SetPlayPosition( pos );
      }
      break;
    }
    case Toolkit::VideoView::Property::DISPLAY_MODE:
    {
      int mode;
      if( value.Get( mode ) )
      {
        SetDisplayMode( mode );
      }
      break;
    }
  }
}

void VideoView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::VideoView videoView = Toolkit::VideoView::DownCast( Dali::BaseHandle( object ) );

  if( videoView )
  {
    VideoView& impl = GetImpl( videoView );

    impl.SetPropertyInternal( index, value );

    if( index != Toolkit::VideoView::Property::UNDERLAY )
    {
      // Backup values.
      // These values will be used when underlay mode is changed.
      impl.mPropertyBackup[index] = value;
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
      case Toolkit::VideoView::Property::PLAY_POSITION:
      {
        value = impl.GetPlayPosition();
        break;
      }
      case Toolkit::VideoView::Property::DISPLAY_MODE:
      {
        value = impl.GetDisplayMode();
        break;
      }
    }
  }

  return value;
}

void VideoView::SetDepthIndex( int depthIndex )
{
  if( mTextureRenderer )
  {
    mTextureRenderer.SetProperty( Renderer::Property::DEPTH_INDEX, depthIndex );
  }
}

void VideoView::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );

  if( mIsUnderlay )
  {
    SetWindowSurfaceTarget();
  }
}

void VideoView::OnStageDisconnection()
{
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

  if( !self.GetProperty< bool >( Actor::Property::CONNECTED_TO_SCENE ) )
  {
    // When the control is off the stage, it does not have Window.
    return;
  }

  int curPos = mVideoPlayer.GetPlayPosition();

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

  if( mTextureRenderer )
  {
    self.RemoveRenderer( mTextureRenderer );
  }

  // Note VideoPlayer::SetRenderingTarget resets all the options. (e.g. url, mute, looping)
  mVideoPlayer.SetRenderingTarget( Dali::Adaptor::Get().GetNativeWindowHandle( self ) );

  ApplyBackupProperties();

  if( !mOverlayRenderer )
  {
    // For underlay rendering mode, video display area have to be transparent.
    Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
    Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
    mOverlayRenderer = Renderer::New( geometry, shader );
    mOverlayRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::OFF );
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

  if( mOverlayRenderer )
  {
    self.RemoveRenderer( mOverlayRenderer );

    mOverlayRenderer.Reset();
  }

  self.RemovePropertyNotification( mPositionUpdateNotification );
  self.RemovePropertyNotification( mSizeUpdateNotification );
  self.RemovePropertyNotification( mScaleUpdateNotification );

  int curPos = mVideoPlayer.GetPlayPosition();

  Any source;
  Dali::NativeImageSourcePtr nativeImageSourcePtr = Dali::NativeImageSource::New( source );
  mNativeTexture = Dali::Texture::New( *nativeImageSourcePtr );

  if( !mTextureRenderer )
  {
    Dali::Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
    Dali::Shader shader = CreateShader();
    Dali::TextureSet textureSet = Dali::TextureSet::New();
    textureSet.SetTexture( 0u, mNativeTexture );

    mTextureRenderer = Renderer::New( geometry, shader );
    mTextureRenderer.SetTextures( textureSet );
  }
  else
  {
    Dali::TextureSet textureSet = mTextureRenderer.GetTextures();
    textureSet.SetTexture( 0u, mNativeTexture );
  }
  Self().AddRenderer( mTextureRenderer );

  // Note VideoPlayer::SetRenderingTarget resets all the options. (e.g. url, mute, looping)
  mVideoPlayer.SetRenderingTarget( nativeImageSourcePtr );

  ApplyBackupProperties();

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

  bool positionUsesAnchorPoint = self.GetProperty( Actor::Property::POSITION_USES_ANCHOR_POINT ).Get< bool >();
  Vector3 actorSize = self.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ) * self.GetCurrentProperty< Vector3 >( Actor::Property::SCALE );
  Vector3 anchorPointOffSet = actorSize * ( positionUsesAnchorPoint ? self.GetCurrentProperty< Vector3 >( Actor::Property::ANCHOR_POINT ) : AnchorPoint::TOP_LEFT );

  Vector2 screenPosition = self.GetProperty( Actor::Property::SCREEN_POSITION ).Get< Vector2 >();

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

void VideoView::SetSWCodec( bool on )
{
  // If setting SW or HW type is failed , video-view shows video by default codec type.
  // The default codec type is selected by platform.
  if( on )
  {
    mVideoPlayer.SetCodecType( Dali::VideoPlayerPlugin::CodecType::SW );
  }
  else
  {
    mVideoPlayer.SetCodecType( Dali::VideoPlayerPlugin::CodecType::HW );
  }
}

int VideoView::GetPlayPosition()
{
  return mVideoPlayer.GetPlayPosition();
}

void VideoView::SetPlayPosition( int pos )
{
  mVideoPlayer.SetPlayPosition( pos );
}

void VideoView::SetDisplayMode( int mode )
{
  mVideoPlayer.SetDisplayMode( static_cast< Dali::VideoPlayerPlugin::DisplayMode::Type >( mode ) );
}

int VideoView::GetDisplayMode() const
{
  return static_cast< int >( mVideoPlayer.GetDisplayMode() );
}

Any VideoView::GetMediaPlayer()
{
  return mVideoPlayer.GetMediaPlayer();
}

Dali::Shader VideoView::CreateShader()
{
  std::string fragmentShader = "#extension GL_OES_EGL_image_external:require\n";
  std::string vertexShader;
  std::string customFragmentShader;
  bool checkShader = false;

  if( !mEffectPropertyMap.Empty() )
  {
    Property::Value* vertexShaderValue = mEffectPropertyMap.Find( CUSTOM_VERTEX_SHADER );
    if( vertexShaderValue )
    {
      checkShader = GetStringFromProperty( *vertexShaderValue, vertexShader );
    }

    if( !vertexShaderValue || !checkShader )
    {
      vertexShader = VERTEX_SHADER_TEXTURE;
    }

    Property::Value* fragmentShaderValue = mEffectPropertyMap.Find( CUSTOM_FRAGMENT_SHADER );
    if( fragmentShaderValue )
    {
      checkShader = GetStringFromProperty( *fragmentShaderValue, customFragmentShader );

      if( checkShader )
      {
        fragmentShader = customFragmentShader;
      }
    }

    if( !fragmentShaderValue || !checkShader )
    {
      fragmentShader += FRAGMENT_SHADER_TEXTURE;
    }
  }
  else
  {
    vertexShader = VERTEX_SHADER_TEXTURE;
    fragmentShader += FRAGMENT_SHADER_TEXTURE;
  }

  return Dali::Shader::New( vertexShader, fragmentShader );
}

bool VideoView::GetStringFromProperty( const Dali::Property::Value& value, std::string& output )
{
  bool extracted = false;
  if( value.Get( output ) )
  {
    extracted = true;
  }

  return extracted;
}

void VideoView::ApplyBackupProperties()
{
  Property::Map::SizeType pos = 0;
  Property::Map::SizeType count = mPropertyBackup.Count();

  for( ; pos < count; pos++ )
  {
    KeyValuePair property = mPropertyBackup.GetKeyValue( pos );

    SetPropertyInternal( property.first.indexKey, property.second );
  }
}

} // namespace Internal

} // namespace toolkit

} // namespace Dali
