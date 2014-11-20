/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/license/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// FILE HEADER
#include <dali-toolkit/internal/controls/text-view/relayout-utilities.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <dali/public-api/text/text-actor-parameters.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-processor-bidirectional-info.h>
#include <dali-toolkit/internal/controls/text-view/text-view-word-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-helper-functions.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-dbg.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewRelayout
{

const float MINIMUM_FADE_BOUNDARY = 0.05f; // When the fade boundary is the same as the text-view boundary, this constant reduces it in order to avoid a zero division.

RelayoutParameters::RelayoutParameters()
: mPositionOffset(),
  mParagraphSize(),
  mWordSize(),
  mCharacterSize(),
  mIndices(),
  mCharacterGlobalIndex( 0u ),
  mIsFirstCharacter( false ),
  mIsFirstCharacterOfWord( false ),
  mIsNewLine( false ),
  mIsNewParagraphCharacter( false ),
  mIsWhiteSpace( false ),
  mIsVisible( false )
{
}

RelayoutParameters::~RelayoutParameters()
{
}

FadeParameters::FadeParameters()
: mRightFadeBoundary( 0.f ),
  mRightFadeThreshold( 0.f ),
  mRightFadeBoundaryOffset( 0.f ),
  mRightFadeThresholdOffset( 0.f ),
  mRightAlphaCoeficients(),
  mLeftFadeBoundary( 0.f ),
  mLeftFadeThreshold( 0.f ),
  mLeftFadeBoundaryOffset( 0.f ),
  mLeftFadeThresholdOffset( 0.f ),
  mLeftAlphaCoeficients(),
  mTopFadeBoundary( 0.f ),
  mTopFadeThreshold( 0.f ),
  mTopFadeBoundaryOffset( 0.f ),
  mTopFadeThresholdOffset( 0.f ),
  mTopAlphaCoeficients(),
  mBottomFadeBoundary( 0.f ),
  mBottomFadeThreshold( 0.f ),
  mBottomFadeBoundaryOffset( 0.f ),
  mBottomFadeThresholdOffset( 0.f ),
  mBottomAlphaCoeficients(),
  mIsPartiallyVisible( false )
{
}

FadeParameters::~FadeParameters()
{
}

EllipsizeParameters::EllipsizeParameters()
: mPosition(),
  mLineDescender( 0.f ),
  mLineWidth( 0.f ),
  mEllipsizeBoundary(),
  mFirstIndex( 0u ),
  mLastIndex( 0u ),
  mEllipsizeLine( false ),
  mIsLineWidthFullyVisible( false ),
  mIsLineHeightFullyVisible( false ),
  mIsNextLineFullyVisibleHeight( false ),
  mCreateEllipsizedTextActors( false ),
  mLineFits( false ),
  mWordFits( false )
{
}

EllipsizeParameters::~EllipsizeParameters()
{
}

UnderlineInfo::UnderlineInfo()
: mMaxHeight( 0.f ),
  mMaxThickness( 0.f ),
  mPosition( 0.f )
{
}

UnderlineInfo::~UnderlineInfo()
{
}

TextUnderlineStatus::TextUnderlineStatus()
: mUnderlineInfo(),
  mCharacterGlobalIndex( 0u ),
  mLineGlobalIndex( 0u ),
  mCurrentUnderlineStatus( false )
{
}

TextUnderlineStatus::~TextUnderlineStatus()
{
}

LineLayoutInfo::LineLayoutInfo()
: mLineLength( 0.f ),
  mMaxCharHeight( 0.f ),
  mMaxAscender( 0.f )
{
}

LineLayoutInfo::~LineLayoutInfo()
{
}

/**
 * Whether the given text-actor exceeds the left or the right boundary of the text-view.
 *
 * @param[in] position The position of the text-actor.
 * @param[in] size The size of the text-actor.
 * @param[in] parantSize The size of the text-view.
 *
 * @return \e true if the text-actor exceeds the left or the right boundary of the text-view.
 */
bool IsExceedingWidth( const Vector3& position, const Size& size, const Size& parentSize )
{
  return ( ( position.x < 0.f ) ||
           ( position.x + size.width > parentSize.width ) );
}

/**
 * Whether the given text-actor exceeds the top or the bottom boundary of the text-view.
 *
 * @param[in] position The position of the text-actor.
 * @param[in] size The size of the text-actor.
 * @param[in] parantSize The size of the text-view.
 *
 * @return \e true if the text-actor exceeds the top or the bottom boundary of the text-view.
 */
bool IsExceedingHeight( const Vector3& position, const Size& size, const Size& parentSize )
{
  return ( ( position.y > parentSize.height ) ||
           ( position.y < size.height ) );
}

/**
 * Calculates the line length adding the new word or character width.
 *
 * It also returns the length of white spaces if they are at the end of the line.
 *
 * @param[in] isWhiteSpace Whether the word is a white space.
 * @param[in] width The width of the character or word.
 * @param[in] parentWidth The parent width.
 * @param[out] found Whether the sum of the new character or word is exceding the parent's width.
 * @param[out] lineLength The length of the portion of line which doesn't exceed the parant's width
 * @param[out] endWhiteSpaceLength The length of white spaces which are at the end of the line.
 */
void CalculateLineLength( bool isWhiteSpace, float width, float parentWidth, bool& found, float& lineLength, float& endWhiteSpaceLength )
{
  if( lineLength + width > parentWidth )
  {
    found = true;
    lineLength -= endWhiteSpaceLength;
  }
  else
  {
    lineLength += width;

    if( isWhiteSpace )
    {
      endWhiteSpaceLength += width;
    }
    else
    {
      endWhiteSpaceLength = 0.f;
    }
  }
}

struct CurrentTextActorInfo
{
  TextActor textActor;
  Text text;
  Vector3 position;
  Size size;
  Vector4 color;
  TextViewProcessor::CharacterLayoutInfo* characterLayout;
};

void SetVisualParameters( CurrentTextActorInfo& currentTextActorInfo,
                          const TextView::VisualParameters& visualParameters,
                          TextView::RelayoutData& relayoutData,
                          const float lineHeight )
{
  currentTextActorInfo.textActor.SetTextColor( currentTextActorInfo.color );
  if( ( NULL != currentTextActorInfo.characterLayout ) &&
      ( NULL != currentTextActorInfo.characterLayout->mGradientInfo ) )
  {
    currentTextActorInfo.textActor.SetGradientColor( currentTextActorInfo.characterLayout->mGradientInfo->mGradientColor );
    currentTextActorInfo.textActor.SetGradientStartPoint( currentTextActorInfo.characterLayout->mGradientInfo->mStartPoint );
    currentTextActorInfo.textActor.SetGradientEndPoint( currentTextActorInfo.characterLayout->mGradientInfo->mEndPoint );
  }

  // The italics offset is used in the offscreen rendering. When text is in italics, it may exceed the text-view's boundary
  // due to the trick used to implement it.
  const Radian& italicsAngle = currentTextActorInfo.textActor.GetItalicsAngle();
  const float italicsOffset = lineHeight * std::tan( italicsAngle );
  relayoutData.mTextLayoutInfo.mMaxItalicsOffset = std::max( relayoutData.mTextLayoutInfo.mMaxItalicsOffset, italicsOffset );

  // Sets the sort modifier value.
  currentTextActorInfo.textActor.SetSortModifier( visualParameters.mSortModifier );

  // Enables or disables the blending.
  currentTextActorInfo.textActor.SetBlendMode( !visualParameters.mSnapshotModeEnabled ? BlendingMode::ON : BlendingMode::OFF );
}

void CalculateLineLayout( float parentWidth,
                          const TextViewProcessor::TextInfoIndices& indices,
                          const TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo,
                          HorizontalWrapType splitPolicy,
                          float shrinkFactor,
                          LineLayoutInfo& subLineInfo )
{
  subLineInfo.mLineLength = 0.f;
  subLineInfo.mMaxCharHeight = 0.f;
  subLineInfo.mMaxAscender = 0.f;

  float endWhiteSpaceLength = 0.f;

  std::size_t characterIndex = indices.mCharacterIndex;
  float lineOffset = 0.f;
  bool found = false;
  bool isFirstCharacter = true;
  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator wordIt = paragraphLayoutInfo.mWordsLayoutInfo.begin() + indices.mWordIndex,
         wordEndIt = paragraphLayoutInfo.mWordsLayoutInfo.end();
       ( wordIt != wordEndIt ) && !found;
       ++wordIt )
  {
    const TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordIt );

    const float shrunkWordWidth = wordLayoutInfo.mSize.width * shrinkFactor;
    const bool isWhiteSpace = TextViewProcessor::WordSeparator == wordLayoutInfo.mType;

    bool splitByCharacter = false;

    switch( splitPolicy )
    {
      case WrapByCharacter:
      {
        splitByCharacter = true;
        break;
      }
      case WrapByWord:
      case WrapByParagraphCharacter: // Fall through
      {
        splitByCharacter = false;
        break;
      }
      case WrapByWordAndSplit:
      {
        splitByCharacter = ( shrunkWordWidth > parentWidth );
        break;
      }
      case WrapByParagraphCharacterAndSplit:
      {
        if( ( 0u != characterIndex ) ||
            ( ( 0u == characterIndex ) && ( lineOffset + shrunkWordWidth > parentWidth ) ) )
        {
          splitByCharacter = true;
        }
        else
        {
          lineOffset += shrunkWordWidth;
          splitByCharacter = false;
        }
      }
    }

    if( splitByCharacter )
    {
      for( TextViewProcessor::CharacterLayoutInfoContainer::const_iterator charIt = wordLayoutInfo.mCharactersLayoutInfo.begin() + characterIndex,
             charEndIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           ( charIt != charEndIt ) && !found;
           ++charIt )
      {
        const TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *charIt );
        CalculateLineLength( isWhiteSpace, characterLayoutInfo.mSize.width * shrinkFactor, parentWidth, found, subLineInfo.mLineLength, endWhiteSpaceLength );
        if( !found || isFirstCharacter )
        {
          subLineInfo.mMaxCharHeight = std::max( subLineInfo.mMaxCharHeight, characterLayoutInfo.mSize.height );
          subLineInfo.mMaxAscender = std::max( subLineInfo.mMaxAscender, characterLayoutInfo.mAscender );
        }

        // All characters for word 'wordIndex' have been processed.
        // Next word need to process all characters, so the characterIndex is reset to 0.
        characterIndex = 0u;
        isFirstCharacter = false;
      }

      lineOffset += subLineInfo.mLineLength;
    }
    else
    {
      CalculateLineLength( isWhiteSpace, shrunkWordWidth, parentWidth, found, subLineInfo.mLineLength, endWhiteSpaceLength );
      if( !found || isFirstCharacter )
      {
        subLineInfo.mMaxCharHeight = std::max( subLineInfo.mMaxCharHeight, wordLayoutInfo.mSize.height );
        subLineInfo.mMaxAscender = std::max( subLineInfo.mMaxAscender, wordLayoutInfo.mAscender );
      }
      isFirstCharacter = false;
    }
  }

  subLineInfo.mMaxCharHeight *= shrinkFactor;
  subLineInfo.mMaxAscender *= shrinkFactor;
}


/**
 * Sets a character of a line of a bidirectional paragraph in the new position.
 *
 * @param[in] wordsLayoutInfo Layout info of all the words of the paragraph.
 * @param[in] index Index within the paragraph to the character to be set in the new position.
 * @param[in,out] character Reference to the character in the new position.
 */
void SetCharacter( const TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo,
                   std::size_t index,
                   TextViewProcessor::CharacterLayoutInfo& character )
{
  // Traverse all the characters of the paragraph till the one pointed by index is found.
  std::size_t traversedCharacters = 0u;
  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator wordIt = wordsLayoutInfo.begin(),
         wordEndIt = wordsLayoutInfo.end();
       wordIt != wordEndIt;
       ++wordIt )
  {
    const TextViewProcessor::WordLayoutInfo& word( *wordIt );

    const std::size_t numberOfCharacters = word.mCharactersLayoutInfo.size();
    if( index < traversedCharacters + numberOfCharacters  )
    {
      character = *( word.mCharactersLayoutInfo.begin() + ( index - traversedCharacters ) );
      return;
    }
    traversedCharacters += numberOfCharacters;
  }
}

