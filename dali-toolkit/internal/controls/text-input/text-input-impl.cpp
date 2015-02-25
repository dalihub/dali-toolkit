/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-input/text-input-impl.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <sstream>
#include <algorithm>
#include <dali/public-api/adaptor-framework/virtual-keyboard.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-processor.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

using namespace Dali;

// Local Data
namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXT_INPUT");
#endif

const std::size_t DEFAULT_MAX_SIZE( std::numeric_limits<std::size_t>::max() ); // Max possible number
const std::size_t DEFAULT_NUMBER_OF_LINES_LIMIT( std::numeric_limits<std::size_t>::max() ); // Max possible number
const Vector3 DEFAULT_SELECTION_HANDLE_SIZE( 51.0f, 79.0f, 0.0f );  // Selection cursor image size
const Vector3 DEFAULT_GRAB_HANDLE_RELATIVE_SIZE( 1.5f, 2.0f, 1.0f );
const Vector3 DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE( 1.5f, 1.5f, 1.0f );
const Vector4 LIGHTBLUE( 0.07f, 0.41f, 0.59f, 1.0f );    // Used for Selection highlight

const char* DEFAULT_GRAB_HANDLE( DALI_IMAGE_DIR "insertpoint-icon.png" );
const char* DEFAULT_SELECTION_HANDLE_ONE( DALI_IMAGE_DIR "text-input-selection-handle-left.png" );
const char* DEFAULT_SELECTION_HANDLE_TWO( DALI_IMAGE_DIR "text-input-selection-handle-right.png" );
const char* DEFAULT_SELECTION_HANDLE_ONE_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-left-press.png" );
const char* DEFAULT_SELECTION_HANDLE_TWO_PRESSED( DALI_IMAGE_DIR "text-input-selection-handle-right-press.png" );

const std::size_t CURSOR_BLINK_INTERVAL = 500;                              ///< Cursor blink interval
const float CHARACTER_THRESHOLD( 2.5f );                                    ///< the threshold of a line.
const float DISPLAYED_HIGHLIGHT_Z_OFFSET( 0.1f );                           ///< 1. Highlight rendered (z-offset).
const float DISPLAYED_TEXT_VIEW_Z_OFFSET( 0.2f );                           ///< 2. Text rendered (z-offset).
const float UI_Z_OFFSET( 0.2f );                                            ///< 3. Text Selection Handles/Cursor z-offset.

const Vector3 UI_OFFSET(0.0f, 0.0f, UI_Z_OFFSET);                           ///< Text Selection Handles/Cursor offset.
const Vector3 DEFAULT_HANDLE_ONE_OFFSET(0.0f, -5.0f, 0.0f);                 ///< Handle One's Offset
const Vector3 DEFAULT_HANDLE_TWO_OFFSET(0.0f, -5.0f, 0.0f);                 ///< Handle Two's Offset
const float TOP_HANDLE_TOP_OFFSET( 34.0f);                                   ///< Offset between top handle and cutCopyPaste pop-up
const float BOTTOM_HANDLE_BOTTOM_OFFSET(34.0f);                              ///< Offset between bottom handle and cutCopyPaste pop-up
const float CURSOR_THICKNESS(4.0f);
const Degree CURSOR_ANGLE_OFFSET(2.0f);                                     ///< Offset from the angle of italic angle.
const Vector4 DEFAULT_CURSOR_COLOR(1.0f, 1.0f, 1.0f, 1.0f);

const std::string NEWLINE( "\n" );

const TextStyle DEFAULT_TEXT_STYLE;

const unsigned int SCROLL_TICK_INTERVAL = 50u;
const float SCROLL_THRESHOLD = 10.f;
const float SCROLL_SPEED = 15.f;

/**
 * Selection state enumeration (FSM)
 */
enum SelectionState
{
  SelectionNone,                            ///< Currently not encountered selected section.
  SelectionStarted,                         ///< Encountered selected section
  SelectionFinished                         ///< Finished selected section
};

std::size_t FindVisibleCharacterLeft( std::size_t cursorPosition, const Toolkit::TextView::CharacterLayoutInfoContainer& characterLayoutInfoTable )
{
  for( Toolkit::TextView::CharacterLayoutInfoContainer::const_reverse_iterator it = characterLayoutInfoTable.rbegin() + characterLayoutInfoTable.size() - cursorPosition, endIt = characterLayoutInfoTable.rend();
       it != endIt;
       ++it )
  {
    if( ( *it ).mIsVisible )
    {
      return --cursorPosition;
    }

    --cursorPosition;
  }

  return 0u;
}

std::size_t FindVisibleCharacterRight( std::size_t cursorPosition, const Toolkit::TextView::CharacterLayoutInfoContainer& characterLayoutInfoTable  )
{
  for( Toolkit::TextView::CharacterLayoutInfoContainer::const_iterator it = characterLayoutInfoTable.begin() + cursorPosition, endIt = characterLayoutInfoTable.end(); it < endIt; ++it )
  {
    if( ( *it ).mIsVisible )
    {
      return cursorPosition;
    }

    ++cursorPosition;
  }

  return cursorPosition;
}

/**
 * Whether the given position plus the cursor size offset is inside the given boundary.
 *
 * @param[in] position The given position.
 * @param[in] cursorSize The cursor size.
 * @param[in] controlSize The given boundary.
 *
 * @return whether the given position is inside the given boundary.
 */
bool IsPositionInsideBoundaries( const Vector3& position, const Size& cursorSize, const Vector3& controlSize )
{
  return ( position.x >= -Math::MACHINE_EPSILON_1000 ) &&
         ( position.x <= controlSize.width + Math::MACHINE_EPSILON_1000 ) &&
         ( position.y - cursorSize.height >= -Math::MACHINE_EPSILON_1000 ) &&
         ( position.y <= controlSize.height + Math::MACHINE_EPSILON_1000 );
}

/**
 * Splits a text in two halves.
 *
 * If the text's number of characters is odd, firstHalf has one more character.
 *
 * @param[in] text The text to be split.
 * @param[out] firstHalf The first half of the text.
 * @param[out] secondHalf The second half of the text.
 */
void SplitText( const Toolkit::MarkupProcessor::StyledTextArray& text,
                      Toolkit::MarkupProcessor::StyledTextArray& firstHalf,
                      Toolkit::MarkupProcessor::StyledTextArray& secondHalf )
{
  firstHalf.clear();
  secondHalf.clear();

  const std::size_t textLength = text.size();
  const std::size_t half = ( textLength / 2 ) + ( textLength % 2 );

  firstHalf.insert( firstHalf.end(), text.begin(), text.begin() + half );
  secondHalf.insert( secondHalf.end(), text.begin() + half, text.end() );
}

} // end of namespace

namespace Dali
{

namespace Toolkit
{
// Properties
const Property::Index TextInput::HIGHLIGHT_COLOR_PROPERTY                     = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX;
const Property::Index TextInput::CUT_AND_PASTE_COLOR_PROPERTY                 = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+1;
const Property::Index TextInput::CUT_AND_PASTE_PRESSED_COLOR_PROPERTY         = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+2;
const Property::Index TextInput::CUT_AND_PASTE_BORDER_COLOR_PROPERTY          = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+3;
const Property::Index TextInput::CUT_AND_PASTE_ICON_COLOR_PROPERTY            = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+4;
const Property::Index TextInput::CUT_AND_PASTE_ICON_PRESSED_COLOR_PROPERTY    = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+5;
const Property::Index TextInput::CUT_AND_PASTE_TEXT_COLOR_PROPERTY            = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+6;
const Property::Index TextInput::CUT_AND_PASTE_TEXT_PRESSED_COLOR_PROPERTY    = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+7;
const Property::Index TextInput::CUT_BUTTON_POSITION_PRIORITY_PROPERTY        = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+8;
const Property::Index TextInput::COPY_BUTTON_POSITION_PRIORITY_PROPERTY       = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+9;
const Property::Index TextInput::PASTE_BUTTON_POSITION_PRIORITY_PROPERTY      = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+10;
const Property::Index TextInput::SELECT_BUTTON_POSITION_PRIORITY_PROPERTY     = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+11;
const Property::Index TextInput::SELECT_ALL_BUTTON_POSITION_PRIORITY_PROPERTY = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+12;
const Property::Index TextInput::CLIPBOARD_BUTTON_POSITION_PRIORITY_PROPERTY  = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+13;
const Property::Index TextInput::POP_UP_OFFSET_FROM_TEXT_PROPERTY             = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+14;
const Property::Index TextInput::CURSOR_COLOR_PROPERTY                        = Internal::TextInput::TEXTINPUT_PROPERTY_START_INDEX+15;


namespace Internal
{

namespace
{

// Signals

const char* const SIGNAL_START_INPUT =                  "start-input";
const char* const SIGNAL_END_INPUT =                    "end-input";
const char* const SIGNAL_STYLE_CHANGED =                "style-changed";
const char* const SIGNAL_MAX_INPUT_CHARACTERS_REACHED = "max-input-characters-reached";
const char* const SIGNAL_TOOLBAR_DISPLAYED =            "toolbar-displayed";
const char* const SIGNAL_TEXT_EXCEED_BOUNDARIES =       "text-exceed-boundaries";

BaseHandle Create()
{
  return Toolkit::TextInput::New();
}

TypeRegistration typeRegistration( typeid( Toolkit::TextInput ), typeid( Toolkit::Control ), Create );

SignalConnectorType signalConnector1( typeRegistration, SIGNAL_START_INPUT,                  &TextInput::DoConnectSignal );
SignalConnectorType signalConnector2( typeRegistration, SIGNAL_END_INPUT,                    &TextInput::DoConnectSignal );
SignalConnectorType signalConnector3( typeRegistration, SIGNAL_STYLE_CHANGED,                &TextInput::DoConnectSignal );
SignalConnectorType signalConnector4( typeRegistration, SIGNAL_MAX_INPUT_CHARACTERS_REACHED, &TextInput::DoConnectSignal );
SignalConnectorType signalConnector5( typeRegistration, SIGNAL_TOOLBAR_DISPLAYED,            &TextInput::DoConnectSignal );
SignalConnectorType signalConnector6( typeRegistration, SIGNAL_TEXT_EXCEED_BOUNDARIES,       &TextInput::DoConnectSignal );

}

PropertyRegistration property1( typeRegistration, "highlight-color",  Toolkit::TextInput::HIGHLIGHT_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property2( typeRegistration, "cut-and-paste-bg-color",  Toolkit::TextInput::CUT_AND_PASTE_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property3( typeRegistration, "cut-and-paste-pressed-color",  Toolkit::TextInput::CUT_AND_PASTE_PRESSED_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property4( typeRegistration, "cut-and-paste-icon-color",  Toolkit::TextInput::CUT_AND_PASTE_ICON_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property5( typeRegistration, "cut-and-paste-icon-pressed-color",  Toolkit::TextInput::CUT_AND_PASTE_ICON_PRESSED_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property6( typeRegistration, "cut-and-paste-text-color",  Toolkit::TextInput::CUT_AND_PASTE_TEXT_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property7( typeRegistration, "cut-and-paste-text-pressed-color",  Toolkit::TextInput::CUT_AND_PASTE_TEXT_PRESSED_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property8( typeRegistration, "cut-and-paste-border-color",  Toolkit::TextInput::CUT_AND_PASTE_BORDER_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property9( typeRegistration, "cut-button-position-priority",  Toolkit::TextInput::CUT_BUTTON_POSITION_PRIORITY_PROPERTY, Property::UNSIGNED_INTEGER, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property10( typeRegistration, "copy-button-position-priority",  Toolkit::TextInput::COPY_BUTTON_POSITION_PRIORITY_PROPERTY, Property::UNSIGNED_INTEGER, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property11( typeRegistration, "paste-button-position-priority",  Toolkit::TextInput::PASTE_BUTTON_POSITION_PRIORITY_PROPERTY, Property::UNSIGNED_INTEGER, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property12( typeRegistration, "select-button-position-priority",  Toolkit::TextInput::SELECT_BUTTON_POSITION_PRIORITY_PROPERTY, Property::UNSIGNED_INTEGER, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property13( typeRegistration, "select-all-button-position-priority",  Toolkit::TextInput::SELECT_ALL_BUTTON_POSITION_PRIORITY_PROPERTY, Property::UNSIGNED_INTEGER, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property14( typeRegistration, "clipboard-button-position-priority",  Toolkit::TextInput::CLIPBOARD_BUTTON_POSITION_PRIORITY_PROPERTY, Property::UNSIGNED_INTEGER, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property15( typeRegistration, "popup-offset-from-text", Toolkit::TextInput::POP_UP_OFFSET_FROM_TEXT_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );
PropertyRegistration property16( typeRegistration, "cursor-color", Toolkit::TextInput::CURSOR_COLOR_PROPERTY, Property::VECTOR4, &TextInput::SetProperty, &TextInput::GetProperty );


// [TextInput::HighlightInfo] /////////////////////////////////////////////////

void TextInput::HighlightInfo::AddQuad( float x1, float y1, float x2, float y2 )
{
  QuadCoordinates quad(x1, y1, x2, y2);
  mQuadList.push_back( quad );
}

void TextInput::HighlightInfo::Clamp2D(const Vector2& min, const Vector2& max)
{
  for(std::size_t i = 0;i < mQuadList.size(); i++)
  {
    QuadCoordinates& quad = mQuadList[i];

    quad.min.Clamp(min, max);
    quad.max.Clamp(min, max);
  } // end for
}

// [TextInput] ////////////////////////////////////////////////////////////////

Dali::Toolkit::TextInput TextInput::New()
{
  // Create the implementation
  TextInputPtr textInput(new TextInput());
  // Pass ownership to CustomActor via derived handle
  Dali::Toolkit::TextInput handle(*textInput);
  handle.SetName( "TextInput");

  textInput->Initialize();
  return handle;
}

TextInput::TextInput()
:Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS | REQUIRES_STYLE_CHANGE_SIGNALS ) ),
 mState( StateEdit ),
 mStyledText(),
 mInputStyle(),
 mLineHeight( 0.f ),
 mDisplayedTextView(),
 mStyledPlaceHolderText(),
 mMaxStringLength( DEFAULT_MAX_SIZE ),
 mNumberOflinesLimit( DEFAULT_NUMBER_OF_LINES_LIMIT ),
 mCursorPosition( 0 ),
 mActualGrabHandlePosition( 0.0f, 0.0f, 0.0f ),
 mIsSelectionHandleOneFlipped( false ),
 mIsSelectionHandleTwoFlipped( false ),
 mSelectionHandleOneOffset( DEFAULT_HANDLE_ONE_OFFSET ),
 mSelectionHandleTwoOffset( DEFAULT_HANDLE_TWO_OFFSET ),
 mSelectionHandleOneActualPosition( 0.0f, 0.0f , 0.0f ),
 mSelectionHandleTwoActualPosition( 0.0f, 0.0f , 0.0f ),
 mSelectionHandleOnePosition( 0 ),
 mSelectionHandleTwoPosition( 0 ),
 mPreEditString(),
 mPreEditStartPosition( 0 ),
 mPreEditLength ( 0 ),
 mNumberOfSurroundingCharactersDeleted( 0 ),
 mTouchStartTime( 0 ),
 mTextLayoutInfo(),
 mCurrentCopySelecton(),
 mPopupPanel(),
 mScrollTimer(),
 mScrollDisplacement(),
 mCurrentHandlePosition(),
 mCurrentSelectionId(),
 mCurrentSelectionHandlePosition(),
 mRequestedSelection( 0, 0 ),
 mSelectionHandleFlipMargin( 0.0f, 0.0f, 0.0f, 0.0f ),
 mBoundingRectangleWorldCoordinates( 0.0f, 0.0f, 0.0f, 0.0f ),
 mClipboard(),
 mMaterialColor( LIGHTBLUE ),
 mPopupOffsetFromText ( Vector4( 0.0f, TOP_HANDLE_TOP_OFFSET, 0.0f, BOTTOM_HANDLE_BOTTOM_OFFSET ) ),
 mOverrideAutomaticAlignment( false ),
 mCursorRTLEnabled( false ),
 mClosestCursorPositionEOL ( false ),
 mCursorBlinkStatus( true ),
 mCursorVisibility( false ),
 mGrabHandleVisibility( false ),
 mIsCursorInScrollArea( true ),
 mIsGrabHandleInScrollArea( true ),
 mEditModeActive( false ),
 mEditOnTouch( true ),
 mTextSelection( true ),
 mExceedEnabled( true ),
 mGrabHandleEnabled( true ),
 mIsSelectionHandleFlipEnabled( true ),
 mPreEditFlag( false ),
 mIgnoreCommitFlag( false ),
 mIgnoreFirstCommitFlag( false ),
 mSelectingText( false ),
 mPreserveCursorPosition( false ),
 mSelectTextOnCommit( false ),
 mUnderlinedPriorToPreEdit ( false ),
 mCommitByKeyInput( false ),
 mPlaceHolderSet( false ),
 mMarkUpEnabled( false )
{
  // Updates the line height accordingly with the input style.
  UpdateLineHeight();
}

TextInput::~TextInput()
{
  StopCursorBlinkTimer();
}

// Public

std::string TextInput::GetText() const
{
  std::string text;

  // Return text-view's text only if the text-input's text is not empty
  // in order to not to return the placeholder text.
  if( !mStyledText.empty() )
  {
    text = mDisplayedTextView.GetText();
  }

  return text;
}

std::string TextInput::GetMarkupText() const
{
  std::string markupString;
  MarkupProcessor::GetMarkupString( mStyledText, markupString );

  return markupString;
}

void TextInput::ShowPlaceholderText( const MarkupProcessor::StyledTextArray& stylePlaceHolderText )
{
  mDisplayedTextView.SetText( stylePlaceHolderText );
  mPlaceHolderSet = true;
  mDisplayedTextView.SetScrollPosition( Vector2( 0.0f,0.0f ) );
}

void TextInput::SetPlaceholderText( const std::string& placeHolderText )
{
  // Get the placeholder styled text array from the markup string.
  MarkupProcessor::GetStyledTextArray( placeHolderText, mStyledPlaceHolderText, IsMarkupProcessingEnabled() );
  if( mStyledText.empty() )
  {
    ShowPlaceholderText( mStyledPlaceHolderText );
  }
}

std::string TextInput::GetPlaceholderText()
{
  // Traverses the styled placeholder array getting only the text.
  //  Note that for some languages a 'character' could be represented by more than one 'char'

  std::string placeholderText;
  for( MarkupProcessor::StyledTextArray::const_iterator it = mStyledPlaceHolderText.begin(), endIt = mStyledPlaceHolderText.end(); it != endIt; ++it )
  {
    placeholderText.append( (*it).mText.GetText() );
  }

  return placeholderText ;
}

void TextInput::SetInitialText(const std::string& initialText)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "SetInitialText string[%s]\n", initialText.c_str() );

  if ( mPreEditFlag ) // If in the pre-edit state and text is being set then discard text being inserted.
  {
    mPreEditFlag = false;
    mIgnoreCommitFlag = true;
  }

  SetText( initialText );
  PreEditReset( false ); // Reset keyboard as text changed
}

void TextInput::SetText(const std::string& initialText)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "SetText string[%s]\n", initialText.c_str() );

  GetStyledTextArray( initialText, mStyledText, IsMarkupProcessingEnabled() );

  if( mStyledText.empty() )
  {
    ShowPlaceholderText( mStyledPlaceHolderText );
  }
  else
  {
    mDisplayedTextView.SetText( mStyledText );
    mPlaceHolderSet = false;
  }

  GetTextLayoutInfo();

  mCursorPosition = mTextLayoutInfo.mCharacterLayoutInfoTable.size();

  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    imfManager.SetCursorPosition( mCursorPosition );
    imfManager.SetSurroundingText( initialText );
    imfManager.NotifyCursorPosition();
  }

  if( IsScrollEnabled() )
  {
    ScrollTextViewToMakeCursorVisible( Vector3( mTextLayoutInfo.mScrollOffset.x, mTextLayoutInfo.mScrollOffset.y, 0.f ) );
  }

  ShowGrabHandleAndSetVisibility( false );

  RemoveHighlight();

  DrawCursor();

  EmitTextModified();
}

void TextInput::SetText( const MarkupProcessor::StyledTextArray& styleText )
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "SetText markup text\n" );

  mDisplayedTextView.SetText( styleText );
  mPlaceHolderSet = false;

  // If text alignment hasn't been manually set by application developer, then we
  // automatically determine the alignment based on the content of the text i.e. what
  // language the text begins with.
  // TODO: This should determine different alignments for each line (broken by '\n') of text.
  if(!mOverrideAutomaticAlignment)
  {
    // Determine bidi direction of first character (skipping past whitespace, numbers, and symbols)
    bool leftToRight(true);

    if( !styleText.empty() )
    {
      bool breakOut(false);

      for( MarkupProcessor::StyledTextArray::const_iterator textIter = styleText.begin(), textEndIter = styleText.end(); ( textIter != textEndIter ) && ( !breakOut ); ++textIter )
      {
        const Text& text = textIter->mText;

        for( std::size_t i = 0; i < text.GetLength(); ++i )
        {
          Character character( text[i] );
          if( character.GetCharacterDirection() != Character::Neutral )
          {
            leftToRight = ( character.GetCharacterDirection() == Character::LeftToRight );
            breakOut = true;
            break;
          }
        }
      }
    }

    // Based on this direction, either left or right align text if not manually set by application developer.
    mDisplayedTextView.SetTextAlignment( static_cast<Toolkit::Alignment::Type>(
                                           ( leftToRight ? Toolkit::Alignment::HorizontalLeft : Toolkit::Alignment::HorizontalRight) |
                                             Toolkit::Alignment::VerticalTop ) );
    mDisplayedTextView.SetLineJustification( leftToRight ? Toolkit::TextView::Left : Toolkit::TextView::Right);
  }

  EmitTextModified();
}

void TextInput::SetMaxCharacterLength(std::size_t maxChars)
{
  mMaxStringLength = maxChars;
}

void TextInput::SetNumberOfLinesLimit(std::size_t maxLines)
{
  DALI_ASSERT_DEBUG( maxLines > 0 )

  if ( maxLines > 0)
  {
    mNumberOflinesLimit = maxLines;
  }
}

std::size_t TextInput::GetNumberOfLinesLimit() const
{
  return mNumberOflinesLimit;
}

std::size_t TextInput::GetNumberOfCharacters() const
{
  return mStyledText.size();
}

// Styling
void TextInput::SetMaterialDiffuseColor( const Vector4& color )
{
  mMaterialColor = color;
  if ( mCustomMaterial )
  {
    mCustomMaterial.SetDiffuseColor( mMaterialColor );
    mMeshData.SetMaterial( mCustomMaterial );
  }
}

const Vector4& TextInput::GetMaterialDiffuseColor() const
{
  return mMaterialColor;
}

// Signals

Toolkit::TextInput::InputSignalType& TextInput::InputStartedSignal()
{
  return mInputStartedSignal;
}

Toolkit::TextInput::InputSignalType& TextInput::InputFinishedSignal()
{
  return mInputFinishedSignal;
}

Toolkit::TextInput::InputSignalType& TextInput::CutAndPasteToolBarDisplayedSignal()
{
  return mCutAndPasteToolBarDisplayed;
}

Toolkit::TextInput::StyleChangedSignalType& TextInput::StyleChangedSignal()
{
  return mStyleChangedSignal;
}

Toolkit::TextInput::TextModifiedSignalType& TextInput::TextModifiedSignal()
{
  return mTextModifiedSignal;
}

Toolkit::TextInput::MaxInputCharactersReachedSignalType& TextInput::MaxInputCharactersReachedSignal()
{
  return mMaxInputCharactersReachedSignal;
}

Toolkit::TextInput::InputTextExceedBoundariesSignalType& TextInput::InputTextExceedBoundariesSignal()
{
  return mInputTextExceedBoundariesSignal;
}

bool TextInput::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::TextInput textInput = Toolkit::TextInput::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_START_INPUT ) )
  {
    textInput.InputStartedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_END_INPUT ) )
  {
    textInput.InputFinishedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_STYLE_CHANGED ) )
  {
    textInput.StyleChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_MAX_INPUT_CHARACTERS_REACHED ) )
  {
    textInput.MaxInputCharactersReachedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_TOOLBAR_DISPLAYED ) )
  {
    textInput.CutAndPasteToolBarDisplayedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_TEXT_EXCEED_BOUNDARIES ) )
  {
    textInput.InputTextExceedBoundariesSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void TextInput::SetEditable(bool editMode, bool setCursorOnTouchPoint, const Vector2& touchPoint)
{
  if(editMode)
  {
    // update line height before calculate the actual position.
    UpdateLineHeight();

    if(!mEditModeActive)
    {
      if( setCursorOnTouchPoint )
      {
        // Sets the cursor position for the given touch point.
        ReturnClosestIndex( touchPoint, mCursorPosition );

        // Creates the grab handle.
        if( IsGrabHandleEnabled() )
        {
          const Vector3 cursorPosition = GetActualPositionFromCharacterPosition(mCursorPosition);

          CreateGrabHandle();

          mActualGrabHandlePosition.x = cursorPosition.x; // Set grab handle to be at the cursor position
          mActualGrabHandlePosition.y = cursorPosition.y; // Set grab handle to be at the cursor position
          mGrabHandle.SetPosition( mActualGrabHandlePosition + UI_OFFSET );
          ShowGrabHandleAndSetVisibility( true );

          // Scrolls the text-view if needed.
          if( IsScrollEnabled() )
          {
            ScrollTextViewToMakeCursorVisible( cursorPosition );
          }
        }
      }
      else
      {
        mCursorPosition = mStyledText.size(); // Initially set cursor position to end of string.
      }
    }

    StartEditMode();
  }
  else
  {
    EndEditMode();
  }
}

bool TextInput::IsEditable() const
{
  return mEditModeActive;
}

void TextInput::SetEditOnTouch( bool editOnTouch )
{
  mEditOnTouch = editOnTouch;
}

bool TextInput::IsEditOnTouch() const
{
  return mEditOnTouch;
}

void TextInput::SetTextSelectable( bool textSelectable )
{
  mTextSelection = textSelectable;
}

bool TextInput::IsTextSelectable() const
{
  return mTextSelection;
}

bool TextInput::IsTextSelected() const
{
  return mHighlightMeshActor;
}

void TextInput::DeSelectText()
{
  RemoveHighlight();
  HidePopup();
  CursorUpdate();
}

void TextInput::SetGrabHandleImage(Dali::Image image )
{
  if (image)
  {
    CreateGrabHandle(image);
  }
}

void TextInput::SetCursorImage(Dali::Image image, const Vector4& border )
{
  DALI_ASSERT_DEBUG ( image && "Create cursor image invalid")

  if ( image )
  {
    mCursor.SetImage( image );
    mCursor.SetNinePatchBorder( border );
  }
}

Vector3 TextInput::GetSelectionHandleSize()
{
  return DEFAULT_SELECTION_HANDLE_SIZE;
}

void TextInput::SetRTLCursorImage(Dali::Image image, const Vector4& border )
{
  DALI_ASSERT_DEBUG ( image && "Create cursor image invalid")

  if ( image )
  {
    mCursorRTL.SetImage( image);
    mCursorRTL.SetNinePatchBorder(  border );
  }
}

void TextInput::EnableGrabHandle(bool toggle)
{
  // enables grab handle with will in turn de-activate magnifier
  mGrabHandleEnabled = toggle;
}

bool TextInput::IsGrabHandleEnabled()
{
  // if false then magnifier will be shown instead.
  return mGrabHandleEnabled;
}

void TextInput::EnableSelectionHandleFlip( bool toggle )
{
  // Deprecated function.  To be removed.
  mIsSelectionHandleFlipEnabled = toggle;
}

bool TextInput::IsSelectionHandleFlipEnabled()
{
  // Deprecated function, To be removed. Returns true as handle flipping always enabled by default so handles do not exceed screen.
  return true;
}

void TextInput::SetSelectionHandleFlipMargin( const Vector4& margin )
{
  // Deprecated function, now just stores margin for retreival, remove completely once depricated Public API removed.
  Vector3 textInputSize = mDisplayedTextView.GetCurrentSize();
  const Vector4 flipBoundary( -margin.x, -margin.y, textInputSize.width + margin.z, textInputSize.height + margin.w );

  mSelectionHandleFlipMargin = margin;
}

void TextInput::SetBoundingRectangle( const Rect<float>& boundingRectangle )
{
  // Convert to world coordinates and store as a Vector4 to be compatiable with Property Notifications.
  Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  const float originX = boundingRectangle.x - 0.5f * stageSize.width;
  const float originY = boundingRectangle.y - 0.5f * stageSize.height;

  const Vector4 boundary( originX,
                          originY,
                          originX + boundingRectangle.width,
                          originY + boundingRectangle.height );

  mBoundingRectangleWorldCoordinates = boundary;
}

const Rect<float> TextInput::GetBoundingRectangle() const
{
  Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  const float originX = mBoundingRectangleWorldCoordinates.x + 0.5f * stageSize.width;
  const float originY = mBoundingRectangleWorldCoordinates.y + 0.5f * stageSize.height;

  Rect<float>boundingRect( originX, originY, mBoundingRectangleWorldCoordinates.z - mBoundingRectangleWorldCoordinates.x, mBoundingRectangleWorldCoordinates.w - mBoundingRectangleWorldCoordinates.y);

  return boundingRect;
}

const Vector4& TextInput::GetSelectionHandleFlipMargin()
{
  return mSelectionHandleFlipMargin;
}

void TextInput::SetTextColor( const Vector4& color )
{
  mDisplayedTextView.SetColor( color );
}

void TextInput::SetActiveStyle( const TextStyle& style, const TextStyle::Mask mask )
{
  if( style != mInputStyle )
  {
    // different style.
    bool emitSignal = false;

    // mask: modify style according to mask, if different emit signal.
    const TextStyle oldInputStyle( mInputStyle );

    // Copy the new style.
    mInputStyle.Copy( style, mask );

    // if style has changed, emit signal.
    if( oldInputStyle != mInputStyle )
    {
      emitSignal = true;
    }

    // Updates the line height accordingly with the input style.
    UpdateLineHeight();

    // Changing font point size will require the cursor to be re-sized
    DrawCursor();

    if( emitSignal )
    {
      EmitStyleChangedSignal();
    }
  }
}

void TextInput::ApplyStyle( const TextStyle& style, const TextStyle::Mask mask )
{
  if ( IsTextSelected() )
  {
    const std::size_t begin = std::min(mSelectionHandleOnePosition, mSelectionHandleTwoPosition);
    const std::size_t end = std::max(mSelectionHandleOnePosition, mSelectionHandleTwoPosition) - 1;

    if( !mTextLayoutInfo.mCharacterLogicalToVisualMap.empty() )
    {
      ApplyStyleToRange(style, mask, mTextLayoutInfo.mCharacterLogicalToVisualMap[begin], mTextLayoutInfo.mCharacterLogicalToVisualMap[end]);
    }

    // Keeps the old style to be compared with the new one.
    const TextStyle oldInputStyle( mInputStyle );

    // Copy only those parameters from the style which are set in the mask.
    mInputStyle.Copy( style, mask );

    if( mInputStyle != oldInputStyle )
    {
      // Updates the line height accordingly with the input style.
      UpdateLineHeight();

      EmitStyleChangedSignal();
    }
  }
}

void TextInput::ApplyStyleToAll( const TextStyle& style, const TextStyle::Mask mask )
{
  if( !mStyledText.empty() )
  {
    ApplyStyleToRange( style, mask, 0, mStyledText.size() - 1 );
  }
}

TextStyle TextInput::GetStyleAtCursor() const
{
  TextStyle style;

  if ( !mStyledText.empty() && ( mCursorPosition > 0 ) )
  {
    DALI_ASSERT_DEBUG( ( 0 <= mCursorPosition-1 ) && ( mCursorPosition-1 < mStyledText.size() ) );
    style = mStyledText.at( mCursorPosition-1 ).mStyle;
  }
  else // No text.
  {
    style = mInputStyle;

    if ( mInputStyle.GetFontPointSize() <  Math::MACHINE_EPSILON_1000 )
    {
      Dali::Font defaultFont = Dali::Font::New();
      style.SetFontPointSize( PointSize( defaultFont.GetPointSize()) );
    }
  }

  return style;
}

TextStyle TextInput::GetStyleAt( std::size_t position ) const
{
  DALI_ASSERT_DEBUG( ( 0 <= position ) && ( position <= mStyledText.size() ) );

  if( position >= mStyledText.size() )
  {
    position = mStyledText.size() - 1;
  }

  return mStyledText.at( position ).mStyle;
}

void TextInput::SetTextAlignment( Toolkit::Alignment::Type align )
{
  mDisplayedTextView.SetTextAlignment( align );
  mOverrideAutomaticAlignment = true;
}

void TextInput::SetTextLineJustification( Toolkit::TextView::LineJustification justification )
{
  mDisplayedTextView.SetLineJustification( justification );
  mOverrideAutomaticAlignment = true;
}

void TextInput::SetFadeBoundary( const Toolkit::TextView::FadeBoundary& fadeBoundary )
{
  mDisplayedTextView.SetFadeBoundary( fadeBoundary );
}

const Toolkit::TextView::FadeBoundary& TextInput::GetFadeBoundary() const
{
  return mDisplayedTextView.GetFadeBoundary();
}

Toolkit::Alignment::Type TextInput::GetTextAlignment() const
{
  return mDisplayedTextView.GetTextAlignment();
}

void TextInput::SetMultilinePolicy( Toolkit::TextView::MultilinePolicy policy )
{
  mDisplayedTextView.SetMultilinePolicy( policy );
}

Toolkit::TextView::MultilinePolicy TextInput::GetMultilinePolicy() const
{
  return mDisplayedTextView.GetMultilinePolicy();
}

void TextInput::SetWidthExceedPolicy( Toolkit::TextView::ExceedPolicy policy )
{
  mDisplayedTextView.SetWidthExceedPolicy( policy );
}

Toolkit::TextView::ExceedPolicy TextInput::GetWidthExceedPolicy() const
{
  return mDisplayedTextView.GetWidthExceedPolicy();
}

void TextInput::SetHeightExceedPolicy( Toolkit::TextView::ExceedPolicy policy )
{
  mDisplayedTextView.SetHeightExceedPolicy( policy );
}

Toolkit::TextView::ExceedPolicy TextInput::GetHeightExceedPolicy() const
{
  return mDisplayedTextView.GetHeightExceedPolicy();
}

void TextInput::SetExceedEnabled( bool enable )
{
  mExceedEnabled = enable;
}

bool TextInput::GetExceedEnabled() const
{
  return mExceedEnabled;
}

void TextInput::SetBackground(Dali::Image image )
{
  // TODO Should add this function and add public api to match.
}

bool TextInput::OnTouchEvent(const TouchEvent& event)
{
  return false;
}

bool TextInput::OnKeyEvent(const KeyEvent& event)
{
  switch( event.state )
  {
    case KeyEvent::Down:
    {
      return OnKeyDownEvent(event);
    }
    break;

    case KeyEvent::Up:
    {
      return OnKeyUpEvent(event);
    }
    break;

    default:
    {
      return false;
    }
    break;
  }
}

void TextInput::OnKeyInputFocusGained()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, ">>OnKeyInputFocusGained\n" );

