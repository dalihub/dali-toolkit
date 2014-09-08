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
#include <dali-toolkit/internal/controls/text-view/text-view-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/split-by-new-line-char-policies.h>
#include <dali-toolkit/internal/controls/text-view/split-by-word-policies.h>
#include <dali-toolkit/internal/controls/text-view/split-by-char-policies.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-word-processor.h>
#include <dali-toolkit/internal/controls/text-view/relayout-utilities.h>

namespace Dali
{

namespace Toolkit
{

const Property::Index TextView::PROPERTY_MARKUP_ENABLED( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX );
const Property::Index TextView::PROPERTY_TEXT( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 1 );
const Property::Index TextView::PROPERTY_MULTILINE_POLICY( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 2 );
const Property::Index TextView::PROPERTY_WIDTH_EXCEED_POLICY( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 3 );
const Property::Index TextView::PROPERTY_HEIGHT_EXCEED_POLICY( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 4 );
const Property::Index TextView::PROPERTY_LINE_JUSTIFICATION( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 5 );
const Property::Index TextView::PROPERTY_FADE_BOUNDARY( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 6 );
const Property::Index TextView::PROPERTY_LINE_HEIGHT_OFFSET( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 7 );
const Property::Index TextView::PROPERTY_HORIZONTAL_ALIGNMENT( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 8 );
const Property::Index TextView::PROPERTY_VERTICAL_ALIGNMENT( Internal::TextView::TEXTVIEW_PROPERTY_START_INDEX + 9 );

namespace Internal
{

namespace
{

const char* MULTILINE_POLICY_NAME[] = {"SplitByNewLineChar", "SplitByWord", "SplitByChar"};
const char* EXCEED_POLICY_NAME[] = {"Original", "Truncate", "Fade", "Split","ShrinkToFit","EllipsizeEnd"};
const char* LINE_JUSTIFICATION_NAME[] = {"Left","Center","Right","Justified"};

// Currently on desktop machines 2k x 2k is the maximum frame buffer size, on target is 4k x 4k.
const float MAX_OFFSCREEN_RENDERING_SIZE = 2048.f;

//Type Registration
BaseHandle Create()
{
  return Toolkit::TextView::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::TextView), typeid(Toolkit::Control), Create );

SignalConnectorType signalConnector1( typeRegistration, Toolkit::TextView::SIGNAL_TEXT_SCROLLED , &TextView::DoConnectSignal );

PropertyRegistration property1( typeRegistration,  "markup-enabled",       Toolkit::TextView::PROPERTY_MARKUP_ENABLED,       Property::BOOLEAN, &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property2( typeRegistration,  "text",                 Toolkit::TextView::PROPERTY_TEXT,                 Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property3( typeRegistration,  "multiline-policy",     Toolkit::TextView::PROPERTY_MULTILINE_POLICY,     Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property4( typeRegistration,  "width-exceed-policy",  Toolkit::TextView::PROPERTY_WIDTH_EXCEED_POLICY,  Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property5( typeRegistration,  "height-exceed-policy", Toolkit::TextView::PROPERTY_HEIGHT_EXCEED_POLICY, Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property6( typeRegistration,  "line-justification",   Toolkit::TextView::PROPERTY_LINE_JUSTIFICATION,   Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property7( typeRegistration,  "fade-boundary",        Toolkit::TextView::PROPERTY_FADE_BOUNDARY,        Property::VECTOR4, &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property8( typeRegistration, "line-height-offset",    Toolkit::TextView::PROPERTY_LINE_HEIGHT_OFFSET,   Property::FLOAT,   &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property9( typeRegistration, "horizontal-alignment",  Toolkit::TextView::PROPERTY_HORIZONTAL_ALIGNMENT, Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );
PropertyRegistration property10( typeRegistration, "vertical-alignment",   Toolkit::TextView::PROPERTY_VERTICAL_ALIGNMENT,   Property::STRING,  &TextView::SetProperty, &TextView::GetProperty );

/**
 * Whether the text-view-processor operation sets, inserts, replaces, removes text.
 *
 * @param[in] metadata The text-view-processor operation.
 *
 * @return \e true if the given text-view-processor operation is modifying the text.
 */
bool IsTextViewProcessorRelayoutOperation( const TextView::TextViewProcessorMetadata& metadata )
{
  return ( ( metadata.mType == TextView::TextSet ) ||
           ( metadata.mType == TextView::TextInserted ) ||
           ( metadata.mType == TextView::TextReplaced ) ||
           ( metadata.mType == TextView::TextRemoved ) ||
           ( metadata.mType == TextView::NewStyle ));
}

/**
 * Whether the text-view-processor operation sets a new line height offset.
 *
 * @param[in] metadata The text-view-processor operation.
 *
 * @return \e true if the given text-view-processor operation sets a new line height offset.
 */
bool IsTextViewProcessorLineHeightOffsetOperation( const TextView::TextViewProcessorMetadata& metadata )
{
  return ( metadata.mType == TextView::NewLineHeight );
}

/**
 * Whether the text-view-processor operation sets a new style.
 *
 * @param[in] metadata The text-view-processor operation.
 *
 * @return \e true if the given text-view-processor operation sets a new style.
 */
bool IsTextViewProcessorNewStyleOperation( const TextView::TextViewProcessorMetadata& metadata )
{
  return ( metadata.mType == TextView::NewStyle );
}

} // namespace

TextView::TextViewProcessorMetadata::TextViewProcessorMetadata()
: mType( TextView::TextSet ),
  mPosition( 0 ),
  mNumberOfCharacters( 0 ),
  mText()
{
}

Toolkit::TextView TextView::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<TextView> textView = new TextView();

  // Pass ownership to CustomActor
  Toolkit::TextView handle( *textView );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  textView->Initialize();

  // Disables by default the offscreen rendering.
  textView->SetSnapshotModeEnabled( false );

  return handle;
}

void TextView::SetText( const std::string& text )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( text, styledText, IsMarkupProcessingEnabled() );

  // Calls SetText() with the styled text array.
  SetText( styledText );
}

void TextView::SetText( const MarkupProcessor::StyledTextArray& text )
{
  // mTextViewProcessorOperations stores the InsertTextAt and RemoveTextFrom operations to transform the initial text to mCurrentStyledText.
  // Once again, if a new text is set, any previous call to InsertTextAt or RemoveTextFrom can be discarted.

  std::vector<TextViewProcessorMetadata>::iterator it = std::remove_if( mTextViewProcessorOperations.begin(), mTextViewProcessorOperations.end(), IsTextViewProcessorRelayoutOperation );
  mTextViewProcessorOperations.erase( it, mTextViewProcessorOperations.end() );

  // Creates metadata with the Set operation.
  TextViewProcessorMetadata metadata;
  metadata.mType = TextView::TextSet;
  metadata.mText = text;

  // Store metadata.
  mTextViewProcessorOperations.push_back( metadata );

  // Updates current styled text.
  mCurrentStyledText = text;

  // Request to be relaid out
  RelayoutRequest();

  // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed on order to retrieve the right values.
  mRelayoutOperations = RELAYOUT_ALL;
}

void TextView::InsertTextAt( std::size_t position, const std::string& text )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( text, styledText, IsMarkupProcessingEnabled() );

  // Calls InsertTextAt() with the styled text array.
  InsertTextAt( position, styledText );
}

void TextView::InsertTextAt( std::size_t position, const MarkupProcessor::StyledTextArray& text )
{
  // Creates metadata with the Insert operation.
  TextViewProcessorMetadata metadata;
  metadata.mType = TextView::TextInserted;
  metadata.mPosition = position;
  metadata.mText = text;

  // Store metadata.
  mTextViewProcessorOperations.push_back( metadata );

  // Updates current styled text.
  mCurrentStyledText.insert( mCurrentStyledText.begin() + position, text.begin(), text.end() );

  // Request to be relaid out
  RelayoutRequest();

  // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed on order to retrieve the right values.
  mRelayoutOperations = RELAYOUT_ALL;
}

void TextView::ReplaceTextFromTo( std::size_t position, std::size_t numberOfCharacters, const std::string& text )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( text, styledText, IsMarkupProcessingEnabled() );

  // Calls ReplaceTextFromTo() with the styled text array.
  ReplaceTextFromTo( position, numberOfCharacters, styledText );
}

void TextView::ReplaceTextFromTo( std::size_t position, std::size_t numberOfCharacters, const MarkupProcessor::StyledTextArray& text )
{
  // Creates metadata with the Insert operation.
  TextViewProcessorMetadata metadata;
  metadata.mType = TextView::TextReplaced;
  metadata.mPosition = position;
  metadata.mNumberOfCharacters = numberOfCharacters;
  metadata.mText = text;

  // Store metadata.
  mTextViewProcessorOperations.push_back( metadata );

  // Updates current styled text.
  MarkupProcessor::StyledTextArray::iterator it = mCurrentStyledText.begin() + position;
  mCurrentStyledText.erase( it, it + numberOfCharacters );
  it = mCurrentStyledText.begin() + position;
  mCurrentStyledText.insert( it, text.begin(), text.end() );

  // Request to be relaid out
  RelayoutRequest();

  // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed on order to retrieve the right values.
  mRelayoutOperations = RELAYOUT_ALL;
}

void TextView::RemoveTextFrom( std::size_t position, std::size_t numberOfCharacters )
{
  // Creates metadata with the Remove operation.
  TextViewProcessorMetadata metadata;
  metadata.mType = TextView::TextRemoved;
  metadata.mPosition = position;
  metadata.mNumberOfCharacters = numberOfCharacters;

  // Store metadata.
  mTextViewProcessorOperations.push_back( metadata );

  // Updates current styled text.
  MarkupProcessor::StyledTextArray::iterator it = mCurrentStyledText.begin() + position;
  mCurrentStyledText.erase( it, it + numberOfCharacters );

  // Request to be relaid out
  RelayoutRequest();

  // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed on order to retrieve the right values.
  mRelayoutOperations = RELAYOUT_ALL;
}

std::string TextView::GetText() const
{
  // Traverses the styled text array getting only the text.
  //  Note that for some languages a 'character' could be represented by more than one 'char'

  std::string text;
  for( MarkupProcessor::StyledTextArray::const_iterator it = mCurrentStyledText.begin(), endIt = mCurrentStyledText.end(); it != endIt; ++it )
  {
    text.append( (*it).mText.GetText() );
  }

  return text;
}

void TextView::SetLineHeightOffset( PointSize offset )
{
  if( fabsf( mLayoutParameters.mLineHeightOffset - offset ) > Math::MACHINE_EPSILON_1000 )
  {
    // Removes any previous operation which modifies the line height offset.
    std::vector<TextViewProcessorMetadata>::iterator it = std::remove_if( mTextViewProcessorOperations.begin(), mTextViewProcessorOperations.end(), IsTextViewProcessorLineHeightOffsetOperation );
    mTextViewProcessorOperations.erase( it, mTextViewProcessorOperations.end() );

    // Creates metadata with the new line height operation.
    TextViewProcessorMetadata metadata;
    metadata.mType = TextView::NewLineHeight;

    mTextViewProcessorOperations.push_back( metadata );

    // Updates line height offset.
    mLayoutParameters.mLineHeightOffset = offset;

    RelayoutRequest();

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed on order to retrieve the right values.
    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_REMOVE_TEXT_ACTORS |
                                                                RELAYOUT_SIZE_POSITION |
                                                                RELAYOUT_ALIGNMENT |
                                                                RELAYOUT_VISIBILITY |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_INSERT_TO_TEXT_VIEW |
                                                                RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }
  }
}

PointSize TextView::GetLineHeightOffset() const
{
  return PointSize( mLayoutParameters.mLineHeightOffset );
}

void TextView::SetStyleToCurrentText( const TextStyle& style, TextStyle::Mask mask )
{
  if( !mCurrentStyledText.empty() )
  {
    const bool checkFontName = mask & TextStyle::FONT;
    const bool checkFontSize = mask & TextStyle::SIZE;
    const bool checkFontStyle = mask & TextStyle::STYLE;

    // Check first if metrics have changed.
    bool metricsChanged = false;
    for( MarkupProcessor::StyledTextArray::const_iterator it = mCurrentStyledText.begin(), endIt = mCurrentStyledText.end(); ( it != endIt ) && !metricsChanged; ++it )
    {
      const MarkupProcessor::StyledText& styledText( *it );

      metricsChanged = ( checkFontName && ( styledText.mStyle.GetFontName() != style.GetFontName() ) ) ||
                       ( checkFontStyle && ( styledText.mStyle.GetFontStyle() != style.GetFontStyle() ) ) ||
                       ( checkFontSize && ( fabsf( styledText.mStyle.GetFontPointSize() - style.GetFontPointSize() ) > Math::MACHINE_EPSILON_1000 ) );
    }

    if( metricsChanged )
    {
      MarkupProcessor::SetTextStyle( mCurrentStyledText, style, mask );

      // If metrics change, new text measurements are needed.
      SetText( mCurrentStyledText );
    }
    else
    {
      // Deletes any previous operation which sets a new style.
      std::vector<TextViewProcessorMetadata>::iterator it = std::remove_if( mTextViewProcessorOperations.begin(), mTextViewProcessorOperations.end(), IsTextViewProcessorNewStyleOperation );
      mTextViewProcessorOperations.erase( it, mTextViewProcessorOperations.end() );

      // Creates metadata with the new style operation.
      TextViewProcessorMetadata metadata;
      metadata.mType = TextView::NewStyle;

      MarkupProcessor::StyledText text;
      text.mStyle = style;
      metadata.mText.push_back( text );
      metadata.mStyleMask = mask;

      mTextViewProcessorOperations.push_back( metadata );

      MarkupProcessor::SetTextStyle( mCurrentStyledText, style, mask );

      RelayoutRequest();

      if( RELAYOUT_ALL != mRelayoutOperations )
      {
        mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                  RELAYOUT_TEXT_ACTOR_UPDATE );
      }
    }
  }

  // Sets the new style to the ellipsize text
  if( !mLayoutParameters.mEllipsizeText.empty() )
  {
    for( MarkupProcessor::StyledTextArray::iterator it = mLayoutParameters.mEllipsizeText.begin(), endIt = mLayoutParameters.mEllipsizeText.end(); it != endIt; ++it )
    {
      (*it).mStyle.Copy( style, mask );
    }

    SetEllipsizeText( mLayoutParameters.mEllipsizeText );
  }
}

void TextView::SetTextAlignment( Toolkit::Alignment::Type align )
{
  if( align != ( mLayoutParameters.mHorizontalAlignment | mLayoutParameters.mVerticalAlignment ) )
  {
    Toolkit::Alignment::Type horizontalAlignment( ( align & Toolkit::Alignment::HorizontalLeft ? Toolkit::Alignment::HorizontalLeft :
                                                    ( align & Toolkit::Alignment::HorizontalCenter ? Toolkit::Alignment::HorizontalCenter :
                                                      ( align & Toolkit::Alignment::HorizontalRight ? Toolkit::Alignment::HorizontalRight : Toolkit::Alignment::HorizontalCenter ) ) ) );
    Toolkit::Alignment::Type verticalAlignment( ( align & Toolkit::Alignment::VerticalTop ? Toolkit::Alignment::VerticalTop :
                                                  ( align & Toolkit::Alignment::VerticalCenter ? Toolkit::Alignment::VerticalCenter :
                                                    ( align & Toolkit::Alignment::VerticalBottom ? Toolkit::Alignment::VerticalBottom : Toolkit::Alignment::VerticalCenter ) ) ) );

    mLayoutParameters.mHorizontalAlignment = horizontalAlignment;
    mLayoutParameters.mVerticalAlignment = verticalAlignment;

    RelayoutRequest();

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_ALIGNMENT |
                                                                RELAYOUT_VISIBILITY );
    }
  }
}

Toolkit::Alignment::Type TextView::GetTextAlignment() const
{
  return static_cast<Toolkit::Alignment::Type>( mLayoutParameters.mHorizontalAlignment | mLayoutParameters.mVerticalAlignment );
}

void TextView::SetMultilinePolicy( Toolkit::TextView::MultilinePolicy policy )
{
  if( policy != mLayoutParameters.mMultilinePolicy )
  {
    mLayoutParameters.mMultilinePolicy = policy;

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed on order to retrieve the right values.
    mRelayoutOperations = RELAYOUT_ALL;

    RelayoutRequest();
  }
}

Toolkit::TextView::MultilinePolicy TextView::GetMultilinePolicy() const
{
  return mLayoutParameters.mMultilinePolicy;
}

void TextView::SetWidthExceedPolicy( Toolkit::TextView::ExceedPolicy policy )
{
  // The layout info could be invalid depending on the current exceed policy and the new one.
  // i.e. if the current policy is Split and the new one is ShrinkToFit then
  // the layout info generated for each char is not needed.
  if( policy != mLayoutParameters.mWidthExceedPolicy )
  {
    mLayoutParameters.mWidthExceedPolicy = policy;

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
    mRelayoutOperations = RELAYOUT_ALL;

    RelayoutRequest();
  }
}

Toolkit::TextView::ExceedPolicy TextView::GetWidthExceedPolicy() const
{
  return mLayoutParameters.mWidthExceedPolicy;
}

void TextView::SetHeightExceedPolicy( Toolkit::TextView::ExceedPolicy policy )
{
  if( policy != mLayoutParameters.mHeightExceedPolicy )
  {
    mLayoutParameters.mHeightExceedPolicy = policy;

    RelayoutRequest();

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_REMOVE_TEXT_ACTORS |
                                                                RELAYOUT_SIZE_POSITION |
                                                                RELAYOUT_ALIGNMENT |
                                                                RELAYOUT_VISIBILITY |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_INSERT_TO_TEXT_VIEW |
                                                                RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }
  }
}

Toolkit::TextView::ExceedPolicy TextView::GetHeightExceedPolicy() const
{
  return mLayoutParameters.mHeightExceedPolicy;
}

void TextView::SetLineJustification( Toolkit::TextView::LineJustification justification )
{
  if( justification != mLayoutParameters.mLineJustification )
  {
    mLayoutParameters.mLineJustification = justification;

    RelayoutRequest();

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_REMOVE_TEXT_ACTORS |
                                                                RELAYOUT_SIZE_POSITION |
                                                                RELAYOUT_ALIGNMENT |
                                                                RELAYOUT_VISIBILITY |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_INSERT_TO_TEXT_VIEW |
                                                                RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }
  }
}

