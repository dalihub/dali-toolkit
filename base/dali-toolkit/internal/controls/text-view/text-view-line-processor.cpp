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
#include "text-view-line-processor.h"
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

LineLayoutInfo::LineLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mLineHeightOffset( 0.f ),
  mWordGroupsLayoutInfo(),
  mNumberOfCharacters( 0 )
{
}

LineLayoutInfo::LineLayoutInfo( const LineLayoutInfo& line )
: mSize( line.mSize ),
  mAscender( line.mAscender ),
  mLineHeightOffset( line.mLineHeightOffset ),
  mWordGroupsLayoutInfo( line.mWordGroupsLayoutInfo ),
  mNumberOfCharacters( line.mNumberOfCharacters )
{
}

LineLayoutInfo& LineLayoutInfo::operator=( const LineLayoutInfo& line )
{
  mSize = line.mSize;
  mAscender = line.mAscender;
  mLineHeightOffset = line.mLineHeightOffset;
  mWordGroupsLayoutInfo = line.mWordGroupsLayoutInfo;
  mNumberOfCharacters = line.mNumberOfCharacters;

  return *this;
}

void UpdateLineLayoutInfo( TextViewProcessor::LineLayoutInfo& lineLayoutInfo, const float lineHeightOffset )
{
  lineLayoutInfo.mSize = Size();

  for( WordGroupLayoutInfoContainer::iterator it = lineLayoutInfo.mWordGroupsLayoutInfo.begin(), endIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordGroupLayoutInfo& layoutInfo( *it );

    UpdateSize( lineLayoutInfo.mSize, layoutInfo.mSize );
  }
  lineLayoutInfo.mSize.height += lineHeightOffset;
}

void CreateLineInfo( const MarkupProcessor::StyledTextArray& line,
                     TextView::RelayoutData& relayoutData,
                     TextViewProcessor::LineLayoutInfo& lineLayoutInfo )
{
  // TODO: Split the line in group of words. Each group of words has only left to right characters or right to left characters but not a mix of both.
  // TODO: set the wordgroup direction (LTR or RTL)
  std::vector<MarkupProcessor::StyledTextArray> wordGroups;
  if( TextProcessor::ContainsRightToLeftCharacter( line ) )
  {
    // If the text is bidirectional, the characters will be converted and reordered
    // as specified by the Unicode Bidirectional Algorithm.

    // Reorders the line and converts arabic glyphs (if any).
    // It also split words in different groups if there are a mix of left to right
    // and right to left text.
    // If the whole line is left to right or right to left all words are grouped in the same group.
    TextProcessor::ConvertBidirectionalText( line,
                                             wordGroups,
                                             relayoutData.mCharacterLogicalToVisualMap,
                                             relayoutData.mCharacterVisualToLogicalMap);
  }
  else
  {
    // No bidirectional text to process.

    if( !line.empty() )
    {
      // Add all words in a group.
      wordGroups.push_back( line );

      // Create trivial bidirectional map tables.
      std::size_t index = 0;
      for( MarkupProcessor::StyledTextArray::const_iterator it = line.begin(), endIt = line.end(); it != endIt; ++it )
      {
        const MarkupProcessor::StyledText& styledText( *it );

        for( std::size_t i = 0, length = styledText.mText.GetLength(); i < length; ++i )
        {
          relayoutData.mCharacterLogicalToVisualMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
          relayoutData.mCharacterVisualToLogicalMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
          ++index;
        }
      }
    }
  }

  // Traverse all group of words.
  for( std::vector<MarkupProcessor::StyledTextArray>::const_iterator groupIt = wordGroups.begin(), groupEndIt = wordGroups.end(); groupIt != groupEndIt; ++groupIt )
  {
    const MarkupProcessor::StyledTextArray& wordGroup( *groupIt );

    // Data structures for the new group of words.
    WordGroupLayoutInfo wordGroupLayoutInfo;

    CreateWordGroupInfo( wordGroup,
                         relayoutData.mTextLayoutInfo,
                         wordGroupLayoutInfo );

    // Update layout info for the current line.
    lineLayoutInfo.mAscender = std::max( lineLayoutInfo.mAscender, wordGroupLayoutInfo.mAscender );
    lineLayoutInfo.mNumberOfCharacters += wordGroupLayoutInfo.mNumberOfCharacters;
    UpdateSize( lineLayoutInfo.mSize, wordGroupLayoutInfo.mSize );

    // Add the group of words to the current line.
    lineLayoutInfo.mWordGroupsLayoutInfo.push_back( wordGroupLayoutInfo );
  } // end of group of words
}

void RemoveWordGroupsFromLine( const std::size_t groupIndex,
                               const std::size_t numberOfGroups,
                               const PointSize& lineHeightOffset,
                               LineLayoutInfo& lineLayout )
{
  // Removes groups of words from a line.

  // * Check if words or lines can be merged after removing a group of words or a line separator have to be done outside this method.

  // * Note: Currently it's only used to remove a number of groups of words from the beginning, or
  //         from groupIndex index to the end. This function doesn't merge groups of words (if a whole group is removed)
  //         TODO: merge groups of words if required.

  const std::size_t groupEndIndex = groupIndex + numberOfGroups;

  // Remove word groups from layout info.
  lineLayout.mWordGroupsLayoutInfo.erase( lineLayout.mWordGroupsLayoutInfo.begin() + groupIndex,
                                          lineLayout.mWordGroupsLayoutInfo.begin() + groupEndIndex );

  // Update layout info.
  lineLayout.mSize = Size();
  lineLayout.mAscender = 0.f;
  lineLayout.mNumberOfCharacters = 0;
  for( WordGroupLayoutInfoContainer::const_iterator it = lineLayout.mWordGroupsLayoutInfo.begin(), endIt = lineLayout.mWordGroupsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    const WordGroupLayoutInfo& group( *it );

    UpdateSize( lineLayout.mSize, group.mSize );
    lineLayout.mAscender = std::max( lineLayout.mAscender, group.mAscender );
    lineLayout.mNumberOfCharacters += group.mNumberOfCharacters;
  }
  lineLayout.mSize.height += lineHeightOffset;
  lineLayout.mLineHeightOffset = lineHeightOffset;
}

