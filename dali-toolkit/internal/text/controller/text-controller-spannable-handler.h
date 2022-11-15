#ifndef DALI_TOOLKIT_TEXT_SPANNABLE_HANDLER_H
#define DALI_TOOLKIT_TEXT_SPANNABLE_HANDLER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Methods that update the text
 */
struct Controller::SpannableHandler
{
  /// @copydoc Text::Contoller::SetSpannedText
  /// @param[in] controller The controller
  static void SetSpannedText(Controller& controller, const Text::Spanned& spannedText);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SPANNABLE_HANDLER_H
