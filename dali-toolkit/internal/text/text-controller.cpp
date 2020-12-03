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
#include <dali-toolkit/internal/text/text-controller.h>

// EXTERNAL INCLUDES
#include <limits>
#include <cmath>
#include <memory.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller-event-handler.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-controller-input-font-handler.h>
#include <dali-toolkit/internal/text/text-controller-placeholder-handler.h>
#include <dali-toolkit/internal/text/text-controller-relayouter.h>
#include <dali-toolkit/internal/text/text-controller-text-updater.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

const std::string EMPTY_STRING("");

int ConvertPixelToPint( float pixel )
{
  unsigned int horizontalDpi = 0u;
  unsigned int verticalDpi = 0u;
  Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
  fontClient.GetDpi( horizontalDpi, verticalDpi );

  return ( pixel * 72.f ) / static_cast< float >( horizontalDpi );
}

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

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
                               EditableControlInterface* editableControlInterface,
                               SelectableControlInterface* selectableControlInterface )
{
  return ControllerPtr( new Controller( controlInterface,
                                        editableControlInterface,
                                        selectableControlInterface ) );
}

// public : Configure the text controller.

void Controller::EnableTextInput( DecoratorPtr decorator, InputMethodContext& inputMethodContext )
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
    mImpl->mEventData = new EventData( decorator, inputMethodContext );
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
  if( enable != mImpl->mMarkupProcessorEnabled )
  {
    //If Text was already set, call the SetText again for enabling or disabling markup
    mImpl->mMarkupProcessorEnabled = enable;
    std::string text;
    GetText( text );
    SetText( text );
  }
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
  return mImpl->mIsTextDirectionRTL;
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

    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | layoutOperations );

    // Need to recalculate natural size
    mImpl->mRecalculateNaturalSize = true;

    mImpl->RequestRelayout();
  }
}

bool Controller::IsMultiLineEnabled() const
{
  return Layout::Engine::MULTI_LINE_BOX == mImpl->mLayoutEngine.GetLayout();
}

void Controller::SetHorizontalAlignment( Text::HorizontalAlignment::Type alignment )
{
  if( alignment != mImpl->mModel->mHorizontalAlignment )
  {
    // Set the alignment.
    mImpl->mModel->mHorizontalAlignment = alignment;

    // Set the flag to redo the alignment operation.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | ALIGN );

    if( mImpl->mEventData )
    {
      mImpl->mEventData->mUpdateAlignment = true;

      // Update the cursor if it's in editing mode
      if( EventData::IsEditingState( mImpl->mEventData->mState ) )
      {
        mImpl->ChangeState( EventData::EDITING );
        mImpl->mEventData->mUpdateCursorPosition = true;
      }
    }

    mImpl->RequestRelayout();
  }
}

Text::HorizontalAlignment::Type Controller::GetHorizontalAlignment() const
{
  return mImpl->mModel->mHorizontalAlignment;
}

void Controller::SetVerticalAlignment( VerticalAlignment::Type alignment )
{
  if( alignment != mImpl->mModel->mVerticalAlignment )
  {
    // Set the alignment.
    mImpl->mModel->mVerticalAlignment = alignment;

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | ALIGN );

    mImpl->RequestRelayout();
  }
}

VerticalAlignment::Type Controller::GetVerticalAlignment() const
{
  return mImpl->mModel->mVerticalAlignment;
}

bool Controller::IsIgnoreSpacesAfterText() const
{
  return mImpl->mModel->mIgnoreSpacesAfterText;
}

void Controller::SetIgnoreSpacesAfterText( bool ignore )
{
  mImpl->mModel->mIgnoreSpacesAfterText = ignore;
}

bool Controller::IsMatchSystemLanguageDirection() const
{
  return mImpl->mModel->mMatchSystemLanguageDirection;
}

void Controller::SetMatchSystemLanguageDirection( bool match )
{
  mImpl->mModel->mMatchSystemLanguageDirection = match;
}

void Controller::SetLayoutDirection( Dali::LayoutDirection::Type layoutDirection )
{
  mImpl->mLayoutDirection = layoutDirection;
}

bool Controller::IsShowingRealText() const
{
  return mImpl->IsShowingRealText();
}


