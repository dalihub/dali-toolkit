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

GradientInfo::GradientInfo()
: mGradientColor(),
  mStartPoint(),
  mEndPoint()
{
}

GradientInfo::~GradientInfo()
{
}

GradientInfo::GradientInfo( const GradientInfo& info )
: mGradientColor( info.mGradientColor ),
  mStartPoint( info.mStartPoint ),
  mEndPoint( info.mEndPoint )
{
}

GradientInfo& GradientInfo::operator=( const GradientInfo& info )
{
  if( this != &info )
  {
    mGradientColor = info.mGradientColor;
    mStartPoint = info.mStartPoint;
    mEndPoint = info.mEndPoint;
  }

  return *this;
}

CharacterLayoutInfo::CharacterLayoutInfo()
: mSize(),
  mBearing( 0.f ),
  mAscender( 0.f ),
  mUnderlineThickness( 0.f ),
  mUnderlinePosition( 0.f ),
  mPosition(),
  mOffset(),
  mGlyphActor(),
  mColorAlpha( 1.f ),
  mGradientInfo( NULL ),
  mIsVisible( true ),
  mSetText( false ),
  mSetStyle( false ),
  mIsColorGlyph( false ),
  mIsRightToLeft( false )
{
}

CharacterLayoutInfo::~CharacterLayoutInfo()
{
  // Deletes the gradient info.
  delete mGradientInfo;
}

CharacterLayoutInfo::CharacterLayoutInfo( const CharacterLayoutInfo& character )
: mSize( character.mSize ),
  mBearing( character.mBearing ),
  mAscender( character.mAscender ),
  mUnderlineThickness( character.mUnderlineThickness ),
  mUnderlinePosition( character.mUnderlinePosition ),
  mPosition( character.mPosition ),
  mOffset( character.mOffset ),
  mGlyphActor( character.mGlyphActor ),
  mColorAlpha( character.mColorAlpha ),
  mGradientInfo( ( NULL == character.mGradientInfo ) ? NULL : new GradientInfo( *character.mGradientInfo ) ), // Copies the gradient info.
  mIsVisible( character.mIsVisible ),
  mSetText( character.mSetText ),
  mSetStyle( character.mSetStyle ),
  mIsColorGlyph( character.mIsColorGlyph ),
  mIsRightToLeft( character.mIsRightToLeft )
{
}

CharacterLayoutInfo& CharacterLayoutInfo::operator=( const CharacterLayoutInfo& character )
{
  mSize = character.mSize;
  mAscender = character.mAscender;
  mBearing = character.mBearing;
  mUnderlineThickness = character.mUnderlineThickness;
  mUnderlinePosition = character.mUnderlinePosition;

  mPosition = character.mPosition;
  mOffset = character.mOffset;

  mGlyphActor = character.mGlyphActor;

  mColorAlpha = character.mColorAlpha;

  // Copies the gradient info.
  if( NULL == character.mGradientInfo )
  {
    // The source doesn't have. Deletes the current one.
    delete mGradientInfo;
    mGradientInfo = NULL;
  }
  else
  {
    // The source has gradient info.
    if( NULL != mGradientInfo )
    {
      // It it has, copy to it.
      *mGradientInfo = *character.mGradientInfo;
    }
    else
    {
      // If it doesn't have, create a new one.
      mGradientInfo = new GradientInfo( *character.mGradientInfo );
    }
  }

  mIsVisible = character.mIsVisible;
  mSetText = character.mSetText;
  mSetStyle = character.mSetStyle;
  mIsColorGlyph = character.mIsColorGlyph;
  mIsRightToLeft = character.mIsRightToLeft;

  return *this;
}

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali
