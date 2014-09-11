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
#include <dali-toolkit/internal/controls/text-view/text-view-line-processor.h>

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

LineLayoutInfo::LineLayoutInfo()
: mSize(),
  mAscender( 0.f ),
  mLineHeightOffset( 0.f ),
  mWordsLayoutInfo(),
  mNumberOfCharacters( 0u )
{
}

LineLayoutInfo::LineLayoutInfo( const LineLayoutInfo& line )
: mSize( line.mSize ),
  mAscender( line.mAscender ),
  mLineHeightOffset( line.mLineHeightOffset ),
  mWordsLayoutInfo( line.mWordsLayoutInfo ),
  mNumberOfCharacters( line.mNumberOfCharacters )
{
}

LineLayoutInfo& LineLayoutInfo::operator=( const LineLayoutInfo& line )
{
  mSize = line.mSize;
  mAscender = line.mAscender;
  mLineHeightOffset = line.mLineHeightOffset;
  mWordsLayoutInfo = line.mWordsLayoutInfo;
  mNumberOfCharacters = line.mNumberOfCharacters;

  return *this;
}

void CreateLineInfo( const MarkupProcessor::StyledTextArray& line,
                     TextView::RelayoutData& relayoutData,
                     LineLayoutInfo& lineLayoutInfo )
{
  // Split the line in words.
  // TODO: Proper RTL support.
  MarkupProcessor::StyledTextArray convertedLine;
  if( TextProcessor::ContainsRightToLeftCharacter( line ) )
  {
    // If the text is bidirectional, the characters will be converted and reordered
    // as specified by the Unicode Bidirectional Algorithm.

    // Reorders the line and converts arabic glyphs (if any).
    TextProcessor::ConvertBidirectionalText( line,
                                             convertedLine,
                                             relayoutData.mCharacterLogicalToVisualMap,
                                             relayoutData.mCharacterVisualToLogicalMap);
  }
  else
  {
    // No bidirectional text to process.
    convertedLine = line;

    // Create trivial bidirectional map tables.
    std::size_t index = 0u;
    for( MarkupProcessor::StyledTextArray::const_iterator it = convertedLine.begin(), endIt = convertedLine.end(); it != endIt; ++it )
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

  // Split the line in words
  std::vector<MarkupProcessor::StyledTextArray> words;
  TextProcessor::SplitInWords( convertedLine, words );

  // if last word has a new line separator, create a new word.
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
          // Last character of this word is a new line character.

          // Remove line separator character from current word.
          styledText.mText.Remove( length - 1u, 1u );

          // Create a new word with the line separator character.
          MarkupProcessor::StyledText newLineText( Text( styledText.mText[length-1u] ), styledText.mStyle );

          MarkupProcessor::StyledTextArray newLineWord;
          newLineWord.push_back( newLineText );

          words.push_back( newLineWord );
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

    // Update layout info for the current line.
    lineLayoutInfo.mAscender = std::max( lineLayoutInfo.mAscender, wordLayoutInfo.mAscender );
    lineLayoutInfo.mNumberOfCharacters += wordLayoutInfo.mCharactersLayoutInfo.size();
    UpdateSize( lineLayoutInfo.mSize, wordLayoutInfo.mSize );

    // Add the word to the current line.
    lineLayoutInfo.mWordsLayoutInfo.push_back( wordLayoutInfo );
  } // end of words
}

void UpdateLayoutInfo( LineLayoutInfo& lineLayoutInfo, const float lineHeightOffset )
{
  // Update layout info.
  lineLayoutInfo.mSize = Size::ZERO;
  lineLayoutInfo.mAscender = 0.f;
  lineLayoutInfo.mNumberOfCharacters = 0u;
  for( WordLayoutInfoContainer::iterator it = lineLayoutInfo.mWordsLayoutInfo.begin(), endIt = lineLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& word( *it );

    UpdateSize( lineLayoutInfo.mSize, word.mSize );
    lineLayoutInfo.mAscender = std::max( lineLayoutInfo.mAscender, word.mAscender );
    lineLayoutInfo.mNumberOfCharacters += word.mCharactersLayoutInfo.size();
  }

  lineLayoutInfo.mSize.height += lineHeightOffset;
  lineLayoutInfo.mLineHeightOffset = lineHeightOffset;
}

void RemoveWordsFromLine( std::size_t wordIndex,
                          std::size_t numberOfWords,
                          float lineHeightOffset,
                          LineLayoutInfo& lineLayout )
{
  // Removes words from a line.

  // * Check if words or lines can be merged after removing a number of words or a line separator needs to be done outside this method.

  // Remove words from layout info.
  lineLayout.mWordsLayoutInfo.erase( lineLayout.mWordsLayoutInfo.begin() + wordIndex,
                                     lineLayout.mWordsLayoutInfo.begin() + ( wordIndex + numberOfWords ) );

  UpdateLayoutInfo( lineLayout, lineHeightOffset );
}

