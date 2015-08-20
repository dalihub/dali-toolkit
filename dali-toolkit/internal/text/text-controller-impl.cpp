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
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>

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
 * @param[in] visualModel The visual model.
 * @param[in] fontClient The font client.
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
  mScrollAfterDelete( false ),
  mAllTextSelected( false )
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

      SetPopupButtons();
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

      SetPopupButtons();
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
  Length numberOfParagraphs = 0u;
  if( BIDI_INFO & operations )
  {
    // Count the number of LINE_NO_BREAK to reserve some space for the vector of paragraph's
    // bidirectional info.

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

      textMirrored = GetMirroredText( utf32Characters,
                                      mirroredUtf32Characters,
                                      bidirectionalInfo );

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
  Vector<GlyphIndex> newParagraphGlyphs;
  newParagraphGlyphs.Reserve( numberOfParagraphs );

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
               charactersPerGlyph,
               newParagraphGlyphs );

    // Create the 'number of glyphs' per character and the glyph to character conversion tables.
    mVisualModel->CreateGlyphsPerCharacterTable( numberOfCharacters );
    mVisualModel->CreateCharacterToGlyphTable( numberOfCharacters );
  }

  const Length numberOfGlyphs = glyphs.Count();

  if( GET_GLYPH_METRICS & operations )
  {
    GlyphInfo* glyphsBuffer = glyphs.Begin();
    mFontClient.GetGlyphMetrics( glyphsBuffer, numberOfGlyphs );

    // Update the width and advance of all new paragraph characters.
    for( Vector<GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(), endIt = newParagraphGlyphs.End(); it != endIt; ++it )
    {
      const GlyphIndex index = *it;
      GlyphInfo& glyph = *( glyphsBuffer + index );

      glyph.xBearing = 0.f;
      glyph.width = 0.f;
      glyph.advance = 0.f;
    }
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

float Controller::Impl::GetDefaultFontLineHeight()
{
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

void Controller::Impl::OnLongPressEvent( const Event& event )
{
  if  ( EventData::EDITING == mEventData->mState )
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

        RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                    mEventData->mRightSelectionPosition );

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
        mEventData->mUpdateLeftSelectionPosition = ( mEventData->mRightSelectionPosition != handlePosition );
        mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateLeftSelectionPosition;

        if( mEventData->mUpdateLeftSelectionPosition )
        {
          mEventData->mLeftSelectionPosition = handlePosition;

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
        mEventData->mUpdateRightSelectionPosition = ( mEventData->mLeftSelectionPosition != handlePosition );
        mEventData->mScrollAfterUpdatePosition = mEventData->mUpdateRightSelectionPosition;
        if( mEventData->mUpdateRightSelectionPosition )
        {
          mEventData->mRightSelectionPosition = handlePosition;
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
        RepositionSelectionHandles( mEventData->mLeftSelectionPosition,
                                    mEventData->mRightSelectionPosition );

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

    const CharacterIndex leftPosition = mEventData->mLeftSelectionPosition;
    const CharacterIndex rightPosition = mEventData->mRightSelectionPosition;

    RepositionSelectionHandles( xPosition,
                                yPosition );

    mEventData->mUpdateLeftSelectionPosition = leftPosition != mEventData->mLeftSelectionPosition;
    mEventData->mUpdateRightSelectionPosition = rightPosition != mEventData->mRightSelectionPosition;

    mEventData->mScrollAfterUpdatePosition = ( ( mEventData->mUpdateLeftSelectionPosition || mEventData->mUpdateRightSelectionPosition ) &&
                                               ( mEventData->mLeftSelectionPosition != mEventData->mRightSelectionPosition ) );
  }
}

void Controller::Impl::OnSelectAllEvent()
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text.
    return;
  }

  if( mEventData->mSelectionEnabled )
  {
    RepositionSelectionHandles( 0u,
                                mLogicalModel->mText.Count() );

    mEventData->mScrollAfterUpdatePosition = true;
    mEventData->mUpdateLeftSelectionPosition = true;
    mEventData->mUpdateRightSelectionPosition = true;
  }
}

