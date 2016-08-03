#ifndef DALI_TOOLKIT_BATCH_IMAGE_VISUAL_PROPERTIES_H
#define DALI_TOOLKIT_BATCH_IMAGE_VISUAL_PROPERTIES_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{

namespace Toolkit
{

namespace BatchImageVisual
{

namespace Property
{

enum
{
  /**
   * @brief The URL of the image.
   * @details Name "url", type Property::STRING.
   * @SINCE_1_1.46
   * @note Mandatory.
   */
  URL = VISUAL_PROPERTY_START_INDEX,

  /**
   * @brief The image width.
   * @details Name "desiredWidth", type Property::INTEGER.
   * @SINCE_1_1.46
   * @note Optional. If not specified, the actual image width is used.
   */
  DESIRED_WIDTH,

  /**
   * @brief The image height.
   * @details Name "desiredHeight", type Property::INTEGER.
   * @SINCE_1_1.46
   * @note Optional. If not specified, the actual image height is used.
   */
  DESIRED_HEIGHT,
};

} // namespace Property

} // namespace BatchImageVisual

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_BATCH_IMAGE_VISUAL_PROPERTIES_H
