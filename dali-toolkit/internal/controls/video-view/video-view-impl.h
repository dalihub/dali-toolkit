#ifndef DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_H
#define DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/video-player-plugin.h>
#include <dali/devel-api/adaptor-framework/video-player.h>
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/integration-api/adaptor-framework/trigger-event-factory.h>
#include <dali/public-api/adaptor-framework/image-options.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/video-view/video-view-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <memory>

namespace Dali
{
namespace Toolkit
{
class VideoView;

namespace Internal
{

class VideoRenderingStrategy;
class WindowSurfaceStrategy;
class NativeImageStrategy;

class VideoView : public ControlImpl
{
protected:
  VideoView(Dali::VideoSyncMode syncMode);

  virtual ~VideoView();

private: // Internal accessors for Rendering Strategies
  friend class VideoRenderingStrategy;
  friend class WindowSurfaceStrategy;
  friend class NativeImageStrategy;

  /**
   * @brief Checks if the video is currently playing.
   *
   * @return True if video is playing, false otherwise.
   */
  bool IsPlay() const { return mIsPlay; }

  /**
   * @brief Checks if overlay texture rendering is enabled.
   *
   * @return True if overlay texture is being used, false otherwise.
   */
  bool IsUsingOverlayTexture() const { return mIsUsingOverlayTexture; }

  /**
   * @brief Gets the native texture used for video rendering.
   *
   * @return The native texture handle.
   */
  Texture GetNativeTexture() const { return mNativeTexture; }

  /**
   * @brief Sets the native texture used for video rendering.
   *
   * @param[in] texture The texture to set.
   */
  void SetNativeTexture(Texture texture) { mNativeTexture = texture; }

  /**
   * @brief Gets the effect property map.
   *
   * @return Const reference to the effect property map.
   */
  const Property::Map& GetEffectPropertyMap() const { return mEffectPropertyMap; }

  /**
   * @brief Gets the texture visual used for rendering.
   *
   * @return The texture visual base handle.
   */
  Dali::Toolkit::Visual::Base GetTextureVisual() const { return mTextureVisual; }

  /**
   * @brief Sets the texture visual used for rendering.
   *
   * @param[in] visual The visual to set.
   */
  void SetTextureVisual(Dali::Toolkit::Visual::Base visual) { mTextureVisual = visual; }

  /**
   * @brief Gets the overlay visual used for rendering.
   *
   * @return The overlay visual base handle.
   */
  Dali::Toolkit::Visual::Base GetOverlayVisual() const { return mOverlayVisual; }

  /**
   * @brief Sets the overlay visual used for rendering.
   *
   * @param[in] visual The visual to set.
   */
  void SetOverlayVisual(Dali::Toolkit::Visual::Base visual) { mOverlayVisual = visual; }

  /**
   * @brief Applies backup properties to the VideoView.
   *
   * This method restores previously backed up properties when switching rendering modes.
   */
  void ApplyBackupProperties();

  /**
   * @brief Called when the window is resized.
   *
   * This method handles window resize events and updates the video display rotation accordingly.
   *
   * @param[in] winHandle The window handle that was resized.
   * @param[in] size The new window size.
   */
  void OnWindowResized(Dali::Window winHandle, Dali::Window::WindowSize size);

public:
  /**
   * @copydoc Toolkit::DevelVideoView::New()
   */
  static Toolkit::VideoView New(VideoSyncMode syncMode);

  /**
   * @brief Creates a new VideoView wrapper using an externally created native player handle.
   *
   * @param[in] playerHandle The externally created player handle with type information.
   * @param[in] syncMode The synchronization mode between the UI (transparent hole) and VideoPlayer
   * @return A handle to a newly allocated Dali VideoView
   */
  static Toolkit::VideoView New(Dali::VideoPlayerPlugin::PlayerHandle playerHandle, VideoSyncMode syncMode = VideoSyncMode::DISABLED);

  /**
   * @brief Sets a video url to play.
   *
   * @SINCE_1_1.38
   * @param [in] url The url of the video resource to play
   */
  void SetUrl(const std::string& url);

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
  void Forward(int millisecond);

