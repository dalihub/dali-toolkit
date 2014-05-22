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

#include "split-by-word-policies.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include "relayout-utilities.h"
#include "text-view-processor.h"
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace SplitByWord
{

namespace
{

Vector3 OriginalPosition( const TextViewRelayout::RelayoutParameters& relayoutParameters,
                          const TextView::LayoutParameters& layoutParameters,
                          TextView::RelayoutData& relayoutData )
{
  const float wordOffset = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.x );
  const float previousPositionY = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.y );

  if( relayoutParameters.mIsNewLine ||
      relayoutParameters.mIsFirstCharacter ||
      ( relayoutParameters.mIsFirstCharacterOfWord && ( wordOffset + relayoutParameters.mWordSize.width > relayoutData.mTextViewSize.width ) ) )
  {
    if( !relayoutParameters.mIsNewLine &&
        ( relayoutParameters.mIsWhiteSpace || relayoutParameters.mIsNewLineCharacter ) )
    {
      // Current character is a white space. Don't want to move a white space to the next line.
      // These white spaces are placed just in the edge.
      return Vector3( relayoutData.mTextViewSize.width - relayoutParameters.mWordSize.width, relayoutParameters.mPositionOffset.y, 0.f );
    }
    else
    {
      // Calculates the length of the portion of the line which doesn't exceed the text-view's width and the max character height for the current line.
      TextViewRelayout::SubLineLayoutInfo subLineInfo;
      subLineInfo.mLineLength = 0.f;
      subLineInfo.mMaxCharHeight = 0.f;
      subLineInfo.mMaxAscender = 0.f;
      const TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + relayoutParameters.mIndices.mLineIndex ) );

      TextViewRelayout::CalculateSubLineLayout( relayoutData.mTextViewSize.width,
                                                relayoutParameters.mIndices,
                                                lineLayoutInfo,
                                                TextViewRelayout::WrapByWord,
                                                1.f, // Shrink factor
                                                subLineInfo );

      if( subLineInfo.mLineLength < Math::MACHINE_EPSILON_1000 )
      {
        // It may mean there is a word which is actually longer than the width of the text-view.
        // In that case the length of this word is needed.
        if( !lineLayoutInfo.mWordGroupsLayoutInfo.empty() )
        {
          const TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *( lineLayoutInfo.mWordGroupsLayoutInfo.begin() + relayoutParameters.mIndices.mGroupIndex ) );
          if( !wordGroupLayoutInfo.mWordsLayoutInfo.empty() )
          {
            const TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *( wordGroupLayoutInfo.mWordsLayoutInfo.begin() + relayoutParameters.mIndices.mWordIndex ) );
            subLineInfo.mLineLength = wordLayoutInfo.mSize.width;
          }
        }
      }

      // Stores some info to calculate the line justification in a post-process.
      TextView::LineJustificationInfo justificationInfo;

      justificationInfo.mIndices = relayoutParameters.mIndices;
      justificationInfo.mLineLength = subLineInfo.mLineLength;

      relayoutData.mLineJustificationInfo.push_back( justificationInfo );

      Toolkit::TextView::LineLayoutInfo lineInfo;
      lineInfo.mCharacterGlobalIndex = relayoutParameters.mCharacterGlobalIndex;    // Index to the first character of the next line.
      lineInfo.mSize = Size( subLineInfo.mLineLength, subLineInfo.mMaxCharHeight ); // Size of this piece of line.
      lineInfo.mAscender = subLineInfo.mMaxAscender;                                // Ascender of this piece of line.
      relayoutData.mLines.push_back( lineInfo );

      return Vector3( 0.f, previousPositionY + subLineInfo.mMaxCharHeight + layoutParameters.mLineHeightOffset, 0.f );
    }
  }
  else
  {
    return Vector3( wordOffset, previousPositionY, 0.f );
  }
}

/**
 * Calculates character position.
 * @param[in] relayoutParameters Temporary layout parameters (previous size, previous position, ... )
 * @param[in] layoutParameters The layout parameters.
 * @param[in] relayoutData The text-view's data structures.
 * @return The character's position.
 */
