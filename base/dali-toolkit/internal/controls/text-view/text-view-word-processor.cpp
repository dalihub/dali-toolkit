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
#include <dali-toolkit/internal/controls/text-view/text-view-word-processor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-view-processor-helper-functions.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-dbg.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

namespace
{

const std::string EMOJI_FONT_NAME( "SamsungEmoji" ); // Emoticons font family name.

/**
 * Updates the word size and ascender.
 *
 * It's called after deleting some characters.
 *
 * @param[in] wordLayout The word layout info.
 */
void UpdateLayoutInfo( WordLayoutInfo& wordLayout )
{
  // Initialize layout info for the whole word.
  wordLayout.mSize = Size();
  wordLayout.mAscender = 0.f;

  // Traverse the character layout info to update the word layout.
  for( CharacterLayoutInfoContainer::iterator layoutIt = wordLayout.mCharactersLayoutInfo.begin(), layoutEndIt = wordLayout.mCharactersLayoutInfo.end();
       layoutIt != layoutEndIt;
       ++layoutIt )
  {
    // Layout info for the current character.
    CharacterLayoutInfo& layoutInfo( *layoutIt );

    // Update layout info for the current word.
    UpdateSize( wordLayout.mSize, layoutInfo.mSize );
    wordLayout.mAscender = std::max( wordLayout.mAscender, layoutInfo.mAscender );
  }
}

} // namespace

/////////////////////
// Layout info.
/////////////////////

WordLayoutInfo::WordLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mType( NoSeparator ),
  mCharactersLayoutInfo()
{
}

WordLayoutInfo::WordLayoutInfo( const WordLayoutInfo& word )
: mSize( word.mSize ),
  mAscender( word.mAscender ),
  mType( word.mType ),
  mCharactersLayoutInfo( word.mCharactersLayoutInfo )
{
}

WordLayoutInfo& WordLayoutInfo::operator=( const WordLayoutInfo& word )
{
  mSize = word.mSize;
  mAscender = word.mAscender;
  mType = word.mType;
  mCharactersLayoutInfo = word.mCharactersLayoutInfo;

  return *this;
}

void CreateWordTextInfo( const MarkupProcessor::StyledTextArray& word,
                         TextViewProcessor::WordLayoutInfo& wordLayoutInfo )
{
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "-->TextViewProcessor::CreateWordTextInfo\n" );
  // Split in characters.
  for( MarkupProcessor::StyledTextArray::const_iterator charIt = word.begin(), charEndIt = word.end(); charIt != charEndIt; ++charIt )
  {
    const MarkupProcessor::StyledText& styledText( *charIt );

    const std::size_t length = styledText.mText.GetLength();

    // It could be a group of characters.
    for( std::size_t index = 0; index < length; ++index )
    {
      MarkupProcessor::StyledText styledCharacter;
      styledCharacter.mStyle = styledText.mStyle;
      Character character = styledText.mText[index];
      styledCharacter.mText.Append( character );

      // Create layout character info.
      CharacterLayoutInfo characterLayoutInfo;

      characterLayoutInfo.mIsColorGlyph = GlyphImage::IsColorGlyph( character );
      DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "  Is color glyph: %s\n", ( characterLayoutInfo.mIsColorGlyph ? "True" : "False" ) );

      if( characterLayoutInfo.mIsColorGlyph )
      {
        styledCharacter.mStyle.SetFontName( EMOJI_FONT_NAME );
      }
      else
      {
        //Choose the right font for the given character and style.
        ChooseFontFamilyName( styledCharacter );
      }

      const Font font = Font::New( FontParameters( styledCharacter.mStyle.GetFontName(), styledCharacter.mStyle.GetFontStyle(), styledCharacter.mStyle.GetFontPointSize() ) );
      const Font::Metrics metrics = font.GetMetrics( character );
      const float ascender = font.GetAscender();

      // Fill Natural size info for current character.
      characterLayoutInfo.mHeight = font.GetLineHeight();
      characterLayoutInfo.mAdvance = metrics.GetAdvance();
      characterLayoutInfo.mBearing = metrics.GetBearing();

      if( character.IsNewLine() && !characterLayoutInfo.mIsColorGlyph )
      {
        // A new line character doesn't have any width.
        characterLayoutInfo.mSize.width = 0.f;
      }
      else
      {
        // Uses advance as width.
        characterLayoutInfo.mSize.width = characterLayoutInfo.mAdvance;
      }
      characterLayoutInfo.mSize.height = characterLayoutInfo.mHeight;
      characterLayoutInfo.mAscender = ascender;

      if( styledCharacter.mStyle.IsUnderlineEnabled() )
      {
        characterLayoutInfo.mUnderlineThickness = font.GetUnderlineThickness(); // Both thickness and position includes the
        characterLayoutInfo.mUnderlinePosition = font.GetUnderlinePosition();   // vertical pad adjust used in effects like glow or shadow.
      }

      // stores the styled text.
      characterLayoutInfo.mStyledText.mText = styledCharacter.mText;
      characterLayoutInfo.mStyledText.mStyle = styledCharacter.mStyle;

      // Add character layout info to the word layout info and update it.
      wordLayoutInfo.mCharactersLayoutInfo.push_back( characterLayoutInfo );
      UpdateSize( wordLayoutInfo.mSize, characterLayoutInfo.mSize );
      wordLayoutInfo.mAscender = std::max( wordLayoutInfo.mAscender, characterLayoutInfo.mAscender );
      wordLayoutInfo.mType = GetTextSeparatorType( character );
    } // end of each character in the group of characters.
  } // end of characters in the word.
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "<--TextViewProcessor::CreateWordTextInfo\n" );
}

