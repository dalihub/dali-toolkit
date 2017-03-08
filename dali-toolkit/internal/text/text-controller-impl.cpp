/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/integration-api/debug.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/color-segmentation.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-run-container.h>

namespace
{

/**
 * @brief Struct used to calculate the selection box.
 */
struct SelectionBoxInfo
{
  float lineOffset;
  float lineHeight;
  float minX;
  float maxX;
};

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const float MAX_FLOAT = std::numeric_limits<float>::max();
const float MIN_FLOAT = std::numeric_limits<float>::min();
const Dali::Toolkit::Text::CharacterDirection LTR = false; ///< Left To Right direction
const uint32_t STAR = 0x2A;

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

EventData::EventData( DecoratorPtr decorator )
: mDecorator( decorator ),
  mImfManager(),
  mPlaceholderTextActive(),
  mPlaceholderTextInactive(),
  mPlaceholderTextColor( 0.8f, 0.8f, 0.8f, 0.8f ),
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
  mUpdateHighlightBox( false ),
  mScrollAfterUpdatePosition( false ),
  mScrollAfterDelete( false ),
  mAllTextSelected( false ),
  mUpdateInputStyle( false ),
  mPasswordInput( false )
{
  mImfManager = ImfManager::Get();
}

EventData::~EventData()
{}

bool Controller::Impl::ProcessInputEvents()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::ProcessInputEvents\n" );
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::ProcessInputEvents no event data\n" );
    return false;
  }

  if( mEventData->mDecorator )
  {
    for( std::vector<Event>::iterator iter = mEventData->mEventQueue.begin();
         iter != mEventData->mEventQueue.end();
         ++iter )
    {
      switch( iter->type )
      {
        case Event::CURSOR_KEY_EVENT:
        {
          OnCursorKeyEvent( *iter );
          break;
        }
        case Event::TAP_EVENT:
        {
          OnTapEvent( *iter );
          break;
        }
        case Event::LONG_PRESS_EVENT:
        {
          OnLongPressEvent( *iter );
          break;
        }
        case Event::PAN_EVENT:
        {
          OnPanEvent( *iter );
          break;
        }
        case Event::GRAB_HANDLE_EVENT:
        case Event::LEFT_SELECTION_HANDLE_EVENT:
        case Event::RIGHT_SELECTION_HANDLE_EVENT: // Fall through
        {
          OnHandleEvent( *iter );
          break;
        }
        case Event::SELECT:
        {
          OnSelectEvent( *iter );
          break;
        }
        case Event::SELECT_ALL:
        {
          OnSelectAllEvent();
          break;
        }
      }
    }
  }

  if( mEventData->mUpdateCursorPosition ||
      mEventData->mUpdateHighlightBox )
  {
    NotifyImfManager();
  }

  // The cursor must also be repositioned after inserts into the model
  if( mEventData->mUpdateCursorPosition )
  {
    // Updates the cursor position and scrolls the text to make it visible.
    CursorInfo cursorInfo;
    // Calculate the cursor position from the new cursor index.
    GetCursorPosition( mEventData->mPrimaryCursorPosition,
                       cursorInfo );

    if( mEventData->mUpdateCursorHookPosition )
    {
      // Update the cursor hook position. Used to move the cursor with the keys 'up' and 'down'.
      mEventData->mCursorHookPositionX = cursorInfo.primaryPosition.x;
      mEventData->mUpdateCursorHookPosition = false;
    }

    // Scroll first the text after delete ...
    if( mEventData->mScrollAfterDelete )
    {
      ScrollTextToMatchCursor( cursorInfo );
    }

    // ... then, text can be scrolled to make the cursor visible.
    if( mEventData->mScrollAfterUpdatePosition )
    {
      const Vector2 currentCursorPosition( cursorInfo.primaryPosition.x, cursorInfo.lineOffset );
      ScrollToMakePositionVisible( currentCursorPosition, cursorInfo.lineHeight );
    }
    mEventData->mScrollAfterUpdatePosition = false;
    mEventData->mScrollAfterDelete = false;

    UpdateCursorPosition( cursorInfo );

    mEventData->mDecoratorUpdated = true;
    mEventData->mUpdateCursorPosition = false;
    mEventData->mUpdateGrabHandlePosition = false;
  }
  else
  {
    CursorInfo leftHandleInfo;
    CursorInfo rightHandleInfo;

    if( mEventData->mUpdateHighlightBox )
    {
      GetCursorPosition( mEventData->mLeftSelectionPosition,
                         leftHandleInfo );

      GetCursorPosition( mEventData->mRightSelectionPosition,
                         rightHandleInfo );

      if( mEventData->mScrollAfterUpdatePosition && ( mEventData->mIsLeftHandleSelected ? mEventData->mUpdateLeftSelectionPosition : mEventData->mUpdateRightSelectionPosition ) )
      {
        CursorInfo& info = mEventData->mIsLeftHandleSelected ? leftHandleInfo : rightHandleInfo;

        const Vector2 currentCursorPosition( info.primaryPosition.x, info.lineOffset );
        ScrollToMakePositionVisible( currentCursorPosition, info.lineHeight );
      }
    }

    if( mEventData->mUpdateLeftSelectionPosition )
    {
      UpdateSelectionHandle( LEFT_SELECTION_HANDLE,
                             leftHandleInfo );

      SetPopupButtons();
      mEventData->mDecoratorUpdated = true;
      mEventData->mUpdateLeftSelectionPosition = false;
    }

    if( mEventData->mUpdateRightSelectionPosition )
    {
      UpdateSelectionHandle( RIGHT_SELECTION_HANDLE,
                             rightHandleInfo );

      SetPopupButtons();
      mEventData->mDecoratorUpdated = true;
      mEventData->mUpdateRightSelectionPosition = false;
    }

    if( mEventData->mUpdateHighlightBox )
    {
      RepositionSelectionHandles();

      mEventData->mUpdateLeftSelectionPosition = false;
      mEventData->mUpdateRightSelectionPosition = false;
      mEventData->mUpdateHighlightBox = false;
    }

    mEventData->mScrollAfterUpdatePosition = false;
  }

  if( mEventData->mUpdateInputStyle )
  {
    // Keep a copy of the current input style.
    InputStyle currentInputStyle;
    currentInputStyle.Copy( mEventData->mInputStyle );

    // Set the default style first.
    RetrieveDefaultInputStyle( mEventData->mInputStyle );

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = ( mEventData->mPrimaryCursorPosition > 0u ) ? mEventData->mPrimaryCursorPosition - 1u : 0u;

    // Retrieve the style from the style runs stored in the logical model.
    mModel->mLogicalModel->RetrieveStyle( styleIndex, mEventData->mInputStyle );

    // Compare if the input style has changed.
    const bool hasInputStyleChanged = !currentInputStyle.Equal( mEventData->mInputStyle );

    if( hasInputStyleChanged )
    {
      const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask( mEventData->mInputStyle );
      // Queue the input style changed signal.
      mEventData->mInputStyleChangedQueue.PushBack( styleChangedMask );
    }

    mEventData->mUpdateInputStyle = false;
  }

  mEventData->mEventQueue.clear();

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::ProcessInputEvents\n" );

  const bool decoratorUpdated = mEventData->mDecoratorUpdated;
  mEventData->mDecoratorUpdated = false;

  return decoratorUpdated;
}