Vector3 SplitWhenExceedPosition( const TextViewRelayout::RelayoutParameters& relayoutParameters,
                                 const TextView::LayoutParameters& layoutParameters,
                                 TextView::RelayoutData& relayoutData )
{
  const float wordOffset = ( relayoutParameters.mIsFirstCharacter ? 0.f :  relayoutParameters.mPositionOffset.x );
  const float previousPositionY = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.y );

  if( ( relayoutParameters.mIsNewLine || relayoutParameters.mIsFirstCharacter ) ||
      ( relayoutParameters.mIsFirstCharacterOfWord && ( wordOffset + relayoutParameters.mWordSize.width > relayoutData.mTextViewSize.width ) ) ||
      ( wordOffset + relayoutParameters.mCharacterSize.width > relayoutData.mTextViewSize.width ) )
  {
    if( !relayoutParameters.mIsNewLine &&
        ( relayoutParameters.mIsWhiteSpace || relayoutParameters.mIsNewLineCharacter ) )
    {
      // Current character is a white space. Don't want to move a white space to the next line.
      // These white spaces are placed just in the edge.
      return Vector3( relayoutData.mTextViewSize.width - relayoutParameters.mWordSize.width, relayoutParameters.mPositionOffset.y, 0.f );
    }
    else
    {
      // Calculates the line length and the max character height for the current line.
      TextViewRelayout::SubLineLayoutInfo subLineInfo;
      subLineInfo.mLineLength = 0.f;
      subLineInfo.mMaxCharHeight = 0.f;
      subLineInfo.mMaxAscender = 0.f;
      const TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + relayoutParameters.mIndices.mLineIndex ) );

      TextViewRelayout::CalculateSubLineLayout( relayoutData.mTextViewSize.width,
                                                relayoutParameters.mIndices,
                                                lineLayoutInfo,
                                                TextViewRelayout::WrapByWordAndSplit,
                                                1.f, // Shrink factor.
                                                subLineInfo );

      // Stores some info to calculate the line justification in a post-process.
      TextView::LineJustificationInfo justificationInfo;

      justificationInfo.mIndices = relayoutParameters.mIndices;
      justificationInfo.mLineLength = subLineInfo.mLineLength;

      relayoutData.mLineJustificationInfo.push_back( justificationInfo );

      Toolkit::TextView::LineLayoutInfo lineInfo;
      lineInfo.mCharacterGlobalIndex = relayoutParameters.mCharacterGlobalIndex;    // Index to the first character of the next line.
      lineInfo.mSize = Size( subLineInfo.mLineLength, subLineInfo.mMaxCharHeight ); // Size of this piece of line.
      lineInfo.mAscender = subLineInfo.mMaxAscender;                                // Ascender of this piece of line.
      relayoutData.mLines.push_back( lineInfo );

      return Vector3( 0.f, previousPositionY + subLineInfo.mMaxCharHeight + layoutParameters.mLineHeightOffset, 0.f );
    }
  }
  else
  {
    return Vector3( wordOffset, previousPositionY, 0.f );
  }
}

/**
 * Calculates character position.
 * @param[in] relayoutParameters Temporary layout parameters (previous size, previous position, ... )
 * @param[in] layoutParameters The layout parameters.
 * @param[in] relayoutData The text-view's data structures.
 * @return The character's position.
 */
