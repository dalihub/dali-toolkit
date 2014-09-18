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
#include <dali-toolkit/internal/controls/text-view/text-view-paragraph-processor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-view-word-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-helper-functions.h>
#include <dali-toolkit/internal/controls/text-view/text-processor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

/////////////////////
// Layout info.
/////////////////////

ParagraphLayoutInfo::ParagraphLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mLineHeightOffset( 0.f ),
  mNumberOfCharacters( 0u ),
  mWordsLayoutInfo()
{
}

ParagraphLayoutInfo::~ParagraphLayoutInfo()
{
}

ParagraphLayoutInfo::ParagraphLayoutInfo( const ParagraphLayoutInfo& paragraph )
: mSize( paragraph.mSize ),
  mAscender( paragraph.mAscender ),
  mLineHeightOffset( paragraph.mLineHeightOffset ),
  mNumberOfCharacters( paragraph.mNumberOfCharacters ),
  mWordsLayoutInfo( paragraph.mWordsLayoutInfo )
{
}

ParagraphLayoutInfo& ParagraphLayoutInfo::operator=( const ParagraphLayoutInfo& paragraph )
{
  mSize = paragraph.mSize;
  mAscender = paragraph.mAscender;
  mLineHeightOffset = paragraph.mLineHeightOffset;
  mNumberOfCharacters = paragraph.mNumberOfCharacters;
  mWordsLayoutInfo = paragraph.mWordsLayoutInfo;

  return *this;
}