void Controller::SetLineWrapMode( Text::LineWrap::Mode lineWrapMode )
{
  if( lineWrapMode != mImpl->mModel->mLineWrapMode )
  {
    // Set the text wrap mode.
    mImpl->mModel->mLineWrapMode = lineWrapMode;


    // Update Text layout for applying wrap mode
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending |
                                                             ALIGN                     |
                                                             LAYOUT                    |
                                                             UPDATE_LAYOUT_SIZE        |
                                                             REORDER                   );
    mImpl->mTextUpdateInfo.mCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mNumberOfCharactersToRemove = mImpl->mTextUpdateInfo.mPreviousNumberOfCharacters;
    mImpl->mTextUpdateInfo.mNumberOfCharactersToAdd = mImpl->mModel->mLogicalModel->mText.Count();

    // Request relayout
    mImpl->RequestRelayout();
  }
}

Text::LineWrap::Mode Controller::GetLineWrapMode() const
{
  return mImpl->mModel->mLineWrapMode;
}

void Controller::SetTextElideEnabled( bool enabled )
{
  mImpl->mModel->mElideEnabled = enabled;
}

bool Controller::IsTextElideEnabled() const
{
  return mImpl->mModel->mElideEnabled;
}

void Controller::SetTextFitEnabled(bool enabled)
{
  mImpl->mTextFitEnabled = enabled;
}

bool Controller::IsTextFitEnabled() const
{
  return mImpl->mTextFitEnabled;
}

void Controller::SetTextFitMinSize( float minSize, FontSizeType type )
{
  switch( type )
  {
    case POINT_SIZE:
    {
      mImpl->mTextFitMinSize = minSize;
      break;
    }
    case PIXEL_SIZE:
    {
      mImpl->mTextFitMinSize = ConvertPixelToPint( minSize );
      break;
    }
  }
}

float Controller::GetTextFitMinSize() const
{
  return mImpl->mTextFitMinSize;
}

void Controller::SetTextFitMaxSize( float maxSize, FontSizeType type )
{
  switch( type )
  {
    case POINT_SIZE:
    {
      mImpl->mTextFitMaxSize = maxSize;
      break;
    }
    case PIXEL_SIZE:
    {
      mImpl->mTextFitMaxSize = ConvertPixelToPint( maxSize );
      break;
    }
  }
}

float Controller::GetTextFitMaxSize() const
{
  return mImpl->mTextFitMaxSize;
}

void Controller::SetTextFitStepSize( float step, FontSizeType type )
{
  switch( type )
  {
    case POINT_SIZE:
    {
      mImpl->mTextFitStepSize = step;
      break;
    }
    case PIXEL_SIZE:
    {
      mImpl->mTextFitStepSize = ConvertPixelToPint( step );
      break;
    }
  }
}

float Controller::GetTextFitStepSize() const
{
  return mImpl->mTextFitStepSize;
}

void Controller::SetTextFitContentSize(Vector2 size)
{
  mImpl->mTextFitContentSize = size;
}

Vector2 Controller::GetTextFitContentSize() const
{
  return mImpl->mTextFitContentSize;
}

void Controller::SetPlaceholderTextElideEnabled( bool enabled )
{
  PlaceholderHandler::SetPlaceholderTextElideEnabled(*this, enabled);
}

bool Controller::IsPlaceholderTextElideEnabled() const
{
  return PlaceholderHandler::IsPlaceholderTextElideEnabled(*this);
}

void Controller::SetSelectionEnabled( bool enabled )
{
  mImpl->mEventData->mSelectionEnabled = enabled;
}

bool Controller::IsSelectionEnabled() const
{
  return mImpl->mEventData->mSelectionEnabled;
}

void Controller::SetShiftSelectionEnabled( bool enabled )
{
  mImpl->mEventData->mShiftSelectionFlag = enabled;
}

bool Controller::IsShiftSelectionEnabled() const
{
  return mImpl->mEventData->mShiftSelectionFlag;
}

void Controller::SetGrabHandleEnabled( bool enabled )
{
  mImpl->mEventData->mGrabHandleEnabled = enabled;
}

bool Controller::IsGrabHandleEnabled() const
{
  return mImpl->mEventData->mGrabHandleEnabled;
}

void Controller::SetGrabHandlePopupEnabled(bool enabled)
{
  mImpl->mEventData->mGrabHandlePopupEnabled = enabled;
}

bool Controller::IsGrabHandlePopupEnabled() const
{
  return mImpl->mEventData->mGrabHandlePopupEnabled;
}

// public : Update

void Controller::SetText( const std::string& text )
{
  TextUpdater::SetText(*this, text);
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
  PlaceholderHandler::SetPlaceholderText(*this, type, text);
}