  mEditModeActive = true;

  mActiveLayer.RaiseToTop(); // Ensure layer holding handles is on top

  mInputStyle = GetStyleAtCursor(); // Inherit style from cursor position

  // Updates the line height accordingly with the input style.
  UpdateLineHeight();

  // Connect the signals to use in text input.
  VirtualKeyboard::StatusChangedSignal().Connect( this, &TextInput::KeyboardStatusChanged );
  VirtualKeyboard::LanguageChangedSignal().Connect( this, &TextInput::SetTextDirection );

  // Set the text direction if empty and connect to the signal to ensure we change direction when the language changes.
  SetTextDirection();

  GetTextLayoutInfo();

  DrawCursor();
  SetCursorVisibility( true );
  StartCursorBlinkTimer();

  Toolkit::TextInput handle( GetOwner() );
  mInputStartedSignal.Emit( handle );

  ImfManager imfManager = ImfManager::Get();

  if ( imfManager )
  {
    imfManager.EventReceivedSignal().Connect(this, &TextInput::ImfEventReceived);

    // Notify that the text editing start.
    imfManager.Activate();

    // When window gain lost focus, the imf manager is deactivated. Thus when window gain focus again, the imf manager must be activated.
    imfManager.SetRestoreAfterFocusLost( true );

    imfManager.SetCursorPosition( mCursorPosition );
    imfManager.NotifyCursorPosition();
  }

  mClipboard = Clipboard::Get(); // Store handle to clipboard

  // Now in edit mode we can accept string to paste from clipboard
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );
  if ( notifier )
  {
    notifier.ContentSelectedSignal().Connect( this, &TextInput::OnClipboardTextSelected );
  }
}

void TextInput::OnKeyInputFocusLost()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, ">>OnKeyInputFocusLost\n" );

  if( mPreEditFlag )
  {
    // If key input focus is lost, it removes the
    // underline from the last pre-edit text.
    RemovePreEditStyle();
    const std::size_t numberOfCharactersDeleted = DeletePreEdit();
    InsertAt( mPreEditString, mPreEditStartPosition, numberOfCharactersDeleted );
    EmitTextModified();
  }

  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    // The text editing is finished. Therefore the imf manager don't have restore activation.
    imfManager.SetRestoreAfterFocusLost( false );

    // Notify that the text editing finish.
    imfManager.Deactivate();

    imfManager.EventReceivedSignal().Disconnect(this, &TextInput::ImfEventReceived);
  }
  // Disconnect signal used the text input.
  VirtualKeyboard::LanguageChangedSignal().Disconnect( this, &TextInput::SetTextDirection );

  Toolkit::TextInput handle( GetOwner() );
  mInputFinishedSignal.Emit( handle );
  mEditModeActive = false;
  mPreEditFlag = false;
  RemoveHighlight();
  SetCursorVisibility( false );
  StopCursorBlinkTimer();

  ShowGrabHandleAndSetVisibility( false );

  mClipboard.Reset();
  // No longer in edit mode so do not want to receive string from clipboard
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );
  if ( notifier )
  {
    notifier.ContentSelectedSignal().Disconnect( this, &TextInput::OnClipboardTextSelected );
  }

  Clipboard clipboard = Clipboard::Get();
  if ( clipboard )
  {
    clipboard.HideClipboard();
  }
}

void TextInput::OnControlStageConnection()
{
  Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

  if ( mBoundingRectangleWorldCoordinates == Vector4::ZERO )
  {
    SetBoundingRectangle( Rect<float>( 0.0f, 0.0f, stageSize.width, stageSize.height ));
  }
}

void TextInput::CreateActiveLayer()
{
  Actor self = Self();
  mActiveLayer = Layer::New();
  mActiveLayer.SetName ( "ActiveLayerActor" );

  mActiveLayer.SetAnchorPoint( AnchorPoint::CENTER);
  mActiveLayer.SetParentOrigin( ParentOrigin::CENTER);
  mActiveLayer.SetPositionInheritanceMode( USE_PARENT_POSITION );

  self.Add( mActiveLayer );
  mActiveLayer.RaiseToTop();
}

void TextInput::OnInitialize()
{
  CreateTextViewActor();

  SetUpTouchEvents();

  // Create 2 cursors (standard LTR and RTL cursor for when text can be added at
  // different positions depending on language)
  mCursor = CreateCursor(DEFAULT_CURSOR_COLOR);
  mCursorRTL = CreateCursor(DEFAULT_CURSOR_COLOR);

  Actor self = Self();
  self.Add( mCursor );
  self.Add( mCursorRTL );

  mCursorVisibility = false;

  CreateActiveLayer(); // todo move this so layer only created when needed.

  // Assign names to image actors
  mCursor.SetName("mainCursor");
  mCursorRTL.SetName("rtlCursor");
}

void TextInput::OnControlSizeSet(const Vector3& targetSize)
{
  mDisplayedTextView.SetSize( targetSize );
  GetTextLayoutInfo();
  mActiveLayer.SetSize(targetSize);
}

void TextInput::OnRelayout( const Vector2& size, ActorSizeContainer& container )
{
  Relayout( mDisplayedTextView, size, container );
  Relayout( mPopupPanel.GetRootActor(), size, container );

  GetTextLayoutInfo();

  DrawCursor();
}

Vector3 TextInput::GetNaturalSize()
{
  Vector3 naturalSize = mDisplayedTextView.GetNaturalSize();

  if( mEditModeActive && ( Vector3::ZERO == naturalSize ) )
  {
    // If the natural is zero, it means there is no text. Let's return the cursor height as the natural height.
    naturalSize.height = mLineHeight;
  }

  return naturalSize;
}

float TextInput::GetHeightForWidth( float width )
{
  float height = mDisplayedTextView.GetHeightForWidth( width );

  if( mEditModeActive && ( fabsf( height ) < Math::MACHINE_EPSILON_1000 ) )
  {
    // If the height is zero, it means there is no text. Let's return the cursor height.
    height = mLineHeight;
  }

  return height;
}

/*end of Virtual methods from parent*/

// Private Internal methods

void TextInput::OnHandlePan(Actor actor, const PanGesture& gesture)
{
  switch (gesture.state)
  {
    case Gesture::Started:
    // fall through so code not duplicated
    case Gesture::Continuing:
    {
      if (actor == mGrabArea)
      {
        SetCursorVisibility( true );
        ShowGrabHandle( mGrabHandleVisibility && mIsGrabHandleInScrollArea );
        MoveGrabHandle( gesture.displacement );
        HidePopup(); // Do not show popup whilst handle is moving
      }
      else if (actor == mHandleOneGrabArea)
      {
        // the displacement in PanGesture is affected by the actor's rotation.
        mSelectionHandleOneActualPosition.x += gesture.displacement.x * mSelectionHandleOne.GetCurrentScale().x;
        mSelectionHandleOneActualPosition.y += gesture.displacement.y * mSelectionHandleOne.GetCurrentScale().y;

        MoveSelectionHandle( HandleOne, gesture.displacement );

        mState = StateDraggingHandle;
        HidePopup();
      }
      else if (actor == mHandleTwoGrabArea)
      {
        // the displacement in PanGesture is affected by the actor's rotation.
        mSelectionHandleTwoActualPosition.x += gesture.displacement.x * mSelectionHandleTwo.GetCurrentScale().x;
        mSelectionHandleTwoActualPosition.y += gesture.displacement.y * mSelectionHandleTwo.GetCurrentScale().y;

        MoveSelectionHandle( HandleTwo, gesture.displacement );

        mState = StateDraggingHandle;
        HidePopup();
      }
    }
    break;

    case Gesture::Finished:
    {
      // Revert back to non-pressed selection handle images
      if (actor == mGrabArea)
      {
        mActualGrabHandlePosition = MoveGrabHandle( gesture.displacement );
        SetCursorVisibility( true );
        SetUpPopupSelection();
        ShowPopup();
      }
      if (actor == mHandleOneGrabArea)
      {
        // the displacement in PanGesture is affected by the actor's rotation.
        mSelectionHandleOneActualPosition.x += gesture.displacement.x * mSelectionHandleOne.GetCurrentScale().x;
        mSelectionHandleOneActualPosition.y += gesture.displacement.y * mSelectionHandleOne.GetCurrentScale().y;

        mSelectionHandleOneActualPosition = MoveSelectionHandle( HandleOne, gesture.displacement );

        mSelectionHandleOne.SetImage( mSelectionHandleOneImage );
        mState = StateEdit;
        ShowPopupCutCopyPaste();
      }
      if (actor == mHandleTwoGrabArea)
      {
        // the displacement in PanGesture is affected by the actor's rotation.
        mSelectionHandleTwoActualPosition.x += gesture.displacement.x * mSelectionHandleTwo.GetCurrentScale().x;
        mSelectionHandleTwoActualPosition.y += gesture.displacement.y * mSelectionHandleTwo.GetCurrentScale().y;

        mSelectionHandleTwoActualPosition = MoveSelectionHandle( HandleTwo, gesture.displacement );

        mSelectionHandleTwo.SetImage( mSelectionHandleTwoImage );
        mState = StateEdit;
        ShowPopupCutCopyPaste();
      }
    }
    break;
    default:
      break;
  }
}

// Stop the flashing animation so easy to see when moved.
bool TextInput::OnPressDown(Dali::Actor actor, const TouchEvent& touch)
{
  if (touch.GetPoint(0).state == TouchPoint::Down)
  {
    SetCursorVisibility( true );
    StopCursorBlinkTimer();
  }
  else if (touch.GetPoint(0).state == TouchPoint::Up)
  {
    SetCursorVisibility( true );
    StartCursorBlinkTimer();
  }
  return false;
}

// selection handle one
bool TextInput::OnHandleOneTouched(Dali::Actor actor, const TouchEvent& touch)
{
  if (touch.GetPoint(0).state == TouchPoint::Down)
  {
    mSelectionHandleOne.SetImage( mSelectionHandleOneImagePressed );
  }
  else if (touch.GetPoint(0).state == TouchPoint::Up)
  {
    mSelectionHandleOne.SetImage( mSelectionHandleOneImage );
  }
  return false;
}

// selection handle two
bool TextInput::OnHandleTwoTouched(Dali::Actor actor, const TouchEvent& touch)
{
  if (touch.GetPoint(0).state == TouchPoint::Down)
  {
    mSelectionHandleTwo.SetImage( mSelectionHandleTwoImagePressed );
  }
  else if (touch.GetPoint(0).state == TouchPoint::Up)
  {
    mSelectionHandleTwo.SetImage( mSelectionHandleTwoImage );
  }
  return false;
}

void TextInput::OnDoubleTap(Dali::Actor actor, const Dali::TapGesture& tap)
{
   // If text exists then select nearest word.
   if ( !mStyledText.empty())
   {
     HidePopup();

     ShowGrabHandleAndSetVisibility( false );


     if ( mPreEditFlag )
     {
       // PreEdit will be committed here without needing a commit from IMF.  Remove pre-edit underline and reset flags which
       // converts the pre-edit word being displayed to a committed word.
       if ( !mUnderlinedPriorToPreEdit )
       {
         TextStyle style;
         style.SetUnderline( false );
         ApplyStyleToRange( style, TextStyle::UNDERLINE , mPreEditStartPosition, mPreEditStartPosition + mPreEditLength -1 );
       }
       mPreEditFlag = false;
       mIgnoreCommitFlag = true; // Predictive word interrupted, text displayed will not change, no need to actually commit.
       // Reset keyboard and set true so cursor position is preserved. Otherwise cursor position will that of the committed text not new tap location.
       PreEditReset( false );
     }
     mCursorPosition = 0;

     mTextLayoutInfo.mScrollOffset = mDisplayedTextView.GetScrollPosition();
     ReturnClosestIndex( tap.localPoint, mCursorPosition );

     std::size_t start = 0;
     std::size_t end = 0;
     Dali::Toolkit::Internal::TextProcessor::FindNearestWord( mStyledText, mCursorPosition, start, end );

     mCursorPosition = end; // Ensure cursor is positioned at end of selected word

     ImfManager imfManager = ImfManager::Get();
     if ( imfManager )
     {
       imfManager.SetCursorPosition ( mCursorPosition );
       imfManager.NotifyCursorPosition();
     }

     if ( !mStyledText.at(end-1).mText[0].IsWhiteSpace() )
     {
       SelectText( start, end );
       ShowPopupCutCopyPaste();
     }
     else
     {
       RemoveHighlight( false ); // Remove highlight but do not auto hide popup
       HidePopup( false ); // Hide popup with setting to do auto show.
       SetUpPopupSelection( false ); // Set to false so if nearest word is whitespace it will not show cut button.
       ShowPopup();
     }
   }
   else if ( mClipboard && mClipboard.NumberOfItems() )
   {
     ShowPopupCutCopyPaste();
   }

   // If no text and clipboard empty then do nothing
}

