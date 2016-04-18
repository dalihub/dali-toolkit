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
#include <memory.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/clipboard-event-notifier.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/markup-processor.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const float MAX_FLOAT = std::numeric_limits<float>::max();
const unsigned int POINTS_PER_INCH = 72;

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

/**
 * @brief Adds a new font description run for the selected text.
 *
 * The new font parameters are added after the call to this method.
 *
 * @param[in] eventData The event data pointer.
 * @param[in] logicalModel The logical model where to add the new font description run.
 */
FontDescriptionRun& UpdateSelectionFontStyleRun( EventData* eventData,
                                                 LogicalModelPtr logicalModel )
{
  const bool handlesCrossed = eventData->mLeftSelectionPosition > eventData->mRightSelectionPosition;

  // Get start and end position of selection
  const CharacterIndex startOfSelectedText = handlesCrossed ? eventData->mRightSelectionPosition : eventData->mLeftSelectionPosition;
  const Length lengthOfSelectedText = ( handlesCrossed ? eventData->mLeftSelectionPosition : eventData->mRightSelectionPosition ) - startOfSelectedText;

  // Add the font run.
  const VectorBase::SizeType numberOfRuns = logicalModel->mFontDescriptionRuns.Count();
  logicalModel->mFontDescriptionRuns.Resize( numberOfRuns + 1u );

  FontDescriptionRun& fontDescriptionRun = *( logicalModel->mFontDescriptionRuns.Begin() + numberOfRuns );

  fontDescriptionRun.characterRun.characterIndex = startOfSelectedText;
  fontDescriptionRun.characterRun.numberOfCharacters = lengthOfSelectedText;

  // Recalculate the selection highlight as the metrics may have changed.
  eventData->mUpdateLeftSelectionPosition = true;
  eventData->mUpdateRightSelectionPosition = true;

  return fontDescriptionRun;
}

ControllerPtr Controller::New( ControlInterface& controlInterface )
{
  return ControllerPtr( new Controller( controlInterface ) );
}

void Controller::EnableTextInput( DecoratorPtr decorator )
{
  if( NULL == mImpl->mEventData )
  {
    mImpl->mEventData = new EventData( decorator );
  }
}

void Controller::SetGlyphType( TextAbstraction::GlyphType glyphType )
{
  // Metrics for bitmap & vector based glyphs are different
  mImpl->mMetrics->SetGlyphType( glyphType );

  // Clear the font-specific data
  ClearFontData();

  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

void Controller::SetMarkupProcessorEnabled( bool enable )
{
  mImpl->mMarkupProcessorEnabled = enable;
}

bool Controller::IsMarkupProcessorEnabled() const
{
  return mImpl->mMarkupProcessorEnabled;
}

void Controller::SetText( const std::string& text )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::SetText\n" );

  // Reset keyboard as text changed
  mImpl->ResetImfManager();

  // Remove the previously set text and style.
  ResetText();

  // Remove the style.
  ClearStyleData();

  CharacterIndex lastCursorIndex = 0u;

  if( NULL != mImpl->mEventData )
  {
    // If popup shown then hide it by switching to Editing state
    if( ( EventData::SELECTING == mImpl->mEventData->mState )          ||
        ( EventData::EDITING_WITH_POPUP == mImpl->mEventData->mState ) ||
        ( EventData::EDITING_WITH_GRAB_HANDLE == mImpl->mEventData->mState ) ||
        ( EventData::EDITING_WITH_PASTE_POPUP == mImpl->mEventData->mState ) )
    {
      mImpl->ChangeState( EventData::EDITING );
    }
  }

  if( !text.empty() )
  {
    mImpl->mVisualModel->SetTextColor( mImpl->mTextColor );

    MarkupProcessData markupProcessData( mImpl->mLogicalModel->mColorRuns,
                                         mImpl->mLogicalModel->mFontDescriptionRuns );

    Length textSize = 0u;
    const uint8_t* utf8 = NULL;
    if( mImpl->mMarkupProcessorEnabled )
    {
      ProcessMarkupString( text, markupProcessData );
      textSize = markupProcessData.markupProcessedText.size();

      // This is a bit horrible but std::string returns a (signed) char*
      utf8 = reinterpret_cast<const uint8_t*>( markupProcessData.markupProcessedText.c_str() );
    }
    else
    {
      textSize = text.size();

      // This is a bit horrible but std::string returns a (signed) char*
      utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );
    }

    //  Convert text into UTF-32
    Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;
    utf32Characters.Resize( textSize );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32( utf8, textSize, utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    DALI_ASSERT_DEBUG( textSize >= characterCount && "Invalid UTF32 conversion length" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::SetText %p UTF8 size %d, UTF32 size %d\n", this, textSize, mImpl->mLogicalModel->mText.Count() );

    // The characters to be added.
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mLogicalModel->mText.Count();

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

  if( NULL != mImpl->mEventData )
  {
    // Cancel previously queued events
    mImpl->mEventData->mEventQueue.clear();
  }

  // Notify IMF as text changed
  NotifyImfManager();

  // Do this last since it provides callbacks into application code
  mImpl->mControlInterface.TextChanged();
}

void Controller::GetText( std::string& text ) const
{
  if( !mImpl->IsShowingPlaceholderText() )
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
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mPrimaryCursorPosition;
  }

  return 0u;
}

void Controller::SetPlaceholderText( PlaceholderType type, const std::string& text )
{
  if( NULL != mImpl->mEventData )
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
        ( 0u == mImpl->mLogicalModel->mText.Count() ) )
    {
      ShowPlaceholderText();
    }
  }
}

void Controller::GetPlaceholderText( PlaceholderType type, std::string& text ) const
{
  if( NULL != mImpl->mEventData )
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

void Controller::SetMaximumNumberOfCharacters( Length maxCharacters )
{
  mImpl->mMaximumNumberOfCharacters = maxCharacters;
}

int Controller::GetMaximumNumberOfCharacters()
{
  return mImpl->mMaximumNumberOfCharacters;
}

void Controller::SetDefaultFontFamily( const std::string& defaultFontFamily )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.family = defaultFontFamily;
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::SetDefaultFontFamily %s\n", defaultFontFamily.c_str());
  mImpl->mFontDefaults->familyDefined = true;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

const std::string& Controller::GetDefaultFontFamily() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.family;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultFontStyle( const std::string& style )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontStyle = style;
}

const std::string& Controller::GetDefaultFontStyle() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontStyle;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultFontWeight( FontWeight weight )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.weight = weight;
  mImpl->mFontDefaults->weightDefined = true;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

FontWeight Controller::GetDefaultFontWeight() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.weight;
  }

  return TextAbstraction::FontWeight::NORMAL;
}

void Controller::SetDefaultFontWidth( FontWidth width )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.width = width;
  mImpl->mFontDefaults->widthDefined = true;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

