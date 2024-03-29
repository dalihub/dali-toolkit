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

// FILE HEADER
#include <dali-toolkit/internal/text/abstract-style-character-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
CharacterIndex AbstractStyleCharacterRun::GetStartCharacterIndex() const
{
  return characterRun.characterIndex;
}

Length AbstractStyleCharacterRun::GetNumberOfCharacters() const
{
  return characterRun.numberOfCharacters;
}

CharacterIndex AbstractStyleCharacterRun::GetEndCharacterIndex() const
{
  return characterRun.GetEndCharacterIndex();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
