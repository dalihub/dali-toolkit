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

} // namespace

/////////////////////
// Layout info.
/////////////////////

WordLayoutInfo::WordLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mType( NoSeparator ),
  mFirstCharacter( 0u ),
  mCharactersLayoutInfo()
{
}

WordLayoutInfo::~WordLayoutInfo()
{
}

WordLayoutInfo::WordLayoutInfo( const WordLayoutInfo& word )
: mSize( word.mSize ),
  mAscender( word.mAscender ),
  mType( word.mType ),
  mFirstCharacter( word.mFirstCharacter ),
  mCharactersLayoutInfo( word.mCharactersLayoutInfo )
{
}

WordLayoutInfo& WordLayoutInfo::operator=( const WordLayoutInfo& word )
{
  mSize = word.mSize;
  mAscender = word.mAscender;
  mType = word.mType;
  mFirstCharacter = word.mFirstCharacter;
  mCharactersLayoutInfo = word.mCharactersLayoutInfo;

  return *this;
}

void CreateWordTextInfo( const Text& paragraph,
                         Vector<TextStyle*>& textStyles,
                         WordLayoutInfo& wordLayoutInfo )
{
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "-->TextViewProcessor::CreateWordTextInfo\n" );
  // Split in characters.
  std::size_t characterIndex = wordLayoutInfo.mFirstCharacter;
  for( CharacterLayoutInfoContainer::iterator it = wordLayoutInfo.mCharactersLayoutInfo.begin(),
         endIt =  wordLayoutInfo.mCharactersLayoutInfo.end();
       it != endIt;
       ++it, ++characterIndex )
  {
    // Gets a reference of the character's layout info.
    CharacterLayoutInfo& characterLayoutInfo( *it );

    // Gets the character and the style for that character from the paragraph.
    Character character = paragraph[characterIndex];
    TextStyle* textStyle = *( textStyles.Begin() + characterIndex );

    // Checks whether the character is an emoticon.
    characterLayoutInfo.mIsColorGlyph = GlyphImage::IsColorGlyph( character );
    DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "  Is color glyph: %s\n", ( characterLayoutInfo.mIsColorGlyph ? "True" : "False" ) );

    if( characterLayoutInfo.mIsColorGlyph )
    {
      // If the character is an emoticon a predefined font is set.
      textStyle->SetFontName( EMOJI_FONT_NAME );
    }
    else
    {
      // Checks if the font family and the font style set in the text style supports the character.
      // If not, it chooses the right font for the given character and style.
      ChooseFontFamilyName( character, *textStyle );
    }

    // Checks whether the charcter is right to left.
    const Character::CharacterDirection direction = character.GetCharacterDirection();
    characterLayoutInfo.mIsRightToLeft = ( ( direction == Character::RightToLeft ) ||
                                           ( direction == Character::RightToLeftWeak ) );

    // Gets the metrics of the font.
    const Font font = Font::New( FontParameters( textStyle->GetFontName(), textStyle->GetFontStyle(), textStyle->GetFontPointSize() ) );
    const Font::Metrics metrics = font.GetMetrics( character );
    const float ascender = font.GetAscender();

    // Fill Natural size info for current character.

    // The font line's height is used as character's height.
    characterLayoutInfo.mSize.height = font.GetLineHeight();

    // The character's advance is used as charcter's width.
    characterLayoutInfo.mSize.width = metrics.GetAdvance();

    // The ascender and bearing are used to position correctly glyphs of different font sizes.
    characterLayoutInfo.mAscender = ascender;
    characterLayoutInfo.mBearing = metrics.GetBearing();

    if( character.IsNewLine() && !characterLayoutInfo.mIsColorGlyph )
    {
      // A new paragraph character '\n'  doesn't have any width.
      characterLayoutInfo.mSize.width = 0.f;
    }

    // Set's the underline thickness and position.
    // Both thickness and position includes the vertical pad adjust used in effects like glow or shadow.
    if( textStyle->IsUnderlineEnabled() )
    {
      characterLayoutInfo.mUnderlineThickness = font.GetUnderlineThickness();
      characterLayoutInfo.mUnderlinePosition = font.GetUnderlinePosition();
    }

    // Updates the word size and ascender.
    UpdateSize( wordLayoutInfo.mSize, characterLayoutInfo.mSize );
    wordLayoutInfo.mAscender = std::max( wordLayoutInfo.mAscender, characterLayoutInfo.mAscender );
  } // end of characters in the word.
  DALI_LOG_INFO( gTextViewProcessorLogFilter, Debug::General, "<--TextViewProcessor::CreateWordTextInfo\n" );
}

