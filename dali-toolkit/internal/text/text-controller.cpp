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
#include <dali-toolkit/internal/text/text-controller.h>

// EXTERNAL INCLUDES
#include <limits>
#include <iostream>
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
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-io.h>
#include <dali-toolkit/internal/text/text-view.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

const float MAX_FLOAT = std::numeric_limits<float>::max();

const std::string EMPTY_STRING("");

float ConvertToEven( float value )
{
  int intValue(static_cast<int>( value ));
  return static_cast<float>(intValue % 2 == 0) ? intValue : (intValue + 1);
}

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

ControllerPtr Controller::New( ControlInterface& controlInterface )
{
  return ControllerPtr( new Controller( controlInterface ) );
}

void Controller::EnableTextInput( DecoratorPtr decorator )
{
  if( !mImpl->mEventData )
  {
    mImpl->mEventData = new EventData( decorator );
  }
}

void Controller::SetText( const std::string& text )
{
  // Remove the previously set text
  ResetText();

  CharacterIndex lastCursorIndex = 0u;

  if( !text.empty() )
  {
    //  Convert text into UTF-32
    Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;
    utf32Characters.Resize( text.size() );

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32( utf8, text.size(), utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    DALI_ASSERT_DEBUG( text.size() >= characterCount && "Invalid UTF32 conversion length" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::SetText %p UTF8 size %d, UTF32 size %d\n", this, text.size(), mImpl->mLogicalModel->mText.Count() );

    // To reset the cursor position
    lastCursorIndex = characterCount;

    // Update the rest of the model during size negotiation
    mImpl->QueueModifyEvent( ModifyEvent::TEXT_REPLACED );

    // The natural size needs to be re-calculated.
    mImpl->mRecalculateNaturalSize = true;

    // Apply modifications to the model
    mImpl->mOperationsPending = ALL_OPERATIONS;
  }
  else
  {
    ShowPlaceholderText();
  }

  // Resets the cursor position.
  ResetCursorPosition( lastCursorIndex );

  // Scrolls the text to make the cursor visible.
  ResetScrollPosition();

  mImpl->RequestRelayout();

  if( mImpl->mEventData )
  {
    // Cancel previously queued events
    mImpl->mEventData->mEventQueue.clear();
  }

  // Reset keyboard as text changed
  mImpl->ResetImfManager();

  // Do this last since it provides callbacks into application code
  mImpl->mControlInterface.TextChanged();
}

void Controller::GetText( std::string& text ) const
{
  if( ! mImpl->IsShowingPlaceholderText() )
  {
    Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;

    if( 0u != utf32Characters.Count() )
    {
      Utf32ToUtf8( &utf32Characters[0], utf32Characters.Count(), text );
    }
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::GetText %p empty (but showing placeholder)\n", this );
  }
}

unsigned int Controller::GetLogicalCursorPosition() const
{
  if( mImpl->mEventData )
  {
    return mImpl->mEventData->mPrimaryCursorPosition;
  }

  return 0u;
}

void Controller::SetPlaceholderText( PlaceholderType type, const std::string& text )
{
  if( mImpl->mEventData )
  {
    if( PLACEHOLDER_TYPE_INACTIVE == type )
    {
      mImpl->mEventData->mPlaceholderTextInactive = text;
    }
    else
    {
      mImpl->mEventData->mPlaceholderTextActive = text;
    }

    // Update placeholder if there is no text
    if( mImpl->IsShowingPlaceholderText() ||
        0u == mImpl->mLogicalModel->mText.Count() )
    {
      ShowPlaceholderText();
    }
  }
}

void Controller::GetPlaceholderText( PlaceholderType type, std::string& text ) const
{
  if( mImpl->mEventData )
  {
    if( PLACEHOLDER_TYPE_INACTIVE == type )
    {
      text = mImpl->mEventData->mPlaceholderTextInactive;
    }
    else
    {
      text = mImpl->mEventData->mPlaceholderTextActive;
    }
  }
}

void Controller::SetMaximumNumberOfCharacters( int maxCharacters )
{
  if ( maxCharacters >= 0 )
  {
    mImpl->mMaximumNumberOfCharacters = maxCharacters;
  }
}

int Controller::GetMaximumNumberOfCharacters()
{
  return mImpl->mMaximumNumberOfCharacters;
}

void Controller::SetDefaultFontFamily( const std::string& defaultFontFamily )
{
  if( !mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultFontFamily = defaultFontFamily;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

const std::string& Controller::GetDefaultFontFamily() const
{
  if( mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mDefaultFontFamily;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultFontStyle( const std::string& defaultFontStyle )
{
  if( !mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultFontStyle = defaultFontStyle;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

const std::string& Controller::GetDefaultFontStyle() const
{
  if( mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mDefaultFontStyle;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultPointSize( float pointSize )
{
  if( !mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultPointSize = pointSize;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

float Controller::GetDefaultPointSize() const
{
  if( mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mDefaultPointSize;
  }

  return 0.0f;
}

void Controller::SetTextColor( const Vector4& textColor )
{
  mImpl->mTextColor = textColor;

  if( !mImpl->IsShowingPlaceholderText() )
  {
    mImpl->mVisualModel->SetTextColor( textColor );

    mImpl->RequestRelayout();
  }
}

const Vector4& Controller::GetTextColor() const
{
  return mImpl->mTextColor;
}

bool Controller::RemoveText( int cursorOffset, int numberOfChars )
{
  bool removed( false );

  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::RemoveText %p mText.Count() %d cursor %d cursorOffset %d numberOfChars %d\n",
                 this, mImpl->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition, cursorOffset, numberOfChars );

  if( ! mImpl->IsShowingPlaceholderText() )
  {
    // Delete at current cursor position
    Vector<Character>& currentText = mImpl->mLogicalModel->mText;
    CharacterIndex& oldCursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

    CharacterIndex cursorIndex = oldCursorIndex;

    // Validate the cursor position & number of characters
    if( static_cast< CharacterIndex >( std::abs( cursorOffset ) ) <= cursorIndex )
    {
      cursorIndex = oldCursorIndex + cursorOffset;
    }

    if( (cursorIndex + numberOfChars) > currentText.Count() )
    {
      numberOfChars = currentText.Count() - cursorIndex;
    }

    if( (cursorIndex + numberOfChars) <= currentText.Count() )
    {
      Vector<Character>::Iterator first = currentText.Begin() + cursorIndex;
      Vector<Character>::Iterator last  = first + numberOfChars;

      currentText.Erase( first, last );

      // Cursor position retreat
      oldCursorIndex = cursorIndex;

      DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::RemoveText %p removed %d\n", this, numberOfChars );
      removed = true;
    }
  }

  return removed;
}

void Controller::SetPlaceholderTextColor( const Vector4& textColor )
{
  if( mImpl->mEventData )
  {
    mImpl->mEventData->mPlaceholderTextColor = textColor;
  }

  if( mImpl->IsShowingPlaceholderText() )
  {
    mImpl->mVisualModel->SetTextColor( textColor );
    mImpl->RequestRelayout();
  }
}

const Vector4& Controller::GetPlaceholderTextColor() const
{
  if( mImpl->mEventData )
  {
    return mImpl->mEventData->mPlaceholderTextColor;
  }

  return Color::BLACK;
}

void Controller::SetShadowOffset( const Vector2& shadowOffset )
{
  mImpl->mVisualModel->SetShadowOffset( shadowOffset );

  mImpl->RequestRelayout();
}

const Vector2& Controller::GetShadowOffset() const
{
  return mImpl->mVisualModel->GetShadowOffset();
}

void Controller::SetShadowColor( const Vector4& shadowColor )
{
  mImpl->mVisualModel->SetShadowColor( shadowColor );

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetShadowColor() const
{
  return mImpl->mVisualModel->GetShadowColor();
}

void Controller::SetUnderlineColor( const Vector4& color )
{
  mImpl->mVisualModel->SetUnderlineColor( color );

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetUnderlineColor() const
{
  return mImpl->mVisualModel->GetUnderlineColor();
}

void Controller::SetUnderlineEnabled( bool enabled )
{
  mImpl->mVisualModel->SetUnderlineEnabled( enabled );

  mImpl->RequestRelayout();
}

bool Controller::IsUnderlineEnabled() const
{
  return mImpl->mVisualModel->IsUnderlineEnabled();
}

void Controller::SetUnderlineHeight( float height )
{
  mImpl->mVisualModel->SetUnderlineHeight( height );

  mImpl->RequestRelayout();
}

float Controller::GetUnderlineHeight() const
{
  return mImpl->mVisualModel->GetUnderlineHeight();
}

void Controller::SetEnableCursorBlink( bool enable )
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "TextInput disabled" );

  if( mImpl->mEventData )
  {
    mImpl->mEventData->mCursorBlinkEnabled = enable;

    if( !enable &&
        mImpl->mEventData->mDecorator )
    {
      mImpl->mEventData->mDecorator->StopCursorBlink();
    }
  }
}

bool Controller::GetEnableCursorBlink() const
{
  if( mImpl->mEventData )
  {
    return mImpl->mEventData->mCursorBlinkEnabled;
  }

  return false;
}

const Vector2& Controller::GetScrollPosition() const
{
  if( mImpl->mEventData )
  {
    return mImpl->mEventData->mScrollPosition;
  }

  return Vector2::ZERO;
}

const Vector2& Controller::GetAlignmentOffset() const
{
  return mImpl->mAlignmentOffset;
}

Vector3 Controller::GetNaturalSize()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::GetNaturalSize\n" );
  Vector3 naturalSize;

  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();

  if( mImpl->mRecalculateNaturalSize )
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>( CONVERT_TO_UTF32  |
                                                                           GET_SCRIPTS       |
                                                                           VALIDATE_FONTS    |
                                                                           GET_LINE_BREAKS   |
                                                                           GET_WORD_BREAKS   |
                                                                           BIDI_INFO         |
                                                                           SHAPE_TEXT        |
                                                                           GET_GLYPH_METRICS );
    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel( onlyOnceOperations );

    // Operations that need to be done if the size changes.
    const OperationsMask sizeOperations =  static_cast<OperationsMask>( LAYOUT |
                                                                        ALIGN  |
                                                                        REORDER );

    DoRelayout( Size( MAX_FLOAT, MAX_FLOAT ),
                static_cast<OperationsMask>( onlyOnceOperations |
                                             sizeOperations ),
                naturalSize.GetVectorXY() );

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending & ~onlyOnceOperations );

    // Do the size related operations again.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | sizeOperations );

    // Stores the natural size to avoid recalculate it again
    // unless the text/style changes.
    mImpl->mVisualModel->SetNaturalSize( naturalSize.GetVectorXY() );

    mImpl->mRecalculateNaturalSize = false;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize calculated %f,%f,%f\n", naturalSize.x, naturalSize.y, naturalSize.z );
  }
  else
  {
    naturalSize = mImpl->mVisualModel->GetNaturalSize();

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize cached %f,%f,%f\n", naturalSize.x, naturalSize.y, naturalSize.z );
  }

  naturalSize.x = ConvertToEven( naturalSize.x );
  naturalSize.y = ConvertToEven( naturalSize.y );

  return naturalSize;
}

float Controller::GetHeightForWidth( float width )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::GetHeightForWidth %p width %f\n", this, width );
  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();

  Size layoutSize;
  if( width != mImpl->mControlSize.width )
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>( CONVERT_TO_UTF32  |
                                                                           GET_SCRIPTS       |
                                                                           VALIDATE_FONTS    |
                                                                           GET_LINE_BREAKS   |
                                                                           GET_WORD_BREAKS   |
                                                                           BIDI_INFO         |
                                                                           SHAPE_TEXT        |
                                                                           GET_GLYPH_METRICS );
    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel( onlyOnceOperations );

    // Operations that need to be done if the size changes.
    const OperationsMask sizeOperations =  static_cast<OperationsMask>( LAYOUT |
                                                                        ALIGN  |
                                                                        REORDER );

    DoRelayout( Size( width, MAX_FLOAT ),
                static_cast<OperationsMask>( onlyOnceOperations |
                                             sizeOperations ),
                layoutSize );

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending & ~onlyOnceOperations );

    // Do the size related operations again.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | sizeOperations );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height );
  }
  else
  {
    layoutSize = mImpl->mVisualModel->GetActualSize();
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth cached %f\n", layoutSize.height );
  }

  return layoutSize.height;
}

bool Controller::Relayout( const Size& size )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::Relayout %p size %f,%f\n", this, size.width, size.height );

  if( ( size.width < Math::MACHINE_EPSILON_1000 ) || ( size.height < Math::MACHINE_EPSILON_1000 ) )
  {
    bool glyphsRemoved( false );
    if( 0u != mImpl->mVisualModel->mGlyphPositions.Count() )
    {
      mImpl->mVisualModel->mGlyphPositions.Clear();
      glyphsRemoved = true;
    }
    // Not worth to relayout if width or height is equal to zero.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::Relayout (skipped)\n" );
    return glyphsRemoved;
  }

  if( size != mImpl->mControlSize )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "new size (previous size %f,%f)\n", mImpl->mControlSize.width, mImpl->mControlSize.height );

    // Operations that need to be done if the size changes.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                             LAYOUT                    |
                                                             ALIGN                     |
                                                             UPDATE_ACTUAL_SIZE        |
                                                             REORDER );

    mImpl->mControlSize = size;
  }

  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();
  mImpl->UpdateModel( mImpl->mOperationsPending );

  Size layoutSize;
  bool updated = DoRelayout( mImpl->mControlSize,
                             mImpl->mOperationsPending,
                             layoutSize );

  // Do not re-do any operation until something changes.
  mImpl->mOperationsPending = NO_OPERATION;

  // After doing the text layout, the alignment offset to place the actor in the desired position can be calculated.
  CalculateTextAlignment( size );

  if( mImpl->mEventData )
  {
    // Move the cursor, grab handle etc.
    updated = mImpl->ProcessInputEvents() || updated;
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::Relayout\n" );
  return updated;
}

void Controller::ProcessModifyEvents()
{
  std::vector<ModifyEvent>& events = mImpl->mModifyEvents;

  for( unsigned int i=0; i<events.size(); ++i )
  {
    if( ModifyEvent::TEXT_REPLACED == events[0].type )
    {
      // A (single) replace event should come first, otherwise we wasted time processing NOOP events
      DALI_ASSERT_DEBUG( 0 == i && "Unexpected TEXT_REPLACED event" );

      TextReplacedEvent();
    }
    else if( ModifyEvent::TEXT_INSERTED == events[0].type )
    {
      TextInsertedEvent();
    }
    else if( ModifyEvent::TEXT_DELETED == events[0].type )
    {
      // Placeholder-text cannot be deleted
      if( !mImpl->IsShowingPlaceholderText() )
      {
        TextDeletedEvent();
      }
    }
  }

  // Discard temporary text
  events.clear();
}

void Controller::ResetText()
{
  // Reset buffers.
  mImpl->mLogicalModel->mText.Clear();
  ClearModelData();

  // We have cleared everything including the placeholder-text
  mImpl->PlaceholderCleared();

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::ResetCursorPosition( CharacterIndex cursorIndex )
{
  // Reset the cursor position
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mPrimaryCursorPosition = cursorIndex;

    // Update the cursor if it's in editing mode.
    if( ( EventData::EDITING == mImpl->mEventData->mState ) ||
        ( EventData::EDITING_WITH_POPUP == mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mUpdateCursorPosition = true;
    }
  }
}

void Controller::ResetScrollPosition()
{
  if( NULL != mImpl->mEventData )
  {
    // Reset the scroll position.
    mImpl->mEventData->mScrollPosition = Vector2::ZERO;
    mImpl->mEventData->mScrollAfterUpdatePosition = true;
  }
}

void Controller::TextReplacedEvent()
{
  // Reset buffers.
  ClearModelData();

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->UpdateModel( ALL_OPERATIONS );
  mImpl->mOperationsPending = static_cast<OperationsMask>( LAYOUT             |
                                                           ALIGN              |
                                                           UPDATE_ACTUAL_SIZE |
                                                           REORDER );
}

void Controller::TextInsertedEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected TextInsertedEvent" );

  // TODO - Optimize this
  ClearModelData();

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->UpdateModel( ALL_OPERATIONS );
  mImpl->mOperationsPending = static_cast<OperationsMask>( LAYOUT             |
                                                           ALIGN              |
                                                           UPDATE_ACTUAL_SIZE |
                                                           REORDER );

  // Queue a cursor reposition event; this must wait until after DoRelayout()
  mImpl->mEventData->mUpdateCursorPosition = true;
  mImpl->mEventData->mScrollAfterUpdatePosition = true;
}

void Controller::TextDeletedEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected TextDeletedEvent" );

  // TODO - Optimize this
  ClearModelData();

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->UpdateModel( ALL_OPERATIONS );
  mImpl->mOperationsPending = static_cast<OperationsMask>( LAYOUT             |
                                                           ALIGN              |
                                                           UPDATE_ACTUAL_SIZE |
                                                           REORDER );

  // Queue a cursor reposition event; this must wait until after DoRelayout()
  mImpl->mEventData->mScrollAfterDelete = true;
}

bool Controller::DoRelayout( const Size& size,
                             OperationsMask operationsRequired,
                             Size& layoutSize )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::DoRelayout %p size %f,%f\n", this, size.width, size.height );
  bool viewUpdated( false );

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mImpl->mOperationsPending & operationsRequired );

  if( LAYOUT & operations )
  {
    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    Length numberOfGlyphs = mImpl->mVisualModel->mGlyphs.Count();

    if( 0u == numberOfGlyphs )
    {
      // Nothing else to do if there is no glyphs.
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::DoRelayout no glyphs, view updated true\n" );
      return true;
    }

    Vector<LineBreakInfo>& lineBreakInfo = mImpl->mLogicalModel->mLineBreakInfo;
    Vector<WordBreakInfo>& wordBreakInfo = mImpl->mLogicalModel->mWordBreakInfo;
    Vector<CharacterDirection>& characterDirection = mImpl->mLogicalModel->mCharacterDirections;
    Vector<GlyphInfo>& glyphs = mImpl->mVisualModel->mGlyphs;
    Vector<CharacterIndex>& glyphsToCharactersMap = mImpl->mVisualModel->mGlyphsToCharacters;
    Vector<Length>& charactersPerGlyph = mImpl->mVisualModel->mCharactersPerGlyph;

    // Set the layout parameters.
    LayoutParameters layoutParameters( size,
                                       mImpl->mLogicalModel->mText.Begin(),
                                       lineBreakInfo.Begin(),
                                       wordBreakInfo.Begin(),
                                       ( 0u != characterDirection.Count() ) ? characterDirection.Begin() : NULL,
                                       numberOfGlyphs,
                                       glyphs.Begin(),
                                       glyphsToCharactersMap.Begin(),
                                       charactersPerGlyph.Begin() );

    // The laid-out lines.
    // It's not possible to know in how many lines the text is going to be laid-out,
    // but it can be resized at least with the number of 'paragraphs' to avoid
    // some re-allocations.
    Vector<LineRun>& lines = mImpl->mVisualModel->mLines;

    // Delete any previous laid out lines before setting the new ones.
    lines.Clear();

    // The capacity of the bidirectional paragraph info is the number of paragraphs.
    lines.Reserve( mImpl->mLogicalModel->mBidirectionalParagraphInfo.Capacity() );

    // Resize the vector of positions to have the same size than the vector of glyphs.
    Vector<Vector2>& glyphPositions = mImpl->mVisualModel->mGlyphPositions;
    glyphPositions.Resize( numberOfGlyphs );

    // Update the visual model.
    viewUpdated = mImpl->mLayoutEngine.LayoutText( layoutParameters,
                                                   glyphPositions,
                                                   lines,
                                                   layoutSize );

    if( viewUpdated )
    {
      // Reorder the lines
      if( REORDER & operations )
      {
        Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mImpl->mLogicalModel->mBidirectionalParagraphInfo;

        // Check first if there are paragraphs with bidirectional info.
        if( 0u != bidirectionalInfo.Count() )
        {
          // Get the lines
          const Length numberOfLines = mImpl->mVisualModel->mLines.Count();

          // Reorder the lines.
          Vector<BidirectionalLineInfoRun> lineBidirectionalInfoRuns;
          lineBidirectionalInfoRuns.Reserve( numberOfLines ); // Reserve because is not known yet how many lines have right to left characters.
          ReorderLines( bidirectionalInfo,
                        lines,
                        lineBidirectionalInfoRuns );

          // Set the bidirectional info into the model.
          const Length numberOfBidirectionalInfoRuns = lineBidirectionalInfoRuns.Count();
          mImpl->mLogicalModel->SetVisualToLogicalMap( lineBidirectionalInfoRuns.Begin(),
                                                       numberOfBidirectionalInfoRuns );

          // Set the bidirectional info per line into the layout parameters.
          layoutParameters.lineBidirectionalInfoRunsBuffer = lineBidirectionalInfoRuns.Begin();
          layoutParameters.numberOfBidirectionalInfoRuns = numberOfBidirectionalInfoRuns;

          // Get the character to glyph conversion table and set into the layout.
          layoutParameters.charactersToGlyphsBuffer = mImpl->mVisualModel->mCharactersToGlyph.Begin();

          // Get the glyphs per character table and set into the layout.
          layoutParameters.glyphsPerCharacterBuffer = mImpl->mVisualModel->mGlyphsPerCharacter.Begin();

          // Re-layout the text. Reorder those lines with right to left characters.
          mImpl->mLayoutEngine.ReLayoutRightToLeftLines( layoutParameters,
                                                         glyphPositions );

          // Free the allocated memory used to store the conversion table in the bidirectional line info run.
          for( Vector<BidirectionalLineInfoRun>::Iterator it = lineBidirectionalInfoRuns.Begin(),
                 endIt = lineBidirectionalInfoRuns.End();
               it != endIt;
               ++it )
          {
            BidirectionalLineInfoRun& bidiLineInfo = *it;

            free( bidiLineInfo.visualToLogicalMap );
          }
        }
      } // REORDER

      // Sets the actual size.
      if( UPDATE_ACTUAL_SIZE & operations )
      {
        mImpl->mVisualModel->SetActualSize( layoutSize );
      }
    } // view updated
  }
  else
  {
    layoutSize = mImpl->mVisualModel->GetActualSize();
  }

  if( ALIGN & operations )
  {
    // The laid-out lines.
    Vector<LineRun>& lines = mImpl->mVisualModel->mLines;

    mImpl->mLayoutEngine.Align( layoutSize,
                                lines );

    viewUpdated = true;
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::DoRelayout, view updated %s\n", ( viewUpdated ? "true" : "false" ) );
  return viewUpdated;
}

