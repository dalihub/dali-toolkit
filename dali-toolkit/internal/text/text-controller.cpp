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
#include <dali-toolkit/internal/text/logical-model.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/text/visual-model.h>

using std::vector;

namespace
{
const float MAX_FLOAT = std::numeric_limits<float>::max();
const std::string EMPTY_STRING;
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
    KEY_EVENT,
    TAP_EVENT,
    GRAB_HANDLE_EVENT
  };

  union Param
  {
    int mInt;
    unsigned int mUint;
    float mFloat;
    char* mString;
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
    mDecoratorUpdated( false ),
    mCursorBlinkEnabled( true )
  {
  }

  /**
   * @brief Helper to move the cursor, grab handle etc.
   */
  bool ProcessInputEvents()
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
          case KEY_EVENT:
          {
            OnKeyEvent( *iter );
            break;
          }
          case TAP_EVENT:
          {
            OnTapEvent( *iter );
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

  void OnKeyEvent( const Event& event )
  {
    int keyCode = event.p1.mInt;

    // Handle state changes
    if( Dali::DALI_KEY_ESCAPE == keyCode )
    {
      ChangeState( INACTIVE ); // Escape key ends edit mode
    }
    else if ( event.p2.mString )
    {
      // Some text may be selected, hiding keyboard causes an empty keystring to be sent, we don't want to delete highlight in this case
      ChangeState( EDITING );
    }

    // Handle the actual key event
    if( Dali::DALI_KEY_BACKSPACE == keyCode )
    {
      HandleBackspaceKey();
    }
    else if( Dali::DALI_KEY_CURSOR_LEFT  == keyCode ||
             Dali::DALI_KEY_CURSOR_RIGHT == keyCode ||
             Dali::DALI_KEY_CURSOR_UP    == keyCode ||
             Dali::DALI_KEY_CURSOR_DOWN  == keyCode )
    {
      HandleCursorKey( keyCode );
    }
    else if ( event.p2.mString )
    {
      HandleKeyString( event.p2.mString );

      delete [] event.p2.mString;
    }
  }

  void HandleBackspaceKey()
  {
    // TODO
  }

  void HandleCursorKey( int keyCode )
  {
    // TODO
  }

  void HandleKeyString( const char* keyString )
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
      GetClosestCursorPosition( xPosition, yPosition, height );
      mDecorator->SetPosition( PRIMARY_CURSOR, xPosition, yPosition, height );

      mDecoratorUpdated = true;
    }
    else if( 2u == tapCount )
    {
      ChangeState( SELECTING );
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

      GetClosestCursorPosition( xPosition, yPosition, height );

      mDecorator->SetPosition( PRIMARY_CURSOR, xPosition, yPosition, height );
      mDecorator->HidePopup();
      mDecoratorUpdated = true;
    }
    else if ( GRAB_HANDLE_RELEASED == state )
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
        mDecorator->SetGrabHandleActive( true );
        mDecorator->SetSelectionActive( false );
        mDecoratorUpdated = true;
      }
    }
  }

  void GetClosestCursorPosition( float& x, float& y, float& height )
  {
    // TODO - Look at LineRuns first

    Text::Length numberOfGlyphs = mVisualModel->GetNumberOfGlyphs();
    if( 0 == numberOfGlyphs )
    {
      return;
    }

    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );
    mVisualModel->GetGlyphs( glyphs.Begin(), 0, numberOfGlyphs );
    const GlyphInfo* const glyphsBuffer = glyphs.Begin();

    Vector<Vector2> positions;
    positions.Resize( numberOfGlyphs );
    mVisualModel->GetGlyphPositions( positions.Begin(), 0, numberOfGlyphs );
    const Vector2* const positionsBuffer = positions.Begin();

    unsigned int closestGlyph = 0;
    float closestDistance = MAX_FLOAT;

    for( unsigned int i = 0, numberOfGLyphs = glyphs.Count(); i < numberOfGLyphs; ++i )
    {
      const GlyphInfo& glyphInfo = *( glyphsBuffer + i );
      const Vector2& position = *( positionsBuffer + i );
      float glyphX = position.x + glyphInfo.width*0.5f;
      float glyphY = position.y + glyphInfo.height*0.5f;

      float distanceToGlyph = fabsf( glyphX - x ) + fabsf( glyphY - y );

      if( distanceToGlyph < closestDistance )
      {
        closestDistance = distanceToGlyph;
        closestGlyph = i;
      }
    }

    // TODO - Consider RTL languages
    x = positions[closestGlyph].x + glyphs[closestGlyph].width;
    y = 0.0f;

    FontMetrics metrics;
    TextAbstraction::FontClient::Get().GetFontMetrics( glyphs[closestGlyph].fontId, metrics );
    height = metrics.height; // TODO - Fix for multi-line
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

  State mState;

  bool mDecoratorUpdated   : 1;
  bool mCursorBlinkEnabled : 1;
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
    mNewText(),
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
  std::string mNewText;                    ///< Temporary stores the text set until the next relayout.
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
  // Keep until size negotiation
  mImpl->mNewText = text;

  // All operations need to be done. (convert to utf32, get break info, ..., layout, ...)
  mImpl->mOperationsPending = ALL_OPERATIONS;

  // The natural size needs to be re-calculated.
  mImpl->mRecalculateNaturalSize = true;

  // Reset buffers.
  mImpl->mLogicalModel->SetText( NULL, 0u );
  mImpl->mLogicalModel->SetScripts( NULL, 0u );
  mImpl->mLogicalModel->SetFonts( NULL, 0u );
  mImpl->mLogicalModel->SetLineBreakInfo( NULL, 0u );
  mImpl->mLogicalModel->SetWordBreakInfo( NULL, 0u );
  mImpl->mLogicalModel->SetBidirectionalInfo( NULL, 0u );
  mImpl->mLogicalModel->SetVisualToLogicalMap( NULL, 0u );
  mImpl->mVisualModel->SetGlyphs( NULL, NULL, NULL, 0u );
  mImpl->mVisualModel->SetGlyphPositions( NULL, 0u );
  mImpl->mVisualModel->SetLines( NULL, 0u );

  if( mImpl->mTextInput )
  {
    // Cancel previously queued events
    mImpl->mTextInput->mEventQueue.clear();

    // TODO - Hide selection decorations
  }
}

