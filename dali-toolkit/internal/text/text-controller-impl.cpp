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
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-io.h>
#include <dali-toolkit/internal/text/text-view.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

/**
 * @brief Some characters can be shaped in more than one glyph.
 * This struct is used to retrieve metrics from these group of glyphs.
 */
struct GlyphMetrics
{
  GlyphMetrics()
  : fontHeight( 0.f ),
    advance( 0.f ),
    ascender( 0.f ),
    xBearing( 0.f )
  {}

  ~GlyphMetrics()
  {}

  float fontHeight; ///< The font's height of that glyphs.
  float advance;    ///< The sum of all the advances of all the glyphs.
  float ascender;   ///< The font's ascender.
  float xBearing;   ///< The x bearing of the first glyph.
};

const std::string EMPTY_STRING("");

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Get some glyph's metrics of a group of glyphs formed as a result of shaping one character.
 *
 * @param[in] glyphIndex The index to the first glyph.
 * @param[in] numberOfGlyphs The number of glyphs.
 * @param[out] glyphMetrics Some glyph metrics (font height, advance, ascender and x bearing).
 * @param[in]
 * @param[in]
 */
void GetGlyphsMetrics( GlyphIndex glyphIndex,
                       Length numberOfGlyphs,
                       GlyphMetrics& glyphMetrics,
                       VisualModelPtr visualModel,
                       TextAbstraction::FontClient& fontClient )
{
  const GlyphInfo* glyphsBuffer = visualModel->mGlyphs.Begin();

  const GlyphInfo& firstGlyph = *( glyphsBuffer + glyphIndex );

  Text::FontMetrics fontMetrics;
  fontClient.GetFontMetrics( firstGlyph.fontId, fontMetrics );

  glyphMetrics.fontHeight = fontMetrics.height;
  glyphMetrics.advance = firstGlyph.advance;
  glyphMetrics.ascender = fontMetrics.ascender;
  glyphMetrics.xBearing = firstGlyph.xBearing;

  for( unsigned int i = 1u; i < numberOfGlyphs; ++i )
  {
    const GlyphInfo& glyphInfo = *( glyphsBuffer + glyphIndex + i );

    glyphMetrics.advance += glyphInfo.advance;
  }
}

EventData::EventData( DecoratorPtr decorator )
: mDecorator( decorator ),
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
  mScrollAfterDelete( false )
{}

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
      }
    }
  }

  // The cursor must also be repositioned after inserts into the model
  if( mEventData->mUpdateCursorPosition )
  {
    // Updates the cursor position and scrolls the text to make it visible.

    UpdateCursorPosition();

    if( mEventData->mScrollAfterUpdatePosition )
    {
      const Vector2& primaryCursorPosition = mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

      ScrollToMakePositionVisible( primaryCursorPosition );
      mEventData->mScrollAfterUpdatePosition = false;
    }

    mEventData->mDecoratorUpdated = true;
    mEventData->mUpdateCursorPosition = false;
  }
  else if( mEventData->mScrollAfterDelete )
  {
    ScrollTextToMatchCursor();
    mEventData->mDecoratorUpdated = true;
    mEventData->mScrollAfterDelete = false;
  }
  else
  {
    bool leftScroll = false;
    bool rightScroll = false;

    if( mEventData->mUpdateLeftSelectionPosition )
    {
      UpdateSelectionHandle( LEFT_SELECTION_HANDLE );

      if( mEventData->mScrollAfterUpdatePosition )
      {
        const Vector2& leftHandlePosition = mEventData->mDecorator->GetPosition( LEFT_SELECTION_HANDLE );

        ScrollToMakePositionVisible( leftHandlePosition );
        leftScroll = true;
      }

      mEventData->mDecoratorUpdated = true;
      mEventData->mUpdateLeftSelectionPosition = false;
    }

    if( mEventData->mUpdateRightSelectionPosition )
    {
      UpdateSelectionHandle( RIGHT_SELECTION_HANDLE );

      if( mEventData->mScrollAfterUpdatePosition )
      {
        const Vector2& rightHandlePosition = mEventData->mDecorator->GetPosition( RIGHT_SELECTION_HANDLE );

        ScrollToMakePositionVisible( rightHandlePosition );
        rightScroll = true;
      }

      mEventData->mDecoratorUpdated = true;
      mEventData->mUpdateRightSelectionPosition = false;
    }

    if( leftScroll || rightScroll )
    {
      mEventData->mScrollAfterUpdatePosition = false;
    }
  }

  mEventData->mEventQueue.clear();

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::ProcessInputEvents\n" );

  const bool decoratorUpdated = mEventData->mDecoratorUpdated;
  mEventData->mDecoratorUpdated = false;

  return decoratorUpdated;
}

