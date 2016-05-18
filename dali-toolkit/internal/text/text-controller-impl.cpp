/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/color-segmentation.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-run-container.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

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
  mScrollPosition(),
  mState( INACTIVE ),
  mPrimaryCursorPosition( 0u ),
  mLeftSelectionPosition( 0u ),
  mRightSelectionPosition( 0u ),
  mPreEditStartPosition( 0u ),
  mPreEditLength( 0u ),
  mIsShowingPlaceholderText( false ),
  mPreEditFlag( false ),
  mDecoratorUpdated( false ),
  mCursorBlinkEnabled( true ),
  mGrabHandleEnabled( true ),
  mGrabHandlePopupEnabled( true ),
  mSelectionEnabled( true ),
  mHorizontalScrollingEnabled( true ),
  mVerticalScrollingEnabled( false ),
  mUpdateCursorPosition( false ),
  mUpdateLeftSelectionPosition( false ),
  mUpdateRightSelectionPosition( false ),
  mScrollAfterUpdatePosition( false ),
  mScrollAfterDelete( false ),
  mAllTextSelected( false ),
  mUpdateInputStyle( false )
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

  // The cursor must also be repositioned after inserts into the model
  if( mEventData->mUpdateCursorPosition )
  {
    // Updates the cursor position and scrolls the text to make it visible.
    CursorInfo cursorInfo;
    GetCursorPosition( mEventData->mPrimaryCursorPosition,
                       cursorInfo );

    if( mEventData->mScrollAfterUpdatePosition )
    {
      ScrollToMakePositionVisible( cursorInfo.primaryPosition );
      mEventData->mScrollAfterUpdatePosition = false;
    }
    else if( mEventData->mScrollAfterDelete )
    {
      ScrollTextToMatchCursor( cursorInfo );
      mEventData->mScrollAfterDelete = false;
    }

    UpdateCursorPosition( cursorInfo );

    mEventData->mDecoratorUpdated = true;
    mEventData->mUpdateCursorPosition = false;
  }
  else
  {
    bool leftScroll = false;
    bool rightScroll = false;

    CursorInfo leftHandleInfo;
    CursorInfo rightHandleInfo;

    if( mEventData->mUpdateLeftSelectionPosition )
    {
      GetCursorPosition( mEventData->mLeftSelectionPosition,
                         leftHandleInfo );

      if( mEventData->mScrollAfterUpdatePosition )
      {
        ScrollToMakePositionVisible( leftHandleInfo.primaryPosition );
        leftScroll = true;
      }
    }

    if( mEventData->mUpdateRightSelectionPosition )
    {
      GetCursorPosition( mEventData->mRightSelectionPosition,
                         rightHandleInfo );

      if( mEventData->mScrollAfterUpdatePosition )
      {
        ScrollToMakePositionVisible( rightHandleInfo.primaryPosition );
        rightScroll = true;
      }
    }

    if( mEventData->mUpdateLeftSelectionPosition )
    {
      UpdateSelectionHandle( LEFT_SELECTION_HANDLE,
                             leftHandleInfo );

      SetPopupButtons();
      mEventData->mDecoratorUpdated = true;
    }

    if( mEventData->mUpdateRightSelectionPosition )
    {
      UpdateSelectionHandle( RIGHT_SELECTION_HANDLE,
                             rightHandleInfo );

      SetPopupButtons();
      mEventData->mDecoratorUpdated = true;
    }

    if( mEventData->mUpdateLeftSelectionPosition || mEventData->mUpdateRightSelectionPosition )
    {
      RepositionSelectionHandles();

      mEventData->mUpdateLeftSelectionPosition = false;
      mEventData->mUpdateRightSelectionPosition = false;
    }

    if( leftScroll || rightScroll )
    {
      mEventData->mScrollAfterUpdatePosition = false;
    }
  }

  if( mEventData->mUpdateInputStyle )
  {
    // Set the default style first.
    RetrieveDefaultInputStyle( mEventData->mInputStyle );

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = ( mEventData->mPrimaryCursorPosition > 0u ) ? mEventData->mPrimaryCursorPosition - 1u : 0u;

    // Retrieve the style from the style runs stored in the logical model.
    mLogicalModel->RetrieveStyle( styleIndex, mEventData->mInputStyle );

    mEventData->mUpdateInputStyle = false;
  }

  mEventData->mEventQueue.clear();

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::ProcessInputEvents\n" );

  const bool decoratorUpdated = mEventData->mDecoratorUpdated;
  mEventData->mDecoratorUpdated = false;

  return decoratorUpdated;
}

void Controller::Impl::CalculateTextUpdateIndices( Length& numberOfCharacters )
{
  mTextUpdateInfo.mParagraphCharacterIndex = 0u;
  mTextUpdateInfo.mStartGlyphIndex = 0u;
  mTextUpdateInfo.mStartLineIndex = 0u;
  numberOfCharacters = 0u;

  const Length numberOfParagraphs = mLogicalModel->mParagraphInfo.Count();
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

      mTextUpdateInfo.mStartGlyphIndex = mVisualModel->mGlyphs.Count();
      mTextUpdateInfo.mStartLineIndex = mVisualModel->mLines.Count() - 1u;

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
    mLogicalModel->FindParagraphs( mTextUpdateInfo.mCharacterIndex,
                                   numberOfCharactersToUpdate,
                                   paragraphsToBeUpdated );
  }

  if( 0u != paragraphsToBeUpdated.Count() )
  {
    const ParagraphRunIndex firstParagraphIndex = *( paragraphsToBeUpdated.Begin() );
    const ParagraphRun& firstParagraph = *( mLogicalModel->mParagraphInfo.Begin() + firstParagraphIndex );
    mTextUpdateInfo.mParagraphCharacterIndex = firstParagraph.characterRun.characterIndex;

    ParagraphRunIndex lastParagraphIndex = *( paragraphsToBeUpdated.End() - 1u );
    const ParagraphRun& lastParagraph = *( mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex );

    if( ( mTextUpdateInfo.mNumberOfCharactersToRemove > 0u ) &&                                            // Some character are removed.
        ( lastParagraphIndex < numberOfParagraphs - 1u ) &&                                                // There is a next paragraph.
        ( ( lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters ) == // The last removed character is the new paragraph character.
          ( mTextUpdateInfo.mCharacterIndex + mTextUpdateInfo.mNumberOfCharactersToRemove ) ) )
    {
      // The new paragraph character of the last updated paragraph has been removed so is going to be merged with the next one.
      const ParagraphRun& lastParagraph = *( mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex + 1u );

      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters - mTextUpdateInfo.mParagraphCharacterIndex;
    }
    else
    {
      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters - mTextUpdateInfo.mParagraphCharacterIndex;
    }
  }

  mTextUpdateInfo.mRequestedNumberOfCharacters = numberOfCharacters + mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;
  mTextUpdateInfo.mStartGlyphIndex = *( mVisualModel->mCharactersToGlyph.Begin() + mTextUpdateInfo.mParagraphCharacterIndex );
}