void RemoveCharactersFromLineInfo( TextView::RelayoutData& relayoutData,
                                   const std::size_t numberOfCharacters,
                                   bool& mergeWords,
                                   bool& mergeLines,
                                   TextInfoIndices& textInfoIndicesBegin,
                                   TextInfoIndices& textInfoIndicesEnd,
                                   TextInfoIndices& textInfoMergeIndicesBegin,
                                   TextInfoIndices& textInfoMergeIndicesEnd,
                                   LineLayoutInfo& lineLayout,
                                   std::vector<TextActor>& removedTextActorsFromFirstWord,
                                   std::vector<TextActor>& removedTextActorsFromLastWord )
{
  const TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  if( textInfoIndicesBegin.mWordIndex < textInfoIndicesEnd.mWordIndex )
  {
    // Deleted text is from different words. The two different words may be merged.

    // Get first word.
    WordLayoutInfo& firstWordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex ) );

    // Get last word.
    WordLayoutInfo& lastWordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex ) );

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
      WordLayoutInfo& previousWordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesBegin.mWordIndex - 1u ) );
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
      else if( ( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() ) && ( textInfoIndicesEnd.mWordIndex + 1u < lineLayout.mWordsLayoutInfo.size() ) )
      {
        // Last word is going to be removed completely.
        // Check if the word after could be merged.

        // Get word after.
        WordLayoutInfo& afterWordLayout( *( lineLayout.mWordsLayoutInfo.begin() + textInfoIndicesEnd.mWordIndex + 1u ) );
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

    if( ( textInfoIndicesEnd.mCharacterIndex + 1u == lastWordLayout.mCharactersLayoutInfo.size() ) && ( textInfoIndicesEnd.mWordIndex + 1u == lineLayout.mWordsLayoutInfo.size() ) )
    {
      // Last word of the line is going to be removed completely.
      // Check if it's a line separator.

      if( LineSeparator == lastWordLayout.mType )
      {
        // The line separator is going to be removed.
        if( textInfoIndicesBegin.mLineIndex + 1u < textLayoutInfo.mLinesLayoutInfo.size() )
        {
          //  Line need to be merged.
          textInfoMergeIndicesBegin.mLineIndex = textInfoIndicesBegin.mLineIndex;
          textInfoMergeIndicesEnd.mLineIndex = textInfoIndicesBegin.mLineIndex + 1u;
          mergeLines= true;

          ++textInfoIndicesBegin.mLineIndex; // increase both indices,
          textInfoIndicesEnd.mLineIndex +=2u; // will delete last line.
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
                                  mergeLines,
                                  textInfoIndicesBegin,
                                  textInfoIndicesEnd,
                                  textInfoMergeIndicesBegin,
                                  textInfoMergeIndicesEnd,
                                  lineLayout,
                                  removedTextActorsFromFirstWord );
  } // word indices
}

void SplitLine( const TextInfoIndices& indices,
                const PointSize& lineHeightOffset,
                LineLayoutInfo& firstLineLayoutInfo,
                LineLayoutInfo& lastLineLayoutInfo )
{
  // Splits a line in two.
  // A word may be split in two as well.

  // * Split the word within the line.
  // * Add last part of the word to the new line.
  // * Add words from wordPosition + 1 to the end.
  // * Update layout info of the last line.
  // * Remove words added to the last part of the line from the first line.

  // early returns!!
  if( ( 0u == indices.mWordIndex ) && ( 0u == indices.mCharacterIndex ) )
  {
    // the whole line goes to the last part.
    lastLineLayoutInfo = firstLineLayoutInfo;

    firstLineLayoutInfo = LineLayoutInfo();

    return;
  }

  if( !firstLineLayoutInfo.mWordsLayoutInfo.empty() )
  {
    const std::size_t numberOfWords = firstLineLayoutInfo.mWordsLayoutInfo.size();
    if( indices.mWordIndex == numberOfWords - 1u )
    {
      const WordLayoutInfo& word( *( firstLineLayoutInfo.mWordsLayoutInfo.end() - 1u ) );
      if( indices.mCharacterIndex == word.mCharactersLayoutInfo.size() )
      {
        // the whole line goes to the first part.

        // Just delete whatever there is in the last part of the line.
        lastLineLayoutInfo = LineLayoutInfo();

        return;
      }
    }
  }

  lastLineLayoutInfo = LineLayoutInfo();

  // 1) Split the word whitin the line.
  WordLayoutInfo& firstWordLayoutInfo( *( firstLineLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex ) );
  WordLayoutInfo lastWordLayoutInfo;

  SplitWord( indices.mCharacterIndex,
             firstWordLayoutInfo,
             lastWordLayoutInfo );

  // 2) Add last part of the word to the new line.
  if( !lastWordLayoutInfo.mCharactersLayoutInfo.empty() )
  {
    lastLineLayoutInfo.mWordsLayoutInfo.push_back( lastWordLayoutInfo );
  }

  // 3) Add words from word-position + 1 to the end.
  lastLineLayoutInfo.mWordsLayoutInfo.insert( lastLineLayoutInfo.mWordsLayoutInfo.end(),
                                              firstLineLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex + 1u,
                                              firstLineLayoutInfo.mWordsLayoutInfo.end() );

  // 4) update layout info of the last line.
  for( WordLayoutInfoContainer::iterator it = lastLineLayoutInfo.mWordsLayoutInfo.begin(), endIt = lastLineLayoutInfo.mWordsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    WordLayoutInfo& layoutInfo( *it );

    UpdateSize( lastLineLayoutInfo.mSize, layoutInfo.mSize );
    lastLineLayoutInfo.mNumberOfCharacters += layoutInfo.mCharactersLayoutInfo.size();
    lastLineLayoutInfo.mAscender = std::max( lastLineLayoutInfo.mAscender, layoutInfo.mAscender );
  }
  lastLineLayoutInfo.mSize.height += lineHeightOffset;
  lastLineLayoutInfo.mLineHeightOffset = lineHeightOffset;

  // 5) Remove words added to the last part of the line from the first line.

  // if the number of characters of the last word of the first line is zero, it should be removed.
  const std::size_t index = ( firstWordLayoutInfo.mCharactersLayoutInfo.empty() ? indices.mWordIndex : indices.mWordIndex + 1u );

  firstLineLayoutInfo.mWordsLayoutInfo.erase( firstLineLayoutInfo.mWordsLayoutInfo.begin() + index, firstLineLayoutInfo.mWordsLayoutInfo.end() );

  // 6) update layout info of the first line.
  UpdateLayoutInfo( firstLineLayoutInfo, lineHeightOffset );
}

void MergeLine( LineLayoutInfo& firstLineLineLayoutInfo,
                const LineLayoutInfo& lastLineLayoutInfo )
{
  // Merges two given lines.
  //
  // Can't merge two lines if the last word of the first one is a line separator (new line character)

  // Early returns.

  if( lastLineLayoutInfo.mWordsLayoutInfo.empty() )
  {
    // Nothing to merge if last line is empty.
    return;
  }

  if( firstLineLineLayoutInfo.mWordsLayoutInfo.empty() )
  {
    // If first line is empty, just copy the last line to the first one.
    firstLineLineLayoutInfo = lastLineLayoutInfo;

   return;
  }

  // Check the last word of the first line doesn't finish with a new line character.
  WordLayoutInfo& lastWordLayout( *( firstLineLineLayoutInfo.mWordsLayoutInfo.end() - 1u ) );
  if( LineSeparator == lastWordLayout.mType )
  {
    DALI_ASSERT_ALWAYS( !"TextViewProcessor::MergeLine(). ERROR: A line can't be merged to another line which finishes with a new line character." );
  }

  // If the las word of the first line or the first word of the last line is a white space, both lines can be concatenated.
  // Otherwise both words need to be merged first.
  const WordLayoutInfo& firstWordLayout( *lastLineLayoutInfo.mWordsLayoutInfo.begin() );

  std::size_t index = 0u;
  if( ( WordSeparator != lastWordLayout.mType ) && ( WordSeparator != firstWordLayout.mType ) && ( LineSeparator != firstWordLayout.mType ) )
  {
    // Last word of the first line is not a word separator and first word of the last line is not a word or line separator.
    // Words need to be merged.

    MergeWord( lastWordLayout,
               firstWordLayout );

    // After merging two words, the rest of the words need to be added.
    ++index; // By increasing this index the word already merged won't be added again.
  }

  // Merge layout info

  // Insert the layout of the words.
  firstLineLineLayoutInfo.mWordsLayoutInfo.insert( firstLineLineLayoutInfo.mWordsLayoutInfo.end(),
                                                   lastLineLayoutInfo.mWordsLayoutInfo.begin() + index, lastLineLayoutInfo.mWordsLayoutInfo.end() );

  // Update the size and other layout parameters.
  UpdateSize( firstLineLineLayoutInfo.mSize, lastLineLayoutInfo.mSize );
  firstLineLineLayoutInfo.mAscender = std::max( firstLineLineLayoutInfo.mAscender, lastLineLayoutInfo.mAscender );
  firstLineLineLayoutInfo.mLineHeightOffset = std::max( firstLineLineLayoutInfo.mLineHeightOffset, lastLineLayoutInfo.mLineHeightOffset );
  firstLineLineLayoutInfo.mNumberOfCharacters += lastLineLayoutInfo.mNumberOfCharacters;
}

WordLayoutInfo GetLastWordLayoutInfo( const LineLayoutInfo& lineLayoutInfo )
{
  WordLayoutInfo layoutInfo;

  if( !lineLayoutInfo.mWordsLayoutInfo.empty() )
  {
    layoutInfo = *( lineLayoutInfo.mWordsLayoutInfo.end() - 1u );
  }

  return layoutInfo;
}

CharacterLayoutInfo GetFirstCharacterLayoutInfo( const LineLayoutInfo& lineLayoutInfo )
{
  CharacterLayoutInfo layoutInfo;

  if( !lineLayoutInfo.mWordsLayoutInfo.empty() )
  {
    const WordLayoutInfo& wordInfo( *lineLayoutInfo.mWordsLayoutInfo.begin() );

    layoutInfo = GetFirstCharacterLayoutInfo( wordInfo );
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

    CollectTextActorsFromWords( textActors, line, 0u, line.mWordsLayoutInfo.size() );
  }
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
