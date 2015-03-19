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
#include <vector>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-io.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

using std::vector;

namespace
{

const float MAX_FLOAT = std::numeric_limits<float>::max();
const std::string EMPTY_STRING;

enum ModifyType
{
  REPLACE_TEXT, ///< Replace the entire text
  INSERT_TEXT,  ///< Insert characters at the current cursor position
  DELETE_TEXT   ///< Delete a character at the current cursor position
};

struct ModifyEvent
{
  ModifyType type;
  std::string text;
};

} // namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Controller::TextInput
{
  // Used to queue input events until DoRelayout()
  enum EventType
  {
    KEYBOARD_FOCUS_GAIN_EVENT,
    KEYBOARD_FOCUS_LOST_EVENT,
    CURSOR_KEY_EVENT,
    TAP_EVENT,
    PAN_EVENT,
    GRAB_HANDLE_EVENT
  };

  union Param
  {
    int mInt;
    unsigned int mUint;
    float mFloat;
  };

  struct Event
  {
    Event( EventType eventType )
    : type( eventType )
    {
      p1.mInt = 0;
      p2.mInt = 0;
    }

    EventType type;
    Param p1;
    Param p2;
    Param p3;
  };

  enum State
  {
    INACTIVE,
    SELECTING,
    EDITING
  };

  TextInput( LogicalModelPtr logicalModel,
             VisualModelPtr visualModel,
             DecoratorPtr decorator )
  : mLogicalModel( logicalModel ),
    mVisualModel( visualModel ),
    mDecorator( decorator ),
    mState( INACTIVE ),
    mPrimaryCursorPosition( 0u ),
    mSecondaryCursorPosition( 0u ),
    mDecoratorUpdated( false ),
    mCursorBlinkEnabled( true ),
    mGrabHandleEnabled( false ),
    mGrabHandlePopupEnabled( false ),
    mSelectionEnabled( false ),
    mHorizontalScrollingEnabled( true ),
    mVerticalScrollingEnabled( false ),
    mUpdateCursorPosition( false )
  {
  }

  /**
   * @brief Helper to move the cursor, grab handle etc.
   */
  bool ProcessInputEvents( const Vector2& controlSize )
  {
    mDecoratorUpdated = false;

    if( mDecorator )
    {
      for( vector<TextInput::Event>::iterator iter = mEventQueue.begin(); iter != mEventQueue.end(); ++iter )
      {
        switch( iter->type )
        {
          case KEYBOARD_FOCUS_GAIN_EVENT:
          {
            OnKeyboardFocus( true );
            break;
          }
          case KEYBOARD_FOCUS_LOST_EVENT:
          {
            OnKeyboardFocus( false );
            break;
          }
          case CURSOR_KEY_EVENT:
          {
            OnCursorKeyEvent( *iter );
            break;
          }
          case TAP_EVENT:
          {
            OnTapEvent( *iter );
            break;
          }
          case PAN_EVENT:
          {
            OnPanEvent( *iter, controlSize );
            break;
          }
          case GRAB_HANDLE_EVENT:
          {
            OnGrabHandleEvent( *iter );
            break;
          }
        }
      }
    }

    // The cursor must also be repositioned after inserts into the model
    if( mUpdateCursorPosition )
    {
      UpdateCursorPosition();
      mUpdateCursorPosition = false;
    }

    mEventQueue.clear();

    return mDecoratorUpdated;
  }

  void OnKeyboardFocus( bool hasFocus )
  {
    if( !hasFocus )
    {
      ChangeState( INACTIVE );
    }
    else
    {
      ChangeState( EDITING );
    }
  }

  void OnCursorKeyEvent( const Event& event )
  {
    int keyCode = event.p1.mInt;

    if( Dali::DALI_KEY_CURSOR_LEFT == keyCode )
    {
      // TODO
    }
    else if( Dali::DALI_KEY_CURSOR_RIGHT == keyCode )
    {
      // TODO
    }
    else if( Dali::DALI_KEY_CURSOR_UP == keyCode )
    {
      // TODO
    }
    else if(   Dali::DALI_KEY_CURSOR_DOWN == keyCode )
    {
      // TODO
    }
  }

  void HandleCursorKey( int keyCode )
  {
    // TODO
  }

  void OnTapEvent( const Event& event )
  {
    unsigned int tapCount = event.p1.mUint;

    if( 1u == tapCount )
    {
      ChangeState( EDITING );

      float xPosition = event.p2.mFloat;
      float yPosition = event.p3.mFloat;
      float height(0.0f);
      GetClosestCursorPosition( mPrimaryCursorPosition, xPosition, yPosition, height );
      mDecorator->SetPosition( PRIMARY_CURSOR, xPosition, yPosition, height );
      mUpdateCursorPosition = false;

      mDecoratorUpdated = true;
    }
    else if( mSelectionEnabled &&
             2u == tapCount )
    {
      ChangeState( SELECTING );
    }
  }

  void OnPanEvent( const Event& event, const Vector2& controlSize )
  {
    int state = event.p1.mInt;

    if( Gesture::Started    == state ||
        Gesture::Continuing == state )
    {
      const Vector2& actualSize = mVisualModel->GetActualSize();

      if( mHorizontalScrollingEnabled )
      {
        float displacementX = event.p2.mFloat;
        mScrollPosition.x += displacementX;

        // Clamp between -space & 0
        float contentWidth = actualSize.width;
        float space = (contentWidth > controlSize.width) ? contentWidth - controlSize.width : 0.0f;
        mScrollPosition.x = ( mScrollPosition.x < -space ) ? -space : mScrollPosition.x;
        mScrollPosition.x = ( mScrollPosition.x > 0 )      ?      0 : mScrollPosition.x;

        mDecoratorUpdated = true;
      }
      if( mVerticalScrollingEnabled )
      {
        float displacementY = event.p3.mFloat;
        mScrollPosition.y += displacementY;

        // Clamp between -space & 0
        float space = (actualSize.height > controlSize.height) ? actualSize.height - controlSize.height : 0.0f;
        mScrollPosition.y = ( mScrollPosition.y < -space ) ? -space : mScrollPosition.y;
        mScrollPosition.y = ( mScrollPosition.y > 0 )      ?      0 : mScrollPosition.y;

        mDecoratorUpdated = true;
      }
    }
  }

  void OnGrabHandleEvent( const Event& event )
  {
    unsigned int state = event.p1.mUint;

    if( GRAB_HANDLE_PRESSED == state )
    {
      float xPosition = event.p2.mFloat;
      float yPosition = event.p3.mFloat;
      float height(0.0f);

      GetClosestCursorPosition( mPrimaryCursorPosition, xPosition, yPosition, height );

      mDecorator->SetPosition( PRIMARY_CURSOR, xPosition, yPosition, height );
      mDecorator->HidePopup();
      mDecoratorUpdated = true;
    }
    else if ( mGrabHandlePopupEnabled &&
              GRAB_HANDLE_RELEASED == state )
    {
      mDecorator->ShowPopup();
    }
  }

  void ChangeState( State newState )
  {
    if( mState != newState )
    {
      mState = newState;

      if( INACTIVE == mState )
      {
        mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
        mDecorator->StopCursorBlink();
        mDecorator->SetGrabHandleActive( false );
        mDecorator->SetSelectionActive( false );
        mDecorator->HidePopup();
        mDecoratorUpdated = true;
      }
      else if ( SELECTING == mState )
      {
        mDecorator->SetActiveCursor( ACTIVE_CURSOR_NONE );
        mDecorator->StopCursorBlink();
        mDecorator->SetGrabHandleActive( false );
        mDecorator->SetSelectionActive( true );
        mDecoratorUpdated = true;
      }
      else if( EDITING == mState )
      {
        mDecorator->SetActiveCursor( ACTIVE_CURSOR_PRIMARY );
        if( mCursorBlinkEnabled )
        {
          mDecorator->StartCursorBlink();
        }
        if( mGrabHandleEnabled )
        {
          mDecorator->SetGrabHandleActive( true );
        }
        mDecorator->SetSelectionActive( false );
        mDecoratorUpdated = true;
      }
    }
  }

  LineIndex GetClosestLine( float y )
  {
    LineIndex lineIndex( 0u );

    const Vector<LineRun>& lines = mVisualModel->mLines;
    for( float totalHeight = 0; lineIndex < lines.Count(); ++lineIndex )
    {
      totalHeight += lines[lineIndex].lineSize.height;
      if( y < totalHeight )
      {
        break;
      }
    }

    return lineIndex;
  }

  void GetClosestCursorPosition( CharacterIndex& logical, float& visualX, float& visualY, float& height )
  {
    Length numberOfGlyphs = mVisualModel->mGlyphs.Count();
    Length numberOfLines  = mVisualModel->mLines.Count();
    if( 0 == numberOfGlyphs ||
        0 == numberOfLines )
    {
      return;
    }

    // Transform to visual model coords
    visualX -= mScrollPosition.x;
    visualY -= mScrollPosition.y;

    // Find which line is closest
    LineIndex lineIndex( GetClosestLine( visualY ) );

    const Vector<GlyphInfo>& glyphs = mVisualModel->mGlyphs;
    const GlyphInfo* const glyphsBuffer = glyphs.Begin();

    const Vector<Vector2>& positions = mVisualModel->mGlyphPositions;
    const Vector2* const positionsBuffer = positions.Begin();

    unsigned int closestGlyph = 0;
    bool leftOfGlyph( false ); // which side of the glyph?
    float closestDistance = MAX_FLOAT;

    const LineRun& line = mVisualModel->mLines[lineIndex];
    GlyphIndex startGlyph = line.glyphIndex;
    GlyphIndex endGlyph   = line.glyphIndex + line.numberOfGlyphs;
    DALI_ASSERT_DEBUG( endGlyph <= glyphs.Count() && "Invalid line info" );

    for( GlyphIndex i = startGlyph; i < endGlyph; ++i )
    {
      const GlyphInfo& glyphInfo = *( glyphsBuffer + i );
      const Vector2& position = *( positionsBuffer + i );
      float glyphX = position.x + glyphInfo.width*0.5f;
      float glyphY = position.y + glyphInfo.height*0.5f;

      float distanceToGlyph = fabsf( glyphX - visualX ) + fabsf( glyphY - visualY );

      if( distanceToGlyph < closestDistance )
      {
        closestDistance = distanceToGlyph;
        closestGlyph = i;
        leftOfGlyph = ( visualX < glyphX );
      }
    }

    // Calculate the logical position
    logical = mVisualModel->GetCharacterIndex( closestGlyph );

    // Returns the visual position of the glyph
    visualX = positions[closestGlyph].x;
    if( !leftOfGlyph )
    {
      visualX += glyphs[closestGlyph].width;

      //if( LTR ) TODO
        ++logical;
    }
    else// if ( RTL ) TODO
    {
      //++logical;
    }
    visualY = 0.0f;

    height = line.lineSize.height;
  }

  void UpdateCursorPosition()
  {
    if( 0 == mVisualModel->mGlyphs.Count() )
    {
      return;
    }

    // FIXME GetGlyphIndex() is behaving strangely
#if 0
    GlyphIndex cursorGlyph = mVisualModel->GetGlyphIndex( mPrimaryCursorPosition );
#else
    GlyphIndex cursorGlyph( 0u );
    for( cursorGlyph = 0; cursorGlyph < mVisualModel->mGlyphs.Count(); ++cursorGlyph )
    {
      if( mPrimaryCursorPosition == mVisualModel->GetCharacterIndex( cursorGlyph ) )
      {
        break;
      }
    }
#endif

    float visualX( 0.0f );
    float visualY( 0.0f );
    LineIndex lineIndex( 0u );
    const Vector<LineRun>& lineRuns = mVisualModel->mLines;

    if( cursorGlyph > 0 )
    {
      --cursorGlyph;

      visualX = mVisualModel->mGlyphPositions[ cursorGlyph ].x;
      //if( LTR ) TODO
        visualX += mVisualModel->mGlyphs[ cursorGlyph ].width;

      // Find the line height
      for( GlyphIndex lastGlyph = 0; lineIndex < lineRuns.Count(); ++lineIndex )
      {
        lastGlyph = (lineRuns[lineIndex].glyphIndex + lineRuns[lineIndex].numberOfGlyphs);
        if( cursorGlyph < lastGlyph )
        {
          break;
        }
      }
    }

    mDecorator->SetPosition( PRIMARY_CURSOR, visualX, visualY, lineRuns[lineIndex].lineSize.height );
    mDecoratorUpdated = true;
  }

  LogicalModelPtr mLogicalModel;
  VisualModelPtr  mVisualModel;
  DecoratorPtr    mDecorator;

  std::string mPlaceholderText;

  /**
   * This is used to delay handling events until after the model has been updated.
   * The number of updates to the model is minimized to improve performance.
   */
  vector<Event> mEventQueue; ///< The queue of touch events etc.

  State mState; ///< Selection mode, edit mode etc.

  CharacterIndex mPrimaryCursorPosition;   ///< Index into logical model for primary cursor
  CharacterIndex mSecondaryCursorPosition; ///< Index into logical model for secondary cursor

  /**
   * 0,0 means that the top-left corner of the layout matches the top-left corner of the UI control.
   * Typically this will have a negative value with scrolling occurs.
   */
  Vector2 mScrollPosition; ///< The text is offset by this position when scrolling.

  bool mDecoratorUpdated           : 1; ///< True if the decorator was updated during event processing
  bool mCursorBlinkEnabled         : 1; ///< True if cursor should blink when active
  bool mGrabHandleEnabled          : 1; ///< True if grab handle is enabled
  bool mGrabHandlePopupEnabled     : 1; ///< True if the grab handle popu-up should be shown
  bool mSelectionEnabled           : 1; ///< True if selection handles, highlight etc. are enabled
  bool mHorizontalScrollingEnabled : 1; ///< True if horizontal scrolling is enabled
  bool mVerticalScrollingEnabled   : 1; ///< True if vertical scrolling is enabled
  bool mUpdateCursorPosition       : 1; ///< True if the visual position of the cursor must be recalculated
};