void Controller::SetMultiLineEnabled( bool enable )
{
  const LayoutEngine::Layout layout = enable ? LayoutEngine::MULTI_LINE_BOX : LayoutEngine::SINGLE_LINE_BOX;

  if( layout != mImpl->mLayoutEngine.GetLayout() )
  {
    // Set the layout type.
    mImpl->mLayoutEngine.SetLayout( layout );

    // Set the flags to redo the layout operations
    const OperationsMask layoutOperations =  static_cast<OperationsMask>( LAYOUT             |
                                                                          UPDATE_ACTUAL_SIZE |
                                                                          ALIGN              |
                                                                          REORDER );

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | layoutOperations );

    mImpl->RequestRelayout();
  }
}

bool Controller::IsMultiLineEnabled() const
{
  return LayoutEngine::MULTI_LINE_BOX == mImpl->mLayoutEngine.GetLayout();
}

void Controller::SetHorizontalAlignment( LayoutEngine::HorizontalAlignment alignment )
{
  if( alignment != mImpl->mLayoutEngine.GetHorizontalAlignment() )
  {
    // Set the alignment.
    mImpl->mLayoutEngine.SetHorizontalAlignment( alignment );

    // Set the flag to redo the alignment operation.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | ALIGN );

    mImpl->RequestRelayout();
  }
}

