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

// INTERNAL INCLUDES
#include "text-view-word-group-processor.h"
#include "text-view-word-processor.h"
#include "text-view-processor-helper-functions.h"
#include "text-processor.h"

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

WordGroupLayoutInfo::WordGroupLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mDirection( LTR ),
  mWordsLayoutInfo(),
  mNumberOfCharacters( 0 )
{
}

WordGroupLayoutInfo::WordGroupLayoutInfo( const WordGroupLayoutInfo& group )
: mSize( group.mSize ),
  mAscender( group.mAscender ),
  mDirection( group.mDirection ),
  mWordsLayoutInfo( group.mWordsLayoutInfo ),
  mNumberOfCharacters( group.mNumberOfCharacters )
{
}

WordGroupLayoutInfo& WordGroupLayoutInfo::operator=( const WordGroupLayoutInfo& group )
{
  mSize = group.mSize;
  mAscender = group.mAscender;
  mDirection = group.mDirection;
  mWordsLayoutInfo = group.mWordsLayoutInfo;
  mNumberOfCharacters = group.mNumberOfCharacters;

  return *this;
}

void UpdateGroupLayoutInfo( TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo )
{
  wordGroupLayoutInfo.mSize = Size();

  for( WordLayoutInfoContainer::iterator it = wordGroupLayoutInfo.mWordsLayoutInfo.begin(), endIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& layoutInfo( *it );

    UpdateSize( wordGroupLayoutInfo.mSize, layoutInfo.mSize );
  }
}

void CreateWordGroupInfo( const MarkupProcessor::StyledTextArray& wordGroup,
                          TextViewProcessor::TextLayoutInfo& textLayoutInfo,
                          TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo )
{
  // Set the direction of the group.
  wordGroupLayoutInfo.mDirection = ( TextProcessor::BeginsRightToLeftCharacter( wordGroup ) ? TextViewProcessor::RTL : TextViewProcessor::LTR );

  // Split the group of words in words
  std::vector<MarkupProcessor::StyledTextArray> words;
  TextProcessor::SplitInWords( wordGroup, words );

  // if last word has a new line separator, create a new word.
  if( !words.empty() )
  {
    MarkupProcessor::StyledTextArray& word( *( words.end() - 1 ) );
    if( word.size() > 1 )
    {
      // do nothing if the word has only one character.
      MarkupProcessor::StyledText& styledText( *( word.end() - 1 ) );
      if( !styledText.mText.IsEmpty() )
      {
        const std::size_t length = styledText.mText.GetLength();
        if( styledText.mText[length-1].IsNewLine() )
        {
          // Last character of this word is a new line character.

          // Remove line separator character from current word.
          styledText.mText.Remove( length - 1, 1 );

          // Create a new word with the line separator character.
          MarkupProcessor::StyledText newLineText( Text( styledText.mText[length-1] ), styledText.mStyle );

          MarkupProcessor::StyledTextArray newLineWord;
          newLineWord.push_back( newLineText );

          words.push_back( newLineWord );
        }
      }
    }
  }

  // Reverse if right to left.
  if( TextViewProcessor::RTL == wordGroupLayoutInfo.mDirection )
  {
    std::reverse( words.begin(), words.end() );
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
        lastCharacterFont = ( *( wordLayoutInfo.mCharactersLayoutInfo.end() - 1 ) ).mStyledText.mStyle.GetFontName();
      }
    }

    // Update layout info for the current group of words.
    wordGroupLayoutInfo.mNumberOfCharacters += wordLayoutInfo.mCharactersLayoutInfo.size();
    UpdateSize( wordGroupLayoutInfo.mSize, wordLayoutInfo.mSize );
    wordGroupLayoutInfo.mAscender = std::max( wordGroupLayoutInfo.mAscender, wordLayoutInfo.mAscender );

    // Add current word to the group of words.
    wordGroupLayoutInfo.mWordsLayoutInfo.push_back( wordLayoutInfo );

    // Update the max word width figure.
    textLayoutInfo.mMaxWordWidth = std::max( textLayoutInfo.mMaxWordWidth, wordLayoutInfo.mSize.width );
  } // end of words
}

