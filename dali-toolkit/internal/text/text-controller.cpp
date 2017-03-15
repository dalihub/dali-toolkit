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
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
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

/**
 * @brief Adds a new font description run for the selected text.
 *
 * The new font parameters are added after the call to this method.
 *
 * @param[in] eventData The event data pointer.
 * @param[in] logicalModel The logical model where to add the new font description run.
 * @param[out] startOfSelectedText Index to the first selected character.
 * @param[out] lengthOfSelectedText Number of selected characters.
 */
FontDescriptionRun& UpdateSelectionFontStyleRun( EventData* eventData,
                                                 LogicalModelPtr logicalModel,
                                                 CharacterIndex& startOfSelectedText,
                                                 Length& lengthOfSelectedText )
{
  const bool handlesCrossed = eventData->mLeftSelectionPosition > eventData->mRightSelectionPosition;

  // Get start and end position of selection
  startOfSelectedText = handlesCrossed ? eventData->mRightSelectionPosition : eventData->mLeftSelectionPosition;
  lengthOfSelectedText = ( handlesCrossed ? eventData->mLeftSelectionPosition : eventData->mRightSelectionPosition ) - startOfSelectedText;

  // Add the font run.
  const VectorBase::SizeType numberOfRuns = logicalModel->mFontDescriptionRuns.Count();
  logicalModel->mFontDescriptionRuns.Resize( numberOfRuns + 1u );

  FontDescriptionRun& fontDescriptionRun = *( logicalModel->mFontDescriptionRuns.Begin() + numberOfRuns );

  fontDescriptionRun.characterRun.characterIndex = startOfSelectedText;
  fontDescriptionRun.characterRun.numberOfCharacters = lengthOfSelectedText;

  // Recalculate the selection highlight as the metrics may have changed.
  eventData->mUpdateLeftSelectionPosition = true;
  eventData->mUpdateRightSelectionPosition = true;
  eventData->mUpdateHighlightBox = true;

  return fontDescriptionRun;
}

// public : Constructor.

ControllerPtr Controller::New()
{
  return ControllerPtr( new Controller() );
}

ControllerPtr Controller::New( ControlInterface* controlInterface )
{
  return ControllerPtr( new Controller( controlInterface ) );
}

ControllerPtr Controller::New( ControlInterface* controlInterface,
                               EditableControlInterface* editableControlInterface )
{
  return ControllerPtr( new Controller( controlInterface,
                                        editableControlInterface ) );
}

// public : Configure the text controller.

void Controller::EnableTextInput( DecoratorPtr decorator )
{
  if( !decorator )
  {
    delete mImpl->mEventData;
    mImpl->mEventData = NULL;

    // Nothing else to do.
    return;
  }

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

void Controller::SetAutoScrollEnabled( bool enable )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled[%s] SingleBox[%s]-> [%p]\n", (enable)?"true":"false", ( mImpl->mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX)?"true":"false", this );

  if( mImpl->mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX )
  {
    if( enable )
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled for SINGLE_LINE_BOX\n" );
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               LAYOUT                    |
                                                               ALIGN                     |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               UPDATE_DIRECTION          |
                                                               REORDER );

    }
    else
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::SetAutoScrollEnabled Disabling autoscroll\n");
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               LAYOUT                    |
                                                               ALIGN                     |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               REORDER );
    }

    mImpl->mIsAutoScrollEnabled = enable;
    mImpl->RequestRelayout();
  }
  else
  {
    DALI_LOG_WARNING( "Attempted AutoScrolling on a non SINGLE_LINE_BOX, request ignored\n" );
    mImpl->mIsAutoScrollEnabled = false;
  }
}

bool Controller::IsAutoScrollEnabled() const
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::IsAutoScrollEnabled[%s]\n", mImpl->mIsAutoScrollEnabled?"true":"false" );

  return mImpl->mIsAutoScrollEnabled;
}

CharacterDirection Controller::GetAutoScrollDirection() const
{
  return mImpl->mAutoScrollDirectionRTL;
}

float Controller::GetAutoScrollLineAlignment() const
{
  float offset = 0.f;

  if( mImpl->mModel->mVisualModel &&
      ( 0u != mImpl->mModel->mVisualModel->mLines.Count() ) )
  {
    offset = ( *mImpl->mModel->mVisualModel->mLines.Begin() ).alignmentOffset;
  }

  return offset;
}

void Controller::SetHorizontalScrollEnabled( bool enable )
{
  if( ( NULL != mImpl->mEventData ) &&
      mImpl->mEventData->mDecorator )
  {
    mImpl->mEventData->mDecorator->SetHorizontalScrollEnabled( enable );
  }
}
bool Controller::IsHorizontalScrollEnabled() const
{
  if( ( NULL != mImpl->mEventData ) &&
      mImpl->mEventData->mDecorator )
  {
    return mImpl->mEventData->mDecorator->IsHorizontalScrollEnabled();
  }

  return false;
}

void Controller::SetVerticalScrollEnabled( bool enable )
{
  if( ( NULL != mImpl->mEventData ) &&
      mImpl->mEventData->mDecorator )
  {
    if( mImpl->mEventData->mDecorator )
    {
      mImpl->mEventData->mDecorator->SetVerticalScrollEnabled( enable );
    }
  }
}

bool Controller::IsVerticalScrollEnabled() const
{
  if( ( NULL != mImpl->mEventData ) &&
      mImpl->mEventData->mDecorator )
  {
    return mImpl->mEventData->mDecorator->IsVerticalScrollEnabled();
  }

  return false;
}

void Controller::SetSmoothHandlePanEnabled( bool enable )
{
  if( ( NULL != mImpl->mEventData ) &&
      mImpl->mEventData->mDecorator )
  {
    mImpl->mEventData->mDecorator->SetSmoothHandlePanEnabled( enable );
  }
}

bool Controller::IsSmoothHandlePanEnabled() const
{
  if( ( NULL != mImpl->mEventData ) &&
      mImpl->mEventData->mDecorator )
  {
    return mImpl->mEventData->mDecorator->IsSmoothHandlePanEnabled();
  }

  return false;
}

void Controller::SetMaximumNumberOfCharacters( Length maxCharacters )
{
  mImpl->mMaximumNumberOfCharacters = maxCharacters;
}

int Controller::GetMaximumNumberOfCharacters()
{
  return mImpl->mMaximumNumberOfCharacters;
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

void Controller::SetMultiLineEnabled( bool enable )
{
  const Layout::Engine::Type layout = enable ? Layout::Engine::MULTI_LINE_BOX : Layout::Engine::SINGLE_LINE_BOX;

  if( layout != mImpl->mLayoutEngine.GetLayout() )
  {
    // Set the layout type.
    mImpl->mLayoutEngine.SetLayout( layout );

    // Set the flags to redo the layout operations
    const OperationsMask layoutOperations =  static_cast<OperationsMask>( LAYOUT             |
                                                                          UPDATE_LAYOUT_SIZE |
                                                                          ALIGN              |
                                                                          REORDER );

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | layoutOperations );

    mImpl->RequestRelayout();
  }
}

