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
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelTextField
{
InputMethodContext GetInputMethodContext(TextField textField)
{
  return GetImpl(textField).GetInputMethodContext();
}

AnchorClickedSignalType& AnchorClickedSignal(TextField textField)
{
  return GetImpl(textField).AnchorClickedSignal();
}

CursorPositionChangedSignalType& CursorPositionChangedSignal(TextField textField)
{
  return GetImpl(textField).CursorPositionChangedSignal();
}

InputFilteredSignalType& InputFilteredSignal(TextField textField)
{
  return GetImpl(textField).InputFilteredSignal();
}

SelectionChangedSignalType& SelectionChangedSignal(TextField textField)
{
  return GetImpl(textField).SelectionChangedSignal();
}

SelectionClearedSignalType& SelectionClearedSignal(TextField textField)
{
  return GetImpl(textField).SelectionClearedSignal();
}

SelectionStartedSignalType& SelectionStartedSignal(TextField textField)
{
  return GetImpl(textField).SelectionStartedSignal();
}

void SelectWholeText(TextField textField)
{
  GetImpl(textField).SelectWholeText();
}

void SelectNone(TextField textField)
{
  GetImpl(textField).SelectNone();
}

void SelectText(TextField textField, const uint32_t start, const uint32_t end)
{
  GetImpl(textField).SelectText(start, end);
}

Vector<Vector2> GetTextSize(TextField textField, const uint32_t startIndex, const uint32_t endIndex)
{
  return GetImpl(textField).GetTextSize(startIndex, endIndex);
}

Vector<Vector2> GetTextPosition(TextField textField, const uint32_t startIndex, const uint32_t endIndex)
{
  return GetImpl(textField).GetTextPosition(startIndex, endIndex);
}

Rect<> GetTextBoundingRectangle(TextField textField, uint32_t startIndex, uint32_t endIndex)
{
  return GetImpl(textField).GetTextBoundingRectangle(startIndex, endIndex);
}

string CopyText(TextField textField)
{
  return GetImpl(textField).CopyText();
}

string CutText(TextField textField)
{
  return GetImpl(textField).CutText();
}

void PasteText(TextField textField)
{
  GetImpl(textField).PasteText();
}

void SetRemoveFrontInset(TextField textField, const bool remove)
{
  GetImpl(textField).SetRemoveFrontInset(remove);
}

bool IsRemoveFrontInset(TextField textField)
{
  return GetImpl(textField).IsRemoveFrontInset();
}

void SetRemoveBackInset(TextField textField, const bool remove)
{
  GetImpl(textField).SetRemoveBackInset(remove);
}

bool IsRemoveBackInset(TextField textField)
{
  return GetImpl(textField).IsRemoveBackInset();
}

} // namespace DevelTextField

} // namespace Toolkit

} // namespace Dali
