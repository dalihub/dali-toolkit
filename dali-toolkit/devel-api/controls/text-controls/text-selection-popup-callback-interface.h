#ifndef __DALI_TOOLKIT_TEXT_SELECTION_POPUP_CALLBACK_INTERFACE_H__
#define __DALI_TOOLKIT_TEXT_SELECTION_POPUP_CALLBACK_INTERFACE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Interface used to receive the TextSelectionPopup's button callbacks.
 */
class TextSelectionPopupCallbackInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~TextSelectionPopupCallbackInterface()
  {}

  /**
   * @brief Called when a button is touched.
   *
   * @param[in] button The button identifier.
   */
  virtual void TextPopupButtonTouched( TextSelectionPopup::Buttons button ) = 0;
};

} // namespace Toolkit

} // namespace Dali


#endif // __DALI_TOOLKIT_TEXT_SELECTION_POPUP_CALLBACK_INTERFACE_H__