FontWidth Controller::GetDefaultFontWidth() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.width;
  }

  return TextAbstraction::FontWidth::NORMAL;
}

void Controller::SetDefaultFontSlant( FontSlant slant )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.slant = slant;
  mImpl->mFontDefaults->slantDefined = true;

  // Clear the font-specific data
  ClearFontData();

  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

FontSlant Controller::GetDefaultFontSlant() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.slant;
  }

  return TextAbstraction::FontSlant::NORMAL;
}

void Controller::SetDefaultPointSize( float pointSize )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultPointSize = pointSize;
  mImpl->mFontDefaults->sizeDefined = true;

  unsigned int horizontalDpi( 0u );
  unsigned int verticalDpi( 0u );
  mImpl->mFontClient.GetDpi( horizontalDpi, verticalDpi );

  // Adjust the metrics if the fixed-size font should be down-scaled
  int maxEmojiSize( pointSize/POINTS_PER_INCH * verticalDpi );
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::SetDefaultPointSize %p setting MaxEmojiSize %d\n", this, maxEmojiSize );
  mImpl->mMetrics->SetMaxEmojiSize( maxEmojiSize );

  // Clear the font-specific data
  ClearFontData();

  mImpl->mRecalculateNaturalSize = true;

  mImpl->RequestRelayout();
}

float Controller::GetDefaultPointSize() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mDefaultPointSize;
  }

  return 0.0f;
}

void Controller::UpdateAfterFontChange( const std::string& newDefaultFont )
{
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Controller::UpdateAfterFontChange");

  if( !mImpl->mFontDefaults->familyDefined ) // If user defined font then should not update when system font changes
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "Controller::UpdateAfterFontChange newDefaultFont(%s)\n", newDefaultFont.c_str() );
    ClearFontData();
    mImpl->mFontDefaults->mFontDescription.family = newDefaultFont;
    mImpl->mRecalculateNaturalSize = true;
    mImpl->RequestRelayout();
  }
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

bool Controller::RemoveText( int cursorOffset,
                             int numberOfCharacters,
                             UpdateInputStyleType type )
{
  bool removed = false;

  if( NULL == mImpl->mEventData )
  {
    return removed;
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::RemoveText %p mText.Count() %d cursor %d cursorOffset %d numberOfCharacters %d\n",
                 this, mImpl->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition, cursorOffset, numberOfCharacters );

  if( !mImpl->IsShowingPlaceholderText() )
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

    if( ( cursorIndex + numberOfCharacters ) > currentText.Count() )
    {
      numberOfCharacters = currentText.Count() - cursorIndex;
    }

    if( ( cursorIndex + numberOfCharacters ) <= mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters )
    {
      // Mark the paragraphs to be updated.
      mImpl->mTextUpdateInfo.mCharacterIndex = std::min( cursorIndex, mImpl->mTextUpdateInfo.mCharacterIndex );
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove += numberOfCharacters;

      // Update the input style and remove the text's style before removing the text.

      if( UPDATE_INPUT_STYLE == type )
      {
        // Set first the default input style.
        mImpl->RetrieveDefaultInputStyle( mImpl->mEventData->mInputStyle );

        // Update the input style.
        mImpl->mLogicalModel->RetrieveStyle( cursorIndex, mImpl->mEventData->mInputStyle );
      }

      // Updates the text style runs by removing characters. Runs with no characters are removed.
      mImpl->mLogicalModel->UpdateTextStyleRuns( cursorIndex, -numberOfCharacters );

      // Remove the characters.
      Vector<Character>::Iterator first = currentText.Begin() + cursorIndex;
      Vector<Character>::Iterator last  = first + numberOfCharacters;

      currentText.Erase( first, last );

      // Cursor position retreat
      oldCursorIndex = cursorIndex;

      DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::RemoveText %p removed %d\n", this, numberOfCharacters );
      removed = true;
    }
  }

  return removed;
}

void Controller::SetPlaceholderTextColor( const Vector4& textColor )
{
  if( NULL != mImpl->mEventData )
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
  if( NULL != mImpl->mEventData )
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

void Controller::SetInputColor( const Vector4& color )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.textColor = color;
    mImpl->mEventData->mInputStyle.isDefaultColor = false;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      const bool handlesCrossed = mImpl->mEventData->mLeftSelectionPosition > mImpl->mEventData->mRightSelectionPosition;

      // Get start and end position of selection
      const CharacterIndex startOfSelectedText = handlesCrossed ? mImpl->mEventData->mRightSelectionPosition : mImpl->mEventData->mLeftSelectionPosition;
      const Length lengthOfSelectedText = ( handlesCrossed ? mImpl->mEventData->mLeftSelectionPosition : mImpl->mEventData->mRightSelectionPosition ) - startOfSelectedText;

      // Add the color run.
      const VectorBase::SizeType numberOfRuns = mImpl->mLogicalModel->mColorRuns.Count();
      mImpl->mLogicalModel->mColorRuns.Resize( numberOfRuns + 1u );

      ColorRun& colorRun = *( mImpl->mLogicalModel->mColorRuns.Begin() + numberOfRuns );
      colorRun.color = color;
      colorRun.characterRun.characterIndex = startOfSelectedText;
      colorRun.characterRun.numberOfCharacters = lengthOfSelectedText;

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | COLOR );
      mImpl->RequestRelayout();

      mImpl->mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = lengthOfSelectedText;
    }
  }
}

const Vector4& Controller::GetInputColor() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.textColor;
  }

  // Return the default text's color if there is no EventData.
  return mImpl->mTextColor;

}

void Controller::SetInputFontFamily( const std::string& fontFamily )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.familyName = fontFamily;
    mImpl->mEventData->mInputStyle.familyDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mLogicalModel );

      fontDescriptionRun.familyLength = fontFamily.size();
      fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
      memcpy( fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength );
      fontDescriptionRun.familyDefined = true;

      // The memory allocated for the font family name is freed when the font description is removed from the logical model.

      // Request to relayout.
      mImpl->mOperationsPending = ALL_OPERATIONS;
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      // As the font changes, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

const std::string& Controller::GetInputFontFamily() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.familyName;
  }

  // Return the default font's family if there is no EventData.
  return GetDefaultFontFamily();
}

void Controller::SetInputFontStyle( const std::string& fontStyle )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.fontStyle = fontStyle;
  }
}

const std::string& Controller::GetInputFontStyle() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.fontStyle;
  }

  // Return the default font's style if there is no EventData.
  return GetDefaultFontStyle();
}

void Controller::SetInputFontWeight( FontWeight weight )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.weight = weight;
    mImpl->mEventData->mInputStyle.weightDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mLogicalModel );

      fontDescriptionRun.weight = weight;
      fontDescriptionRun.weightDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = ALL_OPERATIONS;
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