bool Controller::IsMultiLineEnabled() const
{
  return Layout::Engine::MULTI_LINE_BOX == mImpl->mLayoutEngine.GetLayout();
}

void Controller::SetHorizontalAlignment( Layout::HorizontalAlignment alignment )
{
  if( alignment != mImpl->mModel->mHorizontalAlignment )
  {
    // Set the alignment.
    mImpl->mModel->mHorizontalAlignment = alignment;

    // Set the flag to redo the alignment operation.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | ALIGN );

    mImpl->RequestRelayout();
  }
}

Layout::HorizontalAlignment Controller::GetHorizontalAlignment() const
{
  return mImpl->mModel->mHorizontalAlignment;
}

void Controller::SetVerticalAlignment( Layout::VerticalAlignment alignment )
{
  if( alignment != mImpl->mModel->mVerticalAlignment )
  {
    // Set the alignment.
    mImpl->mModel->mVerticalAlignment = alignment;

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | ALIGN );

    mImpl->RequestRelayout();
  }
}

Layout::VerticalAlignment Controller::GetVerticalAlignment() const
{
  return mImpl->mModel->mVerticalAlignment;
}

void Controller::SetTextElideEnabled( bool enabled )
{
  mImpl->mModel->mElideEnabled = enabled;
}

bool Controller::IsTextElideEnabled() const
{
  return mImpl->mModel->mElideEnabled;
}

// public : Update

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
    mImpl->mModel->mVisualModel->SetTextColor( mImpl->mTextColor );

    MarkupProcessData markupProcessData( mImpl->mModel->mLogicalModel->mColorRuns,
                                         mImpl->mModel->mLogicalModel->mFontDescriptionRuns );

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
    Vector<Character>& utf32Characters = mImpl->mModel->mLogicalModel->mText;
    utf32Characters.Resize( textSize );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32( utf8, textSize, utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    DALI_ASSERT_DEBUG( textSize >= characterCount && "Invalid UTF32 conversion length" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::SetText %p UTF8 size %d, UTF32 size %d\n", this, textSize, mImpl->mModel->mLogicalModel->mText.Count() );

    // The characters to be added.
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mModel->mLogicalModel->mText.Count();

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

  // Do this last since it provides callbacks into application code.
  if( NULL != mImpl->mEditableControlInterface )
  {
    mImpl->mEditableControlInterface->TextChanged();
  }
}

void Controller::GetText( std::string& text ) const
{
  if( !mImpl->IsShowingPlaceholderText() )
  {
    // Retrieves the text string.
    mImpl->GetText( 0u, text );
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::GetText %p empty (but showing placeholder)\n", this );
  }
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
        ( 0u == mImpl->mModel->mLogicalModel->mText.Count() ) )
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

void Controller::UpdateAfterFontChange( const std::string& newDefaultFont )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::UpdateAfterFontChange\n");

  if( !mImpl->mFontDefaults->familyDefined ) // If user defined font then should not update when system font changes
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "Controller::UpdateAfterFontChange newDefaultFont(%s)\n", newDefaultFont.c_str() );
    mImpl->mFontDefaults->mFontDescription.family = newDefaultFont;

    ClearFontData();

    mImpl->RequestRelayout();
  }
}

// public : Default style & Input style

void Controller::SetDefaultFontFamily( const std::string& defaultFontFamily )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.family = defaultFontFamily;
  DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::SetDefaultFontFamily %s\n", defaultFontFamily.c_str());
  mImpl->mFontDefaults->familyDefined = !defaultFontFamily.empty();

  // Clear the font-specific data
  ClearFontData();

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

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWeightDefined() const
{
  return mImpl->mFontDefaults->weightDefined;
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

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWidthDefined() const
{
  return mImpl->mFontDefaults->widthDefined;
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

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontSlantDefined() const
{
  return mImpl->mFontDefaults->slantDefined;
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

  // Clear the font-specific data
  ClearFontData();

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

void Controller::SetDefaultColor( const Vector4& color )
{
  mImpl->mTextColor = color;

  if( !mImpl->IsShowingPlaceholderText() )
  {
    mImpl->mModel->mVisualModel->SetTextColor( color );

    mImpl->RequestRelayout();
  }
}

const Vector4& Controller::GetDefaultColor() const
{
  return mImpl->mTextColor;
}

void Controller::SetPlaceholderTextColor( const Vector4& textColor )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mPlaceholderTextColor = textColor;
  }

  if( mImpl->IsShowingPlaceholderText() )
  {
    mImpl->mModel->mVisualModel->SetTextColor( textColor );
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
  mImpl->mModel->mVisualModel->SetShadowOffset( shadowOffset );

  mImpl->RequestRelayout();
}

const Vector2& Controller::GetShadowOffset() const
{
  return mImpl->mModel->mVisualModel->GetShadowOffset();
}

void Controller::SetShadowColor( const Vector4& shadowColor )
{
  mImpl->mModel->mVisualModel->SetShadowColor( shadowColor );

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetShadowColor() const
{
  return mImpl->mModel->mVisualModel->GetShadowColor();
}

void Controller::SetUnderlineColor( const Vector4& color )
{
  mImpl->mModel->mVisualModel->SetUnderlineColor( color );

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetUnderlineColor() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineColor();
}

void Controller::SetUnderlineEnabled( bool enabled )
{
  mImpl->mModel->mVisualModel->SetUnderlineEnabled( enabled );

  mImpl->RequestRelayout();
}

bool Controller::IsUnderlineEnabled() const
{
  return mImpl->mModel->mVisualModel->IsUnderlineEnabled();
}

void Controller::SetUnderlineHeight( float height )
{
  mImpl->mModel->mVisualModel->SetUnderlineHeight( height );

  mImpl->RequestRelayout();
}

float Controller::GetUnderlineHeight() const
{
  return mImpl->mModel->mVisualModel->GetUnderlineHeight();
}

void Controller::SetDefaultEmbossProperties( const std::string& embossProperties )
{
  if( NULL == mImpl->mEmbossDefaults )
  {
    mImpl->mEmbossDefaults = new EmbossDefaults();
  }

  mImpl->mEmbossDefaults->properties = embossProperties;
}

const std::string& Controller::GetDefaultEmbossProperties() const
{
  if( NULL != mImpl->mEmbossDefaults )
  {
    return mImpl->mEmbossDefaults->properties;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultOutlineProperties( const std::string& outlineProperties )
{
  if( NULL == mImpl->mOutlineDefaults )
  {
    mImpl->mOutlineDefaults = new OutlineDefaults();
  }

  mImpl->mOutlineDefaults->properties = outlineProperties;
}

const std::string& Controller::GetDefaultOutlineProperties() const
{
  if( NULL != mImpl->mOutlineDefaults )
  {
    return mImpl->mOutlineDefaults->properties;
  }

  return EMPTY_STRING;
}

void Controller::SetDefaultLineSpacing( float lineSpacing )
{
  //TODO finish implementation
  mImpl->mLayoutEngine.SetDefaultLineSpacing( lineSpacing );
}

float Controller::GetDefaultLineSpacing() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSpacing();
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
      const VectorBase::SizeType numberOfRuns = mImpl->mModel->mLogicalModel->mColorRuns.Count();
      mImpl->mModel->mLogicalModel->mColorRuns.Resize( numberOfRuns + 1u );

      ColorRun& colorRun = *( mImpl->mModel->mLogicalModel->mColorRuns.Begin() + numberOfRuns );
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
    mImpl->mEventData->mInputStyle.isFamilyDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      CharacterIndex startOfSelectedText = 0u;
      Length lengthOfSelectedText = 0u;
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mModel->mLogicalModel,
                                                                            startOfSelectedText,
                                                                            lengthOfSelectedText );

      fontDescriptionRun.familyLength = fontFamily.size();
      fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
      memcpy( fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength );
      fontDescriptionRun.familyDefined = true;

      // The memory allocated for the font family name is freed when the font description is removed from the logical model.

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               VALIDATE_FONTS            |
                                                               SHAPE_TEXT                |
                                                               GET_GLYPH_METRICS         |
                                                               LAYOUT                    |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               REORDER                   |
                                                               ALIGN );
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      mImpl->mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = lengthOfSelectedText;

      // As the font changes, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mUpdateHighlightBox = true;
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

void Controller::SetInputFontWeight( FontWeight weight )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.weight = weight;
    mImpl->mEventData->mInputStyle.isWeightDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      CharacterIndex startOfSelectedText = 0u;
      Length lengthOfSelectedText = 0u;
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mModel->mLogicalModel,
                                                                            startOfSelectedText,
                                                                            lengthOfSelectedText );

      fontDescriptionRun.weight = weight;
      fontDescriptionRun.weightDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               VALIDATE_FONTS            |
                                                               SHAPE_TEXT                |
                                                               GET_GLYPH_METRICS         |
                                                               LAYOUT                    |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               REORDER                   |
                                                               ALIGN );
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      mImpl->mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = lengthOfSelectedText;

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mUpdateHighlightBox = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

bool Controller::IsInputFontWeightDefined() const
{
  bool defined = false;

  if( NULL != mImpl->mEventData )
  {
    defined = mImpl->mEventData->mInputStyle.isWeightDefined;
  }

  return defined;
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
    mImpl->mEventData->mInputStyle.isWidthDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      CharacterIndex startOfSelectedText = 0u;
      Length lengthOfSelectedText = 0u;
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mModel->mLogicalModel,
                                                                            startOfSelectedText,
                                                                            lengthOfSelectedText );

      fontDescriptionRun.width = width;
      fontDescriptionRun.widthDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               VALIDATE_FONTS            |
                                                               SHAPE_TEXT                |
                                                               GET_GLYPH_METRICS         |
                                                               LAYOUT                    |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               REORDER                   |
                                                               ALIGN );
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      mImpl->mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = lengthOfSelectedText;

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mUpdateHighlightBox = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

