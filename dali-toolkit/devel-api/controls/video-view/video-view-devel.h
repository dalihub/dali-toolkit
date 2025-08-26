#ifndef DALI_TOOLKIT_VIDEO_VIEW_DEVEL_H
#define DALI_TOOLKIT_VIDEO_VIEW_DEVEL_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/any.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelVideoView
{
/**
 * @brief Returns the internal media player.
 * @param[in] videoView The current VideoView
 * @return The internal media player of current VideoView
 */
DALI_TOOLKIT_API Any GetMediaPlayer(VideoView videoView);

/**
 * @brief Creates an initialized VideoView with synchronization mode.
 *
 * The syncMode is for synchronization between UI(transparent hole) and underlay video.
 *
 * @param[in] syncMode The synchronization mode between the UI (transparent hole) and VideoPlayer
 * @return A handle to a newly allocated Dali VideoView
 */
DALI_TOOLKIT_API VideoView New(VideoSyncMode syncMode);

/**
 * @brief Play the resize or move animation with synchronization between UI(transparent hole) and video player
 *
 * The resize and move animation's play() function is called.
 * If the animation is played, UI and video player will work synchronization.
 *
 * @param[in] videoView The current VideoView
 * @param[in] animation The animation for video view's resize or move.
 */
DALI_TOOLKIT_API void PlayAnimation(VideoView videoView, Animation animation);

/**
 * @brief Sets auto rotation feature. If enabled, video will rotate automatically according to the video orientation.
 *
 * @param[in] videoView The current VideoView
 * @param[in] enable true if auto rotation should be enabled, false otherwise. Default value is false.
 */
DALI_TOOLKIT_API void SetAutoRotationEnabled(VideoView videoView, bool enable);

/**
 * @brief Checks whether auto rotation feature is enabled.
 *
 * @param[in] videoView The current VideoView
 * @return true if auto rotation is enabled, false otherwise. Default value is false.
 */
DALI_TOOLKIT_API bool IsAutoRotationEnabled(VideoView videoView);

/**
 * @brief Sets letter box feature. If enabled, the video will play in the video player's aspect ratio.
 *
 * @param[in] videoView The current VideoView
 * @param[in] enable true if letter box should be enabled, false otherwise. Default value is false.
 */
DALI_TOOLKIT_API void SetLetterBoxEnabled(VideoView videoView, bool enable);

/**
 * @brief Checks whether letter box feature is enabled.
 *
 * @param[in] videoView The current VideoView
 * @return true if letter box is enabled, false otherwise. Default value is false.
 */
DALI_TOOLKIT_API bool IsLetterBoxEnabled(VideoView videoView);

/**
 * @brief Sets the time interval for frame interpolation.
 *
 * The interpolation factor will progress from 0.0 to 1.0 over this duration.
 * This interval is applied after the next call to SetNativeImageSourceForCurrentFrame.
 *
 * @param[in] videoView The current VideoView
 * @param[in] intervalSeconds The duration in seconds for interpolation.
 * @SINCE_2_4.34
 */
DALI_TOOLKIT_API void SetFrameInterpolationInterval(VideoView videoView, float intervalSeconds);

/**
 * @brief Gets the time interval for frame interpolation.
 *
 * @param[in] videoView The current VideoView
 * @return The duration in seconds for interpolation.
 * @note This method must be called on the main thread.
 * @SINCE_2_4.34
 */
DALI_TOOLKIT_API float GetFrameInterpolationInterval(VideoView videoView);

/**
 * @brief Sets the NativeImageSource for the current video frame.
 *
 * This method updates the texture used for the current frame and resets the interpolation timer.
 * The VideoView will then start interpolating between the previous frame and this new frame
 * over the interval set by SetFrameInterpolationInterval.
 *
 * @param[in] videoView The current VideoView
 * @param[in] nativeImageSource A handle to the NativeImageSource containing the current video frame (tbm_surface).
 * @note This method must be called on the main thread.
 * @SINCE_2_4.34
 */
DALI_TOOLKIT_API void SetNativeImageSourceForCurrentFrame(VideoView videoView, NativeImageSourcePtr nativeImageSource);

} // namespace DevelVideoView

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VIDEO_VIEW_DEVEL_H