void Controller::Impl::NotifyImfManager()
{
  if( mEventData && mEventData->mImfManager )
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

    mEventData->mImfManager.SetCursorPosition( cursorPosition );
    mEventData->mImfManager.NotifyCursorPosition();
  }
}

void Controller::Impl::NotifyImfMultiLineStatus()
{
  if ( mEventData )
  {
    Text::Layout::Engine::Type layout = mLayoutEngine.GetLayout();
    mEventData->mImfManager.NotifyTextInputMultiLine( layout == Text::Layout::Engine::MULTI_LINE_BOX );
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

  if( NO_OPERATION != ( GET_WORD_BREAKS & operations ) )
  {
    mModel->mLogicalModel->mLineBreakInfo.Clear();
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

  if( NO_OPERATION != ( GET_WORD_BREAKS & operations ) )
  {
    // Clear the word break info.
    WordBreakInfo* wordBreakInfoBuffer = mModel->mLogicalModel->mWordBreakInfo.Begin();

    mModel->mLogicalModel->mWordBreakInfo.Erase( wordBreakInfoBuffer + startIndex,
                                         wordBreakInfoBuffer + endIndexPlusOne );
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

  Vector<Character> utf32CharactersStar;
  const Length characterCount = mModel->mLogicalModel->mText.Count();
  const bool isPasswordInput = ( mEventData != NULL && mEventData->mPasswordInput &&
        !mEventData->mIsShowingPlaceholderText && characterCount > 0 );

  if (isPasswordInput)
  {
    utf32CharactersStar.Resize( characterCount );

    uint32_t* begin = utf32CharactersStar.Begin();
    uint32_t* end = begin + characterCount;
    while ( begin < end )
    {
      *begin++ = STAR;
    }
  }

  Vector<Character>& utf32Characters = isPasswordInput ? utf32CharactersStar : mModel->mLogicalModel->mText;
  const Length numberOfCharacters = utf32Characters.Count();

  // Index to the first character of the first paragraph to be updated.
  CharacterIndex startIndex = 0u;
  // Number of characters of the paragraphs to be removed.
  Length paragraphCharacters = 0u;

  CalculateTextUpdateIndices( paragraphCharacters );
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

  Vector<WordBreakInfo>& wordBreakInfo = mModel->mLogicalModel->mWordBreakInfo;
  if( NO_OPERATION != ( GET_WORD_BREAKS & operations ) )
  {
    // Retrieves the word break info. The word break info is used to layout the text (where to wrap the text in lines).
    wordBreakInfo.Resize( numberOfCharacters, TextAbstraction::WORD_NO_BREAK );

    SetWordBreakInfo( utf32Characters,
                      startIndex,
                      requestedNumberOfCharacters,
                      wordBreakInfo );
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
      TextAbstraction::PointSize26Dot6 defaultPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE;
      if( NULL != mFontDefaults )
      {
        defaultFontDescription = mFontDefaults->mFontDescription;
        defaultPointSize = mFontDefaults->mDefaultPointSize * 64u;
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
                          bidirectionalInfo );

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

    updated = true;
  }

  if( ( NULL != mEventData ) &&
      mEventData->mPreEditFlag &&
      ( 0u != mModel->mVisualModel->mCharactersToGlyph.Count() ) )
  {
    // Add the underline for the pre-edit text.
    const GlyphIndex* const charactersToGlyphBuffer = mModel->mVisualModel->mCharactersToGlyph.Begin();
    const Length* const glyphsPerCharacterBuffer = mModel->mVisualModel->mGlyphsPerCharacter.Begin();

    const GlyphIndex glyphStart = *( charactersToGlyphBuffer + mEventData->mPreEditStartPosition );
    const CharacterIndex lastPreEditCharacter = mEventData->mPreEditStartPosition + ( ( mEventData->mPreEditLength > 0u ) ? mEventData->mPreEditLength - 1u : 0u );
    const Length numberOfGlyphsLastCharacter = *( glyphsPerCharacterBuffer + lastPreEditCharacter );
    const GlyphIndex glyphEnd = *( charactersToGlyphBuffer + lastPreEditCharacter ) + ( numberOfGlyphsLastCharacter > 1u ? numberOfGlyphsLastCharacter - 1u : 0u );

    GlyphRun underlineRun;
    underlineRun.glyphIndex = glyphStart;
    underlineRun.numberOfGlyphs = 1u + glyphEnd - glyphStart;

    // TODO: At the moment the underline runs are only for pre-edit.
    mModel->mVisualModel->mUnderlineRuns.PushBack( underlineRun );
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
    defaultFontId = mFontClient.GetFontId( fontDescription );
  }
  else
  {
    defaultFontId = mFontDefaults->GetFontId( mFontClient );
  }

  Text::FontMetrics fontMetrics;
  mMetrics->GetFontMetrics( defaultFontId, fontMetrics );

  return( fontMetrics.ascender - fontMetrics.descender );
}

void Controller::Impl::OnCursorKeyEvent( const Event& event )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  int keyCode = event.p1.mInt;

  if( Dali::DALI_KEY_CURSOR_LEFT == keyCode )
  {
    if( mEventData->mPrimaryCursorPosition > 0u )
    {
      mEventData->mPrimaryCursorPosition = CalculateNewCursorIndex( mEventData->mPrimaryCursorPosition - 1u );
    }
  }
  else if( Dali::DALI_KEY_CURSOR_RIGHT == keyCode )
  {
    if( mModel->mLogicalModel->mText.Count() > mEventData->mPrimaryCursorPosition )
    {
      mEventData->mPrimaryCursorPosition = CalculateNewCursorIndex( mEventData->mPrimaryCursorPosition );
    }
  }
  else if( Dali::DALI_KEY_CURSOR_UP == keyCode )
  {
    // Get first the line index of the current cursor position index.
    CharacterIndex characterIndex = 0u;

    if( mEventData->mPrimaryCursorPosition > 0u )
    {
      characterIndex = mEventData->mPrimaryCursorPosition - 1u;
    }

    const LineIndex lineIndex = mModel->mVisualModel->GetLineOfCharacter( characterIndex );

    if( lineIndex > 0u )
    {
      // Retrieve the cursor position info.
      CursorInfo cursorInfo;
      GetCursorPosition( mEventData->mPrimaryCursorPosition,
                         cursorInfo );

      // Get the line above.
      const LineRun& line = *( mModel->mVisualModel->mLines.Begin() + ( lineIndex - 1u ) );

      // Get the next hit 'y' point.
      const float hitPointY = cursorInfo.lineOffset - 0.5f * ( line.ascender - line.descender );

      // Use the cursor hook position 'x' and the next hit 'y' position to calculate the new cursor index.
      mEventData->mPrimaryCursorPosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                                        mModel->mLogicalModel,
                                                                        mMetrics,
                                                                        mEventData->mCursorHookPositionX,
                                                                        hitPointY );
    }
  }
  else if(   Dali::DALI_KEY_CURSOR_DOWN == keyCode )
  {
    // Get first the line index of the current cursor position index.
    CharacterIndex characterIndex = 0u;

    if( mEventData->mPrimaryCursorPosition > 0u )
    {
      characterIndex = mEventData->mPrimaryCursorPosition - 1u;
    }

    const LineIndex lineIndex = mModel->mVisualModel->GetLineOfCharacter( characterIndex );

    if( lineIndex + 1u < mModel->mVisualModel->mLines.Count() )
    {
      // Retrieve the cursor position info.
      CursorInfo cursorInfo;
      GetCursorPosition( mEventData->mPrimaryCursorPosition,
                         cursorInfo );

      // Get the line below.
      const LineRun& line = *( mModel->mVisualModel->mLines.Begin() + lineIndex + 1u );

      // Get the next hit 'y' point.
      const float hitPointY = cursorInfo.lineOffset + cursorInfo.lineHeight + 0.5f * ( line.ascender - line.descender );

      // Use the cursor hook position 'x' and the next hit 'y' position to calculate the new cursor index.
      mEventData->mPrimaryCursorPosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                                        mModel->mLogicalModel,
                                                                        mMetrics,
                                                                        mEventData->mCursorHookPositionX,
                                                                        hitPointY );
    }
  }

  mEventData->mUpdateCursorPosition = true;
  mEventData->mUpdateInputStyle = true;
  mEventData->mScrollAfterUpdatePosition = true;
}