/**
 * Reorders the layout info of each line of the paragraph.
 *
 * Uses the visual to logical conversion table to order the text, styles and character's layout (metrics).
 *
 * @param[in] characterGlobalIndex Index within the whole text of the first character of the paragraph.
 * @param[in,out] rtlParagraph Layout info for the paragraph with rtl text.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void ReorderLayout( std::size_t characterGlobalIndex,
                    TextViewProcessor::ParagraphLayoutInfo& paragraph,
                    TextView::RelayoutData& relayoutData )
{
  // Clear any previous right to left layout.
  if( NULL != paragraph.mRightToLeftLayout )
  {
    paragraph.mRightToLeftLayout->Clear();
    paragraph.mRightToLeftLayout->mPreviousLayoutCleared = true;
  }
  else
  {
    // Create a new right to left layout if there isn't any.
    paragraph.mRightToLeftLayout = new TextViewProcessor::RightToLeftParagraphLayout();
  }

  // Reorder Text and Styles.

  // Reserve space for the styles.
  paragraph.mRightToLeftLayout->mTextStyles.Reserve( paragraph.mTextStyles.Count() );

  // Traverses all the bidirectional info per line.
  for( Vector<TextProcessor::BidirectionalLineInfo*>::ConstIterator it = paragraph.mBidirectionalLinesInfo.Begin(), endIt = paragraph.mBidirectionalLinesInfo.End(); it != endIt; ++it )
  {
    TextProcessor::BidirectionalLineInfo* info( *it );

    const std::size_t characterParagraphIndex = info->mCharacterParagraphIndex;
    const Vector<int>& visualToLogicalMap = info->mVisualToLogicalMap;

    // The text can be appended as it's already reordered.
    paragraph.mRightToLeftLayout->mText.Append( info->mText );

    // The visual to logical map needs to be used to reorder the styles.
    for( std::size_t index = 0u, size = visualToLogicalMap.Count(); index < size; ++index )
    {
      paragraph.mRightToLeftLayout->mTextStyles.PushBack( *( paragraph.mTextStyles.Begin() + ( characterParagraphIndex + *( visualToLogicalMap.Begin() + index ) ) ) );
    }
  }

  // Reorder Layout Info.

  // Reserve space for the new word layout.
  paragraph.mRightToLeftLayout->mWordsLayoutInfo.reserve( paragraph.mWordsLayoutInfo.size() );

  // Traverses all the bidirectional info per line.
  for( Vector<TextProcessor::BidirectionalLineInfo*>::ConstIterator it = paragraph.mBidirectionalLinesInfo.Begin(), endIt = paragraph.mBidirectionalLinesInfo.End(); it != endIt; ++it )
  {
    TextProcessor::BidirectionalLineInfo* info( *it );

    // Reserve space for all characters.
    TextViewProcessor::CharacterLayoutInfoContainer characters;
    characters.resize( info->mNumberOfCharacters );

    // Uses the visual to logical map to set every character in its new position.
    for( std::size_t index = 0u; index < info->mNumberOfCharacters; ++index )
    {
      SetCharacter( paragraph.mWordsLayoutInfo,
                    info->mCharacterParagraphIndex + info->mVisualToLogicalMap[index],
                    *( characters.begin() + index ) );
    }

    // Sets the new 'x' position for each character.
    // Updates the text-view's layout info table with the new position of the character.
    float xPosition = 0.f;
    std::size_t index = 0u;
    for( TextViewProcessor::CharacterLayoutInfoContainer::iterator it = characters.begin(), endIt = characters.end(); it != endIt; ++it, ++index )
    {
      TextViewProcessor::CharacterLayoutInfo& character( *it );

      // Set the 'x' position.
      character.mPosition.x = xPosition;

      // Update layout info table.
      relayoutData.mCharacterLayoutInfoTable[characterGlobalIndex + info->mVisualToLogicalMap[index]].mPosition = character.mPosition;

      // Update the position for the next character.
      xPosition += character.mSize.width;
    }

    // Split the reordered text in words.
    std::size_t previousPosition = 0u;
    Vector<std::size_t> positions;
    TextProcessor::SplitInWords( info->mText, positions );

    // Whether last character is a word or a paragraph separator.
    const std::size_t lastCharacterIndex = info->mText.GetLength() - 1u;
    const bool isLastCharacterParagraphSeparator = info->mText.IsNewLine( lastCharacterIndex );
    const bool isLastCharacterWordSeparator = info->mText.IsWhiteSpace( lastCharacterIndex );

    // Sets the characters into the words they belong to.
    for( Vector<std::size_t>::ConstIterator it = positions.Begin(), endIt = positions.End(); it != endIt; ++it )
    {
      const std::size_t position = *it;

      TextViewProcessor::WordLayoutInfo word;
      word.mCharactersLayoutInfo.insert( word.mCharactersLayoutInfo.end(),
                                         characters.begin() + previousPosition,
                                         characters.begin() + position );

      if( !word.mCharactersLayoutInfo.empty() )
      {
        // Updates the layout of the word.
        TextViewProcessor::UpdateLayoutInfo( word );

        paragraph.mRightToLeftLayout->mWordsLayoutInfo.push_back( word );
      }

      // white space or new paragraph.
      TextViewProcessor::WordLayoutInfo space;

      space.mCharactersLayoutInfo.insert( space.mCharactersLayoutInfo.end(),
                                          characters.begin() + position,
                                          characters.begin() + position + 1u );

      space.mType = TextViewProcessor::WordSeparator;

      TextViewProcessor::UpdateLayoutInfo( space );

      paragraph.mRightToLeftLayout->mWordsLayoutInfo.push_back( space );

      previousPosition = position + 1u;
    }

    // The last word.
    if( previousPosition < paragraph.mRightToLeftLayout->mText.GetLength() )
    {
      TextViewProcessor::WordLayoutInfo word;
      word.mCharactersLayoutInfo.insert( word.mCharactersLayoutInfo.end(),
                                         characters.begin() + previousPosition,
                                         characters.end() );

      if( isLastCharacterParagraphSeparator )
      {
        word.mType = TextViewProcessor::ParagraphSeparator;
      }
      else if( isLastCharacterWordSeparator )
      {
        word.mType = TextViewProcessor::WordSeparator;
      }
      TextViewProcessor::UpdateLayoutInfo( word );

      paragraph.mRightToLeftLayout->mWordsLayoutInfo.push_back( word );
    }
  }
}

/**
 * Creates the bidirectional info needed to reorder each line of the paragraph.
 *
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in,out] paragraph Layout info for the paragraph.
 * @param[in] characterGlobalIndex Index to the character within the whole text.
 * @param[in] lineLayoutInfoIndex Index to the table of lines.
 */
void CreateBidirectionalInfoForLines( TextView::RelayoutData& relayoutData,
                                      TextViewProcessor::ParagraphLayoutInfo& paragraph,
                                      std::size_t& characterGlobalIndex,
                                      std::size_t& lineLayoutInfoIndex )
{
  const std::size_t lineLayoutInfoSize = relayoutData.mLines.size(); // Number or laid out lines.
  bool lineLayoutEnd = false;            // Whether lineLayoutInfoIndex points at the last laid out line.

  // Clear previously created bidirectional info.
  paragraph.ClearBidirectionalInfo();

  // For each character, it sets the character's direction.

  // Initialize the paragraph direction. Used to set the direction of weak characters.
  const bool isParagraphRightToLeft = paragraph.mBidirectionalParagraphInfo->IsRightToLeftParagraph();
  bool isPreviousRightToLeft = isParagraphRightToLeft;

  for( std::size_t index = 0u; index < paragraph.mNumberOfCharacters; ++index )
  {
    // Get the character's layout information (the one is shared with text-input)
    Toolkit::TextView::CharacterLayoutInfo& info = *( relayoutData.mCharacterLayoutInfoTable.begin() + ( characterGlobalIndex + index ) );

    // Gets the character's direction.
    const Character::CharacterDirection direction = paragraph.mText[index].GetCharacterDirection();
    if( Character::RightToLeft == direction )
    {
      info.mIsRightToLeftCharacter = true;
    }
    else if( Character::Neutral == direction )
    {
      // For neutral characters it check's the next and previous directions.
      // If they are equals set that direction. If they are not, sets the paragraph direction.
      // If there is no next, sets the previous direction.

      // Check next character's direction.
      bool isNextRightToLeft = isPreviousRightToLeft;
      if( index < paragraph.mNumberOfCharacters - 1u )
      {
        const Character::CharacterDirection nextDirection = paragraph.mText[index + 1u].GetCharacterDirection();
        isNextRightToLeft = Character::RightToLeft == nextDirection;
      }

      info.mIsRightToLeftCharacter = isPreviousRightToLeft == isNextRightToLeft ? isPreviousRightToLeft : isParagraphRightToLeft;
    }
    else
    {
      info.mIsRightToLeftCharacter = false;
    }

    isPreviousRightToLeft = info.mIsRightToLeftCharacter;
  }

  std::size_t characterParagraphIndex = 0u;   // Index to the character (within the paragraph).
  for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = paragraph.mWordsLayoutInfo.begin(), wordEndIt = paragraph.mWordsLayoutInfo.end();
       wordIt != wordEndIt;
       ++wordIt )
  {
    TextViewProcessor::WordLayoutInfo& word( *wordIt );

    for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
         characterIt != characterEndIt;
         ++characterIt )
    {
      TextProcessor::BidirectionalLineInfo* bidirectionalLineInfo = NULL;

      // Check if there is a new line.
      const bool newLine = !lineLayoutEnd && ( characterGlobalIndex == relayoutData.mLines[lineLayoutInfoIndex].mCharacterGlobalIndex );

      if( newLine )
      {
        // Point to the next line.
        ++lineLayoutInfoIndex;
        if( lineLayoutInfoIndex >= lineLayoutInfoSize )
        {
          // Arrived at last line.
          lineLayoutEnd = true; // Avoids access out of bounds in the relayoutData.mLines vector.
        }

        // Number of characters of the line.
        const size_t numberOfCharacters = ( lineLayoutEnd ? relayoutData.mTextLayoutInfo.mNumberOfCharacters : relayoutData.mLines[lineLayoutInfoIndex].mCharacterGlobalIndex ) - characterGlobalIndex;

        // There is right to left characters in this line. It needs to be reordered.
        bidirectionalLineInfo = new TextProcessor::BidirectionalLineInfo();
        bidirectionalLineInfo->mCharacterParagraphIndex = characterParagraphIndex;
        bidirectionalLineInfo->mNumberOfCharacters = numberOfCharacters;

        // Set all the Text's characters in the visual order and creates the mapping tables.
        TextProcessor::ReorderLine( paragraph.mBidirectionalParagraphInfo,
                                    bidirectionalLineInfo );

        paragraph.mBidirectionalLinesInfo.PushBack( bidirectionalLineInfo );

        for( std::size_t index = 0u; index < numberOfCharacters; ++index )
        {
          relayoutData.mCharacterLogicalToVisualMap.push_back( characterGlobalIndex + bidirectionalLineInfo->mLogicalToVisualMap[index] );
          relayoutData.mCharacterVisualToLogicalMap.push_back( characterGlobalIndex + bidirectionalLineInfo->mVisualToLogicalMap[index] );
        }
      }

      ++characterGlobalIndex;
      ++characterParagraphIndex;
    } // characters
  } // words
}

void ReorderRightToLeftLayout( TextView::RelayoutData& relayoutData )
{
  // Reset conversion tables shared through public-api
  relayoutData.mCharacterLogicalToVisualMap.clear();
  relayoutData.mCharacterVisualToLogicalMap.clear();

  std::size_t characterGlobalIndex = 0u; // Index to the global character (within the whole text).
  std::size_t lineLayoutInfoIndex = 0u;  // Index to the line info.

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraph( *paragraphIt );

    if( NULL != paragraph.mBidirectionalParagraphInfo )
    {
      // There is right to left text in this paragraph.

      // Stores the current global character index as is needed in both functions.
      const std::size_t currentGlobalIndex = characterGlobalIndex;

      // Creates the bidirectional info needed to reorder each line of the paragraph.
      CreateBidirectionalInfoForLines( relayoutData,
                                       paragraph,
                                       characterGlobalIndex,
                                       lineLayoutInfoIndex );

      // Reorder each line of the paragraph
      ReorderLayout( currentGlobalIndex, paragraph, relayoutData );
    }
    else
    {
      // Identity in case the paragraph has no right to left text.
      for( std::size_t index = 0u; index < paragraph.mNumberOfCharacters; ++index )
      {
        const std::size_t globalIndex = characterGlobalIndex + index;
        relayoutData.mCharacterLogicalToVisualMap.push_back( globalIndex );
        relayoutData.mCharacterVisualToLogicalMap.push_back( globalIndex );
      }
      characterGlobalIndex += paragraph.mNumberOfCharacters;
    }
  } // paragraphs
}