void Controller::GetPlaceholderText( PlaceholderType type, std::string& text ) const
{
  PlaceholderHandler::GetPlaceholderText(*this, type, text );
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

void Controller::RetrieveSelection( std::string& selectedText ) const
{
  mImpl->RetrieveSelection( selectedText, false );
}

void Controller::SetSelection( int start, int end )
{
  mImpl->SetSelection( start, end );
}

std::pair< int, int > Controller::GetSelectionIndexes() const
{
  return mImpl->GetSelectionIndexes();
}

void Controller::CopyStringToClipboard( const std::string& source )
{
  mImpl->CopyStringToClipboard( source );
}

void Controller::SendSelectionToClipboard( bool deleteAfterSending )
{
  mImpl->SendSelectionToClipboard( deleteAfterSending );
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

  if( mImpl->mEventData )
  {
    // Update the cursor position if it's in editing mode
    if( EventData::IsEditingState( mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mDecoratorUpdated = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font family is updated.
    }
  }

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

void Controller::SetPlaceholderFontFamily( const std::string& placeholderTextFontFamily )
{
  PlaceholderHandler::SetPlaceholderFontFamily(*this, placeholderTextFontFamily);
}

const std::string& Controller::GetPlaceholderFontFamily() const
{
  return PlaceholderHandler::GetPlaceholderFontFamily(*this);
}

void Controller::SetDefaultFontWeight( FontWeight weight )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.weight = weight;
  mImpl->mFontDefaults->weightDefined = true;

  if( mImpl->mEventData )
  {
    // Update the cursor position if it's in editing mode
    if( EventData::IsEditingState( mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mDecoratorUpdated = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font weight is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWeightDefined() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->weightDefined;
  }

  return false;
}

FontWeight Controller::GetDefaultFontWeight() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.weight;
  }

  return TextAbstraction::FontWeight::NORMAL;
}

void Controller::SetPlaceholderTextFontWeight( FontWeight weight )
{
  PlaceholderHandler::SetPlaceholderTextFontWeight(*this, weight);
}

bool Controller::IsPlaceholderTextFontWeightDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWeightDefined(*this);;
}

FontWeight Controller::GetPlaceholderTextFontWeight() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWeight(*this);
}

void Controller::SetDefaultFontWidth( FontWidth width )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.width = width;
  mImpl->mFontDefaults->widthDefined = true;

  if( mImpl->mEventData )
  {
    // Update the cursor position if it's in editing mode
    if( EventData::IsEditingState( mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mDecoratorUpdated = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font width is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontWidthDefined() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->widthDefined;
  }

  return false;
}

FontWidth Controller::GetDefaultFontWidth() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.width;
  }

  return TextAbstraction::FontWidth::NORMAL;
}

void Controller::SetPlaceholderTextFontWidth( FontWidth width )
{
  PlaceholderHandler::SetPlaceholderTextFontWidth(*this, width);
}

bool Controller::IsPlaceholderTextFontWidthDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontWidthDefined(*this);
}

FontWidth Controller::GetPlaceholderTextFontWidth() const
{
  return PlaceholderHandler::GetPlaceholderTextFontWidth(*this);
}

