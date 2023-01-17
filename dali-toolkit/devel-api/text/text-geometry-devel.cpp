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
#include <dali-toolkit/devel-api/text/text-geometry-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace TextGeometry
{

Rect<float> GetLineBoundingRectangle(TextEditor editor, const uint32_t lineIndex)
{
  return GetImpl(editor).GetLineBoundingRectangle(lineIndex);
}

Rect<float> GetLineBoundingRectangle(TextLabel label, const uint32_t lineIndex)
{
  return GetImpl(label).GetLineBoundingRectangle(lineIndex);
}

Rect<float> GetLineBoundingRectangle(TextField field, const uint32_t lineIndex)
{
  return GetImpl(field).GetLineBoundingRectangle(lineIndex);
}

Rect<float> GetCharacterBoundingRectangle(TextLabel label, const uint32_t charIndex)
{
  return GetImpl(label).GetCharacterBoundingRectangle(charIndex);
}

Rect<float> GetCharacterBoundingRectangle(TextEditor editor, const uint32_t charIndex)
{
  return GetImpl(editor).GetCharacterBoundingRectangle(charIndex);
}

Rect<float> GetCharacterBoundingRectangle(TextField field, const uint32_t charIndex)
{
  return GetImpl(field).GetCharacterBoundingRectangle(charIndex);
}

int GetCharacterIndexAtPosition(TextLabel label, float visualX, float visualY)
{
  return GetImpl(label).GetCharacterIndexAtPosition(visualX, visualY);
}

int GetCharacterIndexAtPosition(TextField field, float visualX, float visualY)
{
  return GetImpl(field).GetCharacterIndexAtPosition(visualX, visualY);
}

int GetCharacterIndexAtPosition(TextEditor editor, float visualX, float visualY)
{
  return GetImpl(editor).GetCharacterIndexAtPosition(visualX, visualY);
}

} //namespace TextGeometry

} // namespace Text

} // namespace Toolkit

} // namespace Dali