Vector3 ShrinkWidthWhenExceedPosition( const TextViewRelayout::RelayoutParameters& relayoutParameters,
                                       const TextView::LayoutParameters& layoutParameters,
                                       TextView::RelayoutData& relayoutData )
{
  const float wordOffset = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.x );
  const float previousPositionY = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.y );
  const Size wordSize = relayoutParameters.mWordSize * relayoutData.mShrinkFactor;

  if( ( relayoutParameters.mIsNewLine || relayoutParameters.mIsFirstCharacter ) ||                                           // isNewLine is true when '\n' is found.
      ( relayoutParameters.mIsFirstCharacterOfWord && ( wordOffset + wordSize.width > relayoutData.mTextViewSize.width ) ) ) // The word doesn't fit in the parent width.
  {
    if( !relayoutParameters.mIsNewLine &&
        ( relayoutParameters.mIsWhiteSpace || relayoutParameters.mIsNewLineCharacter ) )
    {
      // Current character is a white space. Don't want to move a white space to the next line.
      // These white spaces are placed just in the edge.
      return Vector3( relayoutData.mTextViewSize.width - relayoutParameters.mWordSize.width, relayoutParameters.mPositionOffset.y, 0.f );
    }
    else
    {
      // Calculates the line length and the max character height for the current line.
      TextViewRelayout::SubLineLayoutInfo subLineInfo;
      subLineInfo.mLineLength = 0.f;
      subLineInfo.mMaxCharHeight = 0.f;
      subLineInfo.mMaxAscender = 0.f;
      const TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + relayoutParameters.mIndices.mLineIndex ) );

      TextViewRelayout::CalculateSubLineLayout( relayoutData.mTextViewSize.width,
                                                relayoutParameters.mIndices,
                                                lineLayoutInfo,
                                                TextViewRelayout::WrapByWord,
                                                relayoutData.mShrinkFactor,
                                                subLineInfo );

      // Stores some info to calculate the line justification in a post-process.
      TextView::LineJustificationInfo justificationInfo;

      justificationInfo.mIndices = relayoutParameters.mIndices;
      justificationInfo.mLineLength = subLineInfo.mLineLength;

      relayoutData.mLineJustificationInfo.push_back( justificationInfo );

      Toolkit::TextView::LineLayoutInfo lineInfo;
      lineInfo.mCharacterGlobalIndex = relayoutParameters.mCharacterGlobalIndex;    // Index to the first character of the next line.
      lineInfo.mSize = Size( subLineInfo.mLineLength, subLineInfo.mMaxCharHeight ); // Size of this piece of line.
      lineInfo.mAscender = subLineInfo.mMaxAscender;                                // Ascender of this piece of line.
      relayoutData.mLines.push_back( lineInfo );

      return Vector3( 0.f, previousPositionY + subLineInfo.mMaxCharHeight + layoutParameters.mLineHeightOffset * relayoutData.mShrinkFactor, 0.f );
    }
  }
  else
  {
    return Vector3( wordOffset, previousPositionY, 0.f );
  }
}