void Controller::Impl::ClearFullModelData( OperationsMask operations )
{
  if( NO_OPERATION != ( GET_LINE_BREAKS & operations ) )
  {
    mLogicalModel->mLineBreakInfo.Clear();
    mLogicalModel->mParagraphInfo.Clear();
  }

  if( NO_OPERATION != ( GET_WORD_BREAKS & operations ) )
  {
    mLogicalModel->mLineBreakInfo.Clear();
  }

  if( NO_OPERATION != ( GET_SCRIPTS & operations ) )
  {
    mLogicalModel->mScriptRuns.Clear();
  }

  if( NO_OPERATION != ( VALIDATE_FONTS & operations ) )
  {
    mLogicalModel->mFontRuns.Clear();
  }

  if( 0u != mLogicalModel->mBidirectionalParagraphInfo.Count() )
  {
    if( NO_OPERATION != ( BIDI_INFO & operations ) )
    {
      mLogicalModel->mBidirectionalParagraphInfo.Clear();
      mLogicalModel->mCharacterDirections.Clear();
    }

    if( NO_OPERATION != ( REORDER & operations ) )
    {
      // Free the allocated memory used to store the conversion table in the bidirectional line info run.
      for( Vector<BidirectionalLineInfoRun>::Iterator it = mLogicalModel->mBidirectionalLineInfo.Begin(),
             endIt = mLogicalModel->mBidirectionalLineInfo.End();
           it != endIt;
           ++it )
      {
        BidirectionalLineInfoRun& bidiLineInfo = *it;

        free( bidiLineInfo.visualToLogicalMap );
        bidiLineInfo.visualToLogicalMap = NULL;
      }
      mLogicalModel->mBidirectionalLineInfo.Clear();
    }
  }

  if( NO_OPERATION != ( SHAPE_TEXT & operations ) )
  {
    mVisualModel->mGlyphs.Clear();
    mVisualModel->mGlyphsToCharacters.Clear();
    mVisualModel->mCharactersToGlyph.Clear();
    mVisualModel->mCharactersPerGlyph.Clear();
    mVisualModel->mGlyphsPerCharacter.Clear();
    mVisualModel->mGlyphPositions.Clear();
  }

  if( NO_OPERATION != ( LAYOUT & operations ) )
  {
    mVisualModel->mLines.Clear();
  }

  if( NO_OPERATION != ( COLOR & operations ) )
  {
    mVisualModel->mColorIndices.Clear();
  }
}

void Controller::Impl::ClearCharacterModelData( CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations )
{
  const CharacterIndex endIndexPlusOne = endIndex + 1u;

  if( NO_OPERATION != ( GET_LINE_BREAKS & operations ) )
  {
    // Clear the line break info.
    LineBreakInfo* lineBreakInfoBuffer = mLogicalModel->mLineBreakInfo.Begin();

    mLogicalModel->mLineBreakInfo.Erase( lineBreakInfoBuffer + startIndex,
                                         lineBreakInfoBuffer + endIndexPlusOne );

    // Clear the paragraphs.
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mLogicalModel->mParagraphInfo );
  }

  if( NO_OPERATION != ( GET_WORD_BREAKS & operations ) )
  {
    // Clear the word break info.
    WordBreakInfo* wordBreakInfoBuffer = mLogicalModel->mWordBreakInfo.Begin();

    mLogicalModel->mWordBreakInfo.Erase( wordBreakInfoBuffer + startIndex,
                                         wordBreakInfoBuffer + endIndexPlusOne );
  }

  if( NO_OPERATION != ( GET_SCRIPTS & operations ) )
  {
    // Clear the scripts.
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mLogicalModel->mScriptRuns );
  }

  if( NO_OPERATION != ( VALIDATE_FONTS & operations ) )
  {
    // Clear the fonts.
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mLogicalModel->mFontRuns );
  }

  if( 0u != mLogicalModel->mBidirectionalParagraphInfo.Count() )
  {
    if( NO_OPERATION != ( BIDI_INFO & operations ) )
    {
      // Clear the bidirectional paragraph info.
      ClearCharacterRuns( startIndex,
                          endIndex,
                          mLogicalModel->mBidirectionalParagraphInfo );

      // Clear the character's directions.
      CharacterDirection* characterDirectionsBuffer = mLogicalModel->mCharacterDirections.Begin();

      mLogicalModel->mCharacterDirections.Erase( characterDirectionsBuffer + startIndex,
                                                 characterDirectionsBuffer + endIndexPlusOne );
    }

    if( NO_OPERATION != ( REORDER & operations ) )
    {
      uint32_t startRemoveIndex = mLogicalModel->mBidirectionalLineInfo.Count();
      uint32_t endRemoveIndex = startRemoveIndex;
      ClearCharacterRuns( startIndex,
                          endIndex,
                          mLogicalModel->mBidirectionalLineInfo,
                          startRemoveIndex,
                          endRemoveIndex );

      BidirectionalLineInfoRun* bidirectionalLineInfoBuffer = mLogicalModel->mBidirectionalLineInfo.Begin();

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

      mLogicalModel->mBidirectionalLineInfo.Erase( bidirectionalLineInfoBuffer + startRemoveIndex,
                                                   bidirectionalLineInfoBuffer + endRemoveIndex );
    }
  }
}

