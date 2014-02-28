//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER

#include <dali-toolkit/public-api/controls/text-view/text-view.h>

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/text-view/text-view-impl.h>

namespace Dali
{

namespace Toolkit
{


const char* const TextView::SIGNAL_TEXT_SCROLLED = "scrolled";

TextView::CharacterLayoutInfo::CharacterLayoutInfo()
: mSize(),
  mPosition(),
  mIsNewLineChar( false ),
  mIsRightToLeftCharacter( false ),
  mIsVisible( true ),
  mDescender()
{
}

TextView::CharacterLayoutInfo::~CharacterLayoutInfo()
{
}

TextView::CharacterLayoutInfo::CharacterLayoutInfo( const TextView::CharacterLayoutInfo& characterLayoutInfo )
: mSize( characterLayoutInfo.mSize ),
  mPosition( characterLayoutInfo.mPosition ),
  mIsNewLineChar( characterLayoutInfo.mIsNewLineChar ),
  mIsRightToLeftCharacter( characterLayoutInfo.mIsRightToLeftCharacter ),
  mIsVisible( characterLayoutInfo.mIsVisible ),
  mDescender( characterLayoutInfo.mDescender )
{
}

TextView::CharacterLayoutInfo& TextView::CharacterLayoutInfo::operator=( const TextView::CharacterLayoutInfo& characterLayoutInfo )
{
  mSize = characterLayoutInfo.mSize;
  mPosition = characterLayoutInfo.mPosition;
  mIsNewLineChar = characterLayoutInfo.mIsNewLineChar;
  mIsRightToLeftCharacter = characterLayoutInfo.mIsRightToLeftCharacter;
  mIsVisible = characterLayoutInfo.mIsVisible;
  mDescender = characterLayoutInfo.mDescender;

  return *this;
}

TextView::CharacterLayoutInfo::CharacterLayoutInfo( const Size& size,
                                                    const Vector3& position,
                                                    const bool isNewLineChar,
                                                    const bool isRightToLeftCharacter,
                                                    const bool isVisible,
                                                    const float descender )
: mSize( size ),
  mPosition( position ),
  mIsNewLineChar( isNewLineChar ),
  mIsRightToLeftCharacter( isRightToLeftCharacter ),
  mIsVisible( isVisible ),
  mDescender( descender )
{
}

TextView::TextLayoutInfo::TextLayoutInfo()
: mCharacterLayoutInfoTable(),
  mLines(),
  mCharacterLogicalToVisualMap(),
  mCharacterVisualToLogicalMap(),
  mTextSize(),
  mScrollOffset()
{
}

TextView::TextLayoutInfo::~TextLayoutInfo()
{
}

TextView::TextLayoutInfo::TextLayoutInfo( const TextView::TextLayoutInfo& textLayoutInfo )
: mCharacterLayoutInfoTable( textLayoutInfo.mCharacterLayoutInfoTable ),
  mLines( textLayoutInfo.mLines ),
  mCharacterLogicalToVisualMap( textLayoutInfo.mCharacterLogicalToVisualMap ),
  mCharacterVisualToLogicalMap( textLayoutInfo.mCharacterVisualToLogicalMap ),
  mTextSize( textLayoutInfo.mTextSize ),
  mScrollOffset( textLayoutInfo.mScrollOffset )
{
}

TextView::TextLayoutInfo& TextView::TextLayoutInfo::operator=( const TextView::TextLayoutInfo& textLayoutInfo )
{
  mCharacterLayoutInfoTable = textLayoutInfo.mCharacterLayoutInfoTable;
  mLines = textLayoutInfo.mLines;
  mCharacterLogicalToVisualMap = textLayoutInfo.mCharacterLogicalToVisualMap;
  mCharacterVisualToLogicalMap = textLayoutInfo.mCharacterVisualToLogicalMap;
  mTextSize = textLayoutInfo.mTextSize;
  mScrollOffset = textLayoutInfo.mScrollOffset;

  return *this;
}

TextView::FadeBoundary::FadeBoundary()
: mLeft( 0 ),
  mRight( 0 ),
  mTop( 0 ),
  mBottom( 0 )
{
}

TextView::FadeBoundary::FadeBoundary( const PixelSize left, const PixelSize right, const PixelSize top, const PixelSize bottom )
: mLeft( left ),
  mRight( right ),
  mTop( top ),
  mBottom( bottom )
{
}

TextView::TextView()
{
}

TextView::TextView( const TextView& handle )
: Control( handle )
{
}

TextView::TextView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::TextView>(internal);
}

TextView& TextView::operator=( const TextView& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

TextView TextView::New()
{
  return Internal::TextView::New();
}

TextView TextView::New( const std::string& text )
{
  TextView textView = Internal::TextView::New();
  textView.SetText( text );
  return textView;
}

TextView TextView::New( const MarkupProcessor::StyledTextArray& text )
{
  TextView textView = Internal::TextView::New();
  textView.SetText( text );
  return textView;
}

TextView TextView::DownCast( BaseHandle handle )
{
  return Control::DownCast<TextView, Internal::TextView>(handle);
}

TextView::~TextView()
{
}

void TextView::SetText( const std::string& text )
{
  GetImpl( *this ).SetText( text );
}

void TextView::SetText( const MarkupProcessor::StyledTextArray& text )
{
  GetImpl( *this ).SetText( text );
}

void TextView::InsertTextAt( const std::size_t position, const std::string& text )
{
  GetImpl( *this ).InsertTextAt( position, text );
}

void TextView::InsertTextAt( const std::size_t position, const MarkupProcessor::StyledTextArray& text )
{
  GetImpl( *this ).InsertTextAt( position, text );
}

void TextView::ReplaceTextFromTo( const std::size_t position, const std::size_t numberOfCharacters, const std::string& text )
{
  GetImpl( *this ).ReplaceTextFromTo( position, numberOfCharacters, text );
}

void TextView::ReplaceTextFromTo( const std::size_t position, const std::size_t numberOfCharacters, const MarkupProcessor::StyledTextArray& text )
{
  GetImpl( *this ).ReplaceTextFromTo( position, numberOfCharacters, text );
}

void TextView::RemoveTextFrom( const std::size_t position, const std::size_t numberOfCharacters )
{
  GetImpl( *this ).RemoveTextFrom( position, numberOfCharacters );
}

std::string TextView::GetText() const
{
  return GetImpl( *this ).GetText();
}

void TextView::SetLineHeightOffset( const PointSize offset )
{
  GetImpl( *this ).SetLineHeightOffset( offset );
}

PointSize TextView::GetLineHeightOffset() const
{
  return GetImpl( *this ).GetLineHeightOffset();
}

void TextView::SetStyleToCurrentText( const TextStyle& style, const TextStyle::Mask mask )
{
  GetImpl( *this ).SetStyleToCurrentText( style, mask );
}

void TextView::SetTextAlignment( Alignment::Type align )
{
  GetImpl( *this ).SetTextAlignment( align );
}

Alignment::Type TextView::GetTextAlignment() const
{
  return GetImpl( *this ).GetTextAlignment();
}

void TextView::SetMultilinePolicy( TextView::MultilinePolicy policy )
{
  GetImpl( *this ).SetMultilinePolicy( policy );
}

TextView::MultilinePolicy TextView::GetMultilinePolicy() const
{
  return GetImpl( *this ).GetMultilinePolicy();
}

void TextView::SetWidthExceedPolicy( TextView::ExceedPolicy policy )
{
  GetImpl( *this ).SetWidthExceedPolicy( policy );
}

TextView::ExceedPolicy TextView::GetWidthExceedPolicy() const
{
  return GetImpl( *this ).GetWidthExceedPolicy();
}

void TextView::SetHeightExceedPolicy( ExceedPolicy policy )
{
  GetImpl( *this ).SetHeightExceedPolicy( policy );
}

TextView::ExceedPolicy TextView::GetHeightExceedPolicy() const
{
  return GetImpl( *this ).GetHeightExceedPolicy();
}

void TextView::SetLineJustification( TextView::LineJustification justification )
{
  GetImpl( *this ).SetLineJustification( justification );
}

TextView::LineJustification TextView::GetLineJustification() const
{
  return GetImpl( *this ).GetLineJustification();
}

void TextView::SetFadeBoundary( const TextView::FadeBoundary& fadeBoundary )
{
  GetImpl( *this ).SetFadeBoundary( fadeBoundary );
}

const TextView::FadeBoundary& TextView::GetFadeBoundary() const
{
  return GetImpl( *this ).GetFadeBoundary();
}

void TextView::SetEllipsizeText( const std::string& ellipsizeText )
{
  GetImpl( *this ).SetEllipsizeText( ellipsizeText );
}

void TextView::SetEllipsizeText( const MarkupProcessor::StyledTextArray& ellipsizeText )
{
  GetImpl( *this ).SetEllipsizeText( ellipsizeText );
}

std::string TextView::GetEllipsizeText() const
{
  return GetImpl( *this ).GetEllipsizeText();
}

void TextView::GetTextLayoutInfo( TextLayoutInfo& textLayoutInfo )
{
  GetImpl( *this ).GetTextLayoutInfo( textLayoutInfo );
}

void TextView::SetSortModifier( float depthOffset )
{
  GetImpl( *this ).SetSortModifier( depthOffset );
}

void TextView::SetSnapshotModeEnabled( bool enable )
{
  GetImpl( *this ).SetSnapshotModeEnabled( enable );
}

bool TextView::IsSnapshotModeEnabled() const
{
  return GetImpl( *this ).IsSnapshotModeEnabled();
}

void TextView::SetScrollEnabled( bool enable )
{
  GetImpl( *this ).SetScrollEnabled( enable );
}

bool TextView::IsScrollEnabled() const
{
  return GetImpl( *this ).IsScrollEnabled();
}

void TextView::SetScrollPosition( const Vector2& position )
{
  GetImpl( *this ).SetScrollPosition( position );
}

const Vector2& TextView::GetScrollPosition() const
{
  return GetImpl( *this ).GetScrollPosition();
}

bool TextView::IsScrollPositionTrimmed() const
{
  return GetImpl( *this ).IsScrollPositionTrimmed();
}

TextView::ScrolledSignalV2& TextView::ScrolledSignal()
{
  return GetImpl( *this ).ScrolledSignal();
}

TextView::TextView( Internal::TextView& implementation )
: Control( implementation )
{
}

} // namespace Toolkit

} // namespace Dali