float CalculateXoffset( Toolkit::Alignment::Type horizontalTextAlignment, float parentWidth, float wholeTextWidth )
{
  float xOffset( 0.f );
  switch( horizontalTextAlignment )
  {
    case Toolkit::Alignment::HorizontalLeft:
    {
      // nothing to do.
      break;
    }
    case Toolkit::Alignment::HorizontalCenter:
    {
      xOffset = 0.5f * ( parentWidth - wholeTextWidth );
      break;
    }
    case Toolkit::Alignment::HorizontalRight:
    {
      xOffset = parentWidth - wholeTextWidth;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextViewRelayout::CalculateXoffset: Wrong horizontal text alignment. Did you set a vertical one?" );
    }
  }

  return xOffset;
}

float CalculateYoffset( Toolkit::Alignment::Type verticalTextAlignment, float parentHeight, float wholeTextHeight )
{
  float yOffset( 0.f );
  switch( verticalTextAlignment )
  {
    case Toolkit::Alignment::VerticalTop:
    {
      // nothing to do.
      break;
    }
    case Toolkit::Alignment::VerticalCenter:
    {
      yOffset = 0.5f * ( parentHeight - wholeTextHeight );
      break;
    }
    case Toolkit::Alignment::VerticalBottom:
    {
      yOffset = parentHeight - wholeTextHeight;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextViewRelayout::CalculateXoffset: Wrong vertical text alignment. Did you set an horizontal one?" );
    }
  }

  return yOffset;
}

float CalculateJustificationOffset( Toolkit::TextView::LineJustification justification, float wholeTextWidth, float lineLength )
{
  float offset = 0.f;
  switch( justification )
  {
    case Toolkit::TextView::Left:
    {
      offset = 0.f;
      break;
    }
    case Toolkit::TextView::Center:
    {
      offset = 0.5f * ( wholeTextWidth - lineLength );
      break;
    }
    case Toolkit::TextView::Right:
    {
      offset = wholeTextWidth - lineLength;
      break;
    }
    case Toolkit::TextView::Justified:
    {
      offset = 0.f;
      break;
    }
  }

  return offset;
}

bool IsVisible( const Vector3& position, const Size& size, const Size& parentSize, VisibilityTestType type )
{
  bool visible = false;

  switch( type )
  {
    case FULLY_VISIBLE:
    {
      // Whether the text-actor is fully inside the boundaries of the text-view.
      visible = ( ( position.x >= 0.f ) && ( position.x + size.width <= parentSize.width ) &&
                  ( position.y >= size.height ) && ( position.y <= parentSize.height ) );
      break;
    }
    case FULLY_VISIBLE_WIDTH:
    {
      // Whether the text-actor is between the left and right boundaries of the text-view.
      visible = ( ( position.x >= 0.f ) && ( position.x + size.width <= parentSize.width ) );
      break;
    }
    case FULLY_VISIBLE_HEIGHT:
    {
      // Whether the text-actor is between the top and bottom boundaries of the text-view.
      visible = ( ( position.y >= size.height ) && ( position.y <= parentSize.height ) );
      break;
    }
    case PARTIALLY_VISIBLE:
    {
      // Whether the text-actor is partially inside the boundaries of the text-view.
      visible = ( ( position.x < parentSize.width ) &&
                  ( position.x + size.width > 0.f ) &&
                  ( position.y > 0.f ) &&
                  ( position.y - size.height < parentSize.height ) );
      break;
    }
    case PARTIALLY_VISIBLE_WIDTH:
    {
      // Whether the text-actor is partially inside the area defined by the left and the right boundaries of the text-view.
      // It may not be partially inside the text-view.
      visible = ( ( position.x < parentSize.width ) &&
                  ( position.x + size.width > 0.f ) );
      break;
    }
    case PARTIALLY_VISIBLE_HEIGHT:
    {
      // Whether the text-actor is partially inside the area defined by the top and the bottom boundaries of the text-view.
      // It may not be partially inside the text-view.
      visible = ( ( position.y > 0.f ) &&
                  ( position.y - size.height < parentSize.height ) );
      break;
    }
  }

  return visible;
}

Vector2 CalculateRectParameters( const Vector2& p0, const Vector2& p1 )
{
  const float gradient = ( p1.y - p0.y ) / ( p1.x - p0.x );

  return Vector2( gradient, p0.y - gradient * p0.x );
}

void UpdateAlignment( const TextView::LayoutParameters& layoutParameters,
                      TextView::RelayoutData& relayoutData )
{
  // Calculates an offset to align the whole text within the text-view's boundary accordingly with the set alignment and justification options.
  // The offset could be negative if the whole text is bigger than the boundary of the text-view.

  // If the exceed policy is ellipsize at the end, negative offsets are not wanted.
  // In that case, it will align the line to the left and/or top, and ellipsize the end.
  const bool ellipsizeAlignToLeft = ( layoutParameters.mExceedPolicy == TextView::EllipsizeEndOriginal ) ||
                                    ( layoutParameters.mExceedPolicy == TextView::EllipsizeEnd ) ||
                                    ( layoutParameters.mExceedPolicy == TextView::SplitEllipsizeEnd );
  const bool ellipsizeAlignToTop = ( layoutParameters.mExceedPolicy == TextView::EllipsizeEnd ) ||
                                   ( layoutParameters.mExceedPolicy == TextView::SplitEllipsizeEnd );

  RelayoutParameters relayoutParameters;

  // Calculates the vertical and horizontal offsets.
  const float textHorizontalOffset = CalculateXoffset( layoutParameters.mHorizontalAlignment, relayoutData.mTextViewSize.width, relayoutData.mTextSizeForRelayoutOption.width );
  const float textVerticalOffset = CalculateYoffset( layoutParameters.mVerticalAlignment, relayoutData.mTextViewSize.height, relayoutData.mTextSizeForRelayoutOption.height );

  // Index to the global character (within the whole text).
  std::size_t characterGlobalIndex = 0u;

  // Index to the line info.
  std::size_t lineLayoutInfoIndex = 0u;

  relayoutParameters.mIndices.mParagraphIndex = 0u;

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(),
         endParagraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphLayoutIt != endParagraphLayoutIt;
       ++paragraphLayoutIt, ++relayoutParameters.mIndices.mParagraphIndex )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphLayoutIt );

    float justificationOffset = 0.f;

    const std::size_t lineLayoutInfoSize = relayoutData.mLines.size(); // Number of lines.
    bool lineLayoutEnd = false;            // Whether lineLayoutInfoIndex points at the last line.

    relayoutParameters.mIndices.mWordIndex = 0u;

    const bool isRightToLeftLayout = NULL != paragraphLayoutInfo.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraphLayoutInfo.mRightToLeftLayout->mWordsLayoutInfo : paragraphLayoutInfo.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordsLayoutInfo.begin(),
           endWordLayoutIt = wordsLayoutInfo.end();
         wordLayoutIt != endWordLayoutIt;
         ++wordLayoutIt, ++relayoutParameters.mIndices.mWordIndex )
    {
      TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );

      relayoutParameters.mIndices.mCharacterIndex = 0u;

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           characterLayoutIt != endCharacterLayoutIt;
           ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex, ++characterGlobalIndex )
      {
        TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

        // Check if there is a new line.
        const bool newLine = !lineLayoutEnd && ( characterGlobalIndex == relayoutData.mLines[lineLayoutInfoIndex].mCharacterGlobalIndex );

        if( newLine )
        {
          // Calculate line justification offset.
          justificationOffset = CalculateJustificationOffset( layoutParameters.mLineJustification, relayoutData.mTextSizeForRelayoutOption.width, relayoutData.mLines[lineLayoutInfoIndex].mSize.width );

          // Point to the next line.
          ++lineLayoutInfoIndex;
          if( lineLayoutInfoIndex >= lineLayoutInfoSize )
          {
            // Arrived at last line.
            lineLayoutEnd = true; // Avoids access out of bounds in the relayoutData.mLines vector.
          }
        }

        // Deletes the offsets if the exceed policies are EllipsizeEnd.
        const float horizontalOffset = textHorizontalOffset + justificationOffset;
        characterLayoutInfo.mOffset.x = ( ellipsizeAlignToLeft && ( horizontalOffset < 0.f ) ) ? 0.f : horizontalOffset;
        characterLayoutInfo.mOffset.y = ( ellipsizeAlignToTop && ( textVerticalOffset < 0.f ) ) ? 0.f : textVerticalOffset;

        // Updates the size and position table for text-input with the alignment offset.
        Vector3 positionOffset( characterLayoutInfo.mPosition );

        // Update layout info table.
        std::vector<Toolkit::TextView::CharacterLayoutInfo>::iterator infoTableIt = relayoutData.mCharacterLayoutInfoTable.begin() + relayoutData.mCharacterVisualToLogicalMap[characterGlobalIndex];
        Toolkit::TextView::CharacterLayoutInfo& characterTableInfo( *infoTableIt );

        characterTableInfo.mPosition.x = positionOffset.x + characterLayoutInfo.mOffset.x;
        characterTableInfo.mPosition.y = positionOffset.y + characterLayoutInfo.mOffset.y;

        positionOffset.x += characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor;
      } // end characters
    } // end words
  } // end paragraphs
}

void CalculateBearing( TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                       TextView::RelayoutData& relayoutData )
{
  // No bearing used.
  //
  //            gggggggggg
  //            gggggggggg
  //          gggg      gggg
  //          gggg      gggg
  //          gggg      gggg
  //          gggg      gggg
  //          gggg      gggg
  //          gggg      gggg
  //  ggggg     gggggggggg        bb         ggggg
  // gg   gg    gggggggggg        bb        gg   gg
  // gg   gg            gggg      bb        gg   gg
  // gg   gg            gggg      bb        gg   gg
  //  ggggg   gg        gggg      bbbbbbb    ggggg
  //      gg  gg        gggg      bb    bb       gg
  // g    gg    gggggggggg        bb    bb  g    gg
  //  ggggg     gggggggggg        bbbbbbb    ggggg
  //
  // Bearing used.
  //
  //            gggggggggg
  //            gggggggggg
  //          gggg      gggg      bb
  //          gggg      gggg      bb
  //          gggg      gggg      bb
  //  ggggg   gggg      gggg      bb         ggggg
  // gg   gg  gggg      gggg      bbbbbbb   gg   gg
  // gg   gg  gggg      gggg      bb    bb  gg   gg
  // gg   gg    gggggggggg        bb    bb  gg   gg
  //  ggggg     gggggggggg        bbbbbbb    ggggg
  //      gg            gggg                     gg
  // g    gg            gggg                g    gg
  //  ggggg   gg        gggg                 ggggg
  //          gg        gggg
  //            gggggggggg
  //            gggggggggg

  const Toolkit::TextView::LineLayoutInfo& lineInfo( *( relayoutData.mLines.end() - 1u ) );
  const float bearingOffset = ( lineInfo.mSize.height - lineInfo.mAscender ) - ( characterLayoutInfo.mSize.height - characterLayoutInfo.mAscender );

  characterLayoutInfo.mPosition.y -= bearingOffset * relayoutData.mShrinkFactor;
}

void UpdateLayoutInfoTable( Vector4& minMaxXY,
                            TextViewProcessor::WordLayoutInfo& wordLayoutInfo,
                            TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                            RelayoutParameters& relayoutParameters,
                            TextView::RelayoutData& relayoutData )
{
  // updates min and max position to calculate the text size for multiline policies.
  minMaxXY.x = std::min( minMaxXY.x, characterLayoutInfo.mPosition.x );
  minMaxXY.z = std::max( minMaxXY.z, characterLayoutInfo.mPosition.x + characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor );

  minMaxXY.y = std::min( minMaxXY.y, characterLayoutInfo.mPosition.y - characterLayoutInfo.mSize.height * relayoutData.mShrinkFactor );
  minMaxXY.w = std::max( minMaxXY.w, characterLayoutInfo.mPosition.y   );

  // Adds layout info to be retrieved by external controls or applications.
  Vector3 positionOffset( characterLayoutInfo.mPosition );

  const float descender = characterLayoutInfo.mSize.height - characterLayoutInfo.mAscender;

  const Toolkit::TextView::CharacterLayoutInfo characterLayoutTableInfo( Size( characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor,
                                                                               characterLayoutInfo.mSize.height * relayoutData.mShrinkFactor ),
                                                                         positionOffset,
                                                                         ( TextViewProcessor::ParagraphSeparator == wordLayoutInfo.mType ),
                                                                         false, // whether the character is right to left. The value is set in a next step in the CreateBidirectionalInfoForLines function
                                                                         true,  // whether the character is visible.
                                                                         descender );

  relayoutData.mCharacterLayoutInfoTable.push_back( characterLayoutTableInfo );

  positionOffset.x += characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor;
}

