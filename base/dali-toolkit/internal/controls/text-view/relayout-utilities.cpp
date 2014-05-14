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

// FILE HEADER
#include "relayout-utilities.h"

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/text-view/text-view.h>
#include "text-view-line-processor.h"
#include "text-view-word-processor.h"
#include "text-view-processor-helper-functions.h"
#include "text-view-processor-dbg.h"

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
  mLineSize(),
  mWordSize(),
  mCharacterSize(),
  mIndices(),
  mCharacterGlobalIndex( 0u ),
  mIsFirstCharacter( false ),
  mIsFirstCharacterOfWord( false ),
  mIsNewLine( false ),
  mIsNewLineCharacter( false ),
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

SubLineLayoutInfo::SubLineLayoutInfo()
: mLineLength( 0.f ),
  mMaxCharHeight( 0.f ),
  mMaxAscender( 0.f )
{
}

SubLineLayoutInfo::~SubLineLayoutInfo()
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
void CalculateLineLength( const bool isWhiteSpace, const float width, const float parentWidth, bool& found, float& lineLength, float& endWhiteSpaceLength )
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
  Vector4 gradientColor;
  Vector2 startPoint;
  Vector2 endPoint;
};

void SetVisualParameters( CurrentTextActorInfo& currentTextActorInfo,
                          const TextView::VisualParameters& visualParameters,
                          TextView::RelayoutData& relayoutData,
                          const float lineHeight )
{
  currentTextActorInfo.textActor.SetTextColor( currentTextActorInfo.color );
  currentTextActorInfo.textActor.SetGradientColor( currentTextActorInfo.gradientColor );
  currentTextActorInfo.textActor.SetGradientStartPoint( currentTextActorInfo.startPoint );
  currentTextActorInfo.textActor.SetGradientEndPoint( currentTextActorInfo.endPoint );

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

void CalculateSubLineLayout( const float parentWidth,
                             const TextViewProcessor::TextInfoIndices& indices,
                             const TextViewProcessor::LineLayoutInfo& lineLayoutInfo,
                             const HorizontalWrapType splitPolicy,
                             const float shrinkFactor,
                             SubLineLayoutInfo& subLineInfo )
{
  subLineInfo.mLineLength = 0.f;
  subLineInfo.mMaxCharHeight = 0.f;
  subLineInfo.mMaxAscender = 0.f;

  float endWhiteSpaceLength = 0.f;

  std::size_t wordIndex = indices.mWordIndex;
  std::size_t characterIndex = indices.mCharacterIndex;
  float lineOffset = 0.f;
  bool found = false;
  bool isFirstCharacter = true;
  for( TextViewProcessor::WordGroupLayoutInfoContainer::const_iterator wordGroupIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin() + indices.mGroupIndex,
         wordGroupEndIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
       ( wordGroupIt != wordGroupEndIt ) && !found;
       ++wordGroupIt )
  {
    const TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *wordGroupIt );

    for( TextViewProcessor::WordLayoutInfoContainer::const_iterator wordIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin() + wordIndex,
           wordEndIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
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
        case WrapByLine: // Fall through
        {
          splitByCharacter = false;
          break;
        }
        case WrapByWordAndSplit:
        {
          splitByCharacter = ( shrunkWordWidth > parentWidth );
          break;
        }
        case WrapByLineAndSplit:
        {
          if( ( 0 != characterIndex ) ||
              ( ( 0 == characterIndex ) && ( lineOffset + shrunkWordWidth > parentWidth ) ) )
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
          characterIndex = 0;
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

    // All words for group 'groupIndex' have been processed.
    // Next group need to process all words, so the wordIndex is reset to 0.
    wordIndex = 0;
  }

  subLineInfo.mMaxCharHeight *= shrinkFactor;
  subLineInfo.mMaxAscender *= shrinkFactor;
}

float CalculateXoffset( const Toolkit::Alignment::Type horizontalTextAlignment, const float parentWidth, const float wholeTextWidth )
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

float CalculateYoffset( const Toolkit::Alignment::Type verticalTextAlignment, const float parentHeight, const float wholeTextHeight )
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

float CalculateJustificationOffset( const Toolkit::TextView::LineJustification justification, const float wholeTextWidth, const float lineLength )
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

bool IsVisible( const Vector3& position, const Size& size, const Size& parentSize, const VisibilityTestType type )
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

  std::size_t lineJustificationIndex = 0; // Index to the first position of the vector which stores all line justification info.
  std::size_t infoTableCharacterIndex = 0;

  relayoutParameters.mIndices.mLineIndex = 0;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
         endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt, ++relayoutParameters.mIndices.mLineIndex )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    relayoutParameters.mIndices.mGroupIndex = 0;
    float justificationOffset = 0.f;

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

        relayoutParameters.mIndices.mCharacterIndex = 0;

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
               endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
             characterLayoutIt != endCharacterLayoutIt;
             ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex, ++infoTableCharacterIndex )
        {
          TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

          // Calculate line justification offset.
          if( lineJustificationIndex < relayoutData.mLineJustificationInfo.size() )
          {
            const TextView::LineJustificationInfo lineJustificationInfo( *( relayoutData.mLineJustificationInfo.begin() + lineJustificationIndex ) );

            if( relayoutParameters.mIndices == lineJustificationInfo.mIndices )
            {
              justificationOffset = CalculateJustificationOffset( layoutParameters.mLineJustification, relayoutData.mTextSizeForRelayoutOption.width, lineJustificationInfo.mLineLength );
              ++lineJustificationIndex; // increase the index to point the next position in the vector.
            }
          }

          // Deletes the offsets if the exceed policies are EllipsizeEnd.
          const float horizontalOffset = textHorizontalOffset + justificationOffset;
          characterLayoutInfo.mOffset.x = ( ellipsizeAlignToLeft && ( horizontalOffset < 0.f ) ) ? 0.f : horizontalOffset;
          characterLayoutInfo.mOffset.y = ( ellipsizeAlignToTop && ( textVerticalOffset < 0.f ) ) ? 0.f : textVerticalOffset;

          // Updates the size and position table for text-input with the alignment offset.
          Vector3 positionOffset( characterLayoutInfo.mPosition );

          std::vector<Toolkit::TextView::CharacterLayoutInfo>::iterator infoTableIt = relayoutData.mCharacterLayoutInfoTable.begin() + infoTableCharacterIndex;
          Toolkit::TextView::CharacterLayoutInfo& characterTableInfo( *infoTableIt );

          characterTableInfo.mPosition.x = positionOffset.x + characterLayoutInfo.mOffset.x;
          characterTableInfo.mPosition.y = positionOffset.y + characterLayoutInfo.mOffset.y;

          positionOffset.x += characterLayoutInfo.mAdvance * relayoutData.mShrinkFactor;
        } // end characters
      } // end words
    } // end group of words
  } // end lines
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

  const Toolkit::TextView::LineLayoutInfo& lineInfo( *( relayoutData.mLines.end() - 1 ) );
  const float bearingOffset = ( lineInfo.mSize.height - lineInfo.mAscender ) - ( characterLayoutInfo.mSize.height - characterLayoutInfo.mAscender );

  characterLayoutInfo.mPosition.y -= bearingOffset * relayoutData.mShrinkFactor;
}