  /**
   * @copydoc Toolkit::VideoView::Backward()
   */
  void Backward(int millisecond);

  /**
   * @brief Sets the player mute status.
   * @SINCE_1_1.38
   * @param[i] mute The new mute status, true is mute.
   */
  void SetMute(bool mute);

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
  void SetVolume(float left, float right);

  /**
   * @brief Returns current volume factor.
   * @SINCE_1_1.38
   * @param[out] left The current left volume scalar
   * @param[out] right The current right volume scalar
   */
  void GetVolume(float& left, float& right);

  /**
   * @copydoc Dali::Toolkit::VideoView::FinishedSignal()
   */
  Dali::Toolkit::VideoView::VideoViewSignalType& FinishedSignal();

  /**
   * @brief Set property map
   * @SINCE_1_1.38
   * @param[in] map The Dali::Property::Map to use for to display.
   */
  void SetPropertyMap(Property::Map map);

  // Properties
  /**
   * @brief Called when a property of an object of this type is set.
   * @SINCE_1_1.38
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Called to retrieve a property of an object of this type.
   * @SINCE_1_1.38
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);

  /**
   * @brief Set the depth index of this image renderer
   *
   * Renderer with higher depth indices are rendered in front of other visuals with smaller values
   * @SINCE_1_1.38
   * @param[in] depthIndex The depth index of this renderer
   */
  void SetDepthIndex(int depthIndex);

  /**
   * @brief Performs actions as requested using the action name.
   * @SINCE_1_1.38
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return True if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes);

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the c
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor);

  /**
   * @brief Sets underlay flag and initializes new rendering target by flag.
   */
  void SetUnderlay(bool set);

  /**
   * @brief Checks underlay flag.
   */
  bool IsUnderlay();

  /**
   * @brief Sets sw codec type.
   */
  void SetSWCodec(bool on);

  /**
   * @brief Gets play position.
   */
  int GetPlayPosition();

  /**
   * @brief Sets play position.
   */
  void SetPlayPosition(int pos);

  /**
   * @brief Sets Display mode.
   */
  void SetDisplayMode(int mode);

  /**
   * @brief Gets Display mode.
   */
  int GetDisplayMode() const;

  /**
   * @brief Gets internal media player.
   */
  Any GetMediaPlayer();

  /**
   * @brief Play the resize or move animation with synchronization between UI(transparent hole) and video player
   *
   * The resize and move animation's play() function is called.
   * If the animation is played, UI and video player will work synchronization.
   *
   * @param[in] videoView The current VideoView
   * @param[in] animation The animation for video view's resize or move.
   */
  void PlayAnimation(Dali::Animation animation);

  /**
   * @brief Checks whether the actor is set as a video view or not.
   *
   * @param[in] actor The actor to be checked
   * @return True if actor is video view.
   */
  bool IsVideoView(Actor actor) const;

  /**
   * @brief Gets the Video Player.
   *
   * @return The return of video player.
   */
  VideoPlayer GetVideoPlayer();

  /**
   * @brief Sets auto rotation feature. If enabled, video will rotate automatically according to the video orientation.
   * @param[in] enable Whether to enable auto rotation feature. Default is false.
   */
  void SetAutoRotationEnabled(bool enable);

  /**
   * @brief Checks whether auto rotation feature is enabled.
   * @return True if auto rotation feature is enabled. Default is false.
   */
  bool IsAutoRotationEnabled() const;

  /**
   * @brief Sets letter box feature. If enabled, the video will play in the video player's aspect ratio.
   * @param[in] enable Whether to enable letter box feature. Default is false.
   */
  void SetLetterBoxEnabled(bool enable);

  /**
   * @brief Checks whether letter box feature is enabled.
   * @return True if letter box feature is enabled. Default is false.
   */
  bool IsLetterBoxEnabled() const;