bool Controller::IsInputFontWidthDefined() const
{
  bool defined = false;

  if( NULL != mImpl->mEventData )
  {
    defined = mImpl->mEventData->mInputStyle.isWidthDefined;
  }

  return defined;
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
    mImpl->mEventData->mInputStyle.isSlantDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      CharacterIndex startOfSelectedText = 0u;
      Length lengthOfSelectedText = 0u;
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mModel->mLogicalModel,
                                                                            startOfSelectedText,
                                                                            lengthOfSelectedText );

      fontDescriptionRun.slant = slant;
      fontDescriptionRun.slantDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               VALIDATE_FONTS            |
                                                               SHAPE_TEXT                |
                                                               GET_GLYPH_METRICS         |
                                                               LAYOUT                    |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               REORDER                   |
                                                               ALIGN );
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      mImpl->mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = lengthOfSelectedText;

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mUpdateHighlightBox = true;
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }
}

bool Controller::IsInputFontSlantDefined() const
{
  bool defined = false;

  if( NULL != mImpl->mEventData )
  {
    defined = mImpl->mEventData->mInputStyle.isSlantDefined;
  }

  return defined;
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
    mImpl->mEventData->mInputStyle.isSizeDefined = true;

    if( EventData::SELECTING == mImpl->mEventData->mState )
    {
      CharacterIndex startOfSelectedText = 0u;
      Length lengthOfSelectedText = 0u;
      FontDescriptionRun& fontDescriptionRun = UpdateSelectionFontStyleRun( mImpl->mEventData,
                                                                            mImpl->mModel->mLogicalModel,
                                                                            startOfSelectedText,
                                                                            lengthOfSelectedText );

      fontDescriptionRun.size = static_cast<PointSize26Dot6>( size * 64.f );
      fontDescriptionRun.sizeDefined = true;

      // Request to relayout.
      mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                               VALIDATE_FONTS            |
                                                               SHAPE_TEXT                |
                                                               GET_GLYPH_METRICS         |
                                                               LAYOUT                    |
                                                               UPDATE_LAYOUT_SIZE        |
                                                               REORDER                   |
                                                               ALIGN );
      mImpl->mRecalculateNaturalSize = true;
      mImpl->RequestRelayout();

      mImpl->mTextUpdateInfo.mCharacterIndex = startOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = lengthOfSelectedText;

      // As the font might change, recalculate the handle positions is needed.
      mImpl->mEventData->mUpdateLeftSelectionPosition = true;
      mImpl->mEventData->mUpdateRightSelectionPosition = true;
      mImpl->mEventData->mUpdateHighlightBox = true;
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

void Controller::SetInputLineSpacing( float lineSpacing )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.lineSpacing = lineSpacing;
    mImpl->mEventData->mInputStyle.isLineSpacingDefined = true;
  }
}

float Controller::GetInputLineSpacing() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.lineSpacing;
  }

  return 0.f;
}

void Controller::SetInputShadowProperties( const std::string& shadowProperties )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.shadowProperties = shadowProperties;
  }
}

const std::string& Controller::GetInputShadowProperties() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.shadowProperties;
  }

  return EMPTY_STRING;
}

void Controller::SetInputUnderlineProperties( const std::string& underlineProperties )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.underlineProperties = underlineProperties;
  }
}

const std::string& Controller::GetInputUnderlineProperties() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.underlineProperties;
  }

  return EMPTY_STRING;
}

void Controller::SetInputEmbossProperties( const std::string& embossProperties )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.embossProperties = embossProperties;
  }
}

const std::string& Controller::GetInputEmbossProperties() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.embossProperties;
  }

  return GetDefaultEmbossProperties();
}

void Controller::SetInputOutlineProperties( const std::string& outlineProperties )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.outlineProperties = outlineProperties;
  }
}

const std::string& Controller::GetInputOutlineProperties() const
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mInputStyle.outlineProperties;
  }

  return GetDefaultOutlineProperties();
}

void Controller::SetInputModePassword( bool passwordInput )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mPasswordInput = passwordInput;
  }
}

bool Controller::IsInputModePassword()
{
  if( NULL != mImpl->mEventData )
  {
    return mImpl->mEventData->mPasswordInput;
  }
  return false;
}

// public : Queries & retrieves.

Layout::Engine& Controller::GetLayoutEngine()
{
  return mImpl->mLayoutEngine;
}