LayoutEngine::HorizontalAlignment Controller::GetHorizontalAlignment() const
{
  return mImpl->mLayoutEngine.GetHorizontalAlignment();
}

void Controller::SetVerticalAlignment( LayoutEngine::VerticalAlignment alignment )
{
  if( alignment != mImpl->mLayoutEngine.GetVerticalAlignment() )
  {
    // Set the alignment.
    mImpl->mLayoutEngine.SetVerticalAlignment( alignment );

    // Set the flag to redo the alignment operation.
    // TODO : Is not needed re-layout and reorder again but with the current implementation it is.
    //        Im working on a different patch to fix an issue with the alignment. When that patch
    //        is in, this issue can be fixed.
    const OperationsMask layoutOperations =  static_cast<OperationsMask>( LAYOUT             |
                                                                          UPDATE_ACTUAL_SIZE |
                                                                          ALIGN              |
                                                                          REORDER );

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | layoutOperations );

    mImpl->RequestRelayout();
  }
}

LayoutEngine::VerticalAlignment Controller::GetVerticalAlignment() const
{
  return mImpl->mLayoutEngine.GetVerticalAlignment();
}

void Controller::CalculateTextAlignment( const Size& size )
{
  // Get the direction of the first character.
  const CharacterDirection firstParagraphDirection = mImpl->mLogicalModel->GetCharacterDirection( 0u );

  const Size& actualSize = mImpl->mVisualModel->GetActualSize();

  // If the first paragraph is right to left swap ALIGN_BEGIN and ALIGN_END;
  LayoutEngine::HorizontalAlignment horizontalAlignment = mImpl->mLayoutEngine.GetHorizontalAlignment();
  if( firstParagraphDirection &&
      ( LayoutEngine::HORIZONTAL_ALIGN_CENTER != horizontalAlignment ) )
  {
    if( LayoutEngine::HORIZONTAL_ALIGN_BEGIN == horizontalAlignment )
    {
      horizontalAlignment = LayoutEngine::HORIZONTAL_ALIGN_END;
    }
    else
    {
      horizontalAlignment = LayoutEngine::HORIZONTAL_ALIGN_BEGIN;
    }
  }

  switch( horizontalAlignment )
  {
    case LayoutEngine::HORIZONTAL_ALIGN_BEGIN:
    {
      mImpl->mAlignmentOffset.x = 0.f;
      break;
    }
    case LayoutEngine::HORIZONTAL_ALIGN_CENTER:
    {
      const int intOffset = static_cast<int>( 0.5f * ( size.width - actualSize.width ) ); // try to avoid pixel alignment.
      mImpl->mAlignmentOffset.x = static_cast<float>( intOffset );
      break;
    }
    case LayoutEngine::HORIZONTAL_ALIGN_END:
    {
      mImpl->mAlignmentOffset.x = size.width - actualSize.width;
      break;
    }
  }

  const LayoutEngine::VerticalAlignment verticalAlignment = mImpl->mLayoutEngine.GetVerticalAlignment();
  switch( verticalAlignment )
  {
    case LayoutEngine::VERTICAL_ALIGN_TOP:
    {
      mImpl->mAlignmentOffset.y = 0.f;
      break;
    }
    case LayoutEngine::VERTICAL_ALIGN_CENTER:
    {
      const int intOffset = static_cast<int>( 0.5f * ( size.height - actualSize.height ) ); // try to avoid pixel alignment.
      mImpl->mAlignmentOffset.y = static_cast<float>( intOffset );
      break;
    }
    case LayoutEngine::VERTICAL_ALIGN_BOTTOM:
    {
      mImpl->mAlignmentOffset.y = size.height - actualSize.height;
      break;
    }
  }
}