Toolkit::TextView::LineJustification TextView::GetLineJustification() const
{
  return mLayoutParameters.mLineJustification;
}

void TextView::SetFadeBoundary( const Toolkit::TextView::FadeBoundary& fadeBoundary )
{
  if( ( fadeBoundary.mLeft != mVisualParameters.mFadeBoundary.mLeft )   ||
      ( fadeBoundary.mRight != mVisualParameters.mFadeBoundary.mRight ) ||
      ( fadeBoundary.mTop != mVisualParameters.mFadeBoundary.mTop )     ||
      ( fadeBoundary.mBottom != mVisualParameters.mFadeBoundary.mBottom ) )
  {
    mVisualParameters.mFadeBoundary = fadeBoundary;

    RelayoutRequest();

    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_REMOVE_TEXT_ACTORS |
                                                                RELAYOUT_VISIBILITY |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_INSERT_TO_TEXT_VIEW |
                                                                RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }
  }
}

const Toolkit::TextView::FadeBoundary& TextView::GetFadeBoundary() const
{
  return mVisualParameters.mFadeBoundary;
}

void TextView::SetEllipsizeText( const std::string& ellipsizeText )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( ellipsizeText, styledText, IsMarkupProcessingEnabled() );

  SetEllipsizeText( styledText );
}