void RemoveCharactersFromWordGroupInfo( TextView::RelayoutData& relayoutData,
                                        const std::size_t numberOfCharacters,
                                        bool& mergeWords,
                                        bool& mergeLines,
                                        TextViewProcessor::TextInfoIndices& textInfoIndicesBegin,
                                        TextViewProcessor::TextInfoIndices& textInfoIndicesEnd,
                                        TextViewProcessor::TextInfoIndices& textInfoMergeIndicesBegin,
                                        TextViewProcessor::TextInfoIndices& textInfoMergeIndicesEnd,
                                        TextViewProcessor::WordGroupLayoutInfo& groupLayout,
                                        std::vector<TextActor>& removedTextActorsFromFirstWord,
                                        std::vector<TextActor>& removedTextActorsFromLastWord )
{
  const TextViewProcessor::TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  if( textInfoIndicesBegin.mWordIndex < textInfoIndicesEnd.mWordIndex )
  {
    // Deleted text is from different words. The two different words may be merged.

    // Get first word.
    WordLayoutInfo& firstWordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

    // Get last word.
    WordLayoutInfo& lastWordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );

    // whether first or last word need to be split and merged.
    bool mergeFromBegin = false;
    bool mergeToEnd = false;

    if( textInfoIndicesBegin.mCharacterIndex > 0 )
    {
      // First word is going to be split. It could be merged with the last word.
      mergeFromBegin = true;
      textInfoMergeIndicesBegin.mWordIndex = textInfoIndicesBegin.mWordIndex;
    }
    else if( ( textInfoIndicesBegin.mCharacterIndex == 0 ) && ( textInfoIndicesBegin.mWordIndex > 0 ) )
    {
      // First word is going to be removed completely.
      // Check if previous word could be merged.

      // Get word before.
      WordLayoutInfo& previousWordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex - 1 ) );
      if( WordSeparator != previousWordLayout.mType )
      {
        // Previous word is not a word separator, so could be merged.
        mergeFromBegin = true;
        textInfoMergeIndicesBegin.mWordIndex = textInfoIndicesBegin.mWordIndex - 1;
      }
    }

    if( mergeFromBegin )
    {
      // First word (or previous one) could be merged. Check if last one could be merged as well.

      if( textInfoIndicesEnd.mCharacterIndex + 1 < lastWordLayout.mCharactersLayoutInfo.size() )
      {
        // Last word is going to be split. It could be merged with the first word.
        mergeToEnd = true;
        textInfoMergeIndicesEnd.mWordIndex = textInfoIndicesEnd.mWordIndex;
      }
      else if( ( textInfoIndicesEnd.mCharacterIndex + 1 == lastWordLayout.mCharactersLayoutInfo.size() ) && ( textInfoIndicesEnd.mWordIndex + 1 < groupLayout.mWordsLayoutInfo.size() ) )
      {
        // Last word is going to be removed completely.
        // Check if the word after could be merged.

        // Get word after.
        WordLayoutInfo& afterWordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex + 1 ) );
        if( WordSeparator != afterWordLayout.mType )
        {
          // The word after is not a word separator, so could be merged.
          mergeToEnd = true;
          textInfoMergeIndicesEnd.mWordIndex = textInfoIndicesEnd.mWordIndex + 1;
        }
      }

      // Merge words only if both words could be merged.
      mergeWords = mergeFromBegin && mergeToEnd;
    }

    if( ( textInfoIndicesEnd.mCharacterIndex + 1 == lastWordLayout.mCharactersLayoutInfo.size() ) && ( textInfoIndicesEnd.mWordIndex + 1 == groupLayout.mWordsLayoutInfo.size() ) )
    {
      // Last word of the line is going to be removed completely.
      // Check if it's a line separator.

      if( LineSeparator == lastWordLayout.mType )
      {
        // The line separator is going to be removed.
        if( textInfoIndicesBegin.mLineIndex + 1 < textLayoutInfo.mLinesLayoutInfo.size() )
        {
          //  Line need to be merged.
          textInfoMergeIndicesBegin.mLineIndex = textInfoIndicesBegin.mLineIndex;
          textInfoMergeIndicesEnd.mLineIndex = textInfoIndicesBegin.mLineIndex + 1;
          mergeLines= true;

          ++textInfoIndicesBegin.mLineIndex; // increase both indices,
          textInfoIndicesEnd.mLineIndex +=2; // will delete last line.
        }
      }
    }

    if( textInfoIndicesBegin.mCharacterIndex > 0 )
    {
      // First word needs to be split.

      // Store text-actors before removing them.
      CollectTextActors( removedTextActorsFromFirstWord, firstWordLayout, textInfoIndicesBegin.mCharacterIndex, firstWordLayout.mCharactersLayoutInfo.size() );

      RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                                firstWordLayout.mCharactersLayoutInfo.size() - textInfoIndicesBegin.mCharacterIndex,
                                firstWordLayout );

      ++textInfoIndicesBegin.mWordIndex; // will delete from the word after.
    }

    if( textInfoIndicesEnd.mCharacterIndex + 1 < lastWordLayout.mCharactersLayoutInfo.size() )
    {
      // Last word needs to be split.

      // Store text-actors before removing them.
      CollectTextActors( removedTextActorsFromLastWord, lastWordLayout, 0, textInfoIndicesEnd.mCharacterIndex + 1 );

      RemoveCharactersFromWord( 0,
                                textInfoIndicesEnd.mCharacterIndex + 1,
                                lastWordLayout );

      if( mergeWords )
      {
        // This word is going to be merged, so is not needed.
        ++textInfoIndicesEnd.mWordIndex; // will delete the last word.
      }
    }
    else if( textInfoIndicesEnd.mCharacterIndex + 1 == lastWordLayout.mCharactersLayoutInfo.size() )
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
                                  mergeLines,
                                  textInfoIndicesBegin,
                                  textInfoIndicesEnd,
                                  textInfoMergeIndicesBegin,
                                  textInfoMergeIndicesEnd,
                                  groupLayout,
                                  removedTextActorsFromFirstWord );
  } // word indices
}