void Controller::Impl::UpdateModel( OperationsMask operationsRequired )
{
  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mOperationsPending & operationsRequired );

  Vector<Character>& utf32Characters = mLogicalModel->mText;

  const Length numberOfCharacters = utf32Characters.Count();

  Vector<LineBreakInfo>& lineBreakInfo = mLogicalModel->mLineBreakInfo;
  if( GET_LINE_BREAKS & operations )
  {
    // Retrieves the line break info. The line break info is used to split the text in 'paragraphs' to
    // calculate the bidirectional info for each 'paragraph'.
    // It's also used to layout the text (where it should be a new line) or to shape the text (text in different lines
    // is not shaped together).
    lineBreakInfo.Resize( numberOfCharacters, TextAbstraction::LINE_NO_BREAK );

    SetLineBreakInfo( utf32Characters,
                      lineBreakInfo );
  }

  Vector<WordBreakInfo>& wordBreakInfo = mLogicalModel->mWordBreakInfo;
  if( GET_WORD_BREAKS & operations )
  {
    // Retrieves the word break info. The word break info is used to layout the text (where to wrap the text in lines).
    wordBreakInfo.Resize( numberOfCharacters, TextAbstraction::WORD_NO_BREAK );

    SetWordBreakInfo( utf32Characters,
                      wordBreakInfo );
  }

  const bool getScripts = GET_SCRIPTS & operations;
  const bool validateFonts = VALIDATE_FONTS & operations;

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
                                       lineBreakInfo,
                                       scripts );
    }

    if( validateFonts )
    {
      if( 0u == validFonts.Count() )
      {
        // Copy the requested font defaults received via the property system.
        // These may not be valid i.e. may not contain glyphs for the necessary scripts.
        GetDefaultFonts( validFonts, numberOfCharacters );
      }

      // Validates the fonts. If there is a character with no assigned font it sets a default one.
      // After this call, fonts are validated.
      multilanguageSupport.ValidateFonts( utf32Characters,
                                          scripts,
                                          validFonts );
    }
  }

  Vector<Character> mirroredUtf32Characters;
  bool textMirrored = false;
  if( BIDI_INFO & operations )
  {
    // Count the number of LINE_NO_BREAK to reserve some space for the vector of paragraph's
    // bidirectional info.

    Length numberOfParagraphs = 0u;

    const TextAbstraction::LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();
    for( Length index = 0u; index < numberOfCharacters; ++index )
    {
      if( TextAbstraction::LINE_NO_BREAK == *( lineBreakInfoBuffer + index ) )
      {
        ++numberOfParagraphs;
      }
    }

    Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mLogicalModel->mBidirectionalParagraphInfo;
    bidirectionalInfo.Reserve( numberOfParagraphs );

    // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
    SetBidirectionalInfo( utf32Characters,
                          scripts,
                          lineBreakInfo,
                          bidirectionalInfo );

    if( 0u != bidirectionalInfo.Count() )
    {
      // This paragraph has right to left text. Some characters may need to be mirrored.
      // TODO: consider if the mirrored string can be stored as well.

      textMirrored = GetMirroredText( utf32Characters, mirroredUtf32Characters );

      // Only set the character directions if there is right to left characters.
      Vector<CharacterDirection>& directions = mLogicalModel->mCharacterDirections;
      directions.Resize( numberOfCharacters );

      GetCharactersDirection( bidirectionalInfo,
                              directions );
    }
    else
    {
      // There is no right to left characters. Clear the directions vector.
      mLogicalModel->mCharacterDirections.Clear();
    }

   }

  Vector<GlyphInfo>& glyphs = mVisualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = mVisualModel->mGlyphsToCharacters;
  Vector<Length>& charactersPerGlyph = mVisualModel->mCharactersPerGlyph;
  if( SHAPE_TEXT & operations )
  {
    const Vector<Character>& textToShape = textMirrored ? mirroredUtf32Characters : utf32Characters;
    // Shapes the text.
    ShapeText( textToShape,
               lineBreakInfo,
               scripts,
               validFonts,
               glyphs,
               glyphsToCharactersMap,
               charactersPerGlyph );

    // Create the 'number of glyphs' per character and the glyph to character conversion tables.
    mVisualModel->CreateGlyphsPerCharacterTable( numberOfCharacters );
    mVisualModel->CreateCharacterToGlyphTable( numberOfCharacters );
  }

  const Length numberOfGlyphs = glyphs.Count();

  if( GET_GLYPH_METRICS & operations )
  {
    mFontClient.GetGlyphMetrics( glyphs.Begin(), numberOfGlyphs );
  }
}

void Controller::Impl::GetDefaultFonts( Vector<FontRun>& fonts, Length numberOfCharacters )
{
  if( mFontDefaults )
  {
    FontRun fontRun;
    fontRun.characterRun.characterIndex = 0;
    fontRun.characterRun.numberOfCharacters = numberOfCharacters;
    fontRun.fontId = mFontDefaults->GetFontId( mFontClient );
    fontRun.isDefault = true;

    fonts.PushBack( fontRun );
  }
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
    if( mLogicalModel->GetNumberOfCharacters() > mEventData->mPrimaryCursorPosition )
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
  mEventData->mScrollAfterUpdatePosition = true;
}