void TextView::SetEllipsizeText( const MarkupProcessor::StyledTextArray& ellipsizeText )
{
  mLayoutParameters.mEllipsizeText = ellipsizeText;

  mRelayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo = TextViewProcessor::WordLayoutInfo();

  TextViewProcessor::CreateWordTextInfo( mLayoutParameters.mEllipsizeText,
                                         mRelayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo );

  // Request to be relaid out
  RelayoutRequest();

  mRelayoutOperations = RELAYOUT_ALL;
}

std::string TextView::GetEllipsizeText() const
{
  std::string text;
  for( MarkupProcessor::StyledTextArray::const_iterator it = mLayoutParameters.mEllipsizeText.begin(), endIt = mLayoutParameters.mEllipsizeText.end(); it != endIt; ++it )
  {
    text.append( (*it).mText.GetText() );
  }

  return text;
}

void TextView::GetTextLayoutInfo()
{
  const bool relayoutSizeAndPositionNeeded = mRelayoutOperations & RELAYOUT_SIZE_POSITION;
  const bool relayoutAlignmentNeeded = mRelayoutOperations & RELAYOUT_ALIGNMENT;
  const bool relayoutVisibilityNeeded = mRelayoutOperations & RELAYOUT_VISIBILITY;

  if( relayoutSizeAndPositionNeeded || relayoutAlignmentNeeded || relayoutVisibilityNeeded )
  {
    Vector3 textViewSize = GetControlSize();

    if( ( ( textViewSize.width < Math::MACHINE_EPSILON_1000 ) ||
          ( textViewSize.height < Math::MACHINE_EPSILON_1000 ) ) &&
        ( ( Toolkit::TextView::SplitByNewLineChar == mLayoutParameters.mMultilinePolicy ) &&
          ( Toolkit::TextView::Original == mLayoutParameters.mWidthExceedPolicy ) &&
          ( Toolkit::TextView::Original == mLayoutParameters.mHeightExceedPolicy ) ) )
    {
      // In case the control size is not set but the layout settings are the default (split by new line character and original exceed policies)
      // the text natural size can be used.
      textViewSize = GetNaturalSize();
    }

    if( ( textViewSize.width > Math::MACHINE_EPSILON_1000 ) &&
        ( textViewSize.height > Math::MACHINE_EPSILON_1000 ) )
    {
      // Check if the text-view has glyph-actors.
      const bool hasGlyphActors = !mRelayoutData.mGlyphActors.empty();

      RelayoutOperationMask mask = NO_RELAYOUT;
      if( relayoutSizeAndPositionNeeded )
      {
        mask = static_cast<RelayoutOperationMask>( mask | RELAYOUT_SIZE_POSITION );
      }
      if( relayoutAlignmentNeeded )
      {
        mask = static_cast<RelayoutOperationMask>( mask | RELAYOUT_ALIGNMENT );
      }
      if( relayoutVisibilityNeeded )
      {
        mask = static_cast<RelayoutOperationMask>( mask | RELAYOUT_VISIBILITY );
      }

      if( hasGlyphActors )
      {
        // Remove glyph-actors from the text-view as some text-operation like CreateTextInfo()
        // add them to the text-actor cache.
        TextViewRelayout::RemoveGlyphActors( GetRootActor(), mRelayoutData.mGlyphActors );
        mRelayoutData.mGlyphActors.clear();
      }

      // Relays-out but doesn't add glyph-actors to the text-view.
      DoRelayOut( textViewSize.GetVectorXY(), mask );

      if( hasGlyphActors )
      {
        mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_INSERT_TO_TEXT_VIEW );
        mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
      }

    }
  }
}

void TextView::GetTextLayoutInfo( Toolkit::TextView::TextLayoutInfo& textLayoutInfo )
{
  GetTextLayoutInfo();

  textLayoutInfo.mCharacterLayoutInfoTable = mRelayoutData.mCharacterLayoutInfoTable;
  textLayoutInfo.mLines = mRelayoutData.mLines;

  textLayoutInfo.mCharacterLogicalToVisualMap = mRelayoutData.mCharacterLogicalToVisualMap;
  textLayoutInfo.mCharacterVisualToLogicalMap = mRelayoutData.mCharacterVisualToLogicalMap;

  textLayoutInfo.mTextSize = mRelayoutData.mTextSizeForRelayoutOption;

  textLayoutInfo.mScrollOffset = mVisualParameters.mCameraScrollPosition;
}

void TextView::SetSortModifier( float depthOffset )
{
  mVisualParameters.mSortModifier = depthOffset;

  for( std::vector<RenderableActor>::iterator it = mRelayoutData.mGlyphActors.begin(), endIt = mRelayoutData.mGlyphActors.end();
       it != endIt;
       ++it )
  {
    ( *it ).SetSortModifier( depthOffset );
  }

  if( mOffscreenImageActor )
  {
    mOffscreenImageActor.SetSortModifier( depthOffset );
  }
}

void TextView::SetSnapshotModeEnabled( bool enable )
{
  if( enable != mVisualParameters.mSnapshotModeEnabled )
  {
    // Remove first all glyph-actors
    if( !mRelayoutData.mGlyphActors.empty() )
    {
      TextViewRelayout::RemoveGlyphActors( GetRootActor(), mRelayoutData.mGlyphActors );
    }

    mVisualParameters.mSnapshotModeEnabled = enable;
    if( !mLockPreviousSnapshotMode )
    {
      // mPreviousSnapshotModeEnabled stores the snapshot mode value before SetScrollEnabled( true ) is
      // called. However, if SetSnapshotModeEnabled() is called after SetScrollEnabled() then the stored value
      // is updated.
      // As SetSnapshotModeEnabled() is also called from SetScrollEnabled(), the mLockPreviousSnapshotMode prevents
      // to smash the stored value.
      mPreviousSnapshotModeEnabled = enable;
    }

    if( mVisualParameters.mSnapshotModeEnabled )
    {
      // Create a root actor and an image actor for offscreen rendering.
      mOffscreenRootActor = Layer::New();
      mOffscreenImageActor = ImageActor::New();

      mOffscreenRootActor.SetColorMode( USE_OWN_COLOR );
      mOffscreenRootActor.SetPositionInheritanceMode( DONT_INHERIT_POSITION );
      mOffscreenRootActor.SetInheritRotation( false );
      mOffscreenRootActor.SetInheritScale( false );
      mOffscreenRootActor.SetDepthTestDisabled( true );

      mOffscreenRootActor.SetPosition( 0.f, 0.f, 0.f );

      mOffscreenImageActor.SetAnchorPoint( ParentOrigin::CENTER );
      mOffscreenImageActor.SetParentOrigin( ParentOrigin::CENTER );
      mOffscreenImageActor.SetBlendFunc( BlendingFactor::ONE, BlendingFactor::ONE_MINUS_SRC_ALPHA,
                                         BlendingFactor::ONE, BlendingFactor::ONE );

      Actor self = Self();
      self.Add( mOffscreenRootActor );
      self.Add( mOffscreenImageActor );
      mOffscreenImageActor.SetScale( Vector3( 1.f, -1.f, 1.f ) );
    }
    else
    {
      Actor self = Self();

      if( mOffscreenRootActor )
      {
        self.Remove( mOffscreenRootActor );
      }

      if( mOffscreenImageActor )
      {
        self.Remove( mOffscreenImageActor );
      }

      DestroyOffscreenRenderingResources();
    }

    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_REMOVE_TEXT_ACTORS |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_INSERT_TO_TEXT_VIEW |
                                                                RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }
    RelayoutRequest();
  }
}

bool TextView::IsSnapshotModeEnabled() const
{
  return mVisualParameters.mSnapshotModeEnabled;
}

void TextView::SetMarkupProcessingEnabled( bool enable )
{
  mMarkUpEnabled = enable;
}

bool TextView::IsMarkupProcessingEnabled() const
{
  return mMarkUpEnabled;
}

void TextView::SetScrollEnabled( bool enable )
{
  if( enable != mVisualParameters.mScrollEnabled )
  {
    mVisualParameters.mScrollEnabled = enable;

    if( mVisualParameters.mScrollEnabled )
    {
      // Offscreen rendering is needed to enable text scroll.

      // Stores previous value of the snapshot mode.
      mPreviousSnapshotModeEnabled = IsSnapshotModeEnabled();

      {
        // SetSnapshotModeEnabled() modifies the mPreviousSnapshotModeEnabled just in case it's called after SetScrollEnabled(),
        // this lock prevents to modify the mPreviousSnapshotModeEnabled when SetSnapshotModeEnabled() from this method.
        Lock lock( mLockPreviousSnapshotMode );
        SetSnapshotModeEnabled( true );
      }

      // Creates the pan gesture detector and attach the text-view.
      mPanGestureDetector = PanGestureDetector::New();
      mPanGestureDetector.DetectedSignal().Connect( this, &TextView::OnTextPan );
      mPanGestureDetector.Attach( Self() );
    }
    else
    {
      // Removes the pan gesture detector.
      if( mPanGestureDetector )
      {
        mPanGestureDetector.Detach( Self() );
        mPanGestureDetector.DetectedSignal().Disconnect( this, &TextView::OnTextPan );
        mPanGestureDetector.Reset();
      }

      // Restores the previous state for snapshot mode.
      SetSnapshotModeEnabled( mPreviousSnapshotModeEnabled );
    }
  }
}

bool TextView::IsScrollEnabled() const
{
  return mVisualParameters.mScrollEnabled;
}

void TextView::SetScrollPosition( const Vector2& position )
{
  if( position != mVisualParameters.mCameraScrollPosition )
  {
    // Guard against destruction during signal emission
    // Note that Emit() methods are called indirectly from within DoSetScrollPosition()
    Toolkit::TextView handle( GetOwner() );

    DoSetScrollPosition( position );

    // Check if the new scroll position has been trimmed.
    mVisualParameters.mScrollPositionTrimmed = ( position != mVisualParameters.mCameraScrollPosition );
  }
}