FontWeight Controller::GetInputFontWeight() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.weight;
  }

  return GetDefaultFontWeight();
}

void Controller::SetInputFontWidth( FontWidth width )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.width = width;
    mImpl->mEventData->mInputStyle.widthDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mLogicalModel );

      fontDescriptionRun.width = width;
      fontDescriptionRun.widthDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = ALL_OPERATIONS;
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

FontWidth Controller::GetInputFontWidth() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.width;
  }

  return GetDefaultFontWidth();
}

void Controller::SetInputFontSlant( FontSlant slant )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.slant = slant;
    mImpl->mEventData->mInputStyle.slantDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mLogicalModel );

      fontDescriptionRun.slant = slant;
      fontDescriptionRun.slantDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = ALL_OPERATIONS;
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

FontSlant Controller::GetInputFontSlant() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.slant;
  }

  return GetDefaultFontSlant();
}

void Controller::SetInputFontPointSize( float size )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.size = size;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mLogicalModel );

      fontDescriptionRun.size = static_cast<PointSize26Dot6>( size * 64.f );
      fontDescriptionRun.sizeDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = ALL_OPERATIONS;
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

float Controller::GetInputFontPointSize() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.size;
  }

  // Return the default font's point size if there is no EventData.
  return GetDefaultPointSize();
}

void Controller::SetEnableCursorBlink( bool enable )
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "TextInput disabled" );

  if( NULL != mImpl->mEventData )
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
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mCursorBlinkEnabled;
  }

  return false;
}

const Vector2& Controller::GetScrollPosition() const
{
  if( NULL != mImpl->mEventData )
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

    // Layout the text for the new width.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | LAYOUT );

    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mLogicalModel->mText.Count();

    // Store the actual control's width.
    const float actualControlWidth = mImpl->mVisualModel->mControlSize.width;

    DoRelayout( Size( MAX_FLOAT, MAX_FLOAT ),
                static_cast<OperationsMask>( onlyOnceOperations |
                                             LAYOUT ),
                naturalSize.GetVectorXY() );

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending & ~onlyOnceOperations );

    // Do the size related operations again.
    const OperationsMask sizeOperations =  static_cast<OperationsMask>( LAYOUT |
                                                                        ALIGN  |
                                                                        REORDER );
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | sizeOperations );

    // Stores the natural size to avoid recalculate it again
    // unless the text/style changes.
    mImpl->mVisualModel->SetNaturalSize( naturalSize.GetVectorXY() );

    mImpl->mRecalculateNaturalSize = false;

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // Restore the actual control's width.
    mImpl->mVisualModel->mControlSize.width = actualControlWidth;

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
  if( fabsf( width - mImpl->mVisualModel->mControlSize.width ) > Math::MACHINE_EPSILON_1000 )
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


    // Layout the text for the new width.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | LAYOUT );

    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mLogicalModel->mText.Count();

    // Store the actual control's width.
    const float actualControlWidth = mImpl->mVisualModel->mControlSize.width;

    DoRelayout( Size( width, MAX_FLOAT ),
                static_cast<OperationsMask>( onlyOnceOperations |
                                             LAYOUT ),
                layoutSize );

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending & ~onlyOnceOperations );

    // Do the size related operations again.
    const OperationsMask sizeOperations =  static_cast<OperationsMask>( LAYOUT |
                                                                        ALIGN  |
                                                                        REORDER );

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | sizeOperations );

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // Restore the actual control's width.
    mImpl->mVisualModel->mControlSize.width = actualControlWidth;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height );
  }
  else
  {
    layoutSize = mImpl->mVisualModel->GetLayoutSize();
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

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // Not worth to relayout if width or height is equal to zero.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::Relayout (skipped)\n" );

    return glyphsRemoved;
  }

  // Whether a new size has been set.
  const bool newSize = ( size != mImpl->mVisualModel->mControlSize );

  if( newSize )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "new size (previous size %f,%f)\n", mImpl->mVisualModel->mControlSize.width, mImpl->mVisualModel->mControlSize.height );

    // Layout operations that need to be done if the size changes.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                             LAYOUT                    |
                                                             ALIGN                     |
                                                             UPDATE_ACTUAL_SIZE        |
                                                             REORDER );
    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
    mImpl->mTextUpdateInfo.mCharacterIndex = 0u;
  }

  // Whether there are modify events.
  if( 0u != mImpl->mModifyEvents.Count() )
  {
    // Style operations that need to be done if the text is modified.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                             COLOR );
  }

  // Make sure the model is up-to-date before layouting.
  ProcessModifyEvents();
  bool updated = mImpl->UpdateModel( mImpl->mOperationsPending );

  // Layout the text.
  Size layoutSize;
  updated = DoRelayout( size,
                        mImpl->mOperationsPending,
                        layoutSize ) || updated;

  // Do not re-do any operation until something changes.
  mImpl->mOperationsPending = NO_OPERATION;

  // Whether the text control is editable
  const bool isEditable = NULL != mImpl->mEventData;

  // Keep the current offset and alignment as it will be used to update the decorator's positions (if the size changes).
  Vector2 offset;
  if( newSize && isEditable )
  {
    offset = mImpl->mAlignmentOffset + mImpl->mEventData->mScrollPosition;
  }

  // After doing the text layout, the alignment offset to place the actor in the desired position can be calculated.
  CalculateTextAlignment( size );

  if( isEditable )
  {
    if( newSize )
    {
      // If there is a new size, the scroll position needs to be clamped.
      mImpl->ClampHorizontalScroll( layoutSize );

      // Update the decorator's positions is needed if there is a new size.
      mImpl->mEventData->mDecorator->UpdatePositions( mImpl->mAlignmentOffset + mImpl->mEventData->mScrollPosition - offset );
    }

    // Move the cursor, grab handle etc.
    updated = mImpl->ProcessInputEvents() || updated;
  }

  // Clear the update info. This info will be set the next time the text is updated.
  mImpl->mTextUpdateInfo.Clear();
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::Relayout\n" );

  return updated;
}

void Controller::ProcessModifyEvents()
{
  Vector<ModifyEvent>& events = mImpl->mModifyEvents;

  if( 0u == events.Count() )
  {
    // Nothing to do.
    return;
  }

  for( Vector<ModifyEvent>::ConstIterator it = events.Begin(),
         endIt = events.End();
       it != endIt;
       ++it )
  {
    const ModifyEvent& event = *it;

    if( ModifyEvent::TEXT_REPLACED == event.type )
    {
      // A (single) replace event should come first, otherwise we wasted time processing NOOP events
      DALI_ASSERT_DEBUG( it == events.Begin() && "Unexpected TEXT_REPLACED event" );

      TextReplacedEvent();
    }
    else if( ModifyEvent::TEXT_INSERTED == event.type )
    {
      TextInsertedEvent();
    }
    else if( ModifyEvent::TEXT_DELETED == event.type )
    {
      // Placeholder-text cannot be deleted
      if( !mImpl->IsShowingPlaceholderText() )
      {
        TextDeletedEvent();
      }
    }
  }

  if( NULL != mImpl->mEventData )
  {
    // When the text is being modified, delay cursor blinking
    mImpl->mEventData->mDecorator->DelayCursorBlink();
  }

  // Discard temporary text
  events.Clear();
}