void Controller::GetText( std::string& text ) const
{
  if( !mImpl->mNewText.empty() )
  {
    text = mImpl->mNewText;
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
                                                             UPDATE_ACTUAL_SIZE        |
                                                             UPDATE_POSITIONS          |
                                                             UPDATE_LINES              |
                                                             REORDER );

    mImpl->mControlSize = size;
  }

  Size layoutSize;
  bool updated = DoRelayout( mImpl->mControlSize,
                             mImpl->mOperationsPending,
                             layoutSize );

  // Do not re-do any operation until something changes.
  mImpl->mOperationsPending = NO_OPERATION;

  if( mImpl->mTextInput )
  {
    // Move the cursor, grab handle etc.
    updated = mImpl->mTextInput->ProcessInputEvents() || updated;
  }

  return updated;
}

bool Controller::DoRelayout( const Vector2& size,
                             OperationsMask operationsRequired,
                             Size& layoutSize )
{
  bool viewUpdated( false );

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>( mImpl->mOperationsPending & operationsRequired );

  Vector<Character> utf32Characters;
  Length characterCount = 0u;
  if( CONVERT_TO_UTF32 & operations )
  {
    std::string& text = mImpl->mNewText;

    //  Convert text into UTF-32
    utf32Characters.Resize( text.size() );

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    characterCount = Utf8ToUtf32( utf8, text.size(), utf32Characters.Begin() );
    utf32Characters.Resize( characterCount );

    // Sets the text into the model.
    mImpl->mLogicalModel->SetText( utf32Characters.Begin(), characterCount );

    // Discard temporary text
    text.clear();
  }

  const Length numberOfCharacters = mImpl->mLogicalModel->GetNumberOfCharacters();

  Vector<LineBreakInfo> lineBreakInfo;
  if( GET_LINE_BREAKS & operations )
  {
    // Retrieves the line break info. The line break info is used to split the text in 'paragraphs' to
    // calculate the bidirectional info for each 'paragraph'.
    // It's also used to layout the text (where it should be a new line) or to shape the text (text in different lines
    // is not shaped together).
    lineBreakInfo.Resize( characterCount, TextAbstraction::LINE_NO_BREAK );

    SetLineBreakInfo( utf32Characters,
                      lineBreakInfo );

    mImpl->mLogicalModel->SetLineBreakInfo( lineBreakInfo.Begin(), characterCount );
  }

  Vector<WordBreakInfo> wordBreakInfo;
  if( GET_WORD_BREAKS & operations )
  {
    // Retrieves the word break info. The word break info is used to layout the text (where to wrap the text in lines).
    wordBreakInfo.Resize( characterCount, TextAbstraction::WORD_NO_BREAK );

    SetWordBreakInfo( utf32Characters,
                      wordBreakInfo );

    mImpl->mLogicalModel->SetWordBreakInfo( wordBreakInfo.Begin(), characterCount );
  }

  const bool getScripts = GET_SCRIPTS & operations;
  const bool validateFonts = VALIDATE_FONTS & operations;

  Vector<ScriptRun> scripts;
  Vector<FontRun> validFonts;

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

      // Sets the scripts into the model.
      mImpl->mLogicalModel->SetScripts( scripts.Begin(), scripts.Count() );
    }

    if( validateFonts )
    {
      // Copy the requested font defaults received via the property system.
      // These may not be valid i.e. may not contain glyphs for the necessary scripts.
      GetDefaultFonts( validFonts, numberOfCharacters );

      // Validates the fonts. If there is a character with no assigned font it sets a default one.
      // After this call, fonts are validated.
      multilanguageSupport.ValidateFonts( utf32Characters,
                                          scripts,
                                          validFonts );

      // Sets the fonts into the model.
      mImpl->mLogicalModel->SetFonts( validFonts.Begin(), validFonts.Count() );
    }
  }

  Vector<BidirectionalParagraphInfoRun> bidirectionalInfo;
  if( BIDI_INFO & operations )
  {
    // Some vectors with data needed to get the paragraph's bidirectional info may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    if( 0u == utf32Characters.Count() )
    {
      utf32Characters.Resize( numberOfCharacters );

      mImpl->mLogicalModel->GetText( utf32Characters.Begin(),
                                     0u,
                                     numberOfCharacters );
    }

    if( 0u == lineBreakInfo.Count() )
    {
      lineBreakInfo.Resize( numberOfCharacters );

      mImpl->mLogicalModel->GetLineBreakInfo( lineBreakInfo.Begin(),
                                              0u,
                                              numberOfCharacters );
    }

    if( 0u == scripts.Count() )
    {
      scripts.Resize( mImpl->mLogicalModel->GetNumberOfScriptRuns( 0u,
                                                                   numberOfCharacters ) );
      mImpl->mLogicalModel->GetScriptRuns( scripts.Begin(),
                                           0u,
                                           numberOfCharacters );
    }

    // Count the number of LINE_NO_BREAK to reserve some space for the vector of paragraph's
    // bidirectional info.

    Length numberOfParagraphs = 0u;

    const TextAbstraction::LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();
    for( Length index = 0u; index < characterCount; ++index )
    {
      if( TextAbstraction::LINE_NO_BREAK == *( lineBreakInfoBuffer + index ) )
      {
        ++numberOfParagraphs;
      }
    }

    bidirectionalInfo.Reserve( numberOfParagraphs );

    // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
    SetBidirectionalInfo( utf32Characters,
                          scripts,
                          lineBreakInfo,
                          bidirectionalInfo );

    mImpl->mLogicalModel->SetBidirectionalInfo( bidirectionalInfo.Begin(),
                                                bidirectionalInfo.Count() );
  }

  Vector<GlyphInfo> glyphs;
  Vector<CharacterIndex> glyphsToCharactersMap;
  Vector<Length> charactersPerGlyph;
  if( SHAPE_TEXT & operations )
  {
    if( 0u == validFonts.Count() )
    {
      validFonts.Resize( mImpl->mLogicalModel->GetNumberOfFontRuns( 0u,
                                                                    numberOfCharacters ) );
      mImpl->mLogicalModel->GetFontRuns( validFonts.Begin(),
                                         0u,
                                         numberOfCharacters );
    }

    // Shapes the text.
    ShapeText( utf32Characters,
               lineBreakInfo,
               scripts,
               validFonts,
               glyphs,
               glyphsToCharactersMap,
               charactersPerGlyph );
  }

  if( GET_GLYPH_METRICS & operations )
  {
    mImpl->mFontClient.GetGlyphMetrics( glyphs.Begin(), glyphs.Count() );
  }

  Length numberOfGlyphs = glyphs.Count();
  if( 0u != numberOfGlyphs )
  {
    // Sets the glyphs into the model.
    mImpl->mVisualModel->SetGlyphs( glyphs.Begin(),
                                    glyphsToCharactersMap.Begin(),
                                    charactersPerGlyph.Begin(),
                                    numberOfGlyphs );
  }

  if( LAYOUT & operations )
  {
    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    const Length numberOfCharacters = mImpl->mLogicalModel->GetNumberOfCharacters();
    numberOfGlyphs = mImpl->mVisualModel->GetNumberOfGlyphs();

    if( 0u == lineBreakInfo.Count() )
    {
      lineBreakInfo.Resize( numberOfCharacters );
      mImpl->mLogicalModel->GetLineBreakInfo( lineBreakInfo.Begin(),
                                              0u,
                                              numberOfCharacters );
    }

    if( 0u == wordBreakInfo.Count() )
    {
      wordBreakInfo.Resize( numberOfCharacters );
      mImpl->mLogicalModel->GetWordBreakInfo( wordBreakInfo.Begin(),
                                              0u,
                                              numberOfCharacters );
    }

    if( 0u == glyphs.Count() )
    {
      glyphs.Resize( numberOfGlyphs );
      mImpl->mVisualModel->GetGlyphs( glyphs.Begin(),
                                      0u,
                                      numberOfGlyphs );
    }

    if( 0u == glyphsToCharactersMap.Count() )
    {
      glyphsToCharactersMap.Resize( numberOfGlyphs );
      mImpl->mVisualModel->GetGlyphToCharacterMap( glyphsToCharactersMap.Begin(),
                                                   0u,
                                                   numberOfGlyphs );
    }

    if( 0u == charactersPerGlyph.Count() )
    {
      charactersPerGlyph.Resize( numberOfGlyphs );
      mImpl->mVisualModel->GetCharactersPerGlyphMap( charactersPerGlyph.Begin(),
                                                     0u,
                                                     numberOfGlyphs );
    }

    // Set the layout parameters.
    LayoutParameters layoutParameters( size,
                                       lineBreakInfo.Begin(),
                                       wordBreakInfo.Begin(),
                                       numberOfGlyphs,
                                       glyphs.Begin(),
                                       glyphsToCharactersMap.Begin(),
                                       charactersPerGlyph.Begin() );

    // Reserve space to set the positions of the glyphs.
    Vector<Vector2> glyphPositions;
    glyphPositions.Resize( numberOfGlyphs );

    // The laid-out lines.
    // It's not possible to know in how many lines the text is going to be laid-out,
    // but it can be resized at least with the number of 'paragraphs' to avoid
    // some re-allocations.
    Vector<LineRun> lines;
    lines.Reserve( mImpl->mLogicalModel->GetNumberOfBidirectionalInfoRuns( 0u, numberOfCharacters ) );

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
        const Length numberOfBidiParagraphs = mImpl->mLogicalModel->GetNumberOfBidirectionalInfoRuns( 0u, numberOfCharacters );

        if( 0u == bidirectionalInfo.Count() )
        {
          bidirectionalInfo.Resize( numberOfBidiParagraphs );
          mImpl->mLogicalModel->GetBidirectionalInfo( bidirectionalInfo.Begin(),
                                                      0u,
                                                      numberOfCharacters );
        }

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
          Vector<GlyphIndex> characterToGlyphMap;
          characterToGlyphMap.Resize( numberOfCharacters );

          layoutParameters.charactersToGlyphsBuffer = characterToGlyphMap.Begin();
          mImpl->mVisualModel->GetCharacterToGlyphMap( layoutParameters.charactersToGlyphsBuffer,
                                                       0u,
                                                       numberOfCharacters );

          // Get the glyphs per character table and set into the layout.
          Vector<Length> glyphsPerCharacter;
          glyphsPerCharacter.Resize( numberOfCharacters );

          layoutParameters.glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();
          mImpl->mVisualModel->GetGlyphsPerCharacterMap( layoutParameters.glyphsPerCharacterBuffer,
                                                         0u,
                                                         numberOfCharacters );

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
      }

      // Sets the positions into the model.
      if( UPDATE_POSITIONS & operations )
      {
        mImpl->mVisualModel->SetGlyphPositions( glyphPositions.Begin(),
                                                numberOfGlyphs );
      }

      // Sets the lines into the model.
      if( UPDATE_LINES & operations )
      {
        mImpl->mVisualModel->SetLines( lines.Begin(),
                                       lines.Count() );
      }

      // Sets the actual size.
      if( UPDATE_ACTUAL_SIZE & operations )
      {
        mImpl->mVisualModel->SetActualSize( layoutSize );
      }
    }
  }
  else
  {
    layoutSize = mImpl->mVisualModel->GetActualSize();
  }

  return viewUpdated;
}

