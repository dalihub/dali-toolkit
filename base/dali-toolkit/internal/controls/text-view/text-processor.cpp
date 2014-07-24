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

// EXTERNAL INCLUDES
#include <fribidi/fribidi.h>

// INTERNAL INCLUDES

#include <dali-toolkit/public-api/markup-processor/markup-processor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextProcessor
{

void SplitInLines( const MarkupProcessor::StyledTextArray& text,
                   std::vector<MarkupProcessor::StyledTextArray>& lines )
{
  MarkupProcessor::StyledTextArray line;
  for( MarkupProcessor::StyledTextArray::const_iterator it = text.begin(), endIt = text.end(); it != endIt; ++it )
  {
    const MarkupProcessor::StyledText& styledText( *it );

    for( size_t i = 0, length = styledText.mText.GetLength(); i < length; ++i )
    {
      const Dali::Character character = styledText.mText[i];

      if( character.IsNewLine() ) // LF
      {
        Text newText( character );
        MarkupProcessor::StyledText newStyledText( newText, styledText.mStyle );
        line.push_back( newStyledText );

        lines.push_back( line );
        line.clear();
      }
      else
      {
        Text newText( character );
        MarkupProcessor::StyledText newStyledText( newText, styledText.mStyle );
        line.push_back( newStyledText );
      }
    }
  }

  // This line could be empty if the last character of the previous line is a 'new line' character
  // and is the last of the text.
  lines.push_back( line );
}

void SplitInWords( const MarkupProcessor::StyledTextArray& line,
                   std::vector<MarkupProcessor::StyledTextArray>& words )
{
  MarkupProcessor::StyledTextArray word;
  for( MarkupProcessor::StyledTextArray::const_iterator it = line.begin(), endIt = line.end(); it != endIt; ++it )
  {
    const MarkupProcessor::StyledText& styledText( *it );
    const Dali::Character character = styledText.mText[0];

    if( character.IsWhiteSpace() )
    {
      // When a separator is found, the previous word is added to the list,
      // then a new word is initialized and the separator is also added as a word.
      if( !word.empty() )
      {
        words.push_back( word );
        word.clear(); // initializes a new word.
      }

      // Separator added as a word.
      MarkupProcessor::StyledText separatorChar;
      separatorChar.mText.Append( character );
      separatorChar.mStyle = styledText.mStyle;

      MarkupProcessor::StyledTextArray separatorWord;
      separatorWord.push_back( separatorChar );

      words.push_back( separatorWord );
    }
    else
    {
      MarkupProcessor::StyledText styledChar;
      styledChar.mStyle = styledText.mStyle;
      styledChar.mText.Append( character );

      // Add the character to the current word.
      word.push_back( styledChar );
    }
  }

  //Finally the last word need to be added.
  if( !word.empty() )
  {
    words.push_back( word );
  }
}

bool BeginsRightToLeftCharacter( const MarkupProcessor::StyledTextArray& styledText )
{
  for( MarkupProcessor::StyledTextArray::const_iterator it = styledText.begin(), endIt = styledText.end(); it != endIt; ++it )
  {
    const Text& text( (*it).mText );

    for( size_t i = 0, length = text.GetLength(); i < length; ++i )
    {
      Character::CharacterDirection direction = text[i].GetCharacterDirection();
      if( direction != Character::Neutral )
      {
        return ( direction == Character::RightToLeft || direction == Character::RightToLeftWeak );
      }
    }
  }

  return false;
}

bool BeginsRightToLeftCharacter( const Text& text )
{
  for( size_t i = 0, length = text.GetLength(); i < length; ++i )
  {
    Character::CharacterDirection direction = text[i].GetCharacterDirection();
    if( direction != Character::Neutral )
    {
      return ( direction == Character::RightToLeft || direction == Character::RightToLeftWeak );
    }
  }

  return false;
}

bool ContainsRightToLeftCharacter( const MarkupProcessor::StyledTextArray& styledText )
{
  for( MarkupProcessor::StyledTextArray::const_iterator it = styledText.begin(), endIt = styledText.end(); it != endIt; ++it )
  {
    const Text& text( (*it).mText );

    for( size_t i = 0, length = text.GetLength(); i < length; ++i )
    {
      Character::CharacterDirection direction = text[i].GetCharacterDirection();
      if( ( Character::RightToLeft == direction ) || ( Character::RightToLeftWeak == direction ) )
      {
        return true;
      }
    }
  }

  return false;
}

bool ContainsRightToLeftCharacter( const Dali::Text& text )
{
  for( size_t i = 0, length = text.GetLength(); i < length; ++i )
  {
    Character::CharacterDirection direction = ( text[i] ).GetCharacterDirection();
    if( ( Character::RightToLeft == direction ) || ( Character::RightToLeftWeak == direction ) )
    {
      return true;
    }
  }

  return false;
}

void ConvertBidirectionalText( const MarkupProcessor::StyledTextArray& line,
                               std::vector<MarkupProcessor::StyledTextArray>& convertedText,
                               std::vector<int>& logicalToVisualMap,
                               std::vector<int>& visualToLogicalMap )
{
  // Clean vectors first. This function doesn't use any previous value.
  logicalToVisualMap.clear();
  visualToLogicalMap.clear();
  convertedText.clear();

  if( line.empty() )
  {
    // nothing to do if the line is empty.
    return;
  }

  // Get the plain text from the line to be reordered by the BiDirectional algorithm.
  std::string textToBeConverted;
  GetPlainString( line, textToBeConverted );

  const std::size_t stringSize = textToBeConverted.size();

  std::vector<FriBidiChar> logicalStrBuffer;
  std::vector<FriBidiChar> visualStrBuffer;
  // unicode length <= UTF-8 length in bytes (reserve one extra for terminator)
  // pad these buffers with 0's, as it's unclear what fribidi_log2vis does w.r.t.
  // the length of it's output content (appears the same as input content, and does
  // not seem to generate bidi marks i.e. FRIBIDI_CHAR_LRM/FRIBIDI_CHAR_RLM)
  logicalStrBuffer.resize( stringSize+1, 0 );
  visualStrBuffer.resize( stringSize+1, 0 );
  FriBidiChar *logicalStr( &logicalStrBuffer[0] );
  FriBidiChar *visualStr( &visualStrBuffer[0] );

  // Convert UTF-8 string to unicode string
  const std::size_t length = fribidi_charset_to_unicode( FRIBIDI_CHAR_SET_UTF8, textToBeConverted.c_str(), stringSize, logicalStr );

  if( 0 == length )
  {
    DALI_ASSERT_DEBUG( !"TextProcessor::ConvertBidirectionalText. Error when calling at fribidi_charset_to_unicode" );

    return;
  }

  logicalToVisualMap.resize( length );
  visualToLogicalMap.resize( length );

  // Convert and reorder the string as specified by the Unicode Bidirectional Algorithm
  FriBidiCharType baseDirection = FRIBIDI_TYPE_ON;
  fribidi_boolean log2vis = fribidi_log2vis( logicalStr, length, &baseDirection, visualStr, &logicalToVisualMap[0], &visualToLogicalMap[0], NULL );

  if(log2vis)
  {
    // Convert the unicode string back to the UTF-8 string
    std::vector<char> bidiTextConverted;

    bidiTextConverted.resize( length * 4 + 1 ); // Maximum bytes to represent one UTF-8 character is 6.
                                                // Currently Dali doesn't support this UTF-8 extension. Dali only supports 'regular' UTF-8 which has a maximum of 4 bytes per character.

    fribidi_unicode_to_charset( FRIBIDI_CHAR_SET_UTF8, visualStr, length, &bidiTextConverted[0] );

    textToBeConverted = &bidiTextConverted[0];

    // After reorder the text, rebuild the text with the original styles is needed.
    // To assign the original style is needed to use the characterLogicalToVisualMap table.
    Text text( &bidiTextConverted[0] );

    // Split the line in groups of words.
    // Words are grouped if they can be displayed left to right or right to left.
    // Add the correct styles for the characters after they are reordered.

    MarkupProcessor::StyledTextArray groupOfWords;

    Character::CharacterDirection previousDirection = ( BeginsRightToLeftCharacter( line ) ? Character::RightToLeft : Character::LeftToRight );
    for( size_t i = 0; i < length; ++i )
    {
      const Character character( text[i] );

      Character::CharacterDirection currentDirection = character.GetCharacterDirection();
      if( Character::Neutral == currentDirection )
      {
        currentDirection = previousDirection;
      }

      MarkupProcessor::StyledText styledText;
      styledText.mText.Append( character );
      styledText.mStyle = line[visualToLogicalMap[i]].mStyle;

      if( currentDirection != previousDirection )
      {
        if( !groupOfWords.empty() )
        {
          convertedText.push_back( groupOfWords );
          groupOfWords.clear();
        }
      }

      groupOfWords.push_back( styledText );

      previousDirection = currentDirection;
    }

    if( !groupOfWords.empty() )
    {
      convertedText.push_back( groupOfWords );
    }
  }
}

bool IsWhiteSpace( const MarkupProcessor::StyledTextArray& text, size_t offset )
{
  DALI_ASSERT_DEBUG( offset < text.size() );

  // assume 1 Character per StyledText
  return text[offset].mText[0].IsWhiteSpace();
}

void FindNearestWord( const MarkupProcessor::StyledTextArray& text, size_t offset, size_t& start, size_t& end)
{
  const size_t size(text.size());
  offset = std::min(offset, size-1);

  size_t i(offset);
  size_t j(offset);

  // if currently looking at whitespace, then search left and right for non-whitespace.
  if(IsWhiteSpace(text, offset))
  {
    // scan left until non-white space / beginning of string.
    while(i > 0 && IsWhiteSpace(text, i))
    {
      i--;
    }

    // scan right until non-white space / end of string.
    while(j < size && IsWhiteSpace(text, j))
    {
      j++;
    }
  }

  // check if r.h.s. word is closer than l.h.s. word
  if( (j - offset) < // distance to closest right word <
      (offset - i) ) // distance to closest left word
  {
    // point left and right markers on start of right word
    i = j;
  }
  else
  {
    // point left and right markers on end of left word
    j = i;
  }

  // expand left and right markers to encompase entire word
  while(i > 0 && !IsWhiteSpace(text, i-1))
  {
    i--;
  }

  while(j < size && !IsWhiteSpace(text, j))
  {
    j++;
  }

  start = i;
  end = j;
}

} // namespace TextProcessor

} // namespace Internal

} // namespace DaliToolkit

} // namespace Dali
