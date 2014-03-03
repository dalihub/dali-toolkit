//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include "text-view-processor.h"
#include "text-view-word-processor.h"
#include "text-view-word-group-processor.h"
#include "text-view-line-processor.h"
#include "text-view-processor-helper-functions.h"
#include "text-processor.h"
#include "text-view-processor-dbg.h"

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
/**
 * Update text layout info.
 *
 * Updates the size of the whole text, the maximum width of all words and the number of characters.
 *
 * @param[in,out] textLayoutInfo
 */
void UpdateTextLayoutInfo( TextLayoutInfo& textLayoutInfo )
{
  // Initialize members to be updated.
  textLayoutInfo.mWholeTextSize = Size();
  textLayoutInfo.mMaxWordWidth = 0.f;
  textLayoutInfo.mNumberOfCharacters = 0;

  // Traverse all text updating values.
  for( LineLayoutInfoContainer::const_iterator lineIt = textLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = textLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt )
  {
    const LineLayoutInfo& line( *lineIt );

    // Updates text size with the size of all lines.
    UpdateSize( textLayoutInfo.mWholeTextSize, line.mSize, GrowHeight );

    // Updates number of characters.
    textLayoutInfo.mNumberOfCharacters += line.mNumberOfCharacters;

    // Updates the max word's width.
    for( WordGroupLayoutInfoContainer::const_iterator groupIt = line.mWordGroupsLayoutInfo.begin(), groupEndIt = line.mWordGroupsLayoutInfo.end();
         groupIt != groupEndIt;
         ++groupIt )
    {
      const WordGroupLayoutInfo& group( *groupIt );
      for( WordLayoutInfoContainer::const_iterator wordIt = group.mWordsLayoutInfo.begin(), wordEndIt = group.mWordsLayoutInfo.end();
           wordIt != wordEndIt;
           ++wordIt )
      {
        const WordLayoutInfo& word( *wordIt );

        textLayoutInfo.mMaxWordWidth = std::max( textLayoutInfo.mMaxWordWidth, word.mSize.width );
      }
    }
  }
}

} // namespace

// Constructors and assignment operators

TextInfoIndices::TextInfoIndices()
: mLineIndex( 0 ),
  mGroupIndex( 0 ),
  mWordIndex( 0 ),
  mCharacterIndex( 0 )
{
}

TextInfoIndices::TextInfoIndices( const std::size_t lineIndex,
                                  const std::size_t groupIndex,
                                  const std::size_t wordIndex,
                                  const std::size_t characterIndex )
: mLineIndex( lineIndex ),
  mGroupIndex( groupIndex ),
  mWordIndex( wordIndex ),
  mCharacterIndex( characterIndex )
{
}

bool TextInfoIndices::operator==( const TextInfoIndices& indices ) const
{
  return ( ( mLineIndex == indices.mLineIndex ) &&
           ( mGroupIndex == indices.mGroupIndex ) &&
           ( mWordIndex == indices.mWordIndex ) &&
           ( mCharacterIndex == indices.mCharacterIndex ) );
}

/////////////////////
// Layout info.
/////////////////////

TextLayoutInfo::TextLayoutInfo()
: mWholeTextSize(),
  mMaxWordWidth( 0.f ),
  mLinesLayoutInfo(),
  mNumberOfCharacters( 0 ),
  mMaxItalicsOffset( 0.f ),
  mEllipsizeLayoutInfo()
{
}

TextLayoutInfo::TextLayoutInfo( const TextLayoutInfo& text )
: mWholeTextSize( text.mWholeTextSize ),
  mMaxWordWidth( text.mMaxWordWidth ),
  mLinesLayoutInfo( text.mLinesLayoutInfo ),
  mNumberOfCharacters( text.mNumberOfCharacters ),
  mMaxItalicsOffset( text.mMaxItalicsOffset ),
  mEllipsizeLayoutInfo( text.mEllipsizeLayoutInfo )
{
}

TextLayoutInfo& TextLayoutInfo::operator=( const TextLayoutInfo& text )
{
  mWholeTextSize = text.mWholeTextSize;
  mMaxWordWidth = text.mMaxWordWidth;
  mLinesLayoutInfo = text.mLinesLayoutInfo;
  mNumberOfCharacters = text.mNumberOfCharacters;
  mMaxItalicsOffset = text.mMaxItalicsOffset;
  mEllipsizeLayoutInfo = text.mEllipsizeLayoutInfo;

  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CreateTextInfo( const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData )
{
  // * Traverse the given text spliting it in lines, each line in groups of words and each group of words in words.
  // * If possible, it joins characters with same style in one text-actor.
  // * White spaces and new line characters are alone in one word.
  // * Bidirectional text is processed in each line.
  // * A group of words contains text in only one direction (Left to Right or Right to Left but not a mix of both).
  // * Generates a layout data structure to store layout information (size, position, ascender, text direction, etc) and metrics of all characters.
  // * Generates a text-actor data structure to store text, style and text-actors.
  // TODO: finish and test the bidirectional implementation.

  // Collect previously created text-actors.
  std::vector<TextActor> textActors;
  CollectTextActorsFromLines( textActors, relayoutData.mTextLayoutInfo, 0, relayoutData.mTextLayoutInfo.mLinesLayoutInfo.size() );

  if( !textActors.empty() )
  {
    // Add text-actors to the cache.
    relayoutData.mTextActorCache.InsertTextActors( textActors );
    relayoutData.mTextActorCache.ClearTexts();
  }

  // Store the ellipsize layout info before clearing the previous created info.
  const WordLayoutInfo ellipsizeInfo = relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo;

  // clear previous created info.
  relayoutData.mTextLayoutInfo = TextLayoutInfo();
  relayoutData.mCharacterLogicalToVisualMap.clear();
  relayoutData.mCharacterVisualToLogicalMap.clear();

  // Sets the ellipsize layout info.
  relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo = ellipsizeInfo;

  // Split the whole text in lines.
  std::vector<MarkupProcessor::StyledTextArray> lines;
  TextProcessor::SplitInLines( text,
                               lines );

  // Traverse all lines
  for( std::vector<MarkupProcessor::StyledTextArray>::const_iterator lineIt = lines.begin(), lineEndIt = lines.end(); lineIt != lineEndIt; ++lineIt )
  {
    const MarkupProcessor::StyledTextArray& line( *lineIt );

    // Data structures for the new line
    LineLayoutInfo lineLayoutInfo;

    // Fills the line data structures with the layout info.
    CreateLineInfo( line,
                    relayoutData,
                    lineLayoutInfo );

    if( 0 < lineLayoutInfo.mNumberOfCharacters )
    {
      // do not add the line offset if the line has no characters.
      lineLayoutInfo.mSize.height += layoutParameters.mLineHeightOffset;
      lineLayoutInfo.mLineHeightOffset = layoutParameters.mLineHeightOffset;
    }
    else
    {
      // line height needs to be added for the last line.

      float lineHeight = 0.f;
      // Get the last character of the last line.
      if( !relayoutData.mTextLayoutInfo.mLinesLayoutInfo.empty() )
      {
        const LineLayoutInfo& lineInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1 ) );

        const CharacterLayoutInfo characterInfo = GetLastCharacterLayoutInfo( lineInfo );

        lineHeight = characterInfo.mSize.height;
      }

      lineLayoutInfo.mSize.height = lineHeight;
    }

    // Update layout info for the whole text.
    UpdateSize( relayoutData.mTextLayoutInfo.mWholeTextSize, lineLayoutInfo.mSize, GrowHeight );
    relayoutData.mTextLayoutInfo.mNumberOfCharacters += lineLayoutInfo.mNumberOfCharacters;

    // Add the line to the current text.
    relayoutData.mTextLayoutInfo.mLinesLayoutInfo.push_back( lineLayoutInfo );
  } // end of lines
}