LayoutEngine& Controller::GetLayoutEngine()
{
  return mImpl->mLayoutEngine;
}

View& Controller::GetView()
{
  return mImpl->mView;
}

void Controller::KeyboardFocusGainEvent()
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected KeyboardFocusGainEvent" );

  if( mImpl->mEventData )
  {
    mImpl->ChangeState( EventData::EDITING );

    if( mImpl->IsShowingPlaceholderText() )
    {
      // Show alternative placeholder-text when editing
      ShowPlaceholderText();
    }

    mImpl->RequestRelayout();
  }
}

void Controller::KeyboardFocusLostEvent()
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected KeyboardFocusLostEvent" );

  if( mImpl->mEventData )
  {
    mImpl->ChangeState( EventData::INACTIVE );

    if( mImpl->IsShowingPlaceholderText() )
    {
      // Revert to regular placeholder-text when not editing
      ShowPlaceholderText();
    }

    mImpl->RequestRelayout();
  }
}

bool Controller::KeyEvent( const Dali::KeyEvent& keyEvent )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected KeyEvent" );

  bool textChanged( false );

  if( mImpl->mEventData &&
      keyEvent.state == KeyEvent::Down )
  {
    int keyCode = keyEvent.keyCode;
    const std::string& keyString = keyEvent.keyPressed;

    // Pre-process to separate modifying events from non-modifying input events.
    if( Dali::DALI_KEY_ESCAPE == keyCode )
    {
      // Escape key is a special case which causes focus loss
      KeyboardFocusLostEvent();
    }
    else if( Dali::DALI_KEY_CURSOR_LEFT  == keyCode ||
             Dali::DALI_KEY_CURSOR_RIGHT == keyCode ||
             Dali::DALI_KEY_CURSOR_UP    == keyCode ||
             Dali::DALI_KEY_CURSOR_DOWN  == keyCode )
    {
      Event event( Event::CURSOR_KEY_EVENT );
      event.p1.mInt = keyCode;
      mImpl->mEventData->mEventQueue.push_back( event );
    }
    else if( Dali::DALI_KEY_BACKSPACE == keyCode )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::KeyEvent %p DALI_KEY_BACKSPACE\n", this );

      // IMF manager is no longer handling key-events
      mImpl->ClearPreEditFlag();

      bool removed( false );

      if ( EventData::SELECTING         == mImpl->mEventData->mState ||
           EventData::SELECTION_CHANGED == mImpl->mEventData->mState )
      {
        removed = RemoveSelectedText();
      }
      else
      {
        // Remove the character before the current cursor position
        removed = RemoveText( -1, 1 );
      }

      if( removed )
      {
        if( 0u != mImpl->mLogicalModel->mText.Count() ||
            !mImpl->IsPlaceholderAvailable() )
        {
          mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
        }
        else
        {
          ShowPlaceholderText();
          mImpl->mEventData->mUpdateCursorPosition = true;
        }

        textChanged = true;
      }
    }
    else
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::KeyEvent %p keyString %s\n", this, keyString.c_str() );

      // IMF manager is no longer handling key-events
      mImpl->ClearPreEditFlag();

      InsertText( keyString, COMMIT );
      textChanged = true;
    }

    mImpl->ChangeState( EventData::EDITING ); // todo Confirm this is the best place to change the state of

    mImpl->RequestRelayout();
  }

  if( textChanged )
  {
    // Do this last since it provides callbacks into application code
    mImpl->mControlInterface.TextChanged();
  }

  return false;
}

