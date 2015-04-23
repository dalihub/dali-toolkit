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

namespace
{

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
  mPlaceholderText(),
  mEventQueue(),
  mScrollPosition(),
  mState( INACTIVE ),
  mPrimaryCursorPosition( 0u ),
  mSecondaryCursorPosition( 0u ),
  mDecoratorUpdated( false ),
  mCursorBlinkEnabled( true ),
  mGrabHandleEnabled( true ),
  mGrabHandlePopupEnabled( true ),
  mSelectionEnabled( true ),
  mHorizontalScrollingEnabled( true ),
  mVerticalScrollingEnabled( false ),
  mUpdateCursorPosition( false )
{}

EventData::~EventData()
{}

bool Controller::Impl::ProcessInputEvents()
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return false;
  }

  mEventData->mDecoratorUpdated = false;

  if( mEventData->mDecorator )
  {
    for( std::vector<Event>::iterator iter = mEventData->mEventQueue.begin();
         iter != mEventData->mEventQueue.end();
         ++iter )
    {
      switch( iter->type )
      {
      case Event::KEYBOARD_FOCUS_GAIN_EVENT:
      {
        OnKeyboardFocus( true );
        break;
      }
      case Event::KEYBOARD_FOCUS_LOST_EVENT:
      {
        OnKeyboardFocus( false );
        break;
      }
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
      {
        OnGrabHandleEvent( *iter );
        break;
      }
      }
    }
  }

  // The cursor must also be repositioned after inserts into the model
  if( mEventData->mUpdateCursorPosition )
  {
    UpdateCursorPosition();
    mEventData->mUpdateCursorPosition = false;
  }

  mEventData->mEventQueue.clear();

  return mEventData->mDecoratorUpdated;
}

void Controller::Impl::OnKeyboardFocus( bool hasFocus )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  if( !hasFocus )
  {
    ChangeState( EventData::INACTIVE );
  }
  else
  {
    ChangeState( EventData::EDITING );
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

  UpdateCursorPosition();
}

void Controller::Impl::HandleCursorKey( int keyCode )
{
  // TODO
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }
}

void Controller::Impl::OnTapEvent( const Event& event )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  const unsigned int tapCount = event.p1.mUint;

  if( 1u == tapCount )
  {
    ChangeState( EventData::EDITING );

    const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
    const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

    mEventData->mPrimaryCursorPosition = GetClosestCursorIndex( xPosition,
                                                                yPosition );

    UpdateCursorPosition();
  }
  else if( mEventData->mSelectionEnabled &&
           ( 2u == tapCount ) )
  {
    ChangeState( EventData::SELECTING );

    RepositionSelectionHandles( event.p2.mFloat, event.p3.mFloat );
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

    if( mEventData->mVerticalScrollingEnabled )
    {
      const float displacementY = event.p3.mFloat;
      mEventData->mScrollPosition.y += displacementY;

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

    if( mEventData->mDecorator )
    {
      mEventData->mDecorator->UpdatePositions( mEventData->mScrollPosition - currentScroll );
    }
  }
}

void Controller::Impl::OnGrabHandleEvent( const Event& event )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  unsigned int state = event.p1.mUint;

  if( GRAB_HANDLE_PRESSED == state )
  {
    // The event.p2 and event.p3 are in decorator coords. Need to transforms to text coords.
    const float xPosition = event.p2.mFloat - mEventData->mScrollPosition.x - mAlignmentOffset.x;
    const float yPosition = event.p3.mFloat - mEventData->mScrollPosition.y - mAlignmentOffset.y;

    mEventData->mPrimaryCursorPosition = GetClosestCursorIndex( xPosition, yPosition );

    UpdateCursorPosition();

    //mDecorator->HidePopup();
    ChangeState ( EventData::EDITING );
  }
  else if( mEventData->mGrabHandlePopupEnabled &&
           ( GRAB_HANDLE_RELEASED == state ) )
  {
    //mDecorator->ShowPopup();
    ChangeState ( EventData::EDITING_WITH_POPUP );
    mEventData->mDecoratorUpdated = true;
  }
}