void UpdateTextInfo( const std::size_t position,
                     const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData )
{
  // Update current internal data structure with added text.

  // * Creates layout info for the given text.
  // * With the given position, find where to add the text.
  // * If the new text is not added at the end of current text, a line need to be split.
  // * Merge the last line of the new text to the last part or the split line.
  // * Add lines between first and last of the new text.
  // * Merge the first part of the split line with the first line of the new text.
  // * Update layout info and create new text actors if needed.

  // Early returns:

  if( text.empty() )
  {
    // nothing to do if the input text is empty.
    return;
  }

  if( 0 == relayoutData.mTextLayoutInfo.mNumberOfCharacters )
  {
    // Current text is empty. There is no need to update current data structure,
    // just create a new one with the new input text.
    CreateTextInfo( text,
                    layoutParameters,
                    relayoutData );
    return;
  }

  // initial checks.
  if( position > relayoutData.mTextLayoutInfo.mNumberOfCharacters )
  {
    // Asserts if text is to be added out of bounds.
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::UpdateTextInfo (insert). Trying to insert text out of bounds." );
  }

  TextView::RelayoutData relayoutDataForNewText;

  // Creates layout info for the given text.
  // It doesn't create text-actors as text could be added to an existing one.
  CreateTextInfo( text,
                  layoutParameters,
                  relayoutDataForNewText );

  // Update logical-to-visual and visual-to-logical tables.
  // TODO: check that for mixed RTL/LTR text.
  std::size_t index = 0;
  for( std::size_t i = 0; i < relayoutDataForNewText.mTextLayoutInfo.mNumberOfCharacters; ++i )
  {
    relayoutData.mCharacterLogicalToVisualMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
    relayoutData.mCharacterVisualToLogicalMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
    ++index;
  }

  // If a line is split, it stores the last part of the line.
  LineLayoutInfo lastLineLayoutInfo;

  // Stores indices to the line, group of words, word and character of the given position.
  TextInfoIndices textInfoIndices;

  if( position < relayoutData.mTextLayoutInfo.mNumberOfCharacters )
  {
    // Get line, group, word and character indices for given position.
    GetIndicesFromGlobalCharacterIndex( position,
                                        relayoutData.mTextLayoutInfo,
                                        textInfoIndices );

    // 1) Split the line

    // Split a line in two is needed, then merge the first part of the split line
    // with the first line of the new text, add subsequent lines and merge the last line
    // of the new text with the last part of the split line.

    // Implementation notes!
    //
    // These references to the first line are declared in this scope because if new lines are inserted in step 2,
    // they become invalid, making the algorithm to crash if used again.
    // In the step 3, references to the first line are needed and declared again.

    // Stores the first part of the split line.
    LineLayoutInfo& firstLineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndices.mLineIndex ) );

    SplitLine( textInfoIndices,
               PointSize( layoutParameters.mLineHeightOffset ),
               firstLineLayoutInfo,
               lastLineLayoutInfo );
  }
  else
  {
    // Position is just after the last character.
    // Calculates indices for that position.
    if( !relayoutData.mTextLayoutInfo.mLinesLayoutInfo.empty() )
    {
      textInfoIndices.mLineIndex = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.size() - 1;
      const LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1 ) );

      if( !lineLayoutInfo.mWordGroupsLayoutInfo.empty() )
      {
        textInfoIndices.mGroupIndex = lineLayoutInfo.mWordGroupsLayoutInfo.size() - 1;
        const WordGroupLayoutInfo& groupLayoutInfo( *( lineLayoutInfo.mWordGroupsLayoutInfo.end() - 1 ) );

        if( !groupLayoutInfo.mWordsLayoutInfo.empty() )
        {
          textInfoIndices.mWordIndex = groupLayoutInfo.mWordsLayoutInfo.size() - 1;

          const WordLayoutInfo& wordLayoutInfo( *( groupLayoutInfo.mWordsLayoutInfo.end() - 1 ) );
          textInfoIndices.mCharacterIndex = wordLayoutInfo.mCharactersLayoutInfo.size();
        }
      }
    }
  }

  // 2) If the new text has more than 1 line, merge the last line of the input text with the last part of the split line.
  //TODO check this cases ( num lines ==1, >1, >2 ) if it could be simplified.
  if( relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.size() > 1 )
  {
    LineLayoutInfo& lastInputLineLayoutInfo( *( relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.end() - 1 ) );

    MergeLine( lastInputLineLayoutInfo,
               lastLineLayoutInfo );

    if( relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.size() > 2 )
    {
      // Insert all lines except first and last in the text.
      relayoutData.mTextLayoutInfo.mLinesLayoutInfo.insert( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndices.mLineIndex + 1,
                                                            relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.begin() + 1,
                                                            relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.end() - 1 );
    }

    // Insert the last line to the text
    relayoutData.mTextLayoutInfo.mLinesLayoutInfo.insert( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndices.mLineIndex + relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.size() - 1,
                                                          lastInputLineLayoutInfo );
  }
  else
  {
    // Merge the new line to the last part of the split line.
    LineLayoutInfo& inputLineLayoutInfo( *relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.begin() );

    MergeLine( inputLineLayoutInfo,
               lastLineLayoutInfo );
  }

  // 3) Merge the first line of the split text with the first line of the input text.
  LineLayoutInfo& firstLineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndices.mLineIndex ) );
  LineLayoutInfo& firstInputLineLayoutInfo( *relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.begin() );

  MergeLine( firstLineLayoutInfo,
             firstInputLineLayoutInfo );

  // 4) Update text info.

  // Updates the whole text size, maximum word size, etc.
  UpdateTextLayoutInfo( relayoutData.mTextLayoutInfo );
}

void UpdateTextInfo( const std::size_t position,
                     const std::size_t numberOfCharacters,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData,
                     const TextOperationOnRemove clearText )
{
  // Removes 'numberOfCharacters' starting from 'position'.

  // * It checks if text to be deleted is in the same line or not:
  // *   If it is not, check which lines need to be split/merged or deleted.
  // *   If it is but all characters of the line are going to be deleted, just delete the line (nothing needs to be split/merged)
  // *   If only some characters of the same line are going to be deleted, proceed similarly: check if text to be deleted is in the same group of words.
  // *     If it is not, check which groups of words need to be split/merged or deleted. Two groups of words can't be merged if they contain text with different direction (Left to Right / Right to Left)
  // *     If it is but all characters of the group are going to be deleted, delete the group. TODO: Check if previous and following group need to be merged.
  // *     If only some characters of the same group of words need to be deleted, proceed similarly: check if text to be deleted is in the same word.
  // *       If it is not, split/merge words.
  // *       Check if the whole word needs to be deleted.
  // *       Check if only some characters of the word need to be deleted.
  // * Updates layout info.

  // * The algorithm checks if a word separator is deleted (in that case, different words need to be merged) and if a new line separator is deleted (two lines need to be merged).

  // Early return

  if( 0 == numberOfCharacters )
  {
    DALI_ASSERT_DEBUG( !"TextViewProcessor::UpdateTextInfo. WARNING: trying to delete 0 characters!" )

    // nothing to do if no characters are deleted.
    return;
  }

  // Asserts if trying to delete text out of bounds.
  DALI_ASSERT_ALWAYS( position + numberOfCharacters <= relayoutData.mTextLayoutInfo.mNumberOfCharacters && "TextViewProcessor::UpdateTextInfo. ERROR: trying to delete characters out of boundary" );

  // Remove characters from character to visual map and vs //TODO: check this for RTL text!!
  relayoutData.mCharacterLogicalToVisualMap.erase( relayoutData.mCharacterLogicalToVisualMap.end() - numberOfCharacters, relayoutData.mCharacterLogicalToVisualMap.end() );
  relayoutData.mCharacterVisualToLogicalMap.erase( relayoutData.mCharacterVisualToLogicalMap.end() - numberOfCharacters, relayoutData.mCharacterVisualToLogicalMap.end() );

  // Get line, group of words, word and character indices for the given start position.
  TextInfoIndices textInfoIndicesBegin;
  GetIndicesFromGlobalCharacterIndex( position,
                                      relayoutData.mTextLayoutInfo,
                                      textInfoIndicesBegin );

  // Get line, group of words, word and character indices for the given end position (start position + number of characters to be deleted).
  TextInfoIndices textInfoIndicesEnd;
  GetIndicesFromGlobalCharacterIndex( position + numberOfCharacters - 1,
                                      relayoutData.mTextLayoutInfo,
                                      textInfoIndicesEnd );

  // Vectors used to temporary store text-actors removed from text.
  // Three vectors are needed because text-actors are not removed in order
  // but insert them in order is required to reuse them later.
  std::vector<TextActor> removedTextActorsFromBegin;
  std::vector<TextActor> removedTextActorsFromMid;
  std::vector<TextActor> removedTextActorsFromEnd;

  // Whether lines, group of words and words need to be merged.
  bool mergeLines = false;
  bool mergeGroups = false;
  bool mergeWords = false;

  // Indices of the lines, group of words and words to be merged.
  TextInfoIndices textInfoMergeIndicesBegin;
  TextInfoIndices textInfoMergeIndicesEnd;

  const LineLayoutInfo& lineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesBegin.mLineIndex ) ); // used to check the number of characters of the line
                                                                                                                                    // if all characters to be deleted are in the same line.
  if( textInfoIndicesBegin.mLineIndex < textInfoIndicesEnd.mLineIndex )
  {
    // Deleted text is from different lines. It may need to split two lines, and merge first part of the first one with last part of the last one.

    // whether first or last line need to be split and merged with the last part.
    bool mergeFirstLine = false;
    bool mergeLastLine = true;

    textInfoMergeIndicesBegin.mLineIndex = textInfoIndicesBegin.mLineIndex;
    textInfoMergeIndicesEnd.mLineIndex = textInfoIndicesEnd.mLineIndex;

    if( ( textInfoIndicesBegin.mGroupIndex > 0 ) || ( textInfoIndicesBegin.mWordIndex > 0 ) || ( textInfoIndicesBegin.mCharacterIndex > 0 ) )
    {
      // first character to be deleted is not the first one of the current line.
      ++textInfoIndicesBegin.mLineIndex; // won't delete current line

      // As some characters remain, this line could be merged with the last one.
      mergeFirstLine = true;
    }

    // Check if all characters of the last line are going to be deleted.
    bool wholeLinedeleted = false;
    const LineLayoutInfo& lastLineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesEnd.mLineIndex ) );
    if( textInfoIndicesEnd.mGroupIndex + 1 == lastLineLayout.mWordGroupsLayoutInfo.size() )
    {
      const WordGroupLayoutInfo& lastGroupLayout( *( lastLineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesEnd.mGroupIndex ) );
      if( textInfoIndicesEnd.mWordIndex + 1 == lastGroupLayout.mWordsLayoutInfo.size() )
      {
        const WordLayoutInfo& lastWordLayout( *( lastGroupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
        if( textInfoIndicesEnd.mCharacterIndex + 1 == lastWordLayout.mCharactersLayoutInfo.size() )
        {
          // All characters of the last line are going to be deleted.
          ++textInfoIndicesEnd.mLineIndex; // will delete the last line.

          // the whole last line is deleted. Need to check if the next line could be merged.
          mergeLastLine = false;
          wholeLinedeleted = true;
        }
      }
    }

    if( wholeLinedeleted )
    {
      // It means the whole last line is deleted completely.
      // It's needed to check if there is another line after that could be merged.
      if( textInfoIndicesEnd.mLineIndex < relayoutData.mTextLayoutInfo.mLinesLayoutInfo.size() )
      {
          mergeLastLine = true;

          // Point the first characters of the next line.
          textInfoIndicesEnd.mGroupIndex = 0;
          textInfoIndicesEnd.mWordIndex = 0;
          textInfoIndicesEnd.mCharacterIndex = 0;
          textInfoMergeIndicesEnd.mLineIndex = textInfoIndicesEnd.mLineIndex;
      }
    }

    // If some characters remain in the first and last line, they need to be merged.
    mergeLines = mergeFirstLine && mergeLastLine;

    if( mergeLines )
    {
      // last line is going to be merged with the first one, so is not needed.
      ++textInfoIndicesEnd.mLineIndex; // will delete the last line.
    }

    if( mergeFirstLine )
    {
      // Remove characters from the first line.

      // Vectors used to temporary store text-actors removed from the line.
      // Three vectors are needed because text-actors are not removed in order
      // but insert them in order is required to reuse them later.
      std::vector<TextActor> removedTextActorsFromFirstWord;
      std::vector<TextActor> removedTextActorsFromFirstGroup;
      std::vector<TextActor> removedTextActorsFromGroups;

      // As lineIndexBegin has been increased just to not to remove the line, decrease now is needed to access it.
      LineLayoutInfo& lineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesBegin.mLineIndex - 1 ) );

      if( textInfoIndicesBegin.mGroupIndex + 1 < lineLayout.mWordGroupsLayoutInfo.size() )
      {
        // Store text-actors before removing them.
        CollectTextActorsFromGroups( removedTextActorsFromGroups, lineLayout, textInfoIndicesBegin.mGroupIndex + 1, lineLayout.mWordGroupsLayoutInfo.size() );

        // Remove extra groups. If a line has left to right and right to left text, groups after current one could be removed.
        RemoveWordGroupsFromLine( textInfoIndicesBegin.mGroupIndex + 1,
                                  lineLayout.mWordGroupsLayoutInfo.size() - ( textInfoIndicesBegin.mGroupIndex + 1 ),
                                  PointSize( layoutParameters.mLineHeightOffset ),
                                  lineLayout );
      }

      WordGroupLayoutInfo& groupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesBegin.mGroupIndex ) );

      if( ( textInfoIndicesBegin.mWordIndex + 1 < groupLayout.mWordsLayoutInfo.size() ) || ( 0 == textInfoIndicesBegin.mCharacterIndex ) )
      {
        // Remove extra words within current group of words. (and current word if whole characters are removed)
        // 0 == characterIndexBegin means the whole word is deleted.
        const std::size_t wordIndex = ( ( 0 == textInfoIndicesBegin.mCharacterIndex ) ? textInfoIndicesBegin.mWordIndex : textInfoIndicesBegin.mWordIndex + 1 );

        // Store text-actors before removing them.
        CollectTextActorsFromWords( removedTextActorsFromFirstGroup, groupLayout, wordIndex, groupLayout.mWordsLayoutInfo.size() );

        const std::size_t groupNumberCharacters = groupLayout.mNumberOfCharacters;
        RemoveWordsFromWordGroup( wordIndex,
                                  groupLayout.mWordsLayoutInfo.size() - wordIndex,
                                  groupLayout );

        // discount the removed number of characters.
        lineLayout.mNumberOfCharacters -= ( groupNumberCharacters - groupLayout.mNumberOfCharacters );
      }

      if( ( textInfoIndicesBegin.mWordIndex < groupLayout.mWordsLayoutInfo.size() ) && ( textInfoIndicesBegin.mCharacterIndex > 0 ) )
      {
        // Only some characters of the word need to be removed.
        WordLayoutInfo& wordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

        // Store text-actors before removing them.
        CollectTextActors( removedTextActorsFromFirstWord, wordLayout, textInfoIndicesBegin.mCharacterIndex, wordLayout.mCharactersLayoutInfo.size() );

        const std::size_t wordNumberCharacters = wordLayout.mCharactersLayoutInfo.size();
        RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                                  wordLayout.mCharactersLayoutInfo.size() - textInfoIndicesBegin.mCharacterIndex,
                                  wordLayout );

        // discount the removed number of characters.
        const std::size_t removedNumberOfCharacters = ( wordNumberCharacters - wordLayout.mCharactersLayoutInfo.size() );
        groupLayout.mNumberOfCharacters -= removedNumberOfCharacters;
        lineLayout.mNumberOfCharacters -= removedNumberOfCharacters;
      }
      UpdateLineLayoutInfo( lineLayout, layoutParameters.mLineHeightOffset );

      // Insert the text-actors in order.
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromFirstWord.begin(), removedTextActorsFromFirstWord.end() );
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromFirstGroup.begin(), removedTextActorsFromFirstGroup.end() );
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromGroups.begin(), removedTextActorsFromGroups.end() );
    }

    if( mergeLastLine && !wholeLinedeleted )
    {
      // Some characters from the last line need to be removed.

      // Vectors used to temporary store text-actors removed from the group.
      // Three vectors are needed because text-actors are not removed in order
      // but insert them in order is required to reuse them later.
      std::vector<TextActor> removedTextActorsFromFirstWord;
      std::vector<TextActor> removedTextActorsFromFirstGroup;
      std::vector<TextActor> removedTextActorsFromGroups;

      // lineIndexEnd was increased to delete the last line if lines need to be merged.
      // To access now the last line we need to decrease the index.
      const std::size_t lineIndex = ( mergeLines ? textInfoIndicesEnd.mLineIndex - 1 : textInfoIndicesEnd.mLineIndex );

      // Get the last line.
      LineLayoutInfo& lineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + lineIndex ) );

      if( textInfoIndicesEnd.mGroupIndex > 0 )
      {
        // Store text-actors before removing them.
        CollectTextActorsFromGroups( removedTextActorsFromGroups, lineLayout, 0, textInfoIndicesEnd.mGroupIndex );

        // Remove extra groups from the beginning of the line to the current group of words.
        RemoveWordGroupsFromLine( 0,
                                  textInfoIndicesEnd.mGroupIndex,
                                  PointSize( layoutParameters.mLineHeightOffset ),
                                  lineLayout );
      }

      // The group of characters which contains the characters to be removed is now the first one.
      WordGroupLayoutInfo& groupLayout( *lineLayout.mWordGroupsLayoutInfo.begin() );

      // Check if is needed remove the whole word. (If the character index is pointing just after the end of the word)
      const WordLayoutInfo& wordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
      bool removeWholeWord = wordLayout.mCharactersLayoutInfo.size() == textInfoIndicesEnd.mCharacterIndex + 1;

      if( ( textInfoIndicesEnd.mWordIndex > 0 ) || ( removeWholeWord ) )
      {
        // Store text-actors before removing them.
        CollectTextActorsFromWords( removedTextActorsFromFirstGroup, groupLayout, 0, ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1 : textInfoIndicesEnd.mWordIndex );

        // Remove extra words. (and current word if whole characters are removed)
        const std::size_t groupNumberCharacters = groupLayout.mNumberOfCharacters;
        RemoveWordsFromWordGroup( 0,
                                  ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1 : textInfoIndicesEnd.mWordIndex,
                                  groupLayout );

        // discount the removed number of characters.
        lineLayout.mNumberOfCharacters -= ( groupNumberCharacters - groupLayout.mNumberOfCharacters );
      }

      if( !removeWholeWord )
      {
        // Only some characters of the word need to be deleted.

        // After removing all extra words. The word with the characters to be removed is the first one.
        WordLayoutInfo& wordLayout( *groupLayout.mWordsLayoutInfo.begin() );

        // Store text-actors before removing them.
        CollectTextActors( removedTextActorsFromFirstWord, wordLayout, 0, textInfoIndicesEnd.mCharacterIndex + 1 );

        const std::size_t wordNumberCharacters = wordLayout.mCharactersLayoutInfo.size();
        RemoveCharactersFromWord( 0,
                                  textInfoIndicesEnd.mCharacterIndex + 1,
                                  wordLayout );

        // discount the removed number of characters.
        const std::size_t removedNumberOfCharacters = ( wordNumberCharacters - wordLayout.mCharactersLayoutInfo.size() );
        groupLayout.mNumberOfCharacters -= removedNumberOfCharacters;
        lineLayout.mNumberOfCharacters -= removedNumberOfCharacters;
        UpdateGroupLayoutInfo( groupLayout );
      }
      UpdateLineLayoutInfo( lineLayout, layoutParameters.mLineHeightOffset );

      // Insert the text-actors in order.
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromFirstWord.begin(), removedTextActorsFromFirstWord.end() );
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromFirstGroup.begin(), removedTextActorsFromFirstGroup.end() );
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromGroups.begin(), removedTextActorsFromGroups.end() );
    }
  } // end delete text from different lines
  else if( ( textInfoIndicesBegin.mLineIndex == textInfoIndicesEnd.mLineIndex ) && ( lineLayout.mNumberOfCharacters == numberOfCharacters ) )
  {
    // the whole line needs to be deleted.
    ++textInfoIndicesEnd.mLineIndex; // will delete current line.
  }
  else
  {
    // deleted text is within the same line. (merge lines could be needed if the line separator character is deleted)

    // Line which contains the characters to be deleted.
    LineLayoutInfo& lineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesBegin.mLineIndex ) );

    const WordGroupLayoutInfo& groupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesBegin.mGroupIndex ) ); // used to check the number of characters of the group of words
                                                                                                                                // if all characters to be deleted are in the same group of words.
    if( textInfoIndicesBegin.mGroupIndex < textInfoIndicesEnd.mGroupIndex )
    {
      // Deleted text is from different group of words. The two different group of words may be merged if they have text with same direction.

      // whether first or last group of words need to be split and merged with the last part.
      bool splitFirstGroup = false;
      bool splitLastGroup = true;

      textInfoMergeIndicesBegin.mGroupIndex = textInfoIndicesBegin.mGroupIndex;
      textInfoMergeIndicesEnd.mGroupIndex = textInfoIndicesEnd.mGroupIndex;

      if( ( textInfoIndicesBegin.mWordIndex > 0 ) || ( textInfoIndicesBegin.mCharacterIndex > 0 ) )
      {
        // first character to be deleted is not the first one of the current group.
        ++textInfoIndicesBegin.mGroupIndex; // won't delete current group

        // As some characters remain, this group needs to be split and could be merged with the last one.
        splitFirstGroup = true;
      }

      // Check if all characters of the last group are going to be deleted.
      const WordGroupLayoutInfo& lastGroupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesEnd.mGroupIndex ) );
      if( textInfoIndicesEnd.mWordIndex + 1 == lastGroupLayout.mWordsLayoutInfo.size() )
      {
        const WordLayoutInfo& lastWordLayout( *( lastGroupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
        if( textInfoIndicesEnd.mCharacterIndex + 1 == lastWordLayout.mCharactersLayoutInfo.size() )
        {
          // All characters of the last group are going to be deleted.
          ++textInfoIndicesEnd.mGroupIndex; // will delete the last group.

          // The whole last group is deleted. No need to merge groups.
          splitLastGroup = false;
        }
      }

      // Only merge two groups if they are not deleted completely and they have same direction.
      mergeGroups = ( splitFirstGroup && splitLastGroup ) && ( groupLayout.mDirection == lastGroupLayout.mDirection );

      if( mergeGroups )
      {
        // last group is going to be merged.
        ++textInfoIndicesEnd.mGroupIndex; // will delete the last group.
      }

      if( splitFirstGroup )
      {
        // Remove characters from the first group.

        // As wordGroupIndexBegin has been increased just to not to remove the group of words, decrease now is needed to access it.
        WordGroupLayoutInfo& groupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesBegin.mGroupIndex - 1 ) );

        if( ( textInfoIndicesBegin.mWordIndex + 1 < groupLayout.mWordsLayoutInfo.size() ) || ( 0 == textInfoIndicesBegin.mCharacterIndex ) )
        {
          // Remove extra words within current group of words. (and current word if whole characters are removed)
          // 0 == characterIndexBegin means the whole word is deleted.
          const std::size_t wordIndex = ( ( 0 == textInfoIndicesBegin.mCharacterIndex ) ? textInfoIndicesBegin.mWordIndex : textInfoIndicesBegin.mWordIndex + 1 );

          // Store text-actors before removing them.
          CollectTextActorsFromWords( removedTextActorsFromBegin, groupLayout, wordIndex, groupLayout.mWordsLayoutInfo.size() );

          RemoveWordsFromWordGroup( wordIndex,
                                    groupLayout.mWordsLayoutInfo.size() - wordIndex,
                                    groupLayout );
        }

        if( ( textInfoIndicesBegin.mWordIndex < groupLayout.mWordsLayoutInfo.size() ) && ( textInfoIndicesBegin.mCharacterIndex > 0 ) )
        {
          // Only some characters of the word need to be removed.
          WordLayoutInfo& wordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

          // Store text-actors before removing them.
          CollectTextActors( removedTextActorsFromBegin, wordLayout, textInfoIndicesBegin.mCharacterIndex, wordLayout.mCharactersLayoutInfo.size() );

          RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                                    wordLayout.mCharactersLayoutInfo.size() - textInfoIndicesBegin.mCharacterIndex,
                                    wordLayout );
        }
      }

      if( splitLastGroup )
      {
        // Some characters from the last group of words need to be removed.

        // textInfoIndicesEnd.mGroupIndex was increased to delete the last group of words if groups need to be merged.
        // To access now the last group of words we need to decrease the index.
        std::size_t index = mergeGroups ? textInfoIndicesEnd.mGroupIndex - 1 : textInfoIndicesEnd.mGroupIndex;

        // Get the last group of words.
        WordGroupLayoutInfo& groupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + index ) );

        // Check if is needed remove the whole word. (If the character index is pointing just after the end of the word)
        const WordLayoutInfo& wordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
        bool removeWholeWord = wordLayout.mCharactersLayoutInfo.size() == textInfoIndicesEnd.mCharacterIndex + 1;

        if( ( textInfoIndicesEnd.mWordIndex > 0 ) || ( removeWholeWord ) )
        {
          // Store text-actors before removing them.
          CollectTextActorsFromWords( removedTextActorsFromBegin, groupLayout, 0, ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1 : textInfoIndicesEnd.mWordIndex );

          // Remove extra words. (and current word if whole characters are removed)
          RemoveWordsFromWordGroup( 0,
                                    ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1 : textInfoIndicesEnd.mWordIndex,
                                    groupLayout );
        }

        if( !removeWholeWord )
        {
          // Only some characters of the word need to be deleted.

          // After removing all extra words. The word with the characters to be removed is the first one.
          WordLayoutInfo& wordLayout( *groupLayout.mWordsLayoutInfo.begin() );

          // Store text-actors before removing them.
          CollectTextActors( removedTextActorsFromBegin, wordLayout, 0, textInfoIndicesEnd.mCharacterIndex + 1 );

          RemoveCharactersFromWord( 0,
                                    textInfoIndicesEnd.mCharacterIndex + 1,
                                    wordLayout );
        }
      }
    } // end of remove from different groups
    else if( ( textInfoIndicesBegin.mGroupIndex == textInfoIndicesEnd.mGroupIndex ) && ( groupLayout.mNumberOfCharacters == numberOfCharacters ) )
    {
      // The whole group is deleted.
      ++textInfoIndicesEnd.mGroupIndex; // will delete current group.
      // TODO group before and group after need to be merged!!!
    }
    else
    {
      // characters to be deleted are on the same group of words. (words may need to be merged)

      // Group of words which contains the characters to be deleted.
      WordGroupLayoutInfo& groupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesBegin.mGroupIndex ) );

      RemoveCharactersFromWordGroupInfo( relayoutData,
                                         numberOfCharacters,
                                         mergeWords,
                                         mergeLines,
                                         textInfoIndicesBegin,
                                         textInfoIndicesEnd,
                                         textInfoMergeIndicesBegin,
                                         textInfoMergeIndicesEnd,
                                         groupLayout,
                                         removedTextActorsFromBegin,
                                         removedTextActorsFromEnd );

      if( mergeWords )
      {
        // Merges words pointed by textInfoMergeIndicesBegin.mWordIndex and textInfoMergeIndicesEnd.mWordIndex calculated previously.
        DALI_ASSERT_DEBUG( ( textInfoMergeIndicesBegin.mWordIndex < groupLayout.mWordsLayoutInfo.size() ) && "TextViewProcessor::UpdateTextInfo (delete). Word index (begin) out of bounds." );
        DALI_ASSERT_DEBUG( ( textInfoMergeIndicesEnd.mWordIndex < groupLayout.mWordsLayoutInfo.size() ) && "TextViewProcessor::UpdateTextInfo (delete). Word index (end) out of bounds." );

        WordLayoutInfo& firstWordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoMergeIndicesBegin.mWordIndex ) );
        WordLayoutInfo& lastWordLayout( *( groupLayout.mWordsLayoutInfo.begin() + textInfoMergeIndicesEnd.mWordIndex ) );

        MergeWord( firstWordLayout,
                   lastWordLayout );
      }

      // Store text-actors before removing them.
      const std::size_t endIndex = ( mergeWords && ( textInfoIndicesEnd.mWordIndex > 0 ) ) ? textInfoIndicesEnd.mWordIndex - 1 : textInfoIndicesEnd.mWordIndex; // text-actors from the last word may have been added in the merge above.
      CollectTextActorsFromWords( removedTextActorsFromMid, groupLayout, textInfoIndicesBegin.mWordIndex, endIndex );

      // Remove unwanted words using previously calculated indices. (including the last part of the merged word)
      groupLayout.mWordsLayoutInfo.erase( groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex, groupLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex );

      // Update group of words info
      groupLayout.mNumberOfCharacters -= numberOfCharacters;
      groupLayout.mSize = Size();
      groupLayout.mAscender = 0;
      for( WordLayoutInfoContainer::const_iterator it = groupLayout.mWordsLayoutInfo.begin(), endIt = groupLayout.mWordsLayoutInfo.end();
           it != endIt;
           ++it )
      {
        const WordLayoutInfo& layoutInfo( *it );
        UpdateSize( groupLayout.mSize, layoutInfo.mSize );
        groupLayout.mAscender = std::max( groupLayout.mAscender, layoutInfo.mAscender );
      }
    } // end of remove from same group

    if( mergeGroups )
    {
      // Merges group of words pointed by textInfoMergeIndicesBegin.mGroupIndex and textInfoMergeIndicesEnd.mGroupIndex calculated previously.

      WordGroupLayoutInfo& firstGroupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoMergeIndicesBegin.mGroupIndex ) );

      const WordGroupLayoutInfo& lastGroupLayout( *( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoMergeIndicesEnd.mGroupIndex ) );

      MergeWordGroup( firstGroupLayout,
                      lastGroupLayout );
    }

    // Remove unwanted groups of words using previously calculated indices. (including the last part of the merged group of words)
    lineLayout.mWordGroupsLayoutInfo.erase( lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesBegin.mGroupIndex, lineLayout.mWordGroupsLayoutInfo.begin() + textInfoIndicesEnd.mGroupIndex );

    // Update line info.
    lineLayout.mNumberOfCharacters -= numberOfCharacters;
    lineLayout.mSize = Size();
    lineLayout.mAscender = 0;
    for( WordGroupLayoutInfoContainer::const_iterator it = lineLayout.mWordGroupsLayoutInfo.begin(), endIt = lineLayout.mWordGroupsLayoutInfo.end();
         it != endIt;
         ++it )
    {
      const WordGroupLayoutInfo& layoutInfo( *it );
      UpdateSize( lineLayout.mSize, layoutInfo.mSize );
      lineLayout.mAscender = std::max( lineLayout.mAscender, layoutInfo.mAscender );
    }
    lineLayout.mSize.height += layoutParameters.mLineHeightOffset;
    lineLayout.mLineHeightOffset = layoutParameters.mLineHeightOffset;
  }// end delete text from same line.

  if( mergeLines )
  {
    // Merges lines pointed by textInfoMergeIndicesBegin.mLineIndex and textInfoMergeIndicesEnd.mLineIndex calculated previously.

    LineLayoutInfo& firstLineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoMergeIndicesBegin.mLineIndex ) );

    const LineLayoutInfo& lastLineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoMergeIndicesEnd.mLineIndex ) );

    MergeLine( firstLineLayout,
               lastLineLayout );
  }

  // Store text-actors before removing them.
  const std::size_t endIndex = ( mergeLines && ( textInfoIndicesEnd.mLineIndex > 0 ) ) ? textInfoIndicesEnd.mLineIndex - 1 : textInfoIndicesEnd.mLineIndex; // text-actors from the last line may have been added in the merge above.
  CollectTextActorsFromLines( removedTextActorsFromMid,
                              relayoutData.mTextLayoutInfo,
                              textInfoIndicesBegin.mLineIndex,
                              endIndex );

  // Remove unwanted lines using previously calculated indices. (including the last part of the merged line)
  relayoutData.mTextLayoutInfo.mLinesLayoutInfo.erase( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesBegin.mLineIndex,
                                                       relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesEnd.mLineIndex );

  // Update text info.
  UpdateTextLayoutInfo( relayoutData.mTextLayoutInfo );

  // If the last character of the last line is a new line character, an empty line need to be added.
  if( !relayoutData.mTextLayoutInfo.mLinesLayoutInfo.empty() )
  {
    const WordLayoutInfo lastWordLayout = GetLastWordLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1 ) );

    if( LineSeparator == lastWordLayout.mType )
    {
      LineLayoutInfo lastLineLayout;

      const CharacterLayoutInfo layoutInfo = GetLastCharacterLayoutInfo( lastWordLayout );
      lastLineLayout.mSize.height = layoutInfo.mSize.height;

      relayoutData.mTextLayoutInfo.mLinesLayoutInfo.push_back( lastLineLayout );

      relayoutData.mTextLayoutInfo.mWholeTextSize.height += layoutInfo.mSize.height;
    }
  }

  // Clear the text from the text-actors if required.
  if( CLEAR_TEXT == clearText )
  {
    ClearText( removedTextActorsFromEnd );
    ClearText( removedTextActorsFromMid );
    ClearText( removedTextActorsFromBegin );
  }

  // Insert text-actors into the cache.
  // Text-actors are inserted in reverse order to use first the first removed.
  relayoutData.mTextActorCache.InsertTextActors( removedTextActorsFromEnd );
  relayoutData.mTextActorCache.InsertTextActors( removedTextActorsFromMid );
  relayoutData.mTextActorCache.InsertTextActors( removedTextActorsFromBegin );
}

