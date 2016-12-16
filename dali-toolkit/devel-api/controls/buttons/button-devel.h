#ifndef __DALI_TOOLKIT_BUTTON_DEVEL_H__
#define __DALI_TOOLKIT_BUTTON_DEVEL_H__

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
#include <dali-toolkit/public-api/controls/buttons/button.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelButton
{
/**
 *
 * @section ButtonProperties Properties
 * |%Property enum                                         |String name                             |Type          |Writable|Animatable|
 * |-------------------------------------------------------|----------------------------------------|--------------|--------|----------|
 * | Property::DISABLED                                    | disabled                               |  BOOLEAN     | Y      | N        |
 * | Property::AUTO_REPEATING                              | autoRepeating                          |  BOOLEAN     | Y      | N        |
 * | Property::INITIAL_AUTO_REPEATING_DELAY                | initialAutoRepeatingDelay              |  FLOAT       | Y      | N        |
 * | Property::NEXT_AUTO_REPEATING_DELAY                   | nextAutoRepeatingDelay                 |  FLOAT       | Y      | N        |
 * | Property::TOGGLABLE                                   | togglable                              |  BOOLEAN     | Y      | N        |
 * | Property::SELECTED                                    | selected                               |  BOOLEAN     | Y      | N        |
 * | Property::LABEL                                       | label                                  |  MAP         | Y      | N        |
 * | Property::UNSELECTED_STATE_VISUAL                     | unselectedVisual                       |  MAP         | Y      | N        |
 * | Property::SELECTED_VISUAL                             | selectedVisual                         |  MAP         | Y      | N        |
 * | Property::DISABLED_SELECTED_VISUAL                    | disabledSelectedVisual                 |  MAP         | Y      | N        |
 * | Property::DISABLED_UNSELECTED_VISUAL                  | disabledUnselectedVisual               |  MAP         | Y      | N        |
 * | Property::UNSELECTED_BACKGROUND_VISUAL                | unselectedBackgroundVisual             |  MAP         | Y      | N        |
 * | Property::SELECTED_BACKGROUND_VISUAL                  | selectedBackgroundVisual               |  MAP         | Y      | N        |
 * | Property::DISABLED_UNSELECTED_BACKGROUND_VISUAL       | disabledUnselectedBackgroundVisual     |  MAP         | Y      | N        |
 * | Property::DISABLED_SELECTED_BACKGROUND_VISUAL         | disabledSelectedBackgroundVisual       |  MAP         | Y      | N        |
 * | Property::LABEL_RELATIVE_ALIGNMENT                    | labelRelativeAlignment                 |  STRING      | Y      | N        |
 * -------------------------------------------------------------------------------------------------------------------------------------
 *
 *
 */

  namespace Property
  {
    enum Type
    {
      DISABLED = Dali::Toolkit::Button::Property::DISABLED,
      AUTO_REPEATING = Dali::Toolkit::Button::Property::AUTO_REPEATING,
      INITIAL_AUTO_REPEATING_DELAY = Dali::Toolkit::Button::Property::INITIAL_AUTO_REPEATING_DELAY,
      NEXT_AUTO_REPEATING_DELAY = Dali::Toolkit::Button::Property:: NEXT_AUTO_REPEATING_DELAY,
      TOGGLABLE = Dali::Toolkit::Button::Property::TOGGLABLE,
      SELECTED = Dali::Toolkit::Button::Property::SELECTED,
      UNSELECTED_STATE_IMAGE = Dali::Toolkit::Button::Property::UNSELECTED_STATE_IMAGE,
      SELECTED_STATE_IMAGE = Dali::Toolkit::Button::Property::SELECTED_STATE_IMAGE,
      DISABLED_STATE_IMAGE = Dali::Toolkit::Button::Property::DISABLED_STATE_IMAGE,
      UNSELECTED_COLOR = Dali::Toolkit::Button::Property::UNSELECTED_COLOR,
      SELECTED_COLOR = Dali::Toolkit::Button::Property::SELECTED_COLOR,
      LABEL = Dali::Toolkit::Button::Property::LABEL,
      LABEL_TEXT = Dali::Toolkit::Button::Property::LABEL_TEXT,

      /**
       * @brief name "unselectedVisual", type string if it is a url, map otherwise
       * @details Sets the unselected button foreground/icon visual
       */
      UNSELECTED_VISUAL = LABEL_TEXT + 1,

      /**
       * @brief name "selectedImage", type string if it is a url, map otherwise
       * @details Sets the selected button foreground/icon visual
       * @SINCE_1_2.18
       */
      SELECTED_VISUAL,

      /**
       * @brief name "disabledSelectedVisual", type string if it is a url, map otherwise
       * @details Sets the disabled selected state foreground/icon button visual
       */
      DISABLED_SELECTED_VISUAL,

      /**
       * @brief name "disabledUnSelectedVisual", type string if it is a url, map otherwise
       * @details Sets the disabled unselected state foreground/icon visual
       */
      DISABLED_UNSELECTED_VISUAL,

      /**
       * @brief name "unselectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the disabled in the unselected state background, button visual
       */
      UNSELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "selectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the selected background button visual
       */
      SELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "disabledUnselectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the disabled while unselected background button visual
       */
      DISABLED_UNSELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "disabledSelectedBackgroundVisual", type string if it is a url, map otherwise
       * @details Sets the disabled while selected background button visual
       */
      DISABLED_SELECTED_BACKGROUND_VISUAL,

      /**
       * @brief name "labelRelativeAlignment", type STRING
       * @details Sets the position of the the label in relation to the foreground/icon if both present
       */
      LABEL_RELATIVE_ALIGNMENT,

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

#endif // __DALI_TOOLKIT_BUTTON_DEVEL_H__