const Vector2& TextView::GetScrollPosition() const
{
  return mVisualParameters.mCameraScrollPosition;
}

bool TextView::IsScrollPositionTrimmed() const
{
  return mVisualParameters.mScrollPositionTrimmed;
}

Toolkit::TextView::ScrolledSignalV2& TextView::ScrolledSignal()
{
  return mScrolledSignalV2;
}

bool TextView::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::TextView textView = Toolkit::TextView::DownCast(handle);

  if( Dali::Toolkit::TextView::SIGNAL_TEXT_SCROLLED == signalName )
  {
    textView.ScrolledSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

TextView::LayoutParameters::LayoutParameters()
: mMultilinePolicy( Toolkit::TextView::SplitByNewLineChar ),
  mWidthExceedPolicy( Toolkit::TextView::Original ),
  mHeightExceedPolicy( Toolkit::TextView::Original ),
  mHorizontalAlignment( Toolkit::Alignment::HorizontalCenter ),
  mVerticalAlignment( Toolkit::Alignment::VerticalCenter ),
  mLineJustification( Toolkit::TextView::Left ),
  mLineHeightOffset( 0.f ),
  mEllipsizeText(),
  mMarkUpEnabled( false )
{
  // Sets ellipsize text
  MarkupProcessor::StyledTextArray styledEllipsize;
  MarkupProcessor::GetStyledTextArray( std::string( "..." ), mEllipsizeText, false );
}

TextView::LayoutParameters::LayoutParameters( Toolkit::TextView::MultilinePolicy   multilinePolicy,
                                              Toolkit::TextView::ExceedPolicy      widthExceedPolicy,
                                              Toolkit::TextView::ExceedPolicy      heightExceedPolicy,
                                              Toolkit::Alignment::Type             alignmentType,
                                              Toolkit::TextView::LineJustification lineJustification,
                                              float                                lineHeightOffset,
                                              const std::string&                   ellipsizeText,
                                              bool                                 markUpEnabled )
: mMultilinePolicy( multilinePolicy ),
  mWidthExceedPolicy( widthExceedPolicy ),
  mHeightExceedPolicy( heightExceedPolicy ),
  mHorizontalAlignment(),
  mVerticalAlignment(),
  mLineJustification( lineJustification ),
  mLineHeightOffset( lineHeightOffset ),
  mEllipsizeText(),
  mMarkUpEnabled( markUpEnabled )
{
  // Sets alignment
  Toolkit::Alignment::Type horizontalAlignment( ( alignmentType & Toolkit::Alignment::HorizontalLeft ? Toolkit::Alignment::HorizontalLeft :
                                                  ( alignmentType & Toolkit::Alignment::HorizontalCenter ? Toolkit::Alignment::HorizontalCenter :
                                                    ( alignmentType & Toolkit::Alignment::HorizontalRight ? Toolkit::Alignment::HorizontalRight : Toolkit::Alignment::HorizontalCenter ) ) ) );
  Toolkit::Alignment::Type verticalAlignment( ( alignmentType & Toolkit::Alignment::VerticalTop ? Toolkit::Alignment::VerticalTop :
                                                ( alignmentType & Toolkit::Alignment::VerticalCenter ? Toolkit::Alignment::VerticalCenter :
                                                  ( alignmentType & Toolkit::Alignment::VerticalBottom ? Toolkit::Alignment::VerticalBottom : Toolkit::Alignment::VerticalCenter ) ) ) );

  mHorizontalAlignment = horizontalAlignment;
  mVerticalAlignment = verticalAlignment;

  // Sets ellipsize text
  MarkupProcessor::StyledTextArray styledEllipsize;
  MarkupProcessor::GetStyledTextArray( ellipsizeText, mEllipsizeText, mMarkUpEnabled );
}

TextView::LayoutParameters::LayoutParameters( const TextView::LayoutParameters& layoutParameters )
: mMultilinePolicy( layoutParameters.mMultilinePolicy ),
  mWidthExceedPolicy( layoutParameters.mWidthExceedPolicy ),
  mHeightExceedPolicy( layoutParameters.mHeightExceedPolicy ),
  mHorizontalAlignment( layoutParameters.mHorizontalAlignment ),
  mVerticalAlignment( layoutParameters.mVerticalAlignment ),
  mLineJustification( layoutParameters.mLineJustification ),
  mLineHeightOffset( layoutParameters.mLineHeightOffset ),
  mEllipsizeText( layoutParameters.mEllipsizeText ),
  mMarkUpEnabled( layoutParameters.mMarkUpEnabled )
{
}

TextView::LayoutParameters& TextView::LayoutParameters::operator=( const TextView::LayoutParameters& layoutParameters )
{
  mMultilinePolicy = layoutParameters.mMultilinePolicy;
  mWidthExceedPolicy = layoutParameters.mWidthExceedPolicy;
  mHeightExceedPolicy = layoutParameters.mHeightExceedPolicy;
  mHorizontalAlignment = layoutParameters.mHorizontalAlignment;
  mVerticalAlignment = layoutParameters.mVerticalAlignment;
  mLineJustification = layoutParameters.mLineJustification;
  mLineHeightOffset = layoutParameters.mLineHeightOffset;
  mEllipsizeText = layoutParameters.mEllipsizeText;
  mMarkUpEnabled = layoutParameters.mMarkUpEnabled;

  return *this;
}

TextView::VisualParameters::VisualParameters()
: mFadeBoundary(),
  mSortModifier( 0.f ),
  mCameraScrollPosition( 0.f, 0.f ),
  mSnapshotModeEnabled( false ),
  mScrollEnabled( false ),
  mScrollPositionTrimmed( false )
{
}

TextView::VisualParameters::VisualParameters( const VisualParameters& visualParameters )
: mFadeBoundary( visualParameters.mFadeBoundary ),
  mSortModifier( visualParameters.mSortModifier ),
  mCameraScrollPosition( visualParameters.mCameraScrollPosition ),
  mSnapshotModeEnabled( visualParameters.mSnapshotModeEnabled ),
  mScrollEnabled( visualParameters.mScrollEnabled ),
  mScrollPositionTrimmed( visualParameters.mScrollPositionTrimmed )
{
}

TextView::VisualParameters& TextView::VisualParameters::operator=( const TextView::VisualParameters& visualParameters )
{
  mFadeBoundary = visualParameters.mFadeBoundary;
  mSortModifier = visualParameters.mSortModifier;
  mCameraScrollPosition = visualParameters.mCameraScrollPosition;
  mSnapshotModeEnabled = visualParameters.mSnapshotModeEnabled;
  mScrollEnabled = visualParameters.mScrollEnabled;
  mScrollPositionTrimmed = visualParameters.mScrollPositionTrimmed;

  return *this;
}

TextView::RelayoutData::RelayoutData()
: mTextViewSize(),
  mShrinkFactor( 1.f ),
  mTextLayoutInfo(),
  mCharacterLogicalToVisualMap(),
  mCharacterVisualToLogicalMap(),
  mGlyphActors(),
  mCharacterLayoutInfoTable(),
  mLines(),
  mTextSizeForRelayoutOption()
{
}

TextView::RelayoutData::RelayoutData( const TextView::RelayoutData& relayoutData )
: mTextViewSize( relayoutData.mTextViewSize ),
  mShrinkFactor( relayoutData.mShrinkFactor ),
  mTextLayoutInfo( relayoutData.mTextLayoutInfo ),
  mCharacterLogicalToVisualMap( relayoutData.mCharacterLogicalToVisualMap ),
  mCharacterVisualToLogicalMap( relayoutData.mCharacterVisualToLogicalMap ),
  mGlyphActors( relayoutData.mGlyphActors ),
  mCharacterLayoutInfoTable( relayoutData.mCharacterLayoutInfoTable ),
  mLines( relayoutData.mLines ),
  mTextSizeForRelayoutOption( relayoutData.mTextSizeForRelayoutOption )
{
}

TextView::RelayoutData& TextView::RelayoutData::operator=( const TextView::RelayoutData& relayoutData )
{
  mTextViewSize = relayoutData.mTextViewSize;
  mShrinkFactor = relayoutData.mShrinkFactor;
  mTextLayoutInfo = relayoutData.mTextLayoutInfo;
  mCharacterLogicalToVisualMap = relayoutData.mCharacterLogicalToVisualMap;
  mCharacterVisualToLogicalMap = relayoutData.mCharacterVisualToLogicalMap;
  mGlyphActors = relayoutData.mGlyphActors;
  mCharacterLayoutInfoTable = relayoutData.mCharacterLayoutInfoTable;
  mLines = relayoutData.mLines;
  mTextSizeForRelayoutOption = relayoutData.mTextSizeForRelayoutOption;

  return *this;
}

TextView::TextView()
: Control( REQUIRES_STYLE_CHANGE_SIGNALS  ),
  mCurrentStyledText(),
  mTextViewProcessorOperations(),
  mLayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                     Toolkit::TextView::Original,
                     Toolkit::TextView::Original,
                     static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                     Toolkit::TextView::Left,
                     PointSize( 0.f ),
                     std::string( "..." ),
                     false ),
  mVisualParameters(),
  mRelayoutData(),
  mRelayoutOperations( NO_RELAYOUT ),
  mOffscreenRootActor(),
  mOffscreenImageActor(),
  mOffscreenCameraActor(),
  mCurrentOffscreenSize(),
  mFrameBufferImage(),
  mRenderTask(),
  mPanGestureDetector(),
  mLockPreviousSnapshotMode( false ),
  mPreviousSnapshotModeEnabled( false ),
  mMarkUpEnabled( false )
{
  TextViewProcessor::CreateWordTextInfo( mLayoutParameters.mEllipsizeText,
                                         mRelayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo );
}

