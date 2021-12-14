#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H

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
   * @details Name "playRange", Type Property::ARRAY of Property::INTEGER or Property::ARRAY of Property::STRING.
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
   * @details Name "contentInfo", Type Property::MAP.
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
   * @brief Whether to apply mask in loading time or not.
   * @details Name "preappliedMask", type Property::BOOLEAN.
   * If it is true, mask image is applied on the texture in loading time with CPU.
   * If it is false, mask image is applied in runtime in shader.
   * @note It is used in the ImageVisual and AnimatedImageVisual. The default is true.
   */
  PREAPPLIED_MASK = ORIENTATION_CORRECTION + 12
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

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
