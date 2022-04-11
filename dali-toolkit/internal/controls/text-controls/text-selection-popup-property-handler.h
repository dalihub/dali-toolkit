#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_PROPERTY_HANDLER_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_PROPERTY_HANDLER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
 */

#include <dali-toolkit/internal/controls/text-controls/text-selection-popup-impl.h>

namespace Dali::Toolkit::Internal
{
/**
 * Class to manage properties for the TextSelectionPopup
 */
struct TextSelectionPopup::PropertyHandler
{
  /**
   * Set properties on the text selection popup
   *
   * @param[in] selectionPopup The handle for the text selection popup
   * @param[in] index The property index of the property to set
   * @param[in] value The value to set
   */
  static void SetProperty(Toolkit::TextSelectionPopup selectionPopup, Property::Index index, const Property::Value& value);

  /**
   * Get properties from the text selection popup
   *
   * @param[in] selectionPopup The handle for the text selection popup
   * @param[in] index The property index of the property to set
   * @return the value
   */
  static Property::Value GetProperty(Toolkit::TextSelectionPopup selectionPopup, Property::Index index);
};

} // namespace Dali::Toolkit::Internal

#endif //DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_PROPERTY_HANDLER_H