void Controller::Impl::OnTapEvent( const Event& event )
{
  if( NULL != mEventData )
  {
    const unsigned int tapCount = event.p1.mUint;

    if( 1u == tapCount )
    {
      if( ! IsShowingPlaceholderText() )
      {
        const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
        const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

        mEventData->mPrimaryCursorPosition = GetClosestCursorIndex( xPosition,
                                                                    yPosition );
      }
      else
      {
        mEventData->mPrimaryCursorPosition = 0u;
      }

      mEventData->mUpdateCursorPosition = true;
      mEventData->mScrollAfterUpdatePosition = true;
    }
    else if( mEventData->mSelectionEnabled &&
             ( 2u == tapCount ) )
    {
      // The event.p2 and event.p3 are in decorator coords. Need to transforms to text coords.
      const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
      const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

      RepositionSelectionHandles( xPosition,
                                  yPosition );

      mEventData->mScrollAfterUpdatePosition = true;
      mEventData->mUpdateLeftSelectionPosition = true;
      mEventData->mUpdateRightSelectionPosition = true;
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
    const Vector2& actualSize = mVisualModel->GetActualSize();
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

      if( handleNewPosition != mEventData->mLeftSelectionPosition )
      {
        mEventData->mLeftSelectionPosition = handleNewPosition;

        RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                    mEventData->mRightSelectionPosition );

        mEventData->mUpdateLeftSelectionPosition = true;
      }
    }
    else if( Event::RIGHT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState ( EventData::SELECTION_HANDLE_PANNING );

      if( handleNewPosition != mEventData->mRightSelectionPosition )
      {
        mEventData->mRightSelectionPosition = handleNewPosition;

        RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                    mEventData->mRightSelectionPosition );

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

      ChangeState( EventData::EDITING_WITH_POPUP );

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
        mEventData->mUpdateLeftSelectionPosition = mEventData->mLeftSelectionPosition != handlePosition;
        mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateLeftSelectionPosition;
        mEventData->mLeftSelectionPosition = handlePosition;

        if( mEventData->mUpdateLeftSelectionPosition )
        {
          RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                      mEventData->mRightSelectionPosition );
        }
      }
    }
    else if( Event::RIGHT_SELECTION_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::SELECTING );

      if( handleStopScrolling )
      {
        mEventData->mUpdateRightSelectionPosition = mEventData->mRightSelectionPosition != handlePosition;
        mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateRightSelectionPosition;
        mEventData->mRightSelectionPosition = handlePosition;

        if( mEventData->mUpdateRightSelectionPosition )
        {
          RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                      mEventData->mRightSelectionPosition );
        }
      }
    }

    mEventData->mDecoratorUpdated = true;
  } // end ( ( HANDLE_RELEASED == state ) || ( HANDLE_STOP_SCROLLING == state ) )
  else if( HANDLE_SCROLLING == state )
  {
    const float xSpeed = event.p2.mFloat;
    const Vector2& actualSize = mVisualModel->GetActualSize();

    mEventData->mScrollPosition.x += xSpeed;

    ClampHorizontalScroll( actualSize );

    const bool leftSelectionHandleEvent = Event::LEFT_SELECTION_HANDLE_EVENT == event.type;
    const bool rightSelectionHandleEvent = Event::RIGHT_SELECTION_HANDLE_EVENT == event.type;

    if( Event::GRAB_HANDLE_EVENT == event.type )
    {
      ChangeState( EventData::GRAB_HANDLE_PANNING );
    }
    else if( leftSelectionHandleEvent || rightSelectionHandleEvent )
    {
      // TODO: This is recalculating the selection box every time the text is scrolled with the selection handles.
      //       Think if something can be done to save power.

      ChangeState( EventData::SELECTION_HANDLE_PANNING );

      const Vector2& position = mEventData->mDecorator->GetPosition( leftSelectionHandleEvent ? Text::LEFT_SELECTION_HANDLE : Text::RIGHT_SELECTION_HANDLE );

      // Get the new handle position.
      // The selection handle's position is in decorator coords. Need to transforms to text coords.
      const CharacterIndex handlePosition = GetClosestCursorIndex( position.x - mEventData->mScrollPosition.x - mAlignmentOffset.x,
                                                                   position.y - mEventData->mScrollPosition.y - mAlignmentOffset.y );

      if( leftSelectionHandleEvent )
      {
        mEventData->mUpdateLeftSelectionPosition = handlePosition != mEventData->mLeftSelectionPosition;
        mEventData->mLeftSelectionPosition = handlePosition;
      }
      else
      {
        mEventData->mUpdateRightSelectionPosition = handlePosition != mEventData->mRightSelectionPosition;
        mEventData->mRightSelectionPosition = handlePosition;
      }

      if( mEventData->mUpdateLeftSelectionPosition || mEventData->mUpdateRightSelectionPosition )
      {
        RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                    mEventData->mRightSelectionPosition );
      }
    }
    mEventData->mDecoratorUpdated = true;
  } // end ( HANDLE_SCROLLING == state )
}

