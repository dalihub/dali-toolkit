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

// CLASS HEADER
#include <dali-toolkit/internal/text/segmentation.h>

// EXTERNAL INCLUDES
#include <dali/public-api/text-abstraction/segmentation.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

void SetLineBreakInfo( const Vector<Character>& text,
                       Vector<LineBreakInfo>& lineBreakInfo )
{
  const Length numberOfCharacters = text.Count();

  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Retrieve the line break info.
  lineBreakInfo.Resize( numberOfCharacters );
  TextAbstraction::Segmentation::Get().GetLineBreakPositions( text.Begin(),
                                                              numberOfCharacters,
                                                              lineBreakInfo.Begin() );
}

void ReplaceLineBreakInfo( LogicalModel& model,
                           CharacterIndex characterIndex,
                           Length numberOfCharactersToRemove,
                           Length numberOfCharactersToInsert )
{
}

void SetWordBreakInfo( const Vector<Character>& text,
                       Vector<WordBreakInfo>& wordBreakInfo )
{
  const Length numberOfCharacters = text.Count();

  if( 0u == numberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Retrieve the word break info.
  wordBreakInfo.Resize( numberOfCharacters );
  TextAbstraction::Segmentation::Get().GetWordBreakPositions( text.Begin(),
                                                              numberOfCharacters,
                                                              wordBreakInfo.Begin() );
}

void ReplaceWordBreakInfo( LogicalModel& model,
                           CharacterIndex characterIndex,
                           Length numberOfCharactersToRemove,
                           Length numberOfCharactersToInsert )
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