void Controller::Impl::OnTapEvent( const Event& event )
{
  if( NULL != mEventData )
  {
    const unsigned int tapCount = event.p1.mUint;

    if( 1u == tapCount )
    {
      if( IsShowingRealText() )
      {
        // Convert from control's coords to text's coords.
        const float xPosition = event.p2.mFloat - mModel->mScrollPosition.x;
        const float yPosition = event.p3.mFloat - mModel->mScrollPosition.y;

        // Keep the tap 'x' position. Used to move the cursor.
        mEventData->mCursorHookPositionX = xPosition;

        mEventData->mPrimaryCursorPosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                                          mModel->mLogicalModel,
                                                                          mMetrics,
                                                                          xPosition,
                                                                          yPosition );

        // When the cursor position is changing, delay cursor blinking
        mEventData->mDecorator->DelayCursorBlink();
      }
      else
      {
        mEventData->mPrimaryCursorPosition = 0u;
      }

      mEventData->mUpdateCursorPosition = true;
      mEventData->mUpdateGrabHandlePosition = true;
      mEventData->mScrollAfterUpdatePosition = true;
      mEventData->mUpdateInputStyle = true;

      // Notify the cursor position to the imf manager.
      if( mEventData->mImfManager )
      {
        mEventData->mImfManager.SetCursorPosition( mEventData->mPrimaryCursorPosition );
        mEventData->mImfManager.NotifyCursorPosition();
      }
    }
  }
}

void Controller::Impl::OnPanEvent( const Event& event )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  const bool isHorizontalScrollEnabled = mEventData->mDecorator->IsHorizontalScrollEnabled();
  const bool isVerticalScrollEnabled = mEventData->mDecorator->IsVerticalScrollEnabled();

  if( !isHorizontalScrollEnabled && !isVerticalScrollEnabled )
  {
    // Nothing to do if scrolling is not enabled.
    return;
  }

  const int state = event.p1.mInt;

  switch( state )
  {
    case Gesture::Started:
    {
      // Will remove the cursor, handles or text's popup, ...
      ChangeState( EventData::TEXT_PANNING );
      break;
    }
    case Gesture::Continuing:
    {
      const Vector2& layoutSize = mModel->mVisualModel->GetLayoutSize();
      const Vector2 currentScroll = mModel->mScrollPosition;

      if( isHorizontalScrollEnabled )
      {
        const float displacementX = event.p2.mFloat;
        mModel->mScrollPosition.x += displacementX;

        ClampHorizontalScroll( layoutSize );
      }

      if( isVerticalScrollEnabled )
      {
        const float displacementY = event.p3.mFloat;
        mModel->mScrollPosition.y += displacementY;

        ClampVerticalScroll( layoutSize );
      }

      mEventData->mDecorator->UpdatePositions( mModel->mScrollPosition - currentScroll );
      break;
    }
    case Gesture::Finished:
    case Gesture::Cancelled: // FALLTHROUGH
    {
      // Will go back to the previous state to show the cursor, handles, the text's popup, ...
      ChangeState( mEventData->mPreviousState );
      break;
    }
    default:
      break;
  }
}

void Controller::Impl::OnLongPressEvent( const Event& event )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::OnLongPressEvent\n" );

  if( EventData::EDITING == mEventData->mState )
  {
    ChangeState ( EventData::EDITING_WITH_POPUP );
    mEventData->mDecoratorUpdated = true;
    mEventData->mUpdateInputStyle = true;
  }
}

