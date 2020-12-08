/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/text/text-controller-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/renderer.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/color-segmentation.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-controller-impl-event-handler.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit/internal/text/text-selection-handle-controller.h>

using namespace Dali;

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER_BACKGROUND = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute mediump vec4    aColor;
varying   mediump vec4    vColor;
uniform   highp mat4      uMvpMatrix;

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vColor = aColor;
}
);

const char* FRAGMENT_SHADER_BACKGROUND = MAKE_SHADER(
varying mediump vec4      vColor;
uniform lowp    vec4      uColor;

void main()
{
  gl_FragColor = vColor * uColor;
}
);

struct BackgroundVertex
{
  Vector2 mPosition;        ///< Vertex posiiton
  Vector4 mColor;           ///< Vertex color
};

struct BackgroundMesh
{
  Vector< BackgroundVertex > mVertices;    ///< container of vertices
  Vector< unsigned short > mIndices;       ///< container of indices
};

const Dali::Vector4 LIGHT_BLUE( 0.75f, 0.96f, 1.f, 1.f );
const Dali::Vector4 BACKGROUND_SUB4( 0.58f, 0.87f, 0.96f, 1.f );
const Dali::Vector4 BACKGROUND_SUB5( 0.83f, 0.94f, 0.98f, 1.f );
const Dali::Vector4 BACKGROUND_SUB6( 1.f, 0.5f, 0.5f, 1.f );
const Dali::Vector4 BACKGROUND_SUB7( 1.f, 0.8f, 0.8f, 1.f );

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

EventData::EventData( DecoratorPtr decorator, InputMethodContext& inputMethodContext )
: mDecorator( decorator ),
  mInputMethodContext( inputMethodContext ),
  mPlaceholderFont( NULL ),
  mPlaceholderTextActive(),
  mPlaceholderTextInactive(),
  mPlaceholderTextColor( 0.8f, 0.8f, 0.8f, 0.8f ), // This color has been published in the Public API (placeholder-properties.h).
  mEventQueue(),
  mInputStyleChangedQueue(),
  mPreviousState( INACTIVE ),
  mState( INACTIVE ),
  mPrimaryCursorPosition( 0u ),
  mLeftSelectionPosition( 0u ),
  mRightSelectionPosition( 0u ),
  mPreEditStartPosition( 0u ),
  mPreEditLength( 0u ),
  mCursorHookPositionX( 0.f ),
  mDoubleTapAction( Controller::NoTextTap::NO_ACTION ),
  mLongPressAction( Controller::NoTextTap::SHOW_SELECTION_POPUP ),
  mIsShowingPlaceholderText( false ),
  mPreEditFlag( false ),
  mDecoratorUpdated( false ),
  mCursorBlinkEnabled( true ),
  mGrabHandleEnabled( true ),
  mGrabHandlePopupEnabled( true ),
  mSelectionEnabled( true ),
  mUpdateCursorHookPosition( false ),
  mUpdateCursorPosition( false ),
  mUpdateGrabHandlePosition( false ),
  mUpdateLeftSelectionPosition( false ),
  mUpdateRightSelectionPosition( false ),
  mIsLeftHandleSelected( false ),
  mIsRightHandleSelected( false ),
  mUpdateHighlightBox( false ),
  mScrollAfterUpdatePosition( false ),
  mScrollAfterDelete( false ),
  mAllTextSelected( false ),
  mUpdateInputStyle( false ),
  mPasswordInput( false ),
  mCheckScrollAmount( false ),
  mIsPlaceholderPixelSize( false ),
  mIsPlaceholderElideEnabled( false ),
  mPlaceholderEllipsisFlag( false ),
  mShiftSelectionFlag( true ),
  mUpdateAlignment( false ),
  mEditingEnabled( true )
{
}

bool Controller::Impl::ProcessInputEvents()
{
  return ControllerImplEventHandler::ProcessInputEvents(*this);
}

void Controller::Impl::NotifyInputMethodContext()
{
  if( mEventData && mEventData->mInputMethodContext )
  {
    CharacterIndex cursorPosition = GetLogicalCursorPosition();

    const Length numberOfWhiteSpaces = GetNumberOfWhiteSpaces( 0u );

    // Update the cursor position by removing the initial white spaces.
    if( cursorPosition < numberOfWhiteSpaces )
    {
      cursorPosition = 0u;
    }
    else
    {
      cursorPosition -= numberOfWhiteSpaces;
    }

    mEventData->mInputMethodContext.SetCursorPosition( cursorPosition );
    mEventData->mInputMethodContext.NotifyCursorPosition();
  }
}

void Controller::Impl::NotifyInputMethodContextMultiLineStatus()
{
  if ( mEventData && mEventData->mInputMethodContext )
  {
    Text::Layout::Engine::Type layout = mLayoutEngine.GetLayout();
    mEventData->mInputMethodContext.NotifyTextInputMultiLine( layout == Text::Layout::Engine::MULTI_LINE_BOX );
  }
}

CharacterIndex Controller::Impl::GetLogicalCursorPosition() const
{
  CharacterIndex cursorPosition = 0u;

  if( mEventData )
  {
    if( ( EventData::SELECTING == mEventData->mState ) ||
        ( EventData::SELECTION_HANDLE_PANNING == mEventData->mState ) )
    {
      cursorPosition = std::min( mEventData->mRightSelectionPosition, mEventData->mLeftSelectionPosition );
    }
    else
    {
      cursorPosition = mEventData->mPrimaryCursorPosition;
    }
  }

  return cursorPosition;
}

Length Controller::Impl::GetNumberOfWhiteSpaces( CharacterIndex index ) const
{
  Length numberOfWhiteSpaces = 0u;

  // Get the buffer to the text.
  Character* utf32CharacterBuffer = mModel->mLogicalModel->mText.Begin();

  const Length totalNumberOfCharacters = mModel->mLogicalModel->mText.Count();
  for( ; index < totalNumberOfCharacters; ++index, ++numberOfWhiteSpaces )
  {
    if( !TextAbstraction::IsWhiteSpace( *( utf32CharacterBuffer + index ) ) )
    {
      break;
    }
  }

  return numberOfWhiteSpaces;
}

void Controller::Impl::GetText( CharacterIndex index, std::string& text ) const
{
  // Get the total number of characters.
  Length numberOfCharacters = mModel->mLogicalModel->mText.Count();

  // Retrieve the text.
  if( 0u != numberOfCharacters )
  {
    Utf32ToUtf8( mModel->mLogicalModel->mText.Begin() + index, numberOfCharacters - index, text );
  }
}

void Controller::Impl::CalculateTextUpdateIndices( Length& numberOfCharacters )
{
  mTextUpdateInfo.mParagraphCharacterIndex = 0u;
  mTextUpdateInfo.mStartGlyphIndex = 0u;
  mTextUpdateInfo.mStartLineIndex = 0u;
  numberOfCharacters = 0u;

  const Length numberOfParagraphs = mModel->mLogicalModel->mParagraphInfo.Count();
  if( 0u == numberOfParagraphs )
  {
    mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    numberOfCharacters = 0u;

    mTextUpdateInfo.mRequestedNumberOfCharacters = mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

    // Nothing else to do if there are no paragraphs.
    return;
  }

  // Find the paragraphs to be updated.
  Vector<ParagraphRunIndex> paragraphsToBeUpdated;
  if( mTextUpdateInfo.mCharacterIndex >= mTextUpdateInfo.mPreviousNumberOfCharacters )
  {
    // Text is being added at the end of the current text.
    if( mTextUpdateInfo.mIsLastCharacterNewParagraph )
    {
      // Text is being added in a new paragraph after the last character of the text.
      mTextUpdateInfo.mParagraphCharacterIndex = mTextUpdateInfo.mPreviousNumberOfCharacters;
      numberOfCharacters = 0u;
      mTextUpdateInfo.mRequestedNumberOfCharacters = mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

      mTextUpdateInfo.mStartGlyphIndex = mModel->mVisualModel->mGlyphs.Count();
      mTextUpdateInfo.mStartLineIndex = mModel->mVisualModel->mLines.Count() - 1u;

      // Nothing else to do;
      return;
    }

    paragraphsToBeUpdated.PushBack( numberOfParagraphs - 1u );
  }
  else
  {
    Length numberOfCharactersToUpdate = 0u;
    if( mTextUpdateInfo.mFullRelayoutNeeded )
    {
      numberOfCharactersToUpdate = mTextUpdateInfo.mPreviousNumberOfCharacters;
    }
    else
    {
      numberOfCharactersToUpdate = ( mTextUpdateInfo.mNumberOfCharactersToRemove > 0u ) ? mTextUpdateInfo.mNumberOfCharactersToRemove : 1u;
    }
    mModel->mLogicalModel->FindParagraphs( mTextUpdateInfo.mCharacterIndex,
                                           numberOfCharactersToUpdate,
                                           paragraphsToBeUpdated );
  }

  if( 0u != paragraphsToBeUpdated.Count() )
  {
    const ParagraphRunIndex firstParagraphIndex = *( paragraphsToBeUpdated.Begin() );
    const ParagraphRun& firstParagraph = *( mModel->mLogicalModel->mParagraphInfo.Begin() + firstParagraphIndex );
    mTextUpdateInfo.mParagraphCharacterIndex = firstParagraph.characterRun.characterIndex;

    ParagraphRunIndex lastParagraphIndex = *( paragraphsToBeUpdated.End() - 1u );
    const ParagraphRun& lastParagraph = *( mModel->mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex );

    if( ( mTextUpdateInfo.mNumberOfCharactersToRemove > 0u ) &&                                            // Some character are removed.
        ( lastParagraphIndex < numberOfParagraphs - 1u ) &&                                                // There is a next paragraph.
        ( ( lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters ) == // The last removed character is the new paragraph character.
          ( mTextUpdateInfo.mCharacterIndex + mTextUpdateInfo.mNumberOfCharactersToRemove ) ) )
    {
      // The new paragraph character of the last updated paragraph has been removed so is going to be merged with the next one.
      const ParagraphRun& lastParagraph = *( mModel->mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex + 1u );

      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters - mTextUpdateInfo.mParagraphCharacterIndex;
    }
    else
    {
      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters - mTextUpdateInfo.mParagraphCharacterIndex;
    }
  }

  mTextUpdateInfo.mRequestedNumberOfCharacters = numberOfCharacters + mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;
  mTextUpdateInfo.mStartGlyphIndex = *( mModel->mVisualModel->mCharactersToGlyph.Begin() + mTextUpdateInfo.mParagraphCharacterIndex );
}