void RemoveWordsFromWordGroup( const std::size_t wordIndex,
                               const std::size_t numberOfWords,
                               WordGroupLayoutInfo& wordGroupLayoutInfo )
{
  // Removes words from a group of words.

  // * Check if words or lines can be merged after removing a word or line separator have to be done outside this method.

  // * Note: Currently it's only used to remove a number of words from the beginning, or
  //         from wordIndex index to the end. This function doesn't merge words (if a white space is removed)
  //         TODO: merge words if required.

  const std::size_t wordEndIndex = wordIndex + numberOfWords;

  // Remove words from layout info.
  wordGroupLayoutInfo.mWordsLayoutInfo.erase( wordGroupLayoutInfo.mWordsLayoutInfo.begin() + wordIndex,
                                              wordGroupLayoutInfo.mWordsLayoutInfo.begin() + wordEndIndex );

  // update layout info
  wordGroupLayoutInfo.mSize = Size();
  wordGroupLayoutInfo.mAscender = 0.f;
  wordGroupLayoutInfo.mNumberOfCharacters = 0;
  for( WordLayoutInfoContainer::const_iterator it = wordGroupLayoutInfo.mWordsLayoutInfo.begin(), endIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    const WordLayoutInfo& info( *it );

    UpdateSize( wordGroupLayoutInfo.mSize, info.mSize );
    wordGroupLayoutInfo.mAscender = std::max( wordGroupLayoutInfo.mAscender, info.mAscender );
    wordGroupLayoutInfo.mNumberOfCharacters += info.mCharactersLayoutInfo.size();
  }
}