void Controller::SetDefaultFontSlant( FontSlant slant )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  mImpl->mFontDefaults->mFontDescription.slant = slant;
  mImpl->mFontDefaults->slantDefined = true;

  if( mImpl->mEventData )
  {
    // Update the cursor position if it's in editing mode
    if( EventData::IsEditingState( mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mDecoratorUpdated = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font slant is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

bool Controller::IsDefaultFontSlantDefined() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->slantDefined;
  }
  return false;
}

FontSlant Controller::GetDefaultFontSlant() const
{
  if( NULL != mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mFontDescription.slant;
  }

  return TextAbstraction::FontSlant::NORMAL;
}

void Controller::SetPlaceholderTextFontSlant( FontSlant slant )
{
  PlaceholderHandler::SetPlaceholderTextFontSlant(*this, slant);
}

bool Controller::IsPlaceholderTextFontSlantDefined() const
{
  return PlaceholderHandler::IsPlaceholderTextFontSlantDefined(*this);
}

FontSlant Controller::GetPlaceholderTextFontSlant() const
{
  return PlaceholderHandler::GetPlaceholderTextFontSlant(*this);
}

void Controller::SetFontSizeScale( float scale )
{
  mImpl->mFontSizeScale = scale;

  if( mImpl->mEventData )
  {
    // Update the cursor position if it's in editing mode
    if( EventData::IsEditingState( mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mDecoratorUpdated = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font size is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

float Controller::GetFontSizeScale() const
{
  if( nullptr != mImpl->mFontDefaults )
  {
    return mImpl->mFontSizeScale;
  }

  return 1.f;
}

void Controller::SetDefaultFontSize( float fontSize, FontSizeType type )
{
  if( NULL == mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new FontDefaults();
  }

  switch( type )
  {
    case POINT_SIZE:
    {
      mImpl->mFontDefaults->mDefaultPointSize = fontSize;
      mImpl->mFontDefaults->sizeDefined = true;
      break;
    }
    case PIXEL_SIZE:
    {
      // Point size = Pixel size * 72.f / DPI
      unsigned int horizontalDpi = 0u;
      unsigned int verticalDpi = 0u;
      TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
      fontClient.GetDpi( horizontalDpi, verticalDpi );

      mImpl->mFontDefaults->mDefaultPointSize = ( fontSize * 72.f ) / static_cast< float >( horizontalDpi );
      mImpl->mFontDefaults->sizeDefined = true;
      break;
    }
  }

  if( mImpl->mEventData )
  {
    // Update the cursor position if it's in editing mode
    if( EventData::IsEditingState( mImpl->mEventData->mState ) )
    {
      mImpl->mEventData->mDecoratorUpdated = true;
      mImpl->mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font size is updated.
    }
  }

  // Clear the font-specific data
  ClearFontData();

  mImpl->RequestRelayout();
}

float Controller::GetDefaultFontSize( FontSizeType type ) const
{
  float value = 0.0f;
  if( NULL != mImpl->mFontDefaults )
  {
    switch( type )
    {
      case POINT_SIZE:
      {
        value = mImpl->mFontDefaults->mDefaultPointSize;
        break;
      }
      case PIXEL_SIZE:
      {
        // Pixel size = Point size * DPI / 72.f
        unsigned int horizontalDpi = 0u;
        unsigned int verticalDpi = 0u;
        TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
        fontClient.GetDpi( horizontalDpi, verticalDpi );

        value = mImpl->mFontDefaults->mDefaultPointSize * static_cast< float >( horizontalDpi ) / 72.f;
        break;
      }
    }
    return value;
  }

  return value;
}

void Controller::SetPlaceholderTextFontSize( float fontSize, FontSizeType type )
{
  PlaceholderHandler::SetPlaceholderTextFontSize(*this, fontSize, type);
}

float Controller::GetPlaceholderTextFontSize( FontSizeType type ) const
{
  return PlaceholderHandler::GetPlaceholderTextFontSize(*this, type);
}

void Controller::SetDefaultColor( const Vector4& color )
{
  mImpl->mTextColor = color;

  if( !mImpl->IsShowingPlaceholderText() )
  {
    mImpl->mModel->mVisualModel->SetTextColor( color );

    mImpl->mModel->mLogicalModel->mColorRuns.Clear();

    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending | COLOR );

    mImpl->RequestRelayout();
  }
}

const Vector4& Controller::GetDefaultColor() const
{
  return mImpl->mTextColor;
}

void Controller::SetPlaceholderTextColor( const Vector4& textColor )
{
  PlaceholderHandler::SetPlaceholderTextColor(*this, textColor);
}

const Vector4& Controller::GetPlaceholderTextColor() const
{
  return PlaceholderHandler::GetPlaceholderTextColor(*this);
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

void Controller::SetShadowBlurRadius( const float& shadowBlurRadius )
{
  if ( fabsf( GetShadowBlurRadius() - shadowBlurRadius ) > Math::MACHINE_EPSILON_1 )
  {
    mImpl->mModel->mVisualModel->SetShadowBlurRadius( shadowBlurRadius );

    mImpl->RequestRelayout();
  }
}

const float& Controller::GetShadowBlurRadius() const
{
  return mImpl->mModel->mVisualModel->GetShadowBlurRadius();
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

void Controller::SetOutlineColor( const Vector4& color )
{
  mImpl->mModel->mVisualModel->SetOutlineColor( color );

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetOutlineColor() const
{
  return mImpl->mModel->mVisualModel->GetOutlineColor();
}

void Controller::SetOutlineWidth( uint16_t width )
{
  mImpl->mModel->mVisualModel->SetOutlineWidth( width );

  mImpl->RequestRelayout();
}

uint16_t Controller::GetOutlineWidth() const
{
  return mImpl->mModel->mVisualModel->GetOutlineWidth();
}

void Controller::SetBackgroundColor( const Vector4& color )
{
  mImpl->mModel->mVisualModel->SetBackgroundColor( color );

  mImpl->RequestRelayout();
}

const Vector4& Controller::GetBackgroundColor() const
{
  return mImpl->mModel->mVisualModel->GetBackgroundColor();
}

void Controller::SetBackgroundEnabled( bool enabled )
{
  mImpl->mModel->mVisualModel->SetBackgroundEnabled( enabled );

  mImpl->RequestRelayout();
}

bool Controller::IsBackgroundEnabled() const
{
  return mImpl->mModel->mVisualModel->IsBackgroundEnabled();
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

bool Controller::SetDefaultLineSpacing( float lineSpacing )
{
  if( std::fabs( lineSpacing - mImpl->mLayoutEngine.GetDefaultLineSpacing() ) > Math::MACHINE_EPSILON_1000 )
  {
    mImpl->mLayoutEngine.SetDefaultLineSpacing(lineSpacing);
    mImpl->mRecalculateNaturalSize = true;
    return true;
  }
  return false;
}

float Controller::GetDefaultLineSpacing() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSpacing();
}

bool Controller::SetDefaultLineSize( float lineSize )
{
  if( std::fabs( lineSize - mImpl->mLayoutEngine.GetDefaultLineSize() ) > Math::MACHINE_EPSILON_1000 )
  {
    mImpl->mLayoutEngine.SetDefaultLineSize(lineSize);
    mImpl->mRecalculateNaturalSize = true;
    return true;
  }
  return false;
}

float Controller::GetDefaultLineSize() const
{
  return mImpl->mLayoutEngine.GetDefaultLineSize();
}

void Controller::SetInputColor( const Vector4& color )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mInputStyle.textColor = color;
    mImpl->mEventData->mInputStyle.isDefaultColor = false;

    if( EventData::SELECTING == mImpl->mEventData->mState || EventData::EDITING == mImpl->mEventData->mState || EventData::INACTIVE == mImpl->mEventData->mState )
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
  InputFontHandler::SetInputFontFamily(*this, fontFamily);
}

const std::string& Controller::GetInputFontFamily() const
{
  return InputFontHandler::GetInputFontFamily(*this);
}

void Controller::SetInputFontWeight( FontWeight weight )
{
  InputFontHandler::SetInputFontWeight(*this, weight);
}

bool Controller::IsInputFontWeightDefined() const
{
  return InputFontHandler::IsInputFontWeightDefined(*this);
}

FontWeight Controller::GetInputFontWeight() const
{
  return InputFontHandler::GetInputFontWeight(*this);
}

void Controller::SetInputFontWidth( FontWidth width )
{
  InputFontHandler::SetInputFontWidth(*this, width);
}

bool Controller::IsInputFontWidthDefined() const
{
  return InputFontHandler::IsInputFontWidthDefined(*this);
}

FontWidth Controller::GetInputFontWidth() const
{
  return InputFontHandler::GetInputFontWidth(*this);
}

void Controller::SetInputFontSlant( FontSlant slant )
{
  InputFontHandler::SetInputFontSlant(*this, slant);
}

bool Controller::IsInputFontSlantDefined() const
{
  return InputFontHandler::IsInputFontSlantDefined(*this);
}

FontSlant Controller::GetInputFontSlant() const
{
  return InputFontHandler::GetInputFontSlant(*this);
}

void Controller::SetInputFontPointSize( float size )
{
  InputFontHandler::SetInputFontPointSize(*this, size);
}

float Controller::GetInputFontPointSize() const
{
  return InputFontHandler::GetInputFontPointSize(*this);
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

void Controller::SetNoTextDoubleTapAction( NoTextTap::Action action )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mDoubleTapAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextDoubleTapAction() const
{
  NoTextTap::Action action = NoTextTap::NO_ACTION;

  if( NULL != mImpl->mEventData )
  {
    action = mImpl->mEventData->mDoubleTapAction;
  }

  return action;
}

void Controller::SetNoTextLongPressAction( NoTextTap::Action action )
{
  if( NULL != mImpl->mEventData )
  {
    mImpl->mEventData->mLongPressAction = action;
  }
}

Controller::NoTextTap::Action Controller::GetNoTextLongPressAction() const
{
  NoTextTap::Action action = NoTextTap::NO_ACTION;

  if( NULL != mImpl->mEventData )
  {
    action = mImpl->mEventData->mLongPressAction;
  }

  return action;
}

bool Controller::IsUnderlineSetByString()
{
  return mImpl->mUnderlineSetByString;
}

void Controller::UnderlineSetByString( bool setByString )
{
  mImpl->mUnderlineSetByString = setByString;
}

bool Controller::IsShadowSetByString()
{
  return mImpl->mShadowSetByString;
}

void Controller::ShadowSetByString( bool setByString )
{
  mImpl->mShadowSetByString = setByString;
}

bool Controller::IsOutlineSetByString()
{
  return mImpl->mOutlineSetByString;
}

void Controller::OutlineSetByString( bool setByString )
{
  mImpl->mOutlineSetByString = setByString;
}

bool Controller::IsFontStyleSetByString()
{
  return mImpl->mFontStyleSetByString;
}

void Controller::FontStyleSetByString( bool setByString )
{
  mImpl->mFontStyleSetByString = setByString;
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
  return Relayouter::GetNaturalSize(*this);
}

bool Controller::CheckForTextFit( float pointSize, Size& layoutSize )
{
  return Relayouter::CheckForTextFit(*this, pointSize, layoutSize);
}

void Controller::FitPointSizeforLayout( Size layoutSize )
{
  Relayouter::FitPointSizeforLayout(*this, layoutSize);
}

float Controller::GetHeightForWidth( float width )
{
  return Relayouter::GetHeightForWidth(*this, width);
}

int Controller::GetLineCount( float width )
{
  GetHeightForWidth( width );
  int numberofLines = mImpl->mModel->GetNumberOfLines();
  return numberofLines;
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

bool Controller::GetTextScrollInfo( float& scrollPosition, float& controlHeight, float& layoutHeight )
{
  const Vector2& layout = mImpl->mModel->mVisualModel->GetLayoutSize();
  bool isScrolled;

  controlHeight = mImpl->mModel->mVisualModel->mControlSize.height;
  layoutHeight = layout.height;
  scrollPosition = mImpl->mModel->mScrollPosition.y;
  isScrolled = !Equals( mImpl->mModel->mScrollPosition.y, mImpl->mModel->mScrollPositionLast.y, Math::MACHINE_EPSILON_1 );
  return isScrolled;
}

void Controller::SetHiddenInputOption(const Property::Map& options)
{
  if( NULL == mImpl->mHiddenInput )
  {
    mImpl->mHiddenInput = new HiddenText( this );
  }
  mImpl->mHiddenInput->SetProperties(options);
}

void Controller::GetHiddenInputOption(Property::Map& options)
{
  if( NULL != mImpl->mHiddenInput )
  {
    mImpl->mHiddenInput->GetProperties(options);
  }
}

void Controller::SetPlaceholderProperty( const Property::Map& map )
{
  PlaceholderHandler::SetPlaceholderProperty(*this, map);
}

void Controller::GetPlaceholderProperty( Property::Map& map )
{
  PlaceholderHandler::GetPlaceholderProperty(*this, map);
}

Toolkit::DevelText::TextDirection::Type Controller::GetTextDirection()
{
  // Make sure the model is up-to-date before layouting
  ProcessModifyEvents();

  if ( mImpl->mUpdateTextDirection )
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>( CONVERT_TO_UTF32  |
                                                                           GET_SCRIPTS       |
                                                                           VALIDATE_FONTS    |
                                                                           GET_LINE_BREAKS   |
                                                                           BIDI_INFO         |
                                                                           SHAPE_TEXT        |
                                                                           GET_GLYPH_METRICS );

    // Set the update info to relayout the whole text.
    mImpl->mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    mImpl->mTextUpdateInfo.mRequestedNumberOfCharacters = mImpl->mModel->mLogicalModel->mText.Count();

    // Make sure the model is up-to-date before layouting
    mImpl->UpdateModel( onlyOnceOperations );

    Vector3 naturalSize;
    DoRelayout( Size( MAX_FLOAT, MAX_FLOAT ),
                static_cast<OperationsMask>( onlyOnceOperations |
                                             LAYOUT | REORDER | UPDATE_DIRECTION ),
                naturalSize.GetVectorXY() );

    // Do not do again the only once operations.
    mImpl->mOperationsPending = static_cast<OperationsMask>( mImpl->mOperationsPending & ~onlyOnceOperations );

    // Clear the update info. This info will be set the next time the text is updated.
    mImpl->mTextUpdateInfo.Clear();

    // FullRelayoutNeeded should be true because DoRelayout is MAX_FLOAT, MAX_FLOAT.
    mImpl->mTextUpdateInfo.mFullRelayoutNeeded = true;

    mImpl->mUpdateTextDirection = false;
  }

  return mImpl->mIsTextDirectionRTL ? Toolkit::DevelText::TextDirection::RIGHT_TO_LEFT : Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT;
}

Toolkit::DevelText::VerticalLineAlignment::Type Controller::GetVerticalLineAlignment() const
{
  return mImpl->mModel->GetVerticalLineAlignment();
}

void Controller::SetVerticalLineAlignment( Toolkit::DevelText::VerticalLineAlignment::Type alignment )
{
  mImpl->mModel->mVerticalLineAlignment = alignment;
}

// public : Relayout.

Controller::UpdateTextType Controller::Relayout( const Size& size, Dali::LayoutDirection::Type layoutDirection )
{
  return Relayouter::Relayout(*this, size, layoutDirection);
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
  EventHandler::KeyboardFocusGainEvent(*this);
}

void Controller::KeyboardFocusLostEvent()
{
  EventHandler::KeyboardFocusLostEvent(*this);
}

bool Controller::KeyEvent( const Dali::KeyEvent& keyEvent )
{
  return EventHandler::KeyEvent(*this, keyEvent);
}

void Controller::TapEvent( unsigned int tapCount, float x, float y )
{
  EventHandler::TapEvent(*this, tapCount, x, y);
}

void Controller::PanEvent( GestureState state, const Vector2& displacement )
{
  EventHandler::PanEvent(*this, state, displacement);
}

void Controller::LongPressEvent( GestureState state, float x, float y )
{
  EventHandler::LongPressEvent(*this, state, x, y);
}

void Controller::SelectEvent( float x, float y, SelectionType selectType )
{
  EventHandler::SelectEvent(*this, x, y, selectType);
}

void Controller::SetTextSelectionRange(const uint32_t *start, const uint32_t *end)
{
  if( mImpl->mEventData )
  {
    mImpl->mEventData->mCheckScrollAmount = true;
    mImpl->mEventData->mIsLeftHandleSelected = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->SetTextSelectionRange(start, end);
    mImpl->RequestRelayout();
    KeyboardFocusGainEvent();
  }
}

Uint32Pair Controller::GetTextSelectionRange() const
{
  return mImpl->GetTextSelectionRange();
}

CharacterIndex Controller::GetPrimaryCursorPosition() const
{
  return mImpl->GetPrimaryCursorPosition();
}

bool Controller::SetPrimaryCursorPosition( CharacterIndex index )
{
  if( mImpl->mEventData )
  {
    mImpl->mEventData->mCheckScrollAmount = true;
    mImpl->mEventData->mIsLeftHandleSelected = true;
    mImpl->mEventData->mIsRightHandleSelected = true;
    mImpl->mEventData->mCheckScrollAmount = true;
    if( mImpl->SetPrimaryCursorPosition(index) )
    {
      KeyboardFocusGainEvent();
      return true;
    }
  }
  return false;
}

void Controller::SelectWholeText()
{
  SelectEvent( 0.f, 0.f, SelectionType::ALL );
}

void Controller::SelectNone()
{
  SelectEvent( 0.f, 0.f, SelectionType::NONE );
}

string Controller::GetSelectedText() const
{
  string text;
  if( EventData::SELECTING == mImpl->mEventData->mState )
  {
    mImpl->RetrieveSelection( text, false );
  }
  return text;
}

InputMethodContext::CallbackData Controller::OnInputMethodContextEvent( InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent )
{
  return EventHandler::OnInputMethodContextEvent(*this, inputMethodContext, inputMethodContextEvent);
}

void Controller::PasteClipboardItemEvent()
{
  EventHandler::PasteClipboardItemEvent(*this);
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

bool Controller::IsEditable() const
{
  return mImpl->IsEditable();
}

void Controller::SetEditable( bool editable )
{
  mImpl->SetEditable( editable );
  if(mImpl->mEventData && mImpl->mEventData->mDecorator)
  {
    mImpl->mEventData->mDecorator->SetEditable( editable );
  }
}

void Controller::ScrollBy( Vector2 scroll )
{
  if( mImpl->mEventData && (fabs(scroll.x) > Math::MACHINE_EPSILON_0 || fabs(scroll.y) > Math::MACHINE_EPSILON_0))
  {
      const Vector2& layoutSize = mImpl->mModel->mVisualModel->GetLayoutSize();
      const Vector2 currentScroll = mImpl->mModel->mScrollPosition;

      scroll.x = -scroll.x;
      scroll.y = -scroll.y;

      if( fabs(scroll.x) > Math::MACHINE_EPSILON_0 )
      {
        mImpl->mModel->mScrollPosition.x += scroll.x;
        mImpl->ClampHorizontalScroll( layoutSize );
      }

      if( fabs(scroll.y) > Math::MACHINE_EPSILON_0 )
      {
        mImpl->mModel->mScrollPosition.y += scroll.y;
        mImpl->ClampVerticalScroll( layoutSize );
      }

      if (mImpl->mModel->mScrollPosition != currentScroll)
      {
        mImpl->mEventData->mDecorator->UpdatePositions( mImpl->mModel->mScrollPosition - currentScroll );
        mImpl->RequestRelayout();
      }
  }
}

float Controller::GetHorizontalScrollPosition()
{
  if( mImpl->mEventData )
  {
    //scroll values are negative internally so we convert them to positive numbers
    return -mImpl->mModel->mScrollPosition.x;
  }
  return 0;
}

float Controller::GetVerticalScrollPosition()
{
  if( mImpl->mEventData )
  {
    //scroll values are negative internally so we convert them to positive numbers
    return -mImpl->mModel->mScrollPosition.y;
  }
  return 0;
}

void Controller::DecorationEvent( HandleType handleType, HandleState state, float x, float y )
{
  EventHandler::DecorationEvent(*this, handleType, state, x, y);
}

// protected : Inherit from TextSelectionPopup::TextPopupButtonCallbackInterface.

void Controller::TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::Buttons button )
{
  EventHandler::TextPopupButtonTouched(*this, button);
}

void Controller::DisplayTimeExpired()
{
  mImpl->mEventData->mUpdateCursorPosition = true;
  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;

  mImpl->RequestRelayout();
}

// private : Update.

void Controller::InsertText( const std::string& text, Controller::InsertType type )
{
  TextUpdater::InsertText(*this, text, type);
}

void Controller::PasteText( const std::string& stringToPaste )
{
  TextUpdater::PasteText(*this, stringToPaste);
}

bool Controller::RemoveText( int cursorOffset,
                             int numberOfCharacters,
                             UpdateInputStyleType type )
{
  return TextUpdater::RemoveText(*this, cursorOffset, numberOfCharacters, type);
}

bool Controller::RemoveSelectedText()
{
  return TextUpdater::RemoveSelectedText(*this);
}

// private : Relayout.

bool Controller::DoRelayout( const Size& size,
                             OperationsMask operationsRequired,
                             Size& layoutSize )
{
  return Relayouter::DoRelayout(*this, size, operationsRequired, layoutSize);
}

void Controller::CalculateVerticalOffset( const Size& controlSize )
{
  Relayouter::CalculateVerticalOffset(*this, controlSize);
}

// private : Events.

void Controller::ProcessModifyEvents()
{
  EventHandler::ProcessModifyEvents(*this);
}

void Controller::TextReplacedEvent()
{
  EventHandler::TextReplacedEvent(*this);
}

void Controller::TextInsertedEvent()
{
  EventHandler::TextInsertedEvent(*this);
}

void Controller::TextDeletedEvent()
{
  EventHandler::TextDeletedEvent(*this);
}

bool Controller::DeleteEvent( int keyCode )
{
  return EventHandler::DeleteEvent(*this, keyCode);
}

// private : Helpers.

void Controller::ResetText()
{
  TextUpdater::ResetText(*this);
}

void Controller::ShowPlaceholderText()
{
  PlaceholderHandler::ShowPlaceholderText(*this);
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
                                                           BIDI_INFO                 |
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

CharacterIndex Controller::GetCursorPosition()
{
  if( !mImpl->mEventData )
    return 0;

  return mImpl->mEventData->mPrimaryCursorPosition;
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

void Controller::SetControlInterface( ControlInterface* controlInterface )
{
  mImpl->mControlInterface = controlInterface;
}

bool Controller::ShouldClearFocusOnEscape() const
{
  return mImpl->mShouldClearFocusOnEscape;
}

Actor Controller::CreateBackgroundActor()
{
  return mImpl->CreateBackgroundActor();
}

// private : Private contructors & copy operator.

Controller::Controller()
: Controller(nullptr, nullptr, nullptr)
{
}

Controller::Controller( ControlInterface* controlInterface )
:Controller( controlInterface, nullptr, nullptr)
{
}

Controller::Controller( ControlInterface* controlInterface,
                        EditableControlInterface* editableControlInterface,
                        SelectableControlInterface* selectableControlInterface )
: mImpl(new Controller::Impl(controlInterface, editableControlInterface, selectableControlInterface))
{
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