void Controller::Impl::ClearGlyphModelData( CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations )
{
  const CharacterIndex endIndexPlusOne = endIndex + 1u;
  const Length numberOfCharactersRemoved = endIndexPlusOne - startIndex;

  // Convert the character index to glyph index before deleting the character to glyph and the glyphs per character buffers.
  GlyphIndex* charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
  Length* glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();

  const GlyphIndex endGlyphIndexPlusOne = *( charactersToGlyphBuffer + endIndex ) + *( glyphsPerCharacterBuffer + endIndex );
  const Length numberOfGlyphsRemoved = endGlyphIndexPlusOne - mTextUpdateInfo.mStartGlyphIndex;

  if( NO_OPERATION != ( SHAPE_TEXT & operations ) )
  {
    // Update the character to glyph indices.
    for( Vector<GlyphIndex>::Iterator it =  charactersToGlyphBuffer + endIndexPlusOne,
           endIt =  charactersToGlyphBuffer + mVisualModel->mCharactersToGlyph.Count();
         it != endIt;
         ++it )
    {
      CharacterIndex& index = *it;
      index -= numberOfGlyphsRemoved;
    }

    // Clear the character to glyph conversion table.
    mVisualModel->mCharactersToGlyph.Erase( charactersToGlyphBuffer + startIndex,
                                            charactersToGlyphBuffer + endIndexPlusOne );

    // Clear the glyphs per character table.
    mVisualModel->mGlyphsPerCharacter.Erase( glyphsPerCharacterBuffer + startIndex,
                                             glyphsPerCharacterBuffer + endIndexPlusOne );

    // Clear the glyphs buffer.
    GlyphInfo* glyphsBuffer = mVisualModel->mGlyphs.Begin();
    mVisualModel->mGlyphs.Erase( glyphsBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                 glyphsBuffer + endGlyphIndexPlusOne );

    CharacterIndex* glyphsToCharactersBuffer = mVisualModel->mGlyphsToCharacters.Begin();

    // Update the glyph to character indices.
    for( Vector<CharacterIndex>::Iterator it = glyphsToCharactersBuffer + endGlyphIndexPlusOne,
           endIt = glyphsToCharactersBuffer + mVisualModel->mGlyphsToCharacters.Count();
         it != endIt;
         ++it )
    {
      CharacterIndex& index = *it;
      index -= numberOfCharactersRemoved;
    }

    // Clear the glyphs to characters buffer.
    mVisualModel->mGlyphsToCharacters.Erase( glyphsToCharactersBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                             glyphsToCharactersBuffer  + endGlyphIndexPlusOne );

    // Clear the characters per glyph buffer.
    Length* charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();
    mVisualModel->mCharactersPerGlyph.Erase( charactersPerGlyphBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                             charactersPerGlyphBuffer + endGlyphIndexPlusOne );

    // Clear the positions buffer.
    Vector2* positionsBuffer = mVisualModel->mGlyphPositions.Begin();
    mVisualModel->mGlyphPositions.Erase( positionsBuffer + mTextUpdateInfo.mStartGlyphIndex,
                                         positionsBuffer + endGlyphIndexPlusOne );
  }

  if( NO_OPERATION != ( LAYOUT & operations ) )
  {
    // Clear the lines.
    uint32_t startRemoveIndex = mVisualModel->mLines.Count();
    uint32_t endRemoveIndex = startRemoveIndex;
    ClearCharacterRuns( startIndex,
                        endIndex,
                        mVisualModel->mLines,
                        startRemoveIndex,
                        endRemoveIndex );

    // Will update the glyph runs.
    startRemoveIndex = mVisualModel->mLines.Count();
    endRemoveIndex = startRemoveIndex;
    ClearGlyphRuns( mTextUpdateInfo.mStartGlyphIndex,
                    endGlyphIndexPlusOne - 1u,
                    mVisualModel->mLines,
                    startRemoveIndex,
                    endRemoveIndex );

    // Set the line index from where to insert the new laid-out lines.
    mTextUpdateInfo.mStartLineIndex = startRemoveIndex;

    LineRun* linesBuffer = mVisualModel->mLines.Begin();
    mVisualModel->mLines.Erase( linesBuffer + startRemoveIndex,
                                linesBuffer + endRemoveIndex );
  }

  if( NO_OPERATION != ( COLOR & operations ) )
  {
    if( 0u != mVisualModel->mColorIndices.Count() )
    {
      ColorIndex* colorIndexBuffer = mVisualModel->mColorIndices.Begin();
      mVisualModel->mColorIndices.Erase( colorIndexBuffer + mTextUpdateInfo.mStartGlyphIndex,
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
  mTextUpdateInfo.mEstimatedNumberOfLines = std::max( mVisualModel->mLines.Count(), mLogicalModel->mParagraphInfo.Count() );

  mVisualModel->ClearCaches();
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

  Vector<Character>& utf32Characters = mLogicalModel->mText;

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

  Vector<LineBreakInfo>& lineBreakInfo = mLogicalModel->mLineBreakInfo;
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
    mLogicalModel->CreateParagraphInfo( startIndex,
                                        requestedNumberOfCharacters );
    updated = true;
  }

  Vector<WordBreakInfo>& wordBreakInfo = mLogicalModel->mWordBreakInfo;
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

  Vector<ScriptRun>& scripts = mLogicalModel->mScriptRuns;
  Vector<FontRun>& validFonts = mLogicalModel->mFontRuns;

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
      Vector<FontDescriptionRun>& fontDescriptionRuns = mLogicalModel->mFontDescriptionRuns;

      // Get the default font id.
      const FontId defaultFontId = ( NULL == mFontDefaults ) ? 0u : mFontDefaults->GetFontId( mFontClient );

      // Validates the fonts. If there is a character with no assigned font it sets a default one.
      // After this call, fonts are validated.
      multilanguageSupport.ValidateFonts( utf32Characters,
                                          scripts,
                                          fontDescriptionRuns,
                                          defaultFontId,
                                          startIndex,
                                          requestedNumberOfCharacters,
                                          validFonts );
    }
    updated = true;
  }

  Vector<Character> mirroredUtf32Characters;
  bool textMirrored = false;
  const Length numberOfParagraphs = mLogicalModel->mParagraphInfo.Count();
  if( NO_OPERATION != ( BIDI_INFO & operations ) )
  {
    Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mLogicalModel->mBidirectionalParagraphInfo;
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
      Vector<CharacterDirection>& directions = mLogicalModel->mCharacterDirections;
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
      mLogicalModel->mCharacterDirections.Clear();
    }
    updated = true;
  }

  Vector<GlyphInfo>& glyphs = mVisualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = mVisualModel->mGlyphsToCharacters;
  Vector<Length>& charactersPerGlyph = mVisualModel->mCharactersPerGlyph;
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
    mVisualModel->CreateGlyphsPerCharacterTable( startIndex, mTextUpdateInfo.mStartGlyphIndex, requestedNumberOfCharacters );
    mVisualModel->CreateCharacterToGlyphTable( startIndex, mTextUpdateInfo.mStartGlyphIndex, requestedNumberOfCharacters );
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
    SetColorSegmentationInfo( mLogicalModel->mColorRuns,
                              mVisualModel->mCharactersToGlyph,
                              mVisualModel->mGlyphsPerCharacter,
                              startIndex,
                              mTextUpdateInfo.mStartGlyphIndex,
                              requestedNumberOfCharacters,
                              mVisualModel->mColors,
                              mVisualModel->mColorIndices );

    updated = true;
  }

  if( ( NULL != mEventData ) &&
      mEventData->mPreEditFlag &&
      ( 0u != mVisualModel->mCharactersToGlyph.Count() ) )
  {
    // Add the underline for the pre-edit text.
    const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
    const Length* const glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();

    const GlyphIndex glyphStart = *( charactersToGlyphBuffer + mEventData->mPreEditStartPosition );
    const CharacterIndex lastPreEditCharacter = mEventData->mPreEditStartPosition + ( ( mEventData->mPreEditLength > 0u ) ? mEventData->mPreEditLength - 1u : 0u );
    const Length numberOfGlyphsLastCharacter = *( glyphsPerCharacterBuffer + lastPreEditCharacter );
    const GlyphIndex glyphEnd = *( charactersToGlyphBuffer + lastPreEditCharacter ) + ( numberOfGlyphsLastCharacter > 1u ? numberOfGlyphsLastCharacter - 1u : 0u );

    GlyphRun underlineRun;
    underlineRun.glyphIndex = glyphStart;
    underlineRun.numberOfGlyphs = 1u + glyphEnd - glyphStart;

    // TODO: At the moment the underline runs are only for pre-edit.
    mVisualModel->mUnderlineRuns.PushBack( underlineRun );
  }

  // The estimated number of lines. Used to avoid reallocations when layouting.
  mTextUpdateInfo.mEstimatedNumberOfLines = std::max( mVisualModel->mLines.Count(), mLogicalModel->mParagraphInfo.Count() );

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

  inputStyle.familyDefined = false;
  inputStyle.weightDefined = false;
  inputStyle.widthDefined = false;
  inputStyle.slantDefined = false;
  inputStyle.sizeDefined = false;

  // Sets the default font's family name, weight, width, slant and size.
  if( mFontDefaults )
  {
    if( mFontDefaults->familyDefined )
    {
      inputStyle.familyName = mFontDefaults->mFontDescription.family;
      inputStyle.familyDefined = true;
    }

    if( mFontDefaults->weightDefined )
    {
      inputStyle.weight = mFontDefaults->mFontDescription.weight;
      inputStyle.weightDefined = true;
    }

    if( mFontDefaults->widthDefined )
    {
      inputStyle.width = mFontDefaults->mFontDescription.width;
      inputStyle.widthDefined = true;
    }

    if( mFontDefaults->slantDefined )
    {
      inputStyle.slant = mFontDefaults->mFontDescription.slant;
      inputStyle.slantDefined = true;
    }

    if( mFontDefaults->sizeDefined )
    {
      inputStyle.size = mFontDefaults->mDefaultPointSize;
      inputStyle.sizeDefined = true;
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
    if( mLogicalModel->mText.Count() > mEventData->mPrimaryCursorPosition )
    {
      mEventData->mPrimaryCursorPosition = CalculateNewCursorIndex( mEventData->mPrimaryCursorPosition );
    }
  }
  else if( Dali::DALI_KEY_CURSOR_UP == keyCode )
  {
    // TODO
  }
  else if(   Dali::DALI_KEY_CURSOR_DOWN == keyCode )
  {
    // TODO
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
        const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
        const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

        mEventData->mPrimaryCursorPosition = GetClosestCursorIndex( xPosition,
                                                                    yPosition );

        // When the cursor position is changing, delay cursor blinking
        mEventData->mDecorator->DelayCursorBlink();
      }
      else
      {
        mEventData->mPrimaryCursorPosition = 0u;
      }

      mEventData->mUpdateCursorPosition = true;
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

  int state = event.p1.mInt;

  if( Gesture::Started    == state ||
      Gesture::Continuing == state )
  {
    const Vector2& actualSize = mVisualModel->GetLayoutSize();
    const Vector2 currentScroll = mEventData->mScrollPosition;

    if( mEventData->mHorizontalScrollingEnabled )
    {
      const float displacementX = event.p2.mFloat;
      mEventData->mScrollPosition.x += displacementX;

      ClampHorizontalScroll( actualSize );
    }

    if( mEventData->mVerticalScrollingEnabled )
    {
      const float displacementY = event.p3.mFloat;
      mEventData->mScrollPosition.y += displacementY;

      ClampVerticalScroll( actualSize );
    }

    if( mEventData->mDecorator )
    {
      mEventData->mDecorator->UpdatePositions( mEventData->mScrollPosition - currentScroll );
    }
  }
}