void Controller::Impl::OnHandleEvent( const Event& event )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  const unsigned int state = event.p1.mUint;
  const bool handleStopScrolling = ( HANDLE_STOP_SCROLLING == state );
  const bool isSmoothHandlePanEnabled = mEventData->mDecorator->IsSmoothHandlePanEnabled();

  if( HANDLE_PRESSED == state )
  {
    // Convert from decorator's coords to text's coords.
    const float xPosition = event.p2.mFloat - mModel->mScrollPosition.x;
    const float yPosition = event.p3.mFloat - mModel->mScrollPosition.y;

    // Need to calculate the handle's new position.
    const CharacterIndex handleNewPosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                                          mModel->mLogicalModel,
                                                                          mMetrics,
                                                                          xPosition,
                                                                          yPosition );

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::GRAB_HANDLE_PANNING );

      if( handleNewPosition != mEventData->mPrimaryCursorPosition )
      {
        // Updates the cursor position if the handle's new position is different than the current one.
        mEventData->mUpdateCursorPosition = true;
        // Does not update the grab handle position if the smooth panning is enabled. (The decorator does it smooth).
        mEventData->mUpdateGrabHandlePosition = !isSmoothHandlePanEnabled;
        mEventData->mPrimaryCursorPosition = handleNewPosition;
      }

      // Updates the decorator if the soft handle panning is enabled. It triggers a relayout in the decorator and the new position of the handle is set.
      mEventData->mDecoratorUpdated = isSmoothHandlePanEnabled;
    }
    else if( Event::LEFT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::SELECTION_HANDLE_PANNING );

      if( ( handleNewPosition != mEventData->mLeftSelectionPosition ) &&
          ( handleNewPosition != mEventData->mRightSelectionPosition ) )
      {
        // Updates the highlight box if the handle's new position is different than the current one.
        mEventData->mUpdateHighlightBox = true;
        // Does not update the selection handle position if the smooth panning is enabled. (The decorator does it smooth).
        mEventData->mUpdateLeftSelectionPosition = !isSmoothHandlePanEnabled;
        mEventData->mLeftSelectionPosition = handleNewPosition;
      }

      // Updates the decorator if the soft handle panning is enabled. It triggers a relayout in the decorator and the new position of the handle is set.
      mEventData->mDecoratorUpdated = isSmoothHandlePanEnabled;

      // Will define the order to scroll the text to match the handle position.
      mEventData->mIsLeftHandleSelected = true;
    }
    else if( Event::RIGHT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::SELECTION_HANDLE_PANNING );

      if( ( handleNewPosition != mEventData->mRightSelectionPosition ) &&
          ( handleNewPosition != mEventData->mLeftSelectionPosition ) )
      {
        // Updates the highlight box if the handle's new position is different than the current one.
        mEventData->mUpdateHighlightBox = true;
        // Does not update the selection handle position if the smooth panning is enabled. (The decorator does it smooth).
        mEventData->mUpdateRightSelectionPosition = !isSmoothHandlePanEnabled;
        mEventData->mRightSelectionPosition = handleNewPosition;
      }

      // Updates the decorator if the soft handle panning is enabled. It triggers a relayout in the decorator and the new position of the handle is set.
      mEventData->mDecoratorUpdated = isSmoothHandlePanEnabled;

      // Will define the order to scroll the text to match the handle position.
      mEventData->mIsLeftHandleSelected = false;
    }
  } // end ( HANDLE_PRESSED == state )
  else if( ( HANDLE_RELEASED == state ) ||
           handleStopScrolling )
  {
    CharacterIndex handlePosition = 0u;
    if( handleStopScrolling || isSmoothHandlePanEnabled )
    {
      // Convert from decorator's coords to text's coords.
      const float xPosition = event.p2.mFloat - mModel->mScrollPosition.x;
      const float yPosition = event.p3.mFloat - mModel->mScrollPosition.y;

      handlePosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                    mModel->mLogicalModel,
                                                    mMetrics,
                                                    xPosition,
                                                    yPosition );
    }

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      mEventData->mUpdateCursorPosition = true;
      mEventData->mUpdateGrabHandlePosition = true;
      mEventData->mUpdateInputStyle = true;

      if( !IsClipboardEmpty() )
      {
        ChangeState( EventData::EDITING_WITH_PASTE_POPUP ); // Moving grabhandle will show Paste Popup
      }

      if( handleStopScrolling || isSmoothHandlePanEnabled )
      {
        mEventData->mScrollAfterUpdatePosition = true;
        mEventData->mPrimaryCursorPosition = handlePosition;
      }
    }
    else if( Event::LEFT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::SELECTING );

      mEventData->mUpdateHighlightBox = true;
      mEventData->mUpdateLeftSelectionPosition = true;
      mEventData->mUpdateRightSelectionPosition = true;

      if( handleStopScrolling || isSmoothHandlePanEnabled )
      {
        mEventData->mScrollAfterUpdatePosition = true;

        if( ( handlePosition != mEventData->mRightSelectionPosition ) &&
            ( handlePosition != mEventData->mLeftSelectionPosition ) )
        {
          mEventData->mLeftSelectionPosition = handlePosition;
        }
      }
    }
    else if( Event::RIGHT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::SELECTING );

      mEventData->mUpdateHighlightBox = true;
      mEventData->mUpdateRightSelectionPosition = true;
      mEventData->mUpdateLeftSelectionPosition = true;

      if( handleStopScrolling || isSmoothHandlePanEnabled )
      {
        mEventData->mScrollAfterUpdatePosition = true;
        if( ( handlePosition != mEventData->mRightSelectionPosition ) &&
            ( handlePosition != mEventData->mLeftSelectionPosition ) )
        {
          mEventData->mRightSelectionPosition = handlePosition;
        }
      }
    }

    mEventData->mDecoratorUpdated = true;
  } // end ( ( HANDLE_RELEASED == state ) || ( HANDLE_STOP_SCROLLING == state ) )
  else if( HANDLE_SCROLLING == state )
  {
    const float xSpeed = event.p2.mFloat;
    const float ySpeed = event.p3.mFloat;
    const Vector2& layoutSize = mModel->mVisualModel->GetLayoutSize();
    const Vector2 currentScrollPosition = mModel->mScrollPosition;

    mModel->mScrollPosition.x += xSpeed;
    mModel->mScrollPosition.y += ySpeed;

    ClampHorizontalScroll( layoutSize );
    ClampVerticalScroll( layoutSize );

    bool endOfScroll = false;
    if( Vector2::ZERO == ( currentScrollPosition - mModel->mScrollPosition ) )
    {
      // Notify the decorator there is no more text to scroll.
      // The decorator won't send more scroll events.
      mEventData->mDecorator->NotifyEndOfScroll();
      // Still need to set the position of the handle.
      endOfScroll = true;
    }

    // Set the position of the handle.
    const bool scrollRightDirection = xSpeed > 0.f;
    const bool scrollBottomDirection = ySpeed > 0.f;
    const bool leftSelectionHandleEvent = Event::LEFT_SELECTION_HANDLE_EVENT == event.type;
    const bool rightSelectionHandleEvent = Event::RIGHT_SELECTION_HANDLE_EVENT == event.type;

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::GRAB_HANDLE_PANNING );

      // Get the grab handle position in decorator coords.
      Vector2 position = mEventData->mDecorator->GetPosition( GRAB_HANDLE );

      if( mEventData->mDecorator->IsHorizontalScrollEnabled() )
      {
        // Position the grag handle close to either the left or right edge.
        position.x = scrollRightDirection ? 0.f : mModel->mVisualModel->mControlSize.width;
      }

      if( mEventData->mDecorator->IsVerticalScrollEnabled() )
      {
        position.x = mEventData->mCursorHookPositionX;

        // Position the grag handle close to either the top or bottom edge.
        position.y = scrollBottomDirection ? 0.f : mModel->mVisualModel->mControlSize.height;
      }

      // Get the new handle position.
      // The grab handle's position is in decorator's coords. Need to transforms to text's coords.
      const CharacterIndex handlePosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                                         mModel->mLogicalModel,
                                                                         mMetrics,
                                                                         position.x - mModel->mScrollPosition.x,
                                                                         position.y - mModel->mScrollPosition.y );

      if( mEventData->mPrimaryCursorPosition != handlePosition )
      {
        mEventData->mUpdateCursorPosition = true;
        mEventData->mUpdateGrabHandlePosition = !isSmoothHandlePanEnabled;
        mEventData->mScrollAfterUpdatePosition = true;
        mEventData->mPrimaryCursorPosition = handlePosition;
      }
      mEventData->mUpdateInputStyle = mEventData->mUpdateCursorPosition;

      // Updates the decorator if the soft handle panning is enabled.
      mEventData->mDecoratorUpdated = isSmoothHandlePanEnabled;
    }
    else if( leftSelectionHandleEvent || rightSelectionHandleEvent )
    {
      ChangeState( EventData::SELECTION_HANDLE_PANNING );

      // Get the selection handle position in decorator coords.
      Vector2 position = mEventData->mDecorator->GetPosition( leftSelectionHandleEvent ? Text::LEFT_SELECTION_HANDLE : Text::RIGHT_SELECTION_HANDLE );

      if( mEventData->mDecorator->IsHorizontalScrollEnabled() )
      {
        // Position the selection handle close to either the left or right edge.
        position.x = scrollRightDirection ? 0.f : mModel->mVisualModel->mControlSize.width;
      }

      if( mEventData->mDecorator->IsVerticalScrollEnabled() )
      {
        position.x = mEventData->mCursorHookPositionX;

        // Position the grag handle close to either the top or bottom edge.
        position.y = scrollBottomDirection ? 0.f : mModel->mVisualModel->mControlSize.height;
      }

      // Get the new handle position.
      // The selection handle's position is in decorator's coords. Need to transform to text's coords.
      const CharacterIndex handlePosition = Text::GetClosestCursorIndex( mModel->mVisualModel,
                                                                         mModel->mLogicalModel,
                                                                         mMetrics,
                                                                         position.x - mModel->mScrollPosition.x,
                                                                         position.y - mModel->mScrollPosition.y );

      if( leftSelectionHandleEvent )
      {
        const bool differentHandles = ( mEventData->mLeftSelectionPosition != handlePosition ) && ( mEventData->mRightSelectionPosition != handlePosition );

        if( differentHandles || endOfScroll )
        {
          mEventData->mUpdateHighlightBox = true;
          mEventData->mUpdateLeftSelectionPosition = !isSmoothHandlePanEnabled;
          mEventData->mUpdateRightSelectionPosition = isSmoothHandlePanEnabled;
          mEventData->mLeftSelectionPosition = handlePosition;
        }
      }
      else
      {
        const bool differentHandles = ( mEventData->mRightSelectionPosition != handlePosition ) && ( mEventData->mLeftSelectionPosition != handlePosition );
        if( differentHandles || endOfScroll )
        {
          mEventData->mUpdateHighlightBox = true;
          mEventData->mUpdateRightSelectionPosition = !isSmoothHandlePanEnabled;
          mEventData->mUpdateLeftSelectionPosition = isSmoothHandlePanEnabled;
          mEventData->mRightSelectionPosition = handlePosition;
        }
      }

      if( mEventData->mUpdateLeftSelectionPosition || mEventData->mUpdateRightSelectionPosition )
      {
        RepositionSelectionHandles();

        mEventData->mScrollAfterUpdatePosition = !isSmoothHandlePanEnabled;
      }
    }
    mEventData->mDecoratorUpdated = true;
  } // end ( HANDLE_SCROLLING == state )
}