void UpdateTextInfo( const std::size_t position,
                     const std::size_t numberOfCharacters,
                     const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData )
{
  // Replaces 'numberOfCharacters' of text starting from 'position' with the given text.

  // TODO: Temporary implementation with remove and insert.

  // Remove.
  UpdateTextInfo( position,
                  numberOfCharacters,
                  layoutParameters,
                  relayoutData,
                  KEEP_TEXT ); // Do not clear the text from the text-actors.

  // Insert.
  UpdateTextInfo( position,
                  text,
                  layoutParameters,
                  relayoutData );
}

void UpdateTextInfo( const float lineHeightOffset,
                     TextLayoutInfo& textLayoutInfo )
{
  // Updates the space between lines with the new offset value.

  float newTextHeight = 0.f;

  for( LineLayoutInfoContainer::iterator lineIt = textLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = textLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt )
  {
    LineLayoutInfo& lineLayoutInfo( *lineIt );

    lineLayoutInfo.mSize.height += ( lineHeightOffset - lineLayoutInfo.mLineHeightOffset );
    newTextHeight += lineLayoutInfo.mSize.height;

    lineLayoutInfo.mLineHeightOffset = lineHeightOffset;
  }

  textLayoutInfo.mWholeTextSize.height = newTextHeight;
}