View& Controller::GetView()
{
  return mImpl->mView;
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

    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel( onlyOnceOperations );

    // Layout the text for the new width.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | LAYOUT );

    // Store the actual control's size to restore later.
    const Size actualControlSize = mImpl->mModel->mVisualModel->mControlSize;

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
    mImpl->mModel->mVisualModel->SetNaturalSize( naturalSize.GetVectorXY() );

    mImpl->mRecalculateNaturalSize = false;

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // Restore the actual control's size.
    mImpl->mModel->mVisualModel->mControlSize = actualControlSize;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize calculated %f,%f,%f\n", naturalSize.x, naturalSize.y, naturalSize.z );
  }
  else
  {
    naturalSize = mImpl->mModel->mVisualModel->GetNaturalSize();

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
  if( fabsf( width - mImpl->mModel->mVisualModel->mControlSize.width ) > Math::MACHINE_EPSILON_1000 )
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

    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel( onlyOnceOperations );


    // Layout the text for the new width.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | LAYOUT );

    // Store the actual control's width.
    const float actualControlWidth = mImpl->mModel->mVisualModel->mControlSize.width;

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
    mImpl->mModel->mVisualModel->mControlSize.width = actualControlWidth;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height );
  }
  else
  {
    layoutSize = mImpl->mModel->mVisualModel->GetLayoutSize();
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth cached %f\n", layoutSize.height );
  }

  return layoutSize.height;
}

const ModelInterface* const Controller::GetTextModel() const
{
  return mImpl->mModel.Get();
}

float Controller::GetScrollAmountByUserInput()
{
  float scrollAmount = 0.0f;

  if (NULL != mImpl->mEventData && mImpl->mEventData->mCheckScrollAmount)
  {
    scrollAmount = mImpl->mModel->mScrollPosition.y -  mImpl->mModel->mScrollPositionLast.y;
    mImpl->mEventData->mCheckScrollAmount = false;
  }
  return scrollAmount;
}

// public : Relayout.

Controller::UpdateTextType Controller::Relayout( const Size& size )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::Relayout %p size %f,%f, autoScroll[%s]\n", this, size.width, size.height, mImpl->mIsAutoScrollEnabled ?"true":"false"  );

  UpdateTextType updateTextType = NONE_UPDATED;

  if( ( size.width < Math::MACHINE_EPSILON_1000 ) || ( size.height < Math::MACHINE_EPSILON_1000 ) )
  {
    if( 0u != mImpl->mModel->mVisualModel->mGlyphPositions.Count() )
    {
      mImpl->mModel->mVisualModel->mGlyphPositions.Clear();
      updateTextType = MODEL_UPDATED;
    }

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // Not worth to relayout if width or height is equal to zero.
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::Relayout (skipped)\n" );

    return updateTextType;
  }

  // Whether a new size has been set.
  const bool newSize = ( size != mImpl->mModel->mVisualModel->mControlSize );

  if( newSize )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "new size (previous size %f,%f)\n", mImpl->mModel->mVisualModel->mControlSize.width, mImpl->mModel->mVisualModel->mControlSize.height );

    // Layout operations that need to be done if the size changes.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                             LAYOUT                    |
                                                             ALIGN                     |
                                                             UPDATE_LAYOUT_SIZE        |
                                                             REORDER );
    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
    mImpl->mTextUpdateInfo.mCharacterIndex = 0u;

    // Store the size used to layout the text.
    mImpl->mModel->mVisualModel->mControlSize = size;
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

  if( updated )
  {
    updateTextType = MODEL_UPDATED;
  }

  // Do not re-do any operation until something changes.
  mImpl->mOperationsPending = NO_OPERATION;
  mImpl->mModel->mScrollPositionLast = mImpl->mModel->mScrollPosition;

  // Whether the text control is editable
  const bool isEditable = NULL != mImpl->mEventData;

  // Keep the current offset as it will be used to update the decorator's positions (if the size changes).
  Vector2 offset;
  if( newSize && isEditable )
  {
    offset = mImpl->mModel->mScrollPosition;
  }

  if( !isEditable || !IsMultiLineEnabled() )
  {
    // After doing the text layout, the vertical offset to place the actor in the desired position can be calculated.
    CalculateVerticalOffset( size );
  }

  if( isEditable )
  {
    if( newSize )
    {
      // If there is a new size, the scroll position needs to be clamped.
      mImpl->ClampHorizontalScroll( layoutSize );

      // Update the decorator's positions is needed if there is a new size.
      mImpl->mEventData->mDecorator->UpdatePositions( mImpl->mModel->mScrollPosition - offset );
    }

    // Move the cursor, grab handle etc.
    if( mImpl->ProcessInputEvents() )
    {
      updateTextType = static_cast<UpdateTextType>( updateTextType | DECORATOR_UPDATED );
    }
  }

  // Clear the update info. This info will be set the next time the text is updated.
  mImpl->mTextUpdateInfo.Clear();
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::Relayout\n" );

  return updateTextType;
}

void Controller::RequestRelayout()
{
  mImpl->RequestRelayout();
}

// public : Input style change signals.

bool Controller::IsInputStyleChangedSignalsQueueEmpty()
{
  return ( NULL == mImpl->mEventData ) || ( 0u == mImpl->mEventData->mInputStyleChangedQueue.Count() );
}

void Controller::ProcessInputStyleChangedSignals()
{
  if( NULL == mImpl->mEventData )
  {
    // Nothing to do.
    return;
  }

  for( Vector<InputStyle::Mask>::ConstIterator it = mImpl->mEventData->mInputStyleChangedQueue.Begin(),
         endIt = mImpl->mEventData->mInputStyleChangedQueue.End();
       it != endIt;
       ++it )
  {
    const InputStyle::Mask mask = *it;

    if( NULL != mImpl->mEditableControlInterface )
    {
      // Emit the input style changed signal.
      mImpl->mEditableControlInterface->InputStyleChanged( mask );
    }
  }

  mImpl->mEventData->mInputStyleChangedQueue.Clear();
}