void Controller::Impl::OnLongPressEvent( const Event& event )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::OnLongPressEvent\n" );

  if( EventData::EDITING == mEventData->mState )
  {
    ChangeState ( EventData::EDITING_WITH_POPUP );
    mEventData->mDecoratorUpdated = true;
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

  if( HANDLE_PRESSED == state )
  {
    // The event.p2 and event.p3 are in decorator coords. Need to transforms to text coords.
    const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
    const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

    const CharacterIndex handleNewPosition = GetClosestCursorIndex( xPosition, yPosition );

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::GRAB_HANDLE_PANNING );

      if( handleNewPosition != mEventData->mPrimaryCursorPosition )
      {
        mEventData->mPrimaryCursorPosition = handleNewPosition;
        mEventData->mUpdateCursorPosition = true;
      }
    }
    else if( Event::LEFT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::SELECTION_HANDLE_PANNING );

      if( ( handleNewPosition != mEventData->mLeftSelectionPosition ) &&
          ( handleNewPosition != mEventData->mRightSelectionPosition ) )
      {
        mEventData->mLeftSelectionPosition = handleNewPosition;

        mEventData->mUpdateLeftSelectionPosition = true;
      }
    }
    else if( Event::RIGHT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::SELECTION_HANDLE_PANNING );

      if( ( handleNewPosition != mEventData->mRightSelectionPosition ) &&
          ( handleNewPosition != mEventData->mLeftSelectionPosition ) )
      {
        mEventData->mRightSelectionPosition = handleNewPosition;

        mEventData->mUpdateRightSelectionPosition = true;
      }
    }
  } // end ( HANDLE_PRESSED == state )
  else if( ( HANDLE_RELEASED == state ) ||
           handleStopScrolling )
  {
    CharacterIndex handlePosition = 0u;
    if( handleStopScrolling )
    {
      // The event.p2 and event.p3 are in decorator coords. Need to transforms to text coords.
      const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
      const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

      handlePosition = GetClosestCursorIndex( xPosition, yPosition );
    }

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      mEventData->mUpdateCursorPosition = true;
      mEventData->mUpdateInputStyle = true;

      if( !IsClipboardEmpty() )
      {
        ChangeState( EventData::EDITING_WITH_PASTE_POPUP ); // Moving grabhandle will show Paste Popup
      }

      if( handleStopScrolling )
      {
        mEventData->mScrollAfterUpdatePosition = mEventData->mPrimaryCursorPosition != handlePosition;
        mEventData->mPrimaryCursorPosition = handlePosition;
      }
    }
    else if( Event::LEFT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::SELECTING );

      if( handleStopScrolling )
      {
        mEventData->mUpdateLeftSelectionPosition = ( mEventData->mRightSelectionPosition != handlePosition );
        mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateLeftSelectionPosition;

        if( mEventData->mUpdateLeftSelectionPosition )
        {
          mEventData->mLeftSelectionPosition = handlePosition;
        }
      }
    }
    else if( Event::RIGHT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::SELECTING );

      if( handleStopScrolling )
      {
        mEventData->mUpdateRightSelectionPosition = ( mEventData->mLeftSelectionPosition != handlePosition );
        mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateRightSelectionPosition;
        if( mEventData->mUpdateRightSelectionPosition )
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
    const Vector2& actualSize = mVisualModel->GetLayoutSize();
    const Vector2 currentScrollPosition = mEventData->mScrollPosition;

    mEventData->mScrollPosition.x += xSpeed;

    ClampHorizontalScroll( actualSize );

    bool endOfScroll = false;
    if( Vector2::ZERO == ( currentScrollPosition - mEventData->mScrollPosition ) )
    {
      // Notify the decorator there is no more text to scroll.
      // The decorator won't send more scroll events.
      mEventData->mDecorator->NotifyEndOfScroll();
      // Still need to set the position of the handle.
      endOfScroll = true;
    }

    // Set the position of the handle.
    const bool scrollRightDirection = xSpeed > 0.f;
    const bool leftSelectionHandleEvent = Event::LEFT_SELECTION_HANDLE_EVENT == event.type;
    const bool rightSelectionHandleEvent = Event::RIGHT_SELECTION_HANDLE_EVENT == event.type;

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::GRAB_HANDLE_PANNING );

      Vector2 position = mEventData->mDecorator->GetPosition( GRAB_HANDLE );

      // Position the grag handle close to either the left or right edge.
      position.x = scrollRightDirection ? 0.f : mVisualModel->mControlSize.width;

      // Get the new handle position.
      // The grab handle's position is in decorator coords. Need to transforms to text coords.
      const CharacterIndex handlePosition = GetClosestCursorIndex( position.x - mEventData->mScrollPosition.x - mAlignmentOffset.x,
                                                                   position.y - mEventData->mScrollPosition.y - mAlignmentOffset.y );

      mEventData->mUpdateCursorPosition = mEventData->mPrimaryCursorPosition != handlePosition;
      mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateCursorPosition;
      mEventData->mPrimaryCursorPosition = handlePosition;
      mEventData->mUpdateInputStyle = mEventData->mUpdateCursorPosition;
    }
    else if( leftSelectionHandleEvent || rightSelectionHandleEvent )
    {
      // TODO: This is recalculating the selection box every time the text is scrolled with the selection handles.
      //       Think if something can be done to save power.

      ChangeState( EventData::SELECTION_HANDLE_PANNING );

      Vector2 position = mEventData->mDecorator->GetPosition( leftSelectionHandleEvent ? Text::LEFT_SELECTION_HANDLE : Text::RIGHT_SELECTION_HANDLE );

      // Position the selection handle close to either the left or right edge.
      position.x = scrollRightDirection ? 0.f : mVisualModel->mControlSize.width;

      // Get the new handle position.
      // The selection handle's position is in decorator coords. Need to transforms to text coords.
      const CharacterIndex handlePosition = GetClosestCursorIndex( position.x - mEventData->mScrollPosition.x - mAlignmentOffset.x,
                                                                   position.y - mEventData->mScrollPosition.y - mAlignmentOffset.y );

      if( leftSelectionHandleEvent )
      {
        const bool differentHandles = ( mEventData->mLeftSelectionPosition != handlePosition ) && ( mEventData->mRightSelectionPosition != handlePosition );
        mEventData->mUpdateLeftSelectionPosition = endOfScroll || differentHandles;
        if( differentHandles )
        {
          mEventData->mLeftSelectionPosition = handlePosition;
        }
      }
      else
      {
        const bool differentHandles = ( mEventData->mRightSelectionPosition != handlePosition ) && ( mEventData->mLeftSelectionPosition != handlePosition );
        mEventData->mUpdateRightSelectionPosition = endOfScroll || differentHandles;
        if( differentHandles )
        {
          mEventData->mRightSelectionPosition = handlePosition;
        }
      }

      if( mEventData->mUpdateLeftSelectionPosition || mEventData->mUpdateRightSelectionPosition )
      {
        RepositionSelectionHandles();

        mEventData->mScrollAfterUpdatePosition = true;
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
    // The event.p2 and event.p3 are in decorator coords. Need to transforms to text coords.
    const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
    const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

    // Calculates the logical position from the x,y coords.
    RepositionSelectionHandles( xPosition,
                                yPosition );

    mEventData->mUpdateLeftSelectionPosition = true;
    mEventData->mUpdateRightSelectionPosition = true;

    mEventData->mScrollAfterUpdatePosition = ( mEventData->mLeftSelectionPosition != mEventData->mRightSelectionPosition );
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
    mEventData->mLeftSelectionPosition = 0u;
    mEventData->mRightSelectionPosition = mLogicalModel->mText.Count();

    mEventData->mScrollAfterUpdatePosition = true;
    mEventData->mUpdateLeftSelectionPosition = true;
    mEventData->mUpdateRightSelectionPosition = true;
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

  Vector<Character>& utf32Characters = mLogicalModel->mText;
  const Length numberOfCharacters = utf32Characters.Count();

  // Validate the start and end selection points
  if( ( startOfSelectedText + lengthOfSelectedText ) <= numberOfCharacters )
  {
    //Get text as a UTF8 string
    Utf32ToUtf8( &utf32Characters[startOfSelectedText], lengthOfSelectedText, selectedText );

    if( deleteAfterRetrieval ) // Only delete text if copied successfully
    {
      // Set as input style the style of the first deleted character.
      mLogicalModel->RetrieveStyle( startOfSelectedText, mEventData->mInputStyle );

      mLogicalModel->UpdateTextStyleRuns( startOfSelectedText, -static_cast<int>( lengthOfSelectedText ) );

      // Mark the paragraphs to be updated.
      mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;

      // Delete text between handles
      Vector<Character>::Iterator first = utf32Characters.Begin() + startOfSelectedText;
      Vector<Character>::Iterator last  = first + lengthOfSelectedText;
      utf32Characters.Erase( first, last );

      // Scroll after delete.
      mEventData->mPrimaryCursorPosition = handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
      mEventData->mScrollAfterDelete = true;
    }
    // Udpade the cursor position and the decorator.
    // Scroll after the position is updated if is not scrolling after delete.
    mEventData->mUpdateCursorPosition = true;
    mEventData->mScrollAfterUpdatePosition = !mEventData->mScrollAfterDelete;
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

void Controller::Impl::GetTextFromClipboard( unsigned int itemIndex, std::string& retrievedString )
{
  if ( mClipboard )
  {
    retrievedString =  mClipboard.GetItem( itemIndex );
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

  const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const glyphsBuffer = mVisualModel->mGlyphs.Begin();
  const Vector2* const positionsBuffer = mVisualModel->mGlyphPositions.Begin();
  const Length* const charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const glyphToCharacterBuffer = mVisualModel->mGlyphsToCharacters.Begin();
  const CharacterDirection* const modelCharacterDirectionsBuffer = ( 0u != mLogicalModel->mCharacterDirections.Count() ) ? mLogicalModel->mCharacterDirections.Begin() : NULL;

  // TODO: Better algorithm to create the highlight box.
  // TODO: Multi-line.

  // Get the height of the line.
  const Vector<LineRun>& lines = mVisualModel->mLines;
  const LineRun& firstLine = *lines.Begin();
  const float height = firstLine.ascender + -firstLine.descender;

  const bool isLastCharacter = selectionEnd >= mLogicalModel->mText.Count();
  const bool startDirection = ( ( NULL == modelCharacterDirectionsBuffer ) ? false : *( modelCharacterDirectionsBuffer + selectionStart ) );
  const bool endDirection = ( ( NULL == modelCharacterDirectionsBuffer ) ? false : *( modelCharacterDirectionsBuffer + ( selectionEnd - ( isLastCharacter ? 1u : 0u ) ) ) );

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

  // Check if the first glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special code.
  const Length numberOfCharactersStart = *( charactersPerGlyphBuffer + glyphStart );
  bool splitStartGlyph = ( numberOfCharactersStart > 1u ) && HasLigatureMustBreak( mLogicalModel->GetScript( selectionStart ) );

  // Check if the last glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special code.
  const Length numberOfCharactersEnd = *( charactersPerGlyphBuffer + glyphEnd );
  bool splitEndGlyph = ( glyphStart != glyphEnd ) && ( numberOfCharactersEnd > 1u ) && HasLigatureMustBreak( mLogicalModel->GetScript( selectionEndMinusOne ) );

  const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;

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

      const float xPosition = position.x - glyph.xBearing + offset.x + glyphAdvance * static_cast<float>( isCurrentRightToLeft ? ( numberOfCharactersStart - interGlyphIndex - numberOfCharacters ) : interGlyphIndex );

      mEventData->mDecorator->AddHighlight( xPosition,
                                            offset.y,
                                            xPosition + static_cast<float>( numberOfCharacters ) * glyphAdvance,
                                            offset.y + height );

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

      const float xPosition = position.x - glyph.xBearing + offset.x + ( isCurrentRightToLeft ? ( glyphAdvance * static_cast<float>( numberOfCharacters ) ) : 0.f );
      mEventData->mDecorator->AddHighlight( xPosition,
                                            offset.y,
                                            xPosition + static_cast<float>( interGlyphIndex ) * glyphAdvance,
                                            offset.y + height );

      splitEndGlyph = false;
      continue;
    }

    const float xPosition = position.x - glyph.xBearing + offset.x;
    mEventData->mDecorator->AddHighlight( xPosition,
                                          offset.y,
                                          xPosition + glyph.advance,
                                          offset.y + height );
  }

  CursorInfo primaryCursorInfo;
  GetCursorPosition( mEventData->mLeftSelectionPosition,
                     primaryCursorInfo );

  CursorInfo secondaryCursorInfo;
  GetCursorPosition( mEventData->mRightSelectionPosition,
                     secondaryCursorInfo );

  const Vector2 primaryPosition = primaryCursorInfo.primaryPosition + offset;
  const Vector2 secondaryPosition = secondaryCursorInfo.primaryPosition + offset;

  mEventData->mDecorator->SetPosition( LEFT_SELECTION_HANDLE,
                                       primaryPosition.x,
                                       primaryCursorInfo.lineOffset + offset.y,
                                       primaryCursorInfo.lineHeight );

  mEventData->mDecorator->SetPosition( RIGHT_SELECTION_HANDLE,
                                       secondaryPosition.x,
                                       secondaryCursorInfo.lineOffset + offset.y,
                                       secondaryCursorInfo.lineHeight );

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

  const Length numberOfGlyphs = mVisualModel->mGlyphs.Count();
  const Length numberOfLines  = mVisualModel->mLines.Count();
  if( ( 0 == numberOfGlyphs ) ||
      ( 0 == numberOfLines ) )
  {
    // Nothing to do if there is no text.
    return;
  }

  // Find which word was selected
  CharacterIndex selectionStart( 0 );
  CharacterIndex selectionEnd( 0 );
  FindSelectionIndices( visualX, visualY, selectionStart, selectionEnd );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "%p selectionStart %d selectionEnd %d\n", this, selectionStart, selectionEnd );

  if( selectionStart == selectionEnd )
  {
    ChangeState( EventData::EDITING );
    // Nothing to select. i.e. a white space, out of bounds
    return;
  }

  mEventData->mLeftSelectionPosition = selectionStart;
  mEventData->mRightSelectionPosition = selectionEnd;
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
    if( mLogicalModel->mText.Count() && !IsShowingPlaceholderText() )
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
    mEventData->mState = newState;

    if( EventData::INACTIVE == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
      mEventData->mDecorator->StopCursorBlink();
      mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
      mEventData->mDecorator->SetPopupActive( false );
      mEventData->mDecoratorUpdated = true;
      HideClipboard();
    }
    else if( EventData::INTERRUPTED  == mEventData->mState)
    {
      mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
      mEventData->mDecorator->SetPopupActive( false );
      mEventData->mDecoratorUpdated = true;
      HideClipboard();
    }
    else if( EventData::SELECTING == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
      mEventData->mDecorator->StopCursorBlink();
      mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
      mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
      if( mEventData->mGrabHandlePopupEnabled )
      {
        SetPopupButtons();
        mEventData->mDecorator->SetPopupActive( true );
      }
      mEventData->mDecoratorUpdated = true;
    }
    else if( EventData::EDITING == mEventData->mState )
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
      if( mEventData->mGrabHandlePopupEnabled )
      {
        mEventData->mDecorator->SetPopupActive( false );
      }
      mEventData->mDecoratorUpdated = true;
      HideClipboard();
    }
    else if( EventData::EDITING_WITH_POPUP == mEventData->mState )
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
      HideClipboard();
      mEventData->mDecoratorUpdated = true;
    }
    else if( EventData::EDITING_WITH_GRAB_HANDLE == mEventData->mState )
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
      if( mEventData->mGrabHandlePopupEnabled )
      {
        mEventData->mDecorator->SetPopupActive( false );
      }
      mEventData->mDecoratorUpdated = true;
      HideClipboard();
    }
    else if( EventData::SELECTION_HANDLE_PANNING == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
      mEventData->mDecorator->StopCursorBlink();
      mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
      mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
      if( mEventData->mGrabHandlePopupEnabled )
      {
        mEventData->mDecorator->SetPopupActive( false );
      }
      mEventData->mDecoratorUpdated = true;
    }
    else if( EventData::GRAB_HANDLE_PANNING == mEventData->mState )
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
      if( mEventData->mGrabHandlePopupEnabled )
      {
        mEventData->mDecorator->SetPopupActive( false );
      }
      mEventData->mDecoratorUpdated = true;
    }
    else if( EventData::EDITING_WITH_PASTE_POPUP == mEventData->mState )
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

      if( mEventData->mGrabHandlePopupEnabled )
      {
        SetPopupButtons();
        mEventData->mDecorator->SetPopupActive( true );
      }
      HideClipboard();
      mEventData->mDecoratorUpdated = true;
    }
  }
}

