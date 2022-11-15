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

// CLASS HEADER
#include <dali-toolkit/devel-api/controls/text-controls/text-spannable.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void SetSpannedText(TextLabel textLabel, const Spanned& spannedText)
{
  return GetImpl(textLabel).SetSpannedText(spannedText);
}

void SetSpannedText(TextField textField, const Spanned& spannedText)
{
  return GetImpl(textField).SetSpannedText(spannedText);
}

void SetSpannedText(TextEditor textEditor, const Spanned& spannedText)
{
  return GetImpl(textEditor).SetSpannedText(spannedText);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