  /**
   * @brief Sets the time interval for frame interpolation.
   *
   * The interpolation factor will progress from 0.0 to 1.0 over this duration.
   * This interval is applied after the next call to SetVideoFrameBuffer.
   *
   * @param[in] intervalSeconds The duration in seconds for interpolation.
   */
  void SetFrameInterpolationInterval(float intervalSeconds);

  /**
   * @brief Gets the time interval for frame interpolation.
   *
   * @return The duration in seconds for interpolation.
   */
  float GetFrameInterpolationInterval() const;

  /**
   * @brief Enables or disables offscreen frame rendering for video interpolation.
   *
   * When enabled, the video player will use offscreen rendering for frame interpolation,
   * which can improve visual quality for certain video content.
   *
   * @param[in] useOffScreenFrame true to enable offscreen frame rendering, false to disable
   */
  void EnableOffscreenFrameRendering(bool useOffScreenFrame);

  /**
   * @brief Sets the video frame buffer for rendering.
   *
   * This method sets the native image that will be used as the frame buffer
   * for video rendering. The frame buffer contains the surface data for video playback.
   *
   * @param[in] source The native image for video frame buffer
   */
  void SetVideoFrameBuffer(Dali::NativeImagePtr source);

private: // From Control
  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Toolkit::Control::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Toolkit::Control::OnSizeSet()
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

private:
  /**
   * @brief Construct a new VideoView.
   */
  VideoView(const VideoView& videoView);

  // Undefined assignment operator.
  VideoView& operator=(const VideoView& videoView);

  /*
   * @brief Internal version of SetProperty
   */
  void SetPropertyInternal(Property::Index index, const Property::Value& value);

  /*
   * @brief FrameRender's callback function
   *
   * This function means the resize/move animation is finished,
   * so Ui and video player's synchronization can be finished.
   *
   */
  void FrameRenderCallback(int frameID);

  /*
   * @brief Set frameRender Callback function
   *
   * This function is added for listenr the resize/move animation is finished,
   *
   */
  void SetFrameRenderCallback();

  /*
   * @brief resize/move animation finished callback function
   *
   * This function is called the resize/move animation is finished,
   *
   */
  void OnAnimationFinished(Dali::Animation& animation);

  /**
   * @brief Video player event callback function
   */
  void OnVideoPlayerEvent(Dali::VideoPlayerPlugin::PlayerEventType event);

  /**
   * @brief This signal is emitted when an actor's children change their sibling order
   *
   * @param[in] actor parent actor.
   */
  void OnChildOrderChanged(Actor actor);

private:
  Dali::VideoPlayer     mVideoPlayer;
  Dali::ImageDimensions mVideoSize;
  Dali::Property::Map   mPropertyMap;
  Dali::Property::Map   mEffectPropertyMap;
  Dali::Texture         mNativeTexture;

  Dali::Toolkit::VideoView::VideoViewSignalType mFinishedSignal;

  std::string       mUrl;

  bool                        mIsUsingOverlayTexture{false};
  Property::Index             mOverlayTextureVisualIndex{Property::INVALID_INDEX};
  Dali::Toolkit::Visual::Base mOverlayVisual;
  Dali::Toolkit::Visual::Base mOverlayTextureVisual;
  Dali::Toolkit::Visual::Base mTextureVisual;

  std::unique_ptr<VideoRenderingStrategy> mRenderingStrategy;

  Dali::Property::Map mPropertyBackup;

  int mCurrentVideoPlayPosition;
  int mFrameID;

  bool mIsPlay;
  bool mIsUnderlay;
  bool mSettingUnderlay;

  Dali::VideoSyncMode mSyncMode;
  int                 mSiblingOrder;

  float mInterpolationInterval{0.0f}; ///< Target duration for interpolation in seconds
};

} // namespace Internal

inline Toolkit::Internal::VideoView& GetImpl(Toolkit::VideoView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<Toolkit::Internal::VideoView&>(impl);
}

inline const Toolkit::Internal::VideoView& GetImpl(const Toolkit::VideoView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::RefObject& impl = handle.GetImplementation();
  return static_cast<const Toolkit::Internal::VideoView&>(impl);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VIDEO_VIEW_H