void Controller::Impl::RepositionSelectionHandles( float visualX, float visualY )
{
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  // TODO - Find which word was selected

  const Vector<GlyphInfo>& glyphs = mVisualModel->mGlyphs;
  const Vector<Vector2>::SizeType glyphCount = glyphs.Count();

  const Vector<Vector2>& positions = mVisualModel->mGlyphPositions;
  const Vector<Vector2>::SizeType positionCount = positions.Count();

  // Guard against glyphs which did not fit inside the layout
  const Vector<Vector2>::SizeType count = (positionCount < glyphCount) ? positionCount : glyphCount;

  if( count )
  {
    float primaryX   = positions[0].x + mEventData->mScrollPosition.x;
    float secondaryX = positions[count-1].x + glyphs[count-1].width + mEventData->mScrollPosition.x;

    // TODO - multi-line selection
    const Vector<LineRun>& lines = mVisualModel->mLines;
    float height = lines.Count() ? lines[0].ascender + -lines[0].descender : 0.0f;

    mEventData->mDecorator->SetPosition( PRIMARY_SELECTION_HANDLE,     primaryX, mEventData->mScrollPosition.y, height );
    mEventData->mDecorator->SetPosition( SECONDARY_SELECTION_HANDLE, secondaryX, mEventData->mScrollPosition.y, height );

    mEventData->mDecorator->ClearHighlights();
    mEventData->mDecorator->AddHighlight( primaryX, mEventData->mScrollPosition.y, secondaryX, height + mEventData->mScrollPosition.y );
  }
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
      mEventData->mDecorator->SetGrabHandleActive( false );
      mEventData->mDecorator->SetSelectionActive( false );
      mEventData->mDecorator->SetPopupActive( false );
      mEventData->mDecoratorUpdated = true;
    }
    else if ( EventData::SELECTING == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
      mEventData->mDecorator->StopCursorBlink();
      mEventData->mDecorator->SetGrabHandleActive( false );
      mEventData->mDecorator->SetSelectionActive( true );
      mEventData->mDecoratorUpdated = true;
    }
    else if( EventData::EDITING == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
      if( mEventData->mCursorBlinkEnabled )
      {
        mEventData->mDecorator->StartCursorBlink();
      }
      if( mEventData->mGrabHandleEnabled )
      {
        mEventData->mDecorator->SetGrabHandleActive( true );
      }
      if( mEventData->mGrabHandlePopupEnabled )
      {
        mEventData->mDecorator->SetPopupActive( false );
      }
      mEventData->mDecorator->SetSelectionActive( false );
      mEventData->mDecoratorUpdated = true;
    }
    else if( EventData::EDITING_WITH_POPUP == mEventData->mState )
    {
      mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
      if( mEventData->mCursorBlinkEnabled )
      {
        mEventData->mDecorator->StartCursorBlink();
      }
      if( mEventData->mGrabHandleEnabled )
      {
        mEventData->mDecorator->SetGrabHandleActive( true );
      }
      if( mEventData->mGrabHandlePopupEnabled )
      {
        mEventData->mDecorator->SetPopupActive( true );
      }
      mEventData->mDecorator->SetSelectionActive( false );
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

  return lineIndex-1;
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

    const float glyphX = -glyphMetrics.xBearing + position.x + 0.5f * glyphMetrics.advance;

    if( visualX < glyphX )
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

  return hasRightToLeftCharacters ? *( visualToLogicalCursorBuffer + visualIndex ) : visualIndex;
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
  if( NULL == mEventData )
  {
    // Nothing to do if there is no text input.
    return;
  }

  CursorInfo cursorInfo;
  GetCursorPosition( mEventData->mPrimaryCursorPosition,
                     cursorInfo );

  mEventData->mDecorator->SetPosition( PRIMARY_CURSOR,
                                       cursorInfo.primaryPosition.x + mEventData->mScrollPosition.x + mAlignmentOffset.x,
                                       cursorInfo.primaryPosition.y + mEventData->mScrollPosition.y + mAlignmentOffset.y,
                                       cursorInfo.primaryCursorHeight,
                                       cursorInfo.lineHeight );

  if( cursorInfo.isSecondaryCursor )
  {
    mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_BOTH );
    mEventData->mDecorator->SetPosition( SECONDARY_CURSOR,
                                         cursorInfo.secondaryPosition.x + mEventData->mScrollPosition.x + mAlignmentOffset.x,
                                         cursorInfo.secondaryPosition.y + mEventData->mScrollPosition.y + mAlignmentOffset.y,
                                         cursorInfo.secondaryCursorHeight,
                                         cursorInfo.lineHeight );
  }
  else
  {
    mEventData->mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
  }

  mEventData->mUpdateCursorPosition = false;
  mEventData->mDecoratorUpdated = true;
}

void Controller::Impl::RequestRelayout()
{
  mControlInterface.RequestTextRelayout();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
