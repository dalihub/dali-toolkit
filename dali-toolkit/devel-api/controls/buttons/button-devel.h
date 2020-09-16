#ifndef DALI_TOOLKIT_BUTTON_DEVEL_H
#define DALI_TOOLKIT_BUTTON_DEVEL_H

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
#include <dali-toolkit/public-api/controls/buttons/button.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelButton
{
namespace Property
{
enum Type
{
  /**
       * @brief name "labelRelativeAlignment", type STRING
       * @details Sets the position of the the label in relation to the foreground/icon if both present
       */
  LABEL_RELATIVE_ALIGNMENT = Dali::Toolkit::Button::Property::DISABLED_SELECTED_BACKGROUND_VISUAL + 1,

  /**
       * @brief name "labelPadding", type Vector4
       * @details Sets the padding around the text
       */
  LABEL_PADDING,

  /**
       * @brief name "visualPadding", type Vector4
       * @details Sets the padding around the foreground visual
       */
  VISUAL_PADDING
};
} // namespace Property

} // namespace DevelButton

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_BUTTON_DEVEL_H