void UpdateTextInfo( const TextStyle& style,
                     const TextStyle::Mask mask,
                     TextView::RelayoutData& relayoutData )
{
  // Change text style for all text-actors.

  for( LineLayoutInfoContainer::iterator lineIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt )
  {
    LineLayoutInfo& line( *lineIt );

    for( WordGroupLayoutInfoContainer::iterator groupIt = line.mWordGroupsLayoutInfo.begin(), groupEndIt = line.mWordGroupsLayoutInfo.end();
         groupIt != groupEndIt;
         ++groupIt )
    {
      WordGroupLayoutInfo& group( *groupIt );

      for( WordLayoutInfoContainer::iterator wordIt = group.mWordsLayoutInfo.begin(), wordEndIt = group.mWordsLayoutInfo.end();
           wordIt != wordEndIt;
           ++wordIt )
      {
        WordLayoutInfo& word( *wordIt );

        for( CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
             characterIt != characterEndIt;
             ++characterIt )
        {
          CharacterLayoutInfo& characterLayout( *characterIt );

          characterLayout.mStyledText.mStyle.Copy( style, mask );

          // Checks if the font family supports all glyphs. If not, chooses a most suitable one.
          ChooseFontFamilyName( characterLayout.mStyledText );

          // Mark the character to be set the new style into the text-actor.
          characterLayout.mSetStyle = true;
        } // end characters
      } // end words
    } // end group of words
  } // end lines
}