void Controller::InsertText( const std::string& text, Controller::InsertType type )
{
  bool removedPrevious( false );
  bool maxLengthReached( false );

  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected InsertText" )
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::InsertText %p %s (%s) mPrimaryCursorPosition %d mPreEditFlag %d mPreEditStartPosition %d mPreEditLength %d\n",
                 this, text.c_str(), (COMMIT == type ? "COMMIT" : "PRE_EDIT"),
                 mImpl->mEventData->mPrimaryCursorPosition, mImpl->mEventData->mPreEditFlag, mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength );

  Vector<Character> utf32Characters;
  Length characterCount( 0u );

  // Remove the previous IMF pre-edit (predicitive text)
  if( mImpl->mEventData &&
      mImpl->mEventData->mPreEditFlag &&
      0 != mImpl->mEventData->mPreEditLength )
  {
    CharacterIndex offset = mImpl->mEventData->mPrimaryCursorPosition - mImpl->mEventData->mPreEditStartPosition;
    removedPrevious = RemoveText( -static_cast<int>(offset), mImpl->mEventData->mPreEditLength );

    mImpl->mEventData->mPrimaryCursorPosition = mImpl->mEventData->mPreEditStartPosition;
    mImpl->mEventData->mPreEditLength = 0;
  }
  else
  {
    // Remove the previous Selection
    removedPrevious = RemoveSelectedText();
  }

  if( ! text.empty() )
  {
    //  Convert text into UTF-32
    utf32Characters.Resize( text.size() );

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    characterCount = Utf8ToUtf32( utf8, text.size(), utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    DALI_ASSERT_DEBUG( text.size() >= utf32Characters.Count() && "Invalid UTF32 conversion length" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "UTF8 size %d, UTF32 size %d\n", text.size(), utf32Characters.Count() );
  }

  if( 0u != utf32Characters.Count() ) // Check if Utf8ToUtf32 conversion succeeded
  {
    // The placeholder text is no longer needed
    if( mImpl->IsShowingPlaceholderText() )
    {
      ResetText();
    }

    // Handle the IMF (predicitive text) state changes
    if( mImpl->mEventData )
    {
      if( COMMIT == type )
      {
        // IMF manager is no longer handling key-events
        mImpl->ClearPreEditFlag();
      }
      else // PRE_EDIT
      {
        if( ! mImpl->mEventData->mPreEditFlag )
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Entered PreEdit state" );

          // Record the start of the pre-edit text
          mImpl->mEventData->mPreEditStartPosition = mImpl->mEventData->mPrimaryCursorPosition;
        }

        mImpl->mEventData->mPreEditLength = utf32Characters.Count();
        mImpl->mEventData->mPreEditFlag = true;

        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "mPreEditStartPosition %d mPreEditLength %d\n", mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength );
      }
    }

    const Length numberOfCharactersInModel = mImpl->mLogicalModel->mText.Count();

    // Restrict new text to fit within Maximum characters setting
    Length maxSizeOfNewText = std::min ( ( mImpl->mMaximumNumberOfCharacters - numberOfCharactersInModel ), characterCount );
    maxLengthReached = ( characterCount > maxSizeOfNewText );

    // Insert at current cursor position
    CharacterIndex& cursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

    Vector<Character>& modifyText = mImpl->mLogicalModel->mText;

    if( cursorIndex < numberOfCharactersInModel )
    {
      modifyText.Insert( modifyText.Begin() + cursorIndex, utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText );
    }
    else
    {
      modifyText.Insert( modifyText.End(), utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText );
    }

    cursorIndex += maxSizeOfNewText;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Inserted %d characters, new size %d new cursor %d\n", maxSizeOfNewText, mImpl->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition );
  }

  if( 0u == mImpl->mLogicalModel->mText.Count() &&
      mImpl->IsPlaceholderAvailable() )
  {
    // Show place-holder if empty after removing the pre-edit text
    ShowPlaceholderText();
    mImpl->mEventData->mUpdateCursorPosition = true;
    mImpl->ClearPreEditFlag();
  }
  else if( removedPrevious ||
           0 != utf32Characters.Count() )
  {
    // Queue an inserted event
    mImpl->QueueModifyEvent( ModifyEvent::TEXT_INSERTED );
  }

  if( maxLengthReached )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "MaxLengthReached (%d)\n", mImpl->mLogicalModel->mText.Count() );

    mImpl->ResetImfManager();

    // Do this last since it provides callbacks into application code
    mImpl->mControlInterface.MaxLengthReached();
  }
}

