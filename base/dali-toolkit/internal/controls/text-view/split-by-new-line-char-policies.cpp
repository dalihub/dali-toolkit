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

#include "split-by-new-line-char-policies.h"

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

namespace SplitByNewLineChar
{

namespace
{

Vector3 SplitPosition( const TextViewRelayout::RelayoutParameters& relayoutParameters,
                       const TextView::LayoutParameters& layoutParameters,
                       TextView::RelayoutData& relayoutData )
{
  const float wordOffset = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.x );
  const float previousPositionY = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.y );

  if( relayoutParameters.mIsNewLine ||
      relayoutParameters.mIsFirstCharacter ||
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
      const TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *( relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin() + relayoutParameters.mIndices.mLineIndex ) );

      TextViewRelayout::SubLineLayoutInfo subLineInfo;
      subLineInfo.mLineLength = 0.f;
      subLineInfo.mMaxCharHeight = 0.f;
      subLineInfo.mMaxAscender = 0.f;
      TextViewRelayout::CalculateSubLineLayout( relayoutData.mTextViewSize.width,
                                                relayoutParameters.mIndices,
                                                lineLayoutInfo,
                                                TextViewRelayout::WrapByLineAndSplit,
                                                1.f, // Shrink factor
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

void CalculateSizeAndPosition( const TextView::LayoutParameters& layoutParameters,
                               TextView::RelayoutData& relayoutData )
{
  // clear
  relayoutData.mCharacterLayoutInfoTable.clear();
  relayoutData.mLines.clear();
  relayoutData.mTextSizeForRelayoutOption = Size();
  relayoutData.mShrinkFactor = 1.f;

  // Calculates the text size for split by char.
  Vector4 minMaxXY( std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::min(),
                    std::numeric_limits<float>::min() );

  TextViewRelayout::RelayoutParameters relayoutParameters;

  if( TextView::ShrinkOriginal == layoutParameters.mExceedPolicy )
  {
    if( relayoutData.mTextLayoutInfo.mWholeTextSize.width > relayoutData.mTextViewSize.width )
    {
      relayoutData.mShrinkFactor = relayoutData.mTextViewSize.width / relayoutData.mTextLayoutInfo.mWholeTextSize.width;
    }
  }
  else if( TextView::Shrink == layoutParameters.mExceedPolicy )
  {
    if( ( relayoutData.mTextLayoutInfo.mWholeTextSize.width > relayoutData.mTextViewSize.width ) ||
        ( relayoutData.mTextLayoutInfo.mWholeTextSize.height > relayoutData.mTextViewSize.height ) )
    {
      relayoutData.mShrinkFactor = std::min( relayoutData.mTextViewSize.width / relayoutData.mTextLayoutInfo.mWholeTextSize.width,
                                             relayoutData.mTextViewSize.height / relayoutData.mTextLayoutInfo.mWholeTextSize.height );
    }
  }

  relayoutParameters.mIsFirstCharacter = true;
  relayoutParameters.mIndices.mLineIndex = 0;
  relayoutParameters.mPositionOffset = Vector3::ZERO;
  relayoutParameters.mCharacterGlobalIndex = 0;

  for( TextViewProcessor::LineLayoutInfoContainer::iterator lineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin(),
       endLineLayoutIt = relayoutData.mTextLayoutInfo.mLinesLayoutInfo.end();
       lineLayoutIt != endLineLayoutIt;
       ++lineLayoutIt, ++relayoutParameters.mIndices.mLineIndex )
  {
    TextViewProcessor::LineLayoutInfo& lineLayoutInfo( *lineLayoutIt );

    relayoutParameters.mLineSize = lineLayoutInfo.mSize * relayoutData.mShrinkFactor;

    relayoutParameters.mIsNewLine = true;
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

        if( relayoutParameters.mIsNewLine )
        {
          // Stores some info to calculate the line justification in a post-process.
          const bool isSplitOriginal = layoutParameters.mExceedPolicy == TextView::SplitOriginal;

          if( !isSplitOriginal )
          {
            TextView::LineJustificationInfo justificationInfo;

            justificationInfo.mIndices = relayoutParameters.mIndices;
            justificationInfo.mLineLength = relayoutParameters.mLineSize.width;

            relayoutData.mLineJustificationInfo.push_back( justificationInfo );
          }
        }

        for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
             characterLayoutIt != endCharacterLayoutIt;
             ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex )
        {
          TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );
          relayoutParameters.mCharacterSize = characterLayoutInfo.mSize;

          relayoutParameters.mCharacterSize = characterLayoutInfo.mSize;

          switch( layoutParameters.mExceedPolicy )
          {
            case TextView::OriginalShrink:
            case TextView::SplitShrink:
            case TextView::ShrinkFade: // Fall Through
            {
              DALI_LOG_WARNING( "SplitByNewLineChar::CalculateSizeAndPosition() policy not implemented.\n" );
              break;
            }
            case TextView::Original: // Fall Through
            case TextView::ShrinkOriginal: // Fall Through
            case TextView::Shrink: // Fall Through
            case TextView::OriginalFade: // Fall Through
            case TextView::FadeOriginal: // Fall Through
            case TextView::Fade: // Fall Through
            case TextView::EllipsizeEndOriginal: // Fall Through
            case TextView::EllipsizeEnd: // Fall Through
            {
              if( relayoutParameters.mIsNewLine )
              {
                relayoutParameters.mPositionOffset.x = 0.f;
                relayoutParameters.mPositionOffset.y += lineLayoutInfo.mSize.height * relayoutData.mShrinkFactor;
              }

              characterLayoutInfo.mPosition = relayoutParameters.mPositionOffset;

              relayoutParameters.mPositionOffset.x += characterLayoutInfo.mSize.width * relayoutData.mShrinkFactor;

              if( relayoutParameters.mIsNewLine ||
                  relayoutParameters.mIsFirstCharacter )
              {
                Toolkit::TextView::LineLayoutInfo lineInfo;
                lineInfo.mCharacterGlobalIndex = relayoutParameters.mCharacterGlobalIndex;  // Index to the first character of the next line.
                lineInfo.mSize = relayoutParameters.mLineSize;                              // Size of this piece of line.
                lineInfo.mAscender = lineLayoutInfo.mAscender * relayoutData.mShrinkFactor; // Ascender of this piece of line.
                relayoutData.mLines.push_back( lineInfo );
              }
              break;
            }
            case TextView::SplitOriginal:
            case TextView::SplitFade: // Fall Through
            {
              characterLayoutInfo.mPosition = SplitPosition( relayoutParameters,
                                                             layoutParameters,
                                                             relayoutData );

              relayoutParameters.mPositionOffset = characterLayoutInfo.mPosition + Vector3( characterLayoutInfo.mSize.width, 0.f, 0.f );
              break;
            }
            default:
            {
              DALI_LOG_WARNING( "SplitByNewLineChar::CalculateSizeAndPosition() Layout configuration not possible.\n" );
              break;
            }
          }

          // Get last line info and calculate the bearing (used to align glyphs with the baseline).
          TextViewRelayout::CalculateBearing( characterLayoutInfo, relayoutData );

          // updates min and max position to calculate the text size for split by new line char.
          TextViewRelayout::UpdateLayoutInfoTable( minMaxXY,
                                                   wordGroupLayoutInfo,
                                                   wordLayoutInfo,
                                                   characterLayoutInfo,
                                                   relayoutParameters,
                                                   relayoutData );

          ++relayoutParameters.mCharacterGlobalIndex;
          relayoutParameters.mIsFirstCharacter = false;
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

} // namespace SplitByNewLineChar

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
