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
#ifdef DEBUG_ENABLED
  if( gLogFilter->IsEnabledFor(Debug::Verbose) )
  {
    std::string utf8;
    Utf32ToUtf8( text.Begin(), numberOfCharacters, utf8 );

    std::string info;
    info.reserve( numberOfCharacters );
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