void CalculatePositionsForShrinkWhenExceed( TextView::RelayoutData& relayoutData,
                                            const TextView::LayoutParameters& layoutParameters,
                                            const float shrinkFactor,
                                            float& newTextHeight )
{
  const float parentWidth = relayoutData.mTextViewSize.width;
  TextViewProcessor::TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  relayoutData.mLineJustificationInfo.clear();

  // Reset the text height. This value is returned in order to shrink further or not the text.
  newTextHeight = 0.f;

  // Whether the first character is being processed.
  bool isFirstChar = true;

  // Stores the size of the previous character.
  Size previousSize;
  // Stores the position of the previous character.
  Vector3 previousPosition;

  // Reset the index of lines.
  TextViewProcessor::TextInfoIndices indices;

  // Whether the last character of the whole text is a new line char.
  // This information is used to increase or not the height of the whole text by one line.
  // Increase the whole text's height by one line is useful i.e. in TextInput to place the cursor
  // after pressing 'Enter' in the last line.
  bool isLastCharacterNewLineChar = false;
  // Stores the height of the last character. This height used to be added to the whole text height if
  // isLastCharacterNewLineChar is true.
  float lastCharHeight = 0.f;

  relayoutData.mLines.clear();
  std::size_t characterGlobalIndex = 0;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineIt = textLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = textLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt, ++indices.mLineIndex )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineIt );

    // The next character is in a new line.
    bool isNewLine = true;

    // Reset the index of groups of words.
    indices.mGroupIndex = 0;

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin(), groupEndIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
         groupIt != groupEndIt;
         ++groupIt, ++indices.mGroupIndex )
    {
      TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *groupIt );

      // Reset the index of words.
      indices.mWordIndex = 0;

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin(), wordEndIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
           wordIt != wordEndIt;
           ++wordIt, ++indices.mWordIndex )
      {
        TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordIt );

        // Reset the index of the character.
        indices.mCharacterIndex = 0;

        // Whether current character is the first of the word.
        bool isFirstCharOfWord = true;
        const float wordOffset = previousPosition.x + previousSize.width;

        isLastCharacterNewLineChar = ( TextViewProcessor::LineSeparator == wordLayoutInfo.mType );

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator charIt = wordLayoutInfo.mCharactersLayoutInfo.begin(), charEndIt = wordLayoutInfo.mCharactersLayoutInfo.end();
             charIt != charEndIt;
             ++charIt, ++indices.mCharacterIndex )
        {
          TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *charIt );
          lastCharHeight = characterLayoutInfo.mSize.height * shrinkFactor;

          const float previousPositionY = isFirstChar ? 0.f : previousPosition.y;

          if( ( isNewLine || isFirstChar ) ||
              ( isFirstCharOfWord && ( wordOffset + wordLayoutInfo.mSize.width * shrinkFactor > parentWidth ) ) )
          {
            isFirstChar = false;

            // Calculates the line length and the max character height for the current line.
            TextViewRelayout::SubLineLayoutInfo subLineInfo;
            subLineInfo.mLineLength = 0.f;
            subLineInfo.mMaxCharHeight = 0.f;
            subLineInfo.mMaxAscender = 0.f;
            TextViewRelayout::CalculateSubLineLayout( parentWidth,
                                                      indices,
                                                      lineLayoutInfo,
                                                      TextViewRelayout::WrapByWord,
                                                      shrinkFactor,
                                                      subLineInfo );

            characterLayoutInfo.mPosition = Vector3( 0.f, previousPositionY + subLineInfo.mMaxCharHeight + layoutParameters.mLineHeightOffset * shrinkFactor, 0.f );

            newTextHeight += subLineInfo.mMaxCharHeight + layoutParameters.mLineHeightOffset * shrinkFactor;

            Toolkit::TextView::LineLayoutInfo lineInfo;
            lineInfo.mCharacterGlobalIndex = characterGlobalIndex;                        // Index to the first character of the next line.
            lineInfo.mSize = Size( subLineInfo.mLineLength, subLineInfo.mMaxCharHeight ); // Size of this piece of line.
            lineInfo.mAscender = subLineInfo.mMaxAscender;                                // Ascender of this piece of line.
            relayoutData.mLines.push_back( lineInfo );


            // Stores some info to calculate the line justification in a post-process.
            TextView::LineJustificationInfo justificationInfo;

            justificationInfo.mIndices = indices;
            justificationInfo.mLineLength = subLineInfo.mLineLength;

            relayoutData.mLineJustificationInfo.push_back( justificationInfo );
          }
          else
          {
            characterLayoutInfo.mPosition = previousPosition + Vector3( previousSize.width, 0.f, 0.f );
          }

          // Get last line info and calculate the bearing.
          const Toolkit::TextView::LineLayoutInfo& lineInfo( *( relayoutData.mLines.end() - 1 ) );
          const float bearingOffset = ( ( lineInfo.mSize.height - lineInfo.mAscender ) - ( characterLayoutInfo.mSize.height - characterLayoutInfo.mAscender ) ) * shrinkFactor;

          previousSize = characterLayoutInfo.mSize * shrinkFactor;
          previousPosition = characterLayoutInfo.mPosition;
          characterLayoutInfo.mPosition.y -= bearingOffset;
          isFirstCharOfWord = false;
          isNewLine = false;

          ++characterGlobalIndex;
        }
      }
    }
  }

  if( isLastCharacterNewLineChar )
  {
    newTextHeight += lastCharHeight + layoutParameters.mLineHeightOffset * shrinkFactor;
  }
}