void CreateParagraphInfo( const MarkupProcessor::StyledTextArray& paragraph,
                          TextView::RelayoutData& relayoutData,
                          ParagraphLayoutInfo& paragraphLayoutInfo )
{
  // Split the paragraph in words.
  // TODO: Proper RTL support.
  MarkupProcessor::StyledTextArray convertedParagraph;
  if( TextProcessor::ContainsRightToLeftCharacter( paragraph ) )
  {
    // If the text is bidirectional, the characters will be converted and reordered
    // as specified by the Unicode Bidirectional Algorithm.

    // Reorders the paragraph and converts arabic glyphs (if any).
    TextProcessor::ConvertBidirectionalText( paragraph,
                                             convertedParagraph,
                                             relayoutData.mCharacterLogicalToVisualMap,
                                             relayoutData.mCharacterVisualToLogicalMap);
  }
  else
  {
    // No bidirectional text to process.
    convertedParagraph = paragraph;

    // Create trivial bidirectional map tables.
    std::size_t index = 0u;
    for( MarkupProcessor::StyledTextArray::const_iterator it = convertedParagraph.begin(), endIt = convertedParagraph.end(); it != endIt; ++it )
    {
      const MarkupProcessor::StyledText& styledText( *it );

      for( std::size_t i = 0u, length = styledText.mText.GetLength(); i < length; ++i )
      {
        relayoutData.mCharacterLogicalToVisualMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
        relayoutData.mCharacterVisualToLogicalMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
        ++index;
      }
    }
  }

  // Split the paragraph in words
  std::vector<MarkupProcessor::StyledTextArray> words;
  TextProcessor::SplitInWords( convertedParagraph, words );

  // if last word has a new paragraph separator, create a new word.
  if( !words.empty() )
  {
    MarkupProcessor::StyledTextArray& word( *( words.end() - 1u ) );
    if( word.size() > 1u )
    {
      // do nothing if the word has only one character.
      MarkupProcessor::StyledText& styledText( *( word.end() - 1u ) );
      if( !styledText.mText.IsEmpty() )
      {
        const std::size_t length = styledText.mText.GetLength();
        if( styledText.mText[length-1u].IsNewLine() )
        {
          // Last character of this word is a new paragraph character.

          // Remove paragraph separator character from current word.
          styledText.mText.Remove( length - 1u, 1u );

          // Create a new word with the paragraph separator character.
          MarkupProcessor::StyledText newParagraphText( Text( styledText.mText[length-1u] ), styledText.mStyle );

          MarkupProcessor::StyledTextArray newParagraphWord;
          newParagraphWord.push_back( newParagraphText );

          words.push_back( newParagraphWord );
        }
      }
    }
  }

  std::string lastCharacterFont; // Keeps the font used by the last character. It's used to set the font to a word separator.

  // Traverse all words.
  for( std::vector<MarkupProcessor::StyledTextArray>::const_iterator wordIt = words.begin(), wordEndIt = words.end(); wordIt != wordEndIt; ++wordIt )
  {
    const MarkupProcessor::StyledTextArray& word( *wordIt );

    // Data structures for the new word.
    WordLayoutInfo wordLayoutInfo;

    CreateWordTextInfo( word,
                        wordLayoutInfo );

    // White space's size could be different depending on the type of font. It's important to use the same font than the previous character to
    // avoid 'jumps' of characters when there is a switch between one text-actor per character and one text-actor per line and/or style.
    if( WordSeparator == wordLayoutInfo.mType )
    {
      // If current word is a word separator (white space) then the font of the last character is set.
      for( CharacterLayoutInfoContainer::iterator characterIt = wordLayoutInfo.mCharactersLayoutInfo.begin(), characterEndIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           characterIt != characterEndIt;
           ++characterIt )
      {
        CharacterLayoutInfo& characterLayout( *characterIt );

        characterLayout.mStyledText.mStyle.SetFontName( lastCharacterFont );
      }
    }
    else
    {
      // kepps the font of the last character.
      if( !wordLayoutInfo.mCharactersLayoutInfo.empty() )
      {
        lastCharacterFont = ( *( wordLayoutInfo.mCharactersLayoutInfo.end() - 1u ) ).mStyledText.mStyle.GetFontName();
      }
    }

    // Update the max word width figure.
    relayoutData.mTextLayoutInfo.mMaxWordWidth = std::max( relayoutData.mTextLayoutInfo.mMaxWordWidth, wordLayoutInfo.mSize.width );

    // Update layout info for the current paragraph.
    paragraphLayoutInfo.mAscender = std::max( paragraphLayoutInfo.mAscender, wordLayoutInfo.mAscender );
    paragraphLayoutInfo.mNumberOfCharacters += wordLayoutInfo.mCharactersLayoutInfo.size();
    UpdateSize( paragraphLayoutInfo.mSize, wordLayoutInfo.mSize );

    // Add the word to the current paragraph.
    paragraphLayoutInfo.mWordsLayoutInfo.push_back( wordLayoutInfo );
  } // end of words
}

void UpdateLayoutInfo( ParagraphLayoutInfo& paragraphLayoutInfo, float lineHeightOffset )
{
  // Update layout info.
  paragraphLayoutInfo.mSize = Size::ZERO;
  paragraphLayoutInfo.mAscender = 0.f;
  paragraphLayoutInfo.mNumberOfCharacters = 0u;
  for( WordLayoutInfoContainer::iterator it = paragraphLayoutInfo.mWordsLayoutInfo.begin(), endIt = paragraphLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& word( *it );

    UpdateSize( paragraphLayoutInfo.mSize, word.mSize );
    paragraphLayoutInfo.mAscender = std::max( paragraphLayoutInfo.mAscender, word.mAscender );
    paragraphLayoutInfo.mNumberOfCharacters += word.mCharactersLayoutInfo.size();
  }

  paragraphLayoutInfo.mSize.height += lineHeightOffset;
  paragraphLayoutInfo.mLineHeightOffset = lineHeightOffset;
}