void Controller::ResetText()
{
  // Reset buffers.
  mImpl->mLogicalModel->mText.Clear();

  // We have cleared everything including the placeholder-text
  mImpl->PlaceholderCleared();

  mImpl->mTextUpdateInfo.mCharacterIndex = 0u;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = 0u;

  // Clear any previous text.
  mImpl->mTextUpdateInfo.mClearAll = true;

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
    if( EventData::IsEditingState( mImpl->mEventData->mState )  )
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
  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::TextInsertedEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected TextInsertedEvent" );

  if( NULL == mImpl->mEventData )
  {
    return;
  }

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;

  // Queue a cursor reposition event; this must wait until after DoRelayout()
  if( EventData::IsEditingState( mImpl->mEventData->mState ) )
  {
    mImpl->mEventData->mUpdateCursorPosition = true;
    mImpl->mEventData->mScrollAfterUpdatePosition = true;
  }
}

void Controller::TextDeletedEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected TextDeletedEvent" );

  if( NULL == mImpl->mEventData )
  {
    return;
  }

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;

  // Queue a cursor reposition event; this must wait until after DoRelayout()
  mImpl->mEventData->mUpdateCursorPosition = true;
  if( 0u != mImpl->mLogicalModel->mText.Count() )
  {
    mImpl->mEventData->mScrollAfterDelete = true;
  }
}

bool Controller::DoRelayout( const Size& size,
                             OperationsMask operationsRequired,
                             Size& layoutSize )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::DoRelayout %p size %f,%f\n", this, size.width, size.height );
  bool viewUpdated( false );

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mImpl->mOperationsPending & operationsRequired );

  const CharacterIndex startIndex = mImpl->mTextUpdateInfo.mParagraphCharacterIndex;
  const Length requestedNumberOfCharacters = mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters;

  if( LAYOUT & operations )
  {
    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    // Calculate the number of glyphs to layout.
    const Vector<GlyphIndex>& charactersToGlyph = mImpl->mVisualModel->mCharactersToGlyph;
    const Vector<Length>& glyphsPerCharacter = mImpl->mVisualModel->mGlyphsPerCharacter;
    const GlyphIndex* const charactersToGlyphBuffer = charactersToGlyph.Begin();
    const Length* const glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

    const CharacterIndex lastIndex = startIndex + ( ( requestedNumberOfCharacters > 0u ) ? requestedNumberOfCharacters - 1u : 0u );
    const GlyphIndex startGlyphIndex = mImpl->mTextUpdateInfo.mStartGlyphIndex;
    const Length numberOfGlyphs = ( requestedNumberOfCharacters > 0u ) ? *( charactersToGlyphBuffer + lastIndex ) + *( glyphsPerCharacterBuffer + lastIndex ) - startGlyphIndex : 0u;
    const Length totalNumberOfGlyphs = mImpl->mVisualModel->mGlyphs.Count();

    if( 0u == totalNumberOfGlyphs )
    {
      if( UPDATE_ACTUAL_SIZE & operations )
      {
        mImpl->mVisualModel->SetLayoutSize( Size::ZERO );
      }

      // Nothing else to do if there is no glyphs.
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::DoRelayout no glyphs, view updated true\n" );
      return true;
    }

    const Vector<LineBreakInfo>& lineBreakInfo = mImpl->mLogicalModel->mLineBreakInfo;
    const Vector<WordBreakInfo>& wordBreakInfo = mImpl->mLogicalModel->mWordBreakInfo;
    const Vector<CharacterDirection>& characterDirection = mImpl->mLogicalModel->mCharacterDirections;
    const Vector<GlyphInfo>& glyphs = mImpl->mVisualModel->mGlyphs;
    const Vector<CharacterIndex>& glyphsToCharactersMap = mImpl->mVisualModel->mGlyphsToCharacters;
    const Vector<Length>& charactersPerGlyph = mImpl->mVisualModel->mCharactersPerGlyph;
    const Character* const textBuffer = mImpl->mLogicalModel->mText.Begin();

    // Set the layout parameters.
    LayoutParameters layoutParameters( size,
                                       textBuffer,
                                       lineBreakInfo.Begin(),
                                       wordBreakInfo.Begin(),
                                       ( 0u != characterDirection.Count() ) ? characterDirection.Begin() : NULL,
                                       glyphs.Begin(),
                                       glyphsToCharactersMap.Begin(),
                                       charactersPerGlyph.Begin(),
                                       charactersToGlyphBuffer,
                                       glyphsPerCharacterBuffer,
                                       totalNumberOfGlyphs );

    // Resize the vector of positions to have the same size than the vector of glyphs.
    Vector<Vector2>& glyphPositions = mImpl->mVisualModel->mGlyphPositions;
    glyphPositions.Resize( totalNumberOfGlyphs );

    // Whether the last character is a new paragraph character.
    mImpl->mTextUpdateInfo.mIsLastCharacterNewParagraph =  TextAbstraction::IsNewParagraph( *( textBuffer + ( mImpl->mLogicalModel->mText.Count() - 1u ) ) );
    layoutParameters.isLastNewParagraph = mImpl->mTextUpdateInfo.mIsLastCharacterNewParagraph;

    // The initial glyph and the number of glyphs to layout.
    layoutParameters.startGlyphIndex = startGlyphIndex;
    layoutParameters.numberOfGlyphs = numberOfGlyphs;
    layoutParameters.startLineIndex = mImpl->mTextUpdateInfo.mStartLineIndex;
    layoutParameters.estimatedNumberOfLines = mImpl->mTextUpdateInfo.mEstimatedNumberOfLines;

    // Update the visual model.
    viewUpdated = mImpl->mLayoutEngine.LayoutText( layoutParameters,
                                                   glyphPositions,
                                                   mImpl->mVisualModel->mLines,
                                                   layoutSize );

    if( viewUpdated )
    {
      // Reorder the lines
      if( REORDER & operations )
      {
        Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mImpl->mLogicalModel->mBidirectionalParagraphInfo;
        Vector<BidirectionalLineInfoRun>& bidirectionalLineInfo = mImpl->mLogicalModel->mBidirectionalLineInfo;

        // Check first if there are paragraphs with bidirectional info.
        if( 0u != bidirectionalInfo.Count() )
        {
          // Get the lines
          const Length numberOfLines = mImpl->mVisualModel->mLines.Count();

          // Reorder the lines.
          bidirectionalLineInfo.Reserve( numberOfLines ); // Reserve because is not known yet how many lines have right to left characters.
          ReorderLines( bidirectionalInfo,
                        startIndex,
                        requestedNumberOfCharacters,
                        mImpl->mVisualModel->mLines,
                        bidirectionalLineInfo );

          // Set the bidirectional info per line into the layout parameters.
          layoutParameters.lineBidirectionalInfoRunsBuffer = bidirectionalLineInfo.Begin();
          layoutParameters.numberOfBidirectionalInfoRuns = bidirectionalLineInfo.Count();

          // TODO: update the conversion map instead creating it from scratch.
          //       Note this tables store indices to characters, so update the table means modify all the indices of the text after the last updated character.
          //       It's better to refactor this. Store this table per line and don't update the indices.
          //       For the cursor position probably is better to use the function instead creating a table.
          // Set the bidirectional info into the model.
          mImpl->mLogicalModel->SetVisualToLogicalMap( layoutParameters.lineBidirectionalInfoRunsBuffer,
                                                       layoutParameters.numberOfBidirectionalInfoRuns,
                                                       0u,
                                                       mImpl->mLogicalModel->mText.Count() );

          // Re-layout the text. Reorder those lines with right to left characters.
          mImpl->mLayoutEngine.ReLayoutRightToLeftLines( layoutParameters,
                                                         startIndex,
                                                         requestedNumberOfCharacters,
                                                         glyphPositions );

        }
      } // REORDER

      // Sets the actual size.
      if( UPDATE_ACTUAL_SIZE & operations )
      {
        mImpl->mVisualModel->SetLayoutSize( layoutSize );
      }
    } // view updated

    // Store the size used to layout the text.
    mImpl->mVisualModel->mControlSize = size;
  }
  else
  {
    layoutSize = mImpl->mVisualModel->GetLayoutSize();
  }

  if( ALIGN & operations )
  {
    // The laid-out lines.
    Vector<LineRun>& lines = mImpl->mVisualModel->mLines;

    mImpl->mLayoutEngine.Align( size,
                                startIndex,
                                requestedNumberOfCharacters,
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

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | ALIGN );

    mImpl->RequestRelayout();
  }
}