// public : Text-input Event Queuing.

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
      mImpl->mEventData->mUpdateInputStyle = true;
    }
    mImpl->NotifyImfMultiLineStatus();
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

    const bool isNullKey = ( 0 == keyCode ) && ( keyString.empty() );

    // Pre-process to separate modifying events from non-modifying input events.
    if( isNullKey )
    {
      // In some platforms arrive key events with no key code.
      // Do nothing.
    }
    else if( Dali::DALI_KEY_ESCAPE == keyCode )
    {
      // Escape key is a special case which causes focus loss
      KeyboardFocusLostEvent();
    }
    else if( ( Dali::DALI_KEY_CURSOR_LEFT  == keyCode ) ||
             ( Dali::DALI_KEY_CURSOR_RIGHT == keyCode ) ||
             ( Dali::DALI_KEY_CURSOR_UP    == keyCode ) ||
             ( Dali::DALI_KEY_CURSOR_DOWN  == keyCode ) )
    {
      // If don't have any text, do nothing.
      if( !mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters )
      {
        return false;
      }

      uint32_t cursorPosition = mImpl->mEventData->mPrimaryCursorPosition;
      uint32_t numberOfCharacters = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
      uint32_t cursorLine = mImpl->mModel->mVisualModel->GetLineOfCharacter( cursorPosition );
      uint32_t numberOfLines = mImpl->mModel->GetNumberOfLines();

      // Logic to determine whether this text control will lose focus or not.
      if( ( Dali::DALI_KEY_CURSOR_LEFT == keyCode && 0 == cursorPosition ) ||
          ( Dali::DALI_KEY_CURSOR_RIGHT == keyCode && numberOfCharacters == cursorPosition) ||
          ( Dali::DALI_KEY_CURSOR_DOWN == keyCode && cursorLine == numberOfLines -1 ) ||
          ( Dali::DALI_KEY_CURSOR_DOWN == keyCode && numberOfCharacters == cursorPosition && cursorLine -1 == numberOfLines -1 ) ||
          ( Dali::DALI_KEY_CURSOR_UP == keyCode && cursorLine == 0 ) ||
          ( Dali::DALI_KEY_CURSOR_UP == keyCode && numberOfCharacters == cursorPosition && cursorLine == 1 ) )
      {
        return false;
      }

      mImpl->mEventData->mCheckScrollAmount = true;
      Event event( Event::CURSOR_KEY_EVENT );
      event.p1.mInt = keyCode;
      mImpl->mEventData->mEventQueue.push_back( event );
    }
    else if( Dali::DALI_KEY_BACKSPACE == keyCode )
    {
      textChanged = BackspaceKeyEvent();
    }
    else if( IsKey( keyEvent, Dali::DALI_KEY_POWER ) ||
             IsKey( keyEvent, Dali::DALI_KEY_MENU ) ||
             IsKey( keyEvent, Dali::DALI_KEY_HOME ) )
    {
      // Power key/Menu/Home key behaviour does not allow edit mode to resume.
      mImpl->ChangeState( EventData::INACTIVE );

      // This branch avoids calling the InsertText() method of the 'else' branch which can delete selected text.
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

    if ( ( mImpl->mEventData->mState != EventData::INTERRUPTED ) &&
         ( mImpl->mEventData->mState != EventData::INACTIVE ) &&
         ( !isNullKey ) &&
         ( Dali::DALI_KEY_SHIFT_LEFT != keyCode ) )
    {
      // Should not change the state if the key is the shift send by the imf manager.
      // Otherwise, when the state is SELECTING the text controller can't send the right
      // surrounding info to the imf.
      mImpl->ChangeState( EventData::EDITING );
    }

    mImpl->RequestRelayout();
  }

  if( textChanged &&
      ( NULL != mImpl->mEditableControlInterface ) )
  {
    // Do this last since it provides callbacks into application code
    mImpl->mEditableControlInterface->TextChanged();
  }

  return true;
}

void Controller::TapEvent( unsigned int tapCount, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected TapEvent" );

  if( NULL != mImpl->mEventData )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "TapEvent state:%d \n", mImpl->mEventData->mState );
    EventData::State state( mImpl->mEventData->mState );
    bool relayoutNeeded( false );   // to avoid unnecessary relayouts when tapping an empty text-field

    if( mImpl->IsClipboardVisible() )
    {
      if( EventData::INACTIVE == state || EventData::EDITING == state)
      {
        mImpl->ChangeState( EventData::EDITING_WITH_GRAB_HANDLE );
      }
      relayoutNeeded = true;
    }
    else if( 1u == tapCount )
    {
      if( EventData::EDITING_WITH_POPUP == state || EventData::EDITING_WITH_PASTE_POPUP == state )
      {
        mImpl->ChangeState( EventData::EDITING_WITH_GRAB_HANDLE );  // If Popup shown hide it here so can be shown again if required.
      }

      if( mImpl->IsShowingRealText() && ( EventData::INACTIVE != state ) )
      {
        mImpl->ChangeState( EventData::EDITING_WITH_GRAB_HANDLE );
        relayoutNeeded = true;
      }
      else
      {
        if( mImpl->IsShowingPlaceholderText() && !mImpl->IsFocusedPlaceholderAvailable() )
        {
          // Hide placeholder text
          ResetText();
        }

        if( EventData::INACTIVE == state )
        {
          mImpl->ChangeState( EventData::EDITING );
        }
        else if( !mImpl->IsClipboardEmpty() )
        {
          mImpl->ChangeState( EventData::EDITING_WITH_POPUP );
        }
        relayoutNeeded = true;
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

  // Reset keyboard as tap event has occurred.
  mImpl->ResetImfManager();
}

void Controller::PanEvent( Gesture::State state, const Vector2& displacement )
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

ImfManager::ImfCallbackData Controller::OnImfEvent( ImfManager& imfManager, const ImfManager::ImfEventData& imfEvent )
{
  // Whether the text needs to be relaid-out.
  bool requestRelayout = false;

  // Whether to retrieve the text and cursor position to be sent to the IMF manager.
  bool retrieveText = false;
  bool retrieveCursor = false;

  switch( imfEvent.eventName )
  {
    case ImfManager::COMMIT:
    {
      InsertText( imfEvent.predictiveString, Text::Controller::COMMIT );
      requestRelayout = true;
      retrieveCursor = true;
      break;
    }
    case ImfManager::PREEDIT:
    {
      InsertText( imfEvent.predictiveString, Text::Controller::PRE_EDIT );
      requestRelayout = true;
      retrieveCursor = true;
      break;
    }
    case ImfManager::DELETESURROUNDING:
    {
      const bool textDeleted = RemoveText( imfEvent.cursorOffset,
                                           imfEvent.numberOfChars,
                                           DONT_UPDATE_INPUT_STYLE );

      if( textDeleted )
      {
        if( ( 0u != mImpl->mModel->mLogicalModel->mText.Count() ) ||
            !mImpl->IsPlaceholderAvailable() )
        {
          mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
        }
        else
        {
          ShowPlaceholderText();
        }
        mImpl->mEventData->mUpdateCursorPosition = true;
        mImpl->mEventData->mScrollAfterDelete = true;

        requestRelayout = true;
      }
      break;
    }
    case ImfManager::GETSURROUNDING:
    {
      retrieveText = true;
      retrieveCursor = true;
      break;
    }
    case ImfManager::VOID:
    {
      // do nothing
      break;
    }
  } // end switch

  if( requestRelayout )
  {
    mImpl->mOperationsPending = ALL_OPERATIONS;
    mImpl->RequestRelayout();
  }

  std::string text;
  CharacterIndex cursorPosition = 0u;
  Length numberOfWhiteSpaces = 0u;

  if( retrieveCursor )
  {
    numberOfWhiteSpaces = mImpl->GetNumberOfWhiteSpaces( 0u );

    cursorPosition = mImpl->GetLogicalCursorPosition();

    if( cursorPosition < numberOfWhiteSpaces )
    {
      cursorPosition = 0u;
    }
    else
    {
      cursorPosition -= numberOfWhiteSpaces;
    }
  }

  if( retrieveText )
  {
    mImpl->GetText( numberOfWhiteSpaces, text );
  }

  ImfManager::ImfCallbackData callbackData( ( retrieveText || retrieveCursor ), cursorPosition, text, false );

  if( requestRelayout &&
      ( NULL != mImpl->mEditableControlInterface ) )
  {
    // Do this last since it provides callbacks into application code
    mImpl->mEditableControlInterface->TextChanged();
  }

  return callbackData;
}

void Controller::PasteClipboardItemEvent()
{
  // Retrieve the clipboard contents first
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );
  std::string stringToPaste( notifier.GetContent() );

  // Commit the current pre-edit text; the contents of the clipboard should be appended
  mImpl->ResetImfManager();

  // Temporary disable hiding clipboard
  mImpl->SetClipboardHideEnable( false );

  // Paste
  PasteText( stringToPaste );

  mImpl->SetClipboardHideEnable( true );
}