TextView::~TextView()
{
  // Destroys offscreen rendering resources.
  DestroyOffscreenRenderingResources();

  // Destroys scroll pan gesture detector.
  if( mPanGestureDetector )
  {
    mPanGestureDetector.Reset();
  }
}

Vector3 TextView::GetNaturalSize()
{
  if( !mTextViewProcessorOperations.empty() )
  {
    // There are SetText, Inserts or Removes to do. It means the current layout info is not updated.

    if( !mRelayoutData.mGlyphActors.empty() )
    {
      // Remove glyph-actors from the text-view as some text-operation like CreateTextInfo()
      // add them to the text-actor cache.
      TextViewRelayout::RemoveGlyphActors( GetRootActor(), mRelayoutData.mGlyphActors );
      mRelayoutData.mGlyphActors.clear();

      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_INSERT_TO_TEXT_VIEW );
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }

    PerformTextViewProcessorOperations();
  }

  return Vector3( mRelayoutData.mTextLayoutInfo.mWholeTextSize.width, mRelayoutData.mTextLayoutInfo.mWholeTextSize.height, 0.f );
}

float TextView::GetHeightForWidth( float width )
{
  float height = 0.f;

  if( ( Toolkit::TextView::SplitByNewLineChar == mLayoutParameters.mMultilinePolicy ) &&
      ( Toolkit::TextView::Original == mLayoutParameters.mWidthExceedPolicy ) &&
      ( Toolkit::TextView::Original == mLayoutParameters.mHeightExceedPolicy ) )
  {
    // If multiline and exceed policies are 'SplitByNewLineChar' and 'Original' is better get the height from the
    // natural size. GetNaturalSize() for this configuration is faster than DoRelayOut().
    height = GetNaturalSize().height;
  }
  else
  {
    // Check if the given width is different than the current one.
    const bool differentWidth = ( fabsf( width - mRelayoutData.mTextViewSize.width ) > Math::MACHINE_EPSILON_1000 );

    // Check if the text-view has glyph-actors.
    const bool hasGlyphActors = !mRelayoutData.mGlyphActors.empty();

    // Check which layout operations need to be done.
    const bool relayoutSizeAndPositionNeeded = ( mRelayoutOperations & RELAYOUT_SIZE_POSITION ) || differentWidth;

    if( relayoutSizeAndPositionNeeded )
    {
      if( hasGlyphActors )
      {
        // Remove glyph-actors from the text-view as some text-operation like CreateTextInfo()
        // add them to the text-actor cache.
        TextViewRelayout::RemoveGlyphActors( GetRootActor(), mRelayoutData.mGlyphActors );
        mRelayoutData.mGlyphActors.clear();
      }

      // Use the given width.
      const Vector2 textViewSize( width, GetControlSize().height );

      // Relays-out but doesn't add glyph-actors to the text-view.
      DoRelayOut( textViewSize, RELAYOUT_SIZE_POSITION );
    }

    // Retrieve the text height after relayout the text.
    height = mRelayoutData.mTextSizeForRelayoutOption.height;

    if( differentWidth )
    {
      // Revert the relayout operation mask
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_SIZE_POSITION );
    }

    if( hasGlyphActors )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_INSERT_TO_TEXT_VIEW );
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations | RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }

    if( differentWidth || hasGlyphActors )
    {
      RelayoutRequest();
    }
  }

  return height;
}

float TextView::GetWidthForHeight( float height )
{
  // TODO: Needs implementing properly, for now just return the natural width.
  return GetNaturalSize().width;
}


void TextView::OnInitialize()
{
}


void TextView::OnFontChange( bool defaultFontChange, bool defaultFontSizeChange )
{
  mRelayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo = TextViewProcessor::WordLayoutInfo();
  TextViewProcessor::CreateWordTextInfo( mLayoutParameters.mEllipsizeText,
                                         mRelayoutData.mTextLayoutInfo.mEllipsizeLayoutInfo );

  SetText( mCurrentStyledText );
}

void TextView::OnControlSizeSet( const Vector3& size )
{
  if( size.GetVectorXY() != mRelayoutData.mTextViewSize )
  {
    // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
    mRelayoutOperations = RELAYOUT_ALL;

    // Request to be relaid out
    RelayoutRequest();
  }
}

void TextView::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  if( ( size.width < Math::MACHINE_EPSILON_1000 ) || ( size.height < Math::MACHINE_EPSILON_1000 ) )
  {
    // Not worth to relayout if width or height is equal to zero.
    return;
  }

  if( size != mRelayoutData.mTextViewSize )
  {
    // if new size is different than the prevoius one, set positions and maybe sizes of all glyph-actor is needed.
    if( RELAYOUT_ALL != mRelayoutOperations )
    {
      mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                                RELAYOUT_REMOVE_TEXT_ACTORS |
                                                                RELAYOUT_SIZE_POSITION |
                                                                RELAYOUT_ALIGNMENT |
                                                                RELAYOUT_VISIBILITY |
                                                                RELAYOUT_TEXT_ACTOR_UPDATE |
                                                                RELAYOUT_INSERT_TO_TEXT_VIEW |
                                                                RELAYOUT_INSERT_TO_TEXT_ACTOR_LIST );
    }
  }

  // Remove glyph-actors from text-view
  if( !mRelayoutData.mGlyphActors.empty() && ( mRelayoutOperations & RELAYOUT_REMOVE_TEXT_ACTORS ) )
  {
    TextViewRelayout::RemoveGlyphActors( GetRootActor(), mRelayoutData.mGlyphActors );
    mRelayoutData.mGlyphActors.clear();
  }

  if( NO_RELAYOUT != mRelayoutOperations )
  {
    // Relays-out and add glyph-actors to the text-view.
    DoRelayOut( size, mRelayoutOperations );
    ProcessSnapshot( size );
  }

  // Quite likely the texts of the text-actors are not going to be reused, so clear them.
  mRelayoutData.mTextActorCache.ClearTexts();
}

void TextView::PerformTextViewProcessorOperations()
{
  // Traverse the relayout operation vector ...

  // Optimizes some operations.
  OptimizeTextViewProcessorOperations();

  for( std::vector<TextViewProcessorMetadata>::const_iterator it = mTextViewProcessorOperations.begin(), endIt = mTextViewProcessorOperations.end(); it != endIt; ++it )
  {
    const TextViewProcessorMetadata& relayoutMetadata( *it );

    switch( relayoutMetadata.mType )
    {
      case TextView::TextSet:
      {
        TextViewProcessor::CreateTextInfo( relayoutMetadata.mText,
                                           mLayoutParameters,
                                           mRelayoutData );
        break;
      }
      case TextView::TextInserted:
      {
        TextViewProcessor::UpdateTextInfo( relayoutMetadata.mPosition,
                                           relayoutMetadata.mText,
                                           mLayoutParameters,
                                           mRelayoutData );
        break;
      }
      case TextView::TextReplaced:
      {
        TextViewProcessor::UpdateTextInfo( relayoutMetadata.mPosition,
                                           relayoutMetadata.mNumberOfCharacters,
                                           relayoutMetadata.mText,
                                           mLayoutParameters,
                                           mRelayoutData );
        break;
      }
      case TextView::TextRemoved:
      {
        TextViewProcessor::UpdateTextInfo( relayoutMetadata.mPosition,
                                           relayoutMetadata.mNumberOfCharacters,
                                           mLayoutParameters,
                                           mRelayoutData,
                                           TextViewProcessor::CLEAR_TEXT ); // clears the text of the text-actors.
        break;
      }
      case TextView::NewLineHeight:
      {
        TextViewProcessor::UpdateTextInfo( mLayoutParameters.mLineHeightOffset,
                                           mRelayoutData.mTextLayoutInfo );
        break;
      }
      case TextView::NewStyle:
      {
        TextViewProcessor::UpdateTextInfo( ( *relayoutMetadata.mText.begin() ).mStyle,
                                           relayoutMetadata.mStyleMask,
                                           mRelayoutData );
        break;
      }
    }
  }

  // Clear all operations when they are done.
  mTextViewProcessorOperations.clear();
}

void TextView::OptimizeTextViewProcessorOperations()
{
  // TODO: check if some operation can be discarted. i.e. InsertTextAt( 0, "a" ); RemoveTextFrom( 0, 1 );

  // At the moment it only replaces a 'remove 1 character' followed by 'insert 1 character' in the same position by a 'replace' operation.
  // This sequence is used by text-input with predictive text. Change this two operations by a replace allows the text-view processor to
  // use the cache without clearing the text-actors.

  std::vector<TextViewProcessorMetadata> textViewProcessorOperations;

  for( std::vector<TextViewProcessorMetadata>::const_iterator it = mTextViewProcessorOperations.begin(), endIt = mTextViewProcessorOperations.end(); it != endIt; ++it )
  {
    const TextViewProcessorMetadata& relayoutMetadata( *it );

    switch( relayoutMetadata.mType )
    {
      case TextView::TextRemoved:
      {
        bool optimizationDone = false;

        if( it + 1 != endIt )
        {
          const TextViewProcessorMetadata& nextRelayoutMetadata( *( it + 1 ) );
          if( TextView::TextInserted == nextRelayoutMetadata.mType )
          {
            if( relayoutMetadata.mPosition == nextRelayoutMetadata.mPosition )
            {
              optimizationDone = true;
              TextViewProcessorMetadata newRelayoutMetadata;
              newRelayoutMetadata.mType = TextView::TextReplaced;
              newRelayoutMetadata.mPosition = relayoutMetadata.mPosition;
              newRelayoutMetadata.mNumberOfCharacters = relayoutMetadata.mNumberOfCharacters;
              newRelayoutMetadata.mText = nextRelayoutMetadata.mText;
              textViewProcessorOperations.push_back( newRelayoutMetadata );

              // do not access the TextInserted operation in next iteration.
              ++it;
            }
          }
        }

        if( !optimizationDone )
        {
          textViewProcessorOperations.push_back( relayoutMetadata );
        }
        break;
      }
      default:
      {
        textViewProcessorOperations.push_back( relayoutMetadata );
      }
    }
  }

  mTextViewProcessorOperations = textViewProcessorOperations;
}

