#ifndef DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_H
#define DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/images/native-image.h>
#include <dali/devel-api/adaptor-framework/video-player.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>

namespace Dali
{

namespace Toolkit
{

class VideoView;

namespace Internal
{

class VideoView: public Control
{
protected:

  VideoView();

  virtual ~VideoView();

public:

  /**
   * @copydoc Toolkit::VideoView::New()
   */
  static Toolkit::VideoView New();

  /**
   * @brief Sets a video url to play.
   *
   * @SINCE_1_1.38
   * @param [in] url The url of the video resource to play
   */
  void SetUrl( const std::string& url );

  /**
   * @brief Returns a video url.
   * @SINCE_1_1.38
   * @return Url of string type
   */
  std::string GetUrl();

  /**
   * @brief Sets the player looping status.
   *
   * @SINCE_1_1.38
   * @param [in] looping The new looping status: true or false
   */
  void SetLooping(bool looping);

  /**
   * @brief Returns the player looping status.
   *
   * @SINCE_1_1.38
   * @return True if player is looping, false otherwise.
   */
  bool IsLooping();

  /**
   * @copydoc Toolkit::VideoView::Play()
   */
  void Play();

  /**
   * @copydoc Toolkit::VideoView::Pause()
   */
  void Pause();

  /**
   * @copydoc Toolkit::VideoView::Stop()
   */
  void Stop();

  /**
   * @copydoc Toolkit::VideoView::Forward()
   */
  void Forward( int millisecond );

  /**
   * @copydoc Toolkit::VideoView::Backward()
   */
  void Backward( int millisecond );

  /**
   * @brief Sets the player mute status.
   * @SINCE_1_1.38
   * @param[i] mute The new mute status, true is mute.
   */
  void SetMute( bool mute );

  /**
   * @brief Returns the player mute status.
   * @SINCE_1_1.38
   * @return True if player is mute.
   */
  bool IsMuted();

  /**
   * @brief Sets the player volume.
   * @SINCE_1_1.38
   * @param[in] left The left volume scalar
   * @param[in] right The right volume scalar
   */
  void SetVolume( float left, float right );

  /**
   * @brief Returns current volume factor.
   * @SINCE_1_1.38
   * @param[out] left The current left volume scalar
   * @param[out] right The current right volume scalar
   */
  void GetVolume( float& left, float& right );

 /**
   * @copydoc Dali::Toolkit::VideoView::FinishedSignal()
   */
  Dali::Toolkit::VideoView::VideoViewSignalType& FinishedSignal();

  /**
   * @brief Emit the finished signal
   */
  void EmitSignalFinish();

  /**
   * @brief Set property map
   * @SINCE_1_1.38
   * @param[in] map The Dali::Property::Map to use for to display.
   */
  void SetPropertyMap( Property::Map map );

  // Properties
  /**
   * @brief Called when a property of an object of this type is set.
   * @SINCE_1_1.38
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   * @SINCE_1_1.38
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

  /**
   * @brief Set the depth index of this image renderer
   *
   * Renderer with higher depth indices are rendered in front of other visuals with smaller values
   * @SINCE_1_1.38
   * @param[in] depthIndex The depth index of this renderer
   */
  void SetDepthIndex( int depthIndex );


  /**
   * @brief Performs actions as requested using the action name.
   * @SINCE_1_1.38
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return True if action has been accepted by this control
   */
  static bool DoAction( BaseObject* object, const std::string& actionName, const Property::Map& attributes );

  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnStageConnect()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Toolkit::Control::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Toolkit::Control::GetWidthForHeight()
   */
  virtual float GetWidthForHeight( float height );

private:

  // Undefined
  VideoView( const VideoView& videoView );

  VideoView& operator=( const VideoView& videoView );

  void SetWindowSurfaceTarget();

  void SetNativeImageTarget();

private:

  Dali::VideoPlayer mVideoPlayer;
  Dali::ImageDimensions mVideoSize;
  Toolkit::Visual::Base mVisual;
  Dali::Property::Map mPropertyMap;
  Dali::NativeImage mNativeImage; ///< Native image handle for video rendering by texture streaming
  Dali::Toolkit::VideoView::VideoViewSignalType mFinishedSignal;
  std::string mUrl;
  int mCurrentVideoPlayPosition;
  bool mSetRenderingTarget;
  bool mIsPlay;
  bool mIsPause;
};

} // namespace Internal

inline Toolkit::Internal::VideoView& GetImpl( Toolkit::VideoView& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< Toolkit::Internal::VideoView& >( impl );
}

inline const Toolkit::Internal::VideoView& GetImpl( const Toolkit::VideoView& handle )
{
  DALI_ASSERT_ALWAYS( handle );
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast< const Toolkit::Internal::VideoView& >( impl );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VIDEO_VIEW_H