struct Controller::FontDefaults
{
  FontDefaults()
  : mDefaultPointSize(0.0f),
    mFontId(0u)
  {
  }

  FontId GetFontId( TextAbstraction::FontClient& fontClient )
  {
    if( !mFontId )
    {
      Dali::TextAbstraction::PointSize26Dot6 pointSize = mDefaultPointSize*64;
      mFontId = fontClient.GetFontId( mDefaultFontFamily, mDefaultFontStyle, pointSize );
    }

    return mFontId;
  }

  std::string mDefaultFontFamily;
  std::string mDefaultFontStyle;
  float mDefaultPointSize;
  FontId mFontId;
};

struct Controller::Impl
{
  Impl( ControlInterface& controlInterface )
  : mControlInterface( controlInterface ),
    mLogicalModel(),
    mVisualModel(),
    mFontDefaults( NULL ),
    mTextInput( NULL ),
    mFontClient(),
    mView(),
    mLayoutEngine(),
    mModifyEvents(),
    mControlSize(),
    mOperationsPending( NO_OPERATION ),
    mRecalculateNaturalSize( true )
  {
    mLogicalModel = LogicalModel::New();
    mVisualModel  = VisualModel::New();

    mFontClient = TextAbstraction::FontClient::Get();

    mView.SetVisualModel( mVisualModel );
  }