void TextView::DoRelayOut( const Size& textViewSize, RelayoutOperationMask relayoutOperationMask )
{
  // Traverse the relayout operation vector. It fills the natural size, layout and glyph-actor data structures.
  if( !mTextViewProcessorOperations.empty() )
  {
    PerformTextViewProcessorOperations();
  }

  CombineExceedPolicies();

  Actor rootActor;
  if( mVisualParameters.mSnapshotModeEnabled )
  {
    rootActor = mOffscreenRootActor;
  }
  else
  {
    rootActor = Self();
  }

  mRelayoutData.mTextViewSize = textViewSize;
  switch( mLayoutParameters.mMultilinePolicy )
  {
    case Toolkit::TextView::SplitByNewLineChar:              // multiline policy == SplitByNewLineChar.
    {
      SplitByNewLineChar::Relayout( rootActor, relayoutOperationMask, mLayoutParameters, mVisualParameters, mRelayoutData );
      break;
    } // SplitByNewLineChar

    case Toolkit::TextView::SplitByWord:                     // multiline policy == SplitByWord.
    {
      SplitByWord::Relayout( rootActor, relayoutOperationMask, mLayoutParameters, mVisualParameters, mRelayoutData );
      break;
    } // SplitByWord

    case Toolkit::TextView::SplitByChar:                     // multiline policy == SplitByChar.
    {
      SplitByChar::Relayout( rootActor, relayoutOperationMask, mLayoutParameters, mVisualParameters, mRelayoutData );
      break;
    } // SplitByChar
  } // switch( mMultilinePolicy )

  // Remove done operations from the mask.
  mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations & ~relayoutOperationMask );
}

void TextView::ProcessSnapshot( const Size& textViewSize )
{
  if( mVisualParameters.mSnapshotModeEnabled )
  {
    // If layout options change, it's needed generate a new image.

    if( mOffscreenRootActor )
    {
      // Set the root actor visible.
      // The root actor is set to non visible after the render task is processed.
      mOffscreenRootActor.SetVisible( true );

      // The offscreen root actor must have same size as text view. Otherwise, text alignment won't work.
      mOffscreenRootActor.SetSize( textViewSize );
    }

    if( ( mRelayoutData.mTextSizeForRelayoutOption.width > Math::MACHINE_EPSILON_1000 ) &&
        ( mRelayoutData.mTextSizeForRelayoutOption.height > Math::MACHINE_EPSILON_1000 ) )
    {
      // Set the image actor visible.
      // The image actor is set to non visible if there is no text to render.
      mOffscreenImageActor.SetVisible( true );

      // Calculates the offscreen image's size. It takes into account different points:
      // * If text has italics, add a small offset is needed in order to not to cut the text next to the right edge.
      // * There is a maximum texture size the graphic subsystem can load on the memory.
      // * If the scroll is enabled, the offscreen image's size is never bigger than the text-view's size.

      const Size offscreenSize( std::min( MAX_OFFSCREEN_RENDERING_SIZE,
                                          mVisualParameters.mScrollEnabled ? textViewSize.width : std::max( mRelayoutData.mTextSizeForRelayoutOption.width, textViewSize.width ) + mRelayoutData.mTextLayoutInfo.mMaxItalicsOffset ),
                                std::min( MAX_OFFSCREEN_RENDERING_SIZE,
                                          mVisualParameters.mScrollEnabled ? textViewSize.height : std::max( mRelayoutData.mTextSizeForRelayoutOption.height, textViewSize.height ) ) );

      const bool sizeChanged = offscreenSize != mCurrentOffscreenSize;

      if( sizeChanged )
      {
        // Creates a frame buffer for offscreen rendering when the size is negotiated.
        mFrameBufferImage = FrameBufferImage::New( offscreenSize.width,
                                                   offscreenSize.height,
                                                   Pixel::RGBA8888 );

        // Stores current text-view size to avoid create new Dali resources if text changes.
        mCurrentOffscreenSize = offscreenSize;

        if( !mOffscreenCameraActor )
        {
          // Creates a new camera actor.
          mOffscreenCameraActor = CameraActor::New();
          mOffscreenCameraActor.SetParentOrigin( ParentOrigin::CENTER );
          mOffscreenCameraActor.SetAnchorPoint( AnchorPoint::CENTER );
          mOffscreenCameraActor.SetRotation(Degree(180.f), Vector3::YAXIS);

          mOffscreenCameraActor.SetType( Dali::Camera::FREE_LOOK ); // Inherits position from the offscreen root actor.

          mOffscreenRootActor.Add( mOffscreenCameraActor ); // camera to shoot the offscreen text
        }

        // Calculate camera parameters for current text size.
        mOffscreenCameraActor.SetOrthographicProjection( offscreenSize );
      }

      if( mVisualParameters.mScrollEnabled )
      {
        // Updates the offscreen camera position with the new scroll offset.
        mOffscreenCameraActor.SetX( mVisualParameters.mCameraScrollPosition.x );
        mOffscreenCameraActor.SetY( mVisualParameters.mCameraScrollPosition.y );
      }
      else
      {
        // Text's size could be bigger than text-view's size. In that case the camera must be aligned to cover the whole text.
        AlignOffscreenCameraActor( textViewSize, offscreenSize );
      }

      if( !mRenderTask )
      {
        // Creates a new render task.
        mRenderTask = Stage::GetCurrent().GetRenderTaskList().CreateTask();

        mRenderTask.SetSourceActor( mOffscreenRootActor );
        mRenderTask.SetInputEnabled( false );
        mRenderTask.SetClearColor( Color::TRANSPARENT );
        mRenderTask.SetClearEnabled( true );
        mRenderTask.SetExclusive( true );

        // Connects the signal to the TextView::RenderTaskFinished method in order to make the root actor non visible when the render task is processed.
        mRenderTask.FinishedSignal().Connect( this, &TextView::RenderTaskFinished );
      }

      if( sizeChanged )
      {
        mRenderTask.SetCameraActor( mOffscreenCameraActor );
        mRenderTask.SetTargetFrameBuffer( mFrameBufferImage );
      }

      // Process the render task only once every time the text changes or the text-view's size canges.
      mRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
    }
    else
    {
      // If there is no text just make any previous generated image invisible instead to process a render task with no text.
      mOffscreenImageActor.SetVisible( false );
    }
  }
}

void TextView::AlignOffscreenCameraActor( const Size& textViewSize, const Size& offscreenSize )
{
  float xPosition = 0.f;
  float yPosition = 0.f;
  Vector3 parentOrigin = ParentOrigin::CENTER;
  Vector3 anchorPoint = AnchorPoint::CENTER;

  switch( mLayoutParameters.mHorizontalAlignment )
  {
    case Toolkit::Alignment::HorizontalLeft:
    {
      xPosition = 0.5f * ( offscreenSize.width - textViewSize.width );
      parentOrigin.x = 0.f;
      anchorPoint.x = 0.f;
      break;
    }
    case Toolkit::Alignment::HorizontalCenter:
    {
      // nothing to do.
      break;
    }
    case Toolkit::Alignment::HorizontalRight:
    {
      xPosition = 0.5f * ( textViewSize.width - offscreenSize.width );
      parentOrigin.x = 1.f;
      anchorPoint.x = 1.f;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextView::AlignOffscreenCameraActor: Invalid alignment option." )
    }
  }

  switch( mLayoutParameters.mVerticalAlignment )
  {
    case Toolkit::Alignment::VerticalTop:
    {
      yPosition = 0.5f * ( offscreenSize.height - textViewSize.height );
      parentOrigin.y = 0.f;
      anchorPoint.y = 0.f;
      break;
    }
    case Toolkit::Alignment::VerticalCenter:
    {
      // nothing to do.
      break;
    }
    case Toolkit::Alignment::VerticalBottom:
    {
      yPosition = 0.5f * ( textViewSize.height - offscreenSize.height );
      parentOrigin.y = 1.f;
      anchorPoint.y = 1.f;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextView::AlignOffscreenCameraActor: Invalid alignment option." )
    }
  }

  mOffscreenCameraActor.SetX( xPosition );
  mOffscreenCameraActor.SetY( yPosition );

  mOffscreenImageActor.SetParentOrigin( parentOrigin );
  mOffscreenImageActor.SetAnchorPoint( anchorPoint );
}

void TextView::RenderTaskFinished( Dali::RenderTask& renderTask )
{
  // not to process the offscreen root actor by setting its visibility to false.
  mOffscreenRootActor.SetVisible( false );

  // Sets the new size and the new frame buffer to the image actor.
  // Image actor must have same size as text. Otherwise text can be truncated.
  mOffscreenImageActor.SetSize( mCurrentOffscreenSize );
  mOffscreenImageActor.SetImage( mFrameBufferImage );
}

void TextView::DestroyOffscreenRenderingResources()
{
  if( mRenderTask )
  {
    mRenderTask.FinishedSignal().Disconnect( this, &TextView::RenderTaskFinished );

    if( Stage::IsInstalled() )
    {
      Stage::GetCurrent().GetRenderTaskList().RemoveTask( mRenderTask );
    }

    mRenderTask.Reset();
  }

  // Remove and reset the root actor, image actor and camera actor as text-view is not rendering offscreen.
  if( mOffscreenCameraActor )
  {
    mOffscreenRootActor.Remove( mOffscreenCameraActor );

    mOffscreenCameraActor.Reset();
  }

  if( mOffscreenRootActor )
  {
    mOffscreenRootActor.Reset();
  }

  if( mOffscreenImageActor )
  {
    mOffscreenImageActor.Reset();
  }

  mCurrentOffscreenSize = Size( 0.f, 0.f );

  if( mFrameBufferImage )
  {
    mFrameBufferImage.Reset();
  }
}

