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
#include <dali-toolkit/internal/controls/text-view/text-view-paragraph-processor.h>
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
  for( ParagraphLayoutInfoContainer::const_iterator paragraphIt = textLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = textLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    const ParagraphLayoutInfo& paragraph( *paragraphIt );

    // Updates text size with the size of all paragraphs.
    UpdateSize( textLayoutInfo.mWholeTextSize, paragraph.mSize, GrowHeight );

    // Updates number of characters.
    textLayoutInfo.mNumberOfCharacters += paragraph.mNumberOfCharacters;

    // Updates the max word's width.
    for( WordLayoutInfoContainer::const_iterator wordIt = paragraph.mWordsLayoutInfo.begin(), wordEndIt = paragraph.mWordsLayoutInfo.end();
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
: mParagraphIndex( 0u ),
  mWordIndex( 0u ),
  mCharacterIndex( 0u )
{
}

TextInfoIndices::TextInfoIndices( const std::size_t paragraphIndex,
                                  const std::size_t wordIndex,
                                  const std::size_t characterIndex )
: mParagraphIndex( paragraphIndex ),
  mWordIndex( wordIndex ),
  mCharacterIndex( characterIndex )
{
}

bool TextInfoIndices::operator==( const TextInfoIndices& indices ) const
{
  return ( ( mParagraphIndex == indices.mParagraphIndex ) &&
           ( mWordIndex == indices.mWordIndex ) &&
           ( mCharacterIndex == indices.mCharacterIndex ) );
}

/////////////////////
// Layout info.
/////////////////////

TextLayoutInfo::TextLayoutInfo()
: mWholeTextSize(),
  mMaxWordWidth( 0.f ),
  mMaxItalicsOffset( 0.f ),
  mNumberOfCharacters( 0u ),
  mParagraphsLayoutInfo(),
  mEllipsizeLayoutInfo()
{
}

TextLayoutInfo::TextLayoutInfo( const TextLayoutInfo& text )
: mWholeTextSize( text.mWholeTextSize ),
  mMaxWordWidth( text.mMaxWordWidth ),
  mMaxItalicsOffset( text.mMaxItalicsOffset ),
  mNumberOfCharacters( text.mNumberOfCharacters ),
  mParagraphsLayoutInfo( text.mParagraphsLayoutInfo ),
  mEllipsizeLayoutInfo( text.mEllipsizeLayoutInfo )
{
}

TextLayoutInfo& TextLayoutInfo::operator=( const TextLayoutInfo& text )
{
  if( this != &text )
  {
    mWholeTextSize = text.mWholeTextSize;
    mMaxWordWidth = text.mMaxWordWidth;
    mMaxItalicsOffset = text.mMaxItalicsOffset;
    mNumberOfCharacters = text.mNumberOfCharacters;
    mParagraphsLayoutInfo = text.mParagraphsLayoutInfo;
    mEllipsizeLayoutInfo = text.mEllipsizeLayoutInfo;
  }

  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CreateTextInfo( const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData )
{
  // * Traverse the given text spliting it in paragraphs and each paragraph in words.
  // * White spaces and new paragraph characters are alone in one word.
  // * Bidirectional text is processed in each paragraph.
  // * Generates a layout data structure to store layout information (size, position, ascender, text direction, etc) and metrics of all characters.
  // * Generates a text-actor data structure to store text, style and text-actors.
  // TODO: finish and test the bidirectional implementation.

  // Collect previously created text-actors.
  std::vector<TextActor> textActors;
  CollectTextActorsFromParagraphs( textActors, relayoutData.mTextLayoutInfo, 0u, relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.size() );

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

  // Split the whole text in paragraphs.
  std::vector<MarkupProcessor::StyledTextArray> paragraphs;
  TextProcessor::SplitInParagraphs( text,
                                    paragraphs );

  // Traverse all paragraphs
  for( std::vector<MarkupProcessor::StyledTextArray>::const_iterator paragraphIt = paragraphs.begin(), paragraphEndIt = paragraphs.end(); paragraphIt != paragraphEndIt; ++paragraphIt )
  {
    const MarkupProcessor::StyledTextArray& paragraph( *paragraphIt );

    // Data structures for the new paragraph
    ParagraphLayoutInfo paragraphLayoutInfo;

    // Fills the paragraph data structures with the layout info.
    CreateParagraphInfo( paragraph,
                         relayoutData,
                         paragraphLayoutInfo );

    if( 0u < paragraphLayoutInfo.mNumberOfCharacters )
    {
      // do not add the line offset if the paragraph has no characters.
      paragraphLayoutInfo.mSize.height += layoutParameters.mLineHeightOffset;
      paragraphLayoutInfo.mLineHeightOffset = layoutParameters.mLineHeightOffset;
    }
    else
    {
      // line height needs to be added for the last paragraph.

      float lineHeight = 0.f;
      // Get the last character of the last paragraph.
      if( !relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.empty() )
      {
        const ParagraphLayoutInfo& paragraphInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end() - 1u ) );

        const CharacterLayoutInfo characterInfo = GetLastCharacterLayoutInfo( paragraphInfo );

        lineHeight = characterInfo.mSize.height;
      }

      paragraphLayoutInfo.mSize.height = lineHeight;
    }

    // Update layout info for the whole text.
    UpdateSize( relayoutData.mTextLayoutInfo.mWholeTextSize, paragraphLayoutInfo.mSize, GrowHeight );
    relayoutData.mTextLayoutInfo.mNumberOfCharacters += paragraphLayoutInfo.mNumberOfCharacters;

    // Add the paragraph to the current text.
    relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.push_back( paragraphLayoutInfo );
  } // end of paragraphs
}

void UpdateTextInfo( const std::size_t position,
                     const MarkupProcessor::StyledTextArray& text,
                     const TextView::LayoutParameters& layoutParameters,
                     TextView::RelayoutData& relayoutData )
{
  // Update current internal data structure with added text.

  // * Creates layout info for the given text.
  // * With the given position, find where to add the text.
  // * If the new text is not added at the end of current text, a paragraph need to be split.
  // * Merge the last paragraph of the new text to the last part or the split paragraph.
  // * Add paragraphs between first and last of the new text.
  // * Merge the first part of the split paragraph with the first paragraph of the new text.
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

  // If a paragraph is split, it stores the last part of the paragraph.
  ParagraphLayoutInfo lastParagraphLayoutInfo;

  // Stores indices to the paragraph, word and character of the given position.
  TextInfoIndices textInfoIndices;

  if( position < relayoutData.mTextLayoutInfo.mNumberOfCharacters )
  {
    // Get paragraph, word and character indices for given position.
    GetIndicesFromGlobalCharacterIndex( position,
                                        relayoutData.mTextLayoutInfo,
                                        textInfoIndices );

    // 1) Split the paragraph

    // Split a paragraph in two is needed, then merge the first part of the split paragraph
    // with the first paragraph of the new text, add subsequent paragraphs and merge the last paragraph
    // of the new text with the last part of the split paragraph.

    // Implementation notes!
    //
    // These references to the first paragraph are declared in this scope because if new paragraphs are inserted in step 2,
    // they become invalid, making the algorithm to crash if used again.
    // In the step 3, references to the first paragraph are needed and declared again.

    // Stores the first part of the split paragraph.
    ParagraphLayoutInfo& firstParagraphLayoutInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndices.mParagraphIndex ) );

    SplitParagraph( textInfoIndices,
                    PointSize( layoutParameters.mLineHeightOffset ),
                    firstParagraphLayoutInfo,
                    lastParagraphLayoutInfo );
  }
  else
  {
    // Position is just after the last character.
    // Calculates indices for that position.
    if( !relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.empty() )
    {
      textInfoIndices.mParagraphIndex = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.size() - 1u;
      const ParagraphLayoutInfo& paragraphLayoutInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end() - 1u ) );

      if( !paragraphLayoutInfo.mWordsLayoutInfo.empty() )
      {
        textInfoIndices.mWordIndex = paragraphLayoutInfo.mWordsLayoutInfo.size() - 1u;

        const WordLayoutInfo& wordLayoutInfo( *( paragraphLayoutInfo.mWordsLayoutInfo.end() - 1u ) );
        textInfoIndices.mCharacterIndex = wordLayoutInfo.mCharactersLayoutInfo.size();
      }
    }
  }

  // 2) If the new text has more than 1 paragraph, merge the last paragraph of the input text with the last part of the split paragraph.
  //TODO check this cases ( num paragraphs ==1, >1, >2 ) if it could be simplified.
  if( relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.size() > 1u )
  {
    ParagraphLayoutInfo& lastInputParagraphLayoutInfo( *( relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.end() - 1u ) );

    MergeParagraph( lastInputParagraphLayoutInfo,
                    lastParagraphLayoutInfo );

    if( relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.size() > 2u )
    {
      // Insert all paragraphs except first and last in the text.
      relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.insert( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndices.mParagraphIndex + 1u,
                                                            relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + 1u,
                                                            relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.end() - 1u );
    }

    // Insert the last paragraph to the text
    relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.insert( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndices.mParagraphIndex + relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.size() - 1u,
                                                          lastInputParagraphLayoutInfo );
  }
  else
  {
    // Merge the new paragraph to the last part of the split paragraph.
    ParagraphLayoutInfo& inputParagraphLayoutInfo( *relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.begin() );

    MergeParagraph( inputParagraphLayoutInfo,
                    lastParagraphLayoutInfo );
  }

  // 3) Merge the first paragraph of the split text with the first paragraph of the input text.
  ParagraphLayoutInfo& firstParagraphLayoutInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndices.mParagraphIndex ) );
  ParagraphLayoutInfo& firstInputParagraphLayoutInfo( *relayoutDataForNewText.mTextLayoutInfo.mParagraphsLayoutInfo.begin() );

  MergeParagraph( firstParagraphLayoutInfo,
                  firstInputParagraphLayoutInfo );

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

  // * It checks if text to be deleted is in the same paragraph or not:
  // *   If it is not, check which paragraphs need to be split/merged or deleted.
  // *   If it is but all characters of the paragraph are going to be deleted, just delete the paragraph (nothing needs to be split/merged)
  // *   If only some characters of the same paragraph are going to be deleted, proceed similarly: check if text to be deleted is in the same word.
  // *     If it is not, split/merge words.
  // *     Check if the whole word needs to be deleted.
  // *     Check if only some characters of the word need to be deleted.
  // * Updates layout info.

  // * The algorithm checks if a word separator is deleted (in that case, different words need to be merged) and if a new paragraph separator is deleted (two paragraphs need to be merged).

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

  // Get paragraph, word and character indices for the given start position.
  TextInfoIndices textInfoIndicesBegin;
  GetIndicesFromGlobalCharacterIndex( position,
                                      relayoutData.mTextLayoutInfo,
                                      textInfoIndicesBegin );

  // Get paragraph, word and character indices for the given end position (start position + number of characters to be deleted).
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

  // Whether paragraphs and words need to be merged.
  bool mergeParagraphs = false;
  bool mergeWords = false;

  // Indices of the paragraphs and words to be merged.
  TextInfoIndices textInfoMergeIndicesBegin;
  TextInfoIndices textInfoMergeIndicesEnd;

  const ParagraphLayoutInfo& paragraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndicesBegin.mParagraphIndex ) ); // used to check the number of characters of the paragraph
                                                                                                                                    // if all characters to be deleted are in the same paragraph.
  if( textInfoIndicesBegin.mParagraphIndex < textInfoIndicesEnd.mParagraphIndex )
  {
    // Deleted text is from different paragraphs. It may need to split two paragraphs, and merge first part of the first one with last part of the last one.

    // whether first or last paragraph need to be split and merged with the last part.
    bool mergeFirstParagraph = false;
    bool mergeLastParagraph = true;

    textInfoMergeIndicesBegin.mParagraphIndex = textInfoIndicesBegin.mParagraphIndex;
    textInfoMergeIndicesEnd.mParagraphIndex = textInfoIndicesEnd.mParagraphIndex;

    if( ( textInfoIndicesBegin.mWordIndex > 0u ) || ( textInfoIndicesBegin.mCharacterIndex > 0u ) )
    {
      // first character to be deleted is not the first one of the current paragraph.
      ++textInfoIndicesBegin.mParagraphIndex; // won't delete current paragraph

      // As some characters remain, this paragraph could be merged with the last one.
      mergeFirstParagraph = true;
    }

    // Check if all characters of the last paragraph are going to be deleted.
    bool wholeParagraphDeleted = false;
    const ParagraphLayoutInfo& lastParagraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndicesEnd.mParagraphIndex ) );
    if( textInfoIndicesEnd.mWordIndex + 1u == lastParagraphLayout.mWordsLayoutInfo.size() )
    {
      const WordLayoutInfo& lastWordLayout( *( lastParagraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
      if( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() )
      {
        // All characters of the last paragraph are going to be deleted.
        ++textInfoIndicesEnd.mParagraphIndex; // will delete the last paragraph.

        // the whole last paragraph is deleted. Need to check if the next paragraph could be merged.
        mergeLastParagraph = false;
        wholeParagraphDeleted = true;
      }
    }

    if( wholeParagraphDeleted )
    {
      // It means the whole last paragraph is deleted completely.
      // It's needed to check if there is another paragraph after that could be merged.
      if( textInfoIndicesEnd.mParagraphIndex < relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.size() )
      {
        mergeLastParagraph = true;

        // Point the first characters of the next paragraph.
        textInfoIndicesEnd.mWordIndex = 0u;
        textInfoIndicesEnd.mCharacterIndex = 0u;
        textInfoMergeIndicesEnd.mParagraphIndex = textInfoIndicesEnd.mParagraphIndex;
      }
    }

    // If some characters remain in the first and last paragraph, they need to be merged.
    mergeParagraphs = mergeFirstParagraph && mergeLastParagraph;

    if( mergeParagraphs )
    {
      // last paragraph is going to be merged with the first one, so is not needed.
      ++textInfoIndicesEnd.mParagraphIndex; // will delete the last paragraph.
    }

    if( mergeFirstParagraph )
    {
      // Remove characters from the first paragraph.

      // Vectors used to temporary store text-actors removed from the paragraph.
      // Three vectors are needed because text-actors are not removed in order
      // but insert them in order is required to reuse them later.
      std::vector<TextActor> removedTextActorsFromFirstWord;
      std::vector<TextActor> removedTextActorsFromFirstParagraph;

      // As paragraphIndexBegin has been increased just to not to remove the paragraph, decrease now is needed to access it.
      ParagraphLayoutInfo& paragraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndicesBegin.mParagraphIndex - 1u ) );

      if( ( textInfoIndicesBegin.mWordIndex + 1u < paragraphLayout.mWordsLayoutInfo.size() ) || ( 0u == textInfoIndicesBegin.mCharacterIndex ) )
      {
        // Remove extra words within current paragraph. (and current word if whole characters are removed)
        // 0 == characterIndexBegin means the whole word is deleted.
        const std::size_t wordIndex = ( ( 0u == textInfoIndicesBegin.mCharacterIndex ) ? textInfoIndicesBegin.mWordIndex : textInfoIndicesBegin.mWordIndex + 1u );

        // Store text-actors before removing them.
        CollectTextActorsFromWords( removedTextActorsFromFirstParagraph, paragraphLayout, wordIndex, paragraphLayout.mWordsLayoutInfo.size() );

        RemoveWordsFromParagraph( wordIndex,
                                  paragraphLayout.mWordsLayoutInfo.size() - wordIndex,
                                  layoutParameters.mLineHeightOffset,
                                  paragraphLayout );
      }

      if( ( textInfoIndicesBegin.mWordIndex < paragraphLayout.mWordsLayoutInfo.size() ) && ( textInfoIndicesBegin.mCharacterIndex > 0u ) )
      {
        // Only some characters of the word need to be removed.
        WordLayoutInfo& wordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

        // Store text-actors before removing them.
        CollectTextActors( removedTextActorsFromFirstWord, wordLayout, textInfoIndicesBegin.mCharacterIndex, wordLayout.mCharactersLayoutInfo.size() );

        const std::size_t wordNumberCharacters = wordLayout.mCharactersLayoutInfo.size();
        RemoveCharactersFromWord( textInfoIndicesBegin.mCharacterIndex,
                                  wordLayout.mCharactersLayoutInfo.size() - textInfoIndicesBegin.mCharacterIndex,
                                  wordLayout );

        // discount the removed number of characters.
        const std::size_t removedNumberOfCharacters = ( wordNumberCharacters - wordLayout.mCharactersLayoutInfo.size() );
        paragraphLayout.mNumberOfCharacters -= removedNumberOfCharacters;
      }
      UpdateLayoutInfo( paragraphLayout, layoutParameters.mLineHeightOffset );

      // Insert the text-actors in order.
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromFirstWord.begin(), removedTextActorsFromFirstWord.end() );
      removedTextActorsFromBegin.insert( removedTextActorsFromBegin.end(), removedTextActorsFromFirstParagraph.begin(), removedTextActorsFromFirstParagraph.end() );
    }

    if( mergeLastParagraph && !wholeParagraphDeleted )
    {
      // Some characters from the last paragraph need to be removed.

      // Vectors used to temporary store text-actors removed from the paragraph.
      // Three vectors are needed because text-actors are not removed in order
      // but insert them in order is required to reuse them later.
      std::vector<TextActor> removedTextActorsFromFirstWord;
      std::vector<TextActor> removedTextActorsFromFirstParagraph;

      // paragraphIndexEnd was increased to delete the last paragraph if paragraphs need to be merged.
      // To access now the last paragraph we need to decrease the index.
      const std::size_t paragraphIndex = ( mergeParagraphs ? textInfoIndicesEnd.mParagraphIndex - 1u : textInfoIndicesEnd.mParagraphIndex );

      // Get the last paragraph.
      ParagraphLayoutInfo& paragraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + paragraphIndex ) );

      // Check if is needed remove the whole word. (If the character index is pointing just after the end of the word)
      const WordLayoutInfo& wordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );
      bool removeWholeWord = wordLayout.mCharactersLayoutInfo.size() == textInfoIndicesEnd.mCharacterIndex + 1u;

      if( ( textInfoIndicesEnd.mWordIndex > 0u ) || ( removeWholeWord ) )
      {
        // Store text-actors before removing them.
        CollectTextActorsFromWords( removedTextActorsFromFirstParagraph,
                                    paragraphLayout,
                                    0u,
                                    ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1u : textInfoIndicesEnd.mWordIndex );

        // Remove extra words. (and current word if whole characters are removed)
        RemoveWordsFromParagraph( 0u,
                                  ( removeWholeWord ) ? textInfoIndicesEnd.mWordIndex + 1u : textInfoIndicesEnd.mWordIndex,
                                  layoutParameters.mLineHeightOffset,
                                  paragraphLayout );
      }

      if( !removeWholeWord )
      {
        // Only some characters of the word need to be deleted.

        // After removing all extra words. The word with the characters to be removed is the first one.
        WordLayoutInfo& wordLayout( *paragraphLayout.mWordsLayoutInfo.begin() );

        // Store text-actors before removing them.
        CollectTextActors( removedTextActorsFromFirstWord, wordLayout, 0u, textInfoIndicesEnd.mCharacterIndex + 1u );

        const std::size_t wordNumberCharacters = wordLayout.mCharactersLayoutInfo.size();
        RemoveCharactersFromWord( 0u,
                                  textInfoIndicesEnd.mCharacterIndex + 1u,
                                  wordLayout );

        // discount the removed number of characters.
        const std::size_t removedNumberOfCharacters = ( wordNumberCharacters - wordLayout.mCharactersLayoutInfo.size() );
        paragraphLayout.mNumberOfCharacters -= removedNumberOfCharacters;
      }
      UpdateLayoutInfo( paragraphLayout, layoutParameters.mLineHeightOffset );

      // Insert the text-actors in order.
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromFirstWord.begin(), removedTextActorsFromFirstWord.end() );
      removedTextActorsFromEnd.insert( removedTextActorsFromEnd.end(), removedTextActorsFromFirstParagraph.begin(), removedTextActorsFromFirstParagraph.end() );
    }
  } // end delete text from different paragraphs
  else if( ( textInfoIndicesBegin.mParagraphIndex == textInfoIndicesEnd.mParagraphIndex ) && ( paragraphLayout.mNumberOfCharacters == numberOfCharacters ) )
  {
    // the whole paragraph needs to be deleted.
    ++textInfoIndicesEnd.mParagraphIndex; // will delete current paragraph.
  }
  else
  {
    // deleted text is within the same paragraph. (merge paragraphs could be needed if the paragraph separator character is deleted)

    // Paragraph which contains the characters to be deleted.
    ParagraphLayoutInfo& paragraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndicesBegin.mParagraphIndex ) );

    // Remove the characters from the paragraph layout info. It returns whether the current paragraph can be merged with the next one.
    RemoveCharactersFromParagraphInfo( relayoutData,
                                       numberOfCharacters,
                                       mergeWords,
                                       mergeParagraphs,
                                       textInfoIndicesBegin,
                                       textInfoIndicesEnd,
                                       textInfoMergeIndicesBegin,
                                       textInfoMergeIndicesEnd,
                                       paragraphLayout,
                                       removedTextActorsFromBegin,
                                       removedTextActorsFromEnd );

    if( mergeWords )
    {
      // Merges words pointed by textInfoMergeIndicesBegin.mWordIndex and textInfoMergeIndicesEnd.mWordIndex calculated previously.
      DALI_ASSERT_DEBUG( ( textInfoMergeIndicesBegin.mWordIndex < paragraphLayout.mWordsLayoutInfo.size() ) && "TextViewProcessor::UpdateTextInfo (delete). Word index (begin) out of bounds." );
      DALI_ASSERT_DEBUG( ( textInfoMergeIndicesEnd.mWordIndex < paragraphLayout.mWordsLayoutInfo.size() ) && "TextViewProcessor::UpdateTextInfo (delete). Word index (end) out of bounds." );

      WordLayoutInfo& firstWordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoMergeIndicesBegin.mWordIndex ) );
      WordLayoutInfo& lastWordLayout( *( paragraphLayout.mWordsLayoutInfo.begin() + textInfoMergeIndicesEnd.mWordIndex ) );

      MergeWord( firstWordLayout,
                 lastWordLayout );
    }

    // Store text-actors before removing them.
    const std::size_t endIndex = ( mergeWords && ( textInfoIndicesEnd.mWordIndex > 0u ) ) ? textInfoIndicesEnd.mWordIndex - 1u : textInfoIndicesEnd.mWordIndex; // text-actors from the last word may have been added in the merge above.
    CollectTextActorsFromWords( removedTextActorsFromMid, paragraphLayout, textInfoIndicesBegin.mWordIndex, endIndex );

    // Remove unwanted words using previously calculated indices. (including the last part of the merged word)
    paragraphLayout.mWordsLayoutInfo.erase( paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex, paragraphLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex );

    // Update paragraph info.
    UpdateLayoutInfo( paragraphLayout, layoutParameters.mLineHeightOffset );
  }// end delete text from same paragraph.

  if( mergeParagraphs )
  {
    // Merges paragraphs pointed by textInfoMergeIndicesBegin.mParagraphIndex and textInfoMergeIndicesEnd.mParagraphIndex calculated previously.

    ParagraphLayoutInfo& firstParagraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoMergeIndicesBegin.mParagraphIndex ) );

    const ParagraphLayoutInfo& lastParagraphLayout( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoMergeIndicesEnd.mParagraphIndex ) );

    MergeParagraph( firstParagraphLayout,
               lastParagraphLayout );
  }

  // Store text-actors before removing them.
  const std::size_t endIndex = ( mergeParagraphs && ( textInfoIndicesEnd.mParagraphIndex > 0u ) ) ? textInfoIndicesEnd.mParagraphIndex - 1u : textInfoIndicesEnd.mParagraphIndex; // text-actors from the last paragraph may have been added in the merge above.
  CollectTextActorsFromParagraphs( removedTextActorsFromMid,
                              relayoutData.mTextLayoutInfo,
                              textInfoIndicesBegin.mParagraphIndex,
                              endIndex );

  // Remove unwanted paragraphs using previously calculated indices. (including the last part of the merged paragraph)
  relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.erase( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndicesBegin.mParagraphIndex,
                                                       relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + textInfoIndicesEnd.mParagraphIndex );

  // Update text info.
  UpdateLayoutInfo( relayoutData.mTextLayoutInfo );

  // If the last character of the last paragraph is a new paragraph character, an empty paragraph need to be added.
  if( !relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.empty() )
  {
    const WordLayoutInfo lastWordLayout = GetLastWordLayoutInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end() - 1u ) );

    if( ParagraphSeparator == lastWordLayout.mType )
    {
      ParagraphLayoutInfo lastParagraphLayout;

      const CharacterLayoutInfo layoutInfo = GetLastCharacterLayoutInfo( lastWordLayout );
      lastParagraphLayout.mSize.height = layoutInfo.mSize.height;

      relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.push_back( lastParagraphLayout );

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

void UpdateTextInfo( float lineHeightOffset,
                     TextLayoutInfo& textLayoutInfo )
{
  // Updates the space between lines with the new offset value.

  float newTextHeight = 0.f;

  for( ParagraphLayoutInfoContainer::iterator paragraphIt = textLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = textLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphIt );

    paragraphLayoutInfo.mSize.height += ( lineHeightOffset - paragraphLayoutInfo.mLineHeightOffset );
    newTextHeight += paragraphLayoutInfo.mSize.height;

    paragraphLayoutInfo.mLineHeightOffset = lineHeightOffset;
  }

  textLayoutInfo.mWholeTextSize.height = newTextHeight;
}

void UpdateTextInfo( const TextStyle& style,
                     const TextStyle::Mask mask,
                     TextView::RelayoutData& relayoutData )
{
  // Change text style for all text-actors.

  for( ParagraphLayoutInfoContainer::iterator paragraphIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    ParagraphLayoutInfo& paragraph( *paragraphIt );

    for( WordLayoutInfoContainer::iterator wordIt = paragraph.mWordsLayoutInfo.begin(), wordEndIt = paragraph.mWordsLayoutInfo.end();
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
  } // end paragraphs
}

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