  ~Impl()
  {
    delete mTextInput;
  }

  ControlInterface& mControlInterface;     ///< Reference to the text controller.
  LogicalModelPtr mLogicalModel;           ///< Pointer to the logical model.
  VisualModelPtr  mVisualModel;            ///< Pointer to the visual model.
  FontDefaults* mFontDefaults;             ///< Avoid allocating this when the user does not specify a font.
  Controller::TextInput* mTextInput;       ///< Avoid allocating everything for text input until EnableTextInput().
  TextAbstraction::FontClient mFontClient; ///< Handle to the font client.
  View mView;                              ///< The view interface to the rendering back-end.
  LayoutEngine mLayoutEngine;              ///< The layout engine.
  std::vector<ModifyEvent> mModifyEvents;  ///< Temporary stores the text set until the next relayout.
  Size mControlSize;                       ///< The size of the control.
  OperationsMask mOperationsPending;       ///< Operations pending to be done to layout the text.
  bool mRecalculateNaturalSize:1;          ///< Whether the natural size needs to be recalculated.
};

ControllerPtr Controller::New( ControlInterface& controlInterface )
{
  return ControllerPtr( new Controller( controlInterface ) );
}

void Controller::SetText( const std::string& text )
{
  // Cancel previously queued inserts etc.
  mImpl->mModifyEvents.clear();

  // Keep until size negotiation
  ModifyEvent event;
  event.type = REPLACE_TEXT;
  event.text = text;
  mImpl->mModifyEvents.push_back( event );

  if( mImpl->mTextInput )
  {
    // Cancel previously queued events
    mImpl->mTextInput->mEventQueue.clear();

    // TODO - Hide selection decorations
  }
}