void Controller::Impl::OnSelectEvent( const Event& event )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text.
    return;
  }

  if( mEventData->mSelectionEnabled )
  {
    // Convert from control's coords to text's coords.
    const float xPosition = event.p2.mFloat - mModel->mScrollPosition.x;
    const float yPosition = event.p3.mFloat - mModel->mScrollPosition.y;

    // Calculates the logical position from the x,y coords.
    RepositionSelectionHandles( xPosition,
                                yPosition );
  }
}

void Controller::Impl::OnSelectAllEvent()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "OnSelectAllEvent mEventData->mSelectionEnabled%s \n", mEventData->mSelectionEnabled?"true":"false");

  if( NULL == mEventData )
  {
    // Nothing to do if there is no text.
    return;
  }

  if( mEventData->mSelectionEnabled )
  {
    ChangeState( EventData::SELECTING );

    mEventData->mLeftSelectionPosition = 0u;
    mEventData->mRightSelectionPosition = mModel->mLogicalModel->mText.Count();

    mEventData->mScrollAfterUpdatePosition = true;
    mEventData->mUpdateLeftSelectionPosition = true;
    mEventData->mUpdateRightSelectionPosition = true;
    mEventData->mUpdateHighlightBox = true;
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
      mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;

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

bool Controller::Impl::CopyStringToClipboard( std::string& source )
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
  CharacterIndex selectionStart = mEventData->mLeftSelectionPosition;
  CharacterIndex selectionEnd = mEventData->mRightSelectionPosition;

  if( selectionStart == selectionEnd )
  {
    // Nothing to select if handles are in the same place.
    return;
  }

  mEventData->mDecorator->ClearHighlights();

  const GlyphIndex* const charactersToGlyphBuffer = mModel->mVisualModel->mCharactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = mModel->mVisualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const glyphsBuffer = mModel->mVisualModel->mGlyphs.Begin();
  const Vector2* const positionsBuffer = mModel->mVisualModel->mGlyphPositions.Begin();
  const Length* const charactersPerGlyphBuffer = mModel->mVisualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const glyphToCharacterBuffer = mModel->mVisualModel->mGlyphsToCharacters.Begin();
  const CharacterDirection* const modelCharacterDirectionsBuffer = ( 0u != mModel->mLogicalModel->mCharacterDirections.Count() ) ? mModel->mLogicalModel->mCharacterDirections.Begin() : NULL;

  const bool isLastCharacter = selectionEnd >= mModel->mLogicalModel->mText.Count();
  const CharacterDirection startDirection = ( ( NULL == modelCharacterDirectionsBuffer ) ? false : *( modelCharacterDirectionsBuffer + selectionStart ) );
  const CharacterDirection endDirection = ( ( NULL == modelCharacterDirectionsBuffer ) ? false : *( modelCharacterDirectionsBuffer + ( selectionEnd - ( isLastCharacter ? 1u : 0u ) ) ) );

  // Swap the indices if the start is greater than the end.
  const bool indicesSwapped = selectionStart > selectionEnd;

  // Tell the decorator to flip the selection handles if needed.
  mEventData->mDecorator->SetSelectionHandleFlipState( indicesSwapped, startDirection, endDirection );

  if( indicesSwapped )
  {
    std::swap( selectionStart, selectionEnd );
  }

  // Get the indices to the first and last selected glyphs.
  const CharacterIndex selectionEndMinusOne = selectionEnd - 1u;
  const GlyphIndex glyphStart = *( charactersToGlyphBuffer + selectionStart );
  const Length numberOfGlyphs = *( glyphsPerCharacterBuffer + selectionEndMinusOne );
  const GlyphIndex glyphEnd = *( charactersToGlyphBuffer + selectionEndMinusOne ) + ( ( numberOfGlyphs > 0 ) ? numberOfGlyphs - 1u : 0u );

  // Get the lines where the glyphs are laid-out.
  const LineRun* lineRun = mModel->mVisualModel->mLines.Begin();

  LineIndex lineIndex = 0u;
  Length numberOfLines = 0u;
  mModel->mVisualModel->GetNumberOfLines( glyphStart,
                                          1u + glyphEnd - glyphStart,
                                          lineIndex,
                                          numberOfLines );
  const LineIndex firstLineIndex = lineIndex;

  // Create the structure to store some selection box info.
  Vector<SelectionBoxInfo> selectionBoxLinesInfo;
  selectionBoxLinesInfo.Resize( numberOfLines );

  SelectionBoxInfo* selectionBoxInfo = selectionBoxLinesInfo.Begin();
  selectionBoxInfo->minX = MAX_FLOAT;
  selectionBoxInfo->maxX = MIN_FLOAT;

  // Keep the min and max 'x' position to calculate the size and position of the highlighed text.
  float minHighlightX = std::numeric_limits<float>::max();
  float maxHighlightX = std::numeric_limits<float>::min();
  Size highLightSize;
  Vector2 highLightPosition; // The highlight position in decorator's coords.

  // Retrieve the first line and get the line's vertical offset, the line's height and the index to the last glyph.

  // The line's vertical offset of all the lines before the line where the first glyph is laid-out.
  selectionBoxInfo->lineOffset = CalculateLineOffset( mModel->mVisualModel->mLines,
                                                      firstLineIndex );

  // Transform to decorator's (control) coords.
  selectionBoxInfo->lineOffset += mModel->mScrollPosition.y;

  lineRun += firstLineIndex;

  // The line height is the addition of the line ascender and the line descender.
  // However, the line descender has a negative value, hence the subtraction.
  selectionBoxInfo->lineHeight = lineRun->ascender - lineRun->descender;

  GlyphIndex lastGlyphOfLine = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;

  // Check if the first glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special code.
  const Length numberOfCharactersStart = *( charactersPerGlyphBuffer + glyphStart );
  bool splitStartGlyph = ( numberOfCharactersStart > 1u ) && HasLigatureMustBreak( mModel->mLogicalModel->GetScript( selectionStart ) );

  // Check if the last glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special code.
  const Length numberOfCharactersEnd = *( charactersPerGlyphBuffer + glyphEnd );
  bool splitEndGlyph = ( glyphStart != glyphEnd ) && ( numberOfCharactersEnd > 1u ) && HasLigatureMustBreak( mModel->mLogicalModel->GetScript( selectionEndMinusOne ) );

  // The number of quads of the selection box.
  const unsigned int numberOfQuads = 1u + ( glyphEnd - glyphStart ) + ( ( numberOfLines > 1u ) ? 2u * numberOfLines : 0u );
  mEventData->mDecorator->ResizeHighlightQuads( numberOfQuads );

  // Count the actual number of quads.
  unsigned int actualNumberOfQuads = 0u;
  Vector4 quad;

  // Traverse the glyphs.
  for( GlyphIndex index = glyphStart; index <= glyphEnd; ++index )
  {
    const GlyphInfo& glyph = *( glyphsBuffer + index );
    const Vector2& position = *( positionsBuffer + index );

    if( splitStartGlyph )
    {
      // If the first glyph is a ligature that must be broken it may be needed to add only part of the glyph to the highlight box.

      const float glyphAdvance = glyph.advance / static_cast<float>( numberOfCharactersStart );
      const CharacterIndex interGlyphIndex = selectionStart - *( glyphToCharacterBuffer + glyphStart );
      // Get the direction of the character.
      CharacterDirection isCurrentRightToLeft = false;
      if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
      {
        isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + selectionStart );
      }

      // The end point could be in the middle of the ligature.
      // Calculate the number of characters selected.
      const Length numberOfCharacters = ( glyphStart == glyphEnd ) ? ( selectionEnd - selectionStart ) : ( numberOfCharactersStart - interGlyphIndex );

      quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + mModel->mScrollPosition.x + glyphAdvance * static_cast<float>( isCurrentRightToLeft ? ( numberOfCharactersStart - interGlyphIndex - numberOfCharacters ) : interGlyphIndex );
      quad.y = selectionBoxInfo->lineOffset;
      quad.z = quad.x + static_cast<float>( numberOfCharacters ) * glyphAdvance;
      quad.w = selectionBoxInfo->lineOffset + selectionBoxInfo->lineHeight;

      // Store the min and max 'x' for each line.
      selectionBoxInfo->minX = std::min( selectionBoxInfo->minX, quad.x );
      selectionBoxInfo->maxX = std::max( selectionBoxInfo->maxX, quad.z );

      mEventData->mDecorator->AddHighlight( actualNumberOfQuads, quad );
      ++actualNumberOfQuads;

      splitStartGlyph = false;
      continue;
    }

    if( splitEndGlyph && ( index == glyphEnd ) )
    {
      // Equally, if the last glyph is a ligature that must be broken it may be needed to add only part of the glyph to the highlight box.

      const float glyphAdvance = glyph.advance / static_cast<float>( numberOfCharactersEnd );
      const CharacterIndex interGlyphIndex = selectionEnd - *( glyphToCharacterBuffer + glyphEnd );
      // Get the direction of the character.
      CharacterDirection isCurrentRightToLeft = false;
      if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
      {
        isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + selectionEnd );
      }

      const Length numberOfCharacters = numberOfCharactersEnd - interGlyphIndex;

      quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + mModel->mScrollPosition.x + ( isCurrentRightToLeft ? ( glyphAdvance * static_cast<float>( numberOfCharacters ) ) : 0.f );
      quad.y = selectionBoxInfo->lineOffset;
      quad.z = quad.x + static_cast<float>( interGlyphIndex ) * glyphAdvance;
      quad.w = quad.y + selectionBoxInfo->lineHeight;

      // Store the min and max 'x' for each line.
      selectionBoxInfo->minX = std::min( selectionBoxInfo->minX, quad.x );
      selectionBoxInfo->maxX = std::max( selectionBoxInfo->maxX, quad.z );

      mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                            quad );
      ++actualNumberOfQuads;

      splitEndGlyph = false;
      continue;
    }

    quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + mModel->mScrollPosition.x;
    quad.y = selectionBoxInfo->lineOffset;
    quad.z = quad.x + glyph.advance;
    quad.w = quad.y + selectionBoxInfo->lineHeight;

    // Store the min and max 'x' for each line.
    selectionBoxInfo->minX = std::min( selectionBoxInfo->minX, quad.x );
    selectionBoxInfo->maxX = std::max( selectionBoxInfo->maxX, quad.z );

    mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                          quad );
    ++actualNumberOfQuads;

    // Whether to retrieve the next line.
    if( index == lastGlyphOfLine )
    {
      ++lineIndex;
      if( lineIndex < firstLineIndex + numberOfLines )
      {
        // Retrieve the next line.
        ++lineRun;

        // Get the last glyph of the new line.
        lastGlyphOfLine = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;

        // Keep the offset and height of the current selection box.
        const float currentLineOffset = selectionBoxInfo->lineOffset;
        const float currentLineHeight = selectionBoxInfo->lineHeight;

        // Get the selection box info for the next line.
        ++selectionBoxInfo;

        selectionBoxInfo->minX = MAX_FLOAT;
        selectionBoxInfo->maxX = MIN_FLOAT;

        // Update the line's vertical offset.
        selectionBoxInfo->lineOffset = currentLineOffset + currentLineHeight;

        // The line height is the addition of the line ascender and the line descender.
        // However, the line descender has a negative value, hence the subtraction.
        selectionBoxInfo->lineHeight = lineRun->ascender - lineRun->descender;
      }
    }
  }

  // Traverses all the lines and updates the min and max 'x' positions and the total height.
  // The final width is calculated after 'boxifying' the selection.
  for( Vector<SelectionBoxInfo>::ConstIterator it = selectionBoxLinesInfo.Begin(),
         endIt = selectionBoxLinesInfo.End();
       it != endIt;
       ++it )
  {
    const SelectionBoxInfo& info = *it;

    // Update the size of the highlighted text.
    highLightSize.height += info.lineHeight;
    minHighlightX = std::min( minHighlightX, info.minX );
    maxHighlightX = std::max( maxHighlightX, info.maxX );
  }

  // Add extra geometry to 'boxify' the selection.

  if( 1u < numberOfLines )
  {
    // Boxify the first line.
    lineRun = mModel->mVisualModel->mLines.Begin() + firstLineIndex;
    const SelectionBoxInfo& firstSelectionBoxLineInfo = *( selectionBoxLinesInfo.Begin() );

    bool boxifyBegin = ( LTR != lineRun->direction ) && ( LTR != startDirection );
    bool boxifyEnd = ( LTR == lineRun->direction ) && ( LTR == startDirection );

    if( boxifyBegin )
    {
      quad.x = 0.f;
      quad.y = firstSelectionBoxLineInfo.lineOffset;
      quad.z = firstSelectionBoxLineInfo.minX;
      quad.w = firstSelectionBoxLineInfo.lineOffset + firstSelectionBoxLineInfo.lineHeight;

      // Boxify at the beginning of the line.
      mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                            quad );
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
    }

    if( boxifyEnd )
    {
      quad.x = firstSelectionBoxLineInfo.maxX;
      quad.y = firstSelectionBoxLineInfo.lineOffset;
      quad.z = mModel->mVisualModel->mControlSize.width;
      quad.w = firstSelectionBoxLineInfo.lineOffset + firstSelectionBoxLineInfo.lineHeight;

      // Boxify at the end of the line.
      mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                            quad );
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      maxHighlightX = mModel->mVisualModel->mControlSize.width;
    }

    // Boxify the central lines.
    if( 2u < numberOfLines )
    {
      for( Vector<SelectionBoxInfo>::ConstIterator it = selectionBoxLinesInfo.Begin() + 1u,
             endIt = selectionBoxLinesInfo.End() - 1u;
           it != endIt;
           ++it )
      {
        const SelectionBoxInfo& info = *it;

        quad.x = 0.f;
        quad.y = info.lineOffset;
        quad.z = info.minX;
        quad.w = info.lineOffset + info.lineHeight;

        mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                              quad );
        ++actualNumberOfQuads;

        quad.x = info.maxX;
        quad.y = info.lineOffset;
        quad.z = mModel->mVisualModel->mControlSize.width;
        quad.w = info.lineOffset + info.lineHeight;

        mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                              quad );
        ++actualNumberOfQuads;
      }

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
      maxHighlightX = mModel->mVisualModel->mControlSize.width;
    }

    // Boxify the last line.
    lineRun = mModel->mVisualModel->mLines.Begin() + firstLineIndex + numberOfLines - 1u;
    const SelectionBoxInfo& lastSelectionBoxLineInfo = *( selectionBoxLinesInfo.End() - 1u );

    boxifyBegin = ( LTR == lineRun->direction ) && ( LTR == endDirection );
    boxifyEnd = ( LTR != lineRun->direction ) && ( LTR != endDirection );

    if( boxifyBegin )
    {
      quad.x = 0.f;
      quad.y = lastSelectionBoxLineInfo.lineOffset;
      quad.z = lastSelectionBoxLineInfo.minX;
      quad.w = lastSelectionBoxLineInfo.lineOffset + lastSelectionBoxLineInfo.lineHeight;

      // Boxify at the beginning of the line.
      mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                            quad );
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
    }

    if( boxifyEnd )
    {
      quad.x = lastSelectionBoxLineInfo.maxX;
      quad.y = lastSelectionBoxLineInfo.lineOffset;
      quad.z = mModel->mVisualModel->mControlSize.width;
      quad.w = lastSelectionBoxLineInfo.lineOffset + lastSelectionBoxLineInfo.lineHeight;

      // Boxify at the end of the line.
      mEventData->mDecorator->AddHighlight( actualNumberOfQuads,
                                            quad );
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      maxHighlightX = mModel->mVisualModel->mControlSize.width;
    }
  }

  // Set the actual number of quads.
  mEventData->mDecorator->ResizeHighlightQuads( actualNumberOfQuads );

  // Sets the highlight's size and position. In decorator's coords.
  // The highlight's height has been calculated above (before 'boxifying' the highlight).
  highLightSize.width = maxHighlightX - minHighlightX;

  highLightPosition.x = minHighlightX;
  const SelectionBoxInfo& firstSelectionBoxLineInfo = *( selectionBoxLinesInfo.Begin() );
  highLightPosition.y = firstSelectionBoxLineInfo.lineOffset;

  mEventData->mDecorator->SetHighLightBox( highLightPosition, highLightSize );

  if( !mEventData->mDecorator->IsSmoothHandlePanEnabled() )
  {
    CursorInfo primaryCursorInfo;
    GetCursorPosition( mEventData->mLeftSelectionPosition,
                       primaryCursorInfo );

    const Vector2 primaryPosition = primaryCursorInfo.primaryPosition + mModel->mScrollPosition;

    mEventData->mDecorator->SetPosition( LEFT_SELECTION_HANDLE,
                                         primaryPosition.x,
                                         primaryCursorInfo.lineOffset + mModel->mScrollPosition.y,
                                         primaryCursorInfo.lineHeight );

    CursorInfo secondaryCursorInfo;
    GetCursorPosition( mEventData->mRightSelectionPosition,
                       secondaryCursorInfo );

    const Vector2 secondaryPosition = secondaryCursorInfo.primaryPosition + mModel->mScrollPosition;

    mEventData->mDecorator->SetPosition( RIGHT_SELECTION_HANDLE,
                                         secondaryPosition.x,
                                         secondaryCursorInfo.lineOffset + mModel->mScrollPosition.y,
                                         secondaryCursorInfo.lineHeight );
  }

  // Cursor to be positioned at end of selection so if selection interrupted and edit mode restarted the cursor will be at end of selection
  mEventData->mPrimaryCursorPosition = ( indicesSwapped ) ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition;

  // Set the flag to update the decorator.
  mEventData->mDecoratorUpdated = true;
}