float RelayoutForShrinkToFit( TextView::RelayoutData& relayoutData,
                              const TextView::LayoutParameters& layoutParameters )
{
  const Size& textViewSize = relayoutData.mTextViewSize;
  TextViewProcessor::TextLayoutInfo& textLayoutInfo = relayoutData.mTextLayoutInfo;

  // First step is assure the longest word fits in the text view width.
  float shrinkFactor = ( textLayoutInfo.mMaxWordWidth > textViewSize.width ? textViewSize.width / textLayoutInfo.mMaxWordWidth : 1.f );

  // Out parameter. Will store the new text height after relayout the text.
  float newTextHeight = 0.f;

  // Relayout the text for the given character's sizes.
  CalculatePositionsForShrinkWhenExceed( relayoutData,
                                         layoutParameters,
                                         shrinkFactor,
                                         newTextHeight );

  if( newTextHeight > textViewSize.height )
  {
    // After relayouting, the text exceeds the text view height.
    // Find a new scale factor to fit all the text in the text view size is needed.

    // The next algorithm does some iterations to calculate an acceptable scale factor.
    // Some magic numbers are defined.

    const float MIN_RATIO( 0.90f );         // The algorithm finishes if the ratio
    const float MAX_RATIO( 1.00f );         // new_text_height / text_view_height is between this two values
    const unsigned int MAX_ITERATIONS( 8 ); // or max_iteration is reached.

    float ratio = newTextHeight / textViewSize.height;

    float maxScaleFactor = shrinkFactor;                        // bigger scale factors than maxScaleFactor will produce a too big text.
    float minScaleFactor = shrinkFactor * ( textViewSize.height / newTextHeight ); // smaller scale factors than minScaleFactor will produce a too small text.

    for( unsigned int iterations = 0; ( ( MIN_RATIO > ratio ) || ( ratio > MAX_RATIO )  ) && ( iterations < MAX_ITERATIONS ); ++iterations )
    {
      // Calculates the new scale factor.
      // The new scale factor is always between the min and max scale factors.
      // If ratio < 1 it means the text is too small and a bigger scale factor is needed. In this case the algorithm selects a new scale factor close to
      // minScaleFactor. Alternatively if the text is too big a new scale factor close to maxScaleFactor is selected.
      // This allows the text shrink or grow smoothly.
      shrinkFactor = minScaleFactor + ( ratio < 1.f ? 0.4f : 0.6f ) * ( maxScaleFactor - minScaleFactor );

      CalculatePositionsForShrinkWhenExceed( relayoutData, // Relayout the text for the given character's sizes.
                                             layoutParameters,
                                             shrinkFactor,
                                             newTextHeight );

      // Calculates the new text size ratio. It allows update the min and max scale factors.
      // If the ratio is not good enough a new scale factor between min and max could be used in next iteration.
      ratio = newTextHeight / textViewSize.height;
      if( ratio < 1.f )
      {
        minScaleFactor = shrinkFactor;
      }
      else
      {
        maxScaleFactor = shrinkFactor;
      }
    }

    if( ratio > MAX_RATIO )
    {
      // The algorithm didn't find an acceptable scale factor.
      // In that case the text is shrunk to fit in the boundaries of the text view actor.
      shrinkFactor = minScaleFactor;

      CalculatePositionsForShrinkWhenExceed( relayoutData,
                                             layoutParameters,
                                             shrinkFactor,
                                             newTextHeight );
    }
  }

  return shrinkFactor;
}