void Controller::GetText( std::string& text ) const
{
  if( !mImpl->mModifyEvents.empty() &&
       REPLACE_TEXT == mImpl->mModifyEvents[0].type )
  {
    text = mImpl->mModifyEvents[0].text;
  }
  else
  {
    // TODO - Convert from UTF-32
  }
}

void Controller::SetPlaceholderText( const std::string& text )
{
  if( !mImpl->mTextInput )
  {
    mImpl->mTextInput->mPlaceholderText = text;
  }
}

void Controller::GetPlaceholderText( std::string& text ) const
{
  if( !mImpl->mTextInput )
  {
    text = mImpl->mTextInput->mPlaceholderText;
  }
}

void Controller::SetDefaultFontFamily( const std::string& defaultFontFamily )
{
  if( !mImpl->mFontDefaults )
  {
    mImpl->mFontDefaults = new Controller::FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultFontFamily = defaultFontFamily;
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;
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
    mImpl->mFontDefaults = new Controller::FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultFontStyle = defaultFontStyle;
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;
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
    mImpl->mFontDefaults = new Controller::FontDefaults();
  }

  mImpl->mFontDefaults->mDefaultPointSize = pointSize;
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;
}

float Controller::GetDefaultPointSize() const
{
  if( mImpl->mFontDefaults )
  {
    return mImpl->mFontDefaults->mDefaultPointSize;
  }

  return 0.0f;
}