void SplitWordGroup( const TextInfoIndices& indices,
                     WordGroupLayoutInfo& firstWordGroupLayoutInfo,
                     WordGroupLayoutInfo& lastWordGroupLayoutInfo )
{
  // Splits a group of words in two.
  // A word may be split in two as well.

  // * Split the word pointed by indices.mWordIndex using the indices.mCharacterIndex index.
  // * Add the last part of the word as first word of the last part of the group of words.
  // * Add folliwing words to the last part of the new group of words.
  // * Remove from the first part of the group of words all words added to the last part of the group of words.
  // * Update layout info.

  //early returns
  if( ( 0 == indices.mWordIndex ) && ( 0 == indices.mCharacterIndex ) )
  {
    // the whole group of words goes to the last part of the group.
    lastWordGroupLayoutInfo = firstWordGroupLayoutInfo;

    firstWordGroupLayoutInfo = WordGroupLayoutInfo();

    return;
  }

  if( !firstWordGroupLayoutInfo.mWordsLayoutInfo.empty() )
  {
    const std::size_t numberOfWords = firstWordGroupLayoutInfo.mWordsLayoutInfo.size();
    if( indices.mWordIndex == numberOfWords - 1 )
    {
      const WordLayoutInfo& word( *( firstWordGroupLayoutInfo.mWordsLayoutInfo.end() - 1 ) );
      if( indices.mCharacterIndex == word.mCharactersLayoutInfo.size() )
      {
        // the whole group of words goes to the first part.

        // Just delete whatever there is in the last part of the group of words.
        lastWordGroupLayoutInfo = WordGroupLayoutInfo();

        return;
      }
    }
  }

  lastWordGroupLayoutInfo = WordGroupLayoutInfo();

  // 1) Split the word within the group of words to be split.
  WordLayoutInfo& firstWordLayoutInfo( *( firstWordGroupLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex ) );
  WordLayoutInfo lastWordLayoutInfo;

  SplitWord( indices.mCharacterIndex,
             firstWordLayoutInfo,
             lastWordLayoutInfo );

  // 2) Add last part of the word to the new group of words.
  if( !lastWordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    lastWordGroupLayoutInfo.mWordsLayoutInfo.push_back( lastWordLayoutInfo );
  }

  // 3) Add words from word-position + 1 to the end.
  lastWordGroupLayoutInfo.mWordsLayoutInfo.insert( lastWordGroupLayoutInfo.mWordsLayoutInfo.end(),
                                                   firstWordGroupLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex + 1, firstWordGroupLayoutInfo.mWordsLayoutInfo.end() );

  // 4) update layout info of the last group of words.
  lastWordGroupLayoutInfo.mDirection = firstWordGroupLayoutInfo.mDirection;

  for( WordLayoutInfoContainer::iterator it = lastWordGroupLayoutInfo.mWordsLayoutInfo.begin(), endIt = lastWordGroupLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& layoutInfo( *it );

    UpdateSize( lastWordGroupLayoutInfo.mSize, layoutInfo.mSize );
    lastWordGroupLayoutInfo.mNumberOfCharacters += layoutInfo.mCharactersLayoutInfo.size();
    lastWordGroupLayoutInfo.mAscender = std::max( lastWordGroupLayoutInfo.mAscender, layoutInfo.mAscender );
  }

  // 5) Remove words added to the last part of the group of words from the first group of words.

  // if the number of characters of the last word of the first group is zero, it should be removed.
  const std::size_t index = ( firstWordLayoutInfo.mCharactersLayoutInfo.empty() ? indices.mWordIndex : indices.mWordIndex + 1 );

  firstWordGroupLayoutInfo.mWordsLayoutInfo.erase( firstWordGroupLayoutInfo.mWordsLayoutInfo.begin() + index, firstWordGroupLayoutInfo.mWordsLayoutInfo.end() );

  // 6) update layout info of the first group of words.
  firstWordGroupLayoutInfo.mSize = Size();
  firstWordGroupLayoutInfo.mAscender = 0.f;
  firstWordGroupLayoutInfo.mNumberOfCharacters = 0;
  for( WordLayoutInfoContainer::iterator it = firstWordGroupLayoutInfo.mWordsLayoutInfo.begin(), endIt = firstWordGroupLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& layoutInfo( *it );

    UpdateSize( firstWordGroupLayoutInfo.mSize, layoutInfo.mSize );
    firstWordGroupLayoutInfo.mNumberOfCharacters += layoutInfo.mCharactersLayoutInfo.size();
    firstWordGroupLayoutInfo.mAscender = std::max( firstWordGroupLayoutInfo.mAscender, layoutInfo.mAscender );
  }
}