void RemoveWordsFromParagraph( std::size_t wordIndex,
                               std::size_t numberOfWords,
                               float lineHeightOffset,
                               ParagraphLayoutInfo& paragraphLayout )
{
  // Removes words from a paragraph.

  // * Check if words or paragraphs can be merged after removing a number of words or a paragraph separator needs to be done outside this method.

  // Remove words from layout info.
  paragraphLayout.mWordsLayoutInfo.erase( paragraphLayout.mWordsLayoutInfo.begin() + wordIndex,
                                     paragraphLayout.mWordsLayoutInfo.begin() + ( wordIndex + numberOfWords ) );

  UpdateLayoutInfo( paragraphLayout, lineHeightOffset );
}

void RemoveCharactersFromParagraphInfo( TextView::RelayoutData& relayoutData,
                                   const std::size_t numberOfCharacters,
                                   bool& mergeWords,
                                   bool& mergeParagraphs,
                                   TextInfoIndices& textInfoIndicesBegin,
                                   TextInfoIndices& textInfoIndicesEnd,
                                   TextInfoIndices& textInfoMergeIndicesBegin,
                                   TextInfoIndices& textInfoMergeIndicesEnd,
                                   ParagraphLayoutInfo& paragraphLayout,
                                   std::vector<TextActor>& removedTextActorsFromFirstWord,
                                   std::vector<TextActor>& removedTextActorsFromLastWord )
{
  const TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  if( textInfoIndicesBegin.mWordIndex < textInfoIndicesEnd.mWordIndex )
  {
    // Deleted text is from different words. The two different words may be merged.

    // Get first word.
    WordLayoutInfo& firstWordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

    // Get last word.
    WordLayoutInfo& lastWordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );

    // whether first or last word need to be split and merged.
    bool mergeFromBegin = false;
    bool mergeToEnd = false;

    if( textInfoIndicesBegin.mCharacterIndex > 0u )
    {
      // First word is going to be split. It could be merged with the last word.
      mergeFromBegin = true;
      textInfoMergeIndicesBegin.mWordIndex = textInfoIndicesBegin.mWordIndex;
    }
    else if( ( textInfoIndicesBegin.mCharacterIndex == 0u ) && ( textInfoIndicesBegin.mWordIndex > 0u ) )
    {
      // First word is going to be removed completely.
      // Check if previous word could be merged.

      // Get word before.
      WordLayoutInfo& previousWordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex - 1u ) );
      if( WordSeparator != previousWordLayout.mType )
      {
        // Previous word is not a word separator, so could be merged.
        mergeFromBegin = true;
        textInfoMergeIndicesBegin.mWordIndex = textInfoIndicesBegin.mWordIndex - 1u;
      }
    }

    if( mergeFromBegin )
    {
      // First word (or previous one) could be merged. Check if last one could be merged as well.

      if( textInfoIndicesEnd.mCharacterIndex + 1u < lastWordLayout.mCharactersLayoutInfo.size() )
      {
        // Last word is going to be split. It could be merged with the first word.
        mergeToEnd = true;
        textInfoMergeIndicesEnd.mWordIndex = textInfoIndicesEnd.mWordIndex;
      }
      else if( ( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() ) && ( textInfoIndicesEnd.mWordIndex + 1u < paragraphLayout.mWordsLayoutInfo.size() ) )
      {
        // Last word is going to be removed completely.
        // Check if the word after could be merged.

        // Get word after.
        WordLayoutInfo& afterWordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex + 1u ) );
        if( WordSeparator != afterWordLayout.mType )
        {
          // The word after is not a word separator, so could be merged.
          mergeToEnd = true;
          textInfoMergeIndicesEnd.mWordIndex = textInfoIndicesEnd.mWordIndex + 1u;
        }
      }

      // Merge words only if both words could be merged.
      mergeWords = mergeFromBegin && mergeToEnd;
    }

    if( ( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() ) && ( textInfoIndicesEnd.mWordIndex + 1u == paragraphLayout.mWordsLayoutInfo.size() ) )
    {
      // Last word of the paragraph is going to be removed completely.
      // Check if it's a paragraph separator.

      if( ParagraphSeparator == lastWordLayout.mType )
      {
        // The paragraph separator is going to be removed.
        if( textInfoIndicesBegin.mParagraphIndex + 1u < textLayoutInfo.mParagraphsLayoutInfo.size() )
        {
          //  Paragraph need to be merged.
          textInfoMergeIndicesBegin.mParagraphIndex = textInfoIndicesBegin.mParagraphIndex;
          textInfoMergeIndicesEnd.mParagraphIndex = textInfoIndicesBegin.mParagraphIndex + 1u;
          mergeParagraphs= true;

          ++textInfoIndicesBegin.mParagraphIndex; // increase both indices,
          textInfoIndicesEnd.mParagraphIndex +=2u; // will delete last paragraph.
        }
      }
    }

    if( textInfoIndicesBegin.mCharacterIndex > 0u )
    {
      // First word needs to be split.

      // Store text-actors before removing them.
      CollectTextActors( removedTextActorsFromFirstWord, firstWordLayout, textInfoIndicesBegin.mCharacterIndex, firstWordLayout.mCharactersLayoutInfo.size() );

      RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                                firstWordLayout.mCharactersLayoutInfo.size() - textInfoIndicesBegin.mCharacterIndex,
                                firstWordLayout );

      ++textInfoIndicesBegin.mWordIndex; // will delete from the word after.
    }

    if( textInfoIndicesEnd.mCharacterIndex + 1u < lastWordLayout.mCharactersLayoutInfo.size() )
    {
      // Last word needs to be split.

      // Store text-actors before removing them.
      CollectTextActors( removedTextActorsFromLastWord, lastWordLayout, 0u, textInfoIndicesEnd.mCharacterIndex + 1u );

      RemoveCharactersFromWord( 0u,
                                textInfoIndicesEnd.mCharacterIndex + 1u,
                                lastWordLayout );

      if( mergeWords )
      {
        // This word is going to be merged, so is not needed.
        ++textInfoIndicesEnd.mWordIndex; // will delete the last word.
      }
    }
    else if( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() )
    {
      // The whole last word is going to be removed.
      ++textInfoIndicesEnd.mWordIndex; // will delete the last word.

      if( ( WordSeparator == lastWordLayout.mType ) && mergeWords )
      {
        // The last word is a word separator and the word after is going to be merged so is not needed.
        ++textInfoIndicesEnd.mWordIndex; // will delete the word after the last one.
      }
    }
  }
  else
  {
    // Chraracters to be removed are from the same word.

    RemoveCharactersFromWordInfo( relayoutData,
                                  numberOfCharacters,
                                  mergeWords,
                                  mergeParagraphs,
                                  textInfoIndicesBegin,
                                  textInfoIndicesEnd,
                                  textInfoMergeIndicesBegin,
                                  textInfoMergeIndicesEnd,
                                  paragraphLayout,
                                  removedTextActorsFromFirstWord );
  } // word indices
}