void UpdateLayoutInfoTable( Vector4& minMaxXY,
                            TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo,
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

  const Toolkit::TextView::CharacterLayoutInfo characterLayoutTableInfo( Size( characterLayoutInfo.mAdvance * relayoutData.mShrinkFactor,
                                                                               characterLayoutInfo.mHeight * relayoutData.mShrinkFactor ),
                                                                         positionOffset,
                                                                         ( TextViewProcessor::LineSeparator == wordLayoutInfo.mType ),
                                                                         ( TextViewProcessor::RTL == wordGroupLayoutInfo.mDirection ),
                                                                         true,
                                                                         descender );

  relayoutData.mCharacterLayoutInfoTable.push_back( characterLayoutTableInfo );

  positionOffset.x += characterLayoutInfo.mAdvance * relayoutData.mShrinkFactor;
}

void CalculateVisibilityForFade( const Internal::TextView::LayoutParameters& layoutParameters,
                                 TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
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
        gradientColor = characterLayoutInfo.mStyledText.mStyle.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha = gradientColor.a * std::min( 1.f, fadeParameters.mRightAlphaCoeficients.x * position.x + fadeParameters.mRightAlphaCoeficients.y );
        gradientColor.a *= std::max( 0.f, fadeParameters.mRightAlphaCoeficients.x * characterPositionPlusWidth + fadeParameters.mRightAlphaCoeficients.y );

        startPoint = Vector2( std::max( 0.f, ( fadeParameters.mRightFadeThresholdOffset - position.x ) / size.width ), 0.5f );
        endPoint = Vector2( std::min( 1.f, ( relayoutData.mTextViewSize.width - position.x ) / size.width ), 0.5f );
      }
      else if( leftFadeOut )
      {
        gradientColor = characterLayoutInfo.mStyledText.mStyle.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha = std::min( 1.f, fadeParameters.mLeftAlphaCoeficients.x * characterPositionPlusWidth + fadeParameters.mLeftAlphaCoeficients.y );
        gradientColor.a *= gradientColor.a * std::max( 0.f, fadeParameters.mLeftAlphaCoeficients.x * position.x + fadeParameters.mLeftAlphaCoeficients.y );

        startPoint = Vector2( std::max( 0.f, ( fadeParameters.mLeftFadeThresholdOffset - position.x ) / size.width ), 0.5f );
        endPoint = Vector2( std::min( 1.f, -position.x / size.width ), 0.5f );
      }
    }

    if( !( bottomFadeOut && topFadeOut ) )
    {
      // Current implementation can't set gradient parameters for a text-actor exceeding at the same time the top and the bottom boundaries.
      if( bottomFadeOut )
      {
        gradientColor = characterLayoutInfo.mStyledText.mStyle.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha = gradientColor.a * std::min( 1.f, fadeParameters.mBottomAlphaCoeficients.x * characterPositionMinusHeight + fadeParameters.mBottomAlphaCoeficients.y );
        gradientColor.a *= std::max( 0.f, fadeParameters.mBottomAlphaCoeficients.x * position.y + fadeParameters.mBottomAlphaCoeficients.y );

        startPoint = Vector2( 0.5f, std::max( 0.f, ( fadeParameters.mBottomFadeThresholdOffset - characterPositionMinusHeight ) / size.height ) );
        endPoint = Vector2( 0.5f, std::min( 1.f, ( relayoutData.mTextViewSize.height - characterPositionMinusHeight ) / size.height ) );
      }
      else if( topFadeOut )
      {
        gradientColor = characterLayoutInfo.mStyledText.mStyle.GetTextColor();

        // Calculates gradient coeficients.
        characterLayoutInfo.mColorAlpha *= gradientColor.a * std::min( 1.f, fadeParameters.mTopAlphaCoeficients.x * position.y + fadeParameters.mTopAlphaCoeficients.y );
        gradientColor.a *= std::max( 0.f, fadeParameters.mTopAlphaCoeficients.x * characterPositionMinusHeight + fadeParameters.mTopAlphaCoeficients.y );

        startPoint = Vector2( 0.5f, std::max( 0.f, ( fadeParameters.mTopFadeThresholdOffset - characterPositionMinusHeight ) / size.height ) );
        endPoint = Vector2( 0.5f, std::min( 1.f,  -characterPositionMinusHeight / size.height ) );
      }
    }

    characterLayoutInfo.mGradientColor = gradientColor;
    characterLayoutInfo.mStartPoint = startPoint;
    characterLayoutInfo.mEndPoint = endPoint;
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
  for( TextViewProcessor::CharacterLayoutInfoContainer::const_iterator ellipsizeCharacterLayoutIt = relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mCharactersLayoutInfo.begin(),
         endEllipsizeCharacterLayoutIt = relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mCharactersLayoutInfo.end();
       ellipsizeCharacterLayoutIt != endEllipsizeCharacterLayoutIt;
       ++ellipsizeCharacterLayoutIt )
  {
    const TextViewProcessor::CharacterLayoutInfo& ellipsizeCharacterLayoutInfo( *ellipsizeCharacterLayoutIt );

    if( isColorGlyph ||
        ( isColorGlyph != ellipsizeCharacterLayoutInfo.mIsColorGlyph ) ||
        ( ellipsizeStyle != ellipsizeCharacterLayoutInfo.mStyledText.mStyle ) )
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
      ellipsizeText = ellipsizeCharacterLayoutInfo.mStyledText.mText;
      ellipsizeStyle = ellipsizeCharacterLayoutInfo.mStyledText.mStyle;
      ellipsizeSize = ellipsizeCharacterLayoutInfo.mSize;
      isColorGlyph = ellipsizeCharacterLayoutInfo.mIsColorGlyph;

      bearingOffset = ( ellipsizeParameters.mLineDescender - ( ellipsizeCharacterLayoutInfo.mSize.height - ellipsizeCharacterLayoutInfo.mAscender ) ) * relayoutData.mShrinkFactor;
    }
    else
    {
      // Updates text and size with the new character.
      ellipsizeText.Append( ellipsizeCharacterLayoutInfo.mStyledText.mText );
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
  // Traverses the text layout info from the first character of the laid out line
  // to the last one setting to each character its visibility. If needed, it adds the ellipsize text (...).

  // Indices to the first character of the laid out line.
  TextViewProcessor::TextInfoIndices firstIndices;
  TextViewProcessor::GetIndicesFromGlobalCharacterIndex( ellipsizeParameters.mFirstIndex,
                                                         relayoutData.mTextLayoutInfo,
                                                         firstIndices );

  // Indices to the last character of the laid out line.
  TextViewProcessor::TextInfoIndices lastIndices;
  TextViewProcessor::GetIndicesFromGlobalCharacterIndex( ellipsizeParameters.mLastIndex,
                                                         relayoutData.mTextLayoutInfo,
                                                         lastIndices );

  // Defines a boundary by substracting the ellipsize-text's width to the text-view's width.
  // This is the boundary used to check if a character have to be ellipsized.
  ellipsizeParameters.mEllipsizeBoundary = relayoutData.mTextViewSize;
  ellipsizeParameters.mEllipsizeBoundary.width -= relayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo.mSize.width;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + firstIndices.mLineIndex,
         endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + lastIndices.mLineIndex + 1;
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    ellipsizeParameters.mLineFits = ellipsizeParameters.mIsLineWidthFullyVisible && ellipsizeParameters.mIsLineHeightFullyVisible && ellipsizeParameters.mIsNextLineFullyVisibleHeight;

    if( !ellipsizeParameters.mIsNextLineFullyVisibleHeight )
    {
      ellipsizeParameters.mEllipsizeBoundary.width = ellipsizeParameters.mLineWidth;
    }

    bool firstGroup = true;
    bool lastGroup = false;
    std::size_t groupCount = 0;

    bool firstWord = true;
    bool lastWord = false;

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin() + firstIndices.mGroupIndex,
           endGroupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin() + lastIndices.mGroupIndex + 1;
         groupLayoutIt != endGroupLayoutIt;
         ++groupLayoutIt, ++groupCount )
    {
      TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *groupLayoutIt );

      if( groupCount == lastIndices.mGroupIndex - firstIndices.mGroupIndex )
      {
        lastGroup = true;
      }

      std::size_t wordCount = 0;
      const std::size_t firstWordIndex = firstGroup ? firstIndices.mWordIndex : 0u;
      const std::size_t lastWordIndex = lastGroup ? lastIndices.mWordIndex : wordGroupLayoutInfo.mWordsLayoutInfo.size() - 1;

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin() + firstWordIndex,
             endWordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin() + lastWordIndex + 1;
           wordLayoutIt != endWordLayoutIt;
           ++wordLayoutIt, ++wordCount )
      {
        TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );

        if( lastGroup && ( wordCount == lastIndices.mWordIndex - firstWordIndex ) )
        {
          lastWord = true;
        }

        const std::size_t firstCharacterIndex = firstWord ? firstIndices.mCharacterIndex : 0u;
        const std::size_t lastCharacterIndex = lastWord ? lastIndices.mCharacterIndex : wordLayoutInfo.mCharactersLayoutInfo.size() - 1;
        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin() + firstCharacterIndex,
               endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin() + lastCharacterIndex + 1;
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
      firstGroup = false;
    } // end groups
  } // end lines
}