// protected : Inherit from Text::Decorator::ControllerInterface.

void Controller::GetTargetSize( Vector2& targetSize )
{
  targetSize = mImpl->mModel->mVisualModel->mControlSize;
}

void Controller::AddDecoration( Actor& actor, bool needsClipping )
{
  if( NULL != mImpl->mEditableControlInterface )
  {
    mImpl->mEditableControlInterface->AddDecoration( actor, needsClipping );
  }
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

// protected : Inherit from TextSelectionPopup::TextPopupButtonCallbackInterface.

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

      if( ( 0u != mImpl->mModel->mLogicalModel->mText.Count() ) ||
          !mImpl->IsPlaceholderAvailable() )
      {
        mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
      }
      else
      {
        ShowPlaceholderText();
      }

      mImpl->mEventData->mUpdateCursorPosition = true;
      mImpl->mEventData->mScrollAfterDelete = true;

      mImpl->RequestRelayout();

      if( NULL != mImpl->mEditableControlInterface )
      {
        mImpl->mEditableControlInterface->TextChanged();
      }
      break;
    }
    case Toolkit::TextSelectionPopup::COPY:
    {
      mImpl->SendSelectionToClipboard( false ); // Text not modified

      mImpl->mEventData->mUpdateCursorPosition = true;

      mImpl->RequestRelayout(); // Cursor, Handles, Selection Highlight, Popup
      break;
    }
    case Toolkit::TextSelectionPopup::PASTE:
    {
      mImpl->RequestGetTextFromClipboard(); // Request clipboard service to retrieve an item
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

// private : Update.

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
  mImpl->mModel->mVisualModel->mUnderlineRuns.Clear();

  // Keep the current number of characters.
  const Length currentNumberOfCharacters = mImpl->IsShowingRealText() ? mImpl->mModel->mLogicalModel->mText.Count() : 0u;

  // Remove the previous IMF pre-edit.
  if( mImpl->mEventData->mPreEditFlag && ( 0u != mImpl->mEventData->mPreEditLength ) )
  {
    removedPrevious = RemoveText( -static_cast<int>( mImpl->mEventData->mPrimaryCursorPosition - mImpl->mEventData->mPreEditStartPosition ),
                                  mImpl->mEventData->mPreEditLength,
                                  DONT_UPDATE_INPUT_STYLE );

    mImpl->mEventData->mPrimaryCursorPosition = mImpl->mEventData->mPreEditStartPosition;
    mImpl->mEventData->mPreEditLength = 0u;
  }
  else
  {
    // Remove the previous Selection.
    removedPrevious = RemoveSelectedText();
  }

  Vector<Character> utf32Characters;
  Length characterCount = 0u;

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
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Entered PreEdit state\n" );

        // Record the start of the pre-edit text
        mImpl->mEventData->mPreEditStartPosition = mImpl->mEventData->mPrimaryCursorPosition;
      }

      mImpl->mEventData->mPreEditLength = utf32Characters.Count();
      mImpl->mEventData->mPreEditFlag = true;

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "mPreEditStartPosition %d mPreEditLength %d\n", mImpl->mEventData->mPreEditStartPosition, mImpl->mEventData->mPreEditLength );
    }

    const Length numberOfCharactersInModel = mImpl->mModel->mLogicalModel->mText.Count();

    // Restrict new text to fit within Maximum characters setting.
    Length maxSizeOfNewText = std::min( ( mImpl->mMaximumNumberOfCharacters - numberOfCharactersInModel ), characterCount );
    maxLengthReached = ( characterCount > maxSizeOfNewText );

    // The cursor position.
    CharacterIndex& cursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

    // Update the text's style.

    // Updates the text style runs by adding characters.
    mImpl->mModel->mLogicalModel->UpdateTextStyleRuns( cursorIndex, maxSizeOfNewText );

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = ( cursorIndex > 0u ) ? cursorIndex - 1u : 0u;

    // Retrieve the text's style for the given index.
    InputStyle style;
    mImpl->RetrieveDefaultInputStyle( style );
    mImpl->mModel->mLogicalModel->RetrieveStyle( styleIndex, style );

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
      const VectorBase::SizeType numberOfRuns = mImpl->mModel->mLogicalModel->mColorRuns.Count();
      mImpl->mModel->mLogicalModel->mColorRuns.Resize( numberOfRuns + 1u );

      ColorRun& colorRun = *( mImpl->mModel->mLogicalModel->mColorRuns.Begin() + numberOfRuns );
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
      const VectorBase::SizeType numberOfRuns = mImpl->mModel->mLogicalModel->mFontDescriptionRuns.Count();
      mImpl->mModel->mLogicalModel->mFontDescriptionRuns.Resize( numberOfRuns + 1u );

      FontDescriptionRun& fontDescriptionRun = *( mImpl->mModel->mLogicalModel->mFontDescriptionRuns.Begin() + numberOfRuns );

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
    Vector<Character>& modifyText = mImpl->mModel->mLogicalModel->mText;

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

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Inserted %d characters, new size %d new cursor %d\n", maxSizeOfNewText, mImpl->mModel->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition );
  }

  const Length numberOfCharacters = mImpl->IsShowingRealText() ? mImpl->mModel->mLogicalModel->mText.Count() : 0u;

  if( ( 0u == mImpl->mModel->mLogicalModel->mText.Count() ) &&
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

    mImpl->mEventData->mUpdateCursorPosition = true;
    if( numberOfCharacters < currentNumberOfCharacters )
    {
      mImpl->mEventData->mScrollAfterDelete = true;
    }
    else
    {
      mImpl->mEventData->mScrollAfterUpdatePosition = true;
    }
  }

  if( maxLengthReached )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "MaxLengthReached (%d)\n", mImpl->mModel->mLogicalModel->mText.Count() );

    mImpl->ResetImfManager();

    if( NULL != mImpl->mEditableControlInterface )
    {
      // Do this last since it provides callbacks into application code
      mImpl->mEditableControlInterface->MaxLengthReached();
    }
  }
}