// TODO: Change the function name to be more general.
void TextInput::OnTextTap(Dali::Actor actor, const Dali::TapGesture& tap)
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "OnTap mPreEditFlag[%s] mEditOnTouch[%s] mEditModeActive[%s] ", (mPreEditFlag)?"true":"false"
                                                                                                           , (mEditOnTouch)?"true":"false"
                                                                                                           , (mEditModeActive)?"true":"false");

  if( mHandleOneGrabArea == actor || mHandleTwoGrabArea == actor )
  {
    return;
  }

  if( mGrabArea == actor )
  {
    if( mPopupPanel.GetState() == TextInputPopup::StateHidden || mPopupPanel.GetState() == TextInputPopup::StateHiding )
    {
      SetUpPopupSelection();
      ShowPopup();
    }

    return;
  }

  HidePopup();
  RemoveHighlight();

  mTextLayoutInfo.mScrollOffset = mDisplayedTextView.GetScrollPosition();

  // Initially don't create the grab handle.
  bool createGrabHandle = false;

  if ( !mEditModeActive )
  {
    // update line height before calculate the actual position.
    UpdateLineHeight();

    // Only start edit mode if TextInput configured to edit on touch
    if ( mEditOnTouch )
    {
      // Set the initial cursor position in the tap point.
      ReturnClosestIndex(tap.localPoint, mCursorPosition );
      StartEditMode();
    }
  }
  else
  {
    // Show the keyboard if it was hidden.
    if (!VirtualKeyboard::IsVisible())
    {
      VirtualKeyboard::Show();
    }

    // Reset keyboard as tap event has occurred.
    // Set true so cursor position is preserved. Otherwise cursor position will that of the committed text not new tap location.
    PreEditReset( true );

    GetTextLayoutInfo();

    if( !mTextLayoutInfo.mCharacterLayoutInfoTable.empty() ) // If string empty we do not need a grab handle.
    {
      // As already in edit mode, reposition cursor near tap and show grab handle for cursor, if grab handle not enabled then magnifier will be used instead.

      ReturnClosestIndex(tap.localPoint, mCursorPosition );

      DALI_LOG_INFO( gLogFilter, Debug::General, "mCursorPosition[%u]", mCursorPosition );

      // Notify keyboard so it can 're-capture' word for predictive text.
      // As we have done a reset, is this required, expect IMF keyboard to request this information.
      ImfManager imfManager = ImfManager::Get();
      if ( imfManager )
      {
        imfManager.SetCursorPosition ( mCursorPosition );
        imfManager.NotifyCursorPosition();
      }
      const TextStyle oldInputStyle( mInputStyle );

      mInputStyle = GetStyleAtCursor(); // Inherit style from cursor position

      DrawCursor();

      // Create the grab handle.
      // Grab handle is created later.
      createGrabHandle = true;

      if( oldInputStyle != mInputStyle )
      {
        // Updates the line height accordingly with the input style.
        UpdateLineHeight();

        EmitStyleChangedSignal();
      }
    }
  }

  // Edit mode started after grab handle created to ensure the signal InputStarted is sent last.
  // This is used to ensure if selecting text hides the grab handle then this code is run after grab handle is created,
  // otherwise the Grab handle will be shown when selecting.
  if ( createGrabHandle && IsGrabHandleEnabled() )
  {
    Vector3 altPosition;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValid;  // Alternate cursor validity flag.
    bool directionRTL;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    Vector3 cursorPosition = GetActualPositionFromCharacterPosition( mCursorPosition, directionRTL, altPosition, altPositionValid );

    if( altPositionValid )
    {
      // Check which of the positions is the closest.
      if( fabsf( altPosition.x - tap.localPoint.x ) < fabsf( cursorPosition.x - tap.localPoint.x ) )
      {
        cursorPosition = altPosition;
      }
    }

    CreateGrabHandle();

    mActualGrabHandlePosition.x = cursorPosition.x; // Set grab handle to be at the cursor position
    mActualGrabHandlePosition.y = cursorPosition.y; // Set grab handle to be at the cursor position
    mGrabHandle.SetPosition( mActualGrabHandlePosition + UI_OFFSET );
    ShowGrabHandleAndSetVisibility( mIsGrabHandleInScrollArea );

  }
}

void TextInput::OnLongPress(Dali::Actor actor, const Dali::LongPressGesture& longPress)
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "OnLongPress\n" );

  // Ignore longpress if in selection mode already
  if( mHighlightMeshActor )
  {
    return;
  }

  if(longPress.state == Dali::Gesture::Started)
  {
    // Start edit mode on long press
    if ( !mEditModeActive )
    {
      StartEditMode();
    }

    // If text exists then select nearest word.
    if ( !mStyledText.empty())
    {
      HidePopup();

      ShowGrabHandleAndSetVisibility( false );


      if ( mPreEditFlag )
      {
        // PreEdit will be committed here without needing a commit from IMF.  Remove pre-edit underline and reset flags which
        // converts the pre-edit word being displayed to a committed word.
        if ( !mUnderlinedPriorToPreEdit )
        {
          TextStyle style;
          style.SetUnderline( false );
          ApplyStyleToRange( style, TextStyle::UNDERLINE , mPreEditStartPosition, mPreEditStartPosition + mPreEditLength -1 );
        }
        mPreEditFlag = false;
        mIgnoreCommitFlag = true; // Predictive word interrupted, text displayed will not change, no need to actually commit.
        // Reset keyboard and set true so cursor position is preserved. Otherwise cursor position will that of the committed text not new tap location.
        PreEditReset( false );
      }
      mCursorPosition = 0;

      mTextLayoutInfo.mScrollOffset = mDisplayedTextView.GetScrollPosition();
      ReturnClosestIndex( longPress.localPoint, mCursorPosition );

      std::size_t start = 0;
      std::size_t end = 0;
      Dali::Toolkit::Internal::TextProcessor::FindNearestWord( mStyledText, mCursorPosition, start, end );

      mCursorPosition  = end; // Ensure cursor is positioned at end of selected word

      ImfManager imfManager = ImfManager::Get();
      if ( imfManager )
      {
        imfManager.SetCursorPosition ( mCursorPosition );
        imfManager.NotifyCursorPosition();
      }

      SelectText( start, end );
    }

    // if no text but clipboard has content then show paste option, if no text and clipboard empty then do nothing
    if ( ( mClipboard && mClipboard.NumberOfItems() ) || !mStyledText.empty() )
    {
      ShowPopupCutCopyPaste();
    }
  }
}

void TextInput::OnClipboardTextSelected( ClipboardEventNotifier& notifier )
{
  const Text clipboardText( notifier.GetContent() );
  PasteText( clipboardText );

  SetCursorVisibility( true );
  StartCursorBlinkTimer();

  ShowGrabHandleAndSetVisibility( false );


  HidePopup();
}

bool TextInput::OnPopupButtonPressed( Toolkit::Button button )
{
  mPopupPanel.PressedSignal().Disconnect( this, &TextInput::OnPopupButtonPressed );

  const std::string& name = button.GetName();

  if(name == TextInputPopup::OPTION_SELECT_WORD)
  {
    std::size_t start = 0;
    std::size_t end = 0;
    Dali::Toolkit::Internal::TextProcessor::FindNearestWord( mStyledText, mCursorPosition, start, end );

    SelectText( start, end );
  }
  else if(name == TextInputPopup::OPTION_SELECT_ALL)
  {
    SetCursorVisibility(false);
    StopCursorBlinkTimer();

    std::size_t end = mTextLayoutInfo.mCharacterLayoutInfoTable.size();
    std::size_t start = 0;

    SelectText( start, end );
  }
  else if(name == TextInputPopup::OPTION_CUT)
  {
    bool ret = CopySelectedTextToClipboard();

    if ( ret )
    {
      DeleteHighlightedText( true );
      CursorUpdate();
    }

    SetCursorVisibility( true );
    StartCursorBlinkTimer();

    HidePopup();
  }
  else if(name == TextInputPopup::OPTION_COPY)
  {
    CopySelectedTextToClipboard();

    RemoveHighlight();

    SetCursorVisibility( true );
    StartCursorBlinkTimer();

    HidePopup();
  }
  else if(name == TextInputPopup::OPTION_PASTE)
  {
    const Text retrievedString( mClipboard.GetItem( 0 ) );  // currently can only get first item in clip board, index 0;

    PasteText(retrievedString);

    SetCursorVisibility( true );
    StartCursorBlinkTimer();

    ShowGrabHandleAndSetVisibility( false );

    HidePopup();
  }
  else if(name == TextInputPopup::OPTION_CLIPBOARD)
  {
    // In the case of clipboard being shown we do not want to show updated pop-up after hide animation completes
    // Hence pass the false parameter for signalFinished.
    HidePopup( true, false );
    mClipboard.ShowClipboard();
  }

  return false;
}

bool TextInput::OnCursorBlinkTimerTick()
{
  // Cursor blinking
  mCursor.SetVisible( mCursorVisibility && mIsCursorInScrollArea && mCursorBlinkStatus );
  if ( mCursorRTLEnabled )
  {
    mCursorRTL.SetVisible( mCursorVisibility && mIsCursorInScrollArea && mCursorBlinkStatus );
  }
  mCursorBlinkStatus = !mCursorBlinkStatus;

  return true;
}

void TextInput::OnPopupHideFinished(TextInputPopup& popup)
{
  popup.HideFinishedSignal().Disconnect( this, &TextInput::OnPopupHideFinished );

  // Change Popup menu to Cut/Copy/Paste if text has been selected.
  if(mHighlightMeshActor && mState == StateEdit)
  {
    ShowPopupCutCopyPaste();
  }
}

//FIXME this routine needs to be re-written as it contains too many branches.
bool TextInput::OnKeyDownEvent(const KeyEvent& event)
{
  std::string keyName = event.keyPressedName;
  std::string keyString = event.keyPressed;

  DALI_LOG_INFO(gLogFilter, Debug::General, "OnKeyDownEvent keyName[%s] KeyString[%s]\n", keyName.c_str(), keyString.c_str() );

  // Do not consume "Tab" and "Escape" keys.
  if(keyName == "Tab" || keyName == "Escape")
  {
    // Escape key to end the edit mode
    EndEditMode();

    return false;
  }

  HidePopup(); // If Pop-up shown then hides it as editing text.

  // Update Flag, indicates whether to update the text-input contents or not.
  // Any key stroke that results in a visual change of the text-input should
  // set this flag to true.
  bool update(false);

  // Whether to scroll text to cursor position.
  // Scroll is needed always the cursor is updated and after the pre-edit is received.
  bool scroll = false;

  if (keyName == "Return")
  {
    if ( mNumberOflinesLimit > 1) // Prevents New line character / Return adding an extra line if limit set to 1
    {
      bool preEditFlagPreviouslySet( mPreEditFlag );

      // replaces highlighted text with new line
      DeleteHighlightedText( false );

      mCursorPosition = mCursorPosition + InsertAt( Text( NEWLINE ), mCursorPosition, 0 );

      // If we are in pre-edit mode then pressing enter will cause a commit.  But the commit string does not include the
      // '\n' character so we need to ensure that the immediately following commit knows how it occurred.
      if ( mPreEditFlag )
      {
        mCommitByKeyInput = true;
      }

      // If attempting to insert a new-line brings us out of PreEdit mode, then we should not ignore the next commit.
      if ( preEditFlagPreviouslySet && !mPreEditFlag )
      {
        mPreEditFlag = true;
        mIgnoreCommitFlag = false;
      }
      EmitTextModified();
      update = true;
    }
    else
    {
      RemoveHighlight();
    }
  } // Return
  else if ( keyName == "space" )
  {
    if ( mHighlightMeshActor )
    {
      // Some text is selected so erase it before adding space.
      DeleteHighlightedText( true );
    }

    mCursorPosition = mCursorPosition + InsertAt(Text(keyString), mCursorPosition, 0);

    // If we are in pre-edit mode then pressing the space-bar will cause a commit.  But the commit string does not include the
    // ' ' character so we need to ensure that the immediately following commit knows how it occurred.
    if ( mPreEditFlag )
    {
      mCommitByKeyInput = true;
    }
    EmitTextModified();
    update = true;
  } // space
  else if (keyName == "BackSpace")
  {
    if ( mHighlightMeshActor )
    {
      // Some text is selected so erase it
      DeleteHighlightedText( true );
      update = true;
    }
    else
    {
      if ( mCursorPosition > 0 )
      {
        DeleteCharacter( mCursorPosition );
        update = true;
      }
    }
    EmitTextModified();
  } // BackSpace
  else if (keyName == "Right")
  {
    AdvanceCursor();
    RemoveHighlight();
  }
  else if (keyName == "Left")
  {
    AdvanceCursor(true);
    RemoveHighlight();
  }
  else // event is a character
  {
    // Some text may be selected, hiding keyboard causes an empty keystring to be sent, we don't want to delete highlight in this case
    if ( !keyString.empty() )
    {
      // replaces highlighted text with new character
      DeleteHighlightedText( false );

      // Received key String
      mCursorPosition += InsertAt( Text( keyString ), mCursorPosition, 0 );
      update = true;
      EmitTextModified();
    }
  }

  // If key event has resulted in a change in the text/cursor, then trigger a relayout of text
  // as this is a costly operation.
  if(update)
  {
    CursorUpdate();
  }

  if(update || scroll)
  {
    if( IsScrollEnabled() )
    {
      // Calculates the new cursor position (in actor coordinates)
      const Vector3 cursorPosition = GetActualPositionFromCharacterPosition( mCursorPosition );

      ScrollTextViewToMakeCursorVisible( cursorPosition );
    }
  }

  return true;
}

bool TextInput::OnKeyUpEvent(const KeyEvent& event)
{
  std::string keyName = event.keyPressedName;
  std::string keyString = event.keyPressed;

  DALI_LOG_INFO(gLogFilter, Debug::General, "OnKeyUpEvent keyName[%s] KeyString[%s]\n", keyName.c_str(), keyString.c_str() );

  // The selected text become deselected when the key code is DALI_KEY_BACK.
  if( IsTextSelected() && ( keyName == "XF86Stop" || keyName == "XF86Send") )
  {
    DeSelectText();
    return true;
  }

  return false;
}

void TextInput::ChooseRtlSelectionHandlePosition( const Vector3& cursorPositionOne,
                                                  const Vector3& cursorPositionTwo,
                                                  bool altPositionValidOne,
                                                  bool altPositionValidTwo,
                                                  const Vector3& altPositionOne,
                                                  const Vector3& altPositionTwo )
{
  // TODO VCC Valid for one line.
  // Try to place the selection handles. TODO think in something better. Probably need to know the direction of the paragraph.
  if( cursorPositionOne != cursorPositionTwo )
  {
    if( cursorPositionOne.x < cursorPositionTwo.x )
    {
      mSelectionHandleOneActualPosition = cursorPositionOne;
      mSelectionHandleTwoActualPosition = cursorPositionTwo;
    }
    else
    {
      mSelectionHandleOneActualPosition = cursorPositionTwo;
      mSelectionHandleTwoActualPosition = cursorPositionOne;
    }
  }
  else
  {
    mSelectionHandleOneActualPosition = cursorPositionOne;
    if( altPositionValidOne )
    {
      if( altPositionOne.x < mSelectionHandleOneActualPosition.x )
      {
        mSelectionHandleOneActualPosition = altPositionOne;
      }
    }
    if( altPositionValidTwo )
    {
      if(  altPositionTwo.x < mSelectionHandleOneActualPosition.x )
      {
        mSelectionHandleOneActualPosition = altPositionTwo;
      }
    }

    mSelectionHandleTwoActualPosition = cursorPositionTwo;
    if( altPositionValidTwo )
    {
      if(  altPositionTwo.x > mSelectionHandleTwoActualPosition.x )
      {
        mSelectionHandleTwoActualPosition = altPositionTwo;
      }
    }
    if( altPositionValidOne )
    {
      if( altPositionOne.x > mSelectionHandleTwoActualPosition.x )
      {
        mSelectionHandleTwoActualPosition = altPositionOne;
      }
    }
  }
}

void TextInput::OnTextViewScrolled( Toolkit::TextView textView, Vector2 scrollPosition )
{
  // Updates the stored scroll position.
  mTextLayoutInfo.mScrollOffset = textView.GetScrollPosition();

  const Vector3& controlSize = GetControlSize();
  Size cursorSize( CURSOR_THICKNESS, 0.f );

  // Updates the cursor and grab handle position and visibility.
  if( mGrabHandle || mCursor )
  {
    cursorSize.height = GetRowRectFromCharacterPosition( mCursorPosition ).height;

    Vector3 altPosition;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValid;  // Alternate cursor validity flag.
    bool directionRTL;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    Vector3 cursorPosition = GetActualPositionFromCharacterPosition( mCursorPosition, directionRTL, altPosition, altPositionValid );

    if( altPositionValid )
    {
      // Check which of the positions is the closest.
      if( fabsf( altPosition.x - mActualGrabHandlePosition.x ) < fabsf( cursorPosition.x - mActualGrabHandlePosition.x ) )
      {
        cursorPosition = altPosition;
      }
    }

    mIsCursorInScrollArea = mIsGrabHandleInScrollArea = IsPositionInsideBoundaries( cursorPosition, cursorSize, controlSize );

    mActualGrabHandlePosition = cursorPosition.GetVectorXY();

    if( mGrabHandle )
    {
      ShowGrabHandle( mGrabHandleVisibility && mIsGrabHandleInScrollArea );
      mGrabHandle.SetPosition( mActualGrabHandlePosition + UI_OFFSET );
    }

    if( mCursor )
    {
      mCursor.SetVisible( mCursorVisibility && mIsCursorInScrollArea );
      mCursor.SetPosition( mActualGrabHandlePosition + UI_OFFSET );
    }
  }

  // Updates the selection handles and highlighted text position and visibility.
  if( mSelectionHandleOne && mSelectionHandleTwo )
  {
    Vector3 altPositionOne;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValidOne;  // Alternate cursor validity flag.
    bool directionRTLOne;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    Vector3 cursorPositionOne = GetActualPositionFromCharacterPosition( mSelectionHandleOnePosition, directionRTLOne, altPositionOne, altPositionValidOne );

    Vector3 altPositionTwo;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValidTwo;  // Alternate cursor validity flag.
    bool directionRTLTwo;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    Vector3 cursorPositionTwo = GetActualPositionFromCharacterPosition( mSelectionHandleTwoPosition, directionRTLTwo, altPositionTwo, altPositionValidTwo );

    // VCC TODO: This method is a hack for one line.
    ChooseRtlSelectionHandlePosition( cursorPositionOne,
                                      cursorPositionTwo,
                                      altPositionValidOne,
                                      altPositionValidTwo,
                                      altPositionOne,
                                      altPositionTwo );

    cursorSize.height = ( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + mSelectionHandleOnePosition ) ).mSize.height;
    const bool isSelectionHandleOneVisible = IsPositionInsideBoundaries( cursorPositionOne, cursorSize, controlSize );
    cursorSize.height = ( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + mSelectionHandleTwoPosition ) ).mSize.height;
    const bool isSelectionHandleTwoVisible = IsPositionInsideBoundaries( cursorPositionTwo, cursorSize, controlSize );

    mSelectionHandleOne.SetVisible( isSelectionHandleOneVisible );
    mSelectionHandleTwo.SetVisible( isSelectionHandleTwoVisible );
    mSelectionHandleOne.SetPosition( mSelectionHandleOneActualPosition + UI_OFFSET + mSelectionHandleOneOffset );
    mSelectionHandleTwo.SetPosition( mSelectionHandleTwoActualPosition + UI_OFFSET + mSelectionHandleTwoOffset );

    if( mHighlightMeshActor )
    {
      mHighlightMeshActor.SetVisible( true );
      UpdateHighlight();
    }
  }
}

void TextInput::ScrollTextViewToMakeCursorVisible( const Vector3& cursorPosition )
{
  // Scroll the text to make the cursor visible.
  const Size cursorSize( CURSOR_THICKNESS,
                         GetRowRectFromCharacterPosition( mCursorPosition ).height );

  // Need to scroll the text to make the cursor visible and to cover the whole text-input area.

  const Vector3& controlSize = GetControlSize();

  // Calculates the new scroll position.
  Vector2 scrollOffset = mTextLayoutInfo.mScrollOffset;
  if( ( cursorPosition.x < 0.f ) || ( cursorPosition.x > controlSize.width ) )
  {
    scrollOffset.x += cursorPosition.x;
  }

  if( cursorPosition.y - cursorSize.height < 0.f || cursorPosition.y > controlSize.height )
  {
    scrollOffset.y += cursorPosition.y;
  }

  // Sets the new scroll position.
  SetScrollPosition( Vector2::ZERO ); // TODO: need to reset to the zero position in order to make the scroll trim to work.
  SetScrollPosition( scrollOffset );
}

void TextInput::StartScrollTimer()
{
  if( !mScrollTimer )
  {
    mScrollTimer = Timer::New( SCROLL_TICK_INTERVAL );
    mScrollTimer.TickSignal().Connect( this, &TextInput::OnScrollTimerTick );
  }

  if( !mScrollTimer.IsRunning() )
  {
    mScrollTimer.Start();
  }
}

void TextInput::StopScrollTimer()
{
  if( mScrollTimer )
  {
    mScrollTimer.Stop();
  }
}

bool TextInput::OnScrollTimerTick()
{
  // TODO: need to set the new style accordingly the new handle position.

  if( !( mGrabHandleVisibility && mGrabHandle ) && !( mSelectionHandleOne && mSelectionHandleTwo ) )
  {
    // nothing to do if all handles are invisible or doesn't exist.
    return true;
  }

  // Text scrolling

  // Choose between the grab handle or the selection handles.
  Vector3& actualHandlePosition = ( mGrabHandleVisibility && mGrabHandle ) ? mActualGrabHandlePosition : ( mCurrentSelectionId == HandleOne ) ? mSelectionHandleOneActualPosition : mSelectionHandleTwoActualPosition;
  std::size_t& handlePosition = ( mGrabHandleVisibility && mGrabHandle ) ? mCursorPosition : ( mCurrentSelectionId == HandleOne ) ? mSelectionHandleOnePosition : mSelectionHandleTwoPosition;
  Vector3& currentHandlePosition = ( mGrabHandleVisibility && mGrabHandle ) ? mCurrentHandlePosition : mCurrentSelectionHandlePosition;

  std::size_t newCursorPosition = 0;
  ReturnClosestIndex( actualHandlePosition.GetVectorXY(), newCursorPosition );

  // Whether the handle's position is different of the previous one and in the case of the selection handle,
  // the new selection handle's position needs to be different of the other one.
  const bool differentSelectionHandles = ( mGrabHandleVisibility && mGrabHandle ) ? newCursorPosition != handlePosition :
                                         ( mCurrentSelectionId == HandleOne ) ? ( newCursorPosition != handlePosition ) && ( newCursorPosition != mSelectionHandleTwoPosition ) :
                                                                                ( newCursorPosition != handlePosition ) && ( newCursorPosition != mSelectionHandleOnePosition );

  if( differentSelectionHandles )
  {
    handlePosition = newCursorPosition;

    const Vector3 actualPosition = GetActualPositionFromCharacterPosition( newCursorPosition );

    Vector2 scrollDelta = ( actualPosition - currentHandlePosition ).GetVectorXY();

    Vector2 scrollPosition = mDisplayedTextView.GetScrollPosition();
    scrollPosition += scrollDelta;
    SetScrollPosition( scrollPosition );

    if( mDisplayedTextView.IsScrollPositionTrimmed() )
    {
      StopScrollTimer();
    }

    currentHandlePosition = GetActualPositionFromCharacterPosition( newCursorPosition ).GetVectorXY();
  }

  actualHandlePosition.x += mScrollDisplacement.x;
  actualHandlePosition.y += mScrollDisplacement.y;

  return true;
}

// Public Internal Methods (public for testing purpose)

void TextInput::SetUpTouchEvents()
{
  if ( !mTapDetector )
  {
    mTapDetector = TapGestureDetector::New();
    // Attach the actors and connect the signal
    mTapDetector.Attach(Self());

    // As contains children which may register for tap the default control detector is not used.
    mTapDetector.DetectedSignal().Connect(this, &TextInput::OnTextTap);
  }

  if ( !mDoubleTapDetector )
  {
    mDoubleTapDetector = TapGestureDetector::New();
    mDoubleTapDetector.SetTapsRequired( 2 );
    mDoubleTapDetector.DetectedSignal().Connect(this, &TextInput::OnDoubleTap);

    // Only attach and detach the actor to the double tap detector when we enter/leave edit mode
    // so that we do not, unnecessarily, have a double tap request all the time
  }

  if ( !mPanGestureDetector )
  {
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &TextInput::OnHandlePan);
  }

  if ( !mLongPressDetector )
  {
    mLongPressDetector = LongPressGestureDetector::New();
    mLongPressDetector.DetectedSignal().Connect(this, &TextInput::OnLongPress);
    mLongPressDetector.Attach(Self());
  }
}

void TextInput::CreateTextViewActor()
{
  mDisplayedTextView = Toolkit::TextView::New();
  mDisplayedTextView.SetName( "DisplayedTextView ");
  mDisplayedTextView.SetMarkupProcessingEnabled( mMarkUpEnabled );
  mDisplayedTextView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mDisplayedTextView.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mDisplayedTextView.SetMultilinePolicy(Toolkit::TextView::SplitByWord);
  mDisplayedTextView.SetWidthExceedPolicy( Toolkit::TextView::Original );
  mDisplayedTextView.SetHeightExceedPolicy( Toolkit::TextView::Original );
  mDisplayedTextView.SetLineJustification( Toolkit::TextView::Left );
  mDisplayedTextView.SetTextAlignment( static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalLeft | Toolkit::Alignment::VerticalTop ) );
  mDisplayedTextView.SetPosition( Vector3( 0.0f, 0.0f, DISPLAYED_TEXT_VIEW_Z_OFFSET ) );
  mDisplayedTextView.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );

  mDisplayedTextView.ScrolledSignal().Connect( this, &TextInput::OnTextViewScrolled );

  Self().Add( mDisplayedTextView );
}

