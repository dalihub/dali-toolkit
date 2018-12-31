#ifndef DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
  URL                 = Dali::Toolkit::ImageVisual::Property::URL,
  FITTING_MODE        = Dali::Toolkit::ImageVisual::Property::FITTING_MODE,
  SAMPLING_MODE       = Dali::Toolkit::ImageVisual::Property::SAMPLING_MODE,
  DESIRED_WIDTH       = Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH,
  DESIRED_HEIGHT      = Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT,
  SYNCHRONOUS_LOADING = Dali::Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING,
  BORDER_ONLY         = Dali::Toolkit::ImageVisual::Property::BORDER_ONLY,
  PIXEL_AREA          = Dali::Toolkit::ImageVisual::Property::PIXEL_AREA,
  WRAP_MODE_U         = Dali::Toolkit::ImageVisual::Property::WRAP_MODE_U,
  WRAP_MODE_V         = Dali::Toolkit::ImageVisual::Property::WRAP_MODE_V,
  BORDER              = Dali::Toolkit::ImageVisual::Property::BORDER,
  ATLASING            = Dali::Toolkit::ImageVisual::Property::ATLASING,
  ALPHA_MASK_URL      = Dali::Toolkit::ImageVisual::Property::ALPHA_MASK_URL,
  BATCH_SIZE          = Dali::Toolkit::ImageVisual::Property::BATCH_SIZE,
  CACHE_SIZE          = Dali::Toolkit::ImageVisual::Property::CACHE_SIZE,
  FRAME_DELAY         = Dali::Toolkit::ImageVisual::Property::FRAME_DELAY,
  MASK_CONTENT_SCALE  = Dali::Toolkit::ImageVisual::Property::MASK_CONTENT_SCALE,
  CROP_TO_MASK        = Dali::Toolkit::ImageVisual::Property::CROP_TO_MASK,
  LOAD_POLICY         = Dali::Toolkit::ImageVisual::Property::LOAD_POLICY,
  RELEASE_POLICY      = Dali::Toolkit::ImageVisual::Property::RELEASE_POLICY,
  ORIENTATION_CORRECTION = Dali::Toolkit::ImageVisual::Property::ORIENTATION_CORRECTION,

  /**
   * @brief Overlays the auxiliary iamge on top of an NPatch image.
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
   * @brief The number of times the AnimatedImageVisual will be looped.
   * @details Name "loopCount", type Property::INTEGER.
   * @note For Animated images only. Default -1. if < 0, loop unlimited. else, loop loopCount times.
   */
  LOOP_COUNT = ORIENTATION_CORRECTION + 3,

  /**
   * @brief The playing range the AnimatedVectorImageVisual will use.
   *
   * Animation will play between the values specified. Both values should be between 0-1,
   * otherwise they will be ignored. If the range provided is not in proper order ( minimum,maximum ), it will be reordered.
   *
   * @details Name "playRange", Type Property::VECTOR2, between 0 and 1
   * @note Default 0 and 1
   */
  PLAY_RANGE = ORIENTATION_CORRECTION + 4,

  /**
   * @brief The playing state the AnimatedVectorImageVisual will use.
   * @details Name "playState", type PlayState (Property::INTEGER)
   * @note This property is read-only.
   */
  PLAY_STATE = ORIENTATION_CORRECTION + 5,

  /**
   * @brief The animation progress the AnimatedVectorImageVisual will use.
   * @details Name "currentProgress", Type Property::FLOAT, between [0, 1] or between the play range if specified
   * @note This property is read-only.
   */
  CURRENT_PROGRESS = ORIENTATION_CORRECTION + 6
};

} //namespace Property

/**
 * @brief Enumeration for what state the animation is in.
 */
enum class PlayState
{
  STOPPED,   ///< Animation has stopped
  PLAYING,   ///< The animation is playing
  PAUSED     ///< The animation is paused
};

} // namespace DevelImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_VISUALS_IMAGE_VISUAL_PROPERTIES_DEVEL_H