void Controller::PasteText( const std::string& stringToPaste )
{
  InsertText( stringToPaste, Text::Controller::COMMIT );
  mImpl->ChangeState( EventData::EDITING );
  mImpl->RequestRelayout();

  if( NULL != mImpl->mEditableControlInterface )
  {
    // Do this last since it provides callbacks into application code
    mImpl->mEditableControlInterface->TextChanged();
  }
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
                 this, mImpl->mModel->mLogicalModel->mText.Count(), mImpl->mEventData->mPrimaryCursorPosition, cursorOffset, numberOfCharacters );

  if( !mImpl->IsShowingPlaceholderText() )
  {
    // Delete at current cursor position
    Vector<Character>& currentText = mImpl->mModel->mLogicalModel->mText;
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

    if( mImpl->mEventData->mPreEditFlag || // If the preedit flag is enabled, it means two (or more) of them came together i.e. when two keys have been pressed at the same time.
        ( ( cursorIndex + numberOfCharacters ) <= mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters ) )
    {
      // Mark the paragraphs to be updated.
      mImpl->mTextUpdateInfo.mCharacterIndex = std::min( cursorIndex, mImpl->mTextUpdateInfo.mCharacterIndex );
      mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove += numberOfCharacters;

      // Update the input style and remove the text's style before removing the text.

      if( UPDATE_INPUT_STYLE == type )
      {
        // Keep a copy of the current input style.
        InputStyle currentInputStyle;
        currentInputStyle.Copy( mImpl->mEventData->mInputStyle );

        // Set first the default input style.
        mImpl->RetrieveDefaultInputStyle( mImpl->mEventData->mInputStyle );

        // Update the input style.
        mImpl->mModel->mLogicalModel->RetrieveStyle( cursorIndex, mImpl->mEventData->mInputStyle );

        // Compare if the input style has changed.
        const bool hasInputStyleChanged = !currentInputStyle.Equal( mImpl->mEventData->mInputStyle );

        if( hasInputStyleChanged )
        {
          const InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask( mImpl->mEventData->mInputStyle );
          // Queue the input style changed signal.
          mImpl->mEventData->mInputStyleChangedQueue.PushBack( styleChangedMask );
        }
      }

      // Updates the text style runs by removing characters. Runs with no characters are removed.
      mImpl->mModel->mLogicalModel->UpdateTextStyleRuns( cursorIndex, -numberOfCharacters );

      // Remove the characters.
      Vector<Character>::Iterator first = currentText.Begin() + cursorIndex;
      Vector<Character>::Iterator last  = first + numberOfCharacters;

      currentText.Erase( first, last );

      // Cursor position retreat
      oldCursorIndex = cursorIndex;

      mImpl->mEventData->mScrollAfterDelete = true;

      DALI_LOG_INFO( gLogFilter, Debug::General, "Controller::RemoveText %p removed %d\n", this, numberOfCharacters );
      removed = true;
    }
  }

  return removed;
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

// private : Relayout.

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

  // Get the current layout size.
  layoutSize = mImpl->mModel->mVisualModel->GetLayoutSize();

  if( NO_OPERATION != ( LAYOUT & operations ) )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->Controller::DoRelayout LAYOUT & operations\n");

    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    // Calculate the number of glyphs to layout.
    const Vector<GlyphIndex>& charactersToGlyph = mImpl->mModel->mVisualModel->mCharactersToGlyph;
    const Vector<Length>& glyphsPerCharacter = mImpl->mModel->mVisualModel->mGlyphsPerCharacter;
    const GlyphIndex* const charactersToGlyphBuffer = charactersToGlyph.Begin();
    const Length* const glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

    const CharacterIndex lastIndex = startIndex + ( ( requestedNumberOfCharacters > 0u ) ? requestedNumberOfCharacters - 1u : 0u );
    const GlyphIndex startGlyphIndex = mImpl->mTextUpdateInfo.mStartGlyphIndex;
    const Length numberOfGlyphs = ( requestedNumberOfCharacters > 0u ) ? *( charactersToGlyphBuffer + lastIndex ) + *( glyphsPerCharacterBuffer + lastIndex ) - startGlyphIndex : 0u;
    const Length totalNumberOfGlyphs = mImpl->mModel->mVisualModel->mGlyphs.Count();

    if( 0u == totalNumberOfGlyphs )
    {
      if( NO_OPERATION != ( UPDATE_LAYOUT_SIZE & operations ) )
      {
        mImpl->mModel->mVisualModel->SetLayoutSize( Size::ZERO );
      }

      // Nothing else to do if there is no glyphs.
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::DoRelayout no glyphs, view updated true\n" );
      return true;
    }

    const Vector<LineBreakInfo>& lineBreakInfo = mImpl->mModel->mLogicalModel->mLineBreakInfo;
    const Vector<WordBreakInfo>& wordBreakInfo = mImpl->mModel->mLogicalModel->mWordBreakInfo;
    const Vector<CharacterDirection>& characterDirection = mImpl->mModel->mLogicalModel->mCharacterDirections;
    const Vector<GlyphInfo>& glyphs = mImpl->mModel->mVisualModel->mGlyphs;
    const Vector<CharacterIndex>& glyphsToCharactersMap = mImpl->mModel->mVisualModel->mGlyphsToCharacters;
    const Vector<Length>& charactersPerGlyph = mImpl->mModel->mVisualModel->mCharactersPerGlyph;
    const Character* const textBuffer = mImpl->mModel->mLogicalModel->mText.Begin();

    // Set the layout parameters.
    Layout::Parameters layoutParameters( size,
                                         textBuffer,
                                         lineBreakInfo.Begin(),
                                         wordBreakInfo.Begin(),
                                         ( 0u != characterDirection.Count() ) ? characterDirection.Begin() : NULL,
                                         glyphs.Begin(),
                                         glyphsToCharactersMap.Begin(),
                                         charactersPerGlyph.Begin(),
                                         charactersToGlyphBuffer,
                                         glyphsPerCharacterBuffer,
                                         totalNumberOfGlyphs,
                                         mImpl->mModel->mHorizontalAlignment );

    // Resize the vector of positions to have the same size than the vector of glyphs.
    Vector<Vector2>& glyphPositions = mImpl->mModel->mVisualModel->mGlyphPositions;
    glyphPositions.Resize( totalNumberOfGlyphs );

    // Whether the last character is a new paragraph character.
    mImpl->mTextUpdateInfo.mIsLastCharacterNewParagraph =  TextAbstraction::IsNewParagraph( *( textBuffer + ( mImpl->mModel->mLogicalModel->mText.Count() - 1u ) ) );
    layoutParameters.isLastNewParagraph = mImpl->mTextUpdateInfo.mIsLastCharacterNewParagraph;

    // The initial glyph and the number of glyphs to layout.
    layoutParameters.startGlyphIndex = startGlyphIndex;
    layoutParameters.numberOfGlyphs = numberOfGlyphs;
    layoutParameters.startLineIndex = mImpl->mTextUpdateInfo.mStartLineIndex;
    layoutParameters.estimatedNumberOfLines = mImpl->mTextUpdateInfo.mEstimatedNumberOfLines;

    // Update the visual model.
    Size newLayoutSize;
    viewUpdated = mImpl->mLayoutEngine.LayoutText( layoutParameters,
                                                   glyphPositions,
                                                   mImpl->mModel->mVisualModel->mLines,
                                                   newLayoutSize,
                                                   mImpl->mModel->mElideEnabled );

    viewUpdated = viewUpdated || ( newLayoutSize != layoutSize );

    if( viewUpdated )
    {
      layoutSize = newLayoutSize;

      if( NO_OPERATION != ( UPDATE_DIRECTION & operations ) )
      {
        mImpl->mAutoScrollDirectionRTL = false;
      }

      // Reorder the lines
      if( NO_OPERATION != ( REORDER & operations ) )
      {
        Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mImpl->mModel->mLogicalModel->mBidirectionalParagraphInfo;
        Vector<BidirectionalLineInfoRun>& bidirectionalLineInfo = mImpl->mModel->mLogicalModel->mBidirectionalLineInfo;

        // Check first if there are paragraphs with bidirectional info.
        if( 0u != bidirectionalInfo.Count() )
        {
          // Get the lines
          const Length numberOfLines = mImpl->mModel->mVisualModel->mLines.Count();

          // Reorder the lines.
          bidirectionalLineInfo.Reserve( numberOfLines ); // Reserve because is not known yet how many lines have right to left characters.
          ReorderLines( bidirectionalInfo,
                        startIndex,
                        requestedNumberOfCharacters,
                        mImpl->mModel->mVisualModel->mLines,
                        bidirectionalLineInfo );

          // Set the bidirectional info per line into the layout parameters.
          layoutParameters.lineBidirectionalInfoRunsBuffer = bidirectionalLineInfo.Begin();
          layoutParameters.numberOfBidirectionalInfoRuns = bidirectionalLineInfo.Count();

          // Re-layout the text. Reorder those lines with right to left characters.
          mImpl->mLayoutEngine.ReLayoutRightToLeftLines( layoutParameters,
                                                         startIndex,
                                                         requestedNumberOfCharacters,
                                                         glyphPositions );

          if ( ( NO_OPERATION != ( UPDATE_DIRECTION & operations ) ) && ( numberOfLines > 0 ) )
          {
            const LineRun* const firstline = mImpl->mModel->mVisualModel->mLines.Begin();
            if ( firstline )
            {
              mImpl->mAutoScrollDirectionRTL = firstline->direction;
            }
          }
        }
      } // REORDER

      // Sets the layout size.
      if( NO_OPERATION != ( UPDATE_LAYOUT_SIZE & operations ) )
      {
        mImpl->mModel->mVisualModel->SetLayoutSize( layoutSize );
      }
    } // view updated
  }

  if( NO_OPERATION != ( ALIGN & operations ) )
  {
    // The laid-out lines.
    Vector<LineRun>& lines = mImpl->mModel->mVisualModel->mLines;

    // Need to align with the control's size as the text may contain lines
    // starting either with left to right text or right to left.
    mImpl->mLayoutEngine.Align( size,
                                startIndex,
                                requestedNumberOfCharacters,
                                mImpl->mModel->mHorizontalAlignment,
                                lines,
                                mImpl->mModel->mAlignmentOffset );

    viewUpdated = true;
  }