// Start a timer to initiate, used by the cursor to blink.
void TextInput::StartCursorBlinkTimer()
{
  if ( !mCursorBlinkTimer )
  {
    mCursorBlinkTimer = Timer::New( CURSOR_BLINK_INTERVAL );
    mCursorBlinkTimer.TickSignal().Connect( this, &TextInput::OnCursorBlinkTimerTick );
  }

  if ( !mCursorBlinkTimer.IsRunning() )
  {
    mCursorBlinkTimer.Start();
  }
}

// Start a timer to initiate, used by the cursor to blink.
void TextInput::StopCursorBlinkTimer()
{
  if ( mCursorBlinkTimer )
  {
    mCursorBlinkTimer.Stop();
  }
}

void TextInput::StartEditMode()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextInput StartEditMode mEditModeActive[%s]\n", (mEditModeActive)?"true":"false" );

  if(!mEditModeActive)
  {
    SetKeyInputFocus();
  }

  if ( mDoubleTapDetector )
  {
    mDoubleTapDetector.Attach( Self() );
  }
}

void TextInput::EndEditMode()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "TextInput EndEditMode mEditModeActive[%s]\n", (mEditModeActive)?"true":"false" );

  ClearKeyInputFocus();

  if ( mDoubleTapDetector )
  {
    mDoubleTapDetector.Detach( Self() );
  }
}

void TextInput::ApplyPreEditStyle( std::size_t preEditStartPosition, std::size_t preEditStringLength )
{
  if ( mPreEditFlag && ( preEditStringLength > 0 ) )
  {
    mUnderlinedPriorToPreEdit = mInputStyle.IsUnderlineEnabled();
    TextStyle style;
    style.SetUnderline( true );
    ApplyStyleToRange( style, TextStyle::UNDERLINE , preEditStartPosition, preEditStartPosition + preEditStringLength -1 );
  }
}

void TextInput::RemovePreEditStyle()
{
  if ( !mUnderlinedPriorToPreEdit )
  {
    TextStyle style;
    style.SetUnderline( false );
    SetActiveStyle( style, TextStyle::UNDERLINE );
  }
}

// IMF related methods


ImfManager::ImfCallbackData TextInput::ImfEventReceived( Dali::ImfManager& imfManager, const ImfManager::ImfEventData&  imfEvent )
{
  bool update( false );
  bool preeditResetRequired ( false );

  if (imfEvent.eventName != ImfManager::GETSURROUNDING )
  {
    HidePopup(); // If Pop-up shown then hides it as editing text.
  }

  switch ( imfEvent.eventName )
  {
    case ImfManager::PREEDIT:
    {
      mIgnoreFirstCommitFlag = false;

      // Some text may be selected, hiding keyboard causes an empty predictive string to be sent, we don't want to delete highlight in this case
      if ( mHighlightMeshActor &&  (!imfEvent.predictiveString.empty()) )
      {
        // replaces highlighted text with new character
        DeleteHighlightedText( false );
      }

      preeditResetRequired = PreEditReceived( imfEvent.predictiveString, imfEvent.cursorOffset );

      if( IsScrollEnabled() )
      {
        // Calculates the new cursor position (in actor coordinates)
        const Vector3 cursorPosition = GetActualPositionFromCharacterPosition( mCursorPosition );
        ScrollTextViewToMakeCursorVisible( cursorPosition );
      }

      update = true;

      break;
    }
    case ImfManager::COMMIT:
    {
      if( mIgnoreFirstCommitFlag )
      {
        // Do not commit in this case when keyboard sends a commit when shows for the first time (work-around for imf keyboard).
        mIgnoreFirstCommitFlag = false;
      }
      else
      {
        // A Commit message is a word that has been accepted, it may have been a pre-edit word previously but now commited.

        // Some text may be selected, hiding keyboard causes an empty predictive string to be sent, we don't want to delete highlight in this case
        if ( mHighlightMeshActor &&  (!imfEvent.predictiveString.empty()) )
        {
          // replaces highlighted text with new character
          DeleteHighlightedText( false );
        }

       // A PreEditReset can cause a commit message to be sent, the Ignore Commit flag is used in scenarios where the word is
       // not needed, one such scenario is when the pre-edit word is too long to fit.
       if ( !mIgnoreCommitFlag )
       {
         update = CommitReceived( imfEvent.predictiveString );
       }
       else
       {
         mIgnoreCommitFlag = false; // reset ignore flag so next commit is acted upon.
       }
      }

      if( update )
      {
        if( IsScrollEnabled() )
        {
          // Calculates the new cursor position (in actor coordinates)
          const Vector3 cursorPosition = GetActualPositionFromCharacterPosition( mCursorPosition );

          ScrollTextViewToMakeCursorVisible( cursorPosition );
        }
      }
      break;
    }
    case ImfManager::DELETESURROUNDING:
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "ImfEventReceived - delete surrounding mPreEditFlag[%s] cursor offset[%d] characters to delete[%d] position to delete[%u] \n",
                     (mPreEditFlag)?"true":"false", imfEvent.cursorOffset, imfEvent.numberOfChars, static_cast<std::size_t>( mCursorPosition+imfEvent.cursorOffset) );

      mPreEditFlag = false;

      std::size_t toDelete = 0;
      std::size_t numberOfCharacters = 0;

      if( mHighlightMeshActor )
      {
        // delete highlighted text.
        toDelete = std::min( mSelectionHandleOnePosition, mSelectionHandleTwoPosition );
        numberOfCharacters = std::max( mSelectionHandleOnePosition, mSelectionHandleTwoPosition ) - toDelete;
      }
      else
      {
        if( static_cast<std::size_t>(std::abs( imfEvent.cursorOffset )) < mCursorPosition )
        {
          toDelete = mCursorPosition + imfEvent.cursorOffset;
        }
        if( toDelete + imfEvent.numberOfChars > mStyledText.size() )
        {
          numberOfCharacters = mStyledText.size() - toDelete;
        }
        else
        {
          numberOfCharacters = imfEvent.numberOfChars;
        }
      }
      DALI_LOG_INFO( gLogFilter, Debug::General, "ImfEventReceived - deleteSurrounding pre-delete range mCursorPosition[%u] \n", mCursorPosition);
      DeleteRange( toDelete, numberOfCharacters );

      mCursorPosition = toDelete;
      mNumberOfSurroundingCharactersDeleted = numberOfCharacters;

      EmitTextModified();

      DALI_LOG_INFO( gLogFilter, Debug::General, "ImfEventReceived - deleteSurrounding post-delete range mCursorPosition[%u] \n", mCursorPosition);
      break;
    }
    case ImfManager::GETSURROUNDING:
    {
      // If text is selected/highlighted and surrounding text received we do not want the keyboard to store the word at cursor and return it as a predictive word along with
      // the next key pressed.  Instead the Select function sets the cursor position and surrounding text.
      if (! ( mHighlightMeshActor || mSelectingText ) )
      {
        std::string text( GetText() );
        DALI_LOG_INFO( gLogFilter, Debug::General, "OnKey - surrounding text - set text [%s] and cursor[%u] \n", text.c_str(), mCursorPosition );

        imfManager.SetCursorPosition( mCursorPosition );
        imfManager.SetSurroundingText( text );
      }

      if( 0 != mNumberOfSurroundingCharactersDeleted )
      {
        mDisplayedTextView.RemoveTextFrom( mCursorPosition, mNumberOfSurroundingCharactersDeleted );
        mNumberOfSurroundingCharactersDeleted = 0;

        if( mStyledText.empty() )
        {
          ShowPlaceholderText( mStyledPlaceHolderText );
        }
      }
      break;
    }
    case ImfManager::VOID:
    {
      DALI_ASSERT_DEBUG( false );
    }
  } // end switch

  ImfManager::ImfCallbackData callbackData( update, mCursorPosition, GetText(), preeditResetRequired );

  return callbackData;
}

bool TextInput::PreEditReceived(const std::string& keyString, std::size_t cursorOffset )
{
  mPreserveCursorPosition = false;  // As in pre-edit state we should have the cursor at the end of the word displayed not last touch position.

  DALI_LOG_INFO(gLogFilter, Debug::General, ">>PreEditReceived preserveCursorPos[%d] mCursorPos[%d] mPreEditFlag[%d]\n",
                mPreserveCursorPosition, mCursorPosition, mPreEditFlag );

  bool preeditResetRequest ( false );

  if( mPreEditFlag ) // Already in pre-edit state.
  {
    if( mStyledText.size() >= mMaxStringLength )
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "PreEditReceived styledTextSize >= mMaxStringLength \n");
      // Cannot fit these characters into field, clear pre-edit.
      if ( !mUnderlinedPriorToPreEdit )
      {
        TextStyle style;
        style.SetUnderline( false );
        ApplyStyleToRange( style, TextStyle::UNDERLINE , mPreEditStartPosition, mPreEditStartPosition + mPreEditLength -1 );
      }
      mIgnoreCommitFlag = true;
      preeditResetRequest = false; // this will reset the keyboard's predictive suggestions.
      mPreEditFlag = false;
      EmitMaxInputCharactersReachedSignal();
    }
    else
    {
      // delete existing pre-edit string
      const std::size_t numberOfCharactersToReplace = DeletePreEdit();

      // Store new pre-edit string
      mPreEditString.SetText( keyString );

      if ( keyString.empty() )
      {
        mPreEditFlag = false;
        mCursorPosition = mPreEditStartPosition;

        if( mStyledText.empty() )
        {
          ShowPlaceholderText( mStyledPlaceHolderText );
        }
        else
        {
          mDisplayedTextView.RemoveTextFrom( mPreEditStartPosition, numberOfCharactersToReplace );
        }

        GetTextLayoutInfo();
        EmitTextModified();
      }
      else
      {
        // Insert new pre-edit string. InsertAt updates the size and position table.
        mPreEditLength = InsertAt( mPreEditString, mPreEditStartPosition, numberOfCharactersToReplace );
        // If word was too long to be inserted then cursorOffset would be out of range as keyboard assumes there is not limit. Hence use of std::min.
        mCursorPosition = mPreEditStartPosition + std::min( cursorOffset, mPreEditLength );
        ApplyPreEditStyle( mPreEditStartPosition, mPreEditLength );
        DALI_LOG_INFO(gLogFilter, Debug::General, "PreEditReceived mCursorPosition[%u] \n", mCursorPosition);
        EmitTextModified();
      }
      // cursor update to keyboard is not done here as the keyboard knows the cursor position and provides the 'cursorOffset'.
      DrawCursor();
    }
  }
  else  // mPreEditFlag not set
  {
    if ( !keyString.empty() ) // Imf can send an empty pre-edit followed by Backspace instead of a commit.
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "PreEditReceived Initial Pre-Edit string \n");
      // new pre-edit so move into pre-edit state by setting flag
      mPreEditFlag = true;
      mPreEditString.SetText( keyString ); // store new pre-edit string
      mPreEditStartPosition = mCursorPosition; // store starting cursor position of pre-edit so know where to re-start from
      mPreEditLength = InsertAt( mPreEditString, mPreEditStartPosition, 0 );
      // If word was too long to be inserted then cursorOffset would be out of range as keyboard assumes there is not limit. Hence use of std::min.
      mCursorPosition = mPreEditStartPosition + std::min( cursorOffset, mPreEditLength );
      ApplyPreEditStyle( mPreEditStartPosition, mPreEditLength );
      DALI_LOG_INFO(gLogFilter, Debug::General, "PreEditReceived mCursorPosition[%u] mPreEditStartPosition[%u]\n", mCursorPosition, mPreEditStartPosition);
      // cursor update to keyboard is not done here as the keyboard knows the cursor position and provides the 'cursorOffset'.
      DrawCursor();
      EmitTextModified();
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "PreEditReceived with empty keyString\n");
    }
  }

  return preeditResetRequest;
}

bool TextInput::CommitReceived(const std::string& keyString )
{
  DALI_LOG_INFO(gLogFilter, Debug::General, ">>CommitReceived preserveCursorPos[%d] mPreEditStartPosition [%d] mCursorPos[%d] mPreEditFlag[%d] mIgnoreCommitFlag[%s]\n",
      mPreserveCursorPosition, mPreEditStartPosition, mCursorPosition, mPreEditFlag, (mIgnoreCommitFlag)?"true":"false" );

  bool update( false );

  RemovePreEditStyle();

  const std::size_t styledTextSize( mStyledText.size() );
  if( styledTextSize >= mMaxStringLength )
  {
    // Cannot fit these characters into field, clear pre-edit.
    if ( mPreEditFlag )
    {
      mIgnoreCommitFlag = true;
      mPreEditFlag = false;
    }
    EmitMaxInputCharactersReachedSignal();
  }
  else
  {
    if( mPreEditFlag )
    {
      // delete existing pre-edit string
      const std::size_t numberOfCharactersToReplace = DeletePreEdit();
      mPreEditFlag = false;

      DALI_LOG_INFO(gLogFilter, Debug::General, "CommitReceived mPreserveCursorPosition[%s] mPreEditStartPosition[%u]\n",
                    (mPreserveCursorPosition)?"true":"false", mPreEditStartPosition );

      if ( mPreserveCursorPosition ) // PreEditReset has been called triggering this commit.
      {
        // No need to update cursor position as Cursor location given by touch.
        InsertAt( Text( keyString ), mPreEditStartPosition, numberOfCharactersToReplace );
        mPreserveCursorPosition = false;
      }
      else
      {
        // Cursor not set by touch so needs to be re-positioned to input more text
        mCursorPosition = mPreEditStartPosition + InsertAt( Text(keyString), mPreEditStartPosition, numberOfCharactersToReplace ); // update cursor position as InsertAt, re-draw cursor with this

        // If a space or enter caused the commit then our string is one longer than the string given to us by the commit key.
        if ( mCommitByKeyInput )
        {
          mCursorPosition = std::min ( mCursorPosition + 1, mStyledText.size() );
          mCommitByKeyInput = false;
        }
      }

      EmitTextModified();

      if ( mSelectTextOnCommit )
      {
        SelectText(mRequestedSelection.mStartOfSelection, mRequestedSelection.mEndOfSelection );
      }

      update = true;
    }
    else // mPreEditFlag not set
    {
      if ( !mIgnoreCommitFlag ) // Check if this commit should be ignored.
      {
        if( mStyledText.empty() && mPlaceHolderSet )
        {
          // If the styled text is empty and the placeholder text is set, it needs to be cleared.
          mDisplayedTextView.SetText( "" );
          mNumberOfSurroundingCharactersDeleted = 0;
          mPlaceHolderSet = false;
        }
        mCursorPosition = mCursorPosition + InsertAt( Text( keyString ), mCursorPosition, mNumberOfSurroundingCharactersDeleted );
        update = true;
        mNumberOfSurroundingCharactersDeleted = 0;
        EmitTextModified();
      }
      else
      {
        mIgnoreCommitFlag = false; // Reset flag so future commits will not be ignored.
      }
    }
  }

  mSelectTextOnCommit = false;

  DALI_LOG_INFO(gLogFilter, Debug::General, "CommitReceived << mCursorPos[%d] mPreEditFlag[%d] update[%s] \n",
                                             mCursorPosition, mPreEditFlag, (update)?"true":"false" );

  return update;
}

// End of IMF related methods

std::size_t TextInput::DeletePreEdit()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, ">>DeletePreEdit mPreEditFlag[%s] \n", (mPreEditFlag)?"true":"false");

  DALI_ASSERT_DEBUG( mPreEditFlag );

  const std::size_t preEditStringLength = mPreEditString.GetLength();
  const std::size_t styledTextSize = mStyledText.size();

  std::size_t endPosition = mPreEditStartPosition + preEditStringLength;

  // Prevents erase items outside mStyledText bounds.
  if( mPreEditStartPosition > styledTextSize )
  {
    DALI_ASSERT_DEBUG( !"TextInput::DeletePreEdit. mPreEditStartPosition > mStyledText.size()" );
    mPreEditStartPosition = styledTextSize;
  }

  if( ( endPosition > styledTextSize ) || ( endPosition < mPreEditStartPosition ) )
  {
    DALI_ASSERT_DEBUG( !"TextInput::DeletePreEdit. ( endPosition > mStyledText.size() ) || ( endPosition < mPreEditStartPosition )" );
    endPosition = styledTextSize;
  }

  mStyledText.erase( mStyledText.begin() + mPreEditStartPosition, mStyledText.begin() + endPosition );

  // DeletePreEdit() doesn't remove characters from the text-view because may be followed by an InsertAt() which inserts characters,
  // in that case, the Insert should use the returned number of deleted characters and replace the text which helps the text-view to
  // reuse glyphs.
  // In case DeletePreEdit() is not followed by an InsertAt() characters must be deleted after this call.

  return preEditStringLength;
}

void TextInput::PreEditReset( bool preserveCursorPosition )
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "PreEditReset preserveCursorPos[%d] mCursorPos[%d] \n",
                preserveCursorPosition, mCursorPosition);

  // Store flag to indicate that we do not want to lose the cursor position as the reset may have occurred due to touch event moving the cursor.
  mPreserveCursorPosition = preserveCursorPosition;

  // Reset incase we are in a pre-edit state.
  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    imfManager.Reset(); // Will trigger a commit message
  }
}

void TextInput::CursorUpdate()
{
  DrawCursor();

  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    std::string text( GetText() );
    imfManager.SetSurroundingText( text );  // Notifying IMF of a cursor change triggers a surrounding text request so updating it now.
    imfManager.SetCursorPosition ( mCursorPosition );
    imfManager.NotifyCursorPosition();
  }
}

/* Delete highlighted characters redisplay*/
void TextInput::DeleteHighlightedText( bool inheritStyle )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "DeleteHighlightedText handlePosOne[%u] handlePosTwo[%u]\n", mSelectionHandleOnePosition, mSelectionHandleTwoPosition);

  if( mHighlightMeshActor )
  {
    mCursorPosition = std::min( mSelectionHandleOnePosition, mSelectionHandleTwoPosition );

    MarkupProcessor::StyledTextArray::iterator start = mStyledText.begin() + mCursorPosition;
    MarkupProcessor::StyledTextArray::iterator end =  mStyledText.begin() + std::max( mSelectionHandleOnePosition, mSelectionHandleTwoPosition );

    // Get the styled text of the characters to be deleted as it may be needed if
    // the "exceed the text-input's boundaries" option is disabled.
    MarkupProcessor::StyledTextArray styledCharactersToDelete;

    styledCharactersToDelete.insert( styledCharactersToDelete.begin(), start, end );

    mStyledText.erase( start, end ); // erase range of characters

    // Remove text from TextView and update place holder text if required

    // Set the placeholder text only if the styled text is empty.
    if( mStyledText.empty() )
    {
      ShowPlaceholderText( mStyledPlaceHolderText );
    }
    else
    {
      const std::size_t numberOfCharacters = std::max( mSelectionHandleOnePosition, mSelectionHandleTwoPosition ) - mCursorPosition;

      mDisplayedTextView.RemoveTextFrom( mCursorPosition, numberOfCharacters );

      // It may happen than after removing a white space or a new line character,
      // two words merge, this new word could be big enough to not fit in its
      // current line, so moved to the next one, and make some part of the text to
      // exceed the text-input's boundary.
      if( !mExceedEnabled )
      {
        // Get the new text layout after removing some characters.
        mDisplayedTextView.GetTextLayoutInfo( mTextLayoutInfo );

        // Get text-input's size.
        const Vector3& size = GetControlSize();

        if( ( mTextLayoutInfo.mTextSize.width > size.width ) ||
            ( mTextLayoutInfo.mTextSize.height > size.height ) )
        {
          mDisplayedTextView.InsertTextAt( mCursorPosition, styledCharactersToDelete );

          mStyledText.insert( mStyledText.begin() + mCursorPosition,
                              styledCharactersToDelete.begin(),
                              styledCharactersToDelete.end() );
        }
      }
    }
    GetTextLayoutInfo();

    RemoveHighlight();

    EmitTextModified();

    if( inheritStyle )
    {
      const TextStyle oldInputStyle( mInputStyle );

      mInputStyle = GetStyleAtCursor(); // Inherit style from cursor position

      if( oldInputStyle != mInputStyle )
      {
        // Updates the line height accordingly with the input style.
        UpdateLineHeight();

        EmitStyleChangedSignal();
      }
    }
  }
}

void TextInput::DeleteRange( const std::size_t start, const std::size_t ncharacters )
{
  DALI_ASSERT_DEBUG( start <= mStyledText.size() );
  DALI_ASSERT_DEBUG( !mStyledText.empty() );

  DALI_LOG_INFO(gLogFilter, Debug::General, ">>DeleteRange pre mStyledText[%s] mPreEditFlag[%s] \n", GetText().c_str(), (mPreEditFlag)?"true":"false");


  if ( ( !mStyledText.empty()) && ( ( start + ncharacters ) <= mStyledText.size() ) )
  {
    MarkupProcessor::StyledTextArray::iterator itStart =  mStyledText.begin() + start;
    MarkupProcessor::StyledTextArray::iterator itEnd =  mStyledText.begin() + start + ncharacters;

    mStyledText.erase(itStart, itEnd);

    // update the selection handles if they are visible.
    if( mHighlightMeshActor )
    {
      std::size_t& minHandle = ( mSelectionHandleOnePosition <= mSelectionHandleTwoPosition ? mSelectionHandleOnePosition : mSelectionHandleTwoPosition );
      std::size_t& maxHandle = ( mSelectionHandleTwoPosition > mSelectionHandleOnePosition ? mSelectionHandleTwoPosition : mSelectionHandleOnePosition );

      if( minHandle >= start + ncharacters )
      {
        minHandle -= ncharacters;
      }
      else if( ( minHandle > start ) && ( minHandle < start + ncharacters ) )
      {
        minHandle = start;
      }

      if( maxHandle >= start + ncharacters )
      {
        maxHandle -= ncharacters;
      }
      else if( ( maxHandle > start ) && ( maxHandle < start + ncharacters ) )
      {
        maxHandle = start;
      }
    }

    // Set text is not called here as currently it can not process the set text from deletion and then the set text from the in-coming pre-edit.
  }

  DALI_LOG_INFO(gLogFilter, Debug::General, "DeleteRange<< post mStyledText[%s] mPreEditFlag[%s] \n", GetText().c_str(), (mPreEditFlag)?"true":"false");

  // Although mStyledText has been set to a new text string we no longer re-draw the text or notify the cursor change.
  // This is a performance decision as the use of this function often means the text is being replaced or just deleted.
  // Mean we do not re-draw the text more than we have too.
}

/* Delete character at current cursor position and redisplay*/
void TextInput::DeleteCharacter( std::size_t positionToDelete )
{
  // Ensure positionToDelete is not out of bounds.
  DALI_ASSERT_DEBUG( positionToDelete <= mStyledText.size() );
  DALI_ASSERT_DEBUG( !mStyledText.empty() );
  DALI_ASSERT_DEBUG( positionToDelete > 0 );

  DALI_LOG_INFO(gLogFilter, Debug::General, "DeleteCharacter positionToDelete[%u]", positionToDelete );


  if ( ( !mStyledText.empty()) && ( positionToDelete > 0 ) && positionToDelete <= mStyledText.size() )  // don't try to delete if no characters left of cursor
  {
    MarkupProcessor::StyledTextArray::iterator it =  mStyledText.begin() + positionToDelete - 1;

    // Get the styled text of the character to be deleted as it may be needed if
    // the "exceed the text-input's boundaries" option is disabled.
    const MarkupProcessor::StyledText styledCharacterToDelete( *it );

    mStyledText.erase(it);  // erase the character left of positionToDelete

    if( mStyledText.empty() )
    {
      ShowPlaceholderText( mStyledPlaceHolderText );
    }
    else
    {
      mDisplayedTextView.RemoveTextFrom( positionToDelete - 1, 1 );

      const Character characterToDelete = styledCharacterToDelete.mText[0];

      // It may happen than after removing a white space or a new line character,
      // two words merge, this new word could be big enough to not fit in its
      // current line, so moved to the next one, and make some part of the text to
      // exceed the text-input's boundary.
      if( !mExceedEnabled )
      {
        if( characterToDelete.IsWhiteSpace() || characterToDelete.IsNewLine() )
        {
          // Get the new text layout after removing one character.
          mDisplayedTextView.GetTextLayoutInfo( mTextLayoutInfo );

          // Get text-input's size.
          const Vector3& size = GetControlSize();

          if( ( mTextLayoutInfo.mTextSize.width > size.width ) ||
              ( mTextLayoutInfo.mTextSize.height > size.height ) )
          {
            MarkupProcessor::StyledTextArray array;
            array.push_back( styledCharacterToDelete );
            mDisplayedTextView.InsertTextAt( positionToDelete - 1, array );

            mStyledText.insert( mStyledText.begin() + ( positionToDelete - 1 ), styledCharacterToDelete );
          }
        }
      }
    }
    GetTextLayoutInfo();

    ShowGrabHandleAndSetVisibility( false );

    mCursorPosition = positionToDelete -1;

    const TextStyle oldInputStyle( mInputStyle );

    mInputStyle = GetStyleAtCursor(); // Inherit style from cursor position

    if( oldInputStyle != mInputStyle )
    {
      // Updates the line height accordingly with the input style.
      UpdateLineHeight();

      EmitStyleChangedSignal();
    }
  }
}