void SplitParagraph( const TextInfoIndices& indices,
                     float lineHeightOffset,
                     ParagraphLayoutInfo& firstParagraphLayoutInfo,
                     ParagraphLayoutInfo& lastParagraphLayoutInfo )
{
  // Splits a paragraph in two.
  // A word may be split in two as well.

  // * Split the word within the paragraph.
  // * Add last part of the word to the new paragraph.
  // * Add words from wordPosition + 1 to the end.
  // * Update layout info of the last paragraph.
  // * Remove words added to the last part of the paragraph from the first paragraph.

  // early returns!!
  if( ( 0u == indices.mWordIndex ) && ( 0u == indices.mCharacterIndex ) )
  {
    // the whole paragraph goes to the last part.
    lastParagraphLayoutInfo = firstParagraphLayoutInfo;

    firstParagraphLayoutInfo = ParagraphLayoutInfo();

    return;
  }

  if( !firstParagraphLayoutInfo.mWordsLayoutInfo.empty() )
  {
    const std::size_t numberOfWords = firstParagraphLayoutInfo.mWordsLayoutInfo.size();
    if( indices.mWordIndex == numberOfWords - 1u )
    {
      const WordLayoutInfo& word( *( firstParagraphLayoutInfo.mWordsLayoutInfo.end() - 1u ) );
      if( indices.mCharacterIndex == word.mCharactersLayoutInfo.size() )
      {
        // the whole paragraph goes to the first part.

        // Just delete whatever there is in the last part of the paragraph.
        lastParagraphLayoutInfo = ParagraphLayoutInfo();

        return;
      }
    }
  }

  lastParagraphLayoutInfo = ParagraphLayoutInfo();

  // 1) Split the word whitin the paragraph.
  WordLayoutInfo& firstWordLayoutInfo( *( firstParagraphLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex ) );
  WordLayoutInfo lastWordLayoutInfo;

  SplitWord( indices.mCharacterIndex,
             firstWordLayoutInfo,
             lastWordLayoutInfo );

  // 2) Add last part of the word to the new paragraph.
  if( !lastWordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    lastParagraphLayoutInfo.mWordsLayoutInfo.push_back( lastWordLayoutInfo );
  }

  // 3) Add words from word-position + 1 to the end.
  lastParagraphLayoutInfo.mWordsLayoutInfo.insert( lastParagraphLayoutInfo.mWordsLayoutInfo.end(),
                                                   firstParagraphLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex + 1u,
                                                   firstParagraphLayoutInfo.mWordsLayoutInfo.end() );

  // 4) update layout info of the last paragraph.
  for( WordLayoutInfoContainer::iterator it = lastParagraphLayoutInfo.mWordsLayoutInfo.begin(), endIt = lastParagraphLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& layoutInfo( *it );

    UpdateSize( lastParagraphLayoutInfo.mSize, layoutInfo.mSize );
    lastParagraphLayoutInfo.mNumberOfCharacters += layoutInfo.mCharactersLayoutInfo.size();
    lastParagraphLayoutInfo.mAscender = std::max( lastParagraphLayoutInfo.mAscender, layoutInfo.mAscender );
  }
  lastParagraphLayoutInfo.mSize.height += lineHeightOffset;
  lastParagraphLayoutInfo.mLineHeightOffset = lineHeightOffset;

  // 5) Remove words added to the last part of the paragraph from the first paragraph.

  // if the number of characters of the last word of the first paragraph is zero, it should be removed.
  const std::size_t index = ( firstWordLayoutInfo.mCharactersLayoutInfo.empty() ? indices.mWordIndex : indices.mWordIndex + 1u );

  firstParagraphLayoutInfo.mWordsLayoutInfo.erase( firstParagraphLayoutInfo.mWordsLayoutInfo.begin() + index, firstParagraphLayoutInfo.mWordsLayoutInfo.end() );

  // 6) update layout info of the first paragraph.
  UpdateLayoutInfo( firstParagraphLayoutInfo, lineHeightOffset );
}