void SplitLine( const TextInfoIndices& indices,
                const PointSize& lineHeightOffset,
                LineLayoutInfo& firstLineLayoutInfo,
                LineLayoutInfo& lastLineLayoutInfo )
{
  // Splits a line in two.
  // A group of words and a word may be split in two as well.

  // * Split the group of words within the line.
  // * Add last part of the group of words to the new line.
  // * Add groups of words from groupPosition + 1 to the end.
  // * Update layout info of the last line.
  // * Remove groups of words added to the last part of the line from the first line.

  // early returns!!
  if( ( 0 == indices.mGroupIndex ) && ( 0 == indices.mWordIndex ) && ( 0 == indices.mCharacterIndex ) )
  {
    // the whole line goes to the last part.
    lastLineLayoutInfo = firstLineLayoutInfo;

    firstLineLayoutInfo = LineLayoutInfo();

    return;
  }

  if( !firstLineLayoutInfo.mWordGroupsLayoutInfo.empty() )
  {
    const std::size_t numberOfGroups = firstLineLayoutInfo.mWordGroupsLayoutInfo.size();
    if( indices.mGroupIndex == numberOfGroups - 1 )
    {
      const WordGroupLayoutInfo& group( *( firstLineLayoutInfo.mWordGroupsLayoutInfo.end() - 1 ) );

      const std::size_t numberOfWords = group.mWordsLayoutInfo.size();
      if( indices.mWordIndex == numberOfWords - 1 )
      {
        const WordLayoutInfo& word( *( group.mWordsLayoutInfo.end() - 1 ) );
        if( indices.mCharacterIndex == word.mCharactersLayoutInfo.size() )
        {
          // the whole line goes to the first part.

          // Just delete whatever there is in the last part of the line.
          lastLineLayoutInfo = LineLayoutInfo();

          return;
        }
      }
    }
  }

  lastLineLayoutInfo = LineLayoutInfo();

  // 1) Split the group of words whitin the line.
  WordGroupLayoutInfo& firstWordGroupLayoutInfo( *( firstLineLayoutInfo.mWordGroupsLayoutInfo.begin() + indices.mGroupIndex ) );
  WordGroupLayoutInfo lastWordGroupLayoutInfo;

  SplitWordGroup( indices,
                  firstWordGroupLayoutInfo,
                  lastWordGroupLayoutInfo );

  // 2) Add last part of the group of words to the new line.
  if( !lastWordGroupLayoutInfo.mWordsLayoutInfo.empty() )
  {
    lastLineLayoutInfo.mWordGroupsLayoutInfo.push_back( lastWordGroupLayoutInfo );
  }

  // 3) Add groups from group-position + 1 to the end.
  lastLineLayoutInfo.mWordGroupsLayoutInfo.insert( lastLineLayoutInfo.mWordGroupsLayoutInfo.end(),
                                                   firstLineLayoutInfo.mWordGroupsLayoutInfo.begin() + indices.mGroupIndex + 1, firstLineLayoutInfo.mWordGroupsLayoutInfo.end() );

  // 4) update layout info of the last line.
  for( WordGroupLayoutInfoContainer::iterator it = lastLineLayoutInfo.mWordGroupsLayoutInfo.begin(), endIt = lastLineLayoutInfo.mWordGroupsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordGroupLayoutInfo& layoutInfo( *it );

    lastLineLayoutInfo.mNumberOfCharacters += layoutInfo.mNumberOfCharacters;
    UpdateSize( lastLineLayoutInfo.mSize, layoutInfo.mSize );
    lastLineLayoutInfo.mAscender = std::max( lastLineLayoutInfo.mAscender, layoutInfo.mAscender );
  }
  lastLineLayoutInfo.mSize.height += lineHeightOffset;
  lastLineLayoutInfo.mLineHeightOffset = lineHeightOffset;

  // 5) Remove groups of words added to the last part of the line from the first line.

  // if the number of characters of the last group of words of the first line is zero, it should be removed.
  const std::size_t index = ( 0 == firstWordGroupLayoutInfo.mNumberOfCharacters ? indices.mGroupIndex : indices.mGroupIndex + 1 );

  firstLineLayoutInfo.mWordGroupsLayoutInfo.erase( firstLineLayoutInfo.mWordGroupsLayoutInfo.begin() + index, firstLineLayoutInfo.mWordGroupsLayoutInfo.end() );

  // 6) update layout info of the first line.
  firstLineLayoutInfo.mNumberOfCharacters = 0;
  firstLineLayoutInfo.mSize = Size();
  firstLineLayoutInfo.mAscender = 0.f;
  for( WordGroupLayoutInfoContainer::iterator it = firstLineLayoutInfo.mWordGroupsLayoutInfo.begin(), endIt = firstLineLayoutInfo.mWordGroupsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordGroupLayoutInfo& layoutInfo( *it );

    firstLineLayoutInfo.mNumberOfCharacters += layoutInfo.mNumberOfCharacters;
    UpdateSize( firstLineLayoutInfo.mSize, layoutInfo.mSize );
    firstLineLayoutInfo.mAscender = std::max( firstLineLayoutInfo.mAscender, layoutInfo.mAscender );
  }
  firstLineLayoutInfo.mSize.height += lineHeightOffset;
  firstLineLayoutInfo.mLineHeightOffset = lineHeightOffset;
}

