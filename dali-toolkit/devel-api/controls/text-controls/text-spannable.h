#ifndef DALI_TOOLKIT_TEXT_TEXT_SPANNABLE_H
#define DALI_TOOLKIT_TEXT_TEXT_SPANNABLE_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/spanned.h>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Set the @p spannedText into @p textLabel
 * the spanned text contains content (text) and format (spans with ranges)
 * the text is copied into text-controller and the spans are applied on ranges
 *
 * @note the TEXT in @p textLabel will be replaced with text from @p spannedText
 * and all the applied styles @p textLabel will be replaced the styles from @p spannedText
 * in-case there are styles were applied from markup-processor will be removed
 *
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] spannedText the text with spans.
 */
DALI_TOOLKIT_API void SetSpannedText(TextLabel textLabel, const Spanned& spannedText);

/**
 * @brief Set the @p spannedText into @p textField
 * the spanned text contains content (text) and format (spans with ranges)
 * the text is copied into text-controller and the spans are applied on ranges
 *
 * @note the TEXT in @p textField will be replaced with text from @p spannedText
 * and all the applied styles @p textField will be replaced the styles from @p spannedText
 * in-case there are styles were applied from markup-processor will be removed
 *
 *
 * @param[in] textField The instance of TextField.
 * @param[in] spannedText the text with spans.
 */
DALI_TOOLKIT_API void SetSpannedText(TextField textField, const Spanned& spannedText);

/**
 * @brief Set the @p spannedText into @p textEditor
 * the spanned text contains content (text) and format (spans with ranges)
 * the text is copied into text-controller and the spans are applied on ranges
 *
 * @note the TEXT in @p textEditor will be replaced with text from @p spannedText
 * and all the applied styles @p textEditor will be replaced the styles from @p spannedText
 * in-case there are styles were applied from markup-processor will be removed
 *
 *
 * @param[in] textEditor The instance of TextEditor.
 * @param[in] spannedText the text with spans.
 */
DALI_TOOLKIT_API void SetSpannedText(TextEditor textEditor, const Spanned& spannedText);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_TEXT_SPANNABLE_H
