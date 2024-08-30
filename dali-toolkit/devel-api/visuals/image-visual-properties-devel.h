#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelImageVisual
{
namespace Property
{
enum Type
{
  URL                    = Dali::Toolkit::ImageVisual::Property::URL,
  FITTING_MODE           = Dali::Toolkit::ImageVisual::Property::FITTING_MODE,
  SAMPLING_MODE          = Dali::Toolkit::ImageVisual::Property::SAMPLING_MODE,
  DESIRED_WIDTH          = Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH,
  DESIRED_HEIGHT         = Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT,
  SYNCHRONOUS_LOADING    = Dali::Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING,
  BORDER_ONLY            = Dali::Toolkit::ImageVisual::Property::BORDER_ONLY,
  PIXEL_AREA             = Dali::Toolkit::ImageVisual::Property::PIXEL_AREA,
  WRAP_MODE_U            = Dali::Toolkit::ImageVisual::Property::WRAP_MODE_U,
  WRAP_MODE_V            = Dali::Toolkit::ImageVisual::Property::WRAP_MODE_V,
  BORDER                 = Dali::Toolkit::ImageVisual::Property::BORDER,
  ATLASING               = Dali::Toolkit::ImageVisual::Property::ATLASING,
  ALPHA_MASK_URL         = Dali::Toolkit::ImageVisual::Property::ALPHA_MASK_URL,
  BATCH_SIZE             = Dali::Toolkit::ImageVisual::Property::BATCH_SIZE,
  CACHE_SIZE             = Dali::Toolkit::ImageVisual::Property::CACHE_SIZE,
  FRAME_DELAY            = Dali::Toolkit::ImageVisual::Property::FRAME_DELAY,
  MASK_CONTENT_SCALE     = Dali::Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE,
  CROP_TO_MASK           = Dali::Toolkit::ImageVisual::Property::CROP_TO_MASK,
  LOAD_POLICY            = Dali::Toolkit::ImageVisual::Property::LOAD_POLICY,
  RELEASE_POLICY         = Dali::Toolkit::ImageVisual::Property::RELEASE_POLICY,
  ORIENTATION_CORRECTION = Dali::Toolkit::ImageVisual::Property::ORIENTATION_CORRECTION,

  /**
   * @brief Overlays the auxiliary image on top of an NPatch image.
   *
   * The resulting visual image will be at least as large as the
   * smallest possible n-patch or the auxiliary image, whichever is
   * larger.
   *
   * @details Name "auxiliaryImage", Type Property::STRING, URL of the image.
   * @note Default true
   */
  AUXILIARY_IMAGE = ORIENTATION_CORRECTION + 1,

  /**
   * @brief An alpha value for mixing between the masked main NPatch image and the auxiliary image
   * @details Name "auxiliaryImageAlpha", Type Property::FLOAT, between 0 and 1
   * @note Default 0
   */
  AUXILIARY_IMAGE_ALPHA = ORIENTATION_CORRECTION + 2,

  /**
   * @brief The number of times the AnimatedImageVisual or AnimatedVectorImageVisual will be looped.
   * @details Name "loopCount", type Property::INTEGER.
   * @note For Animated images only. Default -1. if < 0, loop unlimited. else, loop loopCount times.
   */
  LOOP_COUNT = ORIENTATION_CORRECTION + 3,

  /**
   * @brief The playing range the AnimatedVectorImageVisual will use.
   *
   * Animation will play between the values specified. The array can have two integer values.
   * Or it can have one or two strings, which are markers. More will be ignored.
   * Both values should be between 0 and the total frame number, otherwise they will be ignored.
   * If the range provided is not in proper order ( minimum, maximum ), it will be reordered.
   *
   * A marker has its start frame and end frame.
   * Animation will play between the start frame and the end frame of the marker if one marker is specified.
   * Or animation will play between the start frame of the first marker and the end frame of the second marker if two markers are specified.
   *
   * @details Name "playRange", Type Property::ARRAY of Property::INTEGER or Property::ARRAY of Property::STRING or Property::STRING (one marker).
   * @note Default 0 and the total frame number.
   */
  PLAY_RANGE = ORIENTATION_CORRECTION + 4,

  /**
   * @brief The playing state the AnimatedVectorImageVisual will use.
   * @details Name "playState", Type PlayState::Type (Property::INTEGER)
   * @note This property is read-only.
   */
  PLAY_STATE = ORIENTATION_CORRECTION + 5,

  /**
   * @brief The current frame number the AnimatedImageVisual and AnimatedVectorImageVisual will use.
   * @details Name "currentFrameNumber", Type Property::INTEGER, between [0, the maximum frame number] or between the play range if specified
   * @note This property is read-only.
   */
  CURRENT_FRAME_NUMBER = ORIENTATION_CORRECTION + 6,

  /**
   * @brief The total frame number the AnimatedImageVisual and AnimatedVectorImageVisual will use.
   * @details Name "totalFrameNumber", Type Property::INTEGER.
   * @note This property is read-only.
   */
  TOTAL_FRAME_NUMBER = ORIENTATION_CORRECTION + 7,

  /**
   * @brief  The stop behavior the AnimatedImageVisual and AnimatedVectorImageVisual will use.
   * @details Name "stopBehavior", Type StopBehavior::Type (Property::INTEGER)
   * @note Default value is StopBehavior::CURRENT_FRAME.
   */
  STOP_BEHAVIOR = ORIENTATION_CORRECTION + 8,

  /**
   * @brief  The looping mode the AnimatedVectorImageVisual will use.
   * @details Name "loopingMode", Type LoopingMode::Type (Property::INTEGER)
   * @note Default value is LoopingMode::RESTART.
   */
  LOOPING_MODE = ORIENTATION_CORRECTION + 9,

  /**
   * @brief The content information the AnimatedVectorImageVisual will use.
   * @details Type Property::MAP.
   * The map contains the layer name as a key and Property::Array as a value.
   * And the array contains 2 integer values which are the frame numbers, the start frame number and the end frame number of the layer.
   * @note This property is read-only.
   */
  CONTENT_INFO = ORIENTATION_CORRECTION + 10,

  /**
   * @brief Whether to redraw the image when the visual is scaled down.
   * @details Name "redrawInScalingDown", type Property::BOOLEAN.
   * @note It is used in the AnimatedVectorImageVisual. The default is true.
   */
  REDRAW_IN_SCALING_DOWN = ORIENTATION_CORRECTION + 11,

  /**
   * @brief Whether to apply mask in loading time or rendering time.
   * @details Name "maskingType", type PlayState::Type (Property::INTEGER).
   * In general, MASKING_ON_LOADING is the default behavior.
   * However, if the visual uses an external texture, only MASKING_ON_RENDERING is possible.
   * So we change its value to MASKING_ON_RENDERING even if the visual sets the MASKING_TYPE as MASKING_ON_LOADING when it uses external texture.
   * @note It is used in the ImageVisual and AnimatedImageVisual. The default is MASKING_ON_LOADING.
   */
  MASKING_TYPE = ORIENTATION_CORRECTION + 12,

  /**
   * @brief If true, uploads texture before ResourceReady signal is emitted. Otherwise uploads after texture load is completed.
   * @details Name "fastTrackUploading", type Property::BOOLEAN
   * If true, the upload happens without event-thread dependency, but the following need to be considered:
   *  - Texture size is not valid until upload is fully complete.
   *  - Texture cannot be cached (a new image is uploaded every time).
   *  - Seamless visual change is not supported.
   *  - The following, if set are also not supported and will be ignored:
   *    - Alpha masking
   *    - Synchronous loading
   *    - Reload action
   *    - Atlas loading
   *    - Custom shader
   * @note Used by the ImageVisual. The default is false.
   */
  FAST_TRACK_UPLOADING = ORIENTATION_CORRECTION + 13,

  /**
   * @brief Whether to enable broken image in image visual.
   * Some of visual don't need to show broken image(ex. placeholder)
   * Disable broken image for these visuals.
   * default is true.
   */
  ENABLE_BROKEN_IMAGE = ORIENTATION_CORRECTION + 14,

  /**
   * @brief The marker information the AnimatedVectorImageVisual will use.
   * @details Type Property::MAP.
   * The map contains the marker name as a key and Property::Array as a value.
   * And the array contains 2 integer values which are the frame numbers, the start frame number and the end frame number of the marker.
   * @note This property is read-only.
   */
  MARKER_INFO = ORIENTATION_CORRECTION + 15,

  /**
   * @brief Whether to AnimatedVectorImageVisual fixed cache or not.
   * @details Name "enableFrameCache", type Property::BOOLEAN.
   * If this property is true, AnimatedVectorImageVisual enable frame cache for loading and keeps loaded frame
   * until the visual is removed. It reduces CPU cost when the animated image will be looping.
   * But it can spend a lot of memory if the resource has high resolution image or many frame count.
   * @note It is used in the AnimatedVectorImageVisual. The default is false
   */
  ENABLE_FRAME_CACHE = ORIENTATION_CORRECTION + 16,

  /**
   * @brief Whether notify AnimatedVectorImageVisual to render thread after every rasterization or not.
   * @details Name "notifyAfterRasterization", type Property::BOOLEAN.
   * If this property is true, AnimatedVectorImageVisual send notify to render thread after every rasterization.
   * If false, AnimatedVectorImageVisual set Renderer's Behaviour as Continouly (mean, always update the render thread.)
   *
   * This flag is useful if given resource has low fps, so we don't need to render every frame.
   * @note It is used in the AnimatedVectorImageVisual. The default is false.
   */
  NOTIFY_AFTER_RASTERIZATION = ORIENTATION_CORRECTION + 17,

  /**
   * @brief Whether to synchronize image texture size to visual size.
   * @details Name "synchronousSizing", type Property::BOOLEAN.
   * If this property is true, ImageVisual ignores mDesiredSize.
   * @note Used by the ImageVisual. The default is false.
   */
  SYNCHRONOUS_SIZING = ORIENTATION_CORRECTION + 18,

  /**
   * @brief Specifies a speed factor for the animated image frame.
   * @details Name "frameSpeedFactor", type Property::FLOAT.
   *
   * The speed factor is a multiplier of the normal velocity of the animation. Values between [0,1] will
   * slow down the animation and values above one will speed up the animation.
   *
   * @note The range of this value is clamped between [0.01f ~ 100.0f].
   *       It might be supported out of bound, and negative value in future.
   * @note It is used in the AnimatedImageVisual and AnimatedVectorImageVisual. The default is 1.0f.
   */
  FRAME_SPEED_FACTOR = ORIENTATION_CORRECTION + 19,
};

} //namespace Property

/**
 * @brief Enumeration for what state the animation is in.
 */
namespace PlayState
{
enum Type
{
  STOPPED, ///< Animation has stopped
  PLAYING, ///< The animation is playing
  PAUSED   ///< The animation is paused
};

} // namespace PlayState

/**
 * @brief Enumeration for what to do when the animation is stopped.
 */
namespace StopBehavior
{
enum Type
{
  CURRENT_FRAME, ///< When the animation is stopped, the current frame is shown.
  FIRST_FRAME,   ///< When the animation is stopped, the first frame is shown.
  LAST_FRAME     ///< When the animation is stopped, the last frame is shown.
};

} // namespace StopBehavior

/**
 * @brief Enumeration for what looping mode is in.
 */
namespace LoopingMode
{
enum Type
{
  RESTART,     ///< When the animation arrives at the end in looping mode, the animation restarts from the beginning.
  AUTO_REVERSE ///< When the animation arrives at the end in looping mode, the animation reverses direction and runs backwards again.
};

} // namespace LoopingMode

/**
 * @brief Enumeration for what masking type is in.
 */
namespace MaskingType
{
enum Type
{
  MASKING_ON_RENDERING, ///< Alpha masking is applied for each rendering time. (On GPU)
  MASKING_ON_LOADING    ///< Alpha masking is applied when the image is loading. (On CPU)
};

}

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
