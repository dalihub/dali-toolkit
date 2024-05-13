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
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelTextEditor
{
InputMethodContext GetInputMethodContext(TextEditor textEditor)
{
  return GetImpl(textEditor).GetInputMethodContext();
}

MaxLengthReachedSignalType& MaxLengthReachedSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).MaxLengthReachedSignal();
}

AnchorClickedSignalType& AnchorClickedSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).AnchorClickedSignal();
}

CursorPositionChangedSignalType& CursorPositionChangedSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).CursorPositionChangedSignal();
}

InputFilteredSignalType& InputFilteredSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).InputFilteredSignal();
}

SelectionChangedSignalType& SelectionChangedSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).SelectionChangedSignal();
}

SelectionClearedSignalType& SelectionClearedSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).SelectionClearedSignal();
}

SelectionStartedSignalType& SelectionStartedSignal(TextEditor textEditor)
{
  return GetImpl(textEditor).SelectionStartedSignal();
}

void SelectWholeText(TextEditor textEditor)
{
  GetImpl(textEditor).SelectWholeText();
}

void SelectNone(TextEditor textEditor)
{
  GetImpl(textEditor).SelectNone();
}

void SelectText(TextEditor textEditor, const uint32_t start, const uint32_t end)
{
  GetImpl(textEditor).SelectText(start, end);
}

void ScrollBy(TextEditor textEditor, Vector2 scroll)
{
  GetImpl(textEditor).ScrollBy(scroll);
}

Vector<Vector2> GetTextSize(TextEditor textEditor, const uint32_t startIndex, const uint32_t endIndex)
{
  return GetImpl(textEditor).GetTextSize(startIndex, endIndex);
}

Vector<Vector2> GetTextPosition(TextEditor textEditor, const uint32_t startIndex, const uint32_t endIndex)
{
  return GetImpl(textEditor).GetTextPosition(startIndex, endIndex);
}

Rect<> GetTextBoundingRectangle(TextEditor textEditor, uint32_t startIndex, uint32_t endIndex)
{
  return GetImpl(textEditor).GetTextBoundingRectangle(startIndex, endIndex);
}

string CopyText(TextEditor textEditor)
{
  return GetImpl(textEditor).CopyText();
}

string CutText(TextEditor textEditor)
{
  return GetImpl(textEditor).CutText();
}

void PasteText(TextEditor textEditor)
{
  GetImpl(textEditor).PasteText();
}

void SetRemoveFrontInset(TextEditor textEditor, const bool remove)
{
  GetImpl(textEditor).SetRemoveFrontInset(remove);
}

bool IsRemoveFrontInset(TextEditor textEditor)
{
  return GetImpl(textEditor).IsRemoveFrontInset();
}

void SetRemoveBackInset(TextEditor textEditor, const bool remove)
{
  GetImpl(textEditor).SetRemoveBackInset(remove);
}

bool IsRemoveBackInset(TextEditor textEditor)
{
  return GetImpl(textEditor).IsRemoveBackInset();
}

} // namespace DevelTextEditor

} // namespace Toolkit

} // namespace Dali