void Controller::Impl::ClearFullModelData( OperationsMask operations )
{
  if( NO_OPERATION != ( GET_LINE_BREAKS & operations ) )
  {
    mModel->mLogicalModel->mLineBreakInfo.Clear();
    mModel->mLogicalModel->mParagraphInfo.Clear();
  }

  if( NO_OPERATION != ( GET_SCRIPTS & operations ) )
  {
    mModel->mLogicalModel->mScriptRuns.Clear();
  }

  if( NO_OPERATION != ( VALIDATE_FONTS & operations ) )
  {
    mModel->mLogicalModel->mFontRuns.Clear();
  }

  if( 0u != mModel->mLogicalModel->mBidirectionalParagraphInfo.Count() )
  {
    if( NO_OPERATION != ( BIDI_INFO & operations ) )
    {
      mModel->mLogicalModel->mBidirectionalParagraphInfo.Clear();
      mModel->mLogicalModel->mCharacterDirections.Clear();
    }

    if( NO_OPERATION != ( REORDER & operations ) )
    {
      // Free the allocated memory used to store the conversion table in the bidirectional line info run.
      for( Vector<BidirectionalLineInfoRun>::Iterator it = mModel->mLogicalModel->mBidirectionalLineInfo.Begin(),
             endIt = mModel->mLogicalModel->mBidirectionalLineInfo.End();
           it != endIt;
           ++it )
      {
        BidirectionalLineInfoRun& bidiLineInfo = *it;

        free( bidiLineInfo.visualToLogicalMap );
        bidiLineInfo.visualToLogicalMap = NULL;
      }
      mModel->mLogicalModel->mBidirectionalLineInfo.Clear();
    }
  }

  if( NO_OPERATION != ( SHAPE_TEXT & operations ) )
  {
    mModel->mVisualModel->mGlyphs.Clear();
    mModel->mVisualModel->mGlyphsToCharacters.Clear();
    mModel->mVisualModel->mCharactersToGlyph.Clear();
    mModel->mVisualModel->mCharactersPerGlyph.Clear();
    mModel->mVisualModel->mGlyphsPerCharacter.Clear();
    mModel->mVisualModel->mGlyphPositions.Clear();
  }

  if( NO_OPERATION != ( LAYOUT & operations ) )
  {
    mModel->mVisualModel->mLines.Clear();
  }

  if( NO_OPERATION != ( COLOR & operations ) )
  {
    mModel->mVisualModel->mColorIndices.Clear();
    mModel->mVisualModel->mBackgroundColorIndices.Clear();
  }
}

void Controller::Impl::ClearCharacterModelData( CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations )
{
  const CharacterIndex endIndexPlusOne = endIndex + 1u;

  if( NO_OPERATION != ( GET_LINE_BREAKS & operations ) )
  {
    // Clear the line break info.
    LineBreakInfo* lineBreakInfoBuffer = mModel->mLogicalModel->mLineBreakInfo.Begin();

    mModel->mLogicalModel->mLineBreakInfo.Erase( lineBreakInfoBuffer + startIndex,
                                                 lineBreakInfoBuffer + endIndexPlusOne );

    // Clear the paragraphs.
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mModel->mLogicalModel->mParagraphInfo );
  }

  if( NO_OPERATION != ( GET_SCRIPTS & operations ) )
  {
    // Clear the scripts.
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mModel->mLogicalModel->mScriptRuns );
  }

  if( NO_OPERATION != ( VALIDATE_FONTS & operations ) )
  {
    // Clear the fonts.
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mModel->mLogicalModel->mFontRuns );
  }

  if( 0u != mModel->mLogicalModel->mBidirectionalParagraphInfo.Count() )
  {
    if( NO_OPERATION != ( BIDI_INFO & operations ) )
    {
      // Clear the bidirectional paragraph info.
      ClearCharacterRuns( startIndex,
                          endIndex,
                          mModel->mLogicalModel->mBidirectionalParagraphInfo );

      // Clear the character's directions.
      CharacterDirection* characterDirectionsBuffer = mModel->mLogicalModel->mCharacterDirections.Begin();

      mModel->mLogicalModel->mCharacterDirections.Erase( characterDirectionsBuffer + startIndex,
                                                         characterDirectionsBuffer + endIndexPlusOne );
    }

    if( NO_OPERATION != ( REORDER & operations ) )
    {
      uint32_t startRemoveIndex = mModel->mLogicalModel->mBidirectionalLineInfo.Count();
      uint32_t endRemoveIndex = startRemoveIndex;
      ClearCharacterRuns( startIndex,
                          endIndex,
                          mModel->mLogicalModel->mBidirectionalLineInfo,
                          startRemoveIndex,
                          endRemoveIndex );

      BidirectionalLineInfoRun* bidirectionalLineInfoBuffer = mModel->mLogicalModel->mBidirectionalLineInfo.Begin();

      // Free the allocated memory used to store the conversion table in the bidirectional line info run.
      for( Vector<BidirectionalLineInfoRun>::Iterator it = bidirectionalLineInfoBuffer + startRemoveIndex,
             endIt = bidirectionalLineInfoBuffer + endRemoveIndex;
           it != endIt;
           ++it )
      {
        BidirectionalLineInfoRun& bidiLineInfo = *it;

        free( bidiLineInfo.visualToLogicalMap );
        bidiLineInfo.visualToLogicalMap = NULL;
      }

      mModel->mLogicalModel->mBidirectionalLineInfo.Erase( bidirectionalLineInfoBuffer + startRemoveIndex,
                                                           bidirectionalLineInfoBuffer + endRemoveIndex );
    }
  }
}