void Controller::GetDefaultFonts( Vector<FontRun>& fonts, Length numberOfCharacters )
{
  if( mImpl->mFontDefaults )
  {
    FontRun fontRun;
    fontRun.characterRun.characterIndex = 0;
    fontRun.characterRun.numberOfCharacters = numberOfCharacters;
    fontRun.fontId = mImpl->mFontDefaults->GetFontId( mImpl->mFontClient );
    fontRun.isDefault = true;

    fonts.PushBack( fontRun );
  }
}

void Controller::EnableTextInput( DecoratorPtr decorator )
{
  if( !mImpl->mTextInput )
  {
    mImpl->mTextInput = new TextInput( mImpl->mLogicalModel, mImpl->mVisualModel, decorator );
  }
}

void Controller::SetEnableCursorBlink( bool enable )
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mTextInput && "TextInput disabled" );

  if( mImpl->mTextInput )
  {
    mImpl->mTextInput->mCursorBlinkEnabled = enable;

    if( !enable &&
        mImpl->mTextInput->mDecorator )
    {
      mImpl->mTextInput->mDecorator->StopCursorBlink();
    }
  }
}

bool Controller::GetEnableCursorBlink() const
{
  if( mImpl->mTextInput )
  {
    return mImpl->mTextInput->mCursorBlinkEnabled;
  }

  return false;
}

const Vector2& Controller::GetScrollPosition() const
{
  if( mImpl->mTextInput )
  {
    return mImpl->mTextInput->mScrollPosition;
  }

  return Vector2::ZERO;
}

Vector3 Controller::GetNaturalSize()
{
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
    UpdateModel( onlyOnceOperations );

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
  }
  else
  {
    naturalSize = mImpl->mVisualModel->GetNaturalSize();
  }

  return naturalSize;
}

float Controller::GetHeightForWidth( float width )
{
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
    UpdateModel( onlyOnceOperations );

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
  }
  else
  {
    layoutSize = mImpl->mVisualModel->GetActualSize();
  }

  return layoutSize.height;
}