void MergeParagraph( ParagraphLayoutInfo& firstParagraphLayoutInfo,
                     const ParagraphLayoutInfo& lastParagraphLayoutInfo )
{
  // Merges two given paragraphs.
  //
  // Can't merge two paragraphs if the last word of the first one is a paragraph separator (new paragraph character)

  // Early returns.

  if( lastParagraphLayoutInfo.mWordsLayoutInfo.empty() )
  {
    // Nothing to merge if last paragraph is empty.
    return;
  }

  if( firstParagraphLayoutInfo.mWordsLayoutInfo.empty() )
  {
    // If first paragraph is empty, just copy the last paragraph to the first one.
    firstParagraphLayoutInfo = lastParagraphLayoutInfo;

   return;
  }

  // Check the last word of the first paragraph doesn't finish with a new paragraph character.
  WordLayoutInfo& lastWordLayout( *( firstParagraphLayoutInfo.mWordsLayoutInfo.end() - 1u ) );
  if( ParagraphSeparator == lastWordLayout.mType )
  {
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeParagraph(). ERROR: A paragraph can't be merged to another paragraph which finishes with a new paragraph character." );
  }

  // If the las word of the first paragraph or the first word of the last paragraph is a white space, both paragraphs can be concatenated.
  // Otherwise both words need to be merged first.
  const WordLayoutInfo& firstWordLayout( *lastParagraphLayoutInfo.mWordsLayoutInfo.begin() );

  std::size_t index = 0u;
  if( ( WordSeparator != lastWordLayout.mType ) && ( WordSeparator != firstWordLayout.mType ) && ( ParagraphSeparator != firstWordLayout.mType ) )
  {
    // Last word of the first paragraph is not a word separator and first word of the last paragraph is not a word or paragraph separator.
    // Words need to be merged.

    MergeWord( lastWordLayout,
               firstWordLayout );

    // After merging two words, the rest of the words need to be added.
    ++index; // By increasing this index the word already merged won't be added again.
  }

  // Merge layout info

  // Insert the layout of the words.
  firstParagraphLayoutInfo.mWordsLayoutInfo.insert( firstParagraphLayoutInfo.mWordsLayoutInfo.end(),
                                                   lastParagraphLayoutInfo.mWordsLayoutInfo.begin() + index, lastParagraphLayoutInfo.mWordsLayoutInfo.end() );

  // Update the size and other layout parameters.
  UpdateSize( firstParagraphLayoutInfo.mSize, lastParagraphLayoutInfo.mSize );
  firstParagraphLayoutInfo.mAscender = std::max( firstParagraphLayoutInfo.mAscender, lastParagraphLayoutInfo.mAscender );
  firstParagraphLayoutInfo.mLineHeightOffset = std::max( firstParagraphLayoutInfo.mLineHeightOffset, lastParagraphLayoutInfo.mLineHeightOffset );
  firstParagraphLayoutInfo.mNumberOfCharacters += lastParagraphLayoutInfo.mNumberOfCharacters;
}