void Controller::Impl::RetrieveSelection( std::string& selectedText, bool deleteAfterRetreival )
{
  if( mEventData->mLeftSelectionPosition ==  mEventData->mRightSelectionPosition )
  {
    // Nothing to select if handles are in the same place.
    selectedText="";
    return;
  }

  //Get start and end position of selection
  uint32_t startOfSelectedText = mEventData->mLeftSelectionPosition;
  uint32_t lengthOfSelectedText =  mEventData->mRightSelectionPosition - startOfSelectedText;

  // Validate the start and end selection points
  if( ( startOfSelectedText >= 0 ) && (  ( startOfSelectedText + lengthOfSelectedText ) <=  mLogicalModel->mText.Count() ) )
  {
    //Get text as a UTF8 string
    Vector<Character>& utf32Characters = mLogicalModel->mText;

    Utf32ToUtf8( &utf32Characters[startOfSelectedText], lengthOfSelectedText, selectedText );

    if ( deleteAfterRetreival  ) // Only delete text if copied successfully
    {
      // Delete text between handles
      Vector<Character>& currentText = mLogicalModel->mText;

      Vector<Character>::Iterator first = currentText.Begin() + startOfSelectedText;
      Vector<Character>::Iterator last  = first + lengthOfSelectedText;
      currentText.Erase( first, last );
    }
    mEventData->mPrimaryCursorPosition = mEventData->mLeftSelectionPosition;
    mEventData->mScrollAfterDelete = true;
    mEventData->mDecoratorUpdated = true;
  }
}

void Controller::Impl::ShowClipboard()
{
  if ( mClipboard )
  {
    mClipboard.ShowClipboard();
  }
}