void CalculateSizeAndPosition( const TextView::LayoutParameters& layoutParameters,
                               TextView::RelayoutData& relayoutData )
{
  TextViewRelayout::RelayoutParameters relayoutParameters;

  // clear
  relayoutData.mCharacterLayoutInfoTable.clear();
  relayoutData.mLines.clear();
  relayoutData.mTextSizeForRelayoutOption = Size();

  // Calculates the text size for split by char.
  Vector4 minMaxXY( std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::min(),
                    std::numeric_limits<float>::min() );

  relayoutData.mShrinkFactor = 1.f; // Shrink factor used when the exceed policy contains ShrinkToFit

  if( TextView::Shrink== layoutParameters.mExceedPolicy )
  {
    // Relays-out the text for the shrink to fit policy.
    relayoutData.mShrinkFactor = RelayoutForShrinkToFit( relayoutData, layoutParameters );
  }
  else if( TextView::ShrinkOriginal == layoutParameters.mExceedPolicy )
  {
    relayoutData.mShrinkFactor = ( relayoutData.mTextLayoutInfo.mMaxWordWidth > relayoutData.mTextViewSize.width ? relayoutData.mTextViewSize.width / relayoutData.mTextLayoutInfo.mMaxWordWidth : 1.f );
  }

  relayoutParameters.mPositionOffset = Vector3::ZERO;
  relayoutParameters.mIsFirstCharacter = true;
  relayoutParameters.mIndices.mLineIndex = 0;
  relayoutParameters.mCharacterGlobalIndex = 0;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
       endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt, ++relayoutParameters.mIndices.mLineIndex )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    relayoutParameters.mIsNewLine = true;
    relayoutParameters.mLineSize = lineLayoutInfo.mSize;
    relayoutParameters.mIndices.mGroupIndex = 0;

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin(),
         endGroupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
         groupLayoutIt != endGroupLayoutIt;
         ++groupLayoutIt, ++relayoutParameters.mIndices.mGroupIndex )
    {
      TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *groupLayoutIt );

      relayoutParameters.mIndices.mWordIndex = 0;

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin(),
           endWordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
           wordLayoutIt != endWordLayoutIt;
           ++wordLayoutIt, ++relayoutParameters.mIndices.mWordIndex )
      {
        TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );
        relayoutParameters.mIsWhiteSpace = TextViewProcessor::WordSeparator == wordLayoutInfo.mType;
        relayoutParameters.mIsNewLineCharacter = TextViewProcessor::LineSeparator == wordLayoutInfo.mType;

        relayoutParameters.mIsFirstCharacterOfWord = true;
        relayoutParameters.mWordSize = wordLayoutInfo.mSize;
        relayoutParameters.mIndices.mCharacterIndex = 0;

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
             ( characterLayoutIt != endCharacterLayoutIt );
             ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex )
        {
          TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

          relayoutParameters.mCharacterSize = characterLayoutInfo.mSize;

          switch( layoutParameters.mExceedPolicy )
          {
            case TextView::OriginalShrink:
            case TextView::SplitShrink:
            case TextView::ShrinkFade:
            {
              DALI_LOG_WARNING( "SplitByWord::CalculateSizeAndPosition() policy not implemented.\n" );
              break;
            }
            case TextView::Original:
            case TextView::OriginalFade:
            case TextView::FadeOriginal:
            case TextView::Fade:
            case TextView::EllipsizeEndOriginal:
            case TextView::EllipsizeEnd: // Fall Through
            {
              characterLayoutInfo.mPosition = OriginalPosition( relayoutParameters,
                                                                layoutParameters,
                                                                relayoutData );

              relayoutParameters.mPositionOffset = characterLayoutInfo.mPosition + Vector3( characterLayoutInfo.mSize.width, 0.f, 0.f );
              break;
            }
            case TextView::SplitOriginal:
            {
              characterLayoutInfo.mPosition = SplitWhenExceedPosition( relayoutParameters,
                                                                       layoutParameters,
                                                                       relayoutData );

              relayoutParameters.mPositionOffset = characterLayoutInfo.mPosition + Vector3( characterLayoutInfo.mSize.width, 0.f, 0.f );
              break;
            }
            case TextView::SplitFade:
            {
              characterLayoutInfo.mPosition = SplitWhenExceedPosition( relayoutParameters,
                                                                       layoutParameters,
                                                                       relayoutData );

              relayoutParameters.mPositionOffset = characterLayoutInfo.mPosition + Vector3( characterLayoutInfo.mSize.width, 0.f, 0.f );
              break;
            }
            case TextView::ShrinkOriginal:
            {
              characterLayoutInfo.mPosition = ShrinkWidthWhenExceedPosition( relayoutParameters,
                                                                             layoutParameters,
                                                                             relayoutData );

              relayoutParameters.mPositionOffset = characterLayoutInfo.mPosition + Vector3( characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor, 0.f, 0.f );
              break;
            }
            case TextView::Shrink:
            {
              // Does nothing. All the job has been done in the RelayoutForShrinkToFit() function.
              break;
            }
            default:
            {
              DALI_LOG_WARNING( "SplitByWord::CalculateSizeAndPosition() policy combination not possible.\n" );
            }
          }

          // Get last line info and calculate the bearing (used to align glyphs with the baseline).
          if( TextView::Shrink != layoutParameters.mExceedPolicy )
          {
            TextViewRelayout::CalculateBearing( characterLayoutInfo, relayoutData );
          }

          // updates min and max position to calculate the text size for split by word.
          TextViewRelayout::UpdateLayoutInfoTable( minMaxXY,
                                                   wordGroupLayoutInfo,
                                                   wordLayoutInfo,
                                                   characterLayoutInfo,
                                                   relayoutParameters,
                                                   relayoutData );

          ++relayoutParameters.mCharacterGlobalIndex;
          relayoutParameters.mIsFirstCharacter = false;
          relayoutParameters.mIsFirstCharacterOfWord = false;
          relayoutParameters.mIsNewLine = false;
        } // end characters
      } // end words
    } // end group of words
  } // end lines

  if( relayoutData.mCharacterLayoutInfoTable.empty() )
  {
    relayoutData.mTextSizeForRelayoutOption = Size();
  }
  else
  {
    relayoutData.mTextSizeForRelayoutOption.width = minMaxXY.z - minMaxXY.x;
    relayoutData.mTextSizeForRelayoutOption.height = minMaxXY.w - minMaxXY.y;
  }

  // Check if the last character is a new line character. In that case the height should be added.
  if( !relayoutData.mTextLayoutInfo.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end() - 1 ) );

    if( lineLayoutInfo.mWordGroupsLayoutInfo.empty() ) // if it's empty, it means the last character is a new line character.
    {
      relayoutData.mTextSizeForRelayoutOption.height += lineLayoutInfo.mSize.height * relayoutData.mShrinkFactor;
    }
  }
}

} // namespace

