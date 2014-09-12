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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-view-processor-types.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

/////////////////////
// Layout info.
/////////////////////

CharacterLayoutInfo::CharacterLayoutInfo()
: mHeight( 0.f ),
  mAdvance( 0.f ),
  mBearing( 0.f ),
  mPosition(),
  mOffset(),
  mSize(),
  mAscender( 0.f ),
  mUnderlineThickness( 0.f ),
  mUnderlinePosition( 0.f ),
  mGlyphActor(),
  mStyledText(),
  mColorAlpha( 1.f ),
  mGradientColor(),
  mStartPoint(),
  mEndPoint(),
  mIsVisible( true ),
  mSetText( true ),
  mSetStyle( true ),
  mIsColorGlyph( false )
{
}

CharacterLayoutInfo::~CharacterLayoutInfo()
{
}

CharacterLayoutInfo::CharacterLayoutInfo( const CharacterLayoutInfo& character )
: mHeight( character.mHeight ),
  mAdvance( character.mAdvance ),
  mBearing( character.mBearing ),
  mPosition( character.mPosition ),
  mOffset( character.mOffset ),
  mSize( character.mSize ),
  mAscender( character.mAscender ),
  mUnderlineThickness( character.mUnderlineThickness ),
  mUnderlinePosition( character.mUnderlinePosition ),
  mGlyphActor( character.mGlyphActor ),
  mStyledText( character.mStyledText ),
  mColorAlpha( character.mColorAlpha ),
  mGradientColor( character.mGradientColor ),
  mStartPoint( character.mStartPoint ),
  mEndPoint( character.mEndPoint ),
  mIsVisible( character.mIsVisible ),
  mSetText( character.mSetText ),
  mSetStyle( character.mSetStyle ),
  mIsColorGlyph( character.mIsColorGlyph )
{
}

CharacterLayoutInfo& CharacterLayoutInfo::operator=( const CharacterLayoutInfo& character )
{
  mHeight = character.mHeight;
  mAdvance = character.mAdvance;
  mBearing = character.mBearing;

  mPosition = character.mPosition;
  mOffset = character.mOffset;
  mSize = character.mSize;
  mAscender = character.mAscender;
  mUnderlineThickness = character.mUnderlineThickness;
  mUnderlinePosition = character.mUnderlinePosition;

  mStyledText = character.mStyledText;
  mColorAlpha = character.mColorAlpha;
  mGradientColor = character.mGradientColor;
  mStartPoint = character.mStartPoint;
  mEndPoint = character.mEndPoint;
  mIsVisible = character.mIsVisible;
  mSetText = character.mSetText;
  mSetStyle = character.mSetStyle;
  mIsColorGlyph = character.mIsColorGlyph;

  mGlyphActor = character.mGlyphActor;

  return *this;
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
