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
#include <dali-toolkit/internal/controls/text-view/text-processor-bidirectional-info.h>

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

void RightToLeftParagraphLayout::Clear()
{
  mWordsLayoutInfo.clear();
  mText = Text();
  mTextStyles.Clear();
}

ParagraphLayoutInfo::ParagraphLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mLineHeightOffset( 0.f ),
  mFirstCharacter( 0u ),
  mNumberOfCharacters( 0u ),
  mWordsLayoutInfo(),
  mText(),
  mTextStyles(),
  mRightToLeftLayout( NULL ),
  mBidirectionalParagraphInfo( NULL ),
  mBidirectionalLinesInfo()
{
}

ParagraphLayoutInfo::~ParagraphLayoutInfo()
{
  if( NULL != mRightToLeftLayout )
  {
    // TextStyle pointers are the same than the ones stored at ParagraphLayoutInfo::mTextStyles.
    // Do not delete them, just clear the vector.
    mRightToLeftLayout->mTextStyles.Clear();

    delete mRightToLeftLayout;
  }

  // Clears text styles. It destroys TextStyle objects.
  ClearStyles();

  // Deletes the bidirectional info for the whole paragraph.
  delete mBidirectionalParagraphInfo;

  // Clears the bidirectional info for all lines. Destroys the BidirectionalLineInfo objects.
  ClearBidirectionalInfo();
}

ParagraphLayoutInfo::ParagraphLayoutInfo( const ParagraphLayoutInfo& paragraph )
: mSize( paragraph.mSize ),
  mAscender( paragraph.mAscender ),
  mLineHeightOffset( paragraph.mLineHeightOffset ),
  mFirstCharacter( paragraph.mFirstCharacter ),
  mNumberOfCharacters( paragraph.mNumberOfCharacters ),
  mWordsLayoutInfo( paragraph.mWordsLayoutInfo ),
  mText( paragraph.mText ),
  mTextStyles(),
  mRightToLeftLayout( NULL ),
  // Copies bidirectional info for the whole paragraph.
  mBidirectionalParagraphInfo( ( NULL == paragraph.mBidirectionalParagraphInfo ) ? NULL : new TextProcessor::BidirectionalParagraphInfo( *paragraph.mBidirectionalParagraphInfo ) ),
  mBidirectionalLinesInfo()
{
  // Copies styles.
  for( Vector<TextStyle*>::ConstIterator it = paragraph.mTextStyles.Begin(), endIt = paragraph.mTextStyles.End(); it != endIt; ++it )
  {
    mTextStyles.PushBack( new TextStyle( *(*it) ) );
  }

  // Copies bidirectional info for each line.
  for( Vector<TextProcessor::BidirectionalLineInfo*>::ConstIterator it = mBidirectionalLinesInfo.Begin(), endIt = mBidirectionalLinesInfo.End(); it != endIt; ++it )
  {
    mBidirectionalLinesInfo.PushBack( new TextProcessor::BidirectionalLineInfo( *( *it ) ) );
  }
}

ParagraphLayoutInfo& ParagraphLayoutInfo::operator=( const ParagraphLayoutInfo& paragraph )
{
  mSize = paragraph.mSize;
  mAscender = paragraph.mAscender;
  mLineHeightOffset = paragraph.mLineHeightOffset;
  mFirstCharacter = paragraph.mFirstCharacter;
  mNumberOfCharacters = paragraph.mNumberOfCharacters;
  mWordsLayoutInfo = paragraph.mWordsLayoutInfo;
  mText = paragraph.mText;

  // If it has styles, destroy them.
  ClearStyles();

  // Copies styles.
  for( Vector<TextStyle*>::ConstIterator it = paragraph.mTextStyles.Begin(), endIt = paragraph.mTextStyles.End(); it != endIt; ++it )
  {
    mTextStyles.PushBack( new TextStyle( *(*it) ) );
  }

  // Copies the paragraph's bidirectiona info.
  if( NULL == paragraph.mBidirectionalParagraphInfo )
  {
    // The source doesn't have bidirectiona info. Deletes the current one.
    delete mBidirectionalParagraphInfo;
    mBidirectionalParagraphInfo = NULL;
  }
  else
  {
    // The source has bidirectional info.
    if( NULL != mBidirectionalParagraphInfo )
    {
      // It it has, copy to it.
      *mBidirectionalParagraphInfo = *paragraph.mBidirectionalParagraphInfo;
    }
    else
    {
      // If it doesn't have, create a new one.
      mBidirectionalParagraphInfo = new TextProcessor::BidirectionalParagraphInfo( *paragraph.mBidirectionalParagraphInfo );
    }
  }

  // If it has bidirectiona info per line, destroy them.
  ClearBidirectionalInfo();

  // Copies bidirectional info per line.
  for( Vector<TextProcessor::BidirectionalLineInfo*>::ConstIterator it = mBidirectionalLinesInfo.Begin(), endIt = mBidirectionalLinesInfo.End(); it != endIt; ++it )
  {
    mBidirectionalLinesInfo.PushBack( new TextProcessor::BidirectionalLineInfo( *( *it ) ) );
  }

  return *this;
}

