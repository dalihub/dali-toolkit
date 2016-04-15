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

// FILE HEADER
#include <dali-toolkit/internal/text/segmentation.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/segmentation.h>
#ifdef DEBUG_ENABLED
#include <string>
#include <dali/integration-api/debug.h>
#endif

// INTERNAL INCLUDES
#ifdef DEBUG_ENABLED
#include <dali-toolkit/internal/text/character-set-conversion.h>
#endif

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_SEGMENTATION");
#endif

} // namespace


namespace Dali
{

namespace Toolkit
{

namespace Text
{

void SetLineBreakInfo( const Vector<Character>& text,
                       CharacterIndex startIndex,
                       Length numberOfCharacters,
                       Vector<LineBreakInfo>& lineBreakInfo )
{
  const Length totalNumberOfCharacters = text.Count();

  if( 0u == totalNumberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Retrieve the line break info.
  lineBreakInfo.Resize( totalNumberOfCharacters );

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  LineBreakInfo* lineBreakInfoBuffer = NULL;
  Vector<LineBreakInfo> newLineBreakInfo;

  if( updateCurrentBuffer )
  {
    newLineBreakInfo.Resize( numberOfCharacters );
    lineBreakInfoBuffer = newLineBreakInfo.Begin();
  }
  else
  {
    lineBreakInfoBuffer = lineBreakInfo.Begin();
  }

  // Retrieve the line break info.
  TextAbstraction::Segmentation::Get().GetLineBreakPositions( text.Begin() + startIndex,
                                                              numberOfCharacters,
                                                              lineBreakInfoBuffer );

  // If the line break info is updated, it needs to be inserted in the model.
  if( updateCurrentBuffer )
  {
    lineBreakInfo.Insert( lineBreakInfo.Begin() + startIndex,
                          newLineBreakInfo.Begin(),
                          newLineBreakInfo.End() );
    lineBreakInfo.Resize( totalNumberOfCharacters );
  }

#ifdef DEBUG_ENABLED
  if( gLogFilter->IsEnabledFor(Debug::Verbose) )
  {
    std::string utf8;
    Utf32ToUtf8( text.Begin(), numberOfCharacters, utf8 );

    std::string info;
    info.reserve( numberOfCharacters );
    for( unsigned int i=0; i<lineBreakInfo.Count(); ++i )
    {
      info.push_back( static_cast<char>('0' + lineBreakInfo[i]) );
    }

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SetLineBreakInfo Characters: %s\n", utf8.c_str() );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SetLineBreakInfo Break info: %s\n", info.c_str() );
  }
#endif
}

void SetWordBreakInfo( const Vector<Character>& text,
                       CharacterIndex startIndex,
                       Length numberOfCharacters,
                       Vector<WordBreakInfo>& wordBreakInfo )
{
  const Length totalNumberOfCharacters = text.Count();

  if( 0u == totalNumberOfCharacters )
  {
    // Nothing to do if there are no characters.
    return;
  }

  // Resize the vector.
  wordBreakInfo.Resize( totalNumberOfCharacters );

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  WordBreakInfo* wordBreakInfoBuffer = NULL;
  Vector<WordBreakInfo> newWordBreakInfo;

  if( updateCurrentBuffer )
  {
    newWordBreakInfo.Resize( numberOfCharacters );
    wordBreakInfoBuffer = newWordBreakInfo.Begin();
  }
  else
  {
    wordBreakInfoBuffer = wordBreakInfo.Begin();
  }

  // Retrieve the word break info.
  TextAbstraction::Segmentation::Get().GetWordBreakPositions( text.Begin() + startIndex,
                                                              numberOfCharacters,
                                                              wordBreakInfoBuffer );

  // If the word break info is updated, it needs to be inserted in the model.
  if( updateCurrentBuffer )
  {
    wordBreakInfo.Insert( wordBreakInfo.Begin() + startIndex,
                          newWordBreakInfo.Begin(),
                          newWordBreakInfo.End() );
    wordBreakInfo.Resize( totalNumberOfCharacters );
  }

#ifdef DEBUG_ENABLED
  if( gLogFilter->IsEnabledFor(Debug::Verbose) )
  {
    std::string utf8;
    Utf32ToUtf8( text.Begin(), totalNumberOfCharacters, utf8 );

    std::string info;
    info.reserve( totalNumberOfCharacters );
    for( unsigned int i=0; i<wordBreakInfo.Count(); ++i )
    {
      info.push_back( static_cast<char>('0' + wordBreakInfo[i]) );
    }

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SetWordBreakInfo Characters: %s\n", utf8.c_str() );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "SetWordBreakInfo Break info: %s\n", info.c_str() );
  }
#endif
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