/*Insert new character into the string and (optionally) redisplay text-input*/
std::size_t TextInput::InsertAt( const Text& newText, const std::size_t insertionPosition, const std::size_t numberOfCharactersToReplace )
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "InsertAt insertionPosition[%u]\n", insertionPosition );

  // Ensure insertionPosition is not out of bounds.
  DALI_ASSERT_ALWAYS( insertionPosition <= mStyledText.size() );

  bool textExceedsMaximunNumberOfCharacters = false;
  bool textExceedsBoundary = false;
  std::size_t insertedStringLength = DoInsertAt( newText, insertionPosition, numberOfCharactersToReplace, textExceedsMaximunNumberOfCharacters, textExceedsBoundary );

  ShowGrabHandleAndSetVisibility( false );

  if( textExceedsMaximunNumberOfCharacters || textExceedsBoundary )
  {
    if( mPreEditFlag )
    {
      mIgnoreCommitFlag = true;
      mPreEditFlag = false;
      // A PreEditReset( false ) should be triggered from here if the keyboards predictive suggestions must be cleared.
      // Although can not directly call PreEditReset() as it will cause a recursive emit loop.
    }

    if( textExceedsMaximunNumberOfCharacters )
    {
      EmitMaxInputCharactersReachedSignal();
    }

    if( textExceedsBoundary )
    {
      EmitInputTextExceedsBoundariesSignal();
      PreEditReset( false );
    }
  }

  return insertedStringLength;
}

ImageActor TextInput::CreateCursor( const Vector4& color)
{
  ImageActor cursor;
  cursor = CreateSolidColorActor(color);
  cursor.SetName( "Cursor" );

  cursor.SetParentOrigin(ParentOrigin::TOP_LEFT);
  cursor.SetAnchorPoint(AnchorPoint::BOTTOM_LEFT);
  cursor.SetVisible(false);

  return cursor;
}

void TextInput::AdvanceCursor(bool reverse, std::size_t places)
{
  // As cursor is not moving due to grab handle, handle should be hidden.
  ShowGrabHandleAndSetVisibility( false );

  bool cursorPositionChanged = false;
  if (reverse)
  {
    if ( mCursorPosition >= places )
    {
      mCursorPosition = mCursorPosition - places;
      cursorPositionChanged = true;
    }
  }
  else
  {
    if ((mCursorPosition + places) <= mStyledText.size())
    {
      mCursorPosition = mCursorPosition + places;
      cursorPositionChanged = true;
    }
  }

  if( cursorPositionChanged )
  {
    const std::size_t cursorPositionForStyle = ( 0 == mCursorPosition ? 0 : mCursorPosition - 1 );

    const TextStyle oldInputStyle( mInputStyle );
    mInputStyle = GetStyleAt( cursorPositionForStyle ); // Inherit style from selected position.

    DrawCursor();

    if( oldInputStyle != mInputStyle )
    {
      // Updates the line height accordingly with the input style.
      UpdateLineHeight();

      EmitStyleChangedSignal();
    }

    ImfManager imfManager = ImfManager::Get();
    if ( imfManager )
    {
      imfManager.SetCursorPosition ( mCursorPosition );
      imfManager.NotifyCursorPosition();
    }
  }
}

void TextInput::DrawCursor()
{
  const Size rowRect = GetRowRectFromCharacterPosition( mCursorPosition );

  // Get height of cursor and set its size
  Size size( CURSOR_THICKNESS, 0.0f );
  if( !mTextLayoutInfo.mCharacterLayoutInfoTable.empty() )
  {
    size.height = rowRect.height;
  }
  else
  {
    // Measure Font so know how big text will be if no initial text to measure.
    size.height = mLineHeight;
  }

  mCursor.SetSize(size);

  // If the character is italic then the cursor also tilts.
  mCursor.SetRotation( mInputStyle.IsItalicsEnabled() ? Degree( mInputStyle.GetItalicsAngle() - CURSOR_ANGLE_OFFSET ) : Degree( 0.f ), Vector3::ZAXIS );

  DALI_ASSERT_DEBUG( mCursorPosition <= mTextLayoutInfo.mCharacterLayoutInfoTable.size() );

  if( ( mCursorPosition <= mTextLayoutInfo.mCharacterLayoutInfoTable.size() ) )
  {
    Vector3 altPosition;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValid;  // Alternate cursor validity flag.
    bool directionRTL;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    Vector3 position = GetActualPositionFromCharacterPosition( mCursorPosition, directionRTL, altPosition, altPositionValid );

    SetAltCursorEnabled( altPositionValid );

    if( !altPositionValid )
    {
      mCursor.SetPosition( position + UI_OFFSET );
    }
    else
    {
      size.height *= 0.5f;
      mCursor.SetSize(size);
      mCursor.SetPosition( position + UI_OFFSET - Vector3( 0.0f, directionRTL ? 0.0f : size.height, 0.0f ) );

      // TODO: change this cursor pos, to be the one where the cursor is sourced from.
      size.height = rowRect.height * 0.5f;
      mCursorRTL.SetSize(size);
      mCursorRTL.SetPosition( altPosition + UI_OFFSET - Vector3( 0.0f, directionRTL ? size.height : 0.0f, 0.0f ) );
    }

    if( IsScrollEnabled() )
    {
      // Whether cursor and grab handle are inside the boundaries of the text-input when text scroll is enabled.
      mIsCursorInScrollArea = mIsGrabHandleInScrollArea = IsPositionInsideBoundaries( position, size, GetControlSize() );
    }
  } // EditMode
}

void TextInput::SetAltCursorEnabled( bool enabled )
{
  mCursorRTLEnabled = enabled;
  mCursorRTL.SetVisible( mCursorVisibility && mCursorRTLEnabled );
}

void TextInput::SetCursorVisibility( bool visible )
{
  mCursorVisibility = visible;
  mCursor.SetVisible( mCursorVisibility && mIsCursorInScrollArea );
  mCursorRTL.SetVisible( mCursorVisibility && mCursorRTLEnabled );
}

void TextInput::CreateGrabHandle( Dali::Image image )
{
  if ( !mGrabHandle )
  {
    if ( !image )
    {
      mGrabHandleImage = ResourceImage::New(DEFAULT_GRAB_HANDLE);
    }
    else
    {
      mGrabHandleImage = image;
    }

    mGrabHandle = ImageActor::New(mGrabHandleImage);
    mGrabHandle.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mGrabHandle.SetAnchorPoint(AnchorPoint::TOP_CENTER);

    mGrabHandle.SetDrawMode(DrawMode::OVERLAY);

    ShowGrabHandleAndSetVisibility( false );

    CreateGrabArea( mGrabHandle );

    mActiveLayer.Add(mGrabHandle);
  }
}

void TextInput::CreateGrabArea( Actor& parent )
{
  mGrabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
  mGrabArea.SetName( "GrabArea" );
  mGrabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );
  mGrabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
  mGrabArea.SetSizeModeFactor( DEFAULT_GRAB_HANDLE_RELATIVE_SIZE );
  mGrabArea.TouchedSignal().Connect(this,&TextInput::OnPressDown);
  mTapDetector.Attach( mGrabArea );
  mPanGestureDetector.Attach( mGrabArea );
  mLongPressDetector.Attach( mGrabArea );

  parent.Add(mGrabArea);
}

Vector3 TextInput::MoveGrabHandle( const Vector2& displacement )
{
  Vector3 actualHandlePosition;

  if (mGrabHandle)
  {
    mActualGrabHandlePosition.x += displacement.x;
    mActualGrabHandlePosition.y += displacement.y;

    // Grab handle should jump to the nearest character and take cursor with it
    std::size_t newCursorPosition = 0;
    ReturnClosestIndex( mActualGrabHandlePosition.GetVectorXY(), newCursorPosition );

    Vector3 altPosition;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValid;  // Alternate cursor validity flag.
    bool directionRTL;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    actualHandlePosition = GetActualPositionFromCharacterPosition( newCursorPosition, directionRTL, altPosition, altPositionValid );

    if( altPositionValid )
    {
      // Check which of the positions is the closest.
      if( fabsf( altPosition.x - mActualGrabHandlePosition.x ) < fabsf( actualHandlePosition.x - mActualGrabHandlePosition.x ) )
      {
        actualHandlePosition = altPosition;
      }
    }

    bool handleVisible = true;

    if( IsScrollEnabled() )
    {
      const Vector3 controlSize = GetControlSize();
      const Size cursorSize = GetRowRectFromCharacterPosition( newCursorPosition );
      // Scrolls the text if the handle is not in a visible position
      handleVisible = IsPositionInsideBoundaries( actualHandlePosition,
                                                  cursorSize,
                                                  controlSize );

      if( handleVisible )
      {
        StopScrollTimer();
        mCurrentHandlePosition = actualHandlePosition;
        mScrollDisplacement = Vector2::ZERO;
      }
      else
      {
        if( ( actualHandlePosition.x < SCROLL_THRESHOLD ) && ( displacement.x <= 0.f ) )
        {
          mScrollDisplacement.x = -SCROLL_SPEED;
        }
        else if( ( actualHandlePosition.x > controlSize.width - SCROLL_THRESHOLD ) && ( displacement.x >= 0.f ) )
        {
          mScrollDisplacement.x = SCROLL_SPEED;
        }
        if( ( actualHandlePosition.y < SCROLL_THRESHOLD ) && ( displacement.y <= 0.f ) )
        {
          mScrollDisplacement.y = -SCROLL_SPEED;
        }
        else if( ( actualHandlePosition.y > controlSize.height - SCROLL_THRESHOLD ) && ( displacement.y >= 0.f ) )
        {
          mScrollDisplacement.y = SCROLL_SPEED;
        }
        StartScrollTimer();
      }
    }

    if( handleVisible &&                           // Only redraw cursor and do updates if position changed
        ( newCursorPosition != mCursorPosition ) ) // and the new position is visible (if scroll is not enabled, it's always true).
    {
      mCursorPosition = newCursorPosition;

      mGrabHandle.SetPosition( actualHandlePosition + UI_OFFSET );

      const TextStyle oldInputStyle( mInputStyle );

      mInputStyle = GetStyleAtCursor(); //Inherit style from cursor position

      CursorUpdate();  // Let keyboard know the new cursor position so can 're-capture' for prediction.

      if( oldInputStyle != mInputStyle )
      {
        // Updates the line height accordingly with the input style.
        UpdateLineHeight();

        EmitStyleChangedSignal();
      }
    }
  }

  return actualHandlePosition;
}

void TextInput::ShowGrabHandle( bool visible )
{
  if ( IsGrabHandleEnabled() )
  {
    if( mGrabHandle )
    {
      mGrabHandle.SetVisible( mGrabHandleVisibility );
    }
    StartMonitoringStageForTouch();
  }
}

void TextInput::ShowGrabHandleAndSetVisibility( bool visible )
{
  mGrabHandleVisibility = visible;
  ShowGrabHandle( visible );
}

// Callbacks connected to be Property notifications for Boundary checking.

void TextInput::OnLeftBoundaryExceeded(PropertyNotification& source)
{
  mIsSelectionHandleOneFlipped = true;
  mSelectionHandleOne.SetScale( -1.0f, 1.0f, 1.0f );
  mSelectionHandleOne.SetAnchorPoint( AnchorPoint::TOP_LEFT);
}

void TextInput::OnReturnToLeftBoundary(PropertyNotification& source)
{
  mIsSelectionHandleOneFlipped = false;
  mSelectionHandleOne.SetScale( 1.0f, 1.0f, 1.0f );
  mSelectionHandleOne.SetAnchorPoint( AnchorPoint::TOP_RIGHT);
}

void TextInput::OnRightBoundaryExceeded(PropertyNotification& source)
{
  mIsSelectionHandleTwoFlipped = true;
  mSelectionHandleTwo.SetScale( -1.0f, 1.0f, 1.0f );
  mSelectionHandleTwo.SetAnchorPoint( AnchorPoint::TOP_RIGHT);
}

void TextInput::OnReturnToRightBoundary(PropertyNotification& source)
{
  mIsSelectionHandleTwoFlipped = false;
  mSelectionHandleTwo.SetScale( 1.0f, 1.0f, 1.0f );
  mSelectionHandleTwo.SetAnchorPoint( AnchorPoint::TOP_LEFT);
}

// todo change PropertyNotification signal definition to include Actor. Hence won't need duplicate functions.
void TextInput::OnHandleOneLeavesBoundary( PropertyNotification& source)
{
  mSelectionHandleOne.SetOpacity(0.0f);
}

void TextInput::OnHandleOneWithinBoundary(PropertyNotification& source)
{
  mSelectionHandleOne.SetOpacity(1.0f);
}

void TextInput::OnHandleTwoLeavesBoundary( PropertyNotification& source)
{
  mSelectionHandleTwo.SetOpacity(0.0f);
}

void TextInput::OnHandleTwoWithinBoundary(PropertyNotification& source)
{
  mSelectionHandleTwo.SetOpacity(1.0f);
}

// End of Callbacks connected to be Property notifications for Boundary checking.

void TextInput::SetUpHandlePropertyNotifications()
{
  /* Property notifications for handles exceeding the boundary and returning back within boundary */

  Vector3 handlesize = GetSelectionHandleSize();

  // Exceeding horizontal boundary
  PropertyNotification leftNotification = mSelectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_X, LessThanCondition( mBoundingRectangleWorldCoordinates.x + handlesize.x) );
  leftNotification.NotifySignal().Connect( this, &TextInput::OnLeftBoundaryExceeded );

  PropertyNotification rightNotification = mSelectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_X, GreaterThanCondition( mBoundingRectangleWorldCoordinates.z - handlesize.x ) );
  rightNotification.NotifySignal().Connect( this, &TextInput::OnRightBoundaryExceeded );

  // Within horizontal boundary
  PropertyNotification leftLeaveNotification = mSelectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_X, GreaterThanCondition( mBoundingRectangleWorldCoordinates.x + 2*handlesize.x ) );
  leftLeaveNotification.NotifySignal().Connect( this, &TextInput::OnReturnToLeftBoundary );

  PropertyNotification rightLeaveNotification = mSelectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_X, LessThanCondition( mBoundingRectangleWorldCoordinates.z - 2*handlesize.x ) );
  rightLeaveNotification.NotifySignal().Connect( this, &TextInput::OnReturnToRightBoundary );

  // Exceeding vertical boundary
  PropertyNotification verticalExceedNotificationOne = mSelectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       OutsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                         mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalExceedNotificationOne.NotifySignal().Connect( this, &TextInput::OnHandleOneLeavesBoundary );

  PropertyNotification verticalExceedNotificationTwo = mSelectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       OutsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                         mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalExceedNotificationTwo.NotifySignal().Connect( this, &TextInput::OnHandleTwoLeavesBoundary );

  // Within vertical boundary
  PropertyNotification verticalWithinNotificationOne = mSelectionHandleOne.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       InsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                        mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalWithinNotificationOne.NotifySignal().Connect( this, &TextInput::OnHandleOneWithinBoundary );

  PropertyNotification verticalWithinNotificationTwo = mSelectionHandleTwo.AddPropertyNotification( Actor::WORLD_POSITION_Y,
                                                       InsideCondition( mBoundingRectangleWorldCoordinates.y + handlesize.y,
                                                                        mBoundingRectangleWorldCoordinates.w - handlesize.y ) );
  verticalWithinNotificationTwo.NotifySignal().Connect( this, &TextInput::OnHandleTwoWithinBoundary );
}

void TextInput::CreateSelectionHandles( std::size_t start, std::size_t end, Dali::Image handleOneImage,  Dali::Image handleTwoImage )
{
  mSelectionHandleOnePosition = start;
  mSelectionHandleTwoPosition = end;

  if ( !mSelectionHandleOne )
  {
    // create normal and pressed images
    mSelectionHandleOneImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_ONE );
    mSelectionHandleOneImagePressed = ResourceImage::New( DEFAULT_SELECTION_HANDLE_ONE_PRESSED );

    mSelectionHandleOne = ImageActor::New( mSelectionHandleOneImage );
    mSelectionHandleOne.SetName("SelectionHandleOne");
    mSelectionHandleOne.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mSelectionHandleOne.SetAnchorPoint( AnchorPoint::TOP_RIGHT ); // Change to BOTTOM_RIGHT if Look'n'Feel requires handle above text.
    mIsSelectionHandleOneFlipped = false;
    mSelectionHandleOne.SetDrawMode( DrawMode::OVERLAY ); // ensure grab handle above text

    mHandleOneGrabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
    mHandleOneGrabArea.SetName("SelectionHandleOneGrabArea");

    mHandleOneGrabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
    mHandleOneGrabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
    mHandleOneGrabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

    mTapDetector.Attach( mHandleOneGrabArea );
    mPanGestureDetector.Attach( mHandleOneGrabArea );

    mHandleOneGrabArea.TouchedSignal().Connect(this,&TextInput::OnHandleOneTouched);

    mSelectionHandleOne.Add( mHandleOneGrabArea );
    mActiveLayer.Add( mSelectionHandleOne );
  }

  if ( !mSelectionHandleTwo )
  {
    // create normal and pressed images
    mSelectionHandleTwoImage = ResourceImage::New( DEFAULT_SELECTION_HANDLE_TWO );
    mSelectionHandleTwoImagePressed = ResourceImage::New( DEFAULT_SELECTION_HANDLE_TWO_PRESSED );

    mSelectionHandleTwo = ImageActor::New( mSelectionHandleTwoImage );
    mSelectionHandleTwo.SetName("SelectionHandleTwo");
    mSelectionHandleTwo.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mSelectionHandleTwo.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mIsSelectionHandleTwoFlipped = false;
    mSelectionHandleTwo.SetDrawMode(DrawMode::OVERLAY); // ensure grab handle above text

    mHandleTwoGrabArea = Actor::New(); // Area that Grab handle responds to, larger than actual handle so easier to move
    mHandleTwoGrabArea.SetName("SelectionHandleTwoGrabArea");
    mHandleTwoGrabArea.SetSizeMode( SIZE_RELATIVE_TO_PARENT );
    mHandleTwoGrabArea.SetSizeModeFactor( DEFAULT_SELECTION_HANDLE_RELATIVE_SIZE );
    mHandleTwoGrabArea.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

    mTapDetector.Attach( mHandleTwoGrabArea );
    mPanGestureDetector.Attach( mHandleTwoGrabArea );

    mHandleTwoGrabArea.TouchedSignal().Connect(this, &TextInput::OnHandleTwoTouched);

    mSelectionHandleTwo.Add( mHandleTwoGrabArea );

    mActiveLayer.Add( mSelectionHandleTwo );
  }

  SetUpHandlePropertyNotifications();

  // update table as text may have changed.
  GetTextLayoutInfo();

  Vector3 altPositionOne;    // Alternate (i.e. opposite direction) cursor position.
  bool altPositionValidOne;  // Alternate cursor validity flag.
  bool directionRTLOne;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
  Vector3 cursorPositionOne = GetActualPositionFromCharacterPosition( mSelectionHandleOnePosition, directionRTLOne, altPositionOne, altPositionValidOne );

  Vector3 altPositionTwo;    // Alternate (i.e. opposite direction) cursor position.
  bool altPositionValidTwo;  // Alternate cursor validity flag.
  bool directionRTLTwo;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
  Vector3 cursorPositionTwo = GetActualPositionFromCharacterPosition( mSelectionHandleTwoPosition, directionRTLTwo, altPositionTwo, altPositionValidTwo );

  // VCC TODO: This method is a hack for one line.
  ChooseRtlSelectionHandlePosition( cursorPositionOne,
                                    cursorPositionTwo,
                                    altPositionValidOne,
                                    altPositionValidTwo,
                                    altPositionOne,
                                    altPositionTwo );

  mSelectionHandleOne.SetPosition( mSelectionHandleOneActualPosition + UI_OFFSET + mSelectionHandleOneOffset );
  mSelectionHandleTwo.SetPosition( mSelectionHandleTwoActualPosition + UI_OFFSET + mSelectionHandleTwoOffset );

  // Calculates and set the visibility if the scroll mode is enabled.
  bool isSelectionHandleOneVisible = true;
  bool isSelectionHandleTwoVisible = true;
  if( IsScrollEnabled() )
  {
    const Vector3& controlSize( GetControlSize() );
    isSelectionHandleOneVisible = IsPositionInsideBoundaries( mSelectionHandleOneActualPosition, Size::ZERO, controlSize );
    isSelectionHandleTwoVisible = IsPositionInsideBoundaries( mSelectionHandleTwoActualPosition, Size::ZERO, controlSize );
    mSelectionHandleOne.SetVisible( isSelectionHandleOneVisible );
    mSelectionHandleTwo.SetVisible( isSelectionHandleTwoVisible );
  }

  CreateHighlight();  // function will only create highlight if not already created.
}

Vector3 TextInput::MoveSelectionHandle( SelectionHandleId handleId, const Vector2& displacement )
{
  Vector3 actualHandlePosition;

  if ( mSelectionHandleOne && mSelectionHandleTwo )
  {
    const Vector3& controlSize = GetControlSize();

    Size cursorSize( CURSOR_THICKNESS, 0.f );

    // Get a reference of the wanted selection handle (handle one or two).
    Vector3& actualSelectionHandlePosition = ( handleId == HandleOne ) ? mSelectionHandleOneActualPosition : mSelectionHandleTwoActualPosition;

    // Get a reference for the current position of the handle and a copy of its pair
    std::size_t& currentSelectionHandlePosition = ( handleId == HandleOne ) ? mSelectionHandleOnePosition : mSelectionHandleTwoPosition;
    const std::size_t pairSelectionHandlePosition = ( handleId == HandleOne ) ? mSelectionHandleTwoPosition : mSelectionHandleOnePosition;

    // Get a handle of the selection handle actor
    ImageActor selectionHandleActor = ( handleId == HandleOne ) ? mSelectionHandleOne : mSelectionHandleTwo;

    // Selection handles should jump to the nearest character
    std::size_t newHandlePosition = 0;
    ReturnClosestIndex( actualSelectionHandlePosition.GetVectorXY(), newHandlePosition );

    Vector3 altPosition;    // Alternate (i.e. opposite direction) cursor position.
    bool altPositionValid;  // Alternate cursor validity flag.
    bool directionRTL;      // Need to know direction of primary cursor (in case we have 2 cursors and need to show them differently)
    actualHandlePosition = GetActualPositionFromCharacterPosition( newHandlePosition, directionRTL, altPosition, altPositionValid );
    if( altPositionValid )
    {
      // Check which of the positions is the closest.
      if( fabsf( altPosition.x - actualSelectionHandlePosition.x ) < fabsf( actualHandlePosition.x - actualSelectionHandlePosition.x ) )
      {
        actualHandlePosition = altPosition;
      }
    }

    bool handleVisible = true;

    if( IsScrollEnabled() )
    {
      mCurrentSelectionId = handleId;

      cursorSize.height = GetRowRectFromCharacterPosition( newHandlePosition ).height;
      // Restricts the movement of the grab handle inside the boundaries of the text-input.
      handleVisible = IsPositionInsideBoundaries( actualHandlePosition,
                                                  cursorSize,
                                                  controlSize );

      if( handleVisible )
      {
        StopScrollTimer();
        mCurrentSelectionHandlePosition = actualHandlePosition;
        mScrollDisplacement = Vector2::ZERO;
      }
      else
      {
        if( ( actualHandlePosition.x < SCROLL_THRESHOLD ) && ( displacement.x <= 0.f ) )
        {
          mScrollDisplacement.x = -SCROLL_SPEED;
        }
        else if( ( actualHandlePosition.x > controlSize.width - SCROLL_THRESHOLD ) && ( displacement.x >= 0.f ) )
        {
          mScrollDisplacement.x = SCROLL_SPEED;
        }
        if( ( actualHandlePosition.y < SCROLL_THRESHOLD ) && ( displacement.y <= 0.f ) )
        {
          mScrollDisplacement.y = -SCROLL_SPEED;
        }
        else if( ( actualHandlePosition.y > controlSize.height - SCROLL_THRESHOLD ) && ( displacement.y >= 0.f ) )
        {
          mScrollDisplacement.y = SCROLL_SPEED;
        }
        StartScrollTimer();
      }
    }

    if ( handleVisible &&                                          // Ensure the handle is visible.
         ( newHandlePosition != pairSelectionHandlePosition ) &&   // Ensure handle one is not the same position as handle two.
         ( newHandlePosition != currentSelectionHandlePosition ) ) // Ensure the handle has moved.
    {
      currentSelectionHandlePosition = newHandlePosition;

      Vector3 selectionHandleOffset = ( handleId == HandleOne ) ? mSelectionHandleOneOffset : mSelectionHandleTwoOffset;
      selectionHandleActor.SetPosition( actualHandlePosition + UI_OFFSET + selectionHandleOffset );

      UpdateHighlight();

      if ( handleId == HandleOne )
      {
        const TextStyle oldInputStyle( mInputStyle );

        // Set Active Style to that of first character in selection
        if( mSelectionHandleOnePosition < mStyledText.size() )
        {
          mInputStyle = ( mStyledText.at( mSelectionHandleOnePosition ) ).mStyle;
        }

        if( oldInputStyle != mInputStyle )
        {
          // Updates the line height accordingly with the input style.
          UpdateLineHeight();

          EmitStyleChangedSignal();
        }
      }
    }
  }

  return actualHandlePosition; // Returns Handle position passed in if new value not assigned.
}