void TextView::OnTextPan( Actor actor, PanGesture gesture )
{
  if( 1u == gesture.numberOfTouches )
  {
    DoSetScrollPosition( mVisualParameters.mCameraScrollPosition - gesture.displacement );
  }
}

void TextView::TrimScrollPosition()
{
  const Vector3& textViewSize = GetControlSize();

  // Before use the text's size, relayout the text is needed to get the actual text size.
  GetTextLayoutInfo();

  // Calculates the range within the text could be scrolled. (When the text is aligned in the center).
  float maxHorizontalDisplacement = std::max( 0.f, 0.5f * ( mRelayoutData.mTextSizeForRelayoutOption.width - textViewSize.width ) );
  float maxVerticalDisplacement = std::max( 0.f, 0.5f * ( mRelayoutData.mTextSizeForRelayoutOption.height - textViewSize.height ) );
  float minHorizontalDisplacement = -maxHorizontalDisplacement;
  float minVerticalDisplacement = -maxVerticalDisplacement;

  // Updates the range if the text is aligned on the right or left.
  switch( mLayoutParameters.mHorizontalAlignment )
  {
    case Toolkit::Alignment::HorizontalLeft:
    {
      maxHorizontalDisplacement *= 2.f;
      minHorizontalDisplacement = 0.f;
      break;
    }
    case Toolkit::Alignment::HorizontalCenter:
    {
      // nothing to do.
      break;
    }
    case Toolkit::Alignment::HorizontalRight:
    {
      maxHorizontalDisplacement = 0.f;
      minHorizontalDisplacement *= 2.f;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextView::TrimScrollPosition: Invalid alignment option." )
    }
  }

  // Updates the range if the text is aligned on the top or bottom.
  switch( mLayoutParameters.mVerticalAlignment )
  {
    case Toolkit::Alignment::VerticalTop:
    {
      maxVerticalDisplacement *= 2.f;
      minVerticalDisplacement = 0.f;
      break;
    }
    case Toolkit::Alignment::VerticalCenter:
    {
      //nothing to do
      break;
    }
    case Toolkit::Alignment::VerticalBottom:
    {
      maxVerticalDisplacement = 0.f;
      minVerticalDisplacement *= 2.f;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextView::TrimScrollPosition: Invalid alignment option." )
    }
  }

  // Trims the scroll position to be within the range.
  mVisualParameters.mCameraScrollPosition.x = std::min( maxHorizontalDisplacement, mVisualParameters.mCameraScrollPosition.x );
  mVisualParameters.mCameraScrollPosition.x = std::max( minHorizontalDisplacement, mVisualParameters.mCameraScrollPosition.x );

  mVisualParameters.mCameraScrollPosition.y = std::min( maxVerticalDisplacement, mVisualParameters.mCameraScrollPosition.y );
  mVisualParameters.mCameraScrollPosition.y = std::max( minVerticalDisplacement, mVisualParameters.mCameraScrollPosition.y );
}

void TextView::DoSetScrollPosition( const Vector2& position )
{
  // Stores old scroll position.
  Vector2 delta( mVisualParameters.mCameraScrollPosition );

  // Updates the scroll position
  mVisualParameters.mCameraScrollPosition = position;

  // Ensures the text-view is covered with text.
  TrimScrollPosition();

  // Calculate the difference with the previous scroll position
  delta.x = mVisualParameters.mCameraScrollPosition.x - delta.x;
  delta.y = mVisualParameters.mCameraScrollPosition.y - delta.y;

  if( mOffscreenRootActor )
  {
    // If there is a render-task it needs to be refreshed. Therefore glyph-actors need to be
    // set to visible.
    mOffscreenRootActor.SetVisible( true );
  }

  if( mOffscreenCameraActor )
  {
    // Update the offscreen camera with the new scroll position.
    mOffscreenCameraActor.SetX( mVisualParameters.mCameraScrollPosition.x );
    mOffscreenCameraActor.SetY( mVisualParameters.mCameraScrollPosition.y );
  }

  if( mRenderTask )
  {
    // Refresh the render-task.
    mRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
  }

  // Emit the signal.
  Toolkit::TextView handle( GetOwner() );
  mScrolledSignalV2.Emit( handle, delta );
}

void TextView::CombineExceedPolicies()
{
  // Calculates the combination of exceed policies.

  switch( mLayoutParameters.mWidthExceedPolicy )
  {
    case Toolkit::TextView::Original:
    {
      switch( mLayoutParameters.mHeightExceedPolicy )
      {
        case Toolkit::TextView::Original:
        {
          mLayoutParameters.mExceedPolicy = Original;
          break;
        }
        case Toolkit::TextView::Fade:
        {
          mLayoutParameters.mExceedPolicy = OriginalFade;
          break;
        }
        case Toolkit::TextView::ShrinkToFit:
        {
          mLayoutParameters.mExceedPolicy = OriginalShrink;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( !"TextView::CombineExceedPolicies() Invalid width and height exceed policies combination" );
        }
      }
      break;
    }
    case Toolkit::TextView::Split:
    {
      switch( mLayoutParameters.mHeightExceedPolicy )
      {
        case Toolkit::TextView::Original:
        {
          mLayoutParameters.mExceedPolicy = SplitOriginal;
          break;
        }
        case Toolkit::TextView::Fade:
        {
          mLayoutParameters.mExceedPolicy = SplitFade;
          break;
        }
        case Toolkit::TextView::ShrinkToFit:
        {
          mLayoutParameters.mExceedPolicy = SplitShrink;
          break;
        }
        case Toolkit::TextView::EllipsizeEnd:
        {
          mLayoutParameters.mExceedPolicy = SplitEllipsizeEnd;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( !"TextView::CombineExceedPolicies() Invalid width and height exceed policies combination" );
        }
      }
      break;
    }
    case Toolkit::TextView::Fade:
    {
      switch( mLayoutParameters.mHeightExceedPolicy )
      {
        case Toolkit::TextView::Original:
        {
          mLayoutParameters.mExceedPolicy = FadeOriginal;
          break;
        }
        case Toolkit::TextView::Fade:
        {
          mLayoutParameters.mExceedPolicy = Fade;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( !"TextView::CombineExceedPolicies() Invalid width and height exceed policies combination" );
        }
      }
      break;
    }
    case Toolkit::TextView::ShrinkToFit:
    {
      switch( mLayoutParameters.mHeightExceedPolicy )
      {
        case Toolkit::TextView::Original:
        {
          mLayoutParameters.mExceedPolicy = ShrinkOriginal;
          break;
        }
        case Toolkit::TextView::Fade:
        {
          mLayoutParameters.mExceedPolicy = ShrinkFade;
          break;
        }
        case Toolkit::TextView::ShrinkToFit:
        {
          mLayoutParameters.mExceedPolicy = Shrink;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( !"TextView::CombineExceedPolicies() Invalid width and height exceed policies combination" );
        }
      }
      break;
    }
    case Toolkit::TextView::EllipsizeEnd:
    {
      switch( mLayoutParameters.mHeightExceedPolicy )
      {
        case Toolkit::TextView::Original:
        {
          mLayoutParameters.mExceedPolicy = EllipsizeEndOriginal;
          break;
        }
        case Toolkit::TextView::EllipsizeEnd:
        {
          mLayoutParameters.mExceedPolicy = EllipsizeEnd;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( !"TextView::CombineExceedPolicies() Invalid width and height exceed policies combination" );
        }
      }
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( !"TextView::CombineExceedPolicies() Invalid width exceed policy" );
    }
  }
}

Actor TextView::GetRootActor() const
{
  // Get the root actor, if text-view was rendering offscreen, or the text-view itself.

  Actor rootActor;

  if( mVisualParameters.mSnapshotModeEnabled )
  {
    rootActor = mOffscreenRootActor;
  }
  else
  {
    rootActor = Self();
  }

  return rootActor;
}

void TextView::OnMarkupEnabledPeopertySet( Property::Value propertyValue )
{
  bool newValue( propertyValue.Get<bool>() );
  if( newValue != IsMarkupProcessingEnabled() )
  {
    SetMarkupProcessingEnabled( newValue );
    if( newValue )
    {
      // If markup processing has been enabled, Ensure current text is reprocessed.
      const std::string& currentText( GetText() );
      if( ! currentText.empty() )
      {
        SetText( currentText );
      }
    }
  }
}

void TextView::OnMultilinePolicyPropertySet( Property::Value propertyValue )
{
  std::string policyName( propertyValue.Get<std::string>() );
  if(policyName == "SplitByNewLineChar")
  {
    SetMultilinePolicy(Toolkit::TextView::SplitByNewLineChar);
  }
  else if(policyName == "SplitByWord")
  {
    SetMultilinePolicy(Toolkit::TextView::SplitByWord);
  }
  else if(policyName == "SplitByChar")
  {
    SetMultilinePolicy(Toolkit::TextView::SplitByChar);
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"TextView::OnMultilinePolicyPropertySet(). Invalid Property value." );
  }
}

void TextView::OnWidthExceedPolicyPropertySet( Property::Value propertyValue )
{
  std::string policyName( propertyValue.Get<std::string>() );
  if(policyName == "Original")
  {
    SetWidthExceedPolicy(Toolkit::TextView::Original);
  }
  else if(policyName == "Fade")
  {
    SetWidthExceedPolicy(Toolkit::TextView::Fade);
  }
  else if(policyName == "Split")
  {
    SetWidthExceedPolicy(Toolkit::TextView::Split);
  }
  else if(policyName == "ShrinkToFit")
  {
    SetWidthExceedPolicy(Toolkit::TextView::ShrinkToFit);
  }
  else if(policyName == "EllipsizeEnd")
  {
    SetWidthExceedPolicy(Toolkit::TextView::EllipsizeEnd);
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"TextView::OnWidthExceedPolicyPropertySet(). Invalid Property value." );
  }
}