void SetTextVisible( TextView::RelayoutData& relayoutData )
{
  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
         endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin(),
           endGroupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
         groupLayoutIt != endGroupLayoutIt;
         ++groupLayoutIt )
    {
      TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *groupLayoutIt );

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin(),
             endWordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
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

          characterLayoutInfo.mIsVisible = true;
          characterLayoutInfo.mGradientColor = Vector4::ZERO;
          characterLayoutInfo.mStartPoint = Vector2::ZERO;
          characterLayoutInfo.mEndPoint = Vector2::ZERO;
          characterLayoutInfo.mColorAlpha = characterLayoutInfo.mStyledText.mStyle.GetTextColor().a;
        } // end characters
      } // end words
    } // end group of words
  } // end lines

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
  fadeParameters.mRightFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mRight > 0 ? fadeParameters.mRightFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mRightFadeThreshold = relayoutData.mTextViewSize.width - fadeParameters.mRightFadeBoundary;
  fadeParameters.mRightFadeThresholdOffset = relayoutData.mTextViewSize.width - fadeParameters.mRightFadeBoundaryOffset;
  fadeParameters.mLeftFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mLeft );
  fadeParameters.mLeftFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mLeft > 0 ? fadeParameters.mLeftFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mLeftFadeThreshold = fadeParameters.mLeftFadeBoundary;
  fadeParameters.mLeftFadeThresholdOffset = fadeParameters.mLeftFadeBoundaryOffset;
  fadeParameters.mTopFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mTop );
  fadeParameters.mTopFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mTop > 0 ? fadeParameters.mTopFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mTopFadeThreshold = fadeParameters.mTopFadeBoundary;
  fadeParameters.mTopFadeThresholdOffset = fadeParameters.mTopFadeBoundaryOffset;
  fadeParameters.mBottomFadeBoundary = static_cast<float>( visualParameters.mFadeBoundary.mBottom );
  fadeParameters.mBottomFadeBoundaryOffset = ( visualParameters.mFadeBoundary.mBottom > 0 ? fadeParameters.mBottomFadeBoundary : MINIMUM_FADE_BOUNDARY );
  fadeParameters.mBottomFadeThreshold = relayoutData.mTextViewSize.height - fadeParameters.mBottomFadeBoundary;
  fadeParameters.mBottomFadeThresholdOffset = relayoutData.mTextViewSize.height - fadeParameters.mBottomFadeBoundaryOffset;

  // Calculates the fade out rect coeficients for the right, left, top and bottom sides of the text-view.
  fadeParameters.mRightAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mRightFadeThresholdOffset, 1.f ), Vector2( relayoutData.mTextViewSize.width, 0.f ) );
  fadeParameters.mLeftAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mLeftFadeThresholdOffset, 1.f ), Vector2( 0.f, 0.f ) );
  fadeParameters.mTopAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mTopFadeThresholdOffset, 1.f ), Vector2( 0.f, 0.f ) );
  fadeParameters.mBottomAlphaCoeficients = CalculateRectParameters( Vector2( fadeParameters.mBottomFadeThresholdOffset, 1.f ), Vector2( relayoutData.mTextViewSize.height, 0.f ) );

  // Traverses all groups of characters and calculates the visibility.

  std::size_t infoTableCharacterIndex = 0;

  relayoutParameters.mIndices.mLineIndex = 0;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
         endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt, ++relayoutParameters.mIndices.mLineIndex )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

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

        relayoutParameters.mIsFirstCharacterOfWord = true;
        relayoutParameters.mWordSize = wordLayoutInfo.mSize;
        relayoutParameters.mIndices.mCharacterIndex = 0;

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
               endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
             characterLayoutIt != endCharacterLayoutIt;
             ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex, ++infoTableCharacterIndex )
        {
          TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

          relayoutParameters.mIsVisible = true;
          fadeParameters.mIsPartiallyVisible = false;

          // Calculates the visibility for the current group of characters.
          CalculateVisibilityForFade( layoutParameters,
                                      characterLayoutInfo,
                                      relayoutParameters,
                                      fadeParameters,
                                      relayoutData );

          // Updates the visibility for text-input..
          std::vector<Toolkit::TextView::CharacterLayoutInfo>::iterator it = relayoutData.mCharacterLayoutInfoTable.begin() + infoTableCharacterIndex;

          Toolkit::TextView::CharacterLayoutInfo& characterLayoutTableInfo( *it );

          characterLayoutTableInfo.mIsVisible = relayoutParameters.mIsVisible;

          relayoutParameters.mIsFirstCharacterOfWord = false;
        } // end group of character
      } // end words
    } // end group of words
  } // end lines
}