void CalculateVisibilityForFade( const Internal::TextView::LayoutParameters& layoutParameters,
                                 TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                                 const TextStyle& style,
                                 RelayoutParameters& relayoutParameters,
                                 FadeParameters& fadeParameters,
                                 TextView::RelayoutData& relayoutData )
{
  if( ( TextView::Fade != layoutParameters.mExceedPolicy ) &&
      ( TextView::SplitFade != layoutParameters.mExceedPolicy ) &&
      ( TextView::FadeOriginal != layoutParameters.mExceedPolicy ) &&
      ( TextView::OriginalFade != layoutParameters.mExceedPolicy ) )
  {
    // nothing to fade
    return;
  }

  // Calculates visibility of a text-actor according the exceed policies.

  // position + alignment offset.
  const Vector3 position( characterLayoutInfo.mPosition.x + characterLayoutInfo.mOffset.x,
                          characterLayoutInfo.mPosition.y + characterLayoutInfo.mOffset.y,
                          characterLayoutInfo.mPosition.z );

  // Whether the text actor is fully, partially or non visible (according exceed policies).
  switch( layoutParameters.mExceedPolicy )
  {
    case TextView::Fade:
    {
      // All text-actors which are not completely inside the text-view's boundaries are set as non visible.
      // All text-actors which are partially inside the text-view's boundaries are set as partially visible.
      if( !IsVisible( position,
                      characterLayoutInfo.mSize,
                      relayoutData.mTextViewSize,
                      FULLY_VISIBLE ) )
      {
        relayoutParameters.mIsVisible = false;
        if( IsVisible( position,
                       characterLayoutInfo.mSize,
                       relayoutData.mTextViewSize,
                       PARTIALLY_VISIBLE ) )
        {
          fadeParameters.mIsPartiallyVisible = true;

          // Checks if a text-actor is exceeding more than one boundary as this case is not supported.
          if( IsExceedingWidth( position,
                                characterLayoutInfo.mSize,
                                relayoutData.mTextViewSize ) &&
              IsExceedingHeight( position,
                                 characterLayoutInfo.mSize,
                                 relayoutData.mTextViewSize ) )
          {
            // Combination not fully supported by text-view.
            // Need to check if text-actor really supports this combination.
            fadeParameters.mIsPartiallyVisible = false;
          }
        }
      }
      break;
    }
    case TextView::FadeOriginal:
    {
      // All text-actors which are not completely between the left and right text-view's boundaries are set as non visible.
      // All text-actors which are partially inside the text-view's boundaries are set as partially visible.
      if( !IsVisible( position,
                      characterLayoutInfo.mSize,
                      relayoutData.mTextViewSize,
                      FULLY_VISIBLE_WIDTH ) )
      {
        relayoutParameters.mIsVisible = false;
        if( IsVisible( position,
                       characterLayoutInfo.mSize,
                       relayoutData.mTextViewSize,
                       PARTIALLY_VISIBLE_WIDTH ) )
        {
          fadeParameters.mIsPartiallyVisible = true;
        }
      }
      break;
    }
    case TextView::OriginalFade:
    case TextView::SplitFade: // Fallthrough
    {
      // All text-actors which are not completely between the top and bottom text-view's boundaries are set as non visible.
      // All text-actors which are partially inside the text-view's boundaries are set as partially visible.
      if( !IsVisible( position,
                      characterLayoutInfo.mSize,
                      relayoutData.mTextViewSize,
                      FULLY_VISIBLE_HEIGHT ) )
      {
        relayoutParameters.mIsVisible = false;
        if( IsVisible( position,
                       characterLayoutInfo.mSize,
                       relayoutData.mTextViewSize,
                       PARTIALLY_VISIBLE_HEIGHT ) )
        {
          fadeParameters.mIsPartiallyVisible = true;
        }
      }
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextViewRelayout::CalculateVisibilityForFade. Wrong exceed policies." )
      break;
    }
  }

  if( relayoutParameters.mIsVisible || fadeParameters.mIsPartiallyVisible )
  {
    characterLayoutInfo.mIsVisible = true;

    const Size size = characterLayoutInfo.mSize * relayoutData.mShrinkFactor;
    const float characterPositionPlusWidth = position.x + size.width;
    const float characterPositionMinusHeight = position.y - size.height;

    // Calculates which edges need to be faded-out.
    bool rightFadeOut = false;
    bool leftFadeOut = false;
    bool bottomFadeOut = false;
    bool topFadeOut = false;

    switch( layoutParameters.mExceedPolicy )
    {
      case TextView::Fade:
      {
        // All text-actors exceeding any of the boundaries will be faded-out.
        rightFadeOut = ( characterPositionPlusWidth > fadeParameters.mRightFadeThreshold );
        leftFadeOut = ( position.x < fadeParameters.mLeftFadeThreshold );
        bottomFadeOut = ( position.y > fadeParameters.mBottomFadeThreshold );
        topFadeOut = ( characterPositionMinusHeight < fadeParameters.mTopFadeThreshold );
        break;
      }
      case TextView::FadeOriginal:
      {
        // Only text-actors exceeding the left or the right boundaries will be faded-out.
        rightFadeOut = ( characterPositionPlusWidth > fadeParameters.mRightFadeThreshold );
        leftFadeOut = ( position.x < fadeParameters.mLeftFadeThreshold );
        break;
      }
      case TextView::SplitFade:
      case TextView::OriginalFade: //Fallthrough
      {
        // Only text-actors exceeding the top or the bottom boundaries will be faded-out.
        bottomFadeOut = ( position.y > fadeParameters.mBottomFadeThreshold );
        topFadeOut = ( characterPositionMinusHeight < fadeParameters.mTopFadeThreshold );
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS( !"TextViewRelayout::CalculateVisibilityForFade. Wrong exceed policies." );
        break;
      }
    }

    // Calculates gradient parameters for a text-actor.
    Vector4 gradientColor = Vector4::ZERO;
    Vector2 startPoint = Vector2::ZERO;
    Vector2 endPoint = Vector2::ZERO;

    if( !( rightFadeOut && leftFadeOut ) )
    {
      // Current implementation can't set gradient parameters for a text-actor exceeding at the same time the left and the right boundaries.
      if( rightFadeOut )
      {
        gradientColor = style.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha = gradientColor.a * std::min( 1.f, fadeParameters.mRightAlphaCoeficients.x * position.x + fadeParameters.mRightAlphaCoeficients.y );
        gradientColor.a *= std::max( 0.f, fadeParameters.mRightAlphaCoeficients.x * characterPositionPlusWidth + fadeParameters.mRightAlphaCoeficients.y );

        startPoint = Vector2( std::max( 0.f, std::min( 1.f, ( fadeParameters.mRightFadeThresholdOffset - position.x ) / size.width ) ), 0.5f );
        endPoint = Vector2( std::min( 1.f, std::max( 0.f, ( relayoutData.mTextViewSize.width - position.x ) / size.width ) ), 0.5f );

        if( NULL == characterLayoutInfo.mGradientInfo )
        {
          characterLayoutInfo.mGradientInfo = new TextViewProcessor::GradientInfo();
        }
      }
      else if( leftFadeOut )
      {
        gradientColor = style.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha = std::min( 1.f, fadeParameters.mLeftAlphaCoeficients.x * characterPositionPlusWidth + fadeParameters.mLeftAlphaCoeficients.y );
        gradientColor.a *= gradientColor.a * std::max( 0.f, fadeParameters.mLeftAlphaCoeficients.x * position.x + fadeParameters.mLeftAlphaCoeficients.y );

        startPoint = Vector2( std::max( 0.f, std::min( 1.f, ( fadeParameters.mLeftFadeThresholdOffset - position.x ) / size.width ) ), 0.5f );
        endPoint = Vector2( std::min( 1.f, std::max( 0.f, -position.x / size.width ) ), 0.5f );

        if( NULL == characterLayoutInfo.mGradientInfo )
        {
          characterLayoutInfo.mGradientInfo = new TextViewProcessor::GradientInfo();
        }
      }
    }

    if( !( bottomFadeOut && topFadeOut ) )
    {
      // Current implementation can't set gradient parameters for a text-actor exceeding at the same time the top and the bottom boundaries.
      if( bottomFadeOut )
      {
        gradientColor = style.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha = gradientColor.a * std::min( 1.f, fadeParameters.mBottomAlphaCoeficients.x * characterPositionMinusHeight + fadeParameters.mBottomAlphaCoeficients.y );
        gradientColor.a *= std::max( 0.f, fadeParameters.mBottomAlphaCoeficients.x * position.y + fadeParameters.mBottomAlphaCoeficients.y );

        startPoint = Vector2( 0.5f, std::max( 0.f, std::min( 1.f, ( fadeParameters.mBottomFadeThresholdOffset - characterPositionMinusHeight ) / size.height ) ) );
        endPoint = Vector2( 0.5f, std::min( 1.f, std::max( 0.f, ( relayoutData.mTextViewSize.height - characterPositionMinusHeight ) / size.height ) ) );

        if( NULL == characterLayoutInfo.mGradientInfo )
        {
          characterLayoutInfo.mGradientInfo = new TextViewProcessor::GradientInfo();
        }
      }
      else if( topFadeOut )
      {
        gradientColor = style.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha *= gradientColor.a * std::min( 1.f, fadeParameters.mTopAlphaCoeficients.x * position.y + fadeParameters.mTopAlphaCoeficients.y );
        gradientColor.a *= std::max( 0.f, fadeParameters.mTopAlphaCoeficients.x * characterPositionMinusHeight + fadeParameters.mTopAlphaCoeficients.y );

        startPoint = Vector2( 0.5f, std::max( 0.f, std::min( 1.f, ( fadeParameters.mTopFadeThresholdOffset - characterPositionMinusHeight ) / size.height ) ) );
        endPoint = Vector2( 0.5f, std::min( 1.f,  std::max( 0.f, -characterPositionMinusHeight / size.height ) ) );

        if( NULL == characterLayoutInfo.mGradientInfo )
        {
          characterLayoutInfo.mGradientInfo = new TextViewProcessor::GradientInfo();
        }
      }
    }

    if( NULL != characterLayoutInfo.mGradientInfo )
    {
      characterLayoutInfo.mGradientInfo->mGradientColor = gradientColor;
      characterLayoutInfo.mGradientInfo->mStartPoint = startPoint;
      characterLayoutInfo.mGradientInfo->mEndPoint = endPoint;
    }
  }
  else
  {
    characterLayoutInfo.mIsVisible = false;
  }
}

bool CalculateVisibilityForEllipsizeEndOriginal( TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                                                 const EllipsizeParameters& ellipsizeParameters )
{
  bool isPartiallyVisible = false;

  if( !IsVisible( ellipsizeParameters.mPosition,
                  characterLayoutInfo.mSize,
                  ellipsizeParameters.mEllipsizeBoundary,
                  FULLY_VISIBLE_WIDTH ) )
  {
    // The character doesn't fit in the text-view's width.
    characterLayoutInfo.mIsVisible = false;

    // Checks if the character is partially visible (it's cut by the boundary)
    isPartiallyVisible = IsVisible( ellipsizeParameters.mPosition,
                                    characterLayoutInfo.mSize,
                                    ellipsizeParameters.mEllipsizeBoundary,
                                    PARTIALLY_VISIBLE_WIDTH );
  }
  else
  {
    // The character fits in the text-view's width. Set it to visible.
    characterLayoutInfo.mIsVisible = true;
  }

  return isPartiallyVisible;
}

bool CalculateVisibilityForEllipsizeEnd( TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                                         const EllipsizeParameters& ellipsizeParameters )
{
  bool isPartiallyVisible = false;

  if( !IsVisible( ellipsizeParameters.mPosition,
                  characterLayoutInfo.mSize,
                  ellipsizeParameters.mEllipsizeBoundary,
                  FULLY_VISIBLE ) )
  {
    // The character is not fully visible. Needs to check if it's partially visible.
    characterLayoutInfo.mIsVisible = false;

    // Checks if the character doesn't cut the bottom edge of the text-view.
    const bool fullyVisibleHeight = IsVisible( ellipsizeParameters.mPosition,
                                               characterLayoutInfo.mSize,
                                               ellipsizeParameters.mEllipsizeBoundary,
                                               FULLY_VISIBLE_HEIGHT );

    // Checks if the character cuts the right edge of the text-view.
    const bool partiallyVisibleWidth = IsVisible( ellipsizeParameters.mPosition,
                                                  characterLayoutInfo.mSize,
                                                  ellipsizeParameters.mEllipsizeBoundary,
                                                  PARTIALLY_VISIBLE_WIDTH );

    // Character will be ellipsized if it cuts the right edge of the text-view but fits completely in the text-view's height.
    isPartiallyVisible = ( fullyVisibleHeight && partiallyVisibleWidth );
  }
  else
  {
    // The character fits in the boundary of the text-view. Set it to visible.
    characterLayoutInfo.mIsVisible = true;
  }

  return isPartiallyVisible;
}

