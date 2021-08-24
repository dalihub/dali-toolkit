/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

} // namespace DevelTextEditor

} // namespace Toolkit

} // namespace Dali