void UpdateVisibilityForEllipsize( const TextView::LayoutParameters& layoutParameters,
                                   const TextView::VisualParameters& visualParameters,
                                   TextView::RelayoutData& relayoutData )
{
  // Traverses the laid-out lines and checks which ones doesn't fit in the text-view's boundary.
  for( Toolkit::TextView::LineLayoutInfoContainer::const_iterator lineInfoIt = relayoutData.mLines.begin(), endLineInfoIt = relayoutData.mLines.end();
       lineInfoIt != endLineInfoIt;
       ++lineInfoIt )
  {
    const Toolkit::TextView::LineLayoutInfo& lineInfo( *lineInfoIt );

    // To check if a laid-out line fits in the text-view's boundary,
    // get the position of the first character is needed and do the test
    // with the laid-out line size.

    // An bearing offset may have been applied to the first character so it's needed to
    // get the start position of the line.

    // Some parameters used in the CalculateVisibilityForEllipsize() function.
    EllipsizeParameters ellipsizeParameters;

    // Retrieves the first index and the last index of the line.
    ellipsizeParameters.mFirstIndex = lineInfo.mCharacterGlobalIndex;
    ellipsizeParameters.mLastIndex = 0;
    if( ( lineInfoIt + 1 ) != endLineInfoIt )
    {
      const Toolkit::TextView::LineLayoutInfo& nextLineInfo( *( lineInfoIt + 1 ) );
      ellipsizeParameters.mLastIndex = nextLineInfo.mCharacterGlobalIndex - 1;
    }
    else
    {
      ellipsizeParameters.mLastIndex = relayoutData.mCharacterLayoutInfoTable.size() - 1;
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
        Toolkit::TextView::LineLayoutInfoContainer::const_iterator nextLineInfoIt = lineInfoIt + 1;
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

void UpdateTextActorInfo( const TextView::VisualParameters& visualParameters,
                          TextView::RelayoutData& relayoutData )
{
  CurrentTextActorInfo currentTextActorInfo;

  // Traverses the text-actor and layout info data structures.
  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
         endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin(),
           endGroupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
         groupLayoutIt != endGroupLayoutIt;
         ++groupLayoutIt )
    {
      TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *groupLayoutIt );

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin(),
             endWordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
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

          if( characterLayoutInfo.mIsColorGlyph )
          {
            ImageActor imageActor = ImageActor::DownCast( characterLayoutInfo.mGlyphActor );

            if( characterLayoutInfo.mSetText )
            {
              GlyphImage image = GlyphImage::New( characterLayoutInfo.mStyledText.mText[0] );

              if( image )
              {
                imageActor.SetImage( image );
              }
              characterLayoutInfo.mSetText = false;
            }

            imageActor.SetPosition( Vector3( characterLayoutInfo.mPosition.x + characterLayoutInfo.mOffset.x,
                                             characterLayoutInfo.mPosition.y + characterLayoutInfo.mOffset.y,
                                             characterLayoutInfo.mPosition.z ) );
            imageActor.SetSize( characterLayoutInfo.mSize );

            // Sets the sort modifier value.
            imageActor.SetSortModifier( visualParameters.mSortModifier );
          }
          else
          {
            TextActor textActor = TextActor::DownCast( characterLayoutInfo.mGlyphActor );
            if( textActor )
            {
              // There is a new text-actor. Set text and everything to the previous one.
              if( currentTextActorInfo.textActor )
              {
                currentTextActorInfo.textActor.SetText( currentTextActorInfo.text );
                currentTextActorInfo.textActor.SetPosition( currentTextActorInfo.position );
                currentTextActorInfo.textActor.SetSize( currentTextActorInfo.size );

                SetVisualParameters( currentTextActorInfo,
                                     visualParameters,
                                     relayoutData,
                                     lineLayoutInfo.mSize.height );
              }

              currentTextActorInfo.text = characterLayoutInfo.mStyledText.mText;
              currentTextActorInfo.position = Vector3( characterLayoutInfo.mPosition.x + characterLayoutInfo.mOffset.x,
                                                       characterLayoutInfo.mPosition.y + characterLayoutInfo.mOffset.y,
                                                       characterLayoutInfo.mPosition.z );
              currentTextActorInfo.size = characterLayoutInfo.mSize * relayoutData.mShrinkFactor;

              currentTextActorInfo.color = characterLayoutInfo.mStyledText.mStyle.GetTextColor();
              currentTextActorInfo.color.a = characterLayoutInfo.mColorAlpha;

              currentTextActorInfo.gradientColor = characterLayoutInfo.mGradientColor;
              currentTextActorInfo.startPoint = characterLayoutInfo.mStartPoint;
              currentTextActorInfo.endPoint = characterLayoutInfo.mEndPoint;

              // Update the current text-actor.
              currentTextActorInfo.textActor = textActor;
            }
            else
            {
              // If this character layout has no text-actor is because this character has the same style than previous one.
              // Add the character to the current text-actor and update the size.
              if( characterLayoutInfo.mIsVisible && ( TextViewProcessor::LineSeparator != wordLayoutInfo.mType ) )
              {
                currentTextActorInfo.text.Append( characterLayoutInfo.mStyledText.mText );

                currentTextActorInfo.position.y = std::min( currentTextActorInfo.position.y, ( characterLayoutInfo.mPosition.y + characterLayoutInfo.mOffset.y ) );
                currentTextActorInfo.size.width += characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor;
                currentTextActorInfo.size.height = std::max( currentTextActorInfo.size.height, characterLayoutInfo.mSize.height * relayoutData.mShrinkFactor );
              }
            }
          }
        } // end characters
      } // end words

      if( !currentTextActorInfo.text.IsEmpty() )
      {
        if( currentTextActorInfo.textActor )
        {
          currentTextActorInfo.textActor.SetText( currentTextActorInfo.text );
          currentTextActorInfo.textActor.SetPosition( currentTextActorInfo.position );
          currentTextActorInfo.textActor.SetSize( currentTextActorInfo.size );

          SetVisualParameters( currentTextActorInfo,
                               visualParameters,
                               relayoutData,
                               lineLayoutInfo.mSize.height );
        }
      }
    } //end groups of words
  } // end lines

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
  // Traverse the whole text to find all groups of consecutive underlined characters in the same laid-out line.
  //
  // Note that relayoutData.mTextLayoutInfo contains layout info per line but these lines are the result of split the whole text every time a '\n' is found.
  // According with the layout option, one of this lines could be laid-out in more than one.

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt )
  {
    TextViewProcessor::LineLayoutInfo& line( *lineIt );

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupIt = line.mWordGroupsLayoutInfo.begin(), groupEndIt = line.mWordGroupsLayoutInfo.end();
         groupIt != groupEndIt;
         ++groupIt )
    {
      TextViewProcessor::WordGroupLayoutInfo& group( *groupIt );

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = group.mWordsLayoutInfo.begin(), wordEndIt = group.mWordsLayoutInfo.end();
           wordIt != wordEndIt;
           ++wordIt )
      {
        TextViewProcessor::WordLayoutInfo& word( *wordIt );

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
             characterIt != characterEndIt;
             ++characterIt )
        {
          TextViewProcessor::CharacterLayoutInfo& characterGroup( *characterIt );

          // Check if current character is the first of a new laid-out line
          const bool isNewLine = ( textUnderlineStatus.mLineGlobalIndex < relayoutData.mLines.size() ) &&
                                 ( textUnderlineStatus.mCharacterGlobalIndex == ( *( relayoutData.mLines.begin() + textUnderlineStatus.mLineGlobalIndex ) ).mCharacterGlobalIndex );
          if( isNewLine )
          {
            ++textUnderlineStatus.mLineGlobalIndex; // If it's a new line, point to the next one.
          }

          if( characterGroup.mStyledText.mStyle.GetUnderline() )
          {
            if( !textUnderlineStatus.mCurrentUnderlineStatus || // Current character is underlined but previous one it wasn't.
                isNewLine )                                     // Current character is underlined and is the first of current laid-out line.
            {
              // Create a new underline info for the current underlined characters.
              UnderlineInfo underlineInfo;
              underlineInfo.mMaxHeight = characterGroup.mSize.height;
              underlineInfo.mMaxThickness = characterGroup.mUnderlineThickness;
              underlineInfo.mPosition = characterGroup.mUnderlinePosition;

              textUnderlineStatus.mUnderlineInfo.push_back( underlineInfo );

              textUnderlineStatus.mCurrentUnderlineStatus = true; // Set the current text is underlined.
            }
            else
            {
              // Retrieve last underline info and update it if current underline thickness is bigger.
              UnderlineInfo& underlineInfo( *( textUnderlineStatus.mUnderlineInfo.end() - 1 ) );

              underlineInfo.mMaxHeight = std::max( underlineInfo.mMaxHeight, characterGroup.mSize.height );

              if( characterGroup.mUnderlineThickness > underlineInfo.mMaxThickness )
              {
                underlineInfo.mMaxThickness = characterGroup.mUnderlineThickness;
                underlineInfo.mPosition = characterGroup.mUnderlinePosition;
              }
            }
          }
          else
          {
            textUnderlineStatus.mCurrentUnderlineStatus = false;
          }

          ++textUnderlineStatus.mCharacterGlobalIndex;
        } // end group of characters.
      } // end words.
    } // end group of words.
  } // end lines.
}