bool Controller::RemoveSelectedText()
{
  bool textRemoved( false );

  if ( EventData::SELECTING         == mImpl->mEventData->mState ||
       EventData::SELECTION_CHANGED == mImpl->mEventData->mState )
  {
    std::string removedString;
    mImpl->RetrieveSelection( removedString, true );

    if( !removedString.empty() )
    {
      textRemoved = true;
      mImpl->ChangeState( EventData::EDITING );
    }
  }

  return textRemoved;
}

void Controller::TapEvent( unsigned int tapCount, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected TapEvent" );

  if( NULL != mImpl->mEventData )
  {
    const bool isShowingPlaceholderText = mImpl->IsShowingPlaceholderText();
    if( 1u == tapCount )
    {
      bool tapDuringEditMode( EventData::EDITING == mImpl->mEventData->mState );

      if( !isShowingPlaceholderText && tapDuringEditMode )
      {
        mImpl->mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, true );
        mImpl->mEventData->mDecorator->SetPopupActive( false );
      }

      mImpl->ChangeState( EventData::EDITING );

      // Handles & cursors must be repositioned after Relayout() i.e. after the Model has been updated
      if( mImpl->mEventData )
      {
        Event event( Event::TAP_EVENT );
        event.p1.mUint = tapCount;
        event.p2.mFloat = x;
        event.p3.mFloat = y;
        mImpl->mEventData->mEventQueue.push_back( event );

        mImpl->RequestRelayout();
      }
    }
    else if( !isShowingPlaceholderText &&
             mImpl->mEventData->mSelectionEnabled &&
             ( 2u == tapCount ) )
    {
      SelectEvent( x, y, false );
    }
  }

  // Reset keyboard as tap event has occurred.
  mImpl->ResetImfManager();
}