void RemoveCharactersFromWordInfo( TextView::RelayoutData& relayoutData,
                                   const std::size_t numberOfCharacters,
                                   bool& mergeWords,
                                   bool& mergeLines,
                                   TextViewProcessor::TextInfoIndices& textInfoIndicesBegin,
                                   TextViewProcessor::TextInfoIndices& textInfoIndicesEnd,
                                   TextViewProcessor::TextInfoIndices& textInfoMergeIndicesBegin,
                                   TextViewProcessor::TextInfoIndices& textInfoMergeIndicesEnd,
                                   TextViewProcessor::WordGroupLayoutInfo& groupLayout,
                                   std::vector<TextActor>& removedTextActors )
{
  const TextViewProcessor::TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  // Get the word.
  WordLayoutInfo& wordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

  if( TextViewProcessor::LineSeparator == wordLayout.mType )
  {
    // If the word is a line separator and there is more lines, then current line and the line after need to be merged.
    if( textInfoIndicesBegin.mLineIndex + 1 < textLayoutInfo.mLinesLayoutInfo.size() )
    {
      // current line is not the last one.

      // Update indices to merge lines.
      textInfoMergeIndicesBegin.mLineIndex = textInfoIndicesBegin.mLineIndex;
      textInfoMergeIndicesEnd.mLineIndex = textInfoIndicesBegin.mLineIndex + 1;

      mergeLines = true;

      ++textInfoIndicesBegin.mLineIndex; // increase both indices,
      textInfoIndicesEnd.mLineIndex +=2; // will delete last line.
    }

    ++textInfoIndicesEnd.mWordIndex; //will delete the line separator;
  }
  else if( WordSeparator == wordLayout.mType )
  {
    // If the word is a word separator. Check if the word before and the word after can be merged.

    if( ( 0 < textInfoIndicesBegin.mWordIndex ) && ( groupLayout.mWordsLayoutInfo.size() > textInfoIndicesBegin.mWordIndex + 1 ) )
    {
      const WordLayoutInfo& wordLayoutBefore( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex - 1 ) );
      const WordLayoutInfo& wordLayoutAfter( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex + 1 ) );

      if( ( NoSeparator == wordLayoutBefore.mType ) && ( NoSeparator == wordLayoutAfter.mType ) )
      {
        // This word is a word separator (white space) and is not the first word of the group nor the last one.
        mergeWords = true;

        // Set indices to merge the words.
        textInfoMergeIndicesBegin.mWordIndex = textInfoIndicesBegin.mWordIndex - 1; // word before word separator.
        textInfoMergeIndicesEnd.mWordIndex = textInfoIndicesBegin.mWordIndex + 1; // word after word separator.

        textInfoIndicesEnd.mWordIndex += 2; // will delete the word separator and the merged word.
      }
      else
      {
        ++textInfoIndicesEnd.mWordIndex; // will delete the word separator;
      }
    }
    else
    {
      ++textInfoIndicesEnd.mWordIndex; // will delete the word separator;
    }
  }
  else if( numberOfCharacters == wordLayout.mCharactersLayoutInfo.size() )
  {
    // The whole word needs to be removed.
    ++textInfoIndicesEnd.mWordIndex; // will delete the current word.
  }
  else
  {
    // Store text-actors before removing them.
    CollectTextActors( removedTextActors, wordLayout, textInfoIndicesBegin.mCharacterIndex, textInfoIndicesBegin.mCharacterIndex + numberOfCharacters );

    // just remove some characters from current word.
    RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                              numberOfCharacters,
                              wordLayout );
  }
}

void RemoveCharactersFromWord( const std::size_t position,
                               const std::size_t numberOfCharacters,
                               WordLayoutInfo& wordLayout )
{
  // Removes a given number of characters from the given word starting from the 'position' index.

  // Early return.
  if( 0 == numberOfCharacters )
  {
    // nothing to do if the number of characters is zero.

    return;
  }

  // Remove characters from layout and text-actor info.
  wordLayout.mCharactersLayoutInfo.erase( wordLayout.mCharactersLayoutInfo.begin() + position, wordLayout.mCharactersLayoutInfo.begin() + position + numberOfCharacters );

  // Some characters have been removed from the word. Update the layout info is needed.
  UpdateLayoutInfo( wordLayout );
}

