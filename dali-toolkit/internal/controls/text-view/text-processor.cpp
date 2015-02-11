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
#include <dali-toolkit/internal/controls/text-view/text-processor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextProcessor
{

void SplitInParagraphs( const MarkupProcessor::StyledTextArray& styledTextArray,
                        std::vector<Text>& paragraphs,
                        std::vector< Vector<TextStyle*> >& styles )
{
  // Stores the text for a paragraph.
  Text paragraph;

  // Stores the styles for each character of the paragraph.
  Vector<TextStyle*> stylesForParagraph;

  // Traverses all styled texts of the array.
  for( MarkupProcessor::StyledTextArray::const_iterator it = styledTextArray.begin(), endIt = styledTextArray.end(); it != endIt; ++it )
  {
    const MarkupProcessor::StyledText& styledText( *it );

    // Traverses all the characters of the styled text (It may have more than one).
    for( size_t i = 0u, length = styledText.mText.GetLength(); i < length; ++i )
    {
      const Dali::Character character = styledText.mText[i];

      if( character.IsNewLine() ) // LF
      {
        // The character is a new paragraph character.

        // Append the new paragraph character.
        paragraph.Append( character );

        // Creates a new text style for the character and insert it to the vector of styles for that paragraph.
        TextStyle* style = new TextStyle( styledText.mStyle );
        stylesForParagraph.PushBack( style );

        // Inserts the paragraph and the styles to the vector of paragraphs and the vector of styles.
        paragraphs.push_back( paragraph );
        styles.push_back( stylesForParagraph );

        // Clears the text and the vector of styles for the next paragraph.
        paragraph = Text();
        stylesForParagraph.Clear();
      }
      else
      {
        // The character is not a new paragraph character.

        // Append it to the paragraph's text
        paragraph.Append( character );

        // Add the style to the vector of styles for that paragraph.
        TextStyle* style = new TextStyle( styledText.mStyle );
        stylesForParagraph.PushBack( style );
      }
    }
  }

  // This paragraph could be empty if the last character of the previous paragraph is a 'new paragraph' character
  // and is the last of the text.
  paragraphs.push_back( paragraph );
  styles.push_back( stylesForParagraph );
}

void SplitInWords( const Dali::Text& paragraph,
                   Vector<std::size_t>& positions )
{
  const std::size_t length = paragraph.GetLength();

  // Magic number: Let's soupose there is ~6 characters per word. Used to do less memory reallocation inside Vector.
  const size_t magicNumberOfWords = ( length / 6u ) + 1u;

  // Find the positions of the new paragraph characters.
  positions.Reserve( magicNumberOfWords );

  // Find the position of all white spaces. A new paragraph character is also considered a white space but it doesn't matter at this point.
  paragraph.Find( Text::WHITE_SPACE, 0u, length - 1u, positions );
}

/**
 * Wheather the character of the text pointed by the given offset is a white space.
 *
 * @param[in] text The text.
 * @param[in] offset Offset pointing the character.
 *
 * @return \e true if the character pointed by the offset is a white space.
 */
bool IsWhiteSpace( const MarkupProcessor::StyledTextArray& text, size_t offset )
{
  DALI_ASSERT_DEBUG( offset < text.size() );

  // assume 1 Character per StyledText
  return ( *( text.begin() + offset ) ).mText[0u].IsWhiteSpace();
}

void FindNearestWord( const MarkupProcessor::StyledTextArray& text, size_t offset, size_t& start, size_t& end)
{
  const size_t size(text.size());
  offset = std::min(offset, size-1u);

  size_t i(offset);
  size_t j(offset);

  // if currently looking at whitespace, then search left and right for non-whitespace.
  if(IsWhiteSpace(text, offset))
  {
    // scan left until non-white space / beginning of string.
    while(i > 0u && IsWhiteSpace(text, i))
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
  while(i > 0u && !IsWhiteSpace(text, i-1u))
  {
    i--;
  }

  while(j < size && !IsWhiteSpace(text, j))
  {
    j++;
  }

  // If both markers at same position and is whitespace then word is a whitespace word
  if ( i == j )
  {
    while(j < size && IsWhiteSpace(text, j))
    {
      j++;
    }

    while(i > 0 && IsWhiteSpace(text, i-1))
    {
      i--;
    }
  }

  start = i;
  end = j;
}

} // namespace TextProcessor

} // namespace Internal

} // namespace DaliToolkit

} // namespace Dali