void Controller::Impl::HideClipboard()
{
  if ( mClipboard )
  {
    mClipboard.HideClipboard();
  }
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

void Controller::Impl::GetTextFromClipboard( unsigned int itemIndex, std::string& retreivedString )
{
  if ( mClipboard )
  {
    retreivedString =  mClipboard.GetItem( itemIndex );
  }
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
  const Length* const charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const glyphToCharacterBuffer = mVisualModel->mGlyphsToCharacters.Begin();
  const CharacterDirection* const modelCharacterDirectionsBuffer = ( 0u != mLogicalModel->mCharacterDirections.Count() ) ? mLogicalModel->mCharacterDirections.Begin() : NULL;

  // TODO: Better algorithm to create the highlight box.
  // TODO: Multi-line.

  // Get the height of the line.
  const Vector<LineRun>& lines = mVisualModel->mLines;
  const LineRun& firstLine = *lines.Begin();
  const float height = firstLine.ascender + -firstLine.descender;

  // Swap the indices if the start is greater than the end.
  const bool indicesSwapped = ( selectionStart > selectionEnd );
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

  // Tell the decorator to swap the selection handles if needed.
  mEventData->mDecorator->SwapSelectionHandlesEnabled( firstLine.direction != indicesSwapped );

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

  mEventData->mDecorator->SetPosition( LEFT_SELECTION_HANDLE, primaryPosition.x, primaryPosition.y, primaryCursorInfo.lineHeight );

  mEventData->mDecorator->SetPosition( RIGHT_SELECTION_HANDLE, secondaryPosition.x, secondaryPosition.y, secondaryCursorInfo.lineHeight );

  // Cursor to be positioned at end of selection so if selection interrupted and edit mode restarted the cursor will be at end of selection
  mEventData->mPrimaryCursorPosition = (indicesSwapped)?mEventData->mLeftSelectionPosition:mEventData->mRightSelectionPosition;

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

  if ( ( EventData::SELECTING == mEventData->mState ) || ( EventData::SELECTION_CHANGED == mEventData->mState ) )
  {
    buttonsToShow = TextSelectionPopup::Buttons(  TextSelectionPopup::CUT | TextSelectionPopup::COPY );

    if ( !IsClipboardEmpty() )
    {
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::PASTE ) );
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::CLIPBOARD ) );
    }

    if ( !mEventData->mAllTextSelected )
    {
      buttonsToShow = TextSelectionPopup::Buttons ( ( buttonsToShow | TextSelectionPopup::SELECT_ALL ) );
    }
  }
  else if  ( EventData::EDITING_WITH_POPUP == mEventData->mState )
  {
    if ( mLogicalModel->mText.Count() && !IsShowingPlaceholderText())
    {
      buttonsToShow = TextSelectionPopup::Buttons( TextSelectionPopup::SELECT | TextSelectionPopup::SELECT_ALL );
    }

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
    else if ( EventData::INTERRUPTED  == mEventData->mState)
    {
      mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
      mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );
      mEventData->mDecorator->SetPopupActive( false );
      mEventData->mDecoratorUpdated = true;
      HideClipboard();
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
        SetPopupButtons();
        mEventData->mDecorator->SetPopupActive( true );
      }
      mEventData->mDecoratorUpdated = true;
    }
    else if ( EventData::SELECTION_CHANGED  == mEventData->mState )
    {
      if( mEventData->mGrabHandlePopupEnabled )
      {
        SetPopupButtons();
        mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
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
  const Length* const charactersPerGlyphBuffer = mVisualModel->mCharactersPerGlyph.Begin();

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

    // Get the script of the character.
    const Script script = mLogicalModel->GetScript( characterLogicalOrderIndex );

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

    // Prevents to jump the whole Latin ligatures like fi, ff, or Arabic ﻻ...
    const Length numberOfCharactersInLigature = HasLigatureMustBreak( script ) ? *( charactersPerGlyphBuffer + glyphLogicalOrderIndex ) : 1u;
    const float glyphAdvance = glyphMetrics.advance / static_cast<float>( numberOfCharactersInLigature );

    for( GlyphIndex index = 0u; !matched && ( index < numberOfCharactersInLigature ); ++index )
    {
      // Find the mid-point of the area containing the glyph
      const float glyphCenter = -glyphMetrics.xBearing + position.x + ( static_cast<float>( index ) + 0.5f ) * glyphAdvance;

      if( visualX < glyphCenter )
      {
        visualIndex += index;
        matched = true;
        break;
      }
    }

    if( matched )
    {
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
  const bool isLastPosition = mLogicalModel->mText.Count() == logical;

  if( isFirstPosition && isLastPosition )
  {
    // There is zero characters. Get the default font's line height.
    cursorInfo.lineHeight = GetDefaultFontLineHeight();
    cursorInfo.primaryCursorHeight = cursorInfo.lineHeight;

    cursorInfo.primaryPosition.x = mEventData->mDecorator->GetCursorWidth();
    cursorInfo.primaryPosition.y = 0.f;

    // Nothing else to do.
    return;
  }

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

  // Set the line height.
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

  // Convert the cursor position into the glyph position.
  const GlyphIndex primaryGlyphIndex = *( charactersToGlyphBuffer + index );
  const Length primaryNumberOfGlyphs = *( glyphsPerCharacterBuffer + index );
  const Length primaryNumberOfCharacters = *( charactersPerGlyphBuffer + primaryGlyphIndex );

  // Get the metrics for the group of glyphs.
  GlyphMetrics glyphMetrics;
  GetGlyphsMetrics( primaryGlyphIndex,
                    primaryNumberOfGlyphs,
                    glyphMetrics,
                    mVisualModel,
                    mFontClient );

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
                      mVisualModel,
                      mFontClient );

    // Set the secondary cursor's position.
    cursorInfo.secondaryPosition.x = -glyphMetrics.xBearing + secondaryPosition.x + ( isCurrentRightToLeft ? 0.f : glyphMetrics.advance );
    cursorInfo.secondaryPosition.y = cursorInfo.lineHeight - cursorInfo.secondaryCursorHeight - line.descender - ( glyphMetrics.fontHeight - glyphMetrics.ascender );
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

void Controller::Impl::UpdateCursorPosition()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::UpdateCursorPosition %p\n", this );
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition no event data\n" );
    return;
  }

  if( IsShowingPlaceholderText() || ( 0u == mLogicalModel->mText.Count() ) )
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
        cursorPosition.x = mEventData->mDecorator->GetCursorWidth();
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_CENTER:
      {
        cursorPosition.x = floor( 0.5f * mVisualModel->mControlSize.width );
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_END:
      {
        cursorPosition.x = mVisualModel->mControlSize.width;
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
        cursorPosition.y = floorf( 0.5f * ( mVisualModel->mControlSize.height - lineHeight ) );
        break;
      }
      case LayoutEngine::VERTICAL_ALIGN_BOTTOM:
      {
        cursorPosition.y = mVisualModel->mControlSize.height - lineHeight;
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
  Vector2 offset;
  bool updateDecorator = false;
  if( position.x < 0.f )
  {
    offset.x = -position.x;
    mEventData->mScrollPosition.x += offset.x;
    updateDecorator = true;
  }
  else if( position.x > mVisualModel->mControlSize.width )
  {
    offset.x = mVisualModel->mControlSize.width - position.x;
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

  const Vector2 offset = mEventData->mScrollPosition + mAlignmentOffset;
  const Vector2 cursorPosition = cursorInfo.primaryPosition + offset;

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
    mEventData->mDecorator->SetPosition( SECONDARY_CURSOR,
                                         cursorInfo.secondaryPosition.x + offset.x,
                                         cursorInfo.secondaryPosition.y + offset.y,
                                         cursorInfo.secondaryCursorHeight,
                                         cursorInfo.lineHeight );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n", cursorInfo.secondaryPosition.x + offset.x, cursorInfo.secondaryPosition.y + offset.y );
  }

  // Set which cursors are active according the state.
  if( ( EventData::EDITING == mEventData->mState ) ||
      ( EventData::EDITING_WITH_POPUP == mEventData->mState ) ||
      ( EventData::GRAB_HANDLE_PANNING == mEventData->mState ) )
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
}

void Controller::Impl::RequestRelayout()
{
  mControlInterface.RequestTextRelayout();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