void SplitWord( const std::size_t position,
                WordLayoutInfo& firstWordLayoutInfo,
                WordLayoutInfo& lastWordLayoutInfo )
{
  // Splits a word in two.
  // It moves characters from the first part of the word to the last one.

  // early returns
  if( 0 == position )
  {
    // the whole word goes to the last part of the word.
    lastWordLayoutInfo = firstWordLayoutInfo;

    firstWordLayoutInfo = WordLayoutInfo();

    return;
  }

  if( position == firstWordLayoutInfo.mCharactersLayoutInfo.size() )
  {
    // the whole word goes to the first part of the word.

    // Just delete whatever there is in the last part of the word.
    lastWordLayoutInfo = WordLayoutInfo();

    return;
  }

  // Initialize output data structures.

  // Layout info
  lastWordLayoutInfo = WordLayoutInfo();

  // Split layout info.

  // Insert characters from the given index 'position' to the end.
  lastWordLayoutInfo.mCharactersLayoutInfo.insert( lastWordLayoutInfo.mCharactersLayoutInfo.end(),
                                                   firstWordLayoutInfo.mCharactersLayoutInfo.begin() + position, firstWordLayoutInfo.mCharactersLayoutInfo.end() );

  // Delete characters from the first part of the word.
  firstWordLayoutInfo.mCharactersLayoutInfo.erase( firstWordLayoutInfo.mCharactersLayoutInfo.begin() + position, firstWordLayoutInfo.mCharactersLayoutInfo.end() );

  // Update the layout info of both new words.
  UpdateLayoutInfo( firstWordLayoutInfo );
  UpdateLayoutInfo( lastWordLayoutInfo );
}

void MergeWord( WordLayoutInfo& firstWordLayoutInfo,
                const WordLayoutInfo& lastWordLayoutInfo )
{
  // Merges two given words.

  // Early returns.
  if( lastWordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    // nothing to do
    return;
  }

  if( firstWordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    // copy last to first

    firstWordLayoutInfo = lastWordLayoutInfo;

    return;
  }

  if( ( NoSeparator != firstWordLayoutInfo.mType ) || ( NoSeparator != lastWordLayoutInfo.mType ) )
  {
    // Do not merge white spaces or new line characters.
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words." );
  }

  // Merge layout info
  firstWordLayoutInfo.mCharactersLayoutInfo.insert( firstWordLayoutInfo.mCharactersLayoutInfo.end(),
                                                    lastWordLayoutInfo.mCharactersLayoutInfo.begin(),
                                                    lastWordLayoutInfo.mCharactersLayoutInfo.end() );

  // Update the word layout info.
  UpdateSize( firstWordLayoutInfo.mSize, lastWordLayoutInfo.mSize );
  firstWordLayoutInfo.mAscender = std::max( firstWordLayoutInfo.mAscender, lastWordLayoutInfo.mAscender );
}

CharacterLayoutInfo GetFirstCharacterLayoutInfo( const WordLayoutInfo& wordLayoutInfo )
{
  CharacterLayoutInfo layoutInfo;

  if( !wordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    layoutInfo = *wordLayoutInfo.mCharactersLayoutInfo.begin();
  }

  return layoutInfo;
}

CharacterLayoutInfo GetLastCharacterLayoutInfo( const WordLayoutInfo& wordLayoutInfo )
{
  CharacterLayoutInfo layoutInfo;

  if( !wordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    layoutInfo = *( wordLayoutInfo.mCharactersLayoutInfo.end() - 1 );
  }

  return layoutInfo;
}

void CollectTextActors( std::vector<TextActor>& textActors, const WordLayoutInfo& word, const std::size_t characterIndexBegin, const std::size_t characterIndexEnd )
{
  for( CharacterLayoutInfoContainer::const_iterator characterIt = word.mCharactersLayoutInfo.begin() + characterIndexBegin,
         characterEndIt = word.mCharactersLayoutInfo.begin() + characterIndexEnd;
       characterIt != characterEndIt;
       ++characterIt )
  {
    const CharacterLayoutInfo& characterLayout( *characterIt );

    if( !characterLayout.mIsColorGlyph )
    {
      TextActor textActor = TextActor::DownCast( characterLayout.mGlyphActor );
      if( textActor )
      {
        textActors.push_back( textActor );
      }
    }
  }
}

void CollectTextActorsFromWords( std::vector<TextActor>& textActors, const WordGroupLayoutInfo& group, const std::size_t wordIndexBegin, const std::size_t wordIndexEnd )
{
  for( WordLayoutInfoContainer::const_iterator wordIt = group.mWordsLayoutInfo.begin() + wordIndexBegin, wordEndIt = group.mWordsLayoutInfo.begin() + wordIndexEnd;
       wordIt != wordEndIt;
       ++wordIt )
  {
    const WordLayoutInfo& word( *wordIt );

    for( CharacterLayoutInfoContainer::const_iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
         characterIt != characterEndIt;
         ++characterIt )
    {
      const CharacterLayoutInfo& characterLayout( *characterIt );

      if( !characterLayout.mIsColorGlyph )
      {
        TextActor textActor = TextActor::DownCast( characterLayout.mGlyphActor );
        if( textActor )
        {
          textActors.push_back( textActor );
        }
      }
    }
  }
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