LineIndex Controller::Impl::GetClosestLine( float y ) const
{
  float totalHeight = 0.f;
  LineIndex lineIndex = 0u;

  const Vector<LineRun>& lines = mVisualModel->mLines;
  for( LineIndex endLine = lines.Count();
       lineIndex < endLine;
       ++lineIndex )
  {
    const LineRun& lineRun = lines[lineIndex];
    totalHeight += lineRun.ascender + -lineRun.descender;
    if( y < totalHeight )
    {
      return lineIndex;
    }
  }

  if( lineIndex == 0 )
  {
    return 0;
  }

  return lineIndex-1;
}

void Controller::Impl::FindSelectionIndices( float visualX, float visualY, CharacterIndex& startIndex, CharacterIndex& endIndex )
{
  CharacterIndex hitCharacter = GetClosestCursorIndex( visualX, visualY );
  DALI_ASSERT_DEBUG( hitCharacter <= mLogicalModel->mText.Count() && "GetClosestCursorIndex returned out of bounds index" );

  if( mLogicalModel->mText.Count() == 0 )
  {
    return;  // if model empty
  }

  if( hitCharacter >= mLogicalModel->mText.Count() )
  {
    // Closest hit character is the last character.
    if( hitCharacter ==  mLogicalModel->mText.Count() )
    {
      hitCharacter--; //Hit character index set to last character in logical model
    }
    else
    {
      // hitCharacter is out of bounds
      return;
    }
  }

  startIndex = hitCharacter;
  endIndex = hitCharacter;
  bool isHitCharacterWhitespace = TextAbstraction::IsWhiteSpace( mLogicalModel->mText[hitCharacter] );

  // Find the start and end of the text
  for( startIndex = hitCharacter; startIndex > 0; --startIndex )
  {
    if( isHitCharacterWhitespace != TextAbstraction::IsWhiteSpace( mLogicalModel->mText[ startIndex-1 ] ) )
    {
      break;
    }
  }
  const CharacterIndex pastTheEnd = mLogicalModel->mText.Count();
  for( endIndex = hitCharacter + 1u; endIndex < pastTheEnd; ++endIndex )
  {
    if( isHitCharacterWhitespace != TextAbstraction::IsWhiteSpace( mLogicalModel->mText[ endIndex ] ) )
    {
      break;
    }
  }
}

