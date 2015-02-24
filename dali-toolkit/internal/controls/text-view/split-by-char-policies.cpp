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
#include <dali-toolkit/internal/controls/text-view/split-by-char-policies.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/relayout-utilities.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace SplitByChar
{

namespace
{

Vector3 NoShrinkWhenExceedPosition( const TextViewRelayout::RelayoutParameters& relayoutParameters,
                                    const TextView::LayoutParameters& layoutParameters,
                                    TextView::RelayoutData& relayoutData )
{
  const float wordOffset = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.x );
  const float previousPositionY = ( relayoutParameters.mIsFirstCharacter ? 0.f : relayoutParameters.mPositionOffset.y );

  if( ( relayoutParameters.mIsNewLine ||
        relayoutParameters.mIsFirstCharacter ||
        ( wordOffset + relayoutParameters.mCharacterSize.width > relayoutData.mTextViewSize.width ) ) )
  {
    if( !relayoutParameters.mIsNewLine &&
        ( relayoutParameters.mIsWhiteSpace || relayoutParameters.mIsNewParagraphCharacter ) )
    {
      // Current character is a white space. Don't want to move a white space to the next line.
      // These white spaces are placed just in the edge.
      return Vector3( relayoutData.mTextViewSize.width - relayoutParameters.mWordSize.width, relayoutParameters.mPositionOffset.y, 0.f );
    }
    else
    {
      // Calculate the line length and the max character height for the current line.
      TextViewRelayout::LineLayoutInfo subLineInfo;
      subLineInfo.mLineLength = 0.f;
      subLineInfo.mMaxCharHeight = 0.f;
      subLineInfo.mMaxAscender = 0.f;
      const TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin() + relayoutParameters.mIndices.mParagraphIndex ) );

      TextViewRelayout::CalculateLineLayout( relayoutData.mTextViewSize.width,
                                             relayoutParameters.mIndices,
                                             paragraphLayoutInfo,
                                             TextViewRelayout::WrapByCharacter,
                                             1.f, // Shrink factor
                                             subLineInfo );

      Toolkit::TextView::LineLayoutInfo lineInfo;
      lineInfo.mCharacterGlobalIndex = relayoutParameters.mCharacterGlobalIndex;    // Index to the first character of the next line.
      lineInfo.mSize = Size( subLineInfo.mLineLength, subLineInfo.mMaxCharHeight ); // Size of this piece of paragraph.
      lineInfo.mAscender = subLineInfo.mMaxAscender;                                // Ascender of this piece of paragraph.
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
  TextViewRelayout::RelayoutParameters relayoutParameters;

  // clear
  relayoutData.mCharacterLayoutInfoTable.clear();
  relayoutData.mLines.clear();
  relayoutData.mTextSizeForRelayoutOption = Size();

  // Calculate the text size for split by char.
  Vector4 minMaxXY( std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::min(),
                    std::numeric_limits<float>::min() );

  relayoutData.mShrinkFactor = 1.f; // Shrink factor used when the exceed policy contains ShrinkToFit

  relayoutParameters.mPositionOffset = Vector3::ZERO;
  relayoutParameters.mIsFirstCharacter = true;
  relayoutParameters.mIndices.mParagraphIndex = 0u;
  relayoutParameters.mCharacterGlobalIndex = 0u;

  for( TextViewProcessor::ParagraphLayoutInfoContainer::iterator paragraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.begin(),
         endParagraphLayoutIt = relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end();
       paragraphLayoutIt != endParagraphLayoutIt;
       ++paragraphLayoutIt, ++relayoutParameters.mIndices.mParagraphIndex )
  {
    TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *paragraphLayoutIt );

    relayoutParameters.mIsNewLine = true;
    relayoutParameters.mParagraphSize = paragraphLayoutInfo.mSize;
    relayoutParameters.mIndices.mWordIndex = 0u;

    for( TextViewProcessor::WordLayoutInfoContainer::iterator wordLayoutIt = paragraphLayoutInfo.mWordsLayoutInfo.begin(),
           endWordLayoutIt = paragraphLayoutInfo.mWordsLayoutInfo.end();
         wordLayoutIt != endWordLayoutIt;
         ++wordLayoutIt, ++relayoutParameters.mIndices.mWordIndex )
    {
      TextViewProcessor::WordLayoutInfo& wordLayoutInfo( *wordLayoutIt );
      relayoutParameters.mIsWhiteSpace = TextViewProcessor::WordSeparator == wordLayoutInfo.mType;
      relayoutParameters.mIsNewParagraphCharacter = TextViewProcessor::ParagraphSeparator == wordLayoutInfo.mType;

      relayoutParameters.mIsFirstCharacterOfWord = true;
      relayoutParameters.mWordSize = wordLayoutInfo.mSize;
      relayoutParameters.mIndices.mCharacterIndex = 0u;

      for( TextViewProcessor::CharacterLayoutInfoContainer::iterator characterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.begin(),
             endCharacterLayoutIt = wordLayoutInfo.mCharactersLayoutInfo.end();
           characterLayoutIt != endCharacterLayoutIt;
           ++characterLayoutIt, ++relayoutParameters.mIndices.mCharacterIndex )
      {
        TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo( *characterLayoutIt );

        relayoutParameters.mCharacterSize = characterLayoutInfo.mSize;

        switch( layoutParameters.mExceedPolicy )
        {
          case TextView::OriginalShrink:
          case TextView::SplitOriginal:
          case TextView::SplitFade:
          case TextView::SplitEllipsizeEnd:
          case TextView::SplitShrink:
          case TextView::ShrinkOriginal:
          case TextView::ShrinkFade:
          case TextView::Shrink:
          case TextView::EllipsizeEndOriginal:
          case TextView::EllipsizeEnd: // Fall Through
          {
            DALI_LOG_WARNING( "SplitByChar::CalculateSizeAndPosition() policy not implemented.\n" );
            break;
          }
          case TextView::OriginalFade:
          case TextView::FadeOriginal:
          case TextView::Original:
          case TextView::Fade: // Fall Through
          {
            characterLayoutInfo.mPosition = NoShrinkWhenExceedPosition( relayoutParameters,
                                                                        layoutParameters,
                                                                        relayoutData );

            relayoutParameters.mPositionOffset = characterLayoutInfo.mPosition + Vector3( characterLayoutInfo.mSize.width, 0.f, 0.f );
            break;
          }
          default:
          {
            DALI_LOG_WARNING( "SplitByChar::CalculateSizeAndPosition() policy combination not possible.\n" );
          }
        }

        // Get last line info and calculate the bearing (used to align glyphs with the baseline).
        TextViewRelayout::CalculateBearing( characterLayoutInfo, relayoutData );

        // updates min and max position to calculate the text size for split by char.
        TextViewRelayout::UpdateLayoutInfoTable( minMaxXY,
                                                 wordLayoutInfo,
                                                 characterLayoutInfo,
                                                 relayoutParameters,
                                                 relayoutData );

        ++relayoutParameters.mCharacterGlobalIndex;
        relayoutParameters.mIsFirstCharacter = false;
        relayoutParameters.mIsNewLine = false;
      } // end characters
    } // end words
  } // end paragraphs

  if( relayoutData.mCharacterLayoutInfoTable.empty() )
  {
    relayoutData.mTextSizeForRelayoutOption = Size();
  }
  else
  {
    relayoutData.mTextSizeForRelayoutOption.width = minMaxXY.z - minMaxXY.x;
    relayoutData.mTextSizeForRelayoutOption.height = minMaxXY.w - minMaxXY.y;
  }

  // Check if the last character is a new paragraph character. In that case the height should be added.
  if( !relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraphLayoutInfo( *( relayoutData.mTextLayoutInfo.mParagraphsLayoutInfo.end() - 1u ) );

    if( paragraphLayoutInfo.mWordsLayoutInfo.empty() ) // if it's empty, it means the last character is a new paragraph character.
    {
      relayoutData.mTextSizeForRelayoutOption.height += paragraphLayoutInfo.mSize.height;
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
    CalculateSizeAndPosition( layoutParameters,
                              relayoutData );

    TextViewRelayout::ReorderRightToLeftLayout( relayoutData );

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

  const bool initializeTextActors = relayoutOperationMask & TextView::RELAYOUT_INITIALIZE_TEXT_ACTORS;
  const bool updateTextActors = relayoutOperationMask & TextView::RELAYOUT_TEXT_ACTOR_UPDATE;
  if( initializeTextActors || updateTextActors )
  {
    TextViewRelayout::UpdateTextActorInfo( visualParameters,
                                           relayoutData,
                                           initializeTextActors );
  }

  if( relayoutOperationMask & TextView::RELAYOUT_INSERT_TO_TEXT_VIEW )
  {
    TextViewRelayout::InsertToTextView( textView,
                                        relayoutData );
  }
}

} // namespace SplitByChar

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