bool Controller::Relayout( const Vector2& size )
{
  if( ( size.width < Math::MACHINE_EPSILON_1000 ) || ( size.height < Math::MACHINE_EPSILON_1000 ) )
  {
    bool glyphsRemoved( false );
    if( 0u != mImpl->mVisualModel->GetNumberOfGlyphPositions() )
    {
      mImpl->mVisualModel->SetGlyphPositions( NULL, 0u );
      glyphsRemoved = true;
    }

    // Not worth to relayout if width or height is equal to zero.
    return glyphsRemoved;
  }

  if( size != mImpl->mControlSize )
  {
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
  UpdateModel( mImpl->mOperationsPending );

  Size layoutSize;
  bool updated = DoRelayout( mImpl->mControlSize,
                             mImpl->mOperationsPending,
                             layoutSize );

  // Do not re-do any operation until something changes.
  mImpl->mOperationsPending = NO_OPERATION;

  if( mImpl->mTextInput )
  {
    // Move the cursor, grab handle etc.
    updated = mImpl->mTextInput->ProcessInputEvents( mImpl->mControlSize ) || updated;
  }

  return updated;
}

void Controller::ProcessModifyEvents()
{
  std::vector<ModifyEvent>& events = mImpl->mModifyEvents;

  for( unsigned int i=0; i<events.size(); ++i )
  {
    if( REPLACE_TEXT == events[0].type )
    {
      // A (single) replace event should come first, otherwise we wasted time processing NOOP events
      DALI_ASSERT_DEBUG( 0 == i && "Unexpected REPLACE event" );

      ReplaceTextEvent( events[0].text );
    }
    else if( INSERT_TEXT == events[0].type )
    {
      InsertTextEvent( events[0].text );
    }
    else if( DELETE_TEXT == events[0].type )
    {
      DeleteTextEvent();
    }
  }

  // Discard temporary text
  events.clear();
}

void Controller::ReplaceTextEvent( const std::string& text )
{
  // Reset buffers.
  mImpl->mLogicalModel->mText.Clear();
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

  //  Convert text into UTF-32
  Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;
  utf32Characters.Resize( text.size() );

  // This is a bit horrible but std::string returns a (signed) char*
  const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

  // Transform a text array encoded in utf8 into an array encoded in utf32.
  // It returns the actual number of characters.
  Length characterCount = Utf8ToUtf32( utf8, text.size(), utf32Characters.Begin() );
  utf32Characters.Resize( characterCount );

  // Reset the cursor position
  if( mImpl->mTextInput )
  {
    mImpl->mTextInput->mPrimaryCursorPosition = characterCount;
    // TODO - handle secondary cursor
  }

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model
  mImpl->mOperationsPending = ALL_OPERATIONS;
  UpdateModel( ALL_OPERATIONS );
  mImpl->mOperationsPending = static_cast<OperationsMask>( LAYOUT             |
                                                           ALIGN              |
                                                           UPDATE_ACTUAL_SIZE |
                                                           REORDER );
}

void Controller::InsertTextEvent( const std::string& text )
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mTextInput && "Unexpected InsertTextEvent" );

  // TODO - Optimize this
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

  //  Convert text into UTF-32
  Vector<Character> utf32Characters;
  utf32Characters.Resize( text.size() );

  // This is a bit horrible but std::string returns a (signed) char*
  const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

  // Transform a text array encoded in utf8 into an array encoded in utf32.
  // It returns the actual number of characters.
  Length characterCount = Utf8ToUtf32( utf8, text.size(), utf32Characters.Begin() );
  utf32Characters.Resize( characterCount );

  // Insert at current cursor position
  Vector<Character>& modifyText = mImpl->mLogicalModel->mText;
  CharacterIndex& cursorIndex = mImpl->mTextInput->mPrimaryCursorPosition;

  if( cursorIndex < modifyText.Count() )
  {
    modifyText.Insert( modifyText.Begin() + cursorIndex, utf32Characters.Begin(), utf32Characters.End() );
  }
  else
  {
    modifyText.Insert( modifyText.End(), utf32Characters.Begin(), utf32Characters.End() );
  }

  // Advance the cursor position
  ++cursorIndex;

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;
  UpdateModel( ALL_OPERATIONS );
  mImpl->mOperationsPending = static_cast<OperationsMask>( LAYOUT             |
                                                           ALIGN              |
                                                           UPDATE_ACTUAL_SIZE |
                                                           REORDER );

  // Queue a cursor reposition event; this must wait until after DoRelayout()
  mImpl->mTextInput->mUpdateCursorPosition = true;
}