LayoutEngine::VerticalAlignment Controller::GetVerticalAlignment() const
{
  return mImpl->mLayoutEngine.GetVerticalAlignment();
}

void Controller::CalculateTextAlignment( const Size& controlSize )
{
  Size layoutSize = mImpl->mVisualModel->GetLayoutSize();

  if( fabsf( layoutSize.height ) < Math::MACHINE_EPSILON_1000 )
  {
    // Get the line height of the default font.
    layoutSize.height = mImpl->GetDefaultFontLineHeight();
  }

  if( LayoutEngine::SINGLE_LINE_BOX == mImpl->mLayoutEngine.GetLayout() )
  {
    // Get the direction of the first character.
    const CharacterDirection firstParagraphDirection = mImpl->mLogicalModel->GetCharacterDirection( 0u );

    // If the first paragraph is right to left swap ALIGN_BEGIN and ALIGN_END;
    LayoutEngine::HorizontalAlignment horizontalAlignment = mImpl->mLayoutEngine.GetHorizontalAlignment();
    if( firstParagraphDirection )
    {
      switch( horizontalAlignment )
      {
        case LayoutEngine::HORIZONTAL_ALIGN_BEGIN:
        {
          horizontalAlignment = LayoutEngine::HORIZONTAL_ALIGN_END;
          break;
        }
        case LayoutEngine::HORIZONTAL_ALIGN_CENTER:
        {
          // Nothing to do.
          break;
        }
        case LayoutEngine::HORIZONTAL_ALIGN_END:
        {
          horizontalAlignment = LayoutEngine::HORIZONTAL_ALIGN_BEGIN;
          break;
        }
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
        mImpl->mAlignmentOffset.x = floorf( 0.5f * ( controlSize.width - layoutSize.width ) ); // try to avoid pixel alignment.
        break;
      }
      case LayoutEngine::HORIZONTAL_ALIGN_END:
      {
        mImpl->mAlignmentOffset.x = controlSize.width - layoutSize.width;
        break;
      }
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
      mImpl->mAlignmentOffset.y = floorf( 0.5f * ( controlSize.height - layoutSize.height ) ); // try to avoid pixel alignment.
      break;
    }
    case LayoutEngine::VERTICAL_ALIGN_BOTTOM:
    {
      mImpl->mAlignmentOffset.y = controlSize.height - layoutSize.height;
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

  if( NULL != mImpl->mEventData )
  {
    if( ( EventData::INACTIVE == mImpl->mEventData->mState ) ||
        ( EventData::INTERRUPTED == mImpl->mEventData->mState ) )
    {
      mImpl->ChangeState( EventData::EDITING );
      mImpl->mEventData->mUpdateCursorPosition = true; //If editing started without tap event, cursor update must be triggered.
    }

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

  if( NULL != mImpl->mEventData )
  {
    if( EventData::INTERRUPTED != mImpl->mEventData->mState )
    {
      mImpl->ChangeState( EventData::INACTIVE );

      if( !mImpl->IsShowingRealText() )
      {
        // Revert to regular placeholder-text when not editing
        ShowPlaceholderText();
      }
    }
  }
  mImpl->RequestRelayout();
}

bool Controller::KeyEvent( const Dali::KeyEvent& keyEvent )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected KeyEvent" );

  bool textChanged( false );

  if( ( NULL != mImpl->mEventData ) &&
      ( keyEvent.state == KeyEvent::Down ) )
  {
    int keyCode = keyEvent.keyCode;
    const std::string& keyString = keyEvent.keyPressed;

    // Pre-process to separate modifying events from non-modifying input events.
    if( Dali::DALI_KEY_ESCAPE == keyCode )
    {
      // Escape key is a special case which causes focus loss
      KeyboardFocusLostEvent();
    }
    else if( ( Dali::DALI_KEY_CURSOR_LEFT  == keyCode ) ||
             ( Dali::DALI_KEY_CURSOR_RIGHT == keyCode ) ||
             ( Dali::DALI_KEY_CURSOR_UP    == keyCode ) ||
             ( Dali::DALI_KEY_CURSOR_DOWN  == keyCode ) )
    {
      Event event( Event::CURSOR_KEY_EVENT );
      event.p1.mInt = keyCode;
      mImpl->mEventData->mEventQueue.push_back( event );
    }
    else if( Dali::DALI_KEY_BACKSPACE == keyCode )
    {
      textChanged = BackspaceKeyEvent();
    }
    else if( IsKey( keyEvent,  Dali::DALI_KEY_POWER ) )
    {
      mImpl->ChangeState( EventData::INTERRUPTED ); // State is not INACTIVE as expect to return to edit mode.
      // Avoids calling the InsertText() method which can delete selected text
    }
    else if( IsKey( keyEvent, Dali::DALI_KEY_MENU ) ||
             IsKey( keyEvent, Dali::DALI_KEY_HOME ) )
    {
      mImpl->ChangeState( EventData::INACTIVE );
      // Menu/Home key behaviour does not allow edit mode to resume like Power key
      // Avoids calling the InsertText() method which can delete selected text
    }
    else if( Dali::DALI_KEY_SHIFT_LEFT == keyCode )
    {
      // DALI_KEY_SHIFT_LEFT is the key code for the Left Shift. It's sent (by the imf?) when the predictive text is enabled
      // and a character is typed after the type of a upper case latin character.

      // Do nothing.
    }
    else
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::KeyEvent %p keyString %s\n", this, keyString.c_str() );

      // IMF manager is no longer handling key-events
      mImpl->ClearPreEditFlag();

      InsertText( keyString, COMMIT );
      textChanged = true;
    }

    if( ( mImpl->mEventData->mState != EventData::INTERRUPTED ) &&
        ( mImpl->mEventData->mState != EventData::INACTIVE ) )
    {
      mImpl->ChangeState( EventData::EDITING );
    }

    mImpl->RequestRelayout();
  }

  if( textChanged )
  {
    // Do this last since it provides callbacks into application code
    mImpl->mControlInterface.TextChanged();
  }

  return true;
}

void Controller::InsertText( const std::string& text, Controller::InsertType type )
{
  bool removedPrevious( false );
  bool maxLengthReached( false );

  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected InsertText" )

  if( NULL == mImpl->mEventData )
  {
    return;
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::InsertText %p %s (%s) mPrimaryCursorPosition %d mPreEditFlag %d mPreEditStartPosition %d mPreEditLength %d\n",
                 this, text.c_str(), (COMMIT == type ? "COMMIT" : "PRE_EDIT"),
                 mImpl->mEventData->mPrimaryCursorPosition, mImpl->mEventData->mPreEditFlag, mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength );

  // TODO: At the moment the underline runs are only for pre-edit.
  mImpl->mVisualModel->mUnderlineRuns.Clear();

  Vector<Character> utf32Characters;
  Length characterCount( 0u );

  // Remove the previous IMF pre-edit (predicitive text)
  if( mImpl->mEventData->mPreEditFlag &&
      ( 0u != mImpl->mEventData->mPreEditLength ) )
  {
    const CharacterIndex offset = mImpl->mEventData->mPrimaryCursorPosition - mImpl->mEventData->mPreEditStartPosition;

    removedPrevious = RemoveText( -static_cast<int>( offset ),
                                  mImpl->mEventData->mPreEditLength,
                                  DONT_UPDATE_INPUT_STYLE );

    mImpl->mEventData->mPrimaryCursorPosition = mImpl->mEventData->mPreEditStartPosition;
    mImpl->mEventData->mPreEditLength = 0u;
  }
  else
  {
    // Remove the previous Selection
    removedPrevious = RemoveSelectedText();
  }

  if( !text.empty() )
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

    mImpl->ChangeState( EventData::EDITING );

    // Handle the IMF (predicitive text) state changes
    if( COMMIT == type )
    {
      // IMF manager is no longer handling key-events
      mImpl->ClearPreEditFlag();
    }
    else // PRE_EDIT
    {
      if( !mImpl->mEventData->mPreEditFlag )
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Entered PreEdit state" );

        // Record the start of the pre-edit text
        mImpl->mEventData->mPreEditStartPosition = mImpl->mEventData->mPrimaryCursorPosition;
      }

      mImpl->mEventData->mPreEditLength = utf32Characters.Count();
      mImpl->mEventData->mPreEditFlag = true;

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "mPreEditStartPosition %d mPreEditLength %d\n", mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength );
    }

    const Length numberOfCharactersInModel = mImpl->mLogicalModel->mText.Count();

    // Restrict new text to fit within Maximum characters setting.
    Length maxSizeOfNewText = std::min( ( mImpl->mMaximumNumberOfCharacters - numberOfCharactersInModel ), characterCount );
    maxLengthReached = ( characterCount > maxSizeOfNewText );

    // The cursor position.
    CharacterIndex& cursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

    // Update the text's style.

    // Updates the text style runs by adding characters.
    mImpl->mLogicalModel->UpdateTextStyleRuns( cursorIndex, maxSizeOfNewText );

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = ( cursorIndex > 0u ) ? cursorIndex - 1u : 0u;

    // Retrieve the text's style for the given index.
    InputStyle style;
    mImpl->RetrieveDefaultInputStyle( style );
    mImpl->mLogicalModel->RetrieveStyle( styleIndex, style );

    // Whether to add a new text color run.
    const bool addColorRun = ( style.textColor != mImpl->mEventData->mInputStyle.textColor );

    // Whether to add a new font run.
    const bool addFontNameRun = style.familyName != mImpl->mEventData->mInputStyle.familyName;
    const bool addFontWeightRun = style.weight != mImpl->mEventData->mInputStyle.weight;
    const bool addFontWidthRun = style.width != mImpl->mEventData->mInputStyle.width;
    const bool addFontSlantRun = style.slant != mImpl->mEventData->mInputStyle.slant;
    const bool addFontSizeRun = style.size != mImpl->mEventData->mInputStyle.size;

    // Add style runs.
    if( addColorRun )
    {
      const VectorBase::SizeType numberOfRuns = mImpl->mLogicalModel->mColorRuns.Count();
      mImpl->mLogicalModel->mColorRuns.Resize( numberOfRuns + 1u );

      ColorRun& colorRun = *( mImpl->mLogicalModel->mColorRuns.Begin() + numberOfRuns );
      colorRun.color = mImpl->mEventData->mInputStyle.textColor;
      colorRun.characterRun.characterIndex = cursorIndex;
      colorRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    if( addFontNameRun   ||
        addFontWeightRun ||
        addFontWidthRun  ||
        addFontSlantRun  ||
        addFontSizeRun )
    {
      const VectorBase::SizeType numberOfRuns = mImpl->mLogicalModel->mFontDescriptionRuns.Count();
      mImpl->mLogicalModel->mFontDescriptionRuns.Resize( numberOfRuns + 1u );

      FontDescriptionRun& fontDescriptionRun = *( mImpl->mLogicalModel->mFontDescriptionRuns.Begin() + numberOfRuns );

      if( addFontNameRun )
      {
        fontDescriptionRun.familyLength = mImpl->mEventData->mInputStyle.familyName.size();
        fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
        memcpy( fontDescriptionRun.familyName, mImpl->mEventData->mInputStyle.familyName.c_str(), fontDescriptionRun.familyLength );
        fontDescriptionRun.familyDefined = true;

        // The memory allocated for the font family name is freed when the font description is removed from the logical model.
      }

      if( addFontWeightRun )
      {
        fontDescriptionRun.weight = mImpl->mEventData->mInputStyle.weight;
        fontDescriptionRun.weightDefined = true;
      }

      if( addFontWidthRun )
      {
        fontDescriptionRun.width = mImpl->mEventData->mInputStyle.width;
        fontDescriptionRun.widthDefined = true;
      }

      if( addFontSlantRun )
      {
        fontDescriptionRun.slant = mImpl->mEventData->mInputStyle.slant;
        fontDescriptionRun.slantDefined = true;
      }

      if( addFontSizeRun )
      {
        fontDescriptionRun.size = static_cast<PointSize26Dot6>( mImpl->mEventData->mInputStyle.size * 64.f );
        fontDescriptionRun.sizeDefined = true;
      }

      fontDescriptionRun.characterRun.characterIndex = cursorIndex;
      fontDescriptionRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    // Insert at current cursor position.
    Vector<Character>& modifyText = mImpl->mLogicalModel->mText;

    if( cursorIndex < numberOfCharactersInModel )
    {
      modifyText.Insert( modifyText.Begin() + cursorIndex, utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText );
    }
    else
    {
      modifyText.Insert( modifyText.End(), utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText );
    }

    // Mark the first paragraph to be updated.
    mImpl->mTextUpdateInfo.mCharacterIndex = std::min( cursorIndex, mImpl->mTextUpdateInfo.mCharacterIndex );
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd += maxSizeOfNewText;

    // Update the cursor index.
    cursorIndex += maxSizeOfNewText;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Inserted %d characters, new size %d new cursor %d\n", maxSizeOfNewText, mImpl->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition );
  }

  if( ( 0u == mImpl->mLogicalModel->mText.Count() ) &&
      mImpl->IsPlaceholderAvailable() )
  {
    // Show place-holder if empty after removing the pre-edit text
    ShowPlaceholderText();
    mImpl->mEventData->mUpdateCursorPosition = true;
    mImpl->ClearPreEditFlag();
  }
  else if( removedPrevious ||
           ( 0 != utf32Characters.Count() ) )
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

  if( EventData::SELECTING == mImpl->mEventData->mState )
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
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "TapEvent state:%d \n", mImpl->mEventData->mState );

    if( 1u == tapCount )
    {
      // This is to avoid unnecessary relayouts when tapping an empty text-field
      bool relayoutNeeded( false );

      if( ( EventData::EDITING_WITH_PASTE_POPUP == mImpl->mEventData->mState ) ||
          ( EventData::EDITING_WITH_PASTE_POPUP == mImpl->mEventData->mState ) )
      {
        mImpl->ChangeState( EventData::EDITING_WITH_GRAB_HANDLE);  // If Popup shown hide it here so can be shown again if required.
      }

      if( mImpl->IsShowingRealText() && ( EventData::INACTIVE != mImpl->mEventData->mState ) )
      {
        // Already in an active state so show a popup
        if( !mImpl->IsClipboardEmpty() )
        {
          // Shows Paste popup but could show full popup with Selection options. ( EDITING_WITH_POPUP )
          mImpl->ChangeState( EventData::EDITING_WITH_PASTE_POPUP );
        }
        else
        {
          mImpl->ChangeState( EventData::EDITING_WITH_GRAB_HANDLE );
        }
        relayoutNeeded = true;
      }
      else
      {
        if( mImpl->IsShowingPlaceholderText() && !mImpl->IsFocusedPlaceholderAvailable() )
        {
          // Hide placeholder text
          ResetText();
        }

        if( EventData::INACTIVE == mImpl->mEventData->mState )
        {
          mImpl->ChangeState( EventData::EDITING );
        }
        else if( !mImpl->IsClipboardEmpty() )
        {
          mImpl->ChangeState( EventData::EDITING_WITH_POPUP );
        }
        relayoutNeeded = true;
      }

      // Handles & cursors must be repositioned after Relayout() i.e. after the Model has been updated
      if( relayoutNeeded )
      {
        Event event( Event::TAP_EVENT );
        event.p1.mUint = tapCount;
        event.p2.mFloat = x;
        event.p3.mFloat = y;
        mImpl->mEventData->mEventQueue.push_back( event );

        mImpl->RequestRelayout();
      }
    }
    else if( 2u == tapCount )
    {
      if( mImpl->mEventData->mSelectionEnabled &&
          mImpl->IsShowingRealText() )
      {
        SelectEvent( x, y, false );
      }
    }
  }

  // Reset keyboard as tap event has occurred.
  mImpl->ResetImfManager();
}