WordLayoutInfo GetLastWordLayoutInfo( const ParagraphLayoutInfo& paragraphLayoutInfo )
{
  WordLayoutInfo layoutInfo;

  if( !paragraphLayoutInfo.mWordsLayoutInfo.empty() )
  {
    layoutInfo = *( paragraphLayoutInfo.mWordsLayoutInfo.end() - 1u );
  }

  return layoutInfo;
}

CharacterLayoutInfo GetFirstCharacterLayoutInfo( const ParagraphLayoutInfo& paragraphLayoutInfo )
{
  CharacterLayoutInfo layoutInfo;

  if( !paragraphLayoutInfo.mWordsLayoutInfo.empty() )
  {
    const WordLayoutInfo& wordInfo( *paragraphLayoutInfo.mWordsLayoutInfo.begin() );

    layoutInfo = GetFirstCharacterLayoutInfo( wordInfo );
  }

  return layoutInfo;
}

CharacterLayoutInfo GetLastCharacterLayoutInfo( const ParagraphLayoutInfo& paragraphLayoutInfo )
{
  const WordLayoutInfo wordInfo = GetLastWordLayoutInfo( paragraphLayoutInfo );

  return GetLastCharacterLayoutInfo( wordInfo );
}

void CollectTextActorsFromParagraphs( std::vector<TextActor>& textActors, const TextLayoutInfo& textLayoutInfo, const std::size_t paragraphIndexBegin, const std::size_t paragraphIndexEnd )
{
  for( ParagraphLayoutInfoContainer::const_iterator paragraphIt = textLayoutInfo.mParagraphsLayoutInfo.begin() + paragraphIndexBegin, paragraphEndIt = textLayoutInfo.mParagraphsLayoutInfo.begin() + paragraphIndexEnd;
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    const ParagraphLayoutInfo& paragraph( *paragraphIt );

    CollectTextActorsFromWords( textActors, paragraph, 0u, paragraph.mWordsLayoutInfo.size() );
  }
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