void ParagraphLayoutInfo::ClearBidirectionalInfo()
{
  // Destroys the bidirectional infor per line.
  for( Vector<TextProcessor::BidirectionalLineInfo*>::Iterator it = mBidirectionalLinesInfo.Begin(), endIt = mBidirectionalLinesInfo.End(); it != endIt; ++it )
  {
    delete *it;
  }
  mBidirectionalLinesInfo.Clear();
}

void ParagraphLayoutInfo::ClearStyles()
{
  // Destroys the styles.
  for( Vector<TextStyle*>::Iterator it = mTextStyles.Begin(), endIt = mTextStyles.End(); it != endIt; ++it )
  {
    delete *it;
  }
  mTextStyles.Clear();
}

void CreateParagraphInfo( TextView::RelayoutData& relayoutData,
                          ParagraphLayoutInfo& paragraphLayoutInfo )
{
  if( TextProcessor::ContainsRightToLeftCharacter( paragraphLayoutInfo.mText ) )
  {
    // If the text is bidirectional, the characters will be converted and reordered
    // as specified by the Unicode Bidirectional Algorithm.

    paragraphLayoutInfo.mBidirectionalParagraphInfo = new TextProcessor::BidirectionalParagraphInfo();

    TextProcessor::ProcessBidirectionalText( paragraphLayoutInfo.mText, paragraphLayoutInfo.mBidirectionalParagraphInfo );
  }

  // Split the paragraph in words. It retrieves the positions of white spaces and the last '\n' if there is one.
  Vector<std::size_t> positions;
  TextProcessor::SplitInWords( paragraphLayoutInfo.mText, positions );

  const std::size_t lastCharacterIndex = paragraphLayoutInfo.mText.GetLength() - 1u;
  const bool isLastCharacterParagraphSeparator = paragraphLayoutInfo.mText.IsNewLine( lastCharacterIndex );

  // The number of words is ~the number of white spaces found + 1u.
  // White spaces are also words.
  // New line characters are also white spaces. If the last character is a white space the +1 is not needed.
  const std::size_t numberOfWords = 2u * positions.Count() + ( isLastCharacterParagraphSeparator ? 0u : 1u );

  // Reserve space for all the words.
  paragraphLayoutInfo.mWordsLayoutInfo.resize( numberOfWords, WordLayoutInfo() );

  // Traverses all positions creating and setting all character layout info objects to every word.
  std::size_t wordIndex = 0u;
  Vector<std::size_t>::ConstIterator positionIt = positions.Begin();
  Vector<std::size_t>::ConstIterator positionEndIt = positions.End();
  std::size_t from = 0u;
  for( std::size_t positionIndex = 0u, size = positions.Count() + 1u; positionIndex < size; ++positionIndex )
  {
    const bool isEndPosition = positionIt == positionEndIt;
    const std::size_t to = isEndPosition ? lastCharacterIndex + 1u : *positionIt;

    if( from < to )
    {
      // The word is not a white space.
      WordLayoutInfo& wordLayoutInfo = *( paragraphLayoutInfo.mWordsLayoutInfo.begin() + wordIndex );
      ++wordIndex;
      // Sets the index within the paragraph to the first character of the word.
      wordLayoutInfo.mFirstCharacter = from;
      // Creates character layout info objects.
      wordLayoutInfo.mCharactersLayoutInfo.resize( ( to - from ), CharacterLayoutInfo() );
    }

    if( !isEndPosition )
    {
      // Create a word for the white space.
      WordLayoutInfo& wordLayoutInfo = *( paragraphLayoutInfo.mWordsLayoutInfo.begin() + wordIndex );
      ++wordIndex;
      // Sets the index within the paragraph to the white space.
      wordLayoutInfo.mFirstCharacter = to;
      wordLayoutInfo.mType = WordSeparator;

      CharacterLayoutInfo characterLayoutInfo;
      wordLayoutInfo.mCharactersLayoutInfo.push_back( characterLayoutInfo );
    }

    from = to + 1u;

    if( !isEndPosition )
    {
      // next white space position.
      ++positionIt;
    }
    else
    {
      // All white space positiona have been traversed.
      // It may be some extra words. i.e if the text is \n.
      // Erase them.
      paragraphLayoutInfo.mWordsLayoutInfo.erase( paragraphLayoutInfo.mWordsLayoutInfo.begin() + wordIndex, paragraphLayoutInfo.mWordsLayoutInfo.end() );

      // Check if the last character is a new paragraph character.
      if( isLastCharacterParagraphSeparator )
      {
        ( *( paragraphLayoutInfo.mWordsLayoutInfo.end() - 1u ) ).mType = ParagraphSeparator;
      }
    }
  }

  // Traverse all words and fill the layout info.
  for( WordLayoutInfoContainer::iterator it = paragraphLayoutInfo.mWordsLayoutInfo.begin(), endIt = paragraphLayoutInfo.mWordsLayoutInfo.end(); it != endIt; ++it )
  {
    WordLayoutInfo& wordLayoutInfo( *it );

    CreateWordTextInfo( paragraphLayoutInfo.mText,
                        paragraphLayoutInfo.mTextStyles,
                        wordLayoutInfo );

    // Update layout info for the current paragraph.
    UpdateSize( paragraphLayoutInfo.mSize, wordLayoutInfo.mSize );
    paragraphLayoutInfo.mAscender = std::max( paragraphLayoutInfo.mAscender, wordLayoutInfo.mAscender );
    paragraphLayoutInfo.mNumberOfCharacters += wordLayoutInfo.mCharactersLayoutInfo.size();

    // Update the max word width figure.
    relayoutData.mTextLayoutInfo.mMaxWordWidth = std::max( relayoutData.mTextLayoutInfo.mMaxWordWidth, wordLayoutInfo.mSize.width );
  } // end of words
}