void Controller::Impl::RepositionSelectionHandles( float visualX, float visualY )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  if( IsShowingPlaceholderText() )
  {
    // Nothing to do if there is the place-holder text.
    return;
  }

  const Length numberOfGlyphs = mModel->mVisualModel->mGlyphs.Count();
  const Length numberOfLines  = mModel->mVisualModel->mLines.Count();
  if( ( 0 == numberOfGlyphs ) ||
      ( 0 == numberOfLines ) )
  {
    // Nothing to do if there is no text.
    return;
  }

  // Find which word was selected
  CharacterIndex selectionStart( 0 );
  CharacterIndex selectionEnd( 0 );
  const bool indicesFound = FindSelectionIndices( mModel->mVisualModel,
                                                  mModel->mLogicalModel,
                                                  mMetrics,
                                                  visualX,
                                                  visualY,
                                                  selectionStart,
                                                  selectionEnd );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "%p selectionStart %d selectionEnd %d\n", this, selectionStart, selectionEnd );

  if( indicesFound )
  {
    ChangeState( EventData::SELECTING );

    mEventData->mLeftSelectionPosition = selectionStart;
    mEventData->mRightSelectionPosition = selectionEnd;

    mEventData->mUpdateLeftSelectionPosition = true;
    mEventData->mUpdateRightSelectionPosition = true;
    mEventData->mUpdateHighlightBox = true;

    // It may happen an IMF commit event arrives before the selection event
    // if the IMF manager is in pre-edit state. The commit event will set the
    // mEventData->mUpdateCursorPosition flag to true. If it's not set back
    // to false, the highlight box won't be updated.
    mEventData->mUpdateCursorPosition = false;

    mEventData->mScrollAfterUpdatePosition = ( mEventData->mLeftSelectionPosition != mEventData->mRightSelectionPosition );
  }
  else
  {
    // Nothing to select. i.e. a white space, out of bounds
    ChangeState( EventData::EDITING );

    mEventData->mPrimaryCursorPosition = selectionEnd;

    mEventData->mUpdateCursorPosition = true;
    mEventData->mUpdateGrabHandlePosition = true;
    mEventData->mScrollAfterUpdatePosition = true;
    mEventData->mUpdateInputStyle = true;
  }
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

  TextSelectionPopup::Buttons buttonsToShow = TextSelectionPopup::NONE;

  if( EventData::SELECTING == mEventData->mState )
  {
    buttonsToShow = TextSelectionPopup::Buttons(  TextSelectionPopup::CUT | TextSelectionPopup::COPY );

    if( !IsClipboardEmpty() )
    {
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
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
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::CLIPBOARD ) );
    }
  }
  else if( EventData::EDITING_WITH_PASTE_POPUP == mEventData->mState )
  {
    if ( !IsClipboardEmpty() )
    {
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
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
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
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
        else
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
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
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
        mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
        mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
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
        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
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

        mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
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

    switch( mModel->mHorizontalAlignment )
    {
      case Layout::HORIZONTAL_ALIGN_BEGIN:
      {
        cursorInfo.primaryPosition.x = 0.f;
        break;
      }
      case Layout::HORIZONTAL_ALIGN_CENTER:
      {
        cursorInfo.primaryPosition.x = floorf( 0.5f * mModel->mVisualModel->mControlSize.width );
        break;
      }
      case Layout::HORIZONTAL_ALIGN_END:
      {
        cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetCursorWidth();
        break;
      }
    }

    // Nothing else to do.
    return;
  }

  Text::GetCursorPosition( mModel->mVisualModel,
                           mModel->mLogicalModel,
                           mMetrics,
                           logical,
                           cursorInfo );

  if( Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout() )
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
  if( ( LEFT_SELECTION_HANDLE != handleType ) &&
      ( RIGHT_SELECTION_HANDLE != handleType ) )
  {
    return;
  }

  const Vector2 cursorPosition = cursorInfo.primaryPosition + mModel->mScrollPosition;

  // Sets the handle's position.
  mEventData->mDecorator->SetPosition( handleType,
                                       cursorPosition.x,
                                       cursorInfo.lineOffset + mModel->mScrollPosition.y,
                                       cursorInfo.lineHeight );

  // If selection handle at start of the text and other at end of the text then all text is selected.
  const CharacterIndex startOfSelection = std::min( mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition );
  const CharacterIndex endOfSelection = std::max ( mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition );
  mEventData->mAllTextSelected = ( startOfSelection == 0 ) && ( endOfSelection == mModel->mLogicalModel->mText.Count() );
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

  if( decoratorPositionBeginY < 0.f )
  {
    mModel->mScrollPosition.y = -position.y;
  }
  else if( decoratorPositionEndY > mModel->mVisualModel->mControlSize.height )
  {
    mModel->mScrollPosition.y = mModel->mVisualModel->mControlSize.height - positionEndY;
  }
}

void Controller::Impl::ScrollTextToMatchCursor( const CursorInfo& cursorInfo )
{
  // Get the current cursor position in decorator coords.
  const Vector2& currentCursorPosition = mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

  // Calculate the offset to match the cursor position before the character was deleted.
  mModel->mScrollPosition.x = currentCursorPosition.x - cursorInfo.primaryPosition.x;
  mModel->mScrollPosition.y = currentCursorPosition.y - cursorInfo.lineOffset;

  ClampHorizontalScroll( mModel->mVisualModel->GetLayoutSize() );
  ClampVerticalScroll( mModel->mVisualModel->GetLayoutSize() );

  // Makes the new cursor position visible if needed.
  ScrollToMakePositionVisible( cursorInfo.primaryPosition, cursorInfo.lineHeight );
}

void Controller::Impl::RequestRelayout()
{
  if( NULL != mControlInterface )
  {
    mControlInterface->RequestTextRelayout();
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