void Controller::PanEvent( Gesture::State state, const Vector2& displacement )
        // Show cursor and grabhandle on first tap, this matches the behaviour of tapping when already editing
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected PanEvent" );

  if( NULL != mImpl->mEventData )
  {
    Event event( Event::PAN_EVENT );
    event.p1.mInt = state;
    event.p2.mFloat = displacement.x;
    event.p3.mFloat = displacement.y;
    mImpl->mEventData->mEventQueue.push_back( event );

    mImpl->RequestRelayout();
  }
}

void Controller::LongPressEvent( Gesture::State state, float x, float y  )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected LongPressEvent" );

  if( ( state == Gesture::Started ) &&
      ( NULL != mImpl->mEventData ) )
  {
    if( !mImpl->IsShowingRealText() )
    {
      Event event( Event::LONG_PRESS_EVENT );
      event.p1.mInt = state;
      mImpl->mEventData->mEventQueue.push_back( event );
      mImpl->RequestRelayout();
    }
    else
    {
      // The 1st long-press on inactive text-field is treated as tap
      if( EventData::INACTIVE == mImpl->mEventData->mState )
      {
        mImpl->ChangeState( EventData::EDITING );

        Event event( Event::TAP_EVENT );
        event.p1.mUint = 1;
        event.p2.mFloat = x;
        event.p3.mFloat = y;
        mImpl->mEventData->mEventQueue.push_back( event );

        mImpl->RequestRelayout();
      }
      else
      {
        // Reset the imf manger to commit the pre-edit before selecting the text.
        mImpl->ResetImfManager();

        SelectEvent( x, y, false );
      }
    }
  }
}

