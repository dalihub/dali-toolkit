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
#include <dali-toolkit/internal/controls/text-view/text-view-processor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-view-word-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-line-processor.h>
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

namespace
{
/**
 * Update text layout info.
 *
 * Updates the size of the whole text, the maximum width of all words and the number of characters.
 *
 * @param[in,out] textLayoutInfo
 */
void UpdateLayoutInfo( TextLayoutInfo& textLayoutInfo )
{
  // Initialize members to be updated.
  textLayoutInfo.mWholeTextSize = Size::ZERO;
  textLayoutInfo.mMaxWordWidth = 0.f;
  textLayoutInfo.mNumberOfCharacters = 0u;

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

    for( WordLayoutInfoContainer::const_iterator wordIt = line.mWordsLayoutInfo.begin(), wordEndIt = line.mWordsLayoutInfo.end();
         wordIt != wordEndIt;
         ++wordIt )
    {
      const WordLayoutInfo& word( *wordIt );

      textLayoutInfo.mMaxWordWidth = std::max( textLayoutInfo.mMaxWordWidth, word.mSize.width );
    }
  }
}

} // namespace

// Constructors and assignment operators

TextInfoIndices::TextInfoIndices()
: mLineIndex( 0u ),
  mWordIndex( 0u ),
  mCharacterIndex( 0u )
{
}

TextInfoIndices::TextInfoIndices( const std::size_t lineIndex,
                                  const std::size_t wordIndex,
                                  const std::size_t characterIndex )
: mLineIndex( lineIndex ),
  mWordIndex( wordIndex ),
  mCharacterIndex( characterIndex )
{
}