void CalculateVisibilityForEllipsize( const Internal::TextView::LayoutParameters& layoutParameters,
                                      TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                                      EllipsizeParameters& ellipsizeParameters,
                                      TextView::RelayoutData& relayoutData )
{
  // Calculates visibility for EllipsizeEnd exceed policies.

  // It defines a boundary on the right side of the text-view by substracting the ellipsize-text's size (...) to the text-view's size.
  // If a character is cut by this boundary and the whole line (if the multi-line policy is split-by-new-line-char)
  // or the whole word (if the multi-line policy is split-by-word) doesn't fit in the text-view's width, then it's replaced by the ellipsize-text.

  // Position of the character used to do the visibility test.
  ellipsizeParameters.mPosition = Vector3( characterLayoutInfo.mPosition.x + characterLayoutInfo.mOffset.x,
                                           characterLayoutInfo.mPosition.y + characterLayoutInfo.mOffset.y,
                                           characterLayoutInfo.mPosition.z );

  // Text will be ellipsized if a character is partially visible (it's cut by the boundary defined in the right side of the text-view).
  bool isPartiallyVisible = false;

  // Checks if the whole line or the whole word fits in the text-view's width accordingly with the multiline policy.
  const bool fitsInWidth = ( Toolkit::TextView::SplitByNewLineChar == layoutParameters.mMultilinePolicy ) ? ellipsizeParameters.mLineFits: ellipsizeParameters.mWordFits;

  // Will only ellipsize the text if it cuts the right vertical edge and it doesn't fit in the text-view's width.
  if( fitsInWidth )
  {
    // The line or word fits completely inside the text-view's width. Nothing else to do.
    characterLayoutInfo.mIsVisible = true;
  }
  else
  {
    // The line or word doesn't fit in the text-view's width.

    // Calculates visibility for each type of ellipsize policies.
    switch( layoutParameters.mExceedPolicy )
    {
      case TextView::EllipsizeEndOriginal:
      {
        // Ellipsizes the text if it doesn't fit in the width but it doesn't ellipsize if the text doesn't fit in the height.

        isPartiallyVisible = CalculateVisibilityForEllipsizeEndOriginal( characterLayoutInfo,
                                                                         ellipsizeParameters );

        break;
      }
      case TextView::SplitEllipsizeEnd:
      case TextView::EllipsizeEnd:
      {
        // Ellipsizes the text if it doesn't fit in the width and fully fits in the text-view's height.

        isPartiallyVisible = CalculateVisibilityForEllipsizeEnd( characterLayoutInfo,
                                                                 ellipsizeParameters );

        break;
      }
      default:
      {
        DALI_ASSERT_DEBUG( !"TextViewRelayout::CalculateVisibilityForEllipsize. Wrong exceed value." );
        break;
      }
    }
  }

  // If the current character is not fully visible but is partially visible, it is cut by the boundary of the text-view.
  // In that case, the charater needs to be replaced by the ellipsize text.
  ellipsizeParameters.mCreateEllipsizedTextActors = ( !characterLayoutInfo.mIsVisible && isPartiallyVisible );
}

void CreateEllipsizeTextActor( const EllipsizeParameters& ellipsizeParameters,
                               TextView::RelayoutData& relayoutData )
{
  // The default ellipsize text is '...' and all dots have the same style. However, a differernt ellipsize text could be set and it can have characters with differernt styles.
  // The code bellow creates the text-actors needed for the ellipsize text.

  // Set ellipsize's position by the end of visible text.
  Vector3 ellipsizePosition = ellipsizeParameters.mPosition;
  // Stores current ellipsize text.
  Text ellipsizeText;
  // Stores current ellipsize style.
  TextStyle ellipsizeStyle;
  // Stores the current size.
  Size ellipsizeSize;
  //Whether current glyph is an emoticon.
  bool isColorGlyph = false;

  float bearingOffset = 0.f;

  // Create ellipsize text-actor.
  std::size_t characterIndex = 0u;
  for( TextViewProcessor::CharacterLayoutInfoContainer::const_iterator ellipsizeCharacterLayoutIt = relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mCharactersLayoutInfo.begin(),
         endEllipsizeCharacterLayoutIt = relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mCharactersLayoutInfo.end();
       ellipsizeCharacterLayoutIt != endEllipsizeCharacterLayoutIt;
       ++ellipsizeCharacterLayoutIt, ++characterIndex )
  {
    const TextViewProcessor::CharacterLayoutInfo& ellipsizeCharacterLayoutInfo( *ellipsizeCharacterLayoutIt );
    const TextStyle& style = *( *( relayoutData.mTextLayoutInfo.mEllipsisTextStyles.Begin() + characterIndex ) );

    if( isColorGlyph ||
        ( isColorGlyph != ellipsizeCharacterLayoutInfo.mIsColorGlyph ) ||
        ( ellipsizeStyle != style ) )
    {
      // The style is different, so a new text-actor is needed.
      if( !ellipsizeText.IsEmpty() )
      {
        // It only creates a text-actor if there is any text.
        RenderableActor ellipsizeGlyphActor = CreateGlyphActor( ellipsizeText, ellipsizeStyle, relayoutData.mTextActorCache );
        ellipsizeGlyphActor.SetSize( ellipsizeSize );
        ellipsizeGlyphActor.SetPosition( Vector3( ellipsizePosition.x, ellipsizePosition.y - bearingOffset, ellipsizePosition.z ) );

        // Updates the position for the next text-actor.
        ellipsizePosition.x += ellipsizeSize.width;

        // Adds the text-actor to the list.
        relayoutData.mEllipsizedGlyphActors.push_back( ellipsizeGlyphActor );
      }

      // Resets the current ellipsize info.
      ellipsizeText = Text( relayoutData.mTextLayoutInfo.mEllipsisText[characterIndex] );
      ellipsizeStyle = style;
      ellipsizeSize = ellipsizeCharacterLayoutInfo.mSize;
      isColorGlyph = ellipsizeCharacterLayoutInfo.mIsColorGlyph;

      bearingOffset = ( ellipsizeParameters.mLineDescender - ( ellipsizeCharacterLayoutInfo.mSize.height - ellipsizeCharacterLayoutInfo.mAscender ) ) * relayoutData.mShrinkFactor;
    }
    else
    {
      // Updates text and size with the new character.
      ellipsizeText.Append( relayoutData.mTextLayoutInfo.mEllipsisText[characterIndex] );
      TextViewProcessor::UpdateSize( ellipsizeSize, ellipsizeCharacterLayoutInfo.mSize );
    }
  }

  if( !ellipsizeText.IsEmpty() )
  {
    // Creates the last glyph-actor.
    RenderableActor ellipsizeGlyphActor = CreateGlyphActor( ellipsizeText, ellipsizeStyle, relayoutData.mTextActorCache );
    ellipsizeGlyphActor.SetSize( ellipsizeSize );
    ellipsizeGlyphActor.SetPosition( Vector3( ellipsizePosition.x, ellipsizePosition.y - bearingOffset, ellipsizePosition.z ) );

    // Adds the glyph-actor to the list.
    relayoutData.mEllipsizedGlyphActors.push_back( ellipsizeGlyphActor );
  }
}

void EllipsizeLine( const TextView::LayoutParameters& layoutParameters,
                    EllipsizeParameters& ellipsizeParameters,
                    TextView::RelayoutData& relayoutData )
{
  // Traverses the text layout info from the first character of the line
  // to the last one setting to each character its visibility. If needed, it adds the ellipsize text (...).

  // Indices to the first character of the line.
  TextViewProcessor::TextInfoIndices firstIndices;
  TextViewProcessor::GetIndicesFromGlobalCharacterIndex( ellipsizeParameters.mFirstIndex,
                                                         relayoutData.mTextLayoutInfo,
                                                         firstIndices );

  // Indices to the last character of the line.
  TextViewProcessor::TextInfoIndices lastIndices;
  TextViewProcessor::GetIndicesFromGlobalCharacterIndex( ellipsizeParameters.mLastIndex,
                                                         relayoutData.mTextLayoutInfo,
                                                         lastIndices );

  // Defines a boundary by substracting the ellipsize-text's width to the text-view's width.
  // This is the boundary used to check if a character have to be ellipsized.
  ellipsizeParameters.mEllipsizeBoundary = relayoutData.mTextViewSize;
  ellipsizeParameters.mEllipsizeBoundary.width -= relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mSize.width;

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + firstIndices.mParagraphIndex,
         endParagraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + lastIndices.mParagraphIndex + 1u;
       paragraphLayoutIt != endParagraphLayoutIt;
       ++paragraphLayoutIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphLayoutIt );

    ellipsizeParameters.mLineFits = ellipsizeParameters.mIsLineWidthFullyVisible && ellipsizeParameters.mIsLineHeightFullyVisible && ellipsizeParameters.mIsNextLineFullyVisibleHeight;

    if( !ellipsizeParameters.mIsNextLineFullyVisibleHeight )
    {
      ellipsizeParameters.mEllipsizeBoundary.width = ellipsizeParameters.mLineWidth;
    }

    bool firstWord = true;
    bool lastWord = false;

    std::size_t wordCount = 0u;

    const bool isRightToLeftLayout = NULL != paragraphLayoutInfo.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraphLayoutInfo.mRightToLeftLayout->mWordsLayoutInfo : paragraphLayoutInfo.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordsLayoutInfo.begin() + firstIndices.mWordIndex,
           endWordLayoutIt = wordsLayoutInfo.begin() + lastIndices.mWordIndex + 1u;
         wordLayoutIt != endWordLayoutIt;
         ++wordLayoutIt, ++wordCount )
    {
      TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );

      if( wordCount == lastIndices.mWordIndex - firstIndices.mWordIndex )
      {
        lastWord = true;
      }

      const std::size_t firstCharacterIndex = firstWord ? firstIndices.mCharacterIndex : 0u;
      const std::size_t lastCharacterIndex = lastWord ? lastIndices.mCharacterIndex : wordLayoutInfo.mCharactersLayoutInfo.size() - 1u;
      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin() + firstCharacterIndex,
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin() + lastCharacterIndex + 1u;
           characterLayoutIt != endCharacterLayoutIt;
           ++characterLayoutIt )
      {
        TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

        if( ellipsizeParameters.mEllipsizeLine )
        {
          // Calculates the character visibility and whether it needs to be replace by ellipsized text.
          CalculateVisibilityForEllipsize( layoutParameters,
                                           characterLayoutInfo,
                                           ellipsizeParameters,
                                           relayoutData );

          if( ellipsizeParameters.mCreateEllipsizedTextActors )
          {
            // Create ellipsize text-actors if the character needs to be replaced.
            CreateEllipsizeTextActor( ellipsizeParameters,
                                      relayoutData );
          }
        }
        else
        {
          if( ( TextView::EllipsizeEnd == layoutParameters.mExceedPolicy ) ||
              ( TextView::SplitEllipsizeEnd == layoutParameters.mExceedPolicy ))
          {
            if( !ellipsizeParameters.mIsLineHeightFullyVisible )
            {
              // Make characters invisible.
              characterLayoutInfo.mIsVisible = false;
            }
          }
        }
      } // end characters
      firstWord = false;
    } // end words
  } // end paragraphs
}

void SetTextVisible( TextView::RelayoutData& relayoutData )
{
  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(),
         endParagraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphLayoutIt != endParagraphLayoutIt;
       ++paragraphLayoutIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphLayoutIt );
    std::size_t characterIndex = 0u;

    const bool isRightToLeftLayout = NULL != paragraphLayoutInfo.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraphLayoutInfo.mRightToLeftLayout->mWordsLayoutInfo : paragraphLayoutInfo.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordsLayoutInfo.begin(),
           endWordLayoutIt = wordsLayoutInfo.end();
         wordLayoutIt != endWordLayoutIt;
         ++wordLayoutIt )
    {
      TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           characterLayoutIt != endCharacterLayoutIt;
           ++characterLayoutIt, ++characterIndex )
      {
        TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

        characterLayoutInfo.mIsVisible = true;
        delete characterLayoutInfo.mGradientInfo;
        characterLayoutInfo.mGradientInfo = NULL;
        characterLayoutInfo.mColorAlpha = ( *( paragraphLayoutInfo.mTextStyles.Begin() + characterIndex ) )->GetTextColor().a;
      } // end characters
    } // end words
  } // end paragraphs

  // Updates the visibility for text-input..
  for( std::vector<Toolkit::TextView::CharacterLayoutInfo>::iterator it = relayoutData.mCharacterLayoutInfoTable.begin(),
         endIt = relayoutData.mCharacterLayoutInfoTable.end();
       it != endIt;
       ++it )
  {
    Toolkit::TextView::CharacterLayoutInfo& characterLayoutInfo( *it );

    characterLayoutInfo.mIsVisible = true;
  }
}