Vector3 Controller::GetNaturalSize()
{
  Vector3 naturalSize;

  if( mImpl->mRecalculateNaturalSize )
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>( CONVERT_TO_UTF32  |
                                                                           GET_SCRIPTS       |
                                                                           VALIDATE_FONTS    |
                                                                           GET_LINE_BREAKS   |
                                                                           GET_WORD_BREAKS   |
                                                                           SHAPE_TEXT        |
                                                                           GET_GLYPH_METRICS );

    // Operations that need to be done if the size changes.
    const OperationsMask sizeOperations =  static_cast<OperationsMask>( LAYOUT |
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
  Size layoutSize;
  if( width != mImpl->mControlSize.width )
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>( CONVERT_TO_UTF32  |
                                                                           GET_SCRIPTS       |
                                                                           VALIDATE_FONTS    |
                                                                           GET_LINE_BREAKS   |
                                                                           GET_WORD_BREAKS   |
                                                                           SHAPE_TEXT        |
                                                                           GET_GLYPH_METRICS );

    // Operations that need to be done if the size changes.
    const OperationsMask sizeOperations =  static_cast<OperationsMask>( LAYOUT |
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

  if( mImpl->mTextInput )
  {
    TextInput::Event event( TextInput::KEY_EVENT );
    event.p1.mInt = keyEvent.keyCode;
    event.p2.mString = NULL;

    const std::string& keyString = keyEvent.keyPressed;
    if ( !keyString.empty() )
    {
      event.p2.mString = new char[keyString.size() + 1];
      std::copy(keyString.begin(), keyString.end(), event.p2.mString);
      event.p2.mString[keyString.size()] = '\0';
    }

    mImpl->mTextInput->mEventQueue.push_back( event );

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