void TextInput::SetSelectionHandlePosition(SelectionHandleId handleId)
{
  const std::size_t selectionHandlePosition = ( handleId == HandleOne ) ? mSelectionHandleOnePosition : mSelectionHandleTwoPosition;
  ImageActor selectionHandleActor = ( handleId == HandleOne ) ? mSelectionHandleOne : mSelectionHandleTwo;

  if ( selectionHandleActor )
  {
    const Vector3 actualHandlePosition = GetActualPositionFromCharacterPosition( selectionHandlePosition );
    Vector3 selectionHandleOffset = ( handleId == HandleOne ) ? mSelectionHandleOneOffset : mSelectionHandleTwoOffset;
    selectionHandleActor.SetPosition( actualHandlePosition + UI_OFFSET + selectionHandleOffset );

    if( IsScrollEnabled() )
    {
      const Size cursorSize( CURSOR_THICKNESS,
                             GetRowRectFromCharacterPosition( selectionHandlePosition ).height );
      selectionHandleActor.SetVisible( IsPositionInsideBoundaries( actualHandlePosition,
                                                                   cursorSize,
                                                                   GetControlSize() ) );
    }
  }
}

void TextInput::GetVisualTextSelection( std::vector<bool>& selectedVisualText, std::size_t startSelection, std::size_t endSelection )
{
  selectedVisualText.resize( mTextLayoutInfo.mCharacterLogicalToVisualMap.size(), false );

  // VCC Set true/false in logical order. TODO : It needs to be checked.

  if( startSelection > endSelection )
  {
    std::swap( startSelection, endSelection );
  }
  std::size_t index = 0u;
  for( std::vector<bool>::iterator it = selectedVisualText.begin(), endIt = selectedVisualText.end(); it != endIt; ++it, ++index )
  {
    if( ( index < startSelection ) || ( endSelection <= index ) )
    {
      *it = false;
    }
    else
    {
      *it = true;
    }
  }
}

// Calculate the dimensions of the quads they will make the highlight mesh
TextInput::HighlightInfo TextInput::CalculateHighlightInfo()
{
  // At the moment there is no public API to modify the block alignment option.

  mNewHighlightInfo.mQuadList.clear(); // clear last quad information.

  if ( !mTextLayoutInfo.mCharacterLayoutInfoTable.empty() && !mTextLayoutInfo.mCharacterLogicalToVisualMap.empty() )
  {
    Toolkit::TextView::CharacterLayoutInfoContainer::iterator it = mTextLayoutInfo.mCharacterLayoutInfoTable.begin();
    Toolkit::TextView::CharacterLayoutInfoContainer::iterator end = mTextLayoutInfo.mCharacterLayoutInfoTable.end();

    // Get vector of flags representing characters that are selected (true) vs unselected (false).
    std::vector<bool> selectedVisualText;
    GetVisualTextSelection( selectedVisualText, mSelectionHandleOnePosition, mSelectionHandleTwoPosition );
    std::vector<bool>::iterator selectedIt = selectedVisualText.begin();
    std::vector<bool>::iterator selectedEndIt = selectedVisualText.end();

    SelectionState selectionState = SelectionNone;          ///< Current selection status of cursor over entire text.
    float rowLeft = 0.0f;
    float rowRight = 0.0f;
    // Keep track of the TextView's min/max extents. Should be able to query this from TextView.
    float maxRowLeft = std::numeric_limits<float>::max();
    float maxRowRight = 0.0f;

    Toolkit::TextView::CharacterLayoutInfoContainer::iterator lastIt = it;

    // Scan through entire text.
    while(it != end)
    {
      // selectionState: None when not in selection, Started when in selection, and Ended when reached end of selection.

      Toolkit::TextView::CharacterLayoutInfo& charInfo(*it);
      bool charSelected = false;
      if( selectedIt != selectedEndIt )
      {
        charSelected = *selectedIt++;
      }

      if( selectionState == SelectionNone )
      {
        if( charSelected )
        {
          selectionState = SelectionStarted;
          rowLeft = charInfo.mPosition.x - mTextLayoutInfo.mScrollOffset.x;
          rowRight = rowLeft + charInfo.mSize.width;
        }
      }
      else if( selectionState == SelectionStarted )
      {
        // break selection on:
        // 1. new line causing selection break. (\n or wordwrap)
        // 2. character not selected.
        if( !charSelected ||
            ( charInfo.mPosition.y - lastIt->mPosition.y > CHARACTER_THRESHOLD ) )
        {
          // finished selection.
          // TODO: TextView should have a table of visual rows, and each character a reference to the row
          // that it resides on. That way this enumeration is not necessary.
          Vector2 min, max;
          if(lastIt->mIsNewParagraphChar)
          {
            // If the last character is a new line, then to get the row rect, we need to scan from the character before the new line.
            lastIt = std::max( mTextLayoutInfo.mCharacterLayoutInfoTable.begin(), lastIt - 1 );
          }
          const Size rowSize( GetRowRectFromCharacterPosition( lastIt - mTextLayoutInfo.mCharacterLayoutInfoTable.begin(), min, max ) );
          maxRowLeft = std::min(maxRowLeft, min.x);
          maxRowRight = std::max(maxRowRight, max.x);
          float rowBottom = lastIt->mPosition.y - mTextLayoutInfo.mScrollOffset.y;
          float rowTop = rowBottom - rowSize.height;

          // Still selected, and block-align mode then set rowRight to max, so it can be clamped afterwards
          if(charSelected)
          {
            rowRight = std::numeric_limits<float>::max();
          }
          mNewHighlightInfo.AddQuad( rowLeft, rowTop, rowRight, rowBottom );

          selectionState = SelectionNone;

          // Still selected? start a new selection
          if( charSelected )
          {
            // if block-align mode then set rowLeft to min, so it can be clamped afterwards
            rowLeft = 0.0f;
            rowRight = ( charInfo.mPosition.x - mTextLayoutInfo.mScrollOffset.x ) + charInfo.mSize.width;
            selectionState = SelectionStarted;
          }
        }
        else
        {
          // build up highlight(s) with this selection data.
          rowLeft = std::min( charInfo.mPosition.x - mTextLayoutInfo.mScrollOffset.x, rowLeft );
          rowRight = std::max( ( charInfo.mPosition.x - mTextLayoutInfo.mScrollOffset.x ) + charInfo.mSize.width, rowRight );
        }
      }

      lastIt = it++;
    }

    // If reached end, and still on selection, then close selection.
    if(it == end)
    {
      if(selectionState == SelectionStarted)
      {
        // finished selection.
        Vector2 min, max;
        if(lastIt->mIsNewParagraphChar)
        {
          lastIt = std::max( mTextLayoutInfo.mCharacterLayoutInfoTable.begin(), lastIt - 1 );
        }
        const Size rowSize( GetRowRectFromCharacterPosition( lastIt - mTextLayoutInfo.mCharacterLayoutInfoTable.begin(), min, max ) );
        maxRowLeft = std::min(maxRowLeft, min.x);
        maxRowRight = std::max(maxRowRight, max.x);
        float rowBottom = lastIt->mPosition.y - mTextLayoutInfo.mScrollOffset.y;
        float rowTop = rowBottom - rowSize.height;
        mNewHighlightInfo.AddQuad( rowLeft, rowTop, rowRight, rowBottom );
      }
    }

    // Get the top left and bottom right corners.
    const Toolkit::TextView::CharacterLayoutInfo& firstCharacter( *mTextLayoutInfo.mCharacterLayoutInfoTable.begin() );
    const Vector2 topLeft( maxRowLeft, firstCharacter.mPosition.y - firstCharacter.mSize.height );
    const Vector2 bottomRight( topLeft.x + mTextLayoutInfo.mTextSize.width, topLeft.y + mTextLayoutInfo.mTextSize.height );

    // Clamp quads so they appear to clip to borders of the whole text.
    mNewHighlightInfo.Clamp2D( topLeft, bottomRight );

    // For block-align align Further Clamp quads to max left and right extents
      // BlockAlign: Will adjust highlight to block:
      // i.e.
      //   H[ello] (top row right = max of all rows right)
      // [--this-] (middle rows' left = min of all rows left, middle rows' right = max of all rows right)
      // [is some] (middle rows' left = min of all rows left, middle rows' right = max of all rows right)
      // [text] (bottom row left = min of all rows left)
      // (common in SMS messaging selection)
      //
      // As opposed to the default which is tight text highlighting.
      //   H[ello]
      //   [this]
      // [is some]
      // [text]
      // (common in regular text editors/web browser selection)
      mNewHighlightInfo.Clamp2D( Vector2(maxRowLeft, topLeft.y), Vector2(maxRowRight, bottomRight.y ) );

    // Finally clamp quads again so they don't exceed the boundry of the control.
    const Vector3& controlSize = GetControlSize();
    mNewHighlightInfo.Clamp2D( Vector2::ZERO, Vector2(controlSize.x, controlSize.y) );
  } // end if

  return mNewHighlightInfo;
}

// VCC TODO: two methods are not needed. this one is a quick hack to fix PLMs. Should implement one which support both directions.
// This method creates one quad per character so different selection boxes for a mix of LTR and RTL languages are created.
TextInput::HighlightInfo TextInput::CalculateHighlightInfoRtl()
{
  // At the moment there is no public API to modify the block alignment option.

  mNewHighlightInfo.mQuadList.clear(); // clear last quad information.

  if ( !mTextLayoutInfo.mCharacterLayoutInfoTable.empty() && !mTextLayoutInfo.mCharacterLogicalToVisualMap.empty() )
  {
    Toolkit::TextView::CharacterLayoutInfoContainer::iterator it = mTextLayoutInfo.mCharacterLayoutInfoTable.begin();
    Toolkit::TextView::CharacterLayoutInfoContainer::iterator end = mTextLayoutInfo.mCharacterLayoutInfoTable.end();

    // Get vector of flags representing characters that are selected (true) vs unselected (false).
    std::vector<bool> selectedVisualText;
    GetVisualTextSelection( selectedVisualText, mSelectionHandleOnePosition, mSelectionHandleTwoPosition );
    std::vector<bool>::iterator selectedIt = selectedVisualText.begin();
    std::vector<bool>::iterator selectedEndIt = selectedVisualText.end();

    // SelectionState selectionState = SelectionNone;          ///< Current selection status of cursor over entire text.
    float rowLeft = 0.0f;
    float rowRight = 0.0f;

    // VCC TODO this is valid for one line.
    Vector2 min, max;
    const Size rowSize = GetRowRectFromCharacterPosition( 0, min, max );

    // Scan through entire text.
    while(it != end)
    {
      // selectionState: None when not in selection, Started when in selection, and Ended when reached end of selection.

      Toolkit::TextView::CharacterLayoutInfo& charInfo(*it);
      bool charSelected = false;
      if( selectedIt != selectedEndIt )
      {
        charSelected = *selectedIt++;
      }

      if( charSelected )
      {
        rowLeft = charInfo.mPosition.x - mTextLayoutInfo.mScrollOffset.x;
        rowRight = rowLeft + charInfo.mSize.width;

        float rowBottom = charInfo.mPosition.y - mTextLayoutInfo.mScrollOffset.y;
        float rowTop = rowBottom - rowSize.height;
        mNewHighlightInfo.AddQuad( rowLeft, rowTop, rowRight, rowBottom );
      }

      ++it;
    }

    // Finally clamp quads again so they don't exceed the boundry of the control.
    const Vector3& controlSize = GetControlSize();
    mNewHighlightInfo.Clamp2D( Vector2::ZERO, Vector2(controlSize.x, controlSize.y) );
  } // end if

  return mNewHighlightInfo;
}

void TextInput::UpdateHighlight()
{
//  Construct a Mesh with a texture to be used as the highlight 'box' for selected text
//
//  Example scenarios where mesh is made from 3, 1, 2, 2 ,3 or 3 quads.
//
//   [ TOP   ]  [ TOP ]      [TOP ]  [ TOP    ]      [ TOP  ]      [ TOP  ]
//  [ MIDDLE ]             [BOTTOM]  [BOTTOM]      [ MIDDLE ]   [ MIDDLE  ]
//  [ BOTTOM]                                      [ MIDDLE ]   [ MIDDLE  ]
//                                                 [BOTTOM]     [ MIDDLE  ]
//                                                              [BOTTOM]
//
//  Each quad is created as 2 triangles.
//  Middle is just 1 quad regardless of its size.
//
//  (0,0)         (0,0)
//     0*    *2     0*       *2
//     TOP          TOP
//     3*    *1     3*       *1
//  4*       *1     4*     *6
//     MIDDLE         BOTTOM
//  6*       *5     7*     *5
//  6*    *8
//   BOTTOM
//  9*    *7
//

  if ( mHighlightMeshActor )
  {
    // vertex and triangle buffers should always be present if MeshActor is alive.
    HighlightInfo newHighlightInfo = CalculateHighlightInfoRtl();
    MeshData::VertexContainer vertices;
    Dali::MeshData::FaceIndices faceIndices;

    if( !newHighlightInfo.mQuadList.empty() )
    {
      std::vector<QuadCoordinates>::iterator iter = newHighlightInfo.mQuadList.begin();
      std::vector<QuadCoordinates>::iterator endIter = newHighlightInfo.mQuadList.end();

      // vertex position defaults to (0 0 0)
      MeshData::Vertex vertex;
      // set normal for all vertices as (0 0 1) pointing outward from TextInput Actor.
      vertex.nZ = 1.0f;

      for(std::size_t v = 0; iter != endIter; ++iter,v+=4 )
      {
        // Add each quad geometry (a sub-selection) to the mesh data.

        // 0-----1
        // |\    |
        // | \ A |
        // |  \  |
        // | B \ |
        // |    \|
        // 2-----3

        QuadCoordinates& quad = *iter;
        // top-left (v+0)
        vertex.x = quad.min.x;
        vertex.y = quad.min.y;
        vertices.push_back( vertex );

        // top-right (v+1)
        vertex.x = quad.max.x;
        vertex.y = quad.min.y;
        vertices.push_back( vertex );

        // bottom-left (v+2)
        vertex.x = quad.min.x;
        vertex.y = quad.max.y;
        vertices.push_back( vertex );

        // bottom-right (v+3)
        vertex.x = quad.max.x;
        vertex.y = quad.max.y;
        vertices.push_back( vertex );

        // triangle A (3, 1, 0)
        faceIndices.push_back( v + 3 );
        faceIndices.push_back( v + 1 );
        faceIndices.push_back( v );

        // triangle B (0, 2, 3)
        faceIndices.push_back( v );
        faceIndices.push_back( v + 2 );
        faceIndices.push_back( v + 3 );

        mMeshData.SetFaceIndices( faceIndices );
      }

      BoneContainer bones(0); // passed empty as bones not required
      mMeshData.SetData( vertices, faceIndices, bones, mCustomMaterial );
      mHighlightMesh.UpdateMeshData(mMeshData);
    }
  }
}

void TextInput::ClearPopup()
{
  mPopupPanel.Clear();
}

void TextInput::AddPopupOptions()
{
  mPopupPanel.AddPopupOptions();
}

void TextInput::SetPopupPosition( const Vector3& position, const Vector2& alternativePosition )
{
  const Vector3& visiblePopUpSize = mPopupPanel.GetVisibileSize();

  Vector3 clampedPosition ( position );
  Vector3 tailOffsetPosition ( position );

  float xOffSet( 0.0f );

  Actor self = Self();
  const Vector3 textViewTopLeftWorldPosition = self.GetCurrentWorldPosition() - self.GetCurrentSize()*0.5f;

  const float popUpLeft = textViewTopLeftWorldPosition.x + position.x - visiblePopUpSize.width*0.5f;
  const float popUpTop = textViewTopLeftWorldPosition.y + position.y - visiblePopUpSize.height;

  // Clamp to left or right or of boundary
  if( popUpLeft < mBoundingRectangleWorldCoordinates.x )
  {
    xOffSet = mBoundingRectangleWorldCoordinates.x - popUpLeft ;
  }
  else if ( popUpLeft + visiblePopUpSize.width > mBoundingRectangleWorldCoordinates.z )
  {
    xOffSet = mBoundingRectangleWorldCoordinates.z - ( popUpLeft + visiblePopUpSize.width );
  }

  clampedPosition.x = position.x + xOffSet;
  tailOffsetPosition.x = -xOffSet;

  // Check if top left of PopUp outside of top bounding rectangle, if so then flip to lower position.
  bool flipTail( false );

  if ( popUpTop < mBoundingRectangleWorldCoordinates.y )
  {
    clampedPosition.y = alternativePosition.y + visiblePopUpSize.height;
    flipTail = true;
  }

  mPopupPanel.GetRootActor().SetPosition( clampedPosition );
  mPopupPanel.SetTailPosition( tailOffsetPosition, flipTail );
}

void TextInput::HidePopup(bool animate, bool signalFinished )
{
  if ( ( mPopupPanel.GetState() == TextInputPopup::StateShowing ) || ( mPopupPanel.GetState() == TextInputPopup::StateShown )  )
  {
    mPopupPanel.Hide( animate );

    if( animate && signalFinished )
    {
      mPopupPanel.HideFinishedSignal().Connect( this, &TextInput::OnPopupHideFinished );
    }
  }
}

void TextInput::ShowPopup( bool animate )
{
  Vector3 position;
  Vector2 alternativePopupPosition;

  if(mHighlightMeshActor && mState == StateEdit)
  {
    Vector3 topHandle;
    Vector3 bottomHandle; // referring to the bottom most point of the handle or the bottom line of selection.
    Size rowSize;
    // When text is selected, show popup above top handle (and text), or below bottom handle.
    // topHandle: referring to the top most point of the handle or the top line of selection.
    if ( mSelectionHandleTwoActualPosition.y > mSelectionHandleOneActualPosition.y )
    {
      topHandle = mSelectionHandleOneActualPosition;
      bottomHandle = mSelectionHandleTwoActualPosition;
      rowSize= GetRowRectFromCharacterPosition( mSelectionHandleOnePosition );
    }
    else
    {
      topHandle = mSelectionHandleTwoActualPosition;
      bottomHandle = mSelectionHandleOneActualPosition;
      rowSize = GetRowRectFromCharacterPosition( mSelectionHandleTwoPosition );
    }
    topHandle.y += -mPopupOffsetFromText.y - rowSize.height;
    position = Vector3(topHandle.x, topHandle.y, 0.0f);

    float xPosition = ( fabsf( topHandle.x - bottomHandle.x ) )*0.5f + std::min( mSelectionHandleOneActualPosition.x , mSelectionHandleTwoActualPosition.x );

    position.x = xPosition;

    // Alternative position if no upper space
    bottomHandle.y += GetSelectionHandleSize().y + mPopupOffsetFromText.w;
    alternativePopupPosition = Vector2 ( position.x, bottomHandle.y );
  }
  else
  {
    // When no text is selected, show popup at world position of grab handle or cursor
    position = GetActualPositionFromCharacterPosition( mCursorPosition );
    const Size rowSize = GetRowRectFromCharacterPosition( mCursorPosition );
    position.y -= ( mPopupOffsetFromText.y + rowSize.height );
    // if can't be positioned above, then position below row.
    alternativePopupPosition = Vector2( position.x, position.y ); // default if no grab handle
    if ( mGrabHandle )
    {
      // If grab handle enabled then position pop-up below the grab handle.
      alternativePopupPosition.y = rowSize.height + mGrabHandle.GetCurrentSize().height + mPopupOffsetFromText.w +50.0f;
    }
  }

  SetPopupPosition( position, alternativePopupPosition );

  // Show popup
  mPopupPanel.Show( Self(), animate );
  StartMonitoringStageForTouch();

  mPopupPanel.PressedSignal().Connect( this, &TextInput::OnPopupButtonPressed );
}

void TextInput::ShowPopupCutCopyPaste()
{
  ClearPopup();

  mPopupPanel.CreateOrderedListOfOptions(); // todo Move this so only run when order has changed
  // Check the selected text is whole text or not.
  if( IsTextSelected() && ( mStyledText.size() != GetSelectedText().size() ) )
  {
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsSelectAll, true );
  }

  if ( !mStyledText.empty() && IsTextSelected() )
  {
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsCopy, true );
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsCut, true );
  }

  if( mClipboard && mClipboard.NumberOfItems() )
  {
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsPaste, true );
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsClipboard, true );
  }

  AddPopupOptions();

  mPopupPanel.Hide(false);
  ShowPopup();
}

void TextInput::SetUpPopupSelection( bool showCutButton )
{
  ClearPopup();
  mPopupPanel.CreateOrderedListOfOptions(); // todo Move this so only run when order has changed
  // If no text exists then don't offer to select
  if ( !mStyledText.empty() )
  {
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsSelectAll, true );
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsSelect, true );
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsCut, ( showCutButton && IsTextSelected() ) );
  }
  // if clipboard has valid contents then offer paste option
  if( mClipboard && mClipboard.NumberOfItems() )
  {
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsPaste, true );
    mPopupPanel.TogglePopupButtonOnOff( TextInputPopup::ButtonsClipboard, true );
  }

  AddPopupOptions();

  mPopupPanel.Hide(false);
}