void UpdateVisibilityForFade( const TextView::LayoutParameters& layoutParameters,
                              const TextView::VisualParameters& visualParameters,
                              TextView::RelayoutData& relayoutData )
{
  RelayoutParameters relayoutParameters;
  FadeParameters fadeParameters;

  // Calculates the fade thresholds (from where the text starts to fade out). If any of the fade boundaries is zero, it sets a very small value just to avoid a zero division.
  fadeParameters.mRightFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mRight );
  fadeParameters.mRightFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mRight > 0u ? fadeParameters.mRightFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mRightFadeThreshold = relayoutData.mTextViewSize.width - fadeParameters.mRightFadeBoundary;
  fadeParameters.mRightFadeThresholdOffset = relayoutData.mTextViewSize.width - fadeParameters.mRightFadeBoundaryOffset;
  fadeParameters.mLeftFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mLeft );
  fadeParameters.mLeftFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mLeft > 0u ? fadeParameters.mLeftFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mLeftFadeThreshold = fadeParameters.mLeftFadeBoundary;
  fadeParameters.mLeftFadeThresholdOffset = fadeParameters.mLeftFadeBoundaryOffset;
  fadeParameters.mTopFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mTop );
  fadeParameters.mTopFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mTop > 0u ? fadeParameters.mTopFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mTopFadeThreshold = fadeParameters.mTopFadeBoundary;
  fadeParameters.mTopFadeThresholdOffset = fadeParameters.mTopFadeBoundaryOffset;
  fadeParameters.mBottomFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mBottom );
  fadeParameters.mBottomFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mBottom > 0u ? fadeParameters.mBottomFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mBottomFadeThreshold = relayoutData.mTextViewSize.height - fadeParameters.mBottomFadeBoundary;
  fadeParameters.mBottomFadeThresholdOffset = relayoutData.mTextViewSize.height - fadeParameters.mBottomFadeBoundaryOffset;

  // Calculates the fade out rect coeficients for the right, left, top and bottom sides of the text-view.
  fadeParameters.mRightAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mRightFadeThresholdOffset, 1.f ), Vector2( relayoutData.mTextViewSize.width, 0.f ) );
  fadeParameters.mLeftAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mLeftFadeThresholdOffset, 1.f ), Vector2( 0.f, 0.f ) );
  fadeParameters.mTopAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mTopFadeThresholdOffset, 1.f ), Vector2( 0.f, 0.f ) );
  fadeParameters.mBottomAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mBottomFadeThresholdOffset, 1.f ), Vector2( relayoutData.mTextViewSize.height, 0.f ) );

  // Traverses all characters and calculates the visibility.

  std::size_t infoTableCharacterIndex = 0u;

  relayoutParameters.mIndices.mParagraphIndex = 0u;

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(),
         endParagraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphLayoutIt != endParagraphLayoutIt;
       ++paragraphLayoutIt, ++relayoutParameters.mIndices.mParagraphIndex )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphLayoutIt );

    std::size_t characterIndex = 0u;
    relayoutParameters.mIndices.mWordIndex = 0u;

    const bool isRightToLeftLayout = NULL != paragraphLayoutInfo.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraphLayoutInfo.mRightToLeftLayout->mWordsLayoutInfo : paragraphLayoutInfo.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordsLayoutInfo.begin(),
           endWordLayoutIt = wordsLayoutInfo.end();
         wordLayoutIt != endWordLayoutIt;
         ++wordLayoutIt, ++relayoutParameters.mIndices.mWordIndex )
    {
      TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );

      relayoutParameters.mIsFirstCharacterOfWord = true;
      relayoutParameters.mWordSize = wordLayoutInfo.mSize;
      relayoutParameters.mIndices.mCharacterIndex = 0u;

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           characterLayoutIt != endCharacterLayoutIt;
           ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex, ++infoTableCharacterIndex, ++characterIndex )
      {
        TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

        relayoutParameters.mIsVisible = true;
        fadeParameters.mIsPartiallyVisible = false;

        // Calculates the visibility for the current character.
        CalculateVisibilityForFade( layoutParameters,
                                    characterLayoutInfo,
                                    *( *( paragraphLayoutInfo.mTextStyles.Begin() + characterIndex ) ),
                                    relayoutParameters,
                                    fadeParameters,
                                    relayoutData );

        // Updates the visibility for text-input..
        std::vector<Toolkit::TextView::CharacterLayoutInfo>::iterator it = relayoutData.mCharacterLayoutInfoTable.begin() + relayoutData.mCharacterVisualToLogicalMap[infoTableCharacterIndex];

        Toolkit::TextView::CharacterLayoutInfo& characterLayoutTableInfo( *it );

        characterLayoutTableInfo.mIsVisible = relayoutParameters.mIsVisible;

        relayoutParameters.mIsFirstCharacterOfWord = false;
      } // end character
    } // end words
  } // end paragraphs
}

void UpdateVisibilityForEllipsize( const TextView::LayoutParameters& layoutParameters,
                                   const TextView::VisualParameters& visualParameters,
                                   TextView::RelayoutData& relayoutData )
{
  // TODO check ellipsis with rtl text.

  // Traverses the lines and checks which ones doesn't fit in the text-view's boundary.
  for( Toolkit::TextView::LineLayoutInfoContainer::const_iterator lineInfoIt = relayoutData.mLines.begin(), endLineInfoIt = relayoutData.mLines.end();
       lineInfoIt != endLineInfoIt;
       ++lineInfoIt )
  {
    const Toolkit::TextView::LineLayoutInfo& lineInfo( *lineInfoIt );

    // To check if a line fits in the text-view's boundary,
    // get the position of the first character is needed and do the test
    // with the line size.

    // An bearing offset may have been applied to the first character so it's needed to
    // get the start position of the line.

    // Some parameters used in the CalculateVisibilityForEllipsize() function.
    EllipsizeParameters ellipsizeParameters;

    // Retrieves the first index and the last index of the line.
    ellipsizeParameters.mFirstIndex = lineInfo.mCharacterGlobalIndex;
    ellipsizeParameters.mLastIndex = 0u;
    if( ( lineInfoIt + 1u ) != endLineInfoIt )
    {
      const Toolkit::TextView::LineLayoutInfo& nextLineInfo( *( lineInfoIt + 1u ) );
      ellipsizeParameters.mLastIndex = nextLineInfo.mCharacterGlobalIndex - 1u;
    }
    else
    {
      ellipsizeParameters.mLastIndex = relayoutData.mCharacterLayoutInfoTable.size() - 1u;
    }

    // Retrieves the first character of the line and build the position of the line with the bearing.
    const Toolkit::TextView::CharacterLayoutInfo& characterInfo = *( relayoutData.mCharacterLayoutInfoTable.begin() + ellipsizeParameters.mFirstIndex );

    // Calculates the bearing offset applied to the first character.
    const float bearingOffset = ( lineInfo.mSize.height - lineInfo.mAscender ) - characterInfo.mDescender;

    // Build the position of the line by removing the bearing offset from the first character's position.
    const Vector3 position( characterInfo.mPosition.x,
                            characterInfo.mPosition.y + bearingOffset,
                            characterInfo.mPosition.z );

    // Checks if the line needs to be ellipsized,
    ellipsizeParameters.mIsLineWidthFullyVisible = IsVisible( position,
                                                              lineInfo.mSize,
                                                              relayoutData.mTextViewSize,
                                                              FULLY_VISIBLE_WIDTH );

    // If the exceed policy is EllipsizeEndOriginal it's enough to check
    // if the line fits in the width.
    ellipsizeParameters.mEllipsizeLine = !ellipsizeParameters.mIsLineWidthFullyVisible;

    // If the exceed policy is EllipsizeEnd, it's needed to check if the next line exceeds the text-view's height.
    // If the next line exceeds the text-view height then it's going to be invisible and current line needs to be ellipsized.
    ellipsizeParameters.mIsLineHeightFullyVisible = true;
    ellipsizeParameters.mIsNextLineFullyVisibleHeight = true;
    if( ( TextView::EllipsizeEnd == layoutParameters.mExceedPolicy ) ||
        ( TextView::SplitEllipsizeEnd == layoutParameters.mExceedPolicy ) )
    {
      // Need to check if there is lines which doesn't fit in the height.

      ellipsizeParameters.mIsLineHeightFullyVisible = IsVisible( position,
                                                                 lineInfo.mSize,
                                                                 relayoutData.mTextViewSize,
                                                                 FULLY_VISIBLE_HEIGHT );

      ellipsizeParameters.mEllipsizeLine = ellipsizeParameters.mEllipsizeLine && ellipsizeParameters.mIsLineHeightFullyVisible;

      if( ellipsizeParameters.mIsLineHeightFullyVisible && !ellipsizeParameters.mEllipsizeLine )
      {
        // Current line is not ellipsized.
        // Need to check if there is a next line and if it's not visible. If there is, current line needs to be ellipsized.
        Toolkit::TextView::LineLayoutInfoContainer::const_iterator nextLineInfoIt = lineInfoIt + 1u;
        if( nextLineInfoIt != endLineInfoIt )
        {
          // Retrives the position of the first character of the line and remove
          // the bearing offset to build to build the position of the line.
          const Toolkit::TextView::LineLayoutInfo& nextLineInfo( *nextLineInfoIt );
          const Toolkit::TextView::CharacterLayoutInfo& characterInfo = *( relayoutData.mCharacterLayoutInfoTable.begin() + nextLineInfo.mCharacterGlobalIndex );

          const float bearingOffset = ( ( lineInfo.mSize.height - lineInfo.mAscender ) - characterInfo.mDescender ) * relayoutData.mShrinkFactor;

          const Vector3 position( characterInfo.mPosition.x,
                                  characterInfo.mPosition.y + bearingOffset,
                                  characterInfo.mPosition.z );

          ellipsizeParameters.mIsNextLineFullyVisibleHeight = IsVisible( position,
                                                                         nextLineInfo.mSize,
                                                                         relayoutData.mTextViewSize,
                                                                         FULLY_VISIBLE_HEIGHT );

          // If the next line is not visible, current line have to be ellipsized.
          ellipsizeParameters.mEllipsizeLine = !ellipsizeParameters.mIsNextLineFullyVisibleHeight;
        }
      }
    }

    if( !ellipsizeParameters.mIsNextLineFullyVisibleHeight )
    {
      ellipsizeParameters.mLineWidth = position.x + lineInfo.mSize.width - relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mSize.width;
    }

    // Sets the line descender.
    ellipsizeParameters.mLineDescender = lineInfo.mSize.height - lineInfo.mAscender;

    // At this point, ellipsizeLine distinguish if a piece of line have to be ellipsized or not.
    EllipsizeLine( layoutParameters, ellipsizeParameters, relayoutData );
  }
}

void UpdateVisibility( const TextView::LayoutParameters& layoutParameters,
                       const TextView::VisualParameters& visualParameters,
                       TextView::RelayoutData& relayoutData )
{
  switch( layoutParameters.mExceedPolicy )
  {
    case TextView::FadeOriginal:
    case TextView::OriginalFade:
    case TextView::Fade:
    case TextView::SplitFade: // Fall through
    {
      UpdateVisibilityForFade( layoutParameters,
                               visualParameters,
                               relayoutData );
      break;
    }
    case TextView::EllipsizeEndOriginal:
    case TextView::SplitEllipsizeEnd:
    case TextView::EllipsizeEnd: // Fall through
    {
      // Set first all characters to visible as UpdateVisibilityForEllipsize() doesn't traverse all of them.
      SetTextVisible( relayoutData );

      UpdateVisibilityForEllipsize( layoutParameters,
                                    visualParameters,
                                    relayoutData );
      break;
    }
    default:
    {
      SetTextVisible( relayoutData );
      break;
    }
  }
}

/**
 * Creates an image actor for the emoticon.
 *
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] characterLayout Layout info for the character.
 * @param[in] character The character.
 */