void Controller::PanEvent( Gesture::State state, const Vector2& displacement )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected PanEvent" );

  if( mImpl->mEventData )
  {
    Event event( Event::PAN_EVENT );
    event.p1.mInt = state;
    event.p2.mFloat = displacement.x;
    event.p3.mFloat = displacement.y;
    mImpl->mEventData->mEventQueue.push_back( event );

    mImpl->RequestRelayout();
  }
}

void Controller::SelectEvent( float x, float y, bool selectAll )
{
  if( mImpl->mEventData )
  {
    if ( mImpl->mEventData->mState == EventData::SELECTING )
    {
      mImpl->ChangeState( EventData::SELECTION_CHANGED );
    }
    else
    {
      mImpl->ChangeState( EventData::SELECTING );
    }

    if( selectAll )
    {
      Event event( Event::SELECT_ALL );
      mImpl->mEventData->mEventQueue.push_back( event );
    }
    else
    {
      Event event( Event::SELECT );
      event.p2.mFloat = x;
      event.p3.mFloat = y;
      mImpl->mEventData->mEventQueue.push_back( event );
    }

    mImpl->RequestRelayout();
  }
}

void Controller::GetTargetSize( Vector2& targetSize )
{
  targetSize = mImpl->mControlSize;
}

void Controller::AddDecoration( Actor& actor, bool needsClipping )
{
  mImpl->mControlInterface.AddDecoration( actor, needsClipping );
}

void Controller::DecorationEvent( HandleType handleType, HandleState state, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected DecorationEvent" );

  if( mImpl->mEventData )
  {
    switch( handleType )
    {
      case GRAB_HANDLE:
      {
        Event event( Event::GRAB_HANDLE_EVENT );
        event.p1.mUint  = state;
        event.p2.mFloat = x;
        event.p3.mFloat = y;

        mImpl->mEventData->mEventQueue.push_back( event );
        break;
      }
      case LEFT_SELECTION_HANDLE:
      {
        Event event( Event::LEFT_SELECTION_HANDLE_EVENT );
        event.p1.mUint  = state;
        event.p2.mFloat = x;
        event.p3.mFloat = y;

        mImpl->mEventData->mEventQueue.push_back( event );
        break;
      }
      case RIGHT_SELECTION_HANDLE:
      {
        Event event( Event::RIGHT_SELECTION_HANDLE_EVENT );
        event.p1.mUint  = state;
        event.p2.mFloat = x;
        event.p3.mFloat = y;

        mImpl->mEventData->mEventQueue.push_back( event );
        break;
      }
      case HANDLE_TYPE_COUNT:
      {
        DALI_ASSERT_DEBUG( !"Controller::HandleEvent. Unexpected handle type" );
      }
    }

    mImpl->RequestRelayout();
  }
}

void Controller::TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::Buttons button )
{
  if( NULL == mImpl->mEventData )
  {
    return;
  }

  switch( button )
  {
    case Toolkit::TextSelectionPopup::CUT:
    {
      mImpl->SendSelectionToClipboard( true ); // Synchronous call to modify text
      mImpl->mOperationsPending = ALL_OPERATIONS;
      if( 0u != mImpl->mLogicalModel->mText.Count() ||
          !mImpl->IsPlaceholderAvailable() )
      {
        mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
      }
      else
      {
        ShowPlaceholderText();
        mImpl->mEventData->mUpdateCursorPosition = true;
      }
      mImpl->RequestRelayout();
      mImpl->mControlInterface.TextChanged();
      break;
    }
    case Toolkit::TextSelectionPopup::COPY:
    {
      mImpl->SendSelectionToClipboard( false ); // Text not modified
      mImpl->RequestRelayout(); // Handles, Selection Highlight, Popup
      break;
    }
    case Toolkit::TextSelectionPopup::PASTE:
    {
      std::string stringToPaste("");
      mImpl->GetTextFromClipboard( 0, stringToPaste ); // Paste latest item from system clipboard
      InsertText( stringToPaste, Text::Controller::CLIPBOARD );
      mImpl->ChangeState( EventData::EDITING );
      mImpl->RequestRelayout();
      break;
    }
    case Toolkit::TextSelectionPopup::SELECT:
    {
      const Vector2& currentCursorPosition = mImpl->mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

      if( mImpl->mEventData->mSelectionEnabled  )
      {
        // Creates a SELECT event.
        SelectEvent( currentCursorPosition.x, currentCursorPosition.y, false );
      }
      break;
    }
    case Toolkit::TextSelectionPopup::SELECT_ALL:
    {
      // Creates a SELECT_ALL event
      SelectEvent( 0.f, 0.f, true );
      break;
    }
    case Toolkit::TextSelectionPopup::CLIPBOARD:
    {
      break;
    }
    case Toolkit::TextSelectionPopup::NONE:
    {
      // Nothing to do.
      break;
    }
  }
}

