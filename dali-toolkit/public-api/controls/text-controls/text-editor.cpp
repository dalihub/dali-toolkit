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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

namespace Dali
{
namespace Toolkit
{
TextEditor TextEditor::New()
{
  return Internal::TextEditor::New();
}

TextEditor TextEditor::New(ControlBehaviour additionalBehaviour)
{
  return Internal::TextEditor::New(static_cast<Toolkit::Internal::Control::ControlBehaviour>(additionalBehaviour));
}

TextEditor::TextEditor()
{
}

TextEditor::TextEditor(const TextEditor& handle) = default;

TextEditor::TextEditor(TextEditor&& rhs) = default;

TextEditor& TextEditor::operator=(const TextEditor& handle) = default;

TextEditor& TextEditor::operator=(TextEditor&& rhs) = default;

TextEditor::~TextEditor()
{
}

TextEditor TextEditor::DownCast(BaseHandle handle)
{
  return Control::DownCast<TextEditor, Internal::TextEditor>(handle);
}

TextEditor::TextChangedSignalType& TextEditor::TextChangedSignal()
{
  return Dali::Toolkit::GetImpl(*this).TextChangedSignal();
}

TextEditor::InputStyleChangedSignalType& TextEditor::InputStyleChangedSignal()
{
  return Dali::Toolkit::GetImpl(*this).InputStyleChangedSignal();
}

TextEditor::ScrollStateChangedSignalType& TextEditor::ScrollStateChangedSignal()
{
  return Dali::Toolkit::GetImpl(*this).ScrollStateChangedSignal();
}

TextEditor::TextEditor(Internal::TextEditor& implementation)
: Control(implementation)
{
}

TextEditor::TextEditor(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::TextEditor>(internal);
}

} // namespace Toolkit

} // namespace Dali