void CreateEmoticon( const TextView::VisualParameters& visualParameters,
                     TextViewProcessor::CharacterLayoutInfo& characterLayout,
                     const Character& character )
{
  // The character is an emoticon.
  ImageActor imageActor = ImageActor::DownCast( characterLayout.mGlyphActor );
  if( !imageActor )
  {
    imageActor = ImageActor::New();

    GlyphImage image = GlyphImage::New( character );

    if( image )
    {
      imageActor.SetImage( image );
    }
  }

  imageActor.SetPosition( Vector3( characterLayout.mPosition.x + characterLayout.mOffset.x,
                                   characterLayout.mPosition.y + characterLayout.mOffset.y,
                                   characterLayout.mPosition.z ) );
  imageActor.SetSize( characterLayout.mSize );

  // Sets the sort modifier value.
  imageActor.SetSortModifier( visualParameters.mSortModifier );

  characterLayout.mGlyphActor = imageActor;
}

/**
 * Creates text-actors for the given text.
 *
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in,out] paragraph Layout info for the paragraph.
 * @param[in,out] wordLayout Layout info for the word.
 * @param[in,out] characterLayout Layout info for the character.
 * @param[in] character The character.
 * @param[in] style The character's style.
 * @param[in,out] currentTextActorInfo Temporary stores the text-actor's info to be set.
 * @param[in,out] createGlyphActors Whether to initialize renderable-actor handles.
 * @param[in,out] textActorCreated Whether a text-actor
 */
void CreateTextActor( const TextView::VisualParameters& visualParameters,
                      TextView::RelayoutData& relayoutData,
                      const TextViewProcessor::ParagraphLayoutInfo& paragraph,
                      TextViewProcessor::WordLayoutInfo& wordLayout,
                      TextViewProcessor::CharacterLayoutInfo& characterLayout,
                      const Character& character,
                      const TextStyle& style,
                      CurrentTextActorInfo& currentTextActorInfo,
                      bool createGlyphActors,
                      bool& textActorCreated )
{
  textActorCreated = false;

  // Set the text-actor for the current traversed text.
  if( currentTextActorInfo.textActor )
  {
    if( ( NULL != currentTextActorInfo.characterLayout ) &&
        currentTextActorInfo.characterLayout->mSetText )
    {
      currentTextActorInfo.textActor.SetText( currentTextActorInfo.text );
      currentTextActorInfo.characterLayout->mSetText = false;
    }
    currentTextActorInfo.textActor.SetPosition( currentTextActorInfo.position );
    currentTextActorInfo.textActor.SetSize( currentTextActorInfo.size );

    SetVisualParameters( currentTextActorInfo,
                         visualParameters,
                         relayoutData,
                         paragraph.mSize.height );
  }

  float rightToLeftOffset = 0.f;
  if( character.IsWhiteSpace() )
  {
    // In left to right text, a word never starts with a white space but
    // it may happen in right to left text as the text is reversed.
    // The text alignment and justification offset is calculated without this white space.
    // It causes a missalignment which can be corrected by removing the size of the white space.
    rightToLeftOffset = characterLayout.mSize.width * relayoutData.mShrinkFactor;
  }

  // Whether this word is not a white space or if it is, it is underlined.
  // Don't want to create text-actors for white spaces unless they are underlined.
  bool isNotWhiteSpace = ( TextViewProcessor::NoSeparator == wordLayout.mType ) || ( ( TextViewProcessor::WordSeparator == wordLayout.mType ) && style.IsUnderlineEnabled() );

  if( isNotWhiteSpace )
  {
    currentTextActorInfo.text = Text( character );
  }
  else
  {
    currentTextActorInfo.text = Text();
  }
  currentTextActorInfo.position = Vector3( characterLayout.mPosition.x + characterLayout.mOffset.x - rightToLeftOffset,
                                           characterLayout.mPosition.y + characterLayout.mOffset.y,
                                           characterLayout.mPosition.z );
  currentTextActorInfo.size = characterLayout.mSize * relayoutData.mShrinkFactor;

  currentTextActorInfo.color = style.GetTextColor();
  currentTextActorInfo.color.a = characterLayout.mColorAlpha;

  TextActor textActor = TextActor::DownCast( characterLayout.mGlyphActor );

  if( createGlyphActors &&  isNotWhiteSpace )
  {
    textActorCreated = true;
    if( textActor )
    {
      // Try to reuse first the text-actor of this character.
      textActor.SetTextStyle( style );
    }
    else
    {
      // If there is no text-actor, try to retrieve one from the cache.
      textActor = relayoutData.mTextActorCache.RetrieveTextActor();

      // If still there is no text-actor, create one.
      if( !textActor )
      {
        TextActorParameters parameters( style, TextActorParameters::FONT_DETECTION_OFF );
        textActor = TextActor::New( Text(), parameters );
        textActor.SetRelayoutEnabled( false );
      }
      else
      {
        textActor.SetTextStyle( style );
      }
    }
    characterLayout.mSetText = true;
    currentTextActorInfo.characterLayout = &characterLayout;

    characterLayout.mGlyphActor = textActor;
  }

  // Update the current text-actor.
  currentTextActorInfo.textActor = textActor;
}

/**
 * Traverses the whole paragraph initializating renderable-actor handles and updating them with the new size and position.
 *
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in,out] paragraph Layout info for the paragraph.
 * @param[in,out] characterGlobalIndex Index to the character within the whole text.
 * @param[in,out] lineLayoutInfoIndex Index to the table of lines.
 * @param[in,out] createGlyphActors Whether to initialize renderable-actor handles.
 */
void UpdateTextActorInfoForParagraph( const TextView::VisualParameters& visualParameters,
                                      TextView::RelayoutData& relayoutData,
                                      TextViewProcessor::ParagraphLayoutInfo& paragraphLayout,
                                      std::size_t& characterGlobalIndex,
                                      std::size_t& lineLayoutInfoIndex,
                                      bool createGlyphActors )
{
  CurrentTextActorInfo currentTextActorInfo;
  currentTextActorInfo.characterLayout = NULL;

  const std::size_t lineLayoutInfoSize = relayoutData.mLines.size(); // Number of lines.
  bool lineLayoutEnd = false;    // Whether lineLayoutInfoIndex points at the last line.
  bool textActorCreated = false; // Whether a text actor has been created for this the current group of characters traversed.

  TextStyle currentStyle;        // style for the current text-actor.

  TextViewProcessor::GradientInfo* currentGradientInfo = NULL; // gradient color for the current text-actor.
                                                               // start point for the current text-actor.
                                                               // end point for the current text-actor.

  bool currentIsColorGlyph = false;      // Whether current glyph is an emoticon.

  std::vector<TextActor> textActorsToRemove; // Keep a vector of text-actors to be included into the cache.

  // Retrieve the layout info to traverse. If there is right to left text it retrieves the right to left layout.
  const bool isRightToLeftLayout = NULL != paragraphLayout.mRightToLeftLayout;

  TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraphLayout.mRightToLeftLayout->mWordsLayoutInfo : paragraphLayout.mWordsLayoutInfo;
  Text& text = isRightToLeftLayout ? paragraphLayout.mRightToLeftLayout->mText : paragraphLayout.mText;
  Vector<TextStyle*>& textStyles = isRightToLeftLayout ? paragraphLayout.mRightToLeftLayout->mTextStyles : paragraphLayout.mTextStyles;

  // In case the previous right to left layout has been cleared, all text-actors have been removed as well. If this bool is set to true, text-actors will be created again.
  createGlyphActors = createGlyphActors || ( ( isRightToLeftLayout ) ? paragraphLayout.mRightToLeftLayout->mPreviousLayoutCleared : false );

  std::size_t characterParagraphIndex = 0u;   // Index to the character (within the paragraph).
  for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = wordsLayoutInfo.begin(), wordEndIt = wordsLayoutInfo.end();
       wordIt != wordEndIt;
       ++wordIt )
  {
    TextViewProcessor::WordLayoutInfo& wordLayout( *wordIt );

    for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterIt = wordLayout.mCharactersLayoutInfo.begin(), characterEndIt = wordLayout.mCharactersLayoutInfo.end();
         characterIt != characterEndIt;
         ++characterIt )
    {
      TextViewProcessor::CharacterLayoutInfo& characterLayout( *characterIt );

      // Check if there is a new line.
      const bool newLine = !lineLayoutEnd && ( characterGlobalIndex == relayoutData.mLines[lineLayoutInfoIndex].mCharacterGlobalIndex );

      if( newLine )
      {
        // Point to the next line.
        ++lineLayoutInfoIndex;
        if( lineLayoutInfoIndex >= lineLayoutInfoSize )
        {
          // Arrived at last line.
          lineLayoutEnd = true; // Avoids access out of bounds in the relayoutData.mLines vector.
        }
        textActorCreated = false;
      }

      // Do not create a glyph-actor if there is no text.
      const Character character = text[characterParagraphIndex];
      const TextStyle& style = *( *( textStyles.Begin() + characterParagraphIndex ) );

      // Check if the character has the same gradient info than the current one.
      bool differentGradientInfo = false;
      if( characterLayout.mGradientInfo && currentGradientInfo )
      {
              differentGradientInfo = ( characterLayout.mGradientInfo->mGradientColor != currentGradientInfo->mGradientColor ) ||
                ( characterLayout.mGradientInfo->mStartPoint != currentGradientInfo->mStartPoint ) ||
                ( characterLayout.mGradientInfo->mEndPoint != currentGradientInfo->mEndPoint );
      }
      else if( ( NULL != currentGradientInfo ) || ( NULL != characterLayout.mGradientInfo ) )
      {
              differentGradientInfo = true;
      }

      if( ( createGlyphActors && !textActorCreated ) ||
          characterLayout.mIsColorGlyph ||
          differentGradientInfo ||
          ( characterLayout.mIsColorGlyph != currentIsColorGlyph ) ||
          ( style != currentStyle ) )
      {
        characterLayout.mSetText = false;
        characterLayout.mSetStyle = false;

        if( characterLayout.mIsColorGlyph )
        {
          CreateEmoticon( visualParameters,
                          characterLayout,
                          character );

          characterLayout.mGlyphActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
          characterLayout.mGlyphActor.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
        }
        else
        {
          // There is a new style or a new line.

          CreateTextActor( visualParameters,
                           relayoutData,
                           paragraphLayout,
                           wordLayout,
                           characterLayout,
                           character,
                           style,
                           currentTextActorInfo,
                           createGlyphActors,
                           textActorCreated );

          if( textActorCreated )
          {
            characterLayout.mGlyphActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
            characterLayout.mGlyphActor.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
          }
        }

        // Update style to be checked with next characters.
        currentStyle = style;
        currentGradientInfo = characterLayout.mGradientInfo;
        currentIsColorGlyph = characterLayout.mIsColorGlyph;
      }
      else
      {
        DALI_ASSERT_DEBUG( !characterLayout.mIsColorGlyph && "TextViewProcessor::InitializeTextActorInfo. An image-actor doesn't store more than one emoticon." );

        // Same style than previous one.

        // Add the character to the current text-actor and update the size.
        if( characterLayout.mIsVisible && ( TextViewProcessor::ParagraphSeparator != wordLayout.mType ) )
        {
          currentTextActorInfo.text.Append( character );

          currentTextActorInfo.position.y = std::min( currentTextActorInfo.position.y, ( characterLayout.mPosition.y + characterLayout.mOffset.y ) );
          currentTextActorInfo.size.width += characterLayout.mSize.width * relayoutData.mShrinkFactor;
          currentTextActorInfo.size.height = std::max( currentTextActorInfo.size.height, characterLayout.mSize.height * relayoutData.mShrinkFactor );
        }
      }

      if( ( createGlyphActors ) &&
          !characterLayout.mIsColorGlyph &&
          !textActorCreated )
      {
        TextActor textActor = TextActor::DownCast( characterLayout.mGlyphActor );
        if( textActor )
        {
          // There is a previously created text-actor for this character.
          // If this character has another one put it into the cache.
          textActor.SetText( "" );
          textActorsToRemove.push_back( textActor );
        }

        if( characterLayout.mGlyphActor )
        {
          characterLayout.mGlyphActor.Reset();
        }
      }
      ++characterGlobalIndex;
      ++characterParagraphIndex;
    } // characters
  } // words

  if( !currentTextActorInfo.text.IsEmpty() )
  {
    if( currentTextActorInfo.textActor )
    {
      if( ( NULL != currentTextActorInfo.characterLayout ) &&
          currentTextActorInfo.characterLayout->mSetText )
      {
        currentTextActorInfo.textActor.SetText( currentTextActorInfo.text );
        currentTextActorInfo.characterLayout->mSetText = false;
      }
      currentTextActorInfo.textActor.SetPosition( currentTextActorInfo.position );
      currentTextActorInfo.textActor.SetSize( currentTextActorInfo.size );

      SetVisualParameters( currentTextActorInfo,
                           visualParameters,
                           relayoutData,
                           paragraphLayout.mSize.height );
    }
  }

  // Insert the spare text-actors into the cache.
  relayoutData.mTextActorCache.InsertTextActors( textActorsToRemove );
}