ImfManager::ImfCallbackData Controller::OnImfEvent( ImfManager& imfManager, const ImfManager::ImfEventData& imfEvent )
{
  bool update( false );
  bool requestRelayout = false;

  std::string text;
  unsigned int cursorPosition( 0 );

  switch ( imfEvent.eventName )
  {
    case ImfManager::COMMIT:
    {
      InsertText( imfEvent.predictiveString, Text::Controller::COMMIT );
      requestRelayout = true;
      break;
    }
    case ImfManager::PREEDIT:
    {
      InsertText( imfEvent.predictiveString, Text::Controller::PRE_EDIT );
      update = true;
      requestRelayout = true;
      break;
    }
    case ImfManager::DELETESURROUNDING:
    {
      RemoveText( imfEvent.cursorOffset, imfEvent.numberOfChars );
      requestRelayout = true;
      break;
    }
    case ImfManager::GETSURROUNDING:
    {
      GetText( text );
      cursorPosition = GetLogicalCursorPosition();

      imfManager.SetSurroundingText( text );
      imfManager.SetCursorPosition( cursorPosition );
      break;
    }
    case ImfManager::VOID:
    {
      // do nothing
      break;
    }
  } // end switch

  if( ImfManager::GETSURROUNDING != imfEvent.eventName )
  {
    GetText( text );
    cursorPosition = GetLogicalCursorPosition();
  }

  if( requestRelayout )
  {
    mImpl->mOperationsPending = ALL_OPERATIONS;
    mImpl->RequestRelayout();

    // Do this last since it provides callbacks into application code
    mImpl->mControlInterface.TextChanged();
  }

  ImfManager::ImfCallbackData callbackData( update, cursorPosition, text, false );

  return callbackData;
}


Controller::~Controller()
{
  delete mImpl;
}

void Controller::ShowPlaceholderText()
{
  if( mImpl->IsPlaceholderAvailable() )
  {
    DALI_ASSERT_DEBUG( mImpl->mEventData && "No placeholder text available" );

    mImpl->mEventData->mIsShowingPlaceholderText = true;

    // Disable handles when showing place-holder text
    mImpl->mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
    mImpl->mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
    mImpl->mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );

    const char* text( NULL );
    size_t size( 0 );

    // TODO - Switch placeholder text styles when changing state
    if( EventData::INACTIVE != mImpl->mEventData->mState &&
        0u != mImpl->mEventData->mPlaceholderTextActive.c_str() )
    {
      text = mImpl->mEventData->mPlaceholderTextActive.c_str();
      size = mImpl->mEventData->mPlaceholderTextActive.size();
    }
    else
    {
      text = mImpl->mEventData->mPlaceholderTextInactive.c_str();
      size = mImpl->mEventData->mPlaceholderTextInactive.size();
    }

    // Reset model for showing placeholder.
    mImpl->mLogicalModel->mText.Clear();
    ClearModelData();
    mImpl->mVisualModel->SetTextColor( mImpl->mEventData->mPlaceholderTextColor );

    // Convert text into UTF-32
    Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;
    utf32Characters.Resize( size );

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32( utf8, size, utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    // Reset the cursor position
    mImpl->mEventData->mPrimaryCursorPosition = 0;

    // The natural size needs to be re-calculated.
    mImpl->mRecalculateNaturalSize = true;

    // Apply modifications to the model
    mImpl->mOperationsPending = ALL_OPERATIONS;

    // Update the rest of the model during size negotiation
    mImpl->QueueModifyEvent( ModifyEvent::TEXT_REPLACED );
  }
}

void Controller::ClearModelData()
{
  // n.b. This does not Clear the mText from mLogicalModel
  mImpl->mLogicalModel->mScriptRuns.Clear();
  mImpl->mLogicalModel->mFontRuns.Clear();
  mImpl->mLogicalModel->mLineBreakInfo.Clear();
  mImpl->mLogicalModel->mWordBreakInfo.Clear();
  mImpl->mLogicalModel->mBidirectionalParagraphInfo.Clear();
  mImpl->mLogicalModel->mCharacterDirections.Clear();
  mImpl->mLogicalModel->mBidirectionalLineInfo.Clear();
  mImpl->mLogicalModel->mLogicalToVisualMap.Clear();
  mImpl->mLogicalModel->mVisualToLogicalMap.Clear();
  mImpl->mVisualModel->mGlyphs.Clear();
  mImpl->mVisualModel->mGlyphsToCharacters.Clear();
  mImpl->mVisualModel->mCharactersToGlyph.Clear();
  mImpl->mVisualModel->mCharactersPerGlyph.Clear();
  mImpl->mVisualModel->mGlyphsPerCharacter.Clear();
  mImpl->mVisualModel->mGlyphPositions.Clear();
  mImpl->mVisualModel->mLines.Clear();
  mImpl->mVisualModel->ClearCaches();
}

void Controller::ClearFontData()
{
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mLogicalModel->mFontRuns.Clear();
  mImpl->mVisualModel->mGlyphs.Clear();
  mImpl->mVisualModel->mGlyphsToCharacters.Clear();
  mImpl->mVisualModel->mCharactersToGlyph.Clear();
  mImpl->mVisualModel->mCharactersPerGlyph.Clear();
  mImpl->mVisualModel->mGlyphsPerCharacter.Clear();
  mImpl->mVisualModel->mGlyphPositions.Clear();
  mImpl->mVisualModel->mLines.Clear();
  mImpl->mVisualModel->ClearCaches();
}

Controller::Controller( ControlInterface& controlInterface )
: mImpl( NULL )
{
  mImpl = new Controller::Impl( controlInterface );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