bool TextInput::ReturnClosestIndex(const Vector2& source, std::size_t& closestIndex )
{
  bool found = false;
  closestIndex = 0;

  std::vector<Toolkit::TextView::CharacterLayoutInfo> matchedCharacters;
  bool lastRightToLeftChar(false);          /// RTL state of previous character encountered (character on the left of touch point)
  bool rightToLeftChar(false);              /// RTL state of current character encountered (character on the right of touch point)
  float glyphIntersection(0.0f);            /// Glyph intersection, the point between the two nearest characters touched.

  const Vector2 sourceScrollOffset( source + mTextLayoutInfo.mScrollOffset );

  if ( !mTextLayoutInfo.mCharacterLayoutInfoTable.empty() )
  {
    float closestYdifference = std::numeric_limits<float>::max();
    std::size_t lineOffset = 0;                  /// Keep track of position of the first character on the matched line of interest.
    std::size_t numberOfMatchedCharacters = 0;

    // 1. Find closest character line to y part of source, create vector of all entries in that Y position
    // TODO: There should be an easy call to enumerate through each visual line, instead of each character on all visual lines.

    for( std::vector<Toolkit::TextView::CharacterLayoutInfo>::const_iterator it = mTextLayoutInfo.mCharacterLayoutInfoTable.begin(), endIt = mTextLayoutInfo.mCharacterLayoutInfoTable.end(); it != endIt; ++it )
    {
      const Toolkit::TextView::CharacterLayoutInfo& info( *it );
      float baselinePosition = info.mPosition.y - info.mDescender;

      if( info.mIsVisible )
      {
        // store difference between source y point and the y position of the current character
        float currentYdifference = fabsf( sourceScrollOffset.y - ( baselinePosition ) );

        if(  currentYdifference < closestYdifference  )
        {
          // closest so far; store this difference and clear previous matchedCharacters as no longer closest
          lineOffset = it - mTextLayoutInfo.mCharacterLayoutInfoTable.begin();
          closestYdifference = currentYdifference;
          matchedCharacters.clear();
          numberOfMatchedCharacters = 0; // reset count
        }

        // add all characters that are on the same Y axis (within the CHARACTER_THRESHOLD) to the matched array.
        if( fabsf( closestYdifference - currentYdifference )  < CHARACTER_THRESHOLD )
        {
          // ignore new line character.
          if( !info.mIsNewParagraphChar )
          {
            matchedCharacters.push_back( info );
            numberOfMatchedCharacters++;
          }
        }
      }
    } // End of loop checking each character's y position in the character layout table

    // Check if last character is a newline, if it is
    // then need pretend there is an imaginary line afterwards,
    // and check if user is touching below previous line.
    const Toolkit::TextView::CharacterLayoutInfo& lastInfo( mTextLayoutInfo.mCharacterLayoutInfoTable[mTextLayoutInfo.mCharacterLayoutInfoTable.size() - 1] );

    if( ( lastInfo.mIsVisible ) && ( lastInfo.mIsNewParagraphChar ) && ( sourceScrollOffset.y > lastInfo.mPosition.y ) )
    {
      closestIndex = mTextLayoutInfo.mCharacterLayoutInfoTable.size();
    }
    else
    {
      // 2 Iterate through matching list of y positions and find closest matching X position.

      bool matched( false );

      // Traverse the characters in the visual order. VCC TODO: check for more than one line.
      std::size_t visualIndex = 0u;
      const std::size_t matchedCharactersSize = matchedCharacters.size();
      for( ; visualIndex < matchedCharactersSize; ++visualIndex )
      {
        const Toolkit::TextView::CharacterLayoutInfo& info( *( matchedCharacters.begin() + mTextLayoutInfo.mCharacterVisualToLogicalMap[visualIndex] ) );

        if( info.mIsVisible )
        {
          // stop when on left side of character's center.
          const float characterMidPointPosition = info.mPosition.x  + ( info.mSize.width * 0.5f ) ;
          if( sourceScrollOffset.x < characterMidPointPosition )
          {
            if(info.mIsRightToLeftCharacter)
            {
              rightToLeftChar = true;
            }
            glyphIntersection = info.mPosition.x;
            matched = true;
            break;
          }

          lastRightToLeftChar = info.mIsRightToLeftCharacter;
        }
      }

      if( visualIndex == matchedCharactersSize )
      {
        rightToLeftChar = lastRightToLeftChar;
      }

      closestIndex = lineOffset + visualIndex;

      mClosestCursorPositionEOL = false; // reset
      if( ( visualIndex == matchedCharactersSize ) && !matched )
      {
        mClosestCursorPositionEOL = true; // Reached end of matched characters in closest line but no match so cursor should be after last character.
      }

      // For RTL characters, need to adjust closestIndex by 1 (as the inequality above would be reverse)
      if( rightToLeftChar && lastRightToLeftChar )
      {
        --closestIndex; // (-1 = numeric_limits<std::size_t>::max())
      }
    }
  }

  // closestIndex is the visual index, need to convert it to the logical index
  if( !mTextLayoutInfo.mCharacterVisualToLogicalMap.empty() )
  {
    if( closestIndex < mTextLayoutInfo.mCharacterVisualToLogicalMap.size() )
    {
      // Checks for situations where user is touching between LTR and RTL
      // characters. To identify if the user means the end of a LTR string
      // or the beginning of an RTL string, and vice versa.
      if( closestIndex > 0 )
      {
        if( rightToLeftChar && !lastRightToLeftChar )
        {
          // [LTR] [RTL]
          //   |..|..|
          //   AAA BBB
          // A: In this touch range, the user is indicating that they wish to place
          // the cursor at the end of the LTR text.
          // B: In this touch range, the user is indicating that they wish to place
          // the cursor at the end of the RTL text.

          // Result of touching A area:
          // [.....LTR]|[RTL......]+
          //
          // |: primary cursor (for typing LTR chars)
          // +: secondary cursor (for typing RTL chars)

          // Result of touching B area:
          // [.....LTR]+[RTL......]|
          //
          // |: primary cursor (for typing RTL chars)
          // +: secondary cursor (for typing LTR chars)

          if( sourceScrollOffset.x < glyphIntersection )
          {
            --closestIndex;
          }
        }
        else if( !rightToLeftChar && lastRightToLeftChar )
        {
          if( sourceScrollOffset.x < glyphIntersection )
          {
            --closestIndex;
          }
        }
      }

      closestIndex = mTextLayoutInfo.mCharacterVisualToLogicalMap[closestIndex];
      // If user touched a left-side of RTL char, and the character on the left was an LTR then position logical cursor
      // one further ahead
      if( rightToLeftChar && !lastRightToLeftChar )
      {
        ++closestIndex;
      }
    }
    else if( closestIndex == std::numeric_limits<std::size_t>::max() ) // -1 RTL (after last arabic character on line)
    {
      closestIndex = mTextLayoutInfo.mCharacterVisualToLogicalMap.size();
    }
    else if( mTextLayoutInfo.mCharacterLayoutInfoTable[ mTextLayoutInfo.mCharacterVisualToLogicalMap[ closestIndex - 1 ] ].mIsRightToLeftCharacter ) // size() LTR (after last european character on line)
    {
      closestIndex = 0;
    }
  }

  return found;
}

float TextInput::GetLineJustificationPosition() const
{
  const Vector3& size = mDisplayedTextView.GetCurrentSize();
  Toolkit::Alignment::Type alignment = mDisplayedTextView.GetTextAlignment();
  float alignmentOffset = 0.f;

  // Work out cursor 'x' position when there are any character accordingly with the text view alignment settings.
  if( alignment & Toolkit::Alignment::HorizontalLeft )
  {
    alignmentOffset = 0.f;
  }
  else if( alignment & Toolkit::Alignment::HorizontalCenter )
  {
    alignmentOffset = 0.5f * ( size.width - mTextLayoutInfo.mTextSize.width );
  }
  else if( alignment & Toolkit::Alignment::HorizontalRight )
  {
    alignmentOffset = size.width - mTextLayoutInfo.mTextSize.width;
  }

  Toolkit::TextView::LineJustification justification = mDisplayedTextView.GetLineJustification();
  float justificationOffset = 0.f;

  switch( justification )
  {
    case Toolkit::TextView::Left:
    {
      justificationOffset = 0.f;
      break;
    }
    case Toolkit::TextView::Center:
    {
      justificationOffset = 0.5f * mTextLayoutInfo.mTextSize.width;
      break;
    }
    case Toolkit::TextView::Right:
    {
      justificationOffset = mTextLayoutInfo.mTextSize.width;
      break;
    }
    case Toolkit::TextView::Justified:
    {
      justificationOffset = 0.f;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( false );
    }
  } // end switch

  return alignmentOffset + justificationOffset;
}

Vector3 TextInput::PositionCursorAfterWordWrap( std::size_t characterPosition ) const
{
  /* Word wrap occurs automatically in TextView when the exceed policy moves a word to the next line when not enough space on current.
     A newline character is not inserted in this case */

  Vector3 cursorPosition;

  Toolkit::TextView::CharacterLayoutInfo currentCharInfo = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterPosition ];

  bool noWrap = true;

  if( characterPosition > 0u )
  {
    Toolkit::TextView::CharacterLayoutInfo previousCharInfo = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterPosition - 1u ];

    // If previous character on a different line then use current characters position
    if( fabsf( (currentCharInfo.mPosition.y - currentCharInfo.mDescender )  - ( previousCharInfo.mPosition.y - previousCharInfo.mDescender) ) > Math::MACHINE_EPSILON_1000 )
    {
      // VCC TODO: PositionCursorAfterWordWrap currently doesn't work for multiline. Need to check this branch.
      if ( mClosestCursorPositionEOL )
      {
        cursorPosition = Vector3( previousCharInfo.mPosition.x + previousCharInfo.mSize.width, previousCharInfo.mPosition.y, previousCharInfo.mPosition.z ) ;
      }
      else
      {
        cursorPosition = Vector3( currentCharInfo.mPosition );
      }

      noWrap = false;
    }
  }

  if( noWrap )
  {
    // If the character is left to right, the position is the character's position plus its width.
    const float ltrOffset = !currentCharInfo.mIsRightToLeftCharacter ? currentCharInfo.mSize.width : 0.f;

    cursorPosition.x = currentCharInfo.mPosition.x  + ltrOffset;
    cursorPosition.y = currentCharInfo.mPosition.y;
  }

  return cursorPosition;
}

Vector3 TextInput::GetActualPositionFromCharacterPosition( std::size_t characterPosition ) const
{
  bool direction = false;
  Vector3 alternatePosition;
  bool alternatePositionValid = false;

  return GetActualPositionFromCharacterPosition( characterPosition, direction, alternatePosition, alternatePositionValid );
}

Vector3 TextInput::GetActualPositionFromCharacterPosition( std::size_t characterPosition, bool& directionRTL, Vector3& alternatePosition, bool& alternatePositionValid ) const
{
  DALI_ASSERT_DEBUG( ( mTextLayoutInfo.mCharacterLayoutInfoTable.size() == mTextLayoutInfo.mCharacterLogicalToVisualMap.size() ) &&
                     ( mTextLayoutInfo.mCharacterLayoutInfoTable.size() == mTextLayoutInfo.mCharacterVisualToLogicalMap.size() ) &&
                     "TextInput::GetActualPositionFromCharacterPosition. All layout tables must have the same size." );

  Vector3 cursorPosition( 0.f, 0.f, 0.f );

  alternatePositionValid = false;
  directionRTL = false;

  if( !mTextLayoutInfo.mCharacterLayoutInfoTable.empty() )
  {
    if( characterPosition == 0u )
    {
      // When the cursor position is at the beginning, it should be at the start of the current character.
      // If the current character is LTR, then the start is on the right side of the glyph.
      // If the current character is RTL, then the start is on the left side of the glyph.

      if( !( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() ) ).mIsVisible )
      {
         characterPosition = FindVisibleCharacter( Right, 0u );
      }

      const Toolkit::TextView::CharacterLayoutInfo& info = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterPosition ];
      const float rtlOffset = info.mIsRightToLeftCharacter ? info.mSize.width : 0.0f;

      cursorPosition.x = info.mPosition.x + rtlOffset;
      cursorPosition.y = info.mPosition.y;
      directionRTL = info.mIsRightToLeftCharacter;
    }
    else if( characterPosition > 0u )
    {
      // Get the direction of the paragraph.
      const std::size_t startCharacterPosition = GetRowStartFromCharacterPosition( characterPosition );
      const bool isParagraphRightToLeft = ( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + startCharacterPosition ) ).mIsRightToLeftCharacter;

      // When cursor is not at beginning, consider possibility of
      // showing 2 cursors. (whereas at beginning we only ever show one cursor)

      // Cursor position should be the end of the last character.
      // If the last character is LTR, then the end is on the right side of the glyph.
      // If the last character is RTL, then the end is on the left side of the glyph.

      --characterPosition;

      if( !( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + characterPosition ) ).mIsVisible )
      {
        characterPosition = FindVisibleCharacter( Left, characterPosition );
      }

      Toolkit::TextView::CharacterLayoutInfo info = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterPosition ];
      if( ( characterPosition > 0u ) && info.mIsNewParagraphChar && !IsScrollEnabled() )
      {
        // VCC TODO : check for a new paragraph character.

        // Prevents the cursor to exceed the boundary if the last visible character is a 'new line character' and the scroll is not enabled.
        const Vector3& size = GetControlSize();

        if( info.mPosition.y + info.mSize.height - mDisplayedTextView.GetLineHeightOffset() > size.height )
        {
          --characterPosition;
        }
        info = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterPosition ];
      }

      if( !info.mIsNewParagraphChar )
      {
        cursorPosition = PositionCursorAfterWordWrap( characterPosition ); // Get position of cursor/handles taking in account auto word wrap.
      }
      else
      {
        // VCC TODO : check for a new paragraph character.

        // When cursor points to first character on new line, position cursor at the start of this glyph.
        if( characterPosition < mTextLayoutInfo.mCharacterLayoutInfoTable.size() )
        {
          const Toolkit::TextView::CharacterLayoutInfo& infoNext = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterPosition ];
          const float start( infoNext.mIsRightToLeftCharacter ? infoNext.mSize.width : 0.0f );

          cursorPosition.x = infoNext.mPosition.x + start;
          cursorPosition.y = infoNext.mPosition.y;
        }
        else
        {
          // If cursor points to the end of text, then can only position
          // cursor where the new line starts based on the line-justification position.
          cursorPosition.x = GetLineJustificationPosition();

          if( characterPosition == mTextLayoutInfo.mCharacterLogicalToVisualMap.size() )
          {
            // If this is after the last character, then we can assume that the new cursor
            // should be exactly one row below the current row.

            const Size rowRect = GetRowRectFromCharacterPosition( characterPosition - 1u );
            cursorPosition.y = info.mPosition.y + rowRect.height;
          }
          else
          {
            // If this is not after last character, then we can use this row's height.
            // should be exactly one row below the current row.

            const Size rowRect = GetRowRectFromCharacterPosition( characterPosition );
            cursorPosition.y = info.mPosition.y + rowRect.height;
          }
        }
      }

      directionRTL = info.mIsRightToLeftCharacter;

      if( 1u < mTextLayoutInfo.mCharacterLayoutInfoTable.size() )
      {
        // 1. When the cursor is neither at the beginning or the end,
        // we can show multiple cursors under situations when the cursor is
        // between RTL and LTR text...
        if( characterPosition + 1u < mTextLayoutInfo.mCharacterLayoutInfoTable.size() )
        {
          std::size_t characterAltPosition = characterPosition + 1u;

          const Toolkit::TextView::CharacterLayoutInfo& infoAlt = mTextLayoutInfo.mCharacterLayoutInfoTable[ characterAltPosition ];

          if(!info.mIsRightToLeftCharacter && infoAlt.mIsRightToLeftCharacter)
          {
            // Stuation occurs when cursor is at the end of English text (LTR) and beginning of Arabic (RTL)
            // Text:     [...LTR...]|[...RTL...]
            // Cursor pos:          ^
            // Alternate cursor pos:            ^
            // In which case we need to display an alternate cursor for the RTL text.

            alternatePosition.x = infoAlt.mPosition.x + infoAlt.mSize.width;
            alternatePosition.y = infoAlt.mPosition.y;
            alternatePositionValid = true;
          }
          else if(info.mIsRightToLeftCharacter && !infoAlt.mIsRightToLeftCharacter)
          {
            // Situation occurs when cursor is at end of the Arabic text (LTR) and beginning of English (RTL)
            // Text:           |[...RTL...] [...LTR....]
            // Cursor pos:     ^
            // Alternate cursor pos:       ^
            // In which case we need to display an alternate cursor for the RTL text.

            alternatePosition.x = infoAlt.mPosition.x;
            alternatePosition.y = infoAlt.mPosition.y;
            alternatePositionValid = true;
          }
        }
        else
        {
          // 2. When the cursor is at the end of the text,
          // and we have multi-directional text,
          // we can also consider showing mulitple cursors.
          // The rule here is:
          // If first and last characters on row are different
          // Directions, then two cursors need to be displayed.

          if( info.mIsRightToLeftCharacter != isParagraphRightToLeft )
          {
            // The last character's direction is differernt than the first one of current paragraph.

            // Get first
            const Toolkit::TextView::CharacterLayoutInfo& infoStart= mTextLayoutInfo.mCharacterLayoutInfoTable[ GetFirstCharacterWithSameDirection( characterPosition ) ];

            if(info.mIsRightToLeftCharacter)
            {
              // For text Starting as LTR and ending as RTL. End cursor position is as follows:
              // Text:     [...LTR...]|[...RTL...]
              // Cursor pos:          ^
              // Alternate cursor pos:            ^
              // In which case we need to display an alternate cursor for the RTL text, this cursor
              // should be at the end of the given line.

              alternatePosition.x = infoStart.mPosition.x + infoStart.mSize.width;
              alternatePosition.y = infoStart.mPosition.y;
              alternatePositionValid = true;
            }
            else if(!info.mIsRightToLeftCharacter) // starting RTL
            {
              // For text Starting as RTL and ending as LTR. End cursor position is as follows:
              // Text:           |[...RTL...] [...LTR....]
              // Cursor pos:     ^
              // Alternate cursor pos:       ^
              // In which case we need to display an alternate cursor for the RTL text.

              alternatePosition.x = infoStart.mPosition.x;
              alternatePosition.y = infoStart.mPosition.y;
              alternatePositionValid = true;
            }
          }
        }
      }
    } // characterPosition > 0
  }
  else
  {
    // If the character table is void, place the cursor accordingly the text alignment.
    const Vector3& size = GetControlSize();

    Toolkit::Alignment::Type alignment = mDisplayedTextView.GetTextAlignment();
    float alignmentOffset = 0.f;

    // Work out cursor 'x' position when there are any character accordingly with the text view alignment settings.
    if( alignment & Toolkit::Alignment::HorizontalLeft )
    {
      alignmentOffset = 0.f;
    }
    else if( alignment & Toolkit::Alignment::HorizontalCenter )
    {
      alignmentOffset = 0.5f * ( size.width );
    }
    else if( alignment & Toolkit::Alignment::HorizontalRight )
    {
      alignmentOffset = size.width;
    }

    // Work out cursor 'x' position when there are any character accordingly with the text view alignment settings.
    cursorPosition.x = alignmentOffset;

    // Work out cursor 'y' position when there are any character accordingly with the text view alignment settings.
    if( alignment & Toolkit::Alignment::VerticalTop )
    {
      cursorPosition.y = mLineHeight;
    }
    else if( alignment & Toolkit::Alignment::VerticalCenter )
    {
      cursorPosition.y = 0.5f * ( size.height + mLineHeight );
    }
    else if( alignment & Toolkit::Alignment::VerticalBottom )
    {
      cursorPosition.y = size.height;
    }
  }

  cursorPosition.x -= mTextLayoutInfo.mScrollOffset.x;
  cursorPosition.y -= mTextLayoutInfo.mScrollOffset.y;

  if( alternatePositionValid )
  {
    alternatePosition.x -= mTextLayoutInfo.mScrollOffset.x;
    alternatePosition.y -= mTextLayoutInfo.mScrollOffset.y;
  }

  return cursorPosition;
}

std::size_t TextInput::GetRowStartFromCharacterPosition( std::size_t logicalPosition ) const
{
  // scan string from current position to beginning of current line to note direction of line
  while( logicalPosition )
  {
    logicalPosition--;
    if( mTextLayoutInfo.mCharacterLayoutInfoTable[logicalPosition].mIsNewParagraphChar )
    {
      logicalPosition++;
      break;
    }
  }

  return logicalPosition;
}

std::size_t TextInput::GetFirstCharacterWithSameDirection( std::size_t logicalPosition ) const
{
  const bool isRightToLeft = mTextLayoutInfo.mCharacterLayoutInfoTable[logicalPosition].mIsRightToLeftCharacter;

  while( logicalPosition )
  {
    logicalPosition--;
    if( isRightToLeft != mTextLayoutInfo.mCharacterLayoutInfoTable[logicalPosition].mIsRightToLeftCharacter )
    {
      logicalPosition++;
      break;
    }
  }

  return logicalPosition;
}

Size TextInput::GetRowRectFromCharacterPosition(std::size_t characterPosition) const
{
  Vector2 min, max;

  return GetRowRectFromCharacterPosition( characterPosition, min, max );
}

Size TextInput::GetRowRectFromCharacterPosition( std::size_t characterPosition, Vector2& min, Vector2& max ) const
{
  // if we have no text content, then return position 0,0 with width 0, and height the same as cursor height.
  if( mTextLayoutInfo.mCharacterLayoutInfoTable.empty() )
  {
    min = Vector2::ZERO;
    max = Vector2(0.0f, mLineHeight);
    return max;
  }

  DALI_ASSERT_DEBUG( characterPosition <= mTextLayoutInfo.mCharacterLayoutInfoTable.size() );

  // Initializes the min and max position.
  const std::size_t initialPosition = ( characterPosition == mTextLayoutInfo.mCharacterLayoutInfoTable.size() ) ? characterPosition - 1u : characterPosition;
  min = ( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + initialPosition ) ).mPosition.GetVectorXY();
  max = min;

  bool found = false;
  // 1) Find the line where the character is laid-out.
  for( Toolkit::TextView::LineLayoutInfoContainer::const_iterator lineIt = mTextLayoutInfo.mLines.begin(), lineEndIt = mTextLayoutInfo.mLines.end();
         !found && ( lineIt != mTextLayoutInfo.mLines.end() );
       ++lineIt )
  {
    const Toolkit::TextView::LineLayoutInfo& lineInfo( *lineIt );

    // Index within the whole text to the last character of the current line.
    std::size_t lastCharacterOfLine = 0u;

    Toolkit::TextView::LineLayoutInfoContainer::const_iterator lineNextIt = lineIt + 1u;
    if( lineNextIt != lineEndIt )
    {
      lastCharacterOfLine = (*lineNextIt).mCharacterGlobalIndex - 1u;
    }
    else
    {
      lastCharacterOfLine = mTextLayoutInfo.mCharacterLayoutInfoTable.size() - 1u;
    }

    // Check if the given chracter position is within the line.
    if( ( lineInfo.mCharacterGlobalIndex <= initialPosition ) && ( initialPosition <= lastCharacterOfLine ) )
    {
      // 2) Get the row rect of all laid-out characters on the line.

      // Need to scan all characters of the line because they are in the logical position.
      for( Toolkit::TextView::CharacterLayoutInfoContainer::const_iterator it = mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + lineInfo.mCharacterGlobalIndex,
             endIt = mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + lastCharacterOfLine + 1u;
           it != endIt;
           ++it )
      {
        const Toolkit::TextView::CharacterLayoutInfo& characterInfo( *it );

        min.x = std::min( min.x, characterInfo.mPosition.x );
        min.y = std::min( min.y, characterInfo.mPosition.y );
        max.x = std::max( max.x, characterInfo.mPosition.x + characterInfo.mSize.width );
        max.y = std::max( max.y, characterInfo.mPosition.y + characterInfo.mSize.height );
      }

      found = true;
    }
  }

  return Size( max.x - min.x, max.y - min.y );
}

bool TextInput::WasTouchedCheck( const Actor& touchedActor ) const
{
  Actor popUpPanel = mPopupPanel.GetRootActor();

  if ( ( touchedActor == Self() ) || ( touchedActor == popUpPanel ) )
  {
    return true;
  }
  else
  {
    Dali::Actor parent( touchedActor.GetParent() );

    if ( parent )
    {
      return WasTouchedCheck( parent );
    }
  }

  return false;
}

void TextInput::StartMonitoringStageForTouch()
{
  Stage stage = Stage::GetCurrent();
  stage.TouchedSignal().Connect( this, &TextInput::OnStageTouched );
}

void TextInput::EndMonitoringStageForTouch()
{
  Stage stage = Stage::GetCurrent();
  stage.TouchedSignal().Disconnect( this, &TextInput::OnStageTouched );
}

void TextInput::OnStageTouched(const TouchEvent& event)
{
  if( event.GetPointCount() > 0 )
  {
    if ( TouchPoint::Down == event.GetPoint(0).state )
    {
      const Actor touchedActor(event.GetPoint(0).hitActor);

      bool popUpShown( false );

      if ( ( mPopupPanel.GetState() == TextInputPopup::StateShowing ) || ( mPopupPanel.GetState() == TextInputPopup::StateShown ) )
      {
        popUpShown = true;
      }

      bool textInputTouched = (touchedActor && WasTouchedCheck( touchedActor ));

      if ( ( mHighlightMeshActor || popUpShown ) && !textInputTouched )
      {
        EndMonitoringStageForTouch();
        HidePopup( true, false );
      }

      if ( ( IsGrabHandleEnabled() && mGrabHandle  ) && !textInputTouched )
      {
        EndMonitoringStageForTouch();
        ShowGrabHandleAndSetVisibility( false );
      }
    }
  }
}