void UpdateTextActorInfo( const TextView::VisualParameters& visualParameters,
                          TextView::RelayoutData& relayoutData,
                          bool createGlyphActors )
{
  if( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.empty() )
  {
    // nothing to do if there is no paragraphs.
    return;
  }

  std::size_t characterGlobalIndex = 0u; // Index to the global character (within the whole text).
  std::size_t lineLayoutInfoIndex = 0u;  // Index to the line info.

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraph( *paragraphIt );

    UpdateTextActorInfoForParagraph( visualParameters,
                                     relayoutData,
                                     paragraph,
                                     characterGlobalIndex,
                                     lineLayoutInfoIndex,
                                     createGlyphActors );
  } // paragraphs

  // Set visual parameters for ellipsis renderable actors.
  for( std::vector<RenderableActor>::iterator it = relayoutData.mEllipsizedGlyphActors.begin(),
         endIt = relayoutData.mEllipsizedGlyphActors.end();
       it != endIt;
       ++it )
  {
    RenderableActor glyphActor = ( *it );

    glyphActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    glyphActor.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );

    // Sets the sort modifier value.
    glyphActor.SetSortModifier( visualParameters.mSortModifier );

    // Enables or disables the blending.
    glyphActor.SetBlendMode( !visualParameters.mSnapshotModeEnabled ? BlendingMode::ON : BlendingMode::OFF );
  }
}

void CalculateUnderlineInfo( TextView::RelayoutData& relayoutData, TextViewRelayout::TextUnderlineStatus& textUnderlineStatus )
{
  // Traverse the whole text to find all groups of consecutive underlined characters in the same line.
  //
  // Note that relayoutData.mTextLayoutInfo contains layout info per paragraph but these paragraphs are the result of split the whole text every time a '\n' is found.
  // According with the layout option, one of this paragraphs could be laid-out in more than one line.

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraph( *paragraphIt );

    std::size_t characterIndex = 0u;

    const bool isRightToLeftLayout = NULL != paragraph.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraph.mRightToLeftLayout->mWordsLayoutInfo : paragraph.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = wordsLayoutInfo.begin(), wordEndIt = wordsLayoutInfo.end();
         wordIt != wordEndIt;
         ++wordIt )
    {
      TextViewProcessor::WordLayoutInfo& word( *wordIt );

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
           characterIt != characterEndIt;
           ++characterIt, ++characterIndex )
      {
        TextViewProcessor::CharacterLayoutInfo& character( *characterIt );
        const TextStyle& style = *( *( paragraph.mTextStyles.Begin() + characterIndex ) );

        // Check if current character is the first of a new line
        const bool isNewLine = ( textUnderlineStatus.mLineGlobalIndex < relayoutData.mLines.size() ) &&
          ( textUnderlineStatus.mCharacterGlobalIndex == ( *( relayoutData.mLines.begin() + textUnderlineStatus.mLineGlobalIndex ) ).mCharacterGlobalIndex );
        if( isNewLine )
        {
          ++textUnderlineStatus.mLineGlobalIndex; // If it's a new line, point to the next one.
        }

        if( style.IsUnderlineEnabled() )
        {
          if( !textUnderlineStatus.mCurrentUnderlineStatus || // Current character is underlined but previous one it wasn't.
              isNewLine )                                     // Current character is underlined and is the first of current line.
          {
            // Create a new underline info for the current underlined characters.
            UnderlineInfo underlineInfo;
            underlineInfo.mMaxHeight = character.mSize.height;
            underlineInfo.mMaxThickness = character.mUnderlineThickness;
            underlineInfo.mPosition = character.mUnderlinePosition;

            textUnderlineStatus.mUnderlineInfo.push_back( underlineInfo );

            textUnderlineStatus.mCurrentUnderlineStatus = true; // Set the current text is underlined.
          }
          else
          {
            // Retrieve last underline info and update it if current underline thickness is bigger.
            UnderlineInfo& underlineInfo( *( textUnderlineStatus.mUnderlineInfo.end() - 1u ) );

            underlineInfo.mMaxHeight = std::max( underlineInfo.mMaxHeight, character.mSize.height );

            if( character.mUnderlineThickness > underlineInfo.mMaxThickness )
            {
              underlineInfo.mMaxThickness = character.mUnderlineThickness;
              underlineInfo.mPosition = character.mUnderlinePosition;
            }
          }
        }
        else
        {
          textUnderlineStatus.mCurrentUnderlineStatus = false;
        }

        ++textUnderlineStatus.mCharacterGlobalIndex;
      } // end characters.
    } // end words.
  } // end paragraphs.
}

void SetUnderlineInfo( TextView::RelayoutData& relayoutData )
{
  // Stores for each group of consecutive underlined characters in each line its maximum thicknes, its position of that thickness and the maximum character's height.
  TextViewRelayout::TextUnderlineStatus textUnderlineStatus;

  // Traverse the whole text to find all groups of consecutive underlined characters in the same line.
  CalculateUnderlineInfo( relayoutData, textUnderlineStatus );

  if( textUnderlineStatus.mUnderlineInfo.empty() )
  {
    // There is no underlined text. Just exit.
    return;
  }

  // At this point textUnderlineStatus.mUnderlineInfo has for each group of consecutive underlined characters their maximum thickness, position and maximum height.
  // Traverse the whole text and set the previously stored underline info in the text style.

  std::vector<UnderlineInfo>::const_iterator underlineInfoIt = textUnderlineStatus.mUnderlineInfo.begin();
  std::vector<UnderlineInfo>::const_iterator underlineInfoEndIt = textUnderlineStatus.mUnderlineInfo.end();

  UnderlineInfo underlineInfo;

  if( underlineInfoIt < underlineInfoEndIt )
  {
    underlineInfo = ( *underlineInfoIt );
  }

  // Whether current text is underlined.
  textUnderlineStatus.mCurrentUnderlineStatus = false;
  textUnderlineStatus.mCharacterGlobalIndex = 0u;
  textUnderlineStatus.mLineGlobalIndex = 0u;

  float currentLineHeight = 0.f;
  float currentLineAscender = 0.f;

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(), paragraphEndIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphIt != paragraphEndIt;
       ++paragraphIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraph( *paragraphIt );
    std::size_t characterIndex = 0u;

    const bool isRightToLeftLayout = NULL != paragraph.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraph.mRightToLeftLayout->mWordsLayoutInfo : paragraph.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = wordsLayoutInfo.begin(), wordEndIt = wordsLayoutInfo.end();
         wordIt != wordEndIt;
         ++wordIt )
    {
      TextViewProcessor::WordLayoutInfo& word( *wordIt );

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
           characterIt != characterEndIt;
           ++characterIt, ++characterIndex )
      {
        TextViewProcessor::CharacterLayoutInfo& character( *characterIt );
        TextStyle& style = *( *( paragraph.mTextStyles.Begin() + characterIndex ) );

        // Check if current character is the first of a new line

        bool isNewLine = false;

        if( textUnderlineStatus.mLineGlobalIndex < relayoutData.mLines.size() )
        {
          const Toolkit::TextView::LineLayoutInfo& lineLayoutInfo( *( relayoutData.mLines.begin() + textUnderlineStatus.mLineGlobalIndex ) );
          isNewLine = ( textUnderlineStatus.mCharacterGlobalIndex == lineLayoutInfo.mCharacterGlobalIndex );

          if( isNewLine )
          {
            currentLineHeight = lineLayoutInfo.mSize.height;
            currentLineAscender = lineLayoutInfo.mAscender;
            ++textUnderlineStatus.mLineGlobalIndex; // If it's a new line, point to the next one.
          }
        }

        if( style.IsUnderlineEnabled() )
        {
          if( textUnderlineStatus.mCurrentUnderlineStatus )
          {
            if( isNewLine )
            {
              // Retrieves the thickness and position for the next piece of underlined text.
              if( underlineInfoIt < underlineInfoEndIt )
              {
                ++underlineInfoIt;
                if( underlineInfoIt < underlineInfoEndIt )
                {
                  underlineInfo = *underlineInfoIt;
                }
              }
            }
          }

          textUnderlineStatus.mCurrentUnderlineStatus = true;

          // Before setting the position it needs to be adjusted to match the base line.
          const float bearingOffset = ( currentLineHeight - currentLineAscender ) - ( character.mSize.height - character.mAscender );
          const float positionOffset = ( underlineInfo.mMaxHeight - character.mSize.height ) - bearingOffset;

          // Sets the underline's parameters.
          style.SetUnderline( true, underlineInfo.mMaxThickness, underlineInfo.mPosition - positionOffset );

          // Mark the character to be set the new style into the text-actor.
          character.mSetStyle = true;
        }
        else
        {
          if( textUnderlineStatus.mCurrentUnderlineStatus )
          {
            textUnderlineStatus.mCurrentUnderlineStatus = false;

            // Retrieves the thickness and position for the next piece of underlined text.
            if( underlineInfoIt < underlineInfoEndIt )
            {
              ++underlineInfoIt;
              if( underlineInfoIt < underlineInfoEndIt )
              {
                underlineInfo = *underlineInfoIt;
              }
            }
          }
        }

        ++textUnderlineStatus.mCharacterGlobalIndex;
      } // end of characters.
    } // end of word.
  } // end of paragraphs.
}

void RemoveGlyphActors( Actor textView,
                        const std::vector<RenderableActor>& glyphActors )
{
  // Removes previously inserted renderable-actors.
  // The SplitByNewLineChar::Relayout(), SplitByWord::Relayout() and SplitByChar::Relayout() functions add
  // renderable-actors to the text-view. A handle to these renderable-actors are stored and passed to this function
  // in order to remove 'only' renderable-actors added by these functions.
  // Any other actor added by a programmer or application won't be removed.

  for( std::vector<RenderableActor>::const_reverse_iterator it = glyphActors.rbegin(), endIt = glyphActors.rend(); it != endIt; ++it )
  {
    textView.Remove( *it );
  }
}

void InsertToTextView( Actor textView,
                       TextView::RelayoutData& relayoutData )
{
  // Add text-actors to the text-view.

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(),
         endParagraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphLayoutIt != endParagraphLayoutIt;
       ++paragraphLayoutIt )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphLayoutIt );

    // Retrieve the layout info to traverse. If there is right to left text it retrieves the right to left layout.
    const bool isRightToLeftLayout = NULL != paragraphLayoutInfo.mRightToLeftLayout;
    TextViewProcessor::WordLayoutInfoContainer& wordsLayoutInfo = isRightToLeftLayout ? paragraphLayoutInfo.mRightToLeftLayout->mWordsLayoutInfo : paragraphLayoutInfo.mWordsLayoutInfo;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordsLayoutInfo.begin(),
           endWordLayoutIt = wordsLayoutInfo.end();
         wordLayoutIt != endWordLayoutIt;
         ++wordLayoutIt )
    {
      TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           characterLayoutIt != endCharacterLayoutIt;
           ++characterLayoutIt )
      {
        TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

        if( characterLayoutInfo.mIsVisible && characterLayoutInfo.mGlyphActor ) // White spaces and '\n' characters doesn't have a text-actor.
        {
          //Add to the text-view.
          textView.Add( characterLayoutInfo.mGlyphActor );
          relayoutData.mGlyphActors.push_back( characterLayoutInfo.mGlyphActor );
        }
      } // end character
    } // end words
  } // end paragraphs

  for( std::vector<RenderableActor>::iterator it = relayoutData.mEllipsizedGlyphActors.begin(),
         endIt = relayoutData.mEllipsizedGlyphActors.end();
       it != endIt;
       ++it )
  {
    RenderableActor glyphActor = ( *it );

    //Add to the text-view.
    textView.Add( glyphActor );
    relayoutData.mGlyphActors.push_back( glyphActor );
  }
  relayoutData.mEllipsizedGlyphActors.clear();
}

RenderableActor CreateGlyphActor( const Text& text, const TextStyle& style, TextActorCache& cache )
{
  TextActor textActor = cache.RetrieveTextActor();

  if( textActor )
  {
    // Update the text-actor.
    textActor.SetText( text );
    textActor.SetTextStyle( style );
  }
  else
  {
    // The text-actor cache is empty. Create a new one.
    TextActorParameters parameters( style, TextActorParameters::FONT_DETECTION_OFF );
    textActor = TextActor::New( text, parameters );
  }

  // Exclude from size negotiation
  textActor.SetRelayoutEnabled( false );

  return textActor;
}

} // namespace TextViewRelayout

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