void Controller::Impl::ClearGlyphModelData( CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations )
{
  const CharacterIndex endIndexPlusOne = endIndex + 1u;
  const Length numberOfCharactersRemoved = endIndexPlusOne - startIndex;

  // Convert the character index to glyph index before deleting the character to glyph and the glyphs per character buffers.
  GlyphIndex* charactersToGlyphBuffer = mModel->mVisualModel->mCharactersToGlyph.Begin();
  Length* glyphsPerCharacterBuffer = mModel->mVisualModel->mGlyphsPerCharacter.Begin();

  const GlyphIndex endGlyphIndexPlusOne = *( charactersToGlyphBuffer + endIndex ) + *( glyphsPerCharacterBuffer + endIndex );
  const Length numberOfGlyphsRemoved = endGlyphIndexPlusOne - mTextUpdateInfo.mStartGlyphIndex;

  if( NO_OPERATION != ( SHAPE_TEXT & operations ) )
  {
    // Update the character to glyph indices.
    for( Vector<GlyphIndex>::Iterator it =  charactersToGlyphBuffer + endIndexPlusOne,
           endIt =  charactersToGlyphBuffer + mModel->mVisualModel->mCharactersToGlyph.Count();
         it != endIt;
         ++it )
    {
      CharacterIndex& index = *it;
      index -= numberOfGlyphsRemoved;
    }

    // Clear the character to glyph conversion table.
    mModel->mVisualModel->mCharactersToGlyph.Erase( charactersToGlyphBuffer + startIndex,
                                                    charactersToGlyphBuffer + endIndexPlusOne );

    // Clear the glyphs per character table.
    mModel->mVisualModel->mGlyphsPerCharacter.Erase( glyphsPerCharacterBuffer + startIndex,
                                                     glyphsPerCharacterBuffer + endIndexPlusOne );

    // Clear the glyphs buffer.
    GlyphInfo* glyphsBuffer = mModel->mVisualModel->mGlyphs.Begin();
    mModel->mVisualModel->mGlyphs.Erase( glyphsBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                         glyphsBuffer + endGlyphIndexPlusOne );

    CharacterIndex* glyphsToCharactersBuffer = mModel->mVisualModel->mGlyphsToCharacters.Begin();

    // Update the glyph to character indices.
    for( Vector<CharacterIndex>::Iterator it = glyphsToCharactersBuffer + endGlyphIndexPlusOne,
           endIt = glyphsToCharactersBuffer + mModel->mVisualModel->mGlyphsToCharacters.Count();
         it != endIt;
         ++it )
    {
      CharacterIndex& index = *it;
      index -= numberOfCharactersRemoved;
    }

    // Clear the glyphs to characters buffer.
    mModel->mVisualModel->mGlyphsToCharacters.Erase( glyphsToCharactersBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                                     glyphsToCharactersBuffer  + endGlyphIndexPlusOne );

    // Clear the characters per glyph buffer.
    Length* charactersPerGlyphBuffer = mModel->mVisualModel->mCharactersPerGlyph.Begin();
    mModel->mVisualModel->mCharactersPerGlyph.Erase( charactersPerGlyphBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                                     charactersPerGlyphBuffer + endGlyphIndexPlusOne );

    // Clear the positions buffer.
    Vector2* positionsBuffer = mModel->mVisualModel->mGlyphPositions.Begin();
    mModel->mVisualModel->mGlyphPositions.Erase( positionsBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                                 positionsBuffer + endGlyphIndexPlusOne );
  }

  if( NO_OPERATION != ( LAYOUT & operations ) )
  {
    // Clear the lines.
    uint32_t startRemoveIndex = mModel->mVisualModel->mLines.Count();
    uint32_t endRemoveIndex = startRemoveIndex;
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mModel->mVisualModel->mLines,
                        startRemoveIndex,
                        endRemoveIndex );

    // Will update the glyph runs.
    startRemoveIndex = mModel->mVisualModel->mLines.Count();
    endRemoveIndex = startRemoveIndex;
    ClearGlyphRuns( mTextUpdateInfo.mStartGlyphIndex,
                    endGlyphIndexPlusOne - 1u,
                    mModel->mVisualModel->mLines,
                    startRemoveIndex,
                    endRemoveIndex );

    // Set the line index from where to insert the new laid-out lines.
    mTextUpdateInfo.mStartLineIndex = startRemoveIndex;

    LineRun* linesBuffer = mModel->mVisualModel->mLines.Begin();
    mModel->mVisualModel->mLines.Erase( linesBuffer + startRemoveIndex,
                                        linesBuffer + endRemoveIndex );
  }

  if( NO_OPERATION != ( COLOR & operations ) )
  {
    if( 0u != mModel->mVisualModel->mColorIndices.Count() )
    {
      ColorIndex* colorIndexBuffer = mModel->mVisualModel->mColorIndices.Begin();
      mModel->mVisualModel->mColorIndices.Erase( colorIndexBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                                 colorIndexBuffer + endGlyphIndexPlusOne );
    }

    if( 0u != mModel->mVisualModel->mBackgroundColorIndices.Count() )
    {
      ColorIndex* backgroundColorIndexBuffer = mModel->mVisualModel->mBackgroundColorIndices.Begin();
      mModel->mVisualModel->mBackgroundColorIndices.Erase( backgroundColorIndexBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                                           backgroundColorIndexBuffer + endGlyphIndexPlusOne );
    }
  }
}

void Controller::Impl::ClearModelData( CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations )
{
  if( mTextUpdateInfo.mClearAll ||
      ( ( 0u == startIndex ) &&
        ( mTextUpdateInfo.mPreviousNumberOfCharacters == endIndex + 1u ) ) )
  {
    ClearFullModelData( operations );
  }
  else
  {
    // Clear the model data related with characters.
    ClearCharacterModelData( startIndex, endIndex, operations );

    // Clear the model data related with glyphs.
    ClearGlyphModelData( startIndex, endIndex, operations );
  }

  // The estimated number of lines. Used to avoid reallocations when layouting.
  mTextUpdateInfo.mEstimatedNumberOfLines = std::max( mModel->mVisualModel->mLines.Count(), mModel->mLogicalModel->mParagraphInfo.Count() );

  mModel->mVisualModel->ClearCaches();
}