void Controller::SelectEvent( float x, float y, bool selectAll )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::SelectEvent\n" );

  if( NULL != mImpl->mEventData )
  {
    mImpl->ChangeState( EventData::SELECTING );

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
  targetSize = mImpl->mVisualModel->mControlSize;
}

void Controller::AddDecoration( Actor& actor, bool needsClipping )
{
  mImpl->mControlInterface.AddDecoration( actor, needsClipping );
}

void Controller::DecorationEvent( HandleType handleType, HandleState state, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected DecorationEvent" );

  if( NULL != mImpl->mEventData )
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
      case LEFT_SELECTION_HANDLE_MARKER:
      case RIGHT_SELECTION_HANDLE_MARKER:
      {
        // Markers do not move the handles.
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

void Controller::PasteText( const std::string& stringToPaste )
{
  InsertText( stringToPaste, Text::Controller::COMMIT );
  mImpl->ChangeState( EventData::EDITING );
  mImpl->RequestRelayout();

  // Do this last since it provides callbacks into application code
  mImpl->mControlInterface.TextChanged();
}

void Controller::PasteClipboardItemEvent()
{
  // Retrieve the clipboard contents first
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );
  std::string stringToPaste( notifier.GetContent() );

  // Commit the current pre-edit text; the contents of the clipboard should be appended
  mImpl->ResetImfManager();

  // Paste
  PasteText( stringToPaste );
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

      // This is to reset the virtual keyboard to Upper-case
      if( 0u == mImpl->mLogicalModel->mText.Count() )
      {
        NotifyImfManager();
      }

      if( ( 0u != mImpl->mLogicalModel->mText.Count() ) ||
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
      PasteText( stringToPaste );
      break;
    }
    case Toolkit::TextSelectionPopup::SELECT:
    {
      const Vector2& currentCursorPosition = mImpl->mEventData->mDecorator->GetPosition( PRIMARY_CURSOR );

      if( mImpl->mEventData->mSelectionEnabled )
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
      mImpl->ShowClipboard();
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
  bool update = false;
  bool requestRelayout = false;

  std::string text;
  unsigned int cursorPosition = 0u;

  switch( imfEvent.eventName )
  {
    case ImfManager::COMMIT:
    {
      InsertText( imfEvent.predictiveString, Text::Controller::COMMIT );
      update = true;
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
      update = RemoveText( imfEvent.cursorOffset,
                           imfEvent.numberOfChars,
                           DONT_UPDATE_INPUT_STYLE );

      if( update )
      {
        if( ( 0u != mImpl->mLogicalModel->mText.Count() ) ||
            !mImpl->IsPlaceholderAvailable() )
        {
          mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
        }
        else
        {
          ShowPlaceholderText();
          mImpl->mEventData->mUpdateCursorPosition = true;
        }
      }
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

bool Controller::BackspaceKeyEvent()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::KeyEvent %p DALI_KEY_BACKSPACE\n", this );

  bool removed = false;

  if( NULL == mImpl->mEventData )
  {
    return removed;
  }

  // IMF manager is no longer handling key-events
  mImpl->ClearPreEditFlag();

  if( EventData::SELECTING == mImpl->mEventData->mState )
  {
    removed = RemoveSelectedText();
  }
  else if( mImpl->mEventData->mPrimaryCursorPosition > 0 )
  {
    // Remove the character before the current cursor position
    removed = RemoveText( -1,
                          1,
                          UPDATE_INPUT_STYLE );
  }

  if( removed )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::KeyEvent %p DALI_KEY_BACKSPACE RemovedText\n", this );
    // Notifiy the IMF manager after text changed
    // Automatic  Upper-case and restarting prediction on an existing word require this.
    NotifyImfManager();

    if( ( 0u != mImpl->mLogicalModel->mText.Count() ) ||
        !mImpl->IsPlaceholderAvailable() )
    {
      mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
    }
    else
    {
      ShowPlaceholderText();
      mImpl->mEventData->mUpdateCursorPosition = true;
    }
  }

  return removed;
}

void Controller::NotifyImfManager()
{
  if( NULL != mImpl->mEventData )
  {
    if( mImpl->mEventData->mImfManager )
    {
      // Notifying IMF of a cursor change triggers a surrounding text request so updating it now.
      std::string text;
      GetText( text );
      mImpl->mEventData->mImfManager.SetSurroundingText( text );

      mImpl->mEventData->mImfManager.SetCursorPosition( GetLogicalCursorPosition() );
      mImpl->mEventData->mImfManager.NotifyCursorPosition();
    }
  }
}

void Controller::ShowPlaceholderText()
{
  if( mImpl->IsPlaceholderAvailable() )
  {
    DALI_ASSERT_DEBUG( mImpl->mEventData && "No placeholder text available" );

    if( NULL == mImpl->mEventData )
    {
      return;
    }

    mImpl->mEventData->mIsShowingPlaceholderText = true;

    // Disable handles when showing place-holder text
    mImpl->mEventData->mDecorator->SetHandleActive( GRAB_HANDLE, false );
    mImpl->mEventData->mDecorator->SetHandleActive( LEFT_SELECTION_HANDLE, false );
    mImpl->mEventData->mDecorator->SetHandleActive( RIGHT_SELECTION_HANDLE, false );

    const char* text( NULL );
    size_t size( 0 );

    // TODO - Switch placeholder text styles when changing state
    if( ( EventData::INACTIVE != mImpl->mEventData->mState ) &&
        ( 0u != mImpl->mEventData->mPlaceholderTextActive.c_str() ) )
    {
      text = mImpl->mEventData->mPlaceholderTextActive.c_str();
      size = mImpl->mEventData->mPlaceholderTextActive.size();
    }
    else
    {
      text = mImpl->mEventData->mPlaceholderTextInactive.c_str();
      size = mImpl->mEventData->mPlaceholderTextInactive.size();
    }

    mImpl->mTextUpdateInfo.mCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;

    // Reset model for showing placeholder.
    mImpl->mLogicalModel->mText.Clear();
    mImpl->mVisualModel->SetTextColor( mImpl->mEventData->mPlaceholderTextColor );

    // Convert text into UTF-32
    Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;
    utf32Characters.Resize( size );

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    const Length characterCount = Utf8ToUtf32( utf8, size, utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    // The characters to be added.
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = characterCount;

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

void Controller::ClearFontData()
{
  if( mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  }
  mImpl->mVisualModel->ClearCaches();

  mImpl->mTextUpdateInfo.mCharacterIndex = 0u;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mLogicalModel->mText.Count();

  mImpl->mTextUpdateInfo.mClearAll = true;
  mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                           VALIDATE_FONTS            |
                                                           SHAPE_TEXT                |
                                                           GET_GLYPH_METRICS         |
                                                           LAYOUT                    |
                                                           UPDATE_ACTUAL_SIZE        |
                                                           REORDER                   |
                                                           ALIGN );
}

void Controller::ClearStyleData()
{
  mImpl->mLogicalModel->mColorRuns.Clear();
  mImpl->mLogicalModel->ClearFontDescriptionRuns();
}

Controller::Controller( ControlInterface& controlInterface )
: mImpl( NULL )
{
  mImpl = new Controller::Impl( controlInterface );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
