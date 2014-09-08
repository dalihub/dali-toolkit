/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-view/text-view-processor-helper-functions.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-view-processor-dbg.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

void UpdateSize( Size& size1, const Size& size2, const SizeGrowType type )
{
  switch( type )
  {
    case GrowWidth:
    {
      size1.width += size2.width;
      size1.height = std::max( size1.height, size2.height );
      break;
    }
    case GrowHeight:
    {
      size1.width = std::max( size1.width, size2.width );
      size1.height += size2.height;
      break;
    }
  }
}

TextSeparatorType GetTextSeparatorType( const Character& character )
{
  // returns if the given character is a line separator '\n', a word separator ' ' or if is not a separator (any other character).
  return ( character.IsNewLine() ? LineSeparator : ( character.IsWhiteSpace() ? WordSeparator : NoSeparator ) );
}

void ChooseFontFamilyName( MarkupProcessor::StyledText& text )
{
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "-->TextViewProcessor::ChooseFontFamilyName\n" );
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "   input font name: [%s]\n", text.mStyle.GetFontName().c_str() );

  bool userDefinedFontFamilyName = false;

  // First check if there is a font defined in the style and it supports the given text.
  if( !text.mStyle.GetFontName().empty() )
  {
    const FontParameters fontParams( text.mStyle.GetFontName(), text.mStyle.GetFontStyle() , text.mStyle.GetFontPointSize() );
    const Font font = Font::New( fontParams );

    if( !font.IsDefaultSystemFont() && font.AllGlyphsSupported( text.mText ) )
    {
      userDefinedFontFamilyName = true;
    }
  }

  if( !userDefinedFontFamilyName )
  {
    const Font defaultSystemFont = Font::New();

    // At this point no font is set or doesn't support the given text.
    if( !defaultSystemFont.AllGlyphsSupported( text.mText ) )
    {
      // If the default system font doesn't support the given text,
      // an appropiate font is selected.
      text.mStyle.SetFontName( Font::GetFamilyForText( text.mText ) );
      // @TODO Font::GetFamilyForText() should return font family and font style.
    }
    else
    {
      // All characters are supported with default font, so use it
      text.mStyle.SetFontName( "" );
    }
  }
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "  output font name: [%s]\n", text.mStyle.GetFontName().c_str() );
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "<--TextViewProcessor::ChooseFontFamilyName\n" );
}

void GetIndicesFromGlobalCharacterIndex( const std::size_t index,
                                         const TextLayoutInfo& textLayoutInfo,
                                         TextInfoIndices& indices )
{
  // TODO : Check for mixed LTR and RTL.

  // clear all indices
  indices = TextInfoIndices();

  // Early return.
  if( textLayoutInfo.mLinesLayoutInfo.empty() )
  {
    // Text is empty. All indices are 0.
    return;
  }

  std::size_t currentIndex = 0; // stores how many characters have been traversed.

  // Traverse all lines, groups of words and words until global index is found.
  bool found = false;
  for( LineLayoutInfoContainer::const_iterator lineIt = textLayoutInfo.mLinesLayoutInfo.begin(),
         lineEndIt = textLayoutInfo.mLinesLayoutInfo.end();
       ( !found ) && ( lineIt != lineEndIt );
       ++lineIt, ++indices.mLineIndex )
  {
    const LineLayoutInfo& lineLayoutInfo( *lineIt );

    if( currentIndex + lineLayoutInfo.mNumberOfCharacters > index )
    {
      // The character is in this line
      for( WordGroupLayoutInfoContainer::const_iterator groupIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin(),
             groupEndIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
           ( !found ) && ( groupIt != groupEndIt );
           ++groupIt, ++indices.mGroupIndex )
      {
        const WordGroupLayoutInfo& wordGroupLayoutInfo( *groupIt );

        if( currentIndex + wordGroupLayoutInfo.mNumberOfCharacters > index )
        {
          // The character is in this group of words.
          for( WordLayoutInfoContainer::const_iterator wordIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin(),
                 wordEndIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
               ( !found ) && ( wordIt != wordEndIt );
               ++wordIt, ++indices.mWordIndex )
          {
            const WordLayoutInfo& wordLayoutInfo( *wordIt );

            if( currentIndex + wordLayoutInfo.mCharactersLayoutInfo.size() > index )
            {
              // The character is in this word
              indices.mCharacterIndex = index - currentIndex;
              found = true;
            }
            else
            {
              // check in the next word.
              currentIndex += wordLayoutInfo.mCharactersLayoutInfo.size();
            }
          } // end words.
          if( !wordGroupLayoutInfo.mWordsLayoutInfo.empty() )
          {
            --indices.mWordIndex;
          }
        }
        else
        {
          // check in the next group of words
          currentIndex += wordGroupLayoutInfo.mNumberOfCharacters;
        }
      } // end groups of words.
      if( !lineLayoutInfo.mWordGroupsLayoutInfo.empty() )
      {
        --indices.mGroupIndex;
      }
    }
    else
    {
      // check in the next line
      currentIndex += lineLayoutInfo.mNumberOfCharacters;
    }
  } // end lines.

  // Need to decrease indices as they have been increased in the last loop.
  if( !textLayoutInfo.mLinesLayoutInfo.empty() )
  {
    --indices.mLineIndex;
  }
}

void ClearText( std::vector<TextActor>& textActors )
{
    for( std::vector<TextActor>::iterator it = textActors.begin(), endIt = textActors.end(); it != endIt; ++it )
    {
      (*it).SetText( std::string( "" ) );
    }
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