bool Controller::Impl::UpdateModel( OperationsMask operationsRequired )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::UpdateModel\n" );

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mOperationsPending & operationsRequired );

  if( NO_OPERATION == operations )
  {
    // Nothing to do if no operations are pending and required.
    return false;
  }

  Vector<Character>& srcCharacters = mModel->mLogicalModel->mText;
  Vector<Character> displayCharacters;
  bool useHiddenText = false;
  if ( mHiddenInput && mEventData != NULL && !mEventData->mIsShowingPlaceholderText)
  {
    mHiddenInput->Substitute( srcCharacters,displayCharacters );
    useHiddenText = true;
  }

  Vector<Character>& utf32Characters = useHiddenText ? displayCharacters : srcCharacters;
  const Length numberOfCharacters = utf32Characters.Count();

  // Index to the first character of the first paragraph to be updated.
  CharacterIndex startIndex = 0u;
  // Number of characters of the paragraphs to be removed.
  Length paragraphCharacters = 0u;

  CalculateTextUpdateIndices( paragraphCharacters );

  // Check whether the indices for updating the text is valid
  if ( numberOfCharacters > 0u &&
       ( mTextUpdateInfo.mParagraphCharacterIndex > numberOfCharacters ||
         mTextUpdateInfo.mRequestedNumberOfCharacters > numberOfCharacters ) )
  {
    std::string currentText;
    Utf32ToUtf8( mModel->mLogicalModel->mText.Begin(), numberOfCharacters, currentText );

    DALI_LOG_ERROR( "Controller::Impl::UpdateModel: mTextUpdateInfo has invalid indices\n" );
    DALI_LOG_ERROR( "Number of characters: %d, current text is: %s\n", numberOfCharacters, currentText.c_str() );

    // Dump mTextUpdateInfo
    DALI_LOG_ERROR( "Dump mTextUpdateInfo:\n" );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mCharacterIndex = %u\n", mTextUpdateInfo.mCharacterIndex );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mNumberOfCharactersToRemove = %u\n", mTextUpdateInfo.mNumberOfCharactersToRemove );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mNumberOfCharactersToAdd = %u\n", mTextUpdateInfo.mNumberOfCharactersToAdd );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mPreviousNumberOfCharacters = %u\n", mTextUpdateInfo.mPreviousNumberOfCharacters );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mParagraphCharacterIndex = %u\n", mTextUpdateInfo.mParagraphCharacterIndex );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mRequestedNumberOfCharacters = %u\n", mTextUpdateInfo.mRequestedNumberOfCharacters );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mStartGlyphIndex = %u\n", mTextUpdateInfo.mStartGlyphIndex );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mStartLineIndex = %u\n", mTextUpdateInfo.mStartLineIndex );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mEstimatedNumberOfLines = %u\n", mTextUpdateInfo.mEstimatedNumberOfLines );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mClearAll = %d\n", mTextUpdateInfo.mClearAll );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mFullRelayoutNeeded = %d\n", mTextUpdateInfo.mFullRelayoutNeeded );
    DALI_LOG_ERROR( "     mTextUpdateInfo.mIsLastCharacterNewParagraph = %d\n", mTextUpdateInfo.mIsLastCharacterNewParagraph );

    return false;
  }

  startIndex = mTextUpdateInfo.mParagraphCharacterIndex;

  if( mTextUpdateInfo.mClearAll ||
      ( 0u != paragraphCharacters ) )
  {
    ClearModelData( startIndex, startIndex + ( ( paragraphCharacters > 0u ) ? paragraphCharacters - 1u : 0u ), operations );
  }

  mTextUpdateInfo.mClearAll = false;

  // Whether the model is updated.
  bool updated = false;

  Vector<LineBreakInfo>& lineBreakInfo = mModel->mLogicalModel->mLineBreakInfo;
  const Length requestedNumberOfCharacters = mTextUpdateInfo.mRequestedNumberOfCharacters;

  if( NO_OPERATION != ( GET_LINE_BREAKS & operations ) )
  {
    // Retrieves the line break info. The line break info is used to split the text in 'paragraphs' to
    // calculate the bidirectional info for each 'paragraph'.
    // It's also used to layout the text (where it should be a new line) or to shape the text (text in different lines
    // is not shaped together).
    lineBreakInfo.Resize( numberOfCharacters, TextAbstraction::LINE_NO_BREAK );

    SetLineBreakInfo( utf32Characters,
                      startIndex,
                      requestedNumberOfCharacters,
                      lineBreakInfo );

    // Create the paragraph info.
    mModel->mLogicalModel->CreateParagraphInfo( startIndex,
                                                requestedNumberOfCharacters );
    updated = true;
  }

  const bool getScripts = NO_OPERATION != ( GET_SCRIPTS & operations );
  const bool validateFonts = NO_OPERATION != ( VALIDATE_FONTS & operations );

  Vector<ScriptRun>& scripts = mModel->mLogicalModel->mScriptRuns;
  Vector<FontRun>& validFonts = mModel->mLogicalModel->mFontRuns;

  if( getScripts || validateFonts )
  {
    // Validates the fonts assigned by the application or assigns default ones.
    // It makes sure all the characters are going to be rendered by the correct font.
    MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

    if( getScripts )
    {
      // Retrieves the scripts used in the text.
      multilanguageSupport.SetScripts( utf32Characters,
                                       startIndex,
                                       requestedNumberOfCharacters,
                                       scripts );
    }

    if( validateFonts )
    {
      // Validate the fonts set through the mark-up string.
      Vector<FontDescriptionRun>& fontDescriptionRuns = mModel->mLogicalModel->mFontDescriptionRuns;

      // Get the default font's description.
      TextAbstraction::FontDescription defaultFontDescription;
      TextAbstraction::PointSize26Dot6 defaultPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE * mFontSizeScale;

      if( IsShowingPlaceholderText() && mEventData && ( NULL != mEventData->mPlaceholderFont ) )
      {
        // If the placeholder font is set specifically, only placeholder font is changed.
        defaultFontDescription = mEventData->mPlaceholderFont->mFontDescription;
        if( mEventData->mPlaceholderFont->sizeDefined )
        {
          defaultPointSize = mEventData->mPlaceholderFont->mDefaultPointSize * mFontSizeScale * 64u;
        }
      }
      else if( NULL != mFontDefaults )
      {
        // Set the normal font and the placeholder font.
        defaultFontDescription = mFontDefaults->mFontDescription;

        if( mTextFitEnabled )
        {
          defaultPointSize = mFontDefaults->mFitPointSize * 64u;
        }
        else
        {
          defaultPointSize = mFontDefaults->mDefaultPointSize * mFontSizeScale * 64u;
        }
      }

      // Validates the fonts. If there is a character with no assigned font it sets a default one.
      // After this call, fonts are validated.
      multilanguageSupport.ValidateFonts( utf32Characters,
                                          scripts,
                                          fontDescriptionRuns,
                                          defaultFontDescription,
                                          defaultPointSize,
                                          startIndex,
                                          requestedNumberOfCharacters,
                                          validFonts );
    }
    updated = true;
  }

  Vector<Character> mirroredUtf32Characters;
  bool textMirrored = false;
  const Length numberOfParagraphs = mModel->mLogicalModel->mParagraphInfo.Count();
  if( NO_OPERATION != ( BIDI_INFO & operations ) )
  {
    Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mModel->mLogicalModel->mBidirectionalParagraphInfo;
    bidirectionalInfo.Reserve( numberOfParagraphs );

    // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
    SetBidirectionalInfo( utf32Characters,
                          scripts,
                          lineBreakInfo,
                          startIndex,
                          requestedNumberOfCharacters,
                          bidirectionalInfo,
                          mModel->mMatchSystemLanguageDirection,
                          mLayoutDirection );

    if( 0u != bidirectionalInfo.Count() )
    {
      // Only set the character directions if there is right to left characters.
      Vector<CharacterDirection>& directions = mModel->mLogicalModel->mCharacterDirections;
      GetCharactersDirection( bidirectionalInfo,
                              numberOfCharacters,
                              startIndex,
                              requestedNumberOfCharacters,
                              directions );

      // This paragraph has right to left text. Some characters may need to be mirrored.
      // TODO: consider if the mirrored string can be stored as well.

      textMirrored = GetMirroredText( utf32Characters,
                                      directions,
                                      bidirectionalInfo,
                                      startIndex,
                                      requestedNumberOfCharacters,
                                      mirroredUtf32Characters );
    }
    else
    {
      // There is no right to left characters. Clear the directions vector.
      mModel->mLogicalModel->mCharacterDirections.Clear();
    }
    updated = true;
  }

  Vector<GlyphInfo>& glyphs = mModel->mVisualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = mModel->mVisualModel->mGlyphsToCharacters;
  Vector<Length>& charactersPerGlyph = mModel->mVisualModel->mCharactersPerGlyph;
  Vector<GlyphIndex> newParagraphGlyphs;
  newParagraphGlyphs.Reserve( numberOfParagraphs );

  const Length currentNumberOfGlyphs = glyphs.Count();
  if( NO_OPERATION != ( SHAPE_TEXT & operations ) )
  {
    const Vector<Character>& textToShape = textMirrored ? mirroredUtf32Characters : utf32Characters;
    // Shapes the text.
    ShapeText( textToShape,
               lineBreakInfo,
               scripts,
               validFonts,
               startIndex,
               mTextUpdateInfo.mStartGlyphIndex,
               requestedNumberOfCharacters,
               glyphs,
               glyphsToCharactersMap,
               charactersPerGlyph,
               newParagraphGlyphs );

    // Create the 'number of glyphs' per character and the glyph to character conversion tables.
    mModel->mVisualModel->CreateGlyphsPerCharacterTable( startIndex, mTextUpdateInfo.mStartGlyphIndex, requestedNumberOfCharacters );
    mModel->mVisualModel->CreateCharacterToGlyphTable( startIndex, mTextUpdateInfo.mStartGlyphIndex, requestedNumberOfCharacters );
    updated = true;
  }

  const Length numberOfGlyphs = glyphs.Count() - currentNumberOfGlyphs;

  if( NO_OPERATION != ( GET_GLYPH_METRICS & operations ) )
  {
    GlyphInfo* glyphsBuffer = glyphs.Begin();
    mMetrics->GetGlyphMetrics( glyphsBuffer + mTextUpdateInfo.mStartGlyphIndex, numberOfGlyphs );

    // Update the width and advance of all new paragraph characters.
    for( Vector<GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(), endIt = newParagraphGlyphs.End(); it != endIt; ++it )
    {
      const GlyphIndex index = *it;
      GlyphInfo& glyph = *( glyphsBuffer + index );

      glyph.xBearing = 0.f;
      glyph.width = 0.f;
      glyph.advance = 0.f;
    }
    updated = true;
  }

  if( ( NULL != mEventData ) &&
      mEventData->mPreEditFlag &&
      ( 0u != mModel->mVisualModel->mCharactersToGlyph.Count() ) )
  {
    Dali::InputMethodContext::PreEditAttributeDataContainer attrs;
    mEventData->mInputMethodContext.GetPreeditStyle( attrs );
    Dali::InputMethodContext::PreeditStyle type = Dali::InputMethodContext::PreeditStyle::NONE;

    // Check the type of preedit and run it.
    for( Dali::InputMethodContext::PreEditAttributeDataContainer::Iterator it = attrs.Begin(), endIt = attrs.End(); it != endIt; it++ )
    {
      Dali::InputMethodContext::PreeditAttributeData attrData = *it;
      DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::UpdateModel PreeditStyle type : %d  start %d end %d \n", attrData.preeditType, attrData.startIndex, attrData.endIndex  );
      type = attrData.preeditType;

      // Check the number of commit characters for the start position.
      unsigned int numberOfCommit = mEventData->mPrimaryCursorPosition - mEventData->mPreEditLength;
      Length numberOfIndices = attrData.endIndex - attrData.startIndex;

      switch( type )
      {
        case Dali::InputMethodContext::PreeditStyle::UNDERLINE:
        {
          // Add the underline for the pre-edit text.
          GlyphRun underlineRun;
          underlineRun.glyphIndex = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          mModel->mVisualModel->mUnderlineRuns.PushBack( underlineRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::REVERSE:
        {
          Vector4 textColor = mModel->mVisualModel->GetTextColor();
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color = textColor;
          mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun );

          Vector4 backgroundColor = mModel->mVisualModel->GetBackgroundColor();
          Vector<ColorRun>  colorRuns;
          colorRuns.Resize( 1u );
          ColorRun& colorRun = *( colorRuns.Begin() );
          colorRun.color = backgroundColor;
          colorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          colorRun.characterRun.numberOfCharacters = numberOfIndices;

          mModel->mLogicalModel->mColorRuns.PushBack( colorRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::HIGHLIGHT:
        {
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color = LIGHT_BLUE;
          mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_1:
        {
          // CUSTOM_PLATFORM_STYLE_1 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color = BACKGROUND_SUB4;
          mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun );

          GlyphRun underlineRun;
          underlineRun.glyphIndex = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          mModel->mVisualModel->mUnderlineRuns.PushBack( underlineRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_2:
        {
          // CUSTOM_PLATFORM_STYLE_2 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color = BACKGROUND_SUB5;
          mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun );

          GlyphRun underlineRun;
          underlineRun.glyphIndex = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          mModel->mVisualModel->mUnderlineRuns.PushBack( underlineRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_3:
        {
          // CUSTOM_PLATFORM_STYLE_3 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color = BACKGROUND_SUB6;
          mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun );

          GlyphRun underlineRun;
          underlineRun.glyphIndex = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          mModel->mVisualModel->mUnderlineRuns.PushBack( underlineRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_4:
        {
          // CUSTOM_PLATFORM_STYLE_4 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color = BACKGROUND_SUB7;
          mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun );

          GlyphRun underlineRun;
          underlineRun.glyphIndex = attrData.startIndex + numberOfCommit;
          underlineRun.numberOfGlyphs = numberOfIndices;
          mModel->mVisualModel->mUnderlineRuns.PushBack( underlineRun );
          break;
        }
        case Dali::InputMethodContext::PreeditStyle::NONE:
        default:
        {
          break;
        }
      }
    }
    attrs.Clear();
    updated = true;
  }

  if( NO_OPERATION != ( COLOR & operations ) )
  {
    // Set the color runs in glyphs.
    SetColorSegmentationInfo( mModel->mLogicalModel->mColorRuns,
                              mModel->mVisualModel->mCharactersToGlyph,
                              mModel->mVisualModel->mGlyphsPerCharacter,
                              startIndex,
                              mTextUpdateInfo.mStartGlyphIndex,
                              requestedNumberOfCharacters,
                              mModel->mVisualModel->mColors,
                              mModel->mVisualModel->mColorIndices );

    // Set the background color runs in glyphs.
    SetColorSegmentationInfo( mModel->mLogicalModel->mBackgroundColorRuns,
                              mModel->mVisualModel->mCharactersToGlyph,
                              mModel->mVisualModel->mGlyphsPerCharacter,
                              startIndex,
                              mTextUpdateInfo.mStartGlyphIndex,
                              requestedNumberOfCharacters,
                              mModel->mVisualModel->mBackgroundColors,
                              mModel->mVisualModel->mBackgroundColorIndices );

    updated = true;
  }


  // The estimated number of lines. Used to avoid reallocations when layouting.
  mTextUpdateInfo.mEstimatedNumberOfLines = std::max( mModel->mVisualModel->mLines.Count(), mModel->mLogicalModel->mParagraphInfo.Count() );

  // Set the previous number of characters for the next time the text is updated.
  mTextUpdateInfo.mPreviousNumberOfCharacters = numberOfCharacters;

  return updated;
}

void Controller::Impl::RetrieveDefaultInputStyle( InputStyle& inputStyle )
{
  // Sets the default text's color.
  inputStyle.textColor = mTextColor;
  inputStyle.isDefaultColor = true;

  inputStyle.familyName.clear();
  inputStyle.weight = TextAbstraction::FontWeight::NORMAL;
  inputStyle.width = TextAbstraction::FontWidth::NORMAL;
  inputStyle.slant = TextAbstraction::FontSlant::NORMAL;
  inputStyle.size = 0.f;

  inputStyle.lineSpacing = 0.f;

  inputStyle.underlineProperties.clear();
  inputStyle.shadowProperties.clear();
  inputStyle.embossProperties.clear();
  inputStyle.outlineProperties.clear();

  inputStyle.isFamilyDefined = false;
  inputStyle.isWeightDefined = false;
  inputStyle.isWidthDefined = false;
  inputStyle.isSlantDefined = false;
  inputStyle.isSizeDefined = false;

  inputStyle.isLineSpacingDefined = false;

  inputStyle.isUnderlineDefined = false;
  inputStyle.isShadowDefined = false;
  inputStyle.isEmbossDefined = false;
  inputStyle.isOutlineDefined = false;

  // Sets the default font's family name, weight, width, slant and size.
  if( mFontDefaults )
  {
    if( mFontDefaults->familyDefined )
    {
      inputStyle.familyName = mFontDefaults->mFontDescription.family;
      inputStyle.isFamilyDefined = true;
    }

    if( mFontDefaults->weightDefined )
    {
      inputStyle.weight = mFontDefaults->mFontDescription.weight;
      inputStyle.isWeightDefined = true;
    }

    if( mFontDefaults->widthDefined )
    {
      inputStyle.width = mFontDefaults->mFontDescription.width;
      inputStyle.isWidthDefined = true;
    }

    if( mFontDefaults->slantDefined )
    {
      inputStyle.slant = mFontDefaults->mFontDescription.slant;
      inputStyle.isSlantDefined = true;
    }

    if( mFontDefaults->sizeDefined )
    {
      inputStyle.size = mFontDefaults->mDefaultPointSize;
      inputStyle.isSizeDefined = true;
    }
  }
}

float Controller::Impl::GetDefaultFontLineHeight()
{
  FontId defaultFontId = 0u;
  if( NULL == mFontDefaults )
  {
    TextAbstraction::FontDescription fontDescription;
    defaultFontId = mFontClient.GetFontId( fontDescription, TextAbstraction::FontClient::DEFAULT_POINT_SIZE * mFontSizeScale );
  }
  else
  {
    defaultFontId = mFontDefaults->GetFontId( mFontClient, mFontDefaults->mDefaultPointSize * mFontSizeScale );
  }

  Text::FontMetrics fontMetrics;
  mMetrics->GetFontMetrics( defaultFontId, fontMetrics );

  return( fontMetrics.ascender - fontMetrics.descender );
}

void Controller::Impl::SetTextSelectionRange(const uint32_t *pStart, const uint32_t *pEnd)
{
  if( nullptr == mEventData )
  {
    // Nothing to do if there is no text.
    return;
  }

  if( mEventData->mSelectionEnabled && (pStart || pEnd))
  {
    uint32_t length = static_cast<uint32_t>(mModel->mLogicalModel->mText.Count());

    if (pStart)
    {
      mEventData->mLeftSelectionPosition = std::min(*pStart, length);
    }
    if (pEnd)
    {
      mEventData->mRightSelectionPosition = std::min(*pEnd, length);
    }

    if (mEventData->mLeftSelectionPosition == mEventData->mRightSelectionPosition)
    {
      ChangeState( EventData::EDITING );
      mEventData->mPrimaryCursorPosition = mEventData->mLeftSelectionPosition = mEventData->mRightSelectionPosition;
      mEventData->mUpdateCursorPosition = true;
    }
    else
    {
      ChangeState( EventData::SELECTING );
      mEventData->mUpdateHighlightBox = true;
      mEventData->mUpdateLeftSelectionPosition = true;
      mEventData->mUpdateRightSelectionPosition = true;
    }
  }
}

CharacterIndex Controller::Impl::GetPrimaryCursorPosition() const
{
  if( nullptr == mEventData )
  {
    return 0;
  }
  return mEventData->mPrimaryCursorPosition;
}

bool Controller::Impl::SetPrimaryCursorPosition( CharacterIndex index )
{
  if( nullptr == mEventData )
  {
    // Nothing to do if there is no text.
    return false;
  }

  if( mEventData->mPrimaryCursorPosition == index )
  {
    // Nothing for same cursor position.
    return false;
  }

  uint32_t length = static_cast<uint32_t>(mModel->mLogicalModel->mText.Count());
  mEventData->mPrimaryCursorPosition = std::min(index, length);
  ChangeState( EventData::EDITING );
  mEventData->mLeftSelectionPosition = mEventData->mRightSelectionPosition = mEventData->mPrimaryCursorPosition;
  mEventData->mUpdateCursorPosition = true;
  ScrollTextToMatchCursor();
  return true;
}

Uint32Pair Controller::Impl::GetTextSelectionRange() const
{
  Uint32Pair range;

  if( mEventData )
  {
    range.first = mEventData->mLeftSelectionPosition;
    range.second = mEventData->mRightSelectionPosition;
  }

  return range;
}

bool Controller::Impl::IsEditable() const
{
  return mEventData && mEventData->mEditingEnabled;
}

void Controller::Impl::SetEditable( bool editable )
{
  if( mEventData)
  {
    mEventData->mEditingEnabled = editable;
  }
}

void Controller::Impl::RetrieveSelection( std::string& selectedText, bool deleteAfterRetrieval )
{
  if( mEventData->mLeftSelectionPosition == mEventData->mRightSelectionPosition )
  {
    // Nothing to select if handles are in the same place.
    selectedText.clear();
    return;
  }

  const bool handlesCrossed = mEventData->mLeftSelectionPosition > mEventData->mRightSelectionPosition;

  //Get start and end position of selection
  const CharacterIndex startOfSelectedText = handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
  const Length lengthOfSelectedText = ( handlesCrossed ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition ) - startOfSelectedText;

  Vector<Character>& utf32Characters = mModel->mLogicalModel->mText;
  const Length numberOfCharacters = utf32Characters.Count();

  // Validate the start and end selection points
  if( ( startOfSelectedText + lengthOfSelectedText ) <= numberOfCharacters )
  {
    //Get text as a UTF8 string
    Utf32ToUtf8( &utf32Characters[startOfSelectedText], lengthOfSelectedText, selectedText );

    if( deleteAfterRetrieval ) // Only delete text if copied successfully
    {
      // Keep a copy of the current input style.
      InputStyle currentInputStyle;
      currentInputStyle.Copy( mEventData->mInputStyle );

      // Set as input style the style of the first deleted character.
      mModel->mLogicalModel->RetrieveStyle( startOfSelectedText, mEventData->mInputStyle );

      // Compare if the input style has changed.
      const bool hasInputStyleChanged = !currentInputStyle.Equal( mEventData->mInputStyle );

      if( hasInputStyleChanged )
      {
        const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask( mEventData->mInputStyle );
        // Queue the input style changed signal.
        mEventData->mInputStyleChangedQueue.PushBack( styleChangedMask );
      }

      mModel->mLogicalModel->UpdateTextStyleRuns( startOfSelectedText, -static_cast<int>( lengthOfSelectedText ) );

      // Mark the paragraphs to be updated.
      if( Layout::Engine::SINGLE_LINE_BOX == mLayoutEngine.GetLayout() )
      {
        mTextUpdateInfo.mCharacterIndex = 0;
        mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
        mTextUpdateInfo.mNumberOfCharactersToAdd = mTextUpdateInfo.mPreviousNumberOfCharacters - lengthOfSelectedText;
        mTextUpdateInfo.mClearAll = true;
      }
      else
      {
        mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
        mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      }

      // Delete text between handles
      Vector<Character>::Iterator first = utf32Characters.Begin() + startOfSelectedText;
      Vector<Character>::Iterator last  = first + lengthOfSelectedText;
      utf32Characters.Erase( first, last );

      // Will show the cursor at the first character of the selection.
      mEventData->mPrimaryCursorPosition = handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
    }
    else
    {
      // Will show the cursor at the last character of the selection.
      mEventData->mPrimaryCursorPosition = handlesCrossed ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition;
    }

    mEventData->mDecoratorUpdated = true;
  }
}

void Controller::Impl::SetSelection( int start, int end )
{
  mEventData->mLeftSelectionPosition = start;
  mEventData->mRightSelectionPosition = end;
  mEventData->mUpdateCursorPosition = true;
}

std::pair< int, int > Controller::Impl::GetSelectionIndexes() const
{
  return { mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition };
}

void Controller::Impl::ShowClipboard()
{
  if( mClipboard )
  {
    mClipboard.ShowClipboard();
  }
}

void Controller::Impl::HideClipboard()
{
  if( mClipboard && mClipboardHideEnabled )
  {
    mClipboard.HideClipboard();
  }
}

void Controller::Impl::SetClipboardHideEnable(bool enable)
{
  mClipboardHideEnabled = enable;
}

bool Controller::Impl::CopyStringToClipboard( const std::string& source )
{
  //Send string to clipboard
  return ( mClipboard && mClipboard.SetItem( source ) );
}

void Controller::Impl::SendSelectionToClipboard( bool deleteAfterSending )
{
  std::string selectedText;
  RetrieveSelection( selectedText, deleteAfterSending );
  CopyStringToClipboard( selectedText );
  ChangeState( EventData::EDITING );
}

void Controller::Impl::RequestGetTextFromClipboard()
{
  if ( mClipboard )
  {
    mClipboard.RequestItem();
  }
}

void Controller::Impl::RepositionSelectionHandles()
{
  SelectionHandleController::Reposition(*this);
}
void Controller::Impl::RepositionSelectionHandles( float visualX, float visualY, Controller::NoTextTap::Action action )
{
  SelectionHandleController::Reposition(*this, visualX, visualY, action);
}

void Controller::Impl::SetPopupButtons()
{
  /**
   *  Sets the Popup buttons to be shown depending on State.
   *
   *  If SELECTING :  CUT & COPY + ( PASTE & CLIPBOARD if content available to paste )
   *
   *  If EDITING_WITH_POPUP : SELECT & SELECT_ALL
   */

  bool isEditable = IsEditable();
  TextSelectionPopup::Buttons buttonsToShow = TextSelectionPopup::NONE;

  if( EventData::SELECTING == mEventData->mState )
  {
    buttonsToShow = TextSelectionPopup::Buttons( TextSelectionPopup::COPY );
    if(isEditable)
    {
      buttonsToShow = TextSelectionPopup::Buttons( buttonsToShow | TextSelectionPopup::CUT );
    }

    if( !IsClipboardEmpty() )
    {
      if(isEditable)
      {
        buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
      }
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::CLIPBOARD ) );
    }

    if( !mEventData->mAllTextSelected )
    {
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::SELECT_ALL ) );
    }
  }
  else if( EventData::EDITING_WITH_POPUP == mEventData->mState )
  {
    if( mModel->mLogicalModel->mText.Count() && !IsShowingPlaceholderText() )
    {
      buttonsToShow = TextSelectionPopup::Buttons( TextSelectionPopup::SELECT | TextSelectionPopup::SELECT_ALL );
    }

    if( !IsClipboardEmpty() )
    {
      if(isEditable)
      {
        buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
      }
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::CLIPBOARD ) );
    }
  }
  else if( EventData::EDITING_WITH_PASTE_POPUP == mEventData->mState )
  {
    if ( !IsClipboardEmpty() )
    {
      if(isEditable)
      {
        buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
      }
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::CLIPBOARD ) );
    }
  }

  mEventData->mDecorator->SetEnabledPopupButtons( buttonsToShow );
}