void Controller::DeleteTextEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mTextInput && "Unexpected InsertTextEvent" );

  // TODO - Optimize this
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

  // Delte at current cursor position
  Vector<Character>& modifyText = mImpl->mLogicalModel->mText;
  CharacterIndex& cursorIndex = mImpl->mTextInput->mPrimaryCursorPosition;

  if( cursorIndex > 0 &&
      cursorIndex-1 < modifyText.Count() )
  {
    modifyText.Remove( modifyText.Begin() + cursorIndex - 1 );

    // Cursor position retreat
    --cursorIndex;
  }

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Apply modifications to the model; TODO - Optimize this
  mImpl->mOperationsPending = ALL_OPERATIONS;
  UpdateModel( ALL_OPERATIONS );
  mImpl->mOperationsPending = static_cast<OperationsMask>( LAYOUT             |
                                                           ALIGN              |
                                                           UPDATE_ACTUAL_SIZE |
                                                           REORDER );

  // Queue a cursor reposition event; this must wait until after DoRelayout()
  mImpl->mTextInput->mUpdateCursorPosition = true;
}

void Controller::UpdateModel( OperationsMask operationsRequired )
{
  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mImpl->mOperationsPending & operationsRequired );

  Vector<Character>& utf32Characters = mImpl->mLogicalModel->mText;

  const Length numberOfCharacters = mImpl->mLogicalModel->GetNumberOfCharacters();

  Vector<LineBreakInfo>& lineBreakInfo = mImpl->mLogicalModel->mLineBreakInfo;
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

  Vector<WordBreakInfo>& wordBreakInfo = mImpl->mLogicalModel->mWordBreakInfo;
  if( GET_WORD_BREAKS & operations )
  {
    // Retrieves the word break info. The word break info is used to layout the text (where to wrap the text in lines).
    wordBreakInfo.Resize( numberOfCharacters, TextAbstraction::WORD_NO_BREAK );

    SetWordBreakInfo( utf32Characters,
                      wordBreakInfo );
  }

  const bool getScripts = GET_SCRIPTS & operations;
  const bool validateFonts = VALIDATE_FONTS & operations;

  Vector<ScriptRun>& scripts = mImpl->mLogicalModel->mScriptRuns;
  Vector<FontRun>& validFonts = mImpl->mLogicalModel->mFontRuns;

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

    Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = mImpl->mLogicalModel->mBidirectionalParagraphInfo;
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
      Vector<CharacterDirection>& directions = mImpl->mLogicalModel->mCharacterDirections;
      directions.Resize( numberOfCharacters );

      GetCharactersDirection( bidirectionalInfo,
                              directions );
    }
    else
    {
      // There is no right to left characters. Clear the directions vector.
      mImpl->mLogicalModel->mCharacterDirections.Clear();
    }

   }

  Vector<GlyphInfo>& glyphs = mImpl->mVisualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = mImpl->mVisualModel->mGlyphsToCharacters;
  Vector<Length>& charactersPerGlyph = mImpl->mVisualModel->mCharactersPerGlyph;
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
  }

  const Length numberOfGlyphs = glyphs.Count();

  if( GET_GLYPH_METRICS & operations )
  {
    mImpl->mFontClient.GetGlyphMetrics( glyphs.Begin(), numberOfGlyphs );
  }

  if( 0u != numberOfGlyphs )
  {
    // Create the glyph to character conversion table and the 'number of glyphs' per character.
    mImpl->mVisualModel->CreateCharacterToGlyphTable(numberOfCharacters );
    mImpl->mVisualModel->CreateGlyphsPerCharacterTable( numberOfCharacters );
  }
}