void MergeWordGroup( WordGroupLayoutInfo& firstWordGroupLayoutInfo,
                     const WordGroupLayoutInfo& lastWordGroupLayoutInfo )
{
  // Merges two given groups of words.
  //
  // Can't merge two groups if they have text with different directions (RTL , LTR )
  // or if the last word of the first one is a line separator (new line character)

  // Early returns

  if( lastWordGroupLayoutInfo.mWordsLayoutInfo.empty() )
  {
    // Nothing to merge if last group is empty.
    return;
  }

  if( firstWordGroupLayoutInfo.mWordsLayoutInfo.empty() )
  {
    // If first group is empty, just copy the last group to the first one.
    firstWordGroupLayoutInfo = lastWordGroupLayoutInfo;

    return;
  }

  // Check both groups have the same direction.
  if( firstWordGroupLayoutInfo.mDirection != lastWordGroupLayoutInfo.mDirection )
  {
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeWordGroup(). ERROR: groups with different direction can't be merged." );
  }

  // Check first group doesn't finish with a new line character.
  WordLayoutInfo& lastWordLayout( *( firstWordGroupLayoutInfo.mWordsLayoutInfo.end() - 1 ) );
  if( LineSeparator == lastWordLayout.mType )
  {
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeWordGroup(). ERROR: A group of words can't be merged to another group which finishes with a new line character." );
  }

  // If the las word of the first group or the first word of the last group is a white space, both groups can be concatenated.
  // Otherwise both words need to be merged first.
  const WordLayoutInfo& firstWordLayout( *lastWordGroupLayoutInfo.mWordsLayoutInfo.begin() );

  std::size_t index = 0;
  if( ( WordSeparator != lastWordLayout.mType ) && ( WordSeparator != firstWordLayout.mType ) && ( LineSeparator != firstWordLayout.mType ) )
  {
    // Last word of the first group is not a word separator and first word of the last group is not a word or line separator.
    // Words need to be merged.

    MergeWord( lastWordLayout,
               firstWordLayout );

    // After merging two words, the rest of the words need to be added.
    ++index; // By increasing this index the word already merged won't be added again.
  }

  // Merge layout info
  firstWordGroupLayoutInfo.mWordsLayoutInfo.insert( firstWordGroupLayoutInfo.mWordsLayoutInfo.end(),
                                                    lastWordGroupLayoutInfo.mWordsLayoutInfo.begin() + index, lastWordGroupLayoutInfo.mWordsLayoutInfo.end() );
  UpdateSize( firstWordGroupLayoutInfo.mSize, lastWordGroupLayoutInfo.mSize );
  firstWordGroupLayoutInfo.mAscender = std::max( firstWordGroupLayoutInfo.mAscender, lastWordGroupLayoutInfo.mAscender );
  firstWordGroupLayoutInfo.mNumberOfCharacters += lastWordGroupLayoutInfo.mNumberOfCharacters;
}

void CollectTextActorsFromGroups( std::vector<TextActor>& textActors, const LineLayoutInfo& line, const std::size_t groupIndexBegin, const std::size_t groupIndexEnd )
{
  for( WordGroupLayoutInfoContainer::const_iterator groupIt = line.mWordGroupsLayoutInfo.begin() + groupIndexBegin, groupEndIt = line.mWordGroupsLayoutInfo.begin() + groupIndexEnd;
       groupIt != groupEndIt;
       ++groupIt )
  {
    const WordGroupLayoutInfo& group( *groupIt );

    CollectTextActorsFromWords( textActors, group, 0, group.mWordsLayoutInfo.size() );
  }
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