void Relayout( Actor textView,
               TextView::RelayoutOperationMask relayoutOperationMask,
               const TextView::LayoutParameters& layoutParameters,
               const TextView::VisualParameters& visualParameters,
               TextView::RelayoutData& relayoutData )
{
  if( relayoutOperationMask & TextView::RELAYOUT_SIZE_POSITION )
  {
    relayoutData.mLineJustificationInfo.clear();
    CalculateSizeAndPosition( layoutParameters,
                              relayoutData );

    TextViewRelayout::SetUnderlineInfo( relayoutData );
  }

  if( relayoutOperationMask & TextView::RELAYOUT_ALIGNMENT )
  {
    TextViewRelayout::UpdateAlignment( layoutParameters,
                                       relayoutData );
  }

  if( relayoutOperationMask & TextView::RELAYOUT_VISIBILITY )
  {
    TextViewRelayout::UpdateVisibility( layoutParameters,
                                        visualParameters,
                                        relayoutData );
  }

  if( relayoutOperationMask & TextView::RELAYOUT_INITIALIZE_TEXT_ACTORS )
  {
    TextViewProcessor::InitializeTextActorInfo( relayoutData );
  }

  if( relayoutOperationMask & TextView::RELAYOUT_TEXT_ACTOR_UPDATE )
  {
    TextViewRelayout::UpdateTextActorInfo( visualParameters,
                                           relayoutData );
  }

  if( ( relayoutOperationMask & TextView::RELAYOUT_INSERT_TO_TEXT_VIEW ) ||
      ( relayoutOperationMask & TextView::RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST ) )
  {
    TextViewRelayout::InsertToTextView( relayoutOperationMask,
                                        textView,
                                        relayoutData );
  }
}

} // namespace SplitByWord

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