bool Controller::DoRelayout( const Vector2& size,
                             OperationsMask operationsRequired,
                             Size& layoutSize )
{
  bool viewUpdated( false );

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mImpl->mOperationsPending & operationsRequired );

  if( LAYOUT & operations )
  {
    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    Length numberOfGlyphs = mImpl->mVisualModel->GetNumberOfGlyphs();

    Vector<LineBreakInfo>& lineBreakInfo = mImpl->mLogicalModel->mLineBreakInfo;
    Vector<WordBreakInfo>& wordBreakInfo = mImpl->mLogicalModel->mWordBreakInfo;
    Vector<GlyphInfo>& glyphs = mImpl->mVisualModel->mGlyphs;
    Vector<CharacterIndex>& glyphsToCharactersMap = mImpl->mVisualModel->mGlyphsToCharacters;
    Vector<Length>& charactersPerGlyph = mImpl->mVisualModel->mCharactersPerGlyph;

    // Set the layout parameters.
    LayoutParameters layoutParameters( size,
                                       mImpl->mLogicalModel->mText.Begin(),
                                       lineBreakInfo.Begin(),
                                       wordBreakInfo.Begin(),
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
          const Length numberOfLines = mImpl->mVisualModel->GetNumberOfLines();

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

      if( ALIGN & operations )
      {
        mImpl->mLayoutEngine.Align( layoutParameters,
                                    lines,
                                    glyphPositions );
      }

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

  return viewUpdated;
}

View& Controller::GetView()
{
  return mImpl->mView;
}

LayoutEngine& Controller::GetLayoutEngine()
{
  return mImpl->mLayoutEngine;
}

void Controller::RequestRelayout()
{
  mImpl->mControlInterface.RequestTextRelayout();
}

void Controller::KeyboardFocusGainEvent()
{
  DALI_ASSERT_DEBUG( mImpl->mTextInput && "Unexpected KeyboardFocusGainEvent" );

  if( mImpl->mTextInput )
  {
    TextInput::Event event( TextInput::KEYBOARD_FOCUS_GAIN_EVENT );
    mImpl->mTextInput->mEventQueue.push_back( event );

    RequestRelayout();
  }
}

void Controller::KeyboardFocusLostEvent()
{
  DALI_ASSERT_DEBUG( mImpl->mTextInput && "Unexpected KeyboardFocusLostEvent" );

  if( mImpl->mTextInput )
  {
    TextInput::Event event( TextInput::KEYBOARD_FOCUS_LOST_EVENT );
    mImpl->mTextInput->mEventQueue.push_back( event );

    RequestRelayout();
  }
}

bool Controller::KeyEvent( const Dali::KeyEvent& keyEvent )
{
  DALI_ASSERT_DEBUG( mImpl->mTextInput && "Unexpected KeyEvent" );

  if( mImpl->mTextInput &&
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
      TextInput::Event event( TextInput::CURSOR_KEY_EVENT );
      event.p1.mInt = keyCode;
      mImpl->mTextInput->mEventQueue.push_back( event );
    }
    else if( Dali::DALI_KEY_BACKSPACE == keyCode )
    {
      // Queue a delete event
      ModifyEvent event;
      event.type = DELETE_TEXT;
      mImpl->mModifyEvents.push_back( event );
    }
    else if( !keyString.empty() )
    {
      // Queue an insert event
      ModifyEvent event;
      event.type = INSERT_TEXT;
      event.text = keyString;
      mImpl->mModifyEvents.push_back( event );
    }

    RequestRelayout();
  }

  return false;
}

void Controller::TapEvent( unsigned int tapCount, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mTextInput && "Unexpected TapEvent" );

  if( mImpl->mTextInput )
  {
    TextInput::Event event( TextInput::TAP_EVENT );
    event.p1.mUint = tapCount;
    event.p2.mFloat = x;
    event.p3.mFloat = y;
    mImpl->mTextInput->mEventQueue.push_back( event );

    RequestRelayout();
  }
}

void Controller::PanEvent( Gesture::State state, const Vector2& displacement )
{
  DALI_ASSERT_DEBUG( mImpl->mTextInput && "Unexpected PanEvent" );

  if( mImpl->mTextInput )
  {
    TextInput::Event event( TextInput::PAN_EVENT );
    event.p1.mInt = state;
    event.p2.mFloat = displacement.x;
    event.p3.mFloat = displacement.y;
    mImpl->mTextInput->mEventQueue.push_back( event );

    RequestRelayout();
  }
}

void Controller::GrabHandleEvent( GrabHandleState state, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mTextInput && "Unexpected GrabHandleEvent" );

  if( mImpl->mTextInput )
  {
    TextInput::Event event( TextInput::GRAB_HANDLE_EVENT );
    event.p1.mUint  = state;
    event.p2.mFloat = x;
    event.p3.mFloat = y;
    mImpl->mTextInput->mEventQueue.push_back( event );

    RequestRelayout();
  }
}

Controller::~Controller()
{
  delete mImpl;
}

Controller::Controller( ControlInterface& controlInterface )
: mImpl( NULL )
{
  mImpl = new Controller::Impl( controlInterface );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