void TextView::OnHeightExceedPolicyPropertySet( Property::Value propertyValue )
{
  std::string policyName( propertyValue.Get<std::string>() );
  if(policyName == "Original")
  {
    SetHeightExceedPolicy(Toolkit::TextView::Original);
  }
  else if(policyName == "Fade")
  {
    SetHeightExceedPolicy(Toolkit::TextView::Fade);
  }
  else if(policyName == "Split")
  {
    SetHeightExceedPolicy(Toolkit::TextView::Split);
  }
  else if(policyName == "ShrinkToFit")
  {
    SetHeightExceedPolicy(Toolkit::TextView::ShrinkToFit);
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"TextView::OnHeightExceedPolicyPropertySet(). Invalid Property value." );
  }
}

void TextView::OnLineJustificationPropertySet( Property::Value propertyValue )
{
  std::string policyName( propertyValue.Get<std::string>() );
  if(policyName == "Left")
  {
    SetLineJustification(Toolkit::TextView::Left);
  }
  else if(policyName == "Center")
  {
    SetLineJustification(Toolkit::TextView::Center);
  }
  else if(policyName == "Right")
  {
    SetLineJustification(Toolkit::TextView::Right);
  }
  else if(policyName == "Justified")
  {
    SetLineJustification(Toolkit::TextView::Justified);
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"TextView::OnLineJustificationPropertySet(). Invalid Property value." );
  }
}

void TextView::OnFadeBoundaryPropertySet( Property::Value propertyValue )
{
  Vector4 value( propertyValue.Get<Vector4>() );
  DALI_ASSERT_ALWAYS( value.x >= 0 && value.y >= 0 && value.z >= 0 && value.w >= 0
                      && "TextView::OnFadeBoundaryPropertySet(). Negative value is invalid. "  );

  Toolkit::TextView::FadeBoundary fadeBoundary( PixelSize( static_cast<unsigned int>( value.x ) ),
                                                PixelSize( static_cast<unsigned int>( value.y ) ),
                                                PixelSize( static_cast<unsigned int>( value.z ) ),
                                                PixelSize( static_cast<unsigned int>( value.w ) ) );

  SetFadeBoundary( fadeBoundary );
}

void TextView::OnAlignmentPropertySet( Property::Index propertyIndex, Property::Value propertyValue )
{
  std::string value( propertyValue.Get<std::string>() );

  if( propertyIndex == Toolkit::TextView::PROPERTY_HORIZONTAL_ALIGNMENT )
  {
    if(value == "HorizontalLeft")
    {
      mLayoutParameters.mHorizontalAlignment = Toolkit::Alignment::HorizontalLeft;
    }
    else if( value == "HorizontalCenter")
    {
      mLayoutParameters.mHorizontalAlignment = Toolkit::Alignment::HorizontalCenter;
    }
    else if( value == "HorizontalRight")
    {
      mLayoutParameters.mHorizontalAlignment = Toolkit::Alignment::HorizontalRight;
    }
    else
    {
      DALI_ASSERT_ALWAYS( !"TextView::OnAlignmentPropertySet(). Invalid Property value." );
    }
  }
  else if( propertyIndex == Toolkit::TextView::PROPERTY_VERTICAL_ALIGNMENT )
  {
    if( value == "VerticalTop" )
    {
      mLayoutParameters.mVerticalAlignment = Toolkit::Alignment::VerticalTop;
    }
    else if( value == "VerticalCenter")
    {
      mLayoutParameters.mVerticalAlignment = Toolkit::Alignment::VerticalCenter;
    }
    else if( value == "VerticalBottom")
    {
      mLayoutParameters.mVerticalAlignment = Toolkit::Alignment::VerticalBottom;
    }
    else
    {
      DALI_ASSERT_ALWAYS( !"TextView::OnAlignmentPropertySet(). Invalid Property value." );
    }
  }

  RelayoutRequest();

  // If a GetTextLayoutInfo() or GetHeightForWidth() arrives, relayout the text synchronously is needed in order to retrieve the right values.
  if( RELAYOUT_ALL != mRelayoutOperations )
  {
    mRelayoutOperations = static_cast<RelayoutOperationMask>( mRelayoutOperations |
                                                              RELAYOUT_TEXT_ACTOR_UPDATE |
                                                              RELAYOUT_ALIGNMENT |
                                                              RELAYOUT_VISIBILITY );
  }
}

std::string TextView::OnHorizontalAlignmentPropertyGet()
{
  if( mLayoutParameters.mHorizontalAlignment == Toolkit::Alignment::HorizontalLeft )
  {
    return "HorizontalLeft";
  }
  else if( mLayoutParameters.mHorizontalAlignment == Toolkit::Alignment::HorizontalCenter )
  {
    return "HorizontalCenter";
  }
  else if( mLayoutParameters.mHorizontalAlignment == Toolkit::Alignment::HorizontalRight )
  {
    return "HorizontalRight";
  }
  else
  {
    DALI_ASSERT_ALWAYS( !"TextView::OnHorizontalAlignmentPropertyGet(). Invalid value." );
  }
}

std::string TextView::OnVerticalAlignmentPropertyGet()
{
  if( mLayoutParameters.mVerticalAlignment == Toolkit::Alignment::VerticalTop )
   {
     return "VerticalTop";
   }
   else if( mLayoutParameters.mVerticalAlignment == Toolkit::Alignment::VerticalCenter )
   {
     return "VerticalCenter";
   }
   else if( mLayoutParameters.mVerticalAlignment == Toolkit::Alignment::VerticalBottom )
   {
     return "VerticalBottom";
   }
   else
   {
     DALI_ASSERT_ALWAYS( !"TextView::OnVerticalAlignmentPropertyGet(). Invalid value." );
   }
}

void TextView::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextView textView = Toolkit::TextView::DownCast( Dali::BaseHandle( object ) );

  if( textView )
  {
    TextView& textViewImpl( GetImpl( textView ) );
    switch( index )
    {
      case Toolkit::TextView::PROPERTY_MARKUP_ENABLED:
      {
        textViewImpl.OnMarkupEnabledPeopertySet( value );
        break;
      }
      case Toolkit::TextView::PROPERTY_TEXT:
      {
        textViewImpl.SetText( value.Get<std::string>() );
        break;
      }
      case Toolkit::TextView::PROPERTY_MULTILINE_POLICY:
      {
        textViewImpl.OnMultilinePolicyPropertySet( value );
        break;
      }
      case Toolkit::TextView::PROPERTY_WIDTH_EXCEED_POLICY:
      {
        textViewImpl.OnWidthExceedPolicyPropertySet( value );
        break;
      }
      case Toolkit::TextView::PROPERTY_HEIGHT_EXCEED_POLICY:
      {
        textViewImpl.OnHeightExceedPolicyPropertySet( value );
        break;
      }
      case Toolkit::TextView::PROPERTY_LINE_JUSTIFICATION:
      {
        textViewImpl.OnLineJustificationPropertySet( value );
        break;
      }
      case Toolkit::TextView::PROPERTY_FADE_BOUNDARY:
      {
        textViewImpl.OnFadeBoundaryPropertySet( value );
        break;
      }
      case Toolkit::TextView::PROPERTY_LINE_HEIGHT_OFFSET:
      {
        Dali::PointSize pointSize( value.Get<float>() );
        textViewImpl.SetLineHeightOffset(pointSize);
        break;
      }
      case Toolkit::TextView::PROPERTY_HORIZONTAL_ALIGNMENT:
      case Toolkit::TextView::PROPERTY_VERTICAL_ALIGNMENT:
      {
        textViewImpl.OnAlignmentPropertySet( index, value );
        break;
      }
    }
  }
}

Property::Value TextView::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextView textView = Toolkit::TextView::DownCast( Dali::BaseHandle( object ) );

  if( textView )
  {
    TextView& textViewImpl( GetImpl( textView ) );
    switch( index )
    {
      case Toolkit::TextView::PROPERTY_MARKUP_ENABLED:
      {
        value = textViewImpl.IsMarkupProcessingEnabled();
        break;
      }
      case Toolkit::TextView::PROPERTY_TEXT:
      {
        value = textViewImpl.GetText();
        break;
      }
      case Toolkit::TextView::PROPERTY_MULTILINE_POLICY:
      {
        value = MULTILINE_POLICY_NAME[ textViewImpl.GetMultilinePolicy() ];
        break;
      }
      case Toolkit::TextView::PROPERTY_WIDTH_EXCEED_POLICY:
      {
        value = EXCEED_POLICY_NAME[ textViewImpl.GetWidthExceedPolicy() ];
        break;
      }
      case Toolkit::TextView::PROPERTY_HEIGHT_EXCEED_POLICY:
      {
        value = EXCEED_POLICY_NAME[ textViewImpl.GetHeightExceedPolicy() ];
        break;
      }
      case Toolkit::TextView::PROPERTY_LINE_JUSTIFICATION:
      {
        value = LINE_JUSTIFICATION_NAME[ textViewImpl.GetLineJustification() ];
        break;
      }
      case Toolkit::TextView::PROPERTY_FADE_BOUNDARY:
      {
        Toolkit::TextView::FadeBoundary boundary = textViewImpl.GetFadeBoundary();
        value = Vector4( static_cast<float>( boundary.mLeft.value ),
                         static_cast<float>( boundary.mRight.value ),
                         static_cast<float>( boundary.mTop.value ),
                         static_cast<float>( boundary.mBottom.value ) );
        break;
      }
      case Toolkit::TextView::PROPERTY_LINE_HEIGHT_OFFSET:
      {
        value = textViewImpl.GetLineHeightOffset().value;
        break;
      }
      case Toolkit::TextView::PROPERTY_HORIZONTAL_ALIGNMENT:
      {
        value = textViewImpl.OnHorizontalAlignmentPropertyGet();
        break;
      }
      case Toolkit::TextView::PROPERTY_VERTICAL_ALIGNMENT:
      {
        value = textViewImpl.OnVerticalAlignmentPropertyGet();
        break;
      }
    }
  }
  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