bool TextInfoIndices::operator==( const TextInfoIndices& indices ) const
{
  return ( ( mLineIndex == indices.mLineIndex ) &&
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
  mNumberOfCharacters( 0u ),
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
  // * Traverse the given text spliting it in lines and each line in words.
  // * White spaces and new line characters are alone in one word.
  // * Bidirectional text is processed in each line.
  // * Generates a layout data structure to store layout information (size, position, ascender, text direction, etc) and metrics of all characters.
  // * Generates a text-actor data structure to store text, style and text-actors.
  // TODO: finish and test the bidirectional implementation.

  // Collect previously created text-actors.
  std::vector<TextActor> textActors;
  CollectTextActorsFromLines( textActors, relayoutData.mTextLayoutInfo, 0u, relayoutData.mTextLayoutInfo.mLinesLayoutInfo.size() );

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

    if( 0u < lineLayoutInfo.mNumberOfCharacters )
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
        const LineLayoutInfo& lineInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1u ) );

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

  if( 0u == relayoutData.mTextLayoutInfo.mNumberOfCharacters )
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
  std::size_t index = 0u;
  for( std::size_t i = 0u; i < relayoutDataForNewText.mTextLayoutInfo.mNumberOfCharacters; ++i )
  {
    relayoutData.mCharacterLogicalToVisualMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
    relayoutData.mCharacterVisualToLogicalMap.push_back( relayoutData.mTextLayoutInfo.mNumberOfCharacters + index );
    ++index;
  }

  // If a line is split, it stores the last part of the line.
  LineLayoutInfo lastLineLayoutInfo;

  // Stores indices to the line, word and character of the given position.
  TextInfoIndices textInfoIndices;

  if( position < relayoutData.mTextLayoutInfo.mNumberOfCharacters )
  {
    // Get line, word and character indices for given position.
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
      textInfoIndices.mLineIndex = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.size() - 1u;
      const LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1u ) );

      if( !lineLayoutInfo.mWordsLayoutInfo.empty() )
      {
        textInfoIndices.mWordIndex = lineLayoutInfo.mWordsLayoutInfo.size() - 1u;

        const WordLayoutInfo& wordLayoutInfo( *( lineLayoutInfo.mWordsLayoutInfo.end() - 1u ) );
        textInfoIndices.mCharacterIndex = wordLayoutInfo.mCharactersLayoutInfo.size();
      }
    }
  }

  // 2) If the new text has more than 1 line, merge the last line of the input text with the last part of the split line.
  //TODO check this cases ( num lines ==1, >1, >2 ) if it could be simplified.
  if( relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.size() > 1u )
  {
    LineLayoutInfo& lastInputLineLayoutInfo( *( relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.end() - 1u ) );

    MergeLine( lastInputLineLayoutInfo,
               lastLineLayoutInfo );

    if( relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.size() > 2u )
    {
      // Insert all lines except first and last in the text.
      relayoutData.mTextLayoutInfo.mLinesLayoutInfo.insert( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndices.mLineIndex + 1u,
                                                            relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.begin() + 1u,
                                                            relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.end() - 1u );
    }

    // Insert the last line to the text
    relayoutData.mTextLayoutInfo.mLinesLayoutInfo.insert( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndices.mLineIndex + relayoutDataForNewText.mTextLayoutInfo.mLinesLayoutInfo.size() - 1u,
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
  UpdateLayoutInfo( relayoutData.mTextLayoutInfo );
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
  // *   If only some characters of the same line are going to be deleted, proceed similarly: check if text to be deleted is in the same word.
  // *     If it is not, split/merge words.
  // *     Check if the whole word needs to be deleted.
  // *     Check if only some characters of the word need to be deleted.
  // * Updates layout info.

  // * The algorithm checks if a word separator is deleted (in that case, different words need to be merged) and if a new line separator is deleted (two lines need to be merged).

  // Early return

  if( 0u == numberOfCharacters )
  {
    DALI_ASSERT_DEBUG( !"TextViewProcessor::UpdateTextInfo. WARNING: trying to delete 0 characters!" );

    // nothing to do if no characters are deleted.
    return;
  }

  // Asserts if trying to delete text out of bounds.
  DALI_ASSERT_ALWAYS( position + numberOfCharacters <= relayoutData.mTextLayoutInfo.mNumberOfCharacters && "TextViewProcessor::UpdateTextInfo. ERROR: trying to delete characters out of boundary" );

  // Remove characters from character to visual map and vs //TODO: check this for RTL text!!
  relayoutData.mCharacterLogicalToVisualMap.erase( relayoutData.mCharacterLogicalToVisualMap.end() - numberOfCharacters, relayoutData.mCharacterLogicalToVisualMap.end() );
  relayoutData.mCharacterVisualToLogicalMap.erase( relayoutData.mCharacterVisualToLogicalMap.end() - numberOfCharacters, relayoutData.mCharacterVisualToLogicalMap.end() );

  // Get line, word and character indices for the given start position.
  TextInfoIndices textInfoIndicesBegin;
  GetIndicesFromGlobalCharacterIndex( position,
                                      relayoutData.mTextLayoutInfo,
                                      textInfoIndicesBegin );

  // Get line, word and character indices for the given end position (start position + number of characters to be deleted).
  TextInfoIndices textInfoIndicesEnd;
  GetIndicesFromGlobalCharacterIndex( position + numberOfCharacters - 1u,
                                      relayoutData.mTextLayoutInfo,
                                      textInfoIndicesEnd );

  // Vectors used to temporary store text-actors removed from text.
  // Three vectors are needed because text-actors are not removed in order
  // but insert them in order is required to reuse them later.
  std::vector<TextActor> removedTextActorsFromBegin;
  std::vector<TextActor> removedTextActorsFromMid;
  std::vector<TextActor> removedTextActorsFromEnd;

  // Whether lines and words need to be merged.
  bool mergeLines = false;
  bool mergeWords = false;

  // Indices of the lines and words to be merged.
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

    if( ( textInfoIndicesBegin.mWordIndex > 0u ) || ( textInfoIndicesBegin.mCharacterIndex > 0u ) )
    {
      // first character to be deleted is not the first one of the current line.
      ++textInfoIndicesBegin.mLineIndex; // won't delete current line

      // As some characters remain, this line could be merged with the last one.
      mergeFirstLine = true;
    }

    // Check if all characters of the last line are going to be deleted.
    bool wholeLineDeleted = false;
    const LineLayoutInfo& lastLineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesEnd.mLineIndex ) );
    if( textInfoIndicesEnd.mWordIndex + 1u == lastLineLayout.mWordsLayoutInfo.size() )
    {
      const WordLayoutInfo& lastWordLayout( *( lastLineLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
      if( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() )
      {
        // All characters of the last line are going to be deleted.
        ++textInfoIndicesEnd.mLineIndex; // will delete the last line.

        // the whole last line is deleted. Need to check if the next line could be merged.
        mergeLastLine = false;
        wholeLineDeleted = true;
      }
    }

    if( wholeLineDeleted )
    {
      // It means the whole last line is deleted completely.
      // It's needed to check if there is another line after that could be merged.
      if( textInfoIndicesEnd.mLineIndex < relayoutData.mTextLayoutInfo.mLinesLayoutInfo.size() )
      {
        mergeLastLine = true;

        // Point the first characters of the next line.
        textInfoIndicesEnd.mWordIndex = 0u;
        textInfoIndicesEnd.mCharacterIndex = 0u;
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
      std::vector<TextActor> removedTextActorsFromFirstLine;

      // As lineIndexBegin has been increased just to not to remove the line, decrease now is needed to access it.
      LineLayoutInfo& lineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesBegin.mLineIndex - 1u ) );

      if( ( textInfoIndicesBegin.mWordIndex + 1u < lineLayout.mWordsLayoutInfo.size() ) || ( 0u == textInfoIndicesBegin.mCharacterIndex ) )
      {
        // Remove extra words within current line. (and current word if whole characters are removed)
        // 0 == characterIndexBegin means the whole word is deleted.
        const std::size_t wordIndex = ( ( 0u == textInfoIndicesBegin.mCharacterIndex ) ? textInfoIndicesBegin.mWordIndex : textInfoIndicesBegin.mWordIndex + 1u );

        // Store text-actors before removing them.
        CollectTextActorsFromWords( removedTextActorsFromFirstLine, lineLayout, wordIndex, lineLayout.mWordsLayoutInfo.size() );

        RemoveWordsFromLine( wordIndex,
                             lineLayout.mWordsLayoutInfo.size() - wordIndex,
                             layoutParameters.mLineHeightOffset,
                             lineLayout );
      }

      if( ( textInfoIndicesBegin.mWordIndex < lineLayout.mWordsLayoutInfo.size() ) && ( textInfoIndicesBegin.mCharacterIndex > 0u ) )
      {
        // Only some characters of the word need to be removed.
        WordLayoutInfo& wordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

        // Store text-actors before removing them.
        CollectTextActors( removedTextActorsFromFirstWord, wordLayout, textInfoIndicesBegin.mCharacterIndex, wordLayout.mCharactersLayoutInfo.size() );

        const std::size_t wordNumberCharacters = wordLayout.mCharactersLayoutInfo.size();
        RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                                  wordLayout.mCharactersLayoutInfo.size() - textInfoIndicesBegin.mCharacterIndex,
                                  wordLayout );

        // discount the removed number of characters.
        const std::size_t removedNumberOfCharacters = ( wordNumberCharacters - wordLayout.mCharactersLayoutInfo.size() );
        lineLayout.mNumberOfCharacters -= removedNumberOfCharacters;
      }
      UpdateLayoutInfo( lineLayout, layoutParameters.mLineHeightOffset );

      // Insert the text-actors in order.
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromFirstWord.begin(), removedTextActorsFromFirstWord.end() );
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromFirstLine.begin(), removedTextActorsFromFirstLine.end() );
    }

    if( mergeLastLine && !wholeLineDeleted )
    {
      // Some characters from the last line need to be removed.

      // Vectors used to temporary store text-actors removed from the line.
      // Three vectors are needed because text-actors are not removed in order
      // but insert them in order is required to reuse them later.
      std::vector<TextActor> removedTextActorsFromFirstWord;
      std::vector<TextActor> removedTextActorsFromFirstLine;

      // lineIndexEnd was increased to delete the last line if lines need to be merged.
      // To access now the last line we need to decrease the index.
      const std::size_t lineIndex = ( mergeLines ? textInfoIndicesEnd.mLineIndex - 1u : textInfoIndicesEnd.mLineIndex );

      // Get the last line.
      LineLayoutInfo& lineLayout( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + lineIndex ) );

      // Check if is needed remove the whole word. (If the character index is pointing just after the end of the word)
      const WordLayoutInfo& wordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
      bool removeWholeWord = wordLayout.mCharactersLayoutInfo.size() == textInfoIndicesEnd.mCharacterIndex + 1u;

      if( ( textInfoIndicesEnd.mWordIndex > 0u ) || ( removeWholeWord ) )
      {
        // Store text-actors before removing them.
        CollectTextActorsFromWords( removedTextActorsFromFirstLine,
                                    lineLayout,
                                    0u,
                                    ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1u : textInfoIndicesEnd.mWordIndex );

        // Remove extra words. (and current word if whole characters are removed)
        RemoveWordsFromLine( 0u,
                             ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1u : textInfoIndicesEnd.mWordIndex,
                             layoutParameters.mLineHeightOffset,
                             lineLayout );
      }

      if( !removeWholeWord )
      {
        // Only some characters of the word need to be deleted.

        // After removing all extra words. The word with the characters to be removed is the first one.
        WordLayoutInfo& wordLayout( *lineLayout.mWordsLayoutInfo.begin() );

        // Store text-actors before removing them.
        CollectTextActors( removedTextActorsFromFirstWord, wordLayout, 0u, textInfoIndicesEnd.mCharacterIndex + 1u );

        const std::size_t wordNumberCharacters = wordLayout.mCharactersLayoutInfo.size();
        RemoveCharactersFromWord( 0u,
                                  textInfoIndicesEnd.mCharacterIndex + 1u,
                                  wordLayout );

        // discount the removed number of characters.
        const std::size_t removedNumberOfCharacters = ( wordNumberCharacters - wordLayout.mCharactersLayoutInfo.size() );
        lineLayout.mNumberOfCharacters -= removedNumberOfCharacters;
      }
      UpdateLayoutInfo( lineLayout, layoutParameters.mLineHeightOffset );

      // Insert the text-actors in order.
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromFirstWord.begin(), removedTextActorsFromFirstWord.end() );
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromFirstLine.begin(), removedTextActorsFromFirstLine.end() );
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

    // Remove the characters from the line layout info. It returns whether the current line can be merged with the next one.
    RemoveCharactersFromLineInfo( relayoutData,
                                  numberOfCharacters,
                                  mergeWords,
                                  mergeLines,
                                  textInfoIndicesBegin,
                                  textInfoIndicesEnd,
                                  textInfoMergeIndicesBegin,
                                  textInfoMergeIndicesEnd,
                                  lineLayout,
                                  removedTextActorsFromBegin,
                                  removedTextActorsFromEnd );

    if( mergeWords )
    {
      // Merges words pointed by textInfoMergeIndicesBegin.mWordIndex and textInfoMergeIndicesEnd.mWordIndex calculated previously.
      DALI_ASSERT_DEBUG( ( textInfoMergeIndicesBegin.mWordIndex < lineLayout.mWordsLayoutInfo.size() ) && "TextViewProcessor::UpdateTextInfo (delete). Word index (begin) out of bounds." );
      DALI_ASSERT_DEBUG( ( textInfoMergeIndicesEnd.mWordIndex < lineLayout.mWordsLayoutInfo.size() ) && "TextViewProcessor::UpdateTextInfo (delete). Word index (end) out of bounds." );

      WordLayoutInfo& firstWordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoMergeIndicesBegin.mWordIndex ) );
      WordLayoutInfo& lastWordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoMergeIndicesEnd.mWordIndex ) );

      MergeWord( firstWordLayout,
                 lastWordLayout );
    }

    // Store text-actors before removing them.
    const std::size_t endIndex = ( mergeWords && ( textInfoIndicesEnd.mWordIndex > 0u ) ) ? textInfoIndicesEnd.mWordIndex - 1u : textInfoIndicesEnd.mWordIndex; // text-actors from the last word may have been added in the merge above.
    CollectTextActorsFromWords( removedTextActorsFromMid, lineLayout, textInfoIndicesBegin.mWordIndex, endIndex );

    // Remove unwanted words using previously calculated indices. (including the last part of the merged word)
    lineLayout.mWordsLayoutInfo.erase( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex, lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex );

    // Update line info.
    UpdateLayoutInfo( lineLayout, layoutParameters.mLineHeightOffset );
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
  const std::size_t endIndex = ( mergeLines && ( textInfoIndicesEnd.mLineIndex > 0u ) ) ? textInfoIndicesEnd.mLineIndex - 1u : textInfoIndicesEnd.mLineIndex; // text-actors from the last line may have been added in the merge above.
  CollectTextActorsFromLines( removedTextActorsFromMid,
                              relayoutData.mTextLayoutInfo,
                              textInfoIndicesBegin.mLineIndex,
                              endIndex );

  // Remove unwanted lines using previously calculated indices. (including the last part of the merged line)
  relayoutData.mTextLayoutInfo.mLinesLayoutInfo.erase( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesBegin.mLineIndex,
                                                       relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + textInfoIndicesEnd.mLineIndex );

  // Update text info.
  UpdateLayoutInfo( relayoutData.mTextLayoutInfo );

  // If the last character of the last line is a new line character, an empty line need to be added.
  if( !relayoutData.mTextLayoutInfo.mLinesLayoutInfo.empty() )
  {
    const WordLayoutInfo lastWordLayout = GetLastWordLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1u ) );

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

    for( WordLayoutInfoContainer::iterator wordIt = line.mWordsLayoutInfo.begin(), wordEndIt = line.mWordsLayoutInfo.end();
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
  } // end lines
}

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