void UpdateLayoutInfo( ParagraphLayoutInfo& paragraphLayoutInfo, float lineHeightOffset )
{
  // Update layout info.

  // Initialize members to be updated.
  paragraphLayoutInfo.mSize = Size::ZERO;
  paragraphLayoutInfo.mAscender = 0.f;
  paragraphLayoutInfo.mNumberOfCharacters = 0u;

  // Traverses all words.
  for( WordLayoutInfoContainer::iterator it = paragraphLayoutInfo.mWordsLayoutInfo.begin(), endIt = paragraphLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& word( *it );

    // Sets the index within the paragraph to the first character of the word.
    word.mFirstCharacter = paragraphLayoutInfo.mNumberOfCharacters;

    // Updates the paragraph's size.
    UpdateSize( paragraphLayoutInfo.mSize, word.mSize );

    // Updates the paragraph's max asender.
    paragraphLayoutInfo.mAscender = std::max( paragraphLayoutInfo.mAscender, word.mAscender );

    // Updates the paragraph's number of characters.
    paragraphLayoutInfo.mNumberOfCharacters += word.mCharactersLayoutInfo.size();
  }

  // Sets the line height offset.
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

  // 7) Split text and styles.

  // Copies the whole text to the last part of the paragraph.
  lastParagraphLayoutInfo.mText = firstParagraphLayoutInfo.mText;

  // Removes from the first part of the paragraph the text that goes to the last part.
  firstParagraphLayoutInfo.mText.Remove( indices.mCharacterParagraphIndex, firstParagraphLayoutInfo.mText.GetLength() - indices.mCharacterParagraphIndex );

  // Removes from the last part of the paragraph the text that remains in the first part.
  lastParagraphLayoutInfo.mText.Remove( 0u, indices.mCharacterParagraphIndex );

  // Sets the character's styles for the last part of the paragraph.
  lastParagraphLayoutInfo.mTextStyles.Insert( lastParagraphLayoutInfo.mTextStyles.End(),
                                         firstParagraphLayoutInfo.mTextStyles.Begin() + indices.mCharacterParagraphIndex,
                                         firstParagraphLayoutInfo.mTextStyles.End() );

  // Removes the character's styles that go to the last part of the paragraph.
  firstParagraphLayoutInfo.mTextStyles.Erase( firstParagraphLayoutInfo.mTextStyles.Begin() + indices.mCharacterParagraphIndex,
                                         firstParagraphLayoutInfo.mTextStyles.End() );
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
  const std::size_t numberOfWords = firstParagraphLayoutInfo.mWordsLayoutInfo.size();
  firstParagraphLayoutInfo.mWordsLayoutInfo.insert( firstParagraphLayoutInfo.mWordsLayoutInfo.end(),
                                                   lastParagraphLayoutInfo.mWordsLayoutInfo.begin() + index, lastParagraphLayoutInfo.mWordsLayoutInfo.end() );

  // Increase the index of the first character of each inserted word.
  for( WordLayoutInfoContainer::iterator it = firstParagraphLayoutInfo.mWordsLayoutInfo.begin() + numberOfWords, endIt = firstParagraphLayoutInfo.mWordsLayoutInfo.end(); it != endIt; ++it )
  {
    WordLayoutInfo& word( *it );
    word.mFirstCharacter += firstParagraphLayoutInfo.mNumberOfCharacters;
  }

  // Update the size and other layout parameters.
  UpdateSize( firstParagraphLayoutInfo.mSize, lastParagraphLayoutInfo.mSize );
  firstParagraphLayoutInfo.mAscender = std::max( firstParagraphLayoutInfo.mAscender, lastParagraphLayoutInfo.mAscender );
  firstParagraphLayoutInfo.mLineHeightOffset = std::max( firstParagraphLayoutInfo.mLineHeightOffset, lastParagraphLayoutInfo.mLineHeightOffset );
  firstParagraphLayoutInfo.mNumberOfCharacters += lastParagraphLayoutInfo.mNumberOfCharacters;

  // Merge text and styles.
  firstParagraphLayoutInfo.mText.Append( lastParagraphLayoutInfo.mText );
  for( Vector<TextStyle*>::ConstIterator it = lastParagraphLayoutInfo.mTextStyles.Begin(), endIt = lastParagraphLayoutInfo.mTextStyles.End(); it != endIt; ++it )
  {
    firstParagraphLayoutInfo.mTextStyles.PushBack( new TextStyle( *(*it) ) );
  }
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