void Controller::Impl::ChangeState( EventData::State newState )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "ChangeState state:%d  newstate:%d\n", mEventData->mState, newState );

  if( mEventData->mState != newState )
  {
    mEventData->mPreviousState = mEventData->mState;
    mEventData->mState = newState;

    switch( mEventData->mState )
    {
      case EventData::INACTIVE:
      {
        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
        mEventData->mDecorator->StopCursorBlink();
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHighlightActive( false );
        mEventData->mDecorator->SetPopupActive( false );
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::INTERRUPTED:
      {
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHighlightActive( false );
        mEventData->mDecorator->SetPopupActive( false );
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::SELECTING:
      {
        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
        mEventData->mDecorator->StopCursorBlink();
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
        if ( mEventData->mGrabHandleEnabled )
        {
          mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
          mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
        }
        mEventData->mDecorator->SetHighlightActive( true );
        if( mEventData->mGrabHandlePopupEnabled )
        {
          SetPopupButtons();
          mEventData->mDecorator->SetPopupActive( true );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING:
      {
        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
        if( mEventData->mCursorBlinkEnabled )
        {
          mEventData->mDecorator->StartCursorBlink();
        }
        // Grab handle is not shown until a tap is received whilst EDITING
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHighlightActive( false );
        if( mEventData->mGrabHandlePopupEnabled )
        {
          mEventData->mDecorator->SetPopupActive( false );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_POPUP:
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "EDITING_WITH_POPUP \n", newState );

        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
        if( mEventData->mCursorBlinkEnabled )
        {
          mEventData->mDecorator->StartCursorBlink();
        }
        if( mEventData->mSelectionEnabled )
        {
          mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
          mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
          mEventData->mDecorator->SetHighlightActive( false );
        }
        else if ( mEventData->mGrabHandleEnabled )
        {
          mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
        }
        if( mEventData->mGrabHandlePopupEnabled )
        {
          SetPopupButtons();
          mEventData->mDecorator->SetPopupActive( true );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_GRAB_HANDLE:
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "EDITING_WITH_GRAB_HANDLE \n", newState );

        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
        if( mEventData->mCursorBlinkEnabled )
        {
          mEventData->mDecorator->StartCursorBlink();
        }
        // Grab handle is not shown until a tap is received whilst EDITING
        if ( mEventData->mGrabHandleEnabled )
        {
          mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
        }
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHighlightActive( false );
        if( mEventData->mGrabHandlePopupEnabled )
        {
          mEventData->mDecorator->SetPopupActive( false );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::SELECTION_HANDLE_PANNING:
      {
        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
        mEventData->mDecorator->StopCursorBlink();
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
        if ( mEventData->mGrabHandleEnabled )
        {
          mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
          mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
        }
        mEventData->mDecorator->SetHighlightActive( true );
        if( mEventData->mGrabHandlePopupEnabled )
        {
          mEventData->mDecorator->SetPopupActive( false );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::GRAB_HANDLE_PANNING:
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GRAB_HANDLE_PANNING \n", newState );

        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
        if( mEventData->mCursorBlinkEnabled )
        {
          mEventData->mDecorator->StartCursorBlink();
        }
        if ( mEventData->mGrabHandleEnabled )
        {
          mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
        }
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHighlightActive( false );
        if( mEventData->mGrabHandlePopupEnabled )
        {
          mEventData->mDecorator->SetPopupActive( false );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_PASTE_POPUP:
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "EDITING_WITH_PASTE_POPUP \n", newState );

        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
        if( mEventData->mCursorBlinkEnabled )
        {
          mEventData->mDecorator->StartCursorBlink();
        }

        if ( mEventData->mGrabHandleEnabled )
        {
          mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
        }
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
        mEventData->mDecorator->SetHighlightActive( false );

        if( mEventData->mGrabHandlePopupEnabled )
        {
          SetPopupButtons();
          mEventData->mDecorator->SetPopupActive( true );
        }
        mEventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::TEXT_PANNING:
      {
        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
        mEventData->mDecorator->StopCursorBlink();
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
        if( mEventData->mDecorator->IsHandleActive( LEFT_SELECTION_HANDLE ) ||
            mEventData->mDecorator->IsHandleActive( RIGHT_SELECTION_HANDLE ) )
        {
          mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
          mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
          mEventData->mDecorator->SetHighlightActive( true );
        }

        if( mEventData->mGrabHandlePopupEnabled )
        {
          mEventData->mDecorator->SetPopupActive( false );
        }

        mEventData->mDecoratorUpdated = true;
        break;
      }
    }
  }
}

void Controller::Impl::GetCursorPosition( CharacterIndex logical,
                                          CursorInfo& cursorInfo )
{
  if( !IsShowingRealText() )
  {
    // Do not want to use the place-holder text to set the cursor position.

    // Use the line's height of the font's family set to set the cursor's size.
    // If there is no font's family set, use the default font.
    // Use the current alignment to place the cursor at the beginning, center or end of the box.

    cursorInfo.lineOffset = 0.f;
    cursorInfo.lineHeight = GetDefaultFontLineHeight();
    cursorInfo.primaryCursorHeight = cursorInfo.lineHeight;

    bool isRTL = false;
    if( mModel->mMatchSystemLanguageDirection )
    {
      isRTL = mLayoutDirection == LayoutDirection::RIGHT_TO_LEFT;
    }

    switch( mModel->mHorizontalAlignment )
    {
      case Text::HorizontalAlignment::BEGIN :
      {
        if( isRTL )
        {
          cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetCursorWidth();
        }
        else
        {
          cursorInfo.primaryPosition.x = 0.f;
        }
        break;
      }
      case Text::HorizontalAlignment::CENTER:
      {
        cursorInfo.primaryPosition.x = floorf( 0.5f * mModel->mVisualModel->mControlSize.width );
        break;
      }
      case Text::HorizontalAlignment::END:
      {
        if( isRTL )
        {
          cursorInfo.primaryPosition.x = 0.f;
        }
        else
        {
          cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetCursorWidth();
        }
        break;
      }
    }

    // Nothing else to do.
    return;
  }

  const bool isMultiLine = ( Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout() );
  GetCursorPositionParameters parameters;
  parameters.visualModel = mModel->mVisualModel;
  parameters.logicalModel = mModel->mLogicalModel;
  parameters.metrics = mMetrics;
  parameters.logical = logical;
  parameters.isMultiline = isMultiLine;

  Text::GetCursorPosition( parameters,
                           cursorInfo );

  // Adds Outline offset.
  const float outlineWidth = static_cast<float>( mModel->GetOutlineWidth() );
  cursorInfo.primaryPosition.x += outlineWidth;
  cursorInfo.primaryPosition.y += outlineWidth;
  cursorInfo.secondaryPosition.x += outlineWidth;
  cursorInfo.secondaryPosition.y += outlineWidth;

  if( isMultiLine )
  {
    // If the text is editable and multi-line, the cursor position after a white space shouldn't exceed the boundaries of the text control.

    // Note the white spaces laid-out at the end of the line might exceed the boundaries of the control.
    // The reason is a wrapped line must not start with a white space so they are laid-out at the end of the line.

    if( 0.f > cursorInfo.primaryPosition.x )
    {
      cursorInfo.primaryPosition.x = 0.f;
    }

    const float edgeWidth = mModel->mVisualModel->mControlSize.width - static_cast<float>( mEventData->mDecorator->GetCursorWidth() );
    if( cursorInfo.primaryPosition.x > edgeWidth )
    {
      cursorInfo.primaryPosition.x = edgeWidth;
    }
  }
}

CharacterIndex Controller::Impl::CalculateNewCursorIndex( CharacterIndex index ) const
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return 0u;
  }

  CharacterIndex cursorIndex = mEventData->mPrimaryCursorPosition;

  const GlyphIndex* const charactersToGlyphBuffer = mModel->mVisualModel->mCharactersToGlyph.Begin();
  const Length* const charactersPerGlyphBuffer = mModel->mVisualModel->mCharactersPerGlyph.Begin();

  GlyphIndex glyphIndex = *( charactersToGlyphBuffer + index );
  Length numberOfCharacters = *( charactersPerGlyphBuffer + glyphIndex );

  if( numberOfCharacters > 1u )
  {
    const Script script = mModel->mLogicalModel->GetScript( index );
    if( HasLigatureMustBreak( script ) )
    {
      // Prevents to jump the whole Latin ligatures like fi, ff, or Arabic ﻻ, ...
      numberOfCharacters = 1u;
    }
  }
  else
  {
    while( 0u == numberOfCharacters )
    {
      ++glyphIndex;
      numberOfCharacters = *( charactersPerGlyphBuffer + glyphIndex );
    }
  }

  if( index < mEventData->mPrimaryCursorPosition )
  {
    cursorIndex -= numberOfCharacters;
  }
  else
  {
    cursorIndex += numberOfCharacters;
  }

  // Will update the cursor hook position.
  mEventData->mUpdateCursorHookPosition = true;

  return cursorIndex;
}

void Controller::Impl::UpdateCursorPosition( const CursorInfo& cursorInfo )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::UpdateCursorPosition %p\n", this );
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition no event data\n" );
    return;
  }

  const Vector2 cursorPosition = cursorInfo.primaryPosition + mModel->mScrollPosition;

  mEventData->mDecorator->SetGlyphOffset( PRIMARY_CURSOR, cursorInfo.glyphOffset );

  // Sets the cursor position.
  mEventData->mDecorator->SetPosition( PRIMARY_CURSOR,
                                       cursorPosition.x,
                                       cursorPosition.y,
                                       cursorInfo.primaryCursorHeight,
                                       cursorInfo.lineHeight );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Primary cursor position: %f,%f\n", cursorPosition.x, cursorPosition.y );

  if( mEventData->mUpdateGrabHandlePosition )
  {
    // Sets the grab handle position.
    mEventData->mDecorator->SetPosition( GRAB_HANDLE,
                                         cursorPosition.x,
                                         cursorInfo.lineOffset + mModel->mScrollPosition.y,
                                         cursorInfo.lineHeight );
  }

  if( cursorInfo.isSecondaryCursor )
  {
    mEventData->mDecorator->SetPosition( SECONDARY_CURSOR,
                                         cursorInfo.secondaryPosition.x + mModel->mScrollPosition.x,
                                         cursorInfo.secondaryPosition.y + mModel->mScrollPosition.y,
                                         cursorInfo.secondaryCursorHeight,
                                         cursorInfo.lineHeight );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n", cursorInfo.secondaryPosition.x + mModel->mScrollPosition.x, cursorInfo.secondaryPosition.y + mModel->mScrollPosition.y );
  }

  // Set which cursors are active according the state.
  if( EventData::IsEditingState( mEventData->mState ) || ( EventData::GRAB_HANDLE_PANNING == mEventData->mState ) )
  {
    if( cursorInfo.isSecondaryCursor )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_BOTH );
    }
    else
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
    }
  }
  else
  {
    mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition\n" );
}

void Controller::Impl::UpdateSelectionHandle( HandleType handleType,
                                              const CursorInfo& cursorInfo )
{
  SelectionHandleController::Update(*this, handleType, cursorInfo);
}

void Controller::Impl::ClampHorizontalScroll( const Vector2& layoutSize )
{
  // Clamp between -space & -alignment offset.

  if( layoutSize.width > mModel->mVisualModel->mControlSize.width )
  {
    const float space = ( layoutSize.width - mModel->mVisualModel->mControlSize.width ) + mModel->mAlignmentOffset;
    mModel->mScrollPosition.x = ( mModel->mScrollPosition.x < -space ) ? -space : mModel->mScrollPosition.x;
    mModel->mScrollPosition.x = ( mModel->mScrollPosition.x > -mModel->mAlignmentOffset ) ? -mModel->mAlignmentOffset : mModel->mScrollPosition.x;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mModel->mScrollPosition.x = 0.f;
  }
}

void Controller::Impl::ClampVerticalScroll( const Vector2& layoutSize )
{
  if( Layout::Engine::SINGLE_LINE_BOX == mLayoutEngine.GetLayout() )
  {
    // Nothing to do if the text is single line.
    return;
  }

  // Clamp between -space & 0.
  if( layoutSize.height > mModel->mVisualModel->mControlSize.height )
  {
    const float space = ( layoutSize.height - mModel->mVisualModel->mControlSize.height );
    mModel->mScrollPosition.y = ( mModel->mScrollPosition.y < -space ) ? -space : mModel->mScrollPosition.y;
    mModel->mScrollPosition.y = ( mModel->mScrollPosition.y > 0.f ) ? 0.f : mModel->mScrollPosition.y;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mModel->mScrollPosition.y = 0.f;
  }
}

void Controller::Impl::ScrollToMakePositionVisible( const Vector2& position, float lineHeight )
{
  const float cursorWidth = mEventData->mDecorator ? static_cast<float>( mEventData->mDecorator->GetCursorWidth() ) : 0.f;

  // position is in actor's coords.
  const float positionEndX = position.x + cursorWidth;
  const float positionEndY = position.y + lineHeight;

  // Transform the position to decorator coords.
  const float decoratorPositionBeginX = position.x + mModel->mScrollPosition.x;
  const float decoratorPositionEndX = positionEndX + mModel->mScrollPosition.x;

  const float decoratorPositionBeginY = position.y + mModel->mScrollPosition.y;
  const float decoratorPositionEndY = positionEndY + mModel->mScrollPosition.y;

  if( decoratorPositionBeginX < 0.f )
  {
    mModel->mScrollPosition.x = -position.x;
  }
  else if( decoratorPositionEndX > mModel->mVisualModel->mControlSize.width )
  {
    mModel->mScrollPosition.x = mModel->mVisualModel->mControlSize.width - positionEndX;
  }

  if( Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout() )
  {
    if( decoratorPositionBeginY < 0.f )
    {
      mModel->mScrollPosition.y = -position.y;
    }
    else if( decoratorPositionEndY > mModel->mVisualModel->mControlSize.height )
    {
      mModel->mScrollPosition.y = mModel->mVisualModel->mControlSize.height - positionEndY;
    }
  }
}

void Controller::Impl::ScrollTextToMatchCursor( const CursorInfo& cursorInfo )
{
  // Get the current cursor position in decorator coords.
  const Vector2& currentCursorPosition = mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

  const LineIndex lineIndex = mModel->mVisualModel->GetLineOfCharacter( mEventData->mPrimaryCursorPosition  );



  // Calculate the offset to match the cursor position before the character was deleted.
  mModel->mScrollPosition.x = currentCursorPosition.x - cursorInfo.primaryPosition.x;

  //If text control has more than two lines and current line index is not last, calculate scrollpositionY
  if( mModel->mVisualModel->mLines.Count() > 1u && lineIndex != mModel->mVisualModel->mLines.Count() -1u )
  {
    const float currentCursorGlyphOffset = mEventData->mDecorator->GetGlyphOffset( PRIMARY_CURSOR );
    mModel->mScrollPosition.y = currentCursorPosition.y - cursorInfo.lineOffset - currentCursorGlyphOffset;
  }


  ClampHorizontalScroll( mModel->mVisualModel->GetLayoutSize() );
  ClampVerticalScroll( mModel->mVisualModel->GetLayoutSize() );

  // Makes the new cursor position visible if needed.
  ScrollToMakePositionVisible( cursorInfo.primaryPosition, cursorInfo.lineHeight );
}

void Controller::Impl::ScrollTextToMatchCursor()
{
  CursorInfo cursorInfo;
  GetCursorPosition( mEventData->mPrimaryCursorPosition, cursorInfo );
  ScrollTextToMatchCursor(cursorInfo);
}

void Controller::Impl::RequestRelayout()
{
  if( NULL != mControlInterface )
  {
    mControlInterface->RequestTextRelayout();
  }
}

Actor Controller::Impl::CreateBackgroundActor()
{
  // NOTE: Currently we only support background color for one line left-to-right text,
  //       so the following calculation is based on one line left-to-right text only!

  Actor actor;

  Length numberOfGlyphs = mView.GetNumberOfGlyphs();
  if( numberOfGlyphs > 0u )
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );

    Vector<Vector2> positions;
    positions.Resize( numberOfGlyphs );

    // Get the line where the glyphs are laid-out.
    const LineRun* lineRun = mModel->mVisualModel->mLines.Begin();
    float alignmentOffset = lineRun->alignmentOffset;
    numberOfGlyphs = mView.GetGlyphs( glyphs.Begin(),
                                      positions.Begin(),
                                      alignmentOffset,
                                      0u,
                                      numberOfGlyphs );

    glyphs.Resize( numberOfGlyphs );
    positions.Resize( numberOfGlyphs );

    const GlyphInfo* const glyphsBuffer = glyphs.Begin();
    const Vector2* const positionsBuffer = positions.Begin();

    BackgroundMesh mesh;
    mesh.mVertices.Reserve( 4u * glyphs.Size() );
    mesh.mIndices.Reserve( 6u * glyphs.Size() );

    const Vector2 textSize = mView.GetLayoutSize();

    const float offsetX = textSize.width * 0.5f;
    const float offsetY = textSize.height * 0.5f;

    const Vector4* const backgroundColorsBuffer = mView.GetBackgroundColors();
    const ColorIndex* const backgroundColorIndicesBuffer = mView.GetBackgroundColorIndices();
    const Vector4& defaultBackgroundColor = mModel->mVisualModel->IsBackgroundEnabled() ? mModel->mVisualModel->GetBackgroundColor() : Color::TRANSPARENT;

    Vector4 quad;
    uint32_t numberOfQuads = 0u;

    for( uint32_t i = 0, glyphSize = glyphs.Size(); i < glyphSize; ++i )
    {
      const GlyphInfo& glyph = *( glyphsBuffer + i );

      // Get the background color of the character.
      // The color index zero is reserved for the default background color (i.e. Color::TRANSPARENT)
      const ColorIndex backgroundColorIndex = ( nullptr == backgroundColorsBuffer ) ? 0u : *( backgroundColorIndicesBuffer + i );
      const Vector4& backgroundColor = ( 0u == backgroundColorIndex ) ? defaultBackgroundColor : *( backgroundColorsBuffer + backgroundColorIndex - 1u );

      // Only create quads for glyphs with a background color
      if ( backgroundColor != Color::TRANSPARENT )
      {
        const Vector2 position = *( positionsBuffer + i );

        if ( i == 0u && glyphSize == 1u ) // Only one glyph in the whole text
        {
          quad.x = position.x;
          quad.y = 0.0f;
          quad.z = quad.x + std::max( glyph.advance, glyph.xBearing + glyph.width );
          quad.w = textSize.height;
        }
        else if ( i == 0u ) // The first glyph in the whole text
        {
          quad.x = position.x;
          quad.y = 0.0f;
          quad.z = quad.x - glyph.xBearing + glyph.advance;
          quad.w = textSize.height;
        }
        else if ( i == glyphSize - 1u ) // The last glyph in the whole text
        {
          quad.x = position.x - glyph.xBearing;
          quad.y = 0.0f;
          quad.z = quad.x + std::max( glyph.advance, glyph.xBearing + glyph.width );
          quad.w = textSize.height;
        }
        else // The glyph in the middle of the text
        {
          quad.x = position.x - glyph.xBearing;
          quad.y = 0.0f;
          quad.z = quad.x + glyph.advance;
          quad.w = textSize.height;
        }

        BackgroundVertex vertex;

        // Top left
        vertex.mPosition.x = quad.x - offsetX;
        vertex.mPosition.y = quad.y - offsetY;
        vertex.mColor = backgroundColor;
        mesh.mVertices.PushBack( vertex );

        // Top right
        vertex.mPosition.x = quad.z - offsetX;
        vertex.mPosition.y = quad.y - offsetY;
        vertex.mColor = backgroundColor;
        mesh.mVertices.PushBack( vertex );

        // Bottom left
        vertex.mPosition.x = quad.x - offsetX;
        vertex.mPosition.y = quad.w - offsetY;
        vertex.mColor = backgroundColor;
        mesh.mVertices.PushBack( vertex );

        // Bottom right
        vertex.mPosition.x = quad.z - offsetX;
        vertex.mPosition.y = quad.w - offsetY;
        vertex.mColor = backgroundColor;
        mesh.mVertices.PushBack( vertex );

        // Six indices in counter clockwise winding
        mesh.mIndices.PushBack( 1u + 4 * numberOfQuads );
        mesh.mIndices.PushBack( 0u + 4 * numberOfQuads );
        mesh.mIndices.PushBack( 2u + 4 * numberOfQuads );
        mesh.mIndices.PushBack( 2u + 4 * numberOfQuads );
        mesh.mIndices.PushBack( 3u + 4 * numberOfQuads );
        mesh.mIndices.PushBack( 1u + 4 * numberOfQuads );

        numberOfQuads++;
      }
    }

    // Only create the background actor if there are glyphs with background color
    if ( mesh.mVertices.Count() > 0u )
    {
      Property::Map quadVertexFormat;
      quadVertexFormat[ "aPosition" ] = Property::VECTOR2;
      quadVertexFormat[ "aColor" ] = Property::VECTOR4;

      VertexBuffer quadVertices = VertexBuffer::New( quadVertexFormat );
      quadVertices.SetData( &mesh.mVertices[ 0 ], mesh.mVertices.Size() );

      Geometry quadGeometry = Geometry::New();
      quadGeometry.AddVertexBuffer( quadVertices );
      quadGeometry.SetIndexBuffer( &mesh.mIndices[ 0 ], mesh.mIndices.Size() );

      if( !mShaderBackground )
      {
        mShaderBackground = Shader::New( VERTEX_SHADER_BACKGROUND, FRAGMENT_SHADER_BACKGROUND );
      }

      Dali::Renderer renderer = Dali::Renderer::New( quadGeometry, mShaderBackground );
      renderer.SetProperty( Dali::Renderer::Property::BLEND_MODE, BlendMode::ON );
      renderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, DepthIndex::CONTENT );

      actor = Actor::New();
      actor.SetProperty( Dali::Actor::Property::NAME, "TextBackgroundColorActor" );
      actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      actor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      actor.SetProperty( Actor::Property::SIZE, textSize );
      actor.SetProperty( Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR );
      actor.AddRenderer( renderer );
    }
  }

  return actor;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