void Controller::Impl::RepositionSelectionHandles( CharacterIndex selectionStart, CharacterIndex selectionEnd )
{
  if( selectionStart == selectionEnd )
  {
    // Nothing to select if handles are in the same place.
    return;
  }

  mEventData->mDecorator->ClearHighlights();

  mEventData->mLeftSelectionPosition = selectionStart;
  mEventData->mRightSelectionPosition = selectionEnd;

  const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
  const Length* const glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const glyphsBuffer = mVisualModel->mGlyphs.Begin();
  const Vector2* const positionsBuffer = mVisualModel->mGlyphPositions.Begin();

  // TODO: Better algorithm to create the highlight box.
  // TODO: Multi-line.

  const Vector<LineRun>& lines = mVisualModel->mLines;
  const LineRun& firstLine = *lines.Begin();
  const float height = firstLine.ascender + -firstLine.descender;

  const bool indicesSwapped = ( selectionStart > selectionEnd );
  if( indicesSwapped )
  {
    std::swap( selectionStart, selectionEnd );
  }

  GlyphIndex glyphStart = *( charactersToGlyphBuffer + selectionStart );
  GlyphIndex glyphEnd = *( charactersToGlyphBuffer + ( selectionEnd - 1u ) ) + *( glyphsPerCharacterBuffer + ( selectionEnd - 1u ) ) - 1u;

  mEventData->mDecorator->SwapSelectionHandlesEnabled( firstLine.direction != indicesSwapped );

  const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;

  for( GlyphIndex index = glyphStart; index <= glyphEnd; ++index )
  {
    const GlyphInfo& glyph = *( glyphsBuffer + index );
    const Vector2& position = *( positionsBuffer + index );

    const float xPosition = position.x - glyph.xBearing + offset.x;
    mEventData->mDecorator->AddHighlight( xPosition, offset.y, xPosition + glyph.advance, height );
  }

  CursorInfo primaryCursorInfo;
  GetCursorPosition( mEventData->mLeftSelectionPosition,
                     primaryCursorInfo );

  CursorInfo secondaryCursorInfo;
  GetCursorPosition( mEventData->mRightSelectionPosition,
                     secondaryCursorInfo );

  const Vector2 primaryPosition = primaryCursorInfo.primaryPosition + offset;
  const Vector2 secondaryPosition = secondaryCursorInfo.primaryPosition + offset;

  mEventData->mDecorator->SetPosition( LEFT_SELECTION_HANDLE, primaryPosition.x, primaryPosition.y, primaryCursorInfo.lineHeight );

  mEventData->mDecorator->SetPosition( RIGHT_SELECTION_HANDLE, secondaryPosition.x, secondaryPosition.y, secondaryCursorInfo.lineHeight );

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
  if( 0 == numberOfGlyphs ||
      0 == numberOfLines )
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

  RepositionSelectionHandles( selectionStart, selectionEnd );
}

void Controller::Impl::ChangeState( EventData::State newState )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

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
    }
    else if ( EventData::SELECTING == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
      mEventData->mDecorator->StopCursorBlink();
      mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, true );
      mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, true );
      if( mEventData->mGrabHandlePopupEnabled )
      {
        TextSelectionPopup::Buttons selectedButtons = TextSelectionPopup::Buttons( TextSelectionPopup::COPY );
        mEventData->mDecorator->SetEnabledPopupButtons( selectedButtons );
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
    }
    else if( EventData::EDITING_WITH_POPUP == mEventData->mState )
    {
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
        TextSelectionPopup::Buttons selectionButtons = TextSelectionPopup::Buttons( TextSelectionPopup::SELECT | TextSelectionPopup::SELECT_ALL );
        mEventData->mDecorator->SetEnabledPopupButtons( selectionButtons );
        mEventData->mDecorator->SetPopupActive( true );
      }
      mEventData->mDecoratorUpdated = true;
    }
    else if ( EventData::SELECTION_HANDLE_PANNING == mEventData->mState )
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
    else if ( EventData::GRAB_HANDLE_PANNING == mEventData->mState )
    {
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
  if( hitCharacter >= mLogicalModel->mText.Count() )
  {
    // Selection out of bounds.
    return;
  }

  startIndex = hitCharacter;
  endIndex = hitCharacter;

  if( !TextAbstraction::IsWhiteSpace( mLogicalModel->mText[hitCharacter] ) )
  {
    // Find the start and end of the text
    for( startIndex = hitCharacter; startIndex > 0; --startIndex )
    {
      Character charCode = mLogicalModel->mText[ startIndex-1 ];
      if( TextAbstraction::IsWhiteSpace( charCode ) )
      {
        break;
      }
    }
    const CharacterIndex pastTheEnd = mLogicalModel->mText.Count();
    for( endIndex = hitCharacter + 1u; endIndex < pastTheEnd; ++endIndex )
    {
      Character charCode = mLogicalModel->mText[ endIndex ];
      if( TextAbstraction::IsWhiteSpace( charCode ) )
      {
        break;
      }
    }
  }
}