void UpdateLayoutInfo( WordLayoutInfo& wordLayout )
{
  // Initialize layout info for the whole word.
  wordLayout.mSize = Size::ZERO;
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

void RemoveCharactersFromWordInfo( TextView::RelayoutData& relayoutData,
                                   const std::size_t numberOfCharacters,
                                   bool& mergeWords,
                                   bool& mergeParagraphs,
                                   TextInfoIndices& textInfoIndicesBegin,
                                   TextInfoIndices& textInfoIndicesEnd,
                                   TextInfoIndices& textInfoMergeIndicesBegin,
                                   TextInfoIndices& textInfoMergeIndicesEnd,
                                   ParagraphLayoutInfo& paragraphLayout,
                                   std::vector<TextActor>& removedTextActors )
{
  const TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  // Get the word.
  WordLayoutInfo& wordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

  if( ParagraphSeparator == wordLayout.mType )
  {
    // If the word is a paragraph separator and there is more paragraphs, then current paragraph and the paragraph after need to be merged.
    if( textInfoIndicesBegin.mParagraphIndex + 1u < textLayoutInfo.mParagraphsLayoutInfo.size() )
    {
      // current paragraph is not the last one.

      // Update indices to merge paragraphs.
      textInfoMergeIndicesBegin.mParagraphIndex = textInfoIndicesBegin.mParagraphIndex;
      textInfoMergeIndicesEnd.mParagraphIndex = textInfoIndicesBegin.mParagraphIndex + 1u;

      mergeParagraphs = true;

      ++textInfoIndicesBegin.mParagraphIndex; // increase both indices,
      textInfoIndicesEnd.mParagraphIndex += 2u; // will delete last paragraph.
    }

    ++textInfoIndicesEnd.mWordIndex; //will delete the paragraph separator;
  }
  else if( WordSeparator == wordLayout.mType )
  {
    // If the word is a word separator. Check if the word before and the word after can be merged.

    if( ( 0u < textInfoIndicesBegin.mWordIndex ) && ( paragraphLayout.mWordsLayoutInfo.size() > textInfoIndicesBegin.mWordIndex + 1u ) )
    {
      const WordLayoutInfo& wordLayoutBefore( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex - 1u ) );
      const WordLayoutInfo& wordLayoutAfter( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex + 1u ) );

      if( ( NoSeparator == wordLayoutBefore.mType ) && ( NoSeparator == wordLayoutAfter.mType ) )
      {
        // This word is a word separator (white space) and is not the first word of the paragraph nor the last one.
        mergeWords = true;

        // Set indices to merge the words.
        textInfoMergeIndicesBegin.mWordIndex = textInfoIndicesBegin.mWordIndex - 1u; // word before word separator.
        textInfoMergeIndicesEnd.mWordIndex = textInfoIndicesBegin.mWordIndex + 1u; // word after word separator.

        textInfoIndicesEnd.mWordIndex += 2u; // will delete the word separator and the merged word.
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
  if( 0u == numberOfCharacters )
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
  if( 0u == position )
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
    // Do not merge white spaces or new paragraph characters.
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words." );
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
    layoutInfo = *( wordLayoutInfo.mCharactersLayoutInfo.end() - 1u );
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

void CollectTextActorsFromWords( std::vector<TextActor>& textActors, const ParagraphLayoutInfo& paragraph, const std::size_t wordIndexBegin, const std::size_t wordIndexEnd )
{
  for( WordLayoutInfoContainer::const_iterator wordIt = paragraph.mWordsLayoutInfo.begin() + wordIndexBegin, wordEndIt = paragraph.mWordsLayoutInfo.begin() + wordIndexEnd;
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