void SetUnderlineInfo( TextView::RelayoutData& relayoutData )
{
  // Stores for each group of consecutive underlined text in each laid-out line its maximum thicknes, its position of that thickness and the maximum character's height.
  TextViewRelayout::TextUnderlineStatus textUnderlineStatus;

  // Traverse the whole text to find all groups of consecutive underlined characters in the same laid-out line.
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
  textUnderlineStatus.mCharacterGlobalIndex = 0;
  textUnderlineStatus.mLineGlobalIndex = 0;

  float currentLineHeight = 0.f;
  float currentLineAscender = 0.f;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(), lineEndIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineIt != lineEndIt;
       ++lineIt )
  {
    TextViewProcessor::LineLayoutInfo& line( *lineIt );

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupIt = line.mWordGroupsLayoutInfo.begin(), groupEndIt = line.mWordGroupsLayoutInfo.end();
         groupIt != groupEndIt;
         ++groupIt )
    {
      TextViewProcessor::WordGroupLayoutInfo& group( *groupIt );

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordIt = group.mWordsLayoutInfo.begin(), wordEndIt = group.mWordsLayoutInfo.end();
           wordIt != wordEndIt;
           ++wordIt )
      {
        TextViewProcessor::WordLayoutInfo& word( *wordIt );

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterIt = word.mCharactersLayoutInfo.begin(), characterEndIt = word.mCharactersLayoutInfo.end();
             characterIt != characterEndIt;
             ++characterIt )
        {
          TextViewProcessor::CharacterLayoutInfo& characterGroup( *characterIt );

          // Check if current character is the first of a new laid-out line

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

          if( characterGroup.mStyledText.mStyle.GetUnderline() )
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

            // Sets the underline's thickness.
            characterGroup.mStyledText.mStyle.SetUnderlineThickness( underlineInfo.mMaxThickness );

            // Before setting the position it needs to be adjusted to match the base line.
            const float bearingOffset = ( currentLineHeight - currentLineAscender ) - ( characterGroup.mSize.height - characterGroup.mAscender );
            const float positionOffset = ( underlineInfo.mMaxHeight - characterGroup.mSize.height ) - bearingOffset;

            // Sets the underline's position.
            characterGroup.mStyledText.mStyle.SetUnderlinePosition( underlineInfo.mPosition - positionOffset );

            // Mark the group of characters to be set the new style into the text-actor.
            characterGroup.mSetStyle = true;
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
        } // end of group of characters.
      } // end of word.
    } // end of group of words.
  } // end of lines.
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