CharacterIndex Controller::Impl::GetClosestCursorIndex( float visualX,
                                                        float visualY )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return 0u;
  }

  CharacterIndex logicalIndex = 0u;

  const Length numberOfGlyphs = mVisualModel->mGlyphs.Count();
  const Length numberOfLines  = mVisualModel->mLines.Count();
  if( 0 == numberOfGlyphs ||
      0 == numberOfLines )
  {
    return logicalIndex;
  }

  // Find which line is closest
  const LineIndex lineIndex = GetClosestLine( visualY );
  const LineRun& line = mVisualModel->mLines[lineIndex];

  // Get the positions of the glyphs.
  const Vector<Vector2>& positions = mVisualModel->mGlyphPositions;
  const Vector2* const positionsBuffer = positions.Begin();

  // Get the visual to logical conversion tables.
  const CharacterIndex* const visualToLogicalBuffer = ( 0u != mLogicalModel->mVisualToLogicalMap.Count() ) ? mLogicalModel->mVisualToLogicalMap.Begin() : NULL;
  const CharacterIndex* const visualToLogicalCursorBuffer = mLogicalModel->mVisualToLogicalCursorMap.Begin();

  // Get the character to glyph conversion table.
  const GlyphIndex* const charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();

  // Get the glyphs per character table.
  const Length* const glyphsPerCharacterBuffer = mVisualModel->mGlyphsPerCharacter.Begin();

  // If the vector is void, there is no right to left characters.
  const bool hasRightToLeftCharacters = NULL != visualToLogicalBuffer;

  const CharacterIndex startCharacter = line.characterRun.characterIndex;
  const CharacterIndex endCharacter   = line.characterRun.characterIndex + line.characterRun.numberOfCharacters;
  DALI_ASSERT_DEBUG( endCharacter <= mLogicalModel->mText.Count() && "Invalid line info" );

  // Whether there is a hit on a glyph.
  bool matched = false;

  // Traverses glyphs in visual order. To do that use the visual to logical conversion table.
  CharacterIndex visualIndex = startCharacter;
  for( ; !matched && ( visualIndex < endCharacter ); ++visualIndex )
  {
    // The character in logical order.
    const CharacterIndex characterLogicalOrderIndex = hasRightToLeftCharacters ? *( visualToLogicalBuffer + visualIndex ) : visualIndex;

    // The first glyph for that character in logical order.
    const GlyphIndex glyphLogicalOrderIndex = *( charactersToGlyphBuffer + characterLogicalOrderIndex );

    // The number of glyphs for that character
    const Length numberOfGlyphs = *( glyphsPerCharacterBuffer + characterLogicalOrderIndex );

    // Get the metrics for the group of glyphs.
    GlyphMetrics glyphMetrics;
    GetGlyphsMetrics( glyphLogicalOrderIndex,
                      numberOfGlyphs,
                      glyphMetrics,
                      mVisualModel,
                      mFontClient );

    const Vector2& position = *( positionsBuffer + glyphLogicalOrderIndex );

    // Find the mid-point of the area containing the glyph
    const float glyphCenter = -glyphMetrics.xBearing + position.x + 0.5f * glyphMetrics.advance;

    if( visualX < glyphCenter )
    {
      matched = true;
      break;
    }
  }

  // Return the logical position of the cursor in characters.

  if( !matched )
  {
    visualIndex = endCharacter;
  }

  logicalIndex = hasRightToLeftCharacters ? *( visualToLogicalCursorBuffer + visualIndex ) : visualIndex;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "%p closest visualIndex %d logicalIndex %d\n", this, visualIndex, logicalIndex );
  return logicalIndex;
}