#if defined(DEBUG_ENABLED)
  std::string currentText;
  GetText( currentText );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Controller::DoRelayout [%p] mImpl->mAutoScrollDirectionRTL[%s] [%s]\n", this, (mImpl->mAutoScrollDirectionRTL)?"true":"false",  currentText.c_str() );
#endif
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--Controller::DoRelayout, view updated %s\n", ( viewUpdated ? "true" : "false" ) );
  return viewUpdated;
}

void Controller::CalculateVerticalOffset( const Size& controlSize )
{
  Size layoutSize = mImpl->mModel->mVisualModel->GetLayoutSize();

  if( fabsf( layoutSize.height ) < Math::MACHINE_EPSILON_1000 )
  {
    // Get the line height of the default font.
    layoutSize.height = mImpl->GetDefaultFontLineHeight();
  }

  switch( mImpl->mModel->mVerticalAlignment )
  {
    case Layout::VERTICAL_ALIGN_TOP:
    {
      mImpl->mModel->mScrollPosition.y = 0.f;
      break;
    }
    case Layout::VERTICAL_ALIGN_CENTER:
    {
      mImpl->mModel->mScrollPosition.y = floorf( 0.5f * ( controlSize.height - layoutSize.height ) ); // try to avoid pixel alignment.
      break;
    }
    case Layout::VERTICAL_ALIGN_BOTTOM:
    {
      mImpl->mModel->mScrollPosition.y = controlSize.height - layoutSize.height;
      break;
    }
  }
}

// private : Events.

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

  mImpl->mEventData->mCheckScrollAmount = true;

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::TextDeletedEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected TextDeletedEvent" );

  if( NULL == mImpl->mEventData )
  {
    return;
  }

  mImpl->mEventData->mCheckScrollAmount = true;

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;
}

void Controller::SelectEvent( float x, float y, bool selectAll )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Controller::SelectEvent\n" );

  if( NULL != mImpl->mEventData )
  {
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

    mImpl->mEventData->mCheckScrollAmount = true;
    mImpl->RequestRelayout();
  }
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
    if( ( 0u != mImpl->mModel->mLogicalModel->mText.Count() ) ||
        !mImpl->IsPlaceholderAvailable() )
    {
      mImpl->QueueModifyEvent( ModifyEvent::TEXT_DELETED );
    }
    else
    {
      ShowPlaceholderText();
    }
    mImpl->mEventData->mUpdateCursorPosition = true;
    mImpl->mEventData->mScrollAfterDelete = true;
  }

  return removed;
}

// private : Helpers.

void Controller::ResetText()
{
  // Reset buffers.
  mImpl->mModel->mLogicalModel->mText.Clear();

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
    mImpl->mModel->mLogicalModel->mText.Clear();
    mImpl->mModel->mVisualModel->SetTextColor( mImpl->mEventData->mPlaceholderTextColor );

    // Convert text into UTF-32
    Vector<Character>& utf32Characters = mImpl->mModel->mLogicalModel->mText;
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

  // Set flags to update the model.
  mImpl->mTextUpdateInfo.mCharacterIndex = 0u;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
  mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mModel->mLogicalModel->mText.Count();

  mImpl->mTextUpdateInfo.mClearAll = true;
  mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
  mImpl->mRecalculateNaturalSize = true;

  mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                           VALIDATE_FONTS            |
                                                           SHAPE_TEXT                |
                                                           GET_GLYPH_METRICS         |
                                                           LAYOUT                    |
                                                           UPDATE_LAYOUT_SIZE        |
                                                           REORDER                   |
                                                           ALIGN );
}

void Controller::ClearStyleData()
{
  mImpl->mModel->mLogicalModel->mColorRuns.Clear();
  mImpl->mModel->mLogicalModel->ClearFontDescriptionRuns();
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
    mImpl->mModel->mScrollPosition = Vector2::ZERO;
    mImpl->mEventData->mScrollAfterUpdatePosition = true;
  }
}

// private : Private contructors & copy operator.

Controller::Controller()
: mImpl( NULL )
{
  mImpl = new Controller::Impl( NULL, NULL );
}

Controller::Controller( ControlInterface* controlInterface )
{
  mImpl = new Controller::Impl( controlInterface, NULL );
}

Controller::Controller( ControlInterface* controlInterface,
                        EditableControlInterface* editableControlInterface )
{
  mImpl = new Controller::Impl( controlInterface,
                                editableControlInterface );
}

// The copy constructor and operator are left unimplemented.

// protected : Destructor.

Controller::~Controller()
{
  delete mImpl;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