void TextInput::SelectText(std::size_t start, std::size_t end)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "SelectText mEditModeActive[%s] grabHandle[%s] start[%u] end[%u] size[%u]\n", mEditModeActive?"true":"false",
                                                                                                                          IsGrabHandleEnabled()?"true":"false",
                                                                                                                          start, end, mTextLayoutInfo.mCharacterLayoutInfoTable.size() );
  DALI_ASSERT_ALWAYS( start <= mTextLayoutInfo.mCharacterLayoutInfoTable.size() && "TextInput::SelectText start out of max range" );
  DALI_ASSERT_ALWAYS( end <= mTextLayoutInfo.mCharacterLayoutInfoTable.size() && "TextInput::SelectText end out of max range" );

  StartMonitoringStageForTouch();

  if ( mEditModeActive ) // Only allow text selection when in edit mode
  {
    // When replacing highlighted text keyboard should ignore current word at cursor hence notify keyboard that the cursor is at the start of the highlight.
    mSelectingText = true;

    std::size_t selectionStartPosition = std::min( start, end );

    // Hide grab handle when selecting.
    ShowGrabHandleAndSetVisibility( false );

    if( start != end )  // something to select
    {
      SetCursorVisibility( false );
      StopCursorBlinkTimer();

      CreateSelectionHandles(start, end);
      UpdateHighlight();

      const TextStyle oldInputStyle( mInputStyle );
      mInputStyle = GetStyleAt( selectionStartPosition ); // Inherit style from selected position.

      if( oldInputStyle != mInputStyle )
      {
        // Updates the line height accordingly with the input style.
        UpdateLineHeight();

        EmitStyleChangedSignal();
      }

      HidePopup();
    }

    mSelectingText = false;
  }
}

MarkupProcessor::StyledTextArray TextInput::GetSelectedText()
{
  MarkupProcessor::StyledTextArray currentSelectedText;

  if ( IsTextSelected() )
  {
    MarkupProcessor::StyledTextArray::iterator it = mStyledText.begin() + std::min(mSelectionHandleOnePosition, mSelectionHandleTwoPosition);
    MarkupProcessor::StyledTextArray::iterator end = mStyledText.begin() + std::max(mSelectionHandleOnePosition, mSelectionHandleTwoPosition);

    for(; it != end; ++it)
    {
      MarkupProcessor::StyledText& styledText( *it );
      currentSelectedText.push_back( styledText );
    }
  }
  return currentSelectedText;
}

void TextInput::ApplyStyleToRange(const TextStyle& style, const TextStyle::Mask mask, const std::size_t begin, const std::size_t end)
{
  const std::size_t beginIndex = std::min( begin, end );
  const std::size_t endIndex = std::max( begin, end );

  // Apply the style
  MarkupProcessor::SetTextStyleToRange( mStyledText, style, mask, beginIndex, endIndex );

  // Create a styled text array used to replace the text into the text-view.
  MarkupProcessor::StyledTextArray text;
  text.insert( text.begin(), mStyledText.begin() + beginIndex, mStyledText.begin() + endIndex + 1 );

  mDisplayedTextView.ReplaceTextFromTo( beginIndex, ( endIndex - beginIndex ) + 1, text );
  GetTextLayoutInfo();

  if( IsScrollEnabled() )
  {
    // Need to set the scroll position as the text's size may have changed.
    ScrollTextViewToMakeCursorVisible( Vector3( mTextLayoutInfo.mScrollOffset.x, mTextLayoutInfo.mScrollOffset.y, 0.f ) );
  }

  ShowGrabHandleAndSetVisibility( false );

  DrawCursor();

  UpdateHighlight();

  // Set Handle positioning as the new style may have repositioned the characters.
  SetSelectionHandlePosition(HandleOne);
  SetSelectionHandlePosition(HandleTwo);
}

void TextInput::KeyboardStatusChanged(bool keyboardShown)
{
  // Just hide the grab handle when keyboard is hidden.
  if (!keyboardShown )
  {
    ShowGrabHandleAndSetVisibility( false );

    // If the keyboard is not now being shown, then hide the popup panel
    mPopupPanel.Hide( true );
  }
}

// Removes highlight and resumes edit mode state
void TextInput::RemoveHighlight( bool hidePopup )
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "RemoveHighlight\n");

  if ( mHighlightMeshActor )
  {
    if ( mSelectionHandleOne )
    {
      mActiveLayer.Remove( mSelectionHandleOne );
      mSelectionHandleOne.Reset();
      mSelectionHandleOneOffset.x = 0.0f;
    }
    if ( mSelectionHandleTwo )
    {
      mActiveLayer.Remove( mSelectionHandleTwo );
      mSelectionHandleTwo.Reset();
      mSelectionHandleTwoOffset.x = 0.0f;
    }

    mNewHighlightInfo.mQuadList.clear();

    Self().Remove( mHighlightMeshActor );

    SetCursorVisibility( true );
    StartCursorBlinkTimer();

    mHighlightMeshActor.Reset();
    // NOTE: We cannot dereference mHighlightMesh, due
    // to a bug in how the scene-graph MeshRenderer uses the Mesh data incorrectly.

    if ( hidePopup )
    {
      HidePopup();
    }
  }

  mSelectionHandleOnePosition = 0;
  mSelectionHandleTwoPosition = 0;
}

void TextInput::CreateHighlight()
{
  if ( !mHighlightMeshActor )
  {
    mMeshData = MeshData( );
    mMeshData.SetHasNormals( true );

    mCustomMaterial = Material::New("CustomMaterial");
    mCustomMaterial.SetDiffuseColor( mMaterialColor );

    mMeshData.SetMaterial( mCustomMaterial );

    mHighlightMesh = Mesh::New( mMeshData );

    mHighlightMeshActor = MeshActor::New( mHighlightMesh );
    mHighlightMeshActor.SetName( "HighlightMeshActor" );
    mHighlightMeshActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mHighlightMeshActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mHighlightMeshActor.SetPosition( 0.0f, 0.0f, DISPLAYED_HIGHLIGHT_Z_OFFSET );
    mHighlightMeshActor.SetAffectedByLighting(false);

    Self().Add(mHighlightMeshActor);
  }
}


bool TextInput::CopySelectedTextToClipboard()
{
  mCurrentCopySelecton.clear();

  mCurrentCopySelecton = GetSelectedText();

  std::string stringToStore;

  /* Create a StyledTextArray from the selected region so can use the MarkUpProcessor to produce
   * a marked up string.
   */
  MarkupProcessor::StyledTextArray selectedText(mCurrentCopySelecton.begin(),mCurrentCopySelecton.end());
  MarkupProcessor::GetPlainString( selectedText, stringToStore );

  bool success = mClipboard.SetItem( stringToStore );
  return success;
}

void TextInput::PasteText( const Text& text )
{
  // Update Flag, indicates whether to update the text-input contents or not.
  // Any key stroke that results in a visual change of the text-input should
  // set this flag to true.
  bool update = false;
  if( mHighlightMeshActor )
  {
    /* if highlighted, delete entire text, and position cursor at start of deleted text. */
    mCursorPosition = std::min(mSelectionHandleOnePosition, mSelectionHandleTwoPosition);

    ImfManager imfManager = ImfManager::Get();
    if ( imfManager )
    {
      imfManager.SetCursorPosition( mCursorPosition );
      imfManager.NotifyCursorPosition();
    }
    DeleteHighlightedText( true );
    update = true;
  }

  bool textExceedsMaximunNumberOfCharacters = false;
  bool textExceedsBoundary = false;

  std::size_t insertedStringLength = DoInsertAt( text, mCursorPosition, 0, textExceedsMaximunNumberOfCharacters, textExceedsBoundary );

  mCursorPosition += insertedStringLength;
  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    imfManager.SetCursorPosition ( mCursorPosition );
    imfManager.NotifyCursorPosition();
  }

  update = update || ( insertedStringLength > 0 );
  if( update )
  {
    CursorUpdate();
    EmitTextModified();
  }

  if( insertedStringLength < text.GetLength() )
  {
    EmitMaxInputCharactersReachedSignal();
  }

  if( textExceedsBoundary )
  {
    EmitInputTextExceedsBoundariesSignal();
  }
}

void TextInput::SetTextDirection()
{
  // Put the cursor to the right if we are empty and an RTL language is being used.
  if ( mStyledText.empty() )
  {
    VirtualKeyboard::TextDirection direction( VirtualKeyboard::GetTextDirection() );

    // Get the current text alignment preserving the vertical alignment. Also preserve the horizontal center
    // alignment as we do not want to set the text direction if we've been asked to be in the center.
    //
    // TODO: Should split SetTextAlignment into two APIs to better handle this (sometimes apps just want to
    //       set vertical alignment but are being forced to set the horizontal alignment as well with the
    //       current API.
    int alignment( mDisplayedTextView.GetTextAlignment() &
                  ( Toolkit::Alignment::VerticalTop |
                    Toolkit::Alignment::VerticalCenter |
                    Toolkit::Alignment::VerticalBottom |
                    Toolkit::Alignment::HorizontalCenter ) );
    Toolkit::TextView::LineJustification justification( mDisplayedTextView.GetLineJustification() );

    // If our alignment is in the center, then do not change.
    if ( !( alignment & Toolkit::Alignment::HorizontalCenter ) )
    {
      alignment |= ( direction == VirtualKeyboard::LeftToRight ) ? Toolkit::Alignment::HorizontalLeft : Toolkit::Alignment::HorizontalRight;
    }

    // If our justification is in the center, then do not change.
    if ( justification != Toolkit::TextView::Center )
    {
      justification = ( direction == VirtualKeyboard::LeftToRight ) ? Toolkit::TextView::Left : Toolkit::TextView::Right;
    }

    mDisplayedTextView.SetTextAlignment( static_cast<Toolkit::Alignment::Type>(alignment) );
    mDisplayedTextView.SetLineJustification( justification );
  }
}

void TextInput::UpdateLineHeight()
{
  Dali::Font font = Dali::Font::New( FontParameters( mInputStyle.GetFontName(), mInputStyle.GetFontStyle(), mInputStyle.GetFontPointSize() ) );
  mLineHeight = font.GetLineHeight();

  // If the height exceed policy is shrink or exceed the boundaries of the text-input is not allowed, then modify the line height is needed.

  const bool shrink = mDisplayedTextView && ( Toolkit::TextView::ShrinkToFit == mDisplayedTextView.GetHeightExceedPolicy() ) && mStyledText.empty();

  if( !mExceedEnabled || shrink )
  {
    mLineHeight = std::min( mLineHeight, GetControlSize().height );
  }
}

std::size_t TextInput::FindVisibleCharacter( FindVisibleCharacterDirection direction , std::size_t cursorPosition ) const
{
  // VCC check if we need do this in the visual order ...
  std::size_t position = 0u;

  const std::size_t tableSize = mTextLayoutInfo.mCharacterLayoutInfoTable.size();

  switch( direction )
  {
    case Left:
    {
      position = FindVisibleCharacterLeft( cursorPosition, mTextLayoutInfo.mCharacterLayoutInfoTable );

      if( !( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + ( tableSize == position ? position - 1u : position ) ) ).mIsVisible )
      {
        position = FindVisibleCharacterRight( cursorPosition, mTextLayoutInfo.mCharacterLayoutInfoTable );
      }
      break;
    }
    case Right:
    {
      position = FindVisibleCharacterRight( cursorPosition, mTextLayoutInfo.mCharacterLayoutInfoTable );
      if( !( *( mTextLayoutInfo.mCharacterLayoutInfoTable.begin() + ( tableSize == position ? position - 1u : position ) ) ).mIsVisible )
      {
        position = FindVisibleCharacterLeft( cursorPosition, mTextLayoutInfo.mCharacterLayoutInfoTable );
      }
      break;
    }
    case ByEnd:
    {
      position = FindVisibleCharacterLeft( 0u, mTextLayoutInfo.mCharacterLayoutInfoTable );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextInput::FindVisibleCharacter() Unknown direction." );
    }
  }

  return position;
}

void TextInput::SetSortModifier( float depthOffset )
{
  if(mDisplayedTextView)
  {
    mDisplayedTextView.SetSortModifier(depthOffset);
  }
}

void TextInput::SetSnapshotModeEnabled( bool enable )
{
  if(mDisplayedTextView)
  {
    mDisplayedTextView.SetSnapshotModeEnabled( enable );
  }
}

bool TextInput::IsSnapshotModeEnabled() const
{
  bool snapshotEnabled = false;

  if(mDisplayedTextView)
  {
    snapshotEnabled = mDisplayedTextView.IsSnapshotModeEnabled();
  }

  return snapshotEnabled;
}

void TextInput::SetMarkupProcessingEnabled( bool enable )
{
  mMarkUpEnabled = enable;
}

bool TextInput::IsMarkupProcessingEnabled() const
{
  return mMarkUpEnabled;
}

void TextInput::SetScrollEnabled( bool enable )
{
  if( mDisplayedTextView )
  {
    mDisplayedTextView.SetScrollEnabled( enable );
  }

  if( !enable )
  {
    // Don't set cursor's and handle's visibility to false if they are outside the
    // boundaries of the text-input.
    mIsCursorInScrollArea = true;
    mIsGrabHandleInScrollArea = true;
    if( mSelectionHandleOne && mSelectionHandleTwo )
    {
      mSelectionHandleOne.SetVisible( true );
      mSelectionHandleTwo.SetVisible( true );

      if( mHighlightMeshActor )
      {
        mHighlightMeshActor.SetVisible( true );
      }
    }
  }
}

bool TextInput::IsScrollEnabled() const
{
  bool scrollEnabled = false;

  if( mDisplayedTextView )
  {
    scrollEnabled = mDisplayedTextView.IsScrollEnabled();
  }

  return scrollEnabled;
}

void TextInput::SetScrollPosition( const Vector2& position )
{
  if( mDisplayedTextView )
  {
    mDisplayedTextView.SetScrollPosition( position );
  }
}

Vector2 TextInput::GetScrollPosition() const
{
  Vector2 scrollPosition;

  if( mDisplayedTextView )
  {
    scrollPosition = mDisplayedTextView.GetScrollPosition();
  }

  return scrollPosition;
}

std::size_t TextInput::DoInsertAt( const Text& text, const std::size_t position, const std::size_t numberOfCharactersToReplace, bool& textExceedsMaximunNumberOfCharacters, bool& textExceedsBoundary )
{
  // determine number of characters that we can write to style text buffer, this is the insertStringLength
  std::size_t insertedStringLength = std::min( text.GetLength(), mMaxStringLength - mStyledText.size() );
  textExceedsMaximunNumberOfCharacters = insertedStringLength < text.GetLength();

  // Add style to the new input text.
  MarkupProcessor::StyledTextArray textToInsert;
  for( std::size_t i = 0; i < insertedStringLength; ++i )
  {
    const MarkupProcessor::StyledText newStyledCharacter( text[i], mInputStyle );
    textToInsert.push_back( newStyledCharacter );
  }

  //Insert text to the TextView.
  const bool emptyTextView = mStyledText.empty();
  if( emptyTextView && mPlaceHolderSet )
  {
    // There is no text set so call to TextView::SetText() is needed in order to clear the placeholder text.
    mDisplayedTextView.SetText( textToInsert );
  }
  else
  {
    if( 0 == numberOfCharactersToReplace )
    {
      mDisplayedTextView.InsertTextAt( position, textToInsert );
    }
    else
    {
      mDisplayedTextView.ReplaceTextFromTo( position, numberOfCharactersToReplace, textToInsert );
    }
  }
  mPlaceHolderSet = false;

  if( textToInsert.empty() )
  {
    // If no text has been inserted, GetTextLayoutInfo() need to be called to check whether mStyledText has some text.
    GetTextLayoutInfo();
  }
  else
  {
    // GetTextLayoutInfo() can't be used here as mStyledText is not updated yet.
    mDisplayedTextView.GetTextLayoutInfo( mTextLayoutInfo );
  }

  textExceedsBoundary = false;

  if( !mExceedEnabled )
  {
    const Vector3& size = GetControlSize();

    if( ( mTextLayoutInfo.mTextSize.width > size.width ) || ( mTextLayoutInfo.mTextSize.height > size.height ) )
    {
      // If new text does not fit within TextView
      mDisplayedTextView.RemoveTextFrom( position, insertedStringLength );
      // previously inserted text has been removed. Call GetTextLayoutInfo() to check whether mStyledText has some text.
      GetTextLayoutInfo();
      textExceedsBoundary = true;
      insertedStringLength = 0;
    }

    if( textExceedsBoundary )
    {
      // Add the part of the text which fits on the text-input.

      // Split the text which doesn't fit in two halves.
      MarkupProcessor::StyledTextArray firstHalf;
      MarkupProcessor::StyledTextArray secondHalf;
      SplitText( textToInsert, firstHalf, secondHalf );

      // Clear text. This text will be filled with the text inserted.
      textToInsert.clear();

      // Where to insert the text.
      std::size_t positionToInsert = position;

      bool end = text.GetLength() <= 1;
      while( !end )
      {
        // Insert text and check ...
        const std::size_t textLength = firstHalf.size();
        mDisplayedTextView.InsertTextAt( positionToInsert, firstHalf );
        mDisplayedTextView.GetTextLayoutInfo( mTextLayoutInfo );

        if( ( mTextLayoutInfo.mTextSize.width > size.width ) || ( mTextLayoutInfo.mTextSize.height > size.height ) )
        {
          // Inserted text doesn't fit.

          // Remove inserted text
          mDisplayedTextView.RemoveTextFrom( positionToInsert, textLength );
          mDisplayedTextView.GetTextLayoutInfo( mTextLayoutInfo );

          // The iteration finishes when only one character doesn't fit.
          end = textLength <= 1;

          if( !end )
          {
            // Prepare next two halves for next iteration.
            MarkupProcessor::StyledTextArray copyText = firstHalf;
            SplitText( copyText, firstHalf, secondHalf );
          }
        }
        else
        {
          // Text fits.

          // store text to be inserted in mStyledText.
          textToInsert.insert( textToInsert.end(), firstHalf.begin(), firstHalf.end() );

          // Increase the inserted characters counter.
          insertedStringLength += textLength;

          // Prepare next two halves for next iteration.
          MarkupProcessor::StyledTextArray copyText = secondHalf;
          SplitText( copyText, firstHalf, secondHalf );

          // Update where next text has to be inserted
          positionToInsert += textLength;
        }
      }
    }
  }

  if( textToInsert.empty() && emptyTextView )
  {
    // No character has been added and the text-view was empty.
    // Show the placeholder text.
    ShowPlaceholderText( mStyledPlaceHolderText );
  }
  else
  {
    MarkupProcessor::StyledTextArray::iterator it = mStyledText.begin() + position;
    mStyledText.insert( it, textToInsert.begin(), textToInsert.end() );
    mPlaceHolderSet = false;
  }

  return insertedStringLength;
}

void TextInput::GetTextLayoutInfo()
{
  if( mStyledText.empty() )
  {
    // The text-input has no text, clear the text-view's layout info.
    mTextLayoutInfo = Toolkit::TextView::TextLayoutInfo();
  }
  else
  {
    if( mDisplayedTextView )
    {
      mDisplayedTextView.GetTextLayoutInfo( mTextLayoutInfo );
    }
    else
    {
      // There is no text-view.
      mTextLayoutInfo = Toolkit::TextView::TextLayoutInfo();
    }
  }
}

void TextInput::SetOffsetFromText( const Vector4& offset )
{
  mPopupOffsetFromText = offset;
}

const Vector4& TextInput::GetOffsetFromText() const
{
  return mPopupOffsetFromText;
}

void TextInput::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::TextInput textInput = Toolkit::TextInput::DownCast( Dali::BaseHandle( object ) );

  if ( textInput )
  {
    TextInput& textInputImpl( GetImpl( textInput ) );

    switch ( propertyIndex )
    {
      case Toolkit::TextInput::HIGHLIGHT_COLOR_PROPERTY:
      {
        textInputImpl.SetMaterialDiffuseColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_PRESSED_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupPressedColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_BORDER_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupBorderColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_ICON_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupIconColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_ICON_PRESSED_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupIconPressedColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_TEXT_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupTextColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_TEXT_PRESSED_COLOR_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetCutPastePopupTextPressedColor( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CUT_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetButtonPriorityPosition( TextInputPopup::ButtonsCut, value.Get<unsigned int>() );
        break;
      }
      case Toolkit::TextInput::COPY_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetButtonPriorityPosition( TextInputPopup::ButtonsCopy, value.Get<unsigned int>() );
        break;
      }
      case Toolkit::TextInput::PASTE_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetButtonPriorityPosition( TextInputPopup::ButtonsPaste, value.Get<unsigned int>() );
        break;
      }
      case Toolkit::TextInput::SELECT_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetButtonPriorityPosition( TextInputPopup::ButtonsSelect, value.Get<unsigned int>() );
        break;
      }
      case Toolkit::TextInput::SELECT_ALL_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetButtonPriorityPosition( TextInputPopup::ButtonsSelectAll, value.Get<unsigned int>() );
        break;
      }
      case Toolkit::TextInput::CLIPBOARD_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        textInputImpl.mPopupPanel.SetButtonPriorityPosition( TextInputPopup::ButtonsClipboard, value.Get<unsigned int>() );
        break;
      }
      case Toolkit::TextInput::POP_UP_OFFSET_FROM_TEXT_PROPERTY:
      {
        textInputImpl.SetOffsetFromText( value.Get< Vector4 >() );
        break;
      }
      case Toolkit::TextInput::CURSOR_COLOR_PROPERTY:
      {
        textInputImpl.mCursor.SetColor( value.Get< Vector4 >() );
      }
    }
  }
}

Property::Value TextInput::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::TextInput textInput = Toolkit::TextInput::DownCast( Dali::BaseHandle( object ) );

  if ( textInput )
  {
    TextInput& textInputImpl( GetImpl( textInput ) );

    switch ( propertyIndex )
    {
      case Toolkit::TextInput::HIGHLIGHT_COLOR_PROPERTY:
      {
        value = textInputImpl.GetMaterialDiffuseColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_COLOR_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_PRESSED_COLOR_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupPressedColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_BORDER_COLOR_PROPERTY :
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupBorderColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_ICON_COLOR_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupIconColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_ICON_PRESSED_COLOR_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupIconPressedColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_TEXT_COLOR_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupTextColor();
        break;
      }
      case Toolkit::TextInput::CUT_AND_PASTE_TEXT_PRESSED_COLOR_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetCutPastePopupTextPressedColor();
        break;
      }
      case Toolkit::TextInput::CUT_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetButtonPriorityPosition( TextInputPopup::ButtonsCut );
        break;
      }
      case Toolkit::TextInput::COPY_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        value =  textInputImpl.mPopupPanel.GetButtonPriorityPosition( TextInputPopup::ButtonsCopy );
        break;
      }
      case Toolkit::TextInput::PASTE_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetButtonPriorityPosition( TextInputPopup::ButtonsPaste );
        break;
      }
      case Toolkit::TextInput::SELECT_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetButtonPriorityPosition( TextInputPopup::ButtonsSelect );
        break;
      }
      case Toolkit::TextInput::SELECT_ALL_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetButtonPriorityPosition( TextInputPopup::ButtonsSelectAll );
        break;
      }
      case Toolkit::TextInput::CLIPBOARD_BUTTON_POSITION_PRIORITY_PROPERTY:
      {
        value = textInputImpl.mPopupPanel.GetButtonPriorityPosition( TextInputPopup::ButtonsClipboard );
        break;
      }
      case Toolkit::TextInput::POP_UP_OFFSET_FROM_TEXT_PROPERTY:
      {
        value = textInputImpl.GetOffsetFromText();
        break;
      }
      case Toolkit::TextInput::CURSOR_COLOR_PROPERTY:
      {
        value = textInputImpl.mCursor.GetCurrentColor();
      }
    }
  }
  return value;
}

void TextInput::EmitStyleChangedSignal()
{
  // emit signal if input style changes.
  Toolkit::TextInput handle( GetOwner() );
  mStyleChangedSignal.Emit( handle, mInputStyle );
}

void TextInput::EmitTextModified()
{
  // emit signal when text changes.
  Toolkit::TextInput handle( GetOwner() );
  mTextModifiedSignal.Emit( handle );
}


void TextInput::EmitMaxInputCharactersReachedSignal()
{
  // emit signal if max characters is reached during text input.
  DALI_LOG_INFO(gLogFilter, Debug::General, "EmitMaxInputCharactersReachedSignal \n");

  Toolkit::TextInput handle( GetOwner() );
  mMaxInputCharactersReachedSignal.Emit( handle );
}

void TextInput::EmitInputTextExceedsBoundariesSignal()
{
  // Emit a signal when the input text exceeds the boundaries of the text input.

  Toolkit::TextInput handle( GetOwner() );
  mInputTextExceedBoundariesSignal.Emit( handle );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