void InitializeTextActorInfo( TextView::RelayoutData& relayoutData )
{
  TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;
  if( textLayoutInfo.mLinesLayoutInfo.empty() )
  {
    // nothing to do if there is no lines.
    return;
  }

  std::size_t characterGlobalIndex = 0; // Index to the global character (within the whole text).
  std::size_t lineLayoutInfoIndex = 0;  // Index to the laid out line info.
  const std::size_t lineLayoutInfoSize = relayoutData.mLines.size(); // Number or laid out lines.
  bool lineLayoutEnd = false;           // Whether lineLayoutInfoIndex points at the last laid out line.

  TextActor currentTextActor;           // text-actor used when the edit mode is disabled.
  TextStyle currentStyle;               // style for the current text-actor.
  Vector4 currentGradientColor;         // gradient color for the current text-actor.
  Vector2 currentStartPoint;            // start point for the current text-actor.
  Vector2 currentEndPoint;              // end point for the current text-actor.

  std::vector<TextActor> textActorsToRemove; // Keep a vector of text-actors to be included into the cache.

  for( LineLayoutInfoContainer::iterator lineIt = textLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = textLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt )
  {
    LineLayoutInfo& line( *lineIt );

    for( WordGroupLayoutInfoContainer::iterator groupIt = line.mWordGroupsLayoutInfo.begin(), groupEndIt = line.mWordGroupsLayoutInfo.end();
         groupIt != groupEndIt;
         ++groupIt )
    {
      WordGroupLayoutInfo& group( *groupIt );

      for( WordLayoutInfoContainer::iterator wordIt = group.mWordsLayoutInfo.begin(), wordEndIt = group.mWordsLayoutInfo.end();
           wordIt != wordEndIt;
           ++wordIt )
      {
        WordLayoutInfo& word( *wordIt );

        for( CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
             characterIt != characterEndIt;
             ++characterIt )
        {
          CharacterLayoutInfo& characterLayout( *characterIt );

          if( !characterLayout.mStyledText.mText.IsEmpty() )
          {
            // Do not create a text-actor if there is no text.
            const std::size_t length = characterLayout.mStyledText.mText.GetLength();
            const Character character = characterLayout.mStyledText.mText[0];

            if( ( 1 < length ) ||
                ( ( 1 == length ) && character.IsWhiteSpace() && characterLayout.mStyledText.mStyle.GetUnderline() ) ||
                ( ( 1 == length ) && !character.IsNewLine() && !character.IsWhiteSpace() ) )
            {
              // Do not create a text-actor if it's a white space (without underline) or a new line character.

              // Creates one text-actor per each counsecutive group of characters, with the same style, per line.

              // Check if there is a new line.
              const bool newLine = !lineLayoutEnd && ( characterGlobalIndex == relayoutData.mLines[lineLayoutInfoIndex].mCharacterGlobalIndex );

              if( ( characterLayout.mStyledText.mStyle != currentStyle ) ||
                  ( characterLayout.mGradientColor != currentGradientColor ) ||
                  ( characterLayout.mStartPoint != currentStartPoint ) ||
                  ( characterLayout.mEndPoint != currentEndPoint ) ||
                  newLine )
              {
                // There is a new style or a new line.
                if( newLine )
                {
                  // Point to the next line.
                  ++lineLayoutInfoIndex;
                  if( lineLayoutInfoIndex >= lineLayoutInfoSize )
                  {
                    // Arrived at last line.
                    lineLayoutEnd = true;
                  }
                }

                if( characterLayout.mTextActor )
                {
                  // Try to reuse first the text-actor of this character.
                  currentTextActor = characterLayout.mTextActor;
                  currentTextActor.SetTextStyle( characterLayout.mStyledText.mStyle );
                }
                else
                {
                  // If there is no text-actor, try to retrieve one from the cache.
                  currentTextActor = relayoutData.mTextActorCache.RetrieveTextActor();

                  // If still there is no text-actor, create one.
                  if( !currentTextActor )
                  {
                    currentTextActor = TextActor::New( Text(), characterLayout.mStyledText.mStyle, false, true );
                  }
                  else
                  {
                    currentTextActor.SetTextStyle( characterLayout.mStyledText.mStyle );
                  }
                }

                // Update style to be checked with next characters.
                currentStyle = characterLayout.mStyledText.mStyle;
                currentGradientColor = characterLayout.mGradientColor;
                currentStartPoint = characterLayout.mStartPoint;
                currentEndPoint = characterLayout.mEndPoint;

                characterLayout.mSetText = false;
                characterLayout.mSetStyle = false;

                characterLayout.mTextActor = currentTextActor;
                characterLayout.mTextActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
                characterLayout.mTextActor.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
              }
              else
              {
                // Same style than previous one.
                if( characterLayout.mTextActor )
                {
                  // There is a previously created text-actor for this character.
                  // If this character has another one put it into the cache.
                  characterLayout.mTextActor.SetText( "" );
                  textActorsToRemove.push_back( characterLayout.mTextActor );
                  characterLayout.mTextActor.Reset();
                }
              }
            } // no white space / new line char
          } // text not empty

          ++characterGlobalIndex;
        } // characters
      } // words
    } // groups
  } // lines

  // Insert the spare text-actors into the cache.
  relayoutData.mTextActorCache.InsertTextActors( textActorsToRemove );
}

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