void MergeLine( LineLayoutInfo& firstLineLineLayoutInfo,
                const LineLayoutInfo& lastLineLayoutInfo )
{
  // Merges two given lines.
  //
  // Can't merge two lines if the last word of the first one is a line separator (new line character)

  // Early returns.

  if( lastLineLayoutInfo.mWordGroupsLayoutInfo.empty() )
  {
    // Nothing to merge if last line is empty.
    return;
  }

  if( firstLineLineLayoutInfo.mWordGroupsLayoutInfo.empty() )
  {
    // If first line is empty, just copy the last line to the first one.
    firstLineLineLayoutInfo = lastLineLayoutInfo;

    return;
  }

  if( 1 == firstLineLineLayoutInfo.mWordGroupsLayoutInfo.size() )
  {
    WordGroupLayoutInfo& wordGroupLayout( *firstLineLineLayoutInfo.mWordGroupsLayoutInfo.begin() );

    if( wordGroupLayout.mWordsLayoutInfo.empty() )
    {
      // If first line is empty, just copy the last line to the first one.
      firstLineLineLayoutInfo = lastLineLayoutInfo;

      return;
    }
  }

  // Check the last word of the last group of the first line doesn't finish with a new line character.
  WordGroupLayoutInfo& lastWordGroupLayout( *( firstLineLineLayoutInfo.mWordGroupsLayoutInfo.end() - 1 ) );
  WordLayoutInfo& lastWordLayout( *( lastWordGroupLayout.mWordsLayoutInfo.end() - 1 ) );
  if( LineSeparator == lastWordLayout.mType )
  {
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeLine(). ERROR: A line can't be merged to another line which finishes with a new line character." );
  }

  // If the last group of the first line has the same direction than the first group of the last line, both lines can be concatenated.
  // Otherwise, both groups have to be merged first.
  const WordGroupLayoutInfo& firstWordGroupLayout( *lastLineLayoutInfo.mWordGroupsLayoutInfo.begin() );

  std::size_t index = 0;
  if( lastWordGroupLayout.mDirection == firstWordGroupLayout.mDirection )
  {
    // Both groups of words have the same direction. They need to be merged.
    MergeWordGroup( lastWordGroupLayout,
                    firstWordGroupLayout );

    // After merging two groups of words, the rest of groups need to be added.
    ++index; // By increasing this index the group of words already merged won't be added again.
  }

  // Merge layout info
  firstLineLineLayoutInfo.mWordGroupsLayoutInfo.insert( firstLineLineLayoutInfo.mWordGroupsLayoutInfo.end(),
                                                        lastLineLayoutInfo.mWordGroupsLayoutInfo.begin() + index, lastLineLayoutInfo.mWordGroupsLayoutInfo.end() );
  UpdateSize( firstLineLineLayoutInfo.mSize, lastLineLayoutInfo.mSize );
  firstLineLineLayoutInfo.mAscender = std::max( firstLineLineLayoutInfo.mAscender, lastLineLayoutInfo.mAscender );
  firstLineLineLayoutInfo.mLineHeightOffset = std::max( firstLineLineLayoutInfo.mLineHeightOffset, lastLineLayoutInfo.mLineHeightOffset );
  firstLineLineLayoutInfo.mNumberOfCharacters += lastLineLayoutInfo.mNumberOfCharacters;

}

WordLayoutInfo GetLastWordLayoutInfo( const LineLayoutInfo& lineLayoutInfo )
{
  WordLayoutInfo layoutInfo;

  if( !lineLayoutInfo.mWordGroupsLayoutInfo.empty() )
  {
    const WordGroupLayoutInfo& groupInfo( *( lineLayoutInfo.mWordGroupsLayoutInfo.end() - 1 ) );

    if( !groupInfo.mWordsLayoutInfo.empty() )
    {
      layoutInfo = *( groupInfo.mWordsLayoutInfo.end() - 1 );
    }
  }

  return layoutInfo;
}

CharacterLayoutInfo GetFirstCharacterLayoutInfo( const LineLayoutInfo& lineLayoutInfo )
{
  CharacterLayoutInfo layoutInfo;

  if( !lineLayoutInfo.mWordGroupsLayoutInfo.empty() )
  {
    const WordGroupLayoutInfo& groupInfo( *lineLayoutInfo.mWordGroupsLayoutInfo.begin() );

    if( !groupInfo.mWordsLayoutInfo.empty() )
    {
      const WordLayoutInfo& wordInfo( *groupInfo.mWordsLayoutInfo.begin() );

      layoutInfo = GetFirstCharacterLayoutInfo( wordInfo );
    }
  }

  return layoutInfo;
}

CharacterLayoutInfo GetLastCharacterLayoutInfo( const LineLayoutInfo& lineLayoutInfo )
{
  const WordLayoutInfo wordInfo = GetLastWordLayoutInfo( lineLayoutInfo );

  return GetLastCharacterLayoutInfo( wordInfo );
}

void CollectTextActorsFromLines( std::vector<TextActor>& textActors, const TextLayoutInfo& textLayoutInfo, const std::size_t lineIndexBegin, const std::size_t lineIndexEnd )
{
  for( LineLayoutInfoContainer::const_iterator lineIt = textLayoutInfo.mLinesLayoutInfo.begin() + lineIndexBegin, lineEndIt = textLayoutInfo.mLinesLayoutInfo.begin() + lineIndexEnd;
       lineIt != lineEndIt;
       ++lineIt )
  {
    const LineLayoutInfo& line( *lineIt );

    CollectTextActorsFromGroups( textActors, line, 0, line.mWordGroupsLayoutInfo.size() );
  }
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