void Controller::Impl::GetCursorPosition( CharacterIndex logical,
                                          CursorInfo& cursorInfo )
{
  // TODO: Check for multiline with \n, etc...

  // Check if the logical position is the first or the last one of the text.
  const bool isFirstPosition = 0u == logical;
  const bool isLastPosition = mLogicalModel->GetNumberOfCharacters() == logical;

  if( isFirstPosition && isLastPosition )
  {
    // There is zero characters. Get the default font.

    FontId defaultFontId = 0u;
    if( NULL == mFontDefaults )
    {
      defaultFontId = mFontClient.GetFontId( EMPTY_STRING,
                                             EMPTY_STRING );
    }
    else
    {
      defaultFontId = mFontDefaults->GetFontId( mFontClient );
    }

    Text::FontMetrics fontMetrics;
    mFontClient.GetFontMetrics( defaultFontId, fontMetrics );

    cursorInfo.lineHeight = fontMetrics.ascender - fontMetrics.descender;
    cursorInfo.primaryCursorHeight = cursorInfo.lineHeight;

    cursorInfo.primaryPosition.x = 0.f;
    cursorInfo.primaryPosition.y = 0.f;

    // Nothing else to do.
    return;
  }

  // Get the previous logical index.
  const CharacterIndex previousLogical = isFirstPosition ? 0u : logical - 1u;

  // Decrease the logical index if it's the last one.
  if( isLastPosition )
  {
    --logical;
  }

  // Get the direction of the character and the previous one.
  const CharacterDirection* const modelCharacterDirectionsBuffer = ( 0u != mLogicalModel->mCharacterDirections.Count() ) ? mLogicalModel->mCharacterDirections.Begin() : NULL;

  CharacterDirection isCurrentRightToLeft = false;
  CharacterDirection isPreviousRightToLeft = false;
  if( NULL != modelCharacterDirectionsBuffer ) // If modelCharacterDirectionsBuffer is NULL, it means the whole text is left to right.
  {
    isCurrentRightToLeft = *( modelCharacterDirectionsBuffer + logical );
    isPreviousRightToLeft = *( modelCharacterDirectionsBuffer + previousLogical );
  }

  // Get the line where the character is laid-out.
  const LineRun* modelLines = mVisualModel->mLines.Begin();

  const LineIndex lineIndex = mVisualModel->GetLineOfCharacter( logical );
  const LineRun& line = *( modelLines + lineIndex );

  // Get the paragraph's direction.
  const CharacterDirection isRightToLeftParagraph = line.direction;

  // Check whether there is an alternative position:

  cursorInfo.isSecondaryCursor = ( isCurrentRightToLeft != isPreviousRightToLeft ) ||
    ( isLastPosition && ( isRightToLeftParagraph != isCurrentRightToLeft ) );

  // Set the line height.
  cursorInfo.lineHeight = line.ascender + -line.descender;

  // Convert the cursor position into the glyph position.
  CharacterIndex characterIndex = logical;
  if( cursorInfo.isSecondaryCursor &&
      ( isRightToLeftParagraph != isCurrentRightToLeft ) )
  {
    characterIndex = previousLogical;
  }

  const GlyphIndex currentGlyphIndex = *( mVisualModel->mCharactersToGlyph.Begin() + characterIndex );
  const Length numberOfGlyphs = *( mVisualModel->mGlyphsPerCharacter.Begin() + characterIndex );
  const Length numberOfCharacters = *( mVisualModel->mCharactersPerGlyph.Begin() +currentGlyphIndex );

  // Get the metrics for the group of glyphs.
  GlyphMetrics glyphMetrics;
  GetGlyphsMetrics( currentGlyphIndex,
                    numberOfGlyphs,
                    glyphMetrics,
                    mVisualModel,
                    mFontClient );

  float interGlyphAdvance = 0.f;
  if( !isLastPosition &&
      ( numberOfCharacters > 1u ) )
  {
    const CharacterIndex firstIndex = *( mVisualModel->mGlyphsToCharacters.Begin() + currentGlyphIndex );
    interGlyphAdvance = static_cast<float>( characterIndex - firstIndex ) * glyphMetrics.advance / static_cast<float>( numberOfCharacters );
  }

  // Get the glyph position and x bearing.
  const Vector2& currentPosition = *( mVisualModel->mGlyphPositions.Begin() + currentGlyphIndex );

  // Set the cursor's height.
  cursorInfo.primaryCursorHeight = glyphMetrics.fontHeight;

  // Set the position.
  cursorInfo.primaryPosition.x = -glyphMetrics.xBearing + currentPosition.x + ( isCurrentRightToLeft ? glyphMetrics.advance : interGlyphAdvance );
  cursorInfo.primaryPosition.y = line.ascender - glyphMetrics.ascender;

  if( isLastPosition )
  {
    // The position of the cursor after the last character needs special
    // care depending on its direction and the direction of the paragraph.

    if( cursorInfo.isSecondaryCursor )
    {
      // Need to find the first character after the last character with the paragraph's direction.
      // i.e l0 l1 l2 r0 r1 should find r0.

      // TODO: check for more than one line!
      characterIndex = isRightToLeftParagraph ? line.characterRun.characterIndex : line.characterRun.characterIndex + line.characterRun.numberOfCharacters - 1u;
      characterIndex = mLogicalModel->GetLogicalCharacterIndex( characterIndex );

      const GlyphIndex glyphIndex = *( mVisualModel->mCharactersToGlyph.Begin() + characterIndex );
      const Length numberOfGlyphs = *( mVisualModel->mGlyphsPerCharacter.Begin() + characterIndex );

      const Vector2& position = *( mVisualModel->mGlyphPositions.Begin() + glyphIndex );

      // Get the metrics for the group of glyphs.
      GlyphMetrics glyphMetrics;
      GetGlyphsMetrics( glyphIndex,
                        numberOfGlyphs,
                        glyphMetrics,
                        mVisualModel,
                        mFontClient );

      cursorInfo.primaryPosition.x = -glyphMetrics.xBearing + position.x + ( isRightToLeftParagraph ? 0.f : glyphMetrics.advance );

      cursorInfo.primaryPosition.y = line.ascender - glyphMetrics.ascender;
    }
    else
    {
      if( !isCurrentRightToLeft )
      {
        cursorInfo.primaryPosition.x += glyphMetrics.advance;
      }
      else
      {
        cursorInfo.primaryPosition.x -= glyphMetrics.advance;
      }
    }
  }

  // Set the alternative cursor position.
  if( cursorInfo.isSecondaryCursor )
  {
    // Convert the cursor position into the glyph position.
    const CharacterIndex previousCharacterIndex = ( ( isRightToLeftParagraph != isCurrentRightToLeft ) ? logical : previousLogical );
    const GlyphIndex previousGlyphIndex = *( mVisualModel->mCharactersToGlyph.Begin() + previousCharacterIndex );
    const Length numberOfGlyphs = *( mVisualModel->mGlyphsPerCharacter.Begin() + previousCharacterIndex );

    // Get the glyph position.
    const Vector2& previousPosition = *( mVisualModel->mGlyphPositions.Begin() + previousGlyphIndex );

    // Get the metrics for the group of glyphs.
    GlyphMetrics glyphMetrics;
    GetGlyphsMetrics( previousGlyphIndex,
                      numberOfGlyphs,
                      glyphMetrics,
                      mVisualModel,
                      mFontClient );

    // Set the cursor position and height.
    cursorInfo.secondaryPosition.x = -glyphMetrics.xBearing + previousPosition.x + ( ( ( isLastPosition && !isCurrentRightToLeft ) ||
                                                                                       ( !isLastPosition && isCurrentRightToLeft )    ) ? glyphMetrics.advance : 0.f );

    cursorInfo.secondaryCursorHeight = 0.5f * glyphMetrics.fontHeight;

    cursorInfo.secondaryPosition.y = cursorInfo.lineHeight - cursorInfo.secondaryCursorHeight - line.descender - ( glyphMetrics.fontHeight - glyphMetrics.ascender );

    // Update the primary cursor height as well.
    cursorInfo.primaryCursorHeight *= 0.5f;
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

  const Script script = mLogicalModel->GetScript( index );
  const GlyphIndex* charactersToGlyphBuffer = mVisualModel->mCharactersToGlyph.Begin();
  const Length* charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();

  Length numberOfCharacters = 0u;
  if( TextAbstraction::LATIN == script )
  {
    // Prevents to jump the whole Latin ligatures like fi, ff, ...
    numberOfCharacters = 1u;
  }
  else
  {
    GlyphIndex glyphIndex = *( charactersToGlyphBuffer + index );
    numberOfCharacters = *( charactersPerGlyphBuffer + glyphIndex );

    while( 0u == numberOfCharacters )
    {
      numberOfCharacters = *( charactersPerGlyphBuffer + glyphIndex );
      ++glyphIndex;
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

void Controller::Impl::UpdateCursorPosition()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::UpdateCursorPosition %p\n", this );
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition no event data\n" );
    return;
  }

  if( IsShowingPlaceholderText() )
  {
    // Do not want to use the place-holder text to set the cursor position.

    // Use the line's height of the font's family set to set the cursor's size.
    // If there is no font's family set, use the default font.
    // Use the current alignment to place the cursor at the beginning, center or end of the box.

    float lineHeight = 0.f;

    FontId defaultFontId = 0u;
    if( NULL == mFontDefaults )
    {
      defaultFontId = mFontClient.GetFontId( EMPTY_STRING,
                                             EMPTY_STRING );
    }
    else
    {
      defaultFontId = mFontDefaults->GetFontId( mFontClient );
    }

    Text::FontMetrics fontMetrics;
    mFontClient.GetFontMetrics( defaultFontId, fontMetrics );

    lineHeight = fontMetrics.ascender - fontMetrics.descender;


    Vector2 cursorPosition;

    switch( mLayoutEngine.GetHorizontalAlignment() )
    {
      case LayoutEngine::HORIZONTAL_ALIGN_BEGIN:
      {
        cursorPosition.x = 1.f;
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_CENTER:
      {
        cursorPosition.x = floor( 0.5f * mControlSize.width );
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_END:
      {
        cursorPosition.x = mControlSize.width;
        break;
      }
    }

    switch( mLayoutEngine.GetVerticalAlignment() )
    {
      case LayoutEngine::VERTICAL_ALIGN_TOP:
      {
        cursorPosition.y = 0.f;
        break;
      }
      case LayoutEngine::VERTICAL_ALIGN_CENTER:
      {
        cursorPosition.y = floorf( 0.5f * ( mControlSize.height - lineHeight ) );
        break;
      }
      case LayoutEngine::VERTICAL_ALIGN_BOTTOM:
      {
        cursorPosition.y = mControlSize.height - lineHeight;
        break;
      }
    }

    mEventData->mDecorator->SetPosition( PRIMARY_CURSOR,
                                         cursorPosition.x,
                                         cursorPosition.y,
                                         lineHeight,
                                         lineHeight );
  }
  else
  {
    CursorInfo cursorInfo;
    GetCursorPosition( mEventData->mPrimaryCursorPosition,
                       cursorInfo );

    const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;
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
                                         cursorPosition.y,
                                         cursorInfo.lineHeight );

    if( cursorInfo.isSecondaryCursor )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_BOTH );
      mEventData->mDecorator->SetPosition( SECONDARY_CURSOR,
                                           cursorInfo.secondaryPosition.x + offset.x,
                                           cursorInfo.secondaryPosition.y + offset.y,
                                           cursorInfo.secondaryCursorHeight,
                                           cursorInfo.lineHeight );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n", cursorInfo.secondaryPosition.x + offset.x, cursorInfo.secondaryPosition.y + offset.y );
    }
    else
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
    }
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition\n" );
}

void Controller::Impl::UpdateSelectionHandle( HandleType handleType )
{
  if( ( LEFT_SELECTION_HANDLE != handleType ) &&
      ( RIGHT_SELECTION_HANDLE != handleType ) )
  {
    return;
  }

  const bool leftSelectionHandle = LEFT_SELECTION_HANDLE == handleType;
  const CharacterIndex index = leftSelectionHandle ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition;

  CursorInfo cursorInfo;
  GetCursorPosition( index,
                     cursorInfo );

  const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;
  const Vector2 cursorPosition = cursorInfo.primaryPosition + offset;

  // Sets the grab handle position.
  mEventData->mDecorator->SetPosition( handleType,
                                       cursorPosition.x,
                                       cursorPosition.y,
                                       cursorInfo.lineHeight );
}

void Controller::Impl::ClampHorizontalScroll( const Vector2& actualSize )
{
  // Clamp between -space & 0 (and the text alignment).
  if( actualSize.width > mControlSize.width )
  {
    const float space = ( actualSize.width - mControlSize.width ) + mAlignmentOffset.x;
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
  if( actualSize.height > mControlSize.height )
  {
    const float space = ( actualSize.height - mControlSize.height ) + mAlignmentOffset.y;
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
  Vector2 offset;
  bool updateDecorator = false;
  if( position.x < 0.f )
  {
    offset.x = -position.x;
    mEventData->mScrollPosition.x += offset.x;
    updateDecorator = true;
  }
  else if( position.x > mControlSize.width )
  {
    offset.x = mControlSize.width - position.x;
    mEventData->mScrollPosition.x += offset.x;
    updateDecorator = true;
  }

  if( updateDecorator && mEventData->mDecorator )
  {
    mEventData->mDecorator->UpdatePositions( offset );
  }

  // TODO : calculate the vertical scroll.
}

void Controller::Impl::ScrollTextToMatchCursor()
{
  // Get the current cursor position in decorator coords.
  const Vector2& currentCursorPosition = mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

  // Calculate the new cursor position.
  CursorInfo cursorInfo;
  GetCursorPosition( mEventData->mPrimaryCursorPosition,
                     cursorInfo );

  // Calculate the offset to match the cursor position before the character was deleted.
  mEventData->mScrollPosition.x = currentCursorPosition.x - cursorInfo.primaryPosition.x - mAlignmentOffset.x;

  ClampHorizontalScroll( mVisualModel->GetActualSize() );
  bool updateCursorPosition = true;

  const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;
  const Vector2 cursorPosition = cursorInfo.primaryPosition + offset;

  if( updateCursorPosition )
  {
    // Sets the cursor position.
    mEventData->mDecorator->SetPosition( PRIMARY_CURSOR,
                                         cursorPosition.x,
                                         cursorPosition.y,
                                         cursorInfo.primaryCursorHeight,
                                         cursorInfo.lineHeight );

    // Sets the grab handle position.
    mEventData->mDecorator->SetPosition( GRAB_HANDLE,
                                         cursorPosition.x,
                                         cursorPosition.y,
                                         cursorInfo.lineHeight );

    if( cursorInfo.isSecondaryCursor )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_BOTH );
      mEventData->mDecorator->SetPosition( SECONDARY_CURSOR,
                                           cursorInfo.secondaryPosition.x + offset.x,
                                           cursorInfo.secondaryPosition.y + offset.y,
                                           cursorInfo.secondaryCursorHeight,
                                           cursorInfo.lineHeight );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n", cursorInfo.secondaryPosition.x + offset.x, cursorInfo.secondaryPosition.y + offset.y );
    }
    else
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
    }
  }
}

void Controller::Impl::RequestRelayout()
{
  mControlInterface.RequestTextRelayout();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
