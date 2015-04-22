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
#include <dali/public-api/adaptor-framework/key.h>

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

const float MAX_FLOAT = std::numeric_limits<float>::max();

const std::string EMPTY_STRING("");

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

void Controller::SetText( const std::string& text )
{
  // Cancel previously queued inserts etc.
  mImpl->mModifyEvents.clear();

  // Keep until size negotiation
  ModifyEvent event;
  event.type = ModifyEvent::REPLACE_TEXT;
  event.text = text;
  mImpl->mModifyEvents.push_back( event );

  if( mImpl->mEventData )
  {
    // Cancel previously queued events
    mImpl->mEventData->mEventQueue.clear();

    // TODO - Hide selection decorations
  }
}

void Controller::GetText( std::string& text ) const
{
  if( !mImpl->mModifyEvents.empty() &&
       ModifyEvent::REPLACE_TEXT == mImpl->mModifyEvents[0].type )
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
  if( !mImpl->mEventData )
  {
    mImpl->mEventData->mPlaceholderText = text;
  }
}

void Controller::GetPlaceholderText( std::string& text ) const
{
  if( !mImpl->mEventData )
  {
    text = mImpl->mEventData->mPlaceholderText;
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
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;

  // Clear the font-specific data
  mImpl->mLogicalModel->mFontRuns.Clear();
  mImpl->mVisualModel->mGlyphs.Clear();
  mImpl->mVisualModel->mGlyphsToCharacters.Clear();
  mImpl->mVisualModel->mCharactersToGlyph.Clear();
  mImpl->mVisualModel->mCharactersPerGlyph.Clear();
  mImpl->mVisualModel->mGlyphsPerCharacter.Clear();
  mImpl->mVisualModel->mGlyphPositions.Clear();
  mImpl->mVisualModel->mLines.Clear();
  mImpl->mVisualModel->ClearCaches();

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
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;

  // Clear the font-specific data
  mImpl->mLogicalModel->mFontRuns.Clear();
  mImpl->mVisualModel->mGlyphs.Clear();
  mImpl->mVisualModel->mGlyphsToCharacters.Clear();
  mImpl->mVisualModel->mCharactersToGlyph.Clear();
  mImpl->mVisualModel->mCharactersPerGlyph.Clear();
  mImpl->mVisualModel->mGlyphsPerCharacter.Clear();
  mImpl->mVisualModel->mGlyphPositions.Clear();
  mImpl->mVisualModel->mLines.Clear();
  mImpl->mVisualModel->ClearCaches();

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
  mImpl->mFontDefaults->mFontId = 0u; // Remove old font ID
  mImpl->mOperationsPending = ALL_OPERATIONS;
  mImpl->mRecalculateNaturalSize = true;

  // Clear the font-specific data
  mImpl->mLogicalModel->mFontRuns.Clear();
  mImpl->mVisualModel->mGlyphs.Clear();
  mImpl->mVisualModel->mGlyphsToCharacters.Clear();
  mImpl->mVisualModel->mCharactersToGlyph.Clear();
  mImpl->mVisualModel->mCharactersPerGlyph.Clear();
  mImpl->mVisualModel->mGlyphsPerCharacter.Clear();
  mImpl->mVisualModel->mGlyphPositions.Clear();
  mImpl->mVisualModel->mLines.Clear();
  mImpl->mVisualModel->ClearCaches();

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

void Controller::GetDefaultFonts( Vector<FontRun>& fonts, Length numberOfCharacters ) const
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

void Controller::SetTextColor( const Vector4& textColor )
{
  mImpl->mVisualModel->SetTextColor( textColor );
}

const Vector4& Controller::GetTextColor() const
{
  return mImpl->mVisualModel->GetTextColor();
}

void Controller::SetShadowOffset( const Vector2& shadowOffset )
{
  mImpl->mVisualModel->SetShadowOffset( shadowOffset );
}

const Vector2& Controller::GetShadowOffset() const
{
  return mImpl->mVisualModel->GetShadowOffset();
}

void Controller::SetShadowColor( const Vector4& shadowColor )
{
  mImpl->mVisualModel->SetShadowColor( shadowColor );
}

const Vector4& Controller::GetShadowColor() const
{
  return mImpl->mVisualModel->GetShadowColor();
}

void Controller::SetUnderlineColor( const Vector4& color )
{
  mImpl->mVisualModel->SetUnderlineColor( color );
}

const Vector4& Controller::GetUnderlineColor() const
{
  return mImpl->mVisualModel->GetUnderlineColor();
}

void Controller::SetUnderlineEnabled( bool enabled )
{
  mImpl->mVisualModel->SetUnderlineEnabled( enabled );
}

bool Controller::IsUnderlineEnabled() const
{
  return mImpl->mVisualModel->IsUnderlineEnabled();
}

void Controller::SetUnderlineHeight( float height )
{
  mImpl->mVisualModel->SetUnderlineHeight( height );
}

float Controller::GetUnderlineHeight() const
{
  return mImpl->mVisualModel->GetUnderlineHeight();
}

void Controller::EnableTextInput( DecoratorPtr decorator )
{
  if( !mImpl->mEventData )
  {
    mImpl->mEventData = new EventData( decorator );
  }
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

bool Controller::Relayout( const Size& size )
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

  // After doing the text layout, the alignment offset to place the actor in the desired position can be calculated.
  CalculateTextAlignment( size );

  if( mImpl->mEventData )
  {
    // Move the cursor, grab handle etc.
    updated = mImpl->ProcessInputEvents() || updated;
  }

  return updated;
}

void Controller::ProcessModifyEvents()
{
  std::vector<ModifyEvent>& events = mImpl->mModifyEvents;

  for( unsigned int i=0; i<events.size(); ++i )
  {
    if( ModifyEvent::REPLACE_TEXT == events[0].type )
    {
      // A (single) replace event should come first, otherwise we wasted time processing NOOP events
      DALI_ASSERT_DEBUG( 0 == i && "Unexpected REPLACE event" );

      ReplaceTextEvent( events[0].text );
    }
    else if( ModifyEvent::INSERT_TEXT == events[0].type )
    {
      InsertTextEvent( events[0].text );
    }
    else if( ModifyEvent::DELETE_TEXT == events[0].type )
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
  mImpl->mVisualModel->ClearCaches();

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
  if( mImpl->mEventData )
  {
    mImpl->mEventData->mPrimaryCursorPosition = characterCount;
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
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected InsertTextEvent" );

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
  mImpl->mVisualModel->ClearCaches();

  //  Convert text into UTF-32
  Vector<Character> utf32Characters;
  utf32Characters.Resize( text.size() );

  // This is a bit horrible but std::string returns a (signed) char*
  const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

  // Transform a text array encoded in utf8 into an array encoded in utf32.
  // It returns the actual number of characters.
  Length characterCount = Utf8ToUtf32( utf8, text.size(), utf32Characters.Begin() );
  utf32Characters.Resize( characterCount );

  const Length numberOfCharactersInModel = mImpl->mLogicalModel->GetNumberOfCharacters();

  // Restrict new text to fit within Maximum characters setting
  Length maxSizeOfNewText = std::min ( ( mImpl->mMaximumNumberOfCharacters - numberOfCharactersInModel ), characterCount );

  // Insert at current cursor position
  CharacterIndex& cursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

  Vector<Character>& modifyText = mImpl->mLogicalModel->mText;

  if( cursorIndex < numberOfCharactersInModel )
  {
    modifyText.Insert( modifyText.Begin() + cursorIndex, utf32Characters.Begin(), utf32Characters.Begin()+ maxSizeOfNewText );
  }
  else
  {
    modifyText.Insert( modifyText.End(), utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText );
  }

  cursorIndex += maxSizeOfNewText;

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
  mImpl->mEventData->mUpdateCursorPosition = true;
  mImpl->mEventData->mScrollAfterUpdateCursorPosition = true;

  if ( characterCount > maxSizeOfNewText )
  {
    mImpl->mControlInterface.MaxLengthReached();
  }
}

void Controller::DeleteTextEvent()
{
  DALI_ASSERT_DEBUG( NULL != mImpl->mEventData && "Unexpected InsertTextEvent" );

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
  mImpl->mVisualModel->ClearCaches();

  // Delte at current cursor position
  Vector<Character>& modifyText = mImpl->mLogicalModel->mText;
  CharacterIndex& cursorIndex = mImpl->mEventData->mPrimaryCursorPosition;

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
  mImpl->mEventData->mUpdateCursorPosition = true;
  mImpl->mEventData->mScrollAfterUpdateCursorPosition = true;
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

    // Create the 'number of glyphs' per character and the glyph to character conversion tables.
    mImpl->mVisualModel->CreateGlyphsPerCharacterTable( numberOfCharacters );
    mImpl->mVisualModel->CreateCharacterToGlyphTable( numberOfCharacters );
  }

  const Length numberOfGlyphs = glyphs.Count();

  if( GET_GLYPH_METRICS & operations )
  {
    mImpl->mFontClient.GetGlyphMetrics( glyphs.Begin(), numberOfGlyphs );
  }
}

bool Controller::DoRelayout( const Size& size,
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

    if( 0u == numberOfGlyphs )
    {
      // Nothing else to do if there is no glyphs.
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
                                    layoutSize,
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
    Event event( Event::KEYBOARD_FOCUS_GAIN_EVENT );
    mImpl->mEventData->mEventQueue.push_back( event );

    mImpl->RequestRelayout();
  }
}

void Controller::KeyboardFocusLostEvent()
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected KeyboardFocusLostEvent" );

  if( mImpl->mEventData )
  {
    Event event( Event::KEYBOARD_FOCUS_LOST_EVENT );
    mImpl->mEventData->mEventQueue.push_back( event );

    mImpl->RequestRelayout();
  }
}

bool Controller::KeyEvent( const Dali::KeyEvent& keyEvent )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected KeyEvent" );

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
      // Queue a delete event
      ModifyEvent event;
      event.type = ModifyEvent::DELETE_TEXT;
      mImpl->mModifyEvents.push_back( event );
    }
    else if( !keyString.empty() )
    {
      // Queue an insert event
      ModifyEvent event;
      event.type = ModifyEvent::INSERT_TEXT;
      event.text = keyString;
      mImpl->mModifyEvents.push_back( event );
    }

    mImpl->ChangeState( EventData::EDITING ); // todo Confirm this is the best place to change the state of

    mImpl->RequestRelayout();
  }

  return false;
}

void Controller::TapEvent( unsigned int tapCount, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Unexpected TapEvent" );

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

void Controller::HandleEvent( HandleType handleType, HandleState state, float x, float y )
{
  DALI_ASSERT_DEBUG( mImpl->mEventData && "Controller::HandleEvent. Unexpected HandleEvent" );

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
