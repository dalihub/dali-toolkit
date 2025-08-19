#ifndef DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_H
#define DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/video-player.h>
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/integration-api/adaptor-framework/trigger-event-factory.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>

namespace Dali
{
namespace Toolkit
{
class VideoView;

namespace Internal
{
class VideoView : public Control
{
protected:
  VideoView(Dali::VideoSyncMode syncMode);

  virtual ~VideoView();

public:
  /**
   * @copydoc Toolkit::DevelVideoView::New()
   */
  static Toolkit::VideoView New(VideoSyncMode syncMode);

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
   * @brief Emit the finished signal
   */
  void EmitSignalFinish();

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
  static bool DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes);

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the c
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  /**
   * @brief Updates video display area for window rendering target
   */
  void UpdateDisplayArea(Dali::PropertyNotification& source);

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
   * This interval is applied after the next call to SetNativeImageSourceForCurrentFrame.
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
   * @brief Sets the NativeImageSource for the current video frame.
   *
   * This method updates the texture used for the current frame and resets the interpolation timer.
   * The VideoView will then start interpolating between the previous frame and this new frame
   * over the interval set by SetFrameInterpolationInterval.
   *
   * @param[in] nativeImageSource A handle to the NativeImageSource containing the current video frame (tbm_surface).
   */
  void SetNativeImageSourceForCurrentFrame(NativeImageSourcePtr nativeImageSource);

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

  /**
   * @brief SetWindowSurfaceTarget for underlay video playback.
   */
  void SetWindowSurfaceTarget();

  /**
   * @brief SetNativeImageTarget for native image video playback.
   */
  void SetNativeImageTarget();

  /**
   * @brief CreateShader for native image target
   */
  Property::Map CreateShader();

  /**
   * @brief Checks whether the property has a string value.
   * @param Property value
   * @param String output
   * @return true if the output was found
   */
  bool GetStringFromProperty(const Dali::Property::Value& value, std::string& output);

  /*
   * @brief Internal version of SetProperty
   */
  void SetPropertyInternal(Property::Index index, const Property::Value& value);

  /*
   * @brief Apply properties after reset video player
   */
  void ApplyBackupProperties();

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
   * @brief Create OverlayTextureVisual
   *
   */
  void CreateOverlayTextureVisual();

  /*
   * @brief resize/move animation finished callback function
   *
   * This function is called the resize/move animation is finished,
   *
   */
  void OnAnimationFinished(Dali::Animation& animation);

  /*
   * @brief window's resize callback function
   * This function is called when window is resized.
   *
   * @param[in] winHandle The resized window's handle.
   * @param[in] size The window's new size.
   */
  void OnWindowResized(Dali::Window winHandle, Dali::Window::WindowSize size);

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
  Dali::DisplayArea mDisplayArea;

  Property::Index             mOverlayTextureVisualIndex{Property::INVALID_INDEX};
  Dali::Toolkit::Visual::Base mOverlayVisual;
  Dali::Toolkit::Visual::Base mOverlayTextureVisual;
  Dali::Toolkit::Visual::Base mTextureVisual;

  Dali::PropertyNotification mPositionUpdateNotification;
  Dali::PropertyNotification mSizeUpdateNotification;
  Dali::PropertyNotification mScaleUpdateNotification;

  Dali::Property::Map mPropertyBackup;

  int mCurrentVideoPlayPosition;
  int mFrameID;

  bool mIsPlay;
  bool mIsUnderlay;

  Dali::VideoSyncMode mSyncMode;
  int                 mSiblingOrder;

  // For frame interpolation
  Texture mPreviousFrameTexture; ///< Texture for mNativeImageSourcePrevious
  Texture mCurrentFrameTexture;  ///< Texture for mNativeImageSourceCurrent

  float mInterpolationInterval;    ///< Target duration for interpolation in seconds

  Property::Index mInterpolationFactorPropertyIndex; ///< Uniform index for uInterpolationFactor in shader
  Dali::Animation mInterpolationAnimation;           ///< Animation to drive the interpolation factor
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