void InsertToTextView( const TextView::RelayoutOperationMask relayoutOperationMask,
                       Actor textView,
                       TextView::RelayoutData& relayoutData )
{
  const bool insertToTextView = relayoutOperationMask & TextView::RELAYOUT_INSERT_TO_TEXT_VIEW;
  const bool insertToTextActorList = relayoutOperationMask & TextView::RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST;

  // Add text-actors to the text-view.

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
         endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    for( TextViewProcessor::WordGroupLayoutInfoContainer::iterator groupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.begin(),
           endGroupLayoutIt = lineLayoutInfo.mWordGroupsLayoutInfo.end();
         groupLayoutIt != endGroupLayoutIt;
         ++groupLayoutIt )
    {
      TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo( *groupLayoutIt );

      for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.begin(),
             endWordLayoutIt = wordGroupLayoutInfo.mWordsLayoutInfo.end();
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
            if( insertToTextView )
            {
              textView.Add( characterLayoutInfo.mGlyphActor );
            }
            if( insertToTextActorList )
            {
              relayoutData.mGlyphActors.push_back( characterLayoutInfo.mGlyphActor );
            }
          }
        } // end group of character
      } // end words
    } // end group of words
  } // end lines

  for( std::vector<RenderableActor>::iterator it = relayoutData.mEllipsizedGlyphActors.begin(),
         endIt = relayoutData.mEllipsizedGlyphActors.end();
       it != endIt;
       ++it )
  {
    RenderableActor glyphActor = ( *it );

    //Add to the text-view.
    if( insertToTextView )
    {
      textView.Add( glyphActor );
    }
    if( insertToTextActorList )
    {
      relayoutData.mGlyphActors.push_back( glyphActor );
    }
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
    textActor = TextActor::New( text, style, false, true );
  }

  return textActor;
}

} // namespace TextViewRelayout

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