CharacterIndex Controller::Impl::GetClosestCursorIndex( float visualX,
                                                        float visualY )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetClosestCursorIndex %p closest visualX %f visualY %f\n", this, visualX, visualY );

  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return 0u;
  }

  CharacterIndex logicalIndex = 0u;

  const Length numberOfGlyphs = mVisualModel->mGlyphs.Count();
  const Length numberOfLines  = mVisualModel->mLines.Count();
  if( ( 0 == numberOfGlyphs ) ||
      ( 0 == numberOfLines ) )
  {
    return logicalIndex;
  }

  // Find which line is closest
  const LineIndex lineIndex = GetClosestLine( visualY );
  const LineRun& line = mVisualModel->mLines[lineIndex];

  // Get the positions of the glyphs.
  const Vector<Vector2>& positions = mVisualModel->mGlyphPositions;
  const Vector2* const positionsBuffer = positions.Begin();

  // Get the character to glyph conversion table.
  const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();

  // Get the glyphs per character table.
  const Length* const glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();

  // Get the glyph's info buffer.
  const GlyphInfo* const glyphInfoBuffer = mVisualModel->mGlyphs.Begin();

  const CharacterIndex startCharacter = line.characterRun.characterIndex;
  const CharacterIndex endCharacter   = line.characterRun.characterIndex + line.characterRun.numberOfCharacters;
  DALI_ASSERT_DEBUG( endCharacter <= mLogicalModel->mText.Count() && "Invalid line info" );

  // Whether there is a hit on a glyph.
  bool matched = false;

  // Traverses glyphs in visual order. To do that use the visual to logical conversion table.
  CharacterIndex visualIndex = startCharacter;
  Length numberOfCharacters = 0u;
  for( ; !matched && ( visualIndex < endCharacter ); ++visualIndex )
  {
    // The character in logical order.
    const CharacterIndex characterLogicalOrderIndex = mLogicalModel->GetLogicalCharacterIndex( visualIndex );

    // Get the script of the character.
    const Script script = mLogicalModel->GetScript( characterLogicalOrderIndex );

    // The number of glyphs for that character
    const Length numberOfGlyphs = *( glyphsPerCharacterBuffer + characterLogicalOrderIndex );
    ++numberOfCharacters;


    if( 0u != numberOfGlyphs )
    {
      // Get the first character/glyph of the group of glyphs.
      const CharacterIndex firstVisualCharacterIndex = 1u + visualIndex - numberOfCharacters;
      const CharacterIndex firstLogicalCharacterIndex = mLogicalModel->GetLogicalCharacterIndex( firstVisualCharacterIndex );
      const GlyphIndex firstLogicalGlyphIndex = *( charactersToGlyphBuffer + firstLogicalCharacterIndex );

      // Get the metrics for the group of glyphs.
      GlyphMetrics glyphMetrics;
      GetGlyphsMetrics( firstLogicalGlyphIndex,
                        numberOfGlyphs,
                        glyphMetrics,
                        glyphInfoBuffer,
                        mMetrics );

      // Get the position of the first glyph.
      const Vector2& position = *( positionsBuffer + firstLogicalGlyphIndex );

      // Whether the glyph can be split, like Latin ligatures fi, ff or Arabic ﻻ.
      const bool isInterglyphIndex = ( numberOfCharacters > numberOfGlyphs ) && HasLigatureMustBreak( script );
      const Length numberOfBlocks = isInterglyphIndex ? numberOfCharacters : 1u;
      const float glyphAdvance = glyphMetrics.advance / static_cast<float>( numberOfBlocks );

      GlyphIndex index = 0u;
      for( ; !matched && ( index < numberOfBlocks ); ++index )
      {
        // Find the mid-point of the area containing the glyph
        const float glyphCenter = -glyphMetrics.xBearing + position.x + ( static_cast<float>( index ) + 0.5f ) * glyphAdvance;

        if( visualX < glyphCenter )
        {
          matched = true;
          break;
        }
      }

      if( matched )
      {
        visualIndex = firstVisualCharacterIndex + index;
        break;
      }

      numberOfCharacters = 0u;
    }

  }

  // Return the logical position of the cursor in characters.

  if( !matched )
  {
    visualIndex = endCharacter;
  }

  logicalIndex = mLogicalModel->GetLogicalCursorIndex( visualIndex );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "%p closest visualIndex %d logicalIndex %d\n", this, visualIndex, logicalIndex );

  DALI_ASSERT_DEBUG( ( logicalIndex <= mLogicalModel->mText.Count() && logicalIndex >= 0 ) && "GetClosestCursorIndex - Out of bounds index" );

  return logicalIndex;
}

void Controller::Impl::GetCursorPosition( CharacterIndex logical,
                                          CursorInfo& cursorInfo )
{
  // TODO: Check for multiline with \n, etc...

  const Length numberOfCharacters = mLogicalModel->mText.Count();
  if( !IsShowingRealText() )
  {
    // Do not want to use the place-holder text to set the cursor position.

    // Use the line's height of the font's family set to set the cursor's size.
    // If there is no font's family set, use the default font.
    // Use the current alignment to place the cursor at the beginning, center or end of the box.

    cursorInfo.lineOffset = 0.f;
    cursorInfo.lineHeight = GetDefaultFontLineHeight();
    cursorInfo.primaryCursorHeight = cursorInfo.lineHeight;

    switch( mLayoutEngine.GetHorizontalAlignment() )
    {
      case LayoutEngine::HORIZONTAL_ALIGN_BEGIN:
      {
        cursorInfo.primaryPosition.x = 0.f;
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_CENTER:
      {
        cursorInfo.primaryPosition.x = floorf( 0.5f * mVisualModel->mControlSize.width );
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_END:
      {
        cursorInfo.primaryPosition.x = mVisualModel->mControlSize.width - mEventData->mDecorator->GetCursorWidth();
        break;
      }
    }

    switch( mLayoutEngine.GetVerticalAlignment() )
    {
      case LayoutEngine::VERTICAL_ALIGN_TOP:
      {
        cursorInfo.primaryPosition.y = 0.f;
        break;
      }
      case LayoutEngine::VERTICAL_ALIGN_CENTER:
      {
        cursorInfo.primaryPosition.y = floorf( 0.5f * ( mVisualModel->mControlSize.height - cursorInfo.lineHeight ) );
        break;
      }
      case LayoutEngine::VERTICAL_ALIGN_BOTTOM:
      {
        cursorInfo.primaryPosition.y = mVisualModel->mControlSize.height - cursorInfo.lineHeight;
        break;
      }
    }

    // Nothing else to do.
    return;
  }

  // Check if the logical position is the first or the last one of the text.
  const bool isFirstPosition = 0u == logical;
  const bool isLastPosition = numberOfCharacters == logical;

  // 'logical' is the logical 'cursor' index.
  // Get the next and current logical 'character' index.
  const CharacterIndex nextCharacterIndex = logical;
  const CharacterIndex characterIndex = isFirstPosition ? logical : logical - 1u;

  // Get the direction of the character and the next one.
  const CharacterDirection* const modelCharacterDirectionsBuffer = ( 0u != mLogicalModel->mCharacterDirections.Count() ) ? mLogicalModel->mCharacterDirections.Begin() : NULL;

  CharacterDirection isCurrentRightToLeft = false;
  CharacterDirection isNextRightToLeft = false;
  if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
  {
    isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + characterIndex );
    isNextRightToLeft = *( modelCharacterDirectionsBuffer + nextCharacterIndex );
  }

  // Get the line where the character is laid-out.
  const LineRun* const modelLines = mVisualModel->mLines.Begin();

  const LineIndex lineIndex = mVisualModel->GetLineOfCharacter( characterIndex );
  const LineRun& line = *( modelLines + lineIndex );

  // Get the paragraph's direction.
  const CharacterDirection isRightToLeftParagraph = line.direction;

  // Check whether there is an alternative position:

  cursorInfo.isSecondaryCursor = ( !isLastPosition && ( isCurrentRightToLeft != isNextRightToLeft ) ) ||
                                 ( isLastPosition && ( isRightToLeftParagraph != isCurrentRightToLeft ) );

  // Set the line offset and height.
  cursorInfo.lineOffset = 0.f;
  cursorInfo.lineHeight = line.ascender + -line.descender;

  // Calculate the primary cursor.

  CharacterIndex index = characterIndex;
  if( cursorInfo.isSecondaryCursor )
  {
    // If there is a secondary position, the primary cursor may be in a different place than the logical index.

    if( isLastPosition )
    {
      // The position of the cursor after the last character needs special
      // care depending on its direction and the direction of the paragraph.

      // Need to find the first character after the last character with the paragraph's direction.
      // i.e l0 l1 l2 r0 r1 should find r0.

      // TODO: check for more than one line!
      index = isRightToLeftParagraph ? line.characterRun.characterIndex : line.characterRun.characterIndex + line.characterRun.numberOfCharacters - 1u;
      index = mLogicalModel->GetLogicalCharacterIndex( index );
    }
    else
    {
      index = ( isRightToLeftParagraph == isCurrentRightToLeft ) ? characterIndex : nextCharacterIndex;
    }
  }

  const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();
  const Length* const charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const glyphsToCharactersBuffer = mVisualModel->mGlyphsToCharacters.Begin();
  const Vector2* const glyphPositionsBuffer = mVisualModel->mGlyphPositions.Begin();
  const GlyphInfo* const glyphInfoBuffer = mVisualModel->mGlyphs.Begin();

  // Convert the cursor position into the glyph position.
  const GlyphIndex primaryGlyphIndex = *( charactersToGlyphBuffer + index );
  const Length primaryNumberOfGlyphs = *( glyphsPerCharacterBuffer + index );
  const Length primaryNumberOfCharacters = *( charactersPerGlyphBuffer + primaryGlyphIndex );

  // Get the metrics for the group of glyphs.
  GlyphMetrics glyphMetrics;
  GetGlyphsMetrics( primaryGlyphIndex,
                    primaryNumberOfGlyphs,
                    glyphMetrics,
                    glyphInfoBuffer,
                    mMetrics );

  // Whether to add the glyph's advance to the cursor position.
  // i.e if the paragraph is left to right and the logical cursor is zero, the position is the position of the first glyph and the advance is not added,
  //     if the logical cursor is one, the position is the position of the first glyph and the advance is added.
  // A 'truth table' was build and an online Karnaugh map tool was used to simplify the logic.
  //
  // FLCP A
  // ------
  // 0000 1
  // 0001 1
  // 0010 0
  // 0011 0
  // 0100 1
  // 0101 0
  // 0110 1
  // 0111 0
  // 1000 0
  // 1001 x
  // 1010 x
  // 1011 1
  // 1100 x
  // 1101 x
  // 1110 x
  // 1111 x
  //
  // Where F -> isFirstPosition
  //       L -> isLastPosition
  //       C -> isCurrentRightToLeft
  //       P -> isRightToLeftParagraph
  //       A -> Whether to add the glyph's advance.

  const bool addGlyphAdvance = ( ( isLastPosition && !isRightToLeftParagraph ) ||
                                 ( isFirstPosition && isRightToLeftParagraph ) ||
                                 ( !isFirstPosition && !isLastPosition && !isCurrentRightToLeft ) );

  float glyphAdvance = addGlyphAdvance ? glyphMetrics.advance : 0.f;

  if( !isLastPosition &&
      ( primaryNumberOfCharacters > 1u ) )
  {
    const CharacterIndex firstIndex = *( glyphsToCharactersBuffer + primaryGlyphIndex );

    bool isCurrentRightToLeft = false;
    if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
    {
      isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + index );
    }

    Length numberOfGlyphAdvance = ( isFirstPosition ? 0u : 1u ) + characterIndex - firstIndex;
    if( isCurrentRightToLeft )
    {
      numberOfGlyphAdvance = primaryNumberOfCharacters - numberOfGlyphAdvance;
    }

    glyphAdvance = static_cast<float>( numberOfGlyphAdvance ) * glyphMetrics.advance / static_cast<float>( primaryNumberOfCharacters );
  }

  // Get the glyph position and x bearing.
  const Vector2& primaryPosition = *( glyphPositionsBuffer + primaryGlyphIndex );

  // Set the primary cursor's height.
  cursorInfo.primaryCursorHeight = cursorInfo.isSecondaryCursor ? 0.5f * glyphMetrics.fontHeight : glyphMetrics.fontHeight;

  // Set the primary cursor's position.
  cursorInfo.primaryPosition.x = -glyphMetrics.xBearing + primaryPosition.x + glyphAdvance;
  cursorInfo.primaryPosition.y = line.ascender - glyphMetrics.ascender;

  // Calculate the secondary cursor.

  if( cursorInfo.isSecondaryCursor )
  {
    // Set the secondary cursor's height.
    cursorInfo.secondaryCursorHeight = 0.5f * glyphMetrics.fontHeight;

    CharacterIndex index = characterIndex;
    if( !isLastPosition )
    {
      index = ( isRightToLeftParagraph == isCurrentRightToLeft ) ? nextCharacterIndex : characterIndex;
    }

    const GlyphIndex secondaryGlyphIndex = *( charactersToGlyphBuffer + index );
    const Length secondaryNumberOfGlyphs = *( glyphsPerCharacterBuffer + index );

    const Vector2& secondaryPosition = *( glyphPositionsBuffer + secondaryGlyphIndex );

    GetGlyphsMetrics( secondaryGlyphIndex,
                      secondaryNumberOfGlyphs,
                      glyphMetrics,
                      glyphInfoBuffer,
                      mMetrics );

    // Set the secondary cursor's position.
    cursorInfo.secondaryPosition.x = -glyphMetrics.xBearing + secondaryPosition.x + ( isCurrentRightToLeft ? 0.f : glyphMetrics.advance );
    cursorInfo.secondaryPosition.y = cursorInfo.lineHeight - cursorInfo.secondaryCursorHeight - line.descender - ( glyphMetrics.fontHeight - glyphMetrics.ascender );
  }

  if( LayoutEngine::MULTI_LINE_BOX == mLayoutEngine.GetLayout() )
  {
    // If the text is editable and multi-line, the cursor position after a white space shouldn't exceed the boundaries of the text control.

    // Note the white spaces laid-out at the end of the line might exceed the boundaries of the control.
    // The reason is a wrapped line must not start with a white space so they are laid-out at the end of the line.

    if( 0.f > cursorInfo.primaryPosition.x )
    {
      cursorInfo.primaryPosition.x = 0.f;
    }

    const float edgeWidth = mVisualModel->mControlSize.width - static_cast<float>( mEventData->mDecorator->GetCursorWidth() );
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

  const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
  const Length* const charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();

  GlyphIndex glyphIndex = *( charactersToGlyphBuffer + index );
  Length numberOfCharacters = *( charactersPerGlyphBuffer + glyphIndex );

  if( numberOfCharacters > 1u )
  {
    const Script script = mLogicalModel->GetScript( index );
    if( HasLigatureMustBreak( script ) )
    {
      // Prevents to jump the whole Latin ligatures like fi, ff, or Arabic ﻻ,  ...
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

  const Vector2 offset = mEventData->mScrollPosition + ( IsShowingRealText() ? mAlignmentOffset : Vector2::ZERO );
  const Vector2 cursorPosition = cursorInfo.primaryPosition + offset;

  // Sets the cursor position.
  mEventData->mDecorator->SetPosition( PRIMARY_CURSOR,
                                       cursorPosition.x,
                                       cursorPosition.y,
                                       cursorInfo.primaryCursorHeight,
                                       cursorInfo.lineHeight );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Primary cursor position: %f,%f\n", cursorPosition.x, cursorPosition.y );

  // Sets the grab handle position.
  mEventData->mDecorator->SetPosition( GRAB_HANDLE,
                                       cursorPosition.x,
                                       cursorInfo.lineOffset + offset.y,
                                       cursorInfo.lineHeight );

  if( cursorInfo.isSecondaryCursor )
  {
    mEventData->mDecorator->SetPosition( SECONDARY_CURSOR,
                                         cursorInfo.secondaryPosition.x + offset.x,
                                         cursorInfo.secondaryPosition.y + offset.y,
                                         cursorInfo.secondaryCursorHeight,
                                         cursorInfo.lineHeight );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n", cursorInfo.secondaryPosition.x + offset.x, cursorInfo.secondaryPosition.y + offset.y );
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

  const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;
  const Vector2 cursorPosition = cursorInfo.primaryPosition + offset;

  // Sets the handle's position.
  mEventData->mDecorator->SetPosition( handleType,
                                       cursorPosition.x,
                                       cursorInfo.lineOffset + offset.y,
                                       cursorInfo.lineHeight );

  // If selection handle at start of the text and other at end of the text then all text is selected.
  const CharacterIndex startOfSelection = std::min( mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition );
  const CharacterIndex endOfSelection = std::max ( mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition );
  mEventData->mAllTextSelected = ( startOfSelection == 0 ) && ( endOfSelection == mLogicalModel->mText.Count() );
}

void Controller::Impl::ClampHorizontalScroll( const Vector2& actualSize )
{
  // Clamp between -space & 0 (and the text alignment).

  if( actualSize.width > mVisualModel->mControlSize.width )
  {
    const float space = ( actualSize.width - mVisualModel->mControlSize.width ) + mAlignmentOffset.x;
    mEventData->mScrollPosition.x = ( mEventData->mScrollPosition.x < -space ) ? -space : mEventData->mScrollPosition.x;
    mEventData->mScrollPosition.x = ( mEventData->mScrollPosition.x > -mAlignmentOffset.x ) ? -mAlignmentOffset.x : mEventData->mScrollPosition.x;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mEventData->mScrollPosition.x = 0.f;
  }
}

void Controller::Impl::ClampVerticalScroll( const Vector2& actualSize )
{
  // Clamp between -space & 0 (and the text alignment).
  if( actualSize.height > mVisualModel->mControlSize.height )
  {
    const float space = ( actualSize.height - mVisualModel->mControlSize.height ) + mAlignmentOffset.y;
    mEventData->mScrollPosition.y = ( mEventData->mScrollPosition.y < -space ) ? -space : mEventData->mScrollPosition.y;
    mEventData->mScrollPosition.y = ( mEventData->mScrollPosition.y > -mAlignmentOffset.y ) ? -mAlignmentOffset.y : mEventData->mScrollPosition.y;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mEventData->mScrollPosition.y = 0.f;
  }
}

void Controller::Impl::ScrollToMakePositionVisible( const Vector2& position )
{
  // position is in actor's coords.
  const float positionEnd = position.x + ( mEventData->mDecorator ? mEventData->mDecorator->GetCursorWidth() : 0.f );

  // Transform the position to decorator coords.
  const float alignment = IsShowingRealText() ? mAlignmentOffset.x : 0.f;
  const float offset = mEventData->mScrollPosition.x + alignment;
  const float decoratorPositionBegin = position.x + offset;
  const float decoratorPositionEnd = positionEnd + offset;

  if( decoratorPositionBegin < 0.f )
  {
    mEventData->mScrollPosition.x = -position.x - alignment;
  }
  else if( decoratorPositionEnd > mVisualModel->mControlSize.width )
  {
    mEventData->mScrollPosition.x = mVisualModel->mControlSize.width - positionEnd - alignment;
  }
}

void Controller::Impl::ScrollTextToMatchCursor( const CursorInfo& cursorInfo )
{
  // Get the current cursor position in decorator coords.
  const Vector2& currentCursorPosition = mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

  // Calculate the offset to match the cursor position before the character was deleted.
  mEventData->mScrollPosition.x = currentCursorPosition.x - cursorInfo.primaryPosition.x - mAlignmentOffset.x;

  ClampHorizontalScroll( mVisualModel->GetLayoutSize() );
}

void Controller::Impl::RequestRelayout()
{
  mControlInterface.RequestTextRelayout();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
