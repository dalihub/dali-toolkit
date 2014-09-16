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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>


// Internal headers are allowed here
#include <dali-toolkit/internal/controls/text-view/text-view-processor-types.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Internal;

void dali_text_view_processor_types_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_text_view_processor_types_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{
// Data structures used to create an 'experiment' in TET cases

//////////////////////////////////////////////////////////////////
} // namespace


int UtcDaliTextViewDefaultConstructorDestructor_PT(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewDefaultConstructorDestructor : ");

  TextViewProcessor::TextInfoIndices indices;
  DALI_TEST_EQUALS( indices.mParagraphIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( indices.mWordIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( indices.mCharacterIndex, 0u, TEST_LOCATION );

  TextViewProcessor::CharacterLayoutInfo characterLayoutInfo;
  DALI_TEST_EQUALS( characterLayoutInfo.mSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mBearing, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mPosition, Vector3::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mOffset, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mAscender, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mUnderlineThickness, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mUnderlinePosition, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( !characterLayoutInfo.mGlyphActor );
  DALI_TEST_CHECK( characterLayoutInfo.mStyledText.mText.IsEmpty() );
  DALI_TEST_EQUALS( characterLayoutInfo.mColorAlpha, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( NULL == characterLayoutInfo.mGradientInfo );
  DALI_TEST_CHECK( characterLayoutInfo.mIsVisible );
  DALI_TEST_CHECK( characterLayoutInfo.mSetText );
  DALI_TEST_CHECK( characterLayoutInfo.mSetStyle );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo;
  DALI_TEST_EQUALS( wordLayoutInfo.mSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo.mAscender, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo.mType, TextViewProcessor::NoSeparator, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo.mCharactersLayoutInfo.size(), 0u, TEST_LOCATION );

  TextViewProcessor::ParagraphLayoutInfo paragraphLayoutInfo;
  DALI_TEST_EQUALS( paragraphLayoutInfo.mSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo.mAscender, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo.mLineHeightOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo.mWordsLayoutInfo.size(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo.mNumberOfCharacters, 0u, TEST_LOCATION );

  TextViewProcessor::TextLayoutInfo textLayoutInfo;
  DALI_TEST_EQUALS( textLayoutInfo.mWholeTextSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mMaxWordWidth, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mParagraphsLayoutInfo.size(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mNumberOfCharacters, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mMaxItalicsOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mEllipsizeLayoutInfo.mSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mEllipsizeLayoutInfo.mAscender, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mEllipsizeLayoutInfo.mType, TextViewProcessor::NoSeparator, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mEllipsizeLayoutInfo.mCharactersLayoutInfo.size(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewCopyConstructorOperator(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewCopyConstructorOperator : ");

  TextViewProcessor::CharacterLayoutInfo characterLayoutInfo;
  characterLayoutInfo.mSize = Vector2( 1.f, 1.f );
  characterLayoutInfo.mBearing = 1.f;
  characterLayoutInfo.mPosition = Vector3( 1.f, 1.f, 1.f );
  characterLayoutInfo.mOffset = Vector2( 1.f, 1.f );
  characterLayoutInfo.mAscender = 1.f;
  characterLayoutInfo.mUnderlineThickness = 1.f;
  characterLayoutInfo.mUnderlinePosition = 1.f;

  characterLayoutInfo.mGlyphActor = TextActor::New( "Hello" );
  characterLayoutInfo.mStyledText.mText = Text( "Hello" );

  TextViewProcessor::GradientInfo* info = new TextViewProcessor::GradientInfo();
  info->mGradientColor = Vector4( 1.f, 1.f, 1.f, 1.f );
  info->mStartPoint = Vector2( 1.f, 1.f );
  info->mEndPoint = Vector2( 1.f, 1.f );

  characterLayoutInfo.mColorAlpha = 0.f;
  characterLayoutInfo.mGradientInfo = info;

  characterLayoutInfo.mIsVisible = false;
  characterLayoutInfo.mSetText = false;
  characterLayoutInfo.mSetStyle = false;

  TextViewProcessor::CharacterLayoutInfo characterLayoutInfo1;
  characterLayoutInfo1 = characterLayoutInfo;

  DALI_TEST_EQUALS( characterLayoutInfo1.mBearing, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mPosition, Vector3( 1.f, 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mOffset, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mAscender, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mUnderlineThickness, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mUnderlinePosition, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( characterLayoutInfo1.mGlyphActor );
  DALI_TEST_EQUALS( characterLayoutInfo1.mStyledText.mText.GetLength(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mColorAlpha, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mGradientInfo->mGradientColor, Vector4( 1.f, 1.f, 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mGradientInfo->mStartPoint, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo1.mGradientInfo->mEndPoint, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( !characterLayoutInfo1.mIsVisible );
  DALI_TEST_CHECK( !characterLayoutInfo1.mSetText );
  DALI_TEST_CHECK( !characterLayoutInfo1.mSetStyle );

  TextViewProcessor::CharacterLayoutInfo characterLayoutInfo2( characterLayoutInfo );
  DALI_TEST_EQUALS( characterLayoutInfo2.mBearing, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mPosition, Vector3( 1.f, 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mOffset, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mAscender, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mUnderlineThickness, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mUnderlinePosition, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( characterLayoutInfo2.mGlyphActor );
  DALI_TEST_EQUALS( characterLayoutInfo2.mStyledText.mText.GetLength(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mColorAlpha, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mGradientInfo->mGradientColor, Vector4( 1.f, 1.f, 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mGradientInfo->mStartPoint, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo2.mGradientInfo->mEndPoint, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( !characterLayoutInfo2.mIsVisible );
  DALI_TEST_CHECK( !characterLayoutInfo2.mSetText );
  DALI_TEST_CHECK( !characterLayoutInfo2.mSetStyle );

  // Increases coverage.
  characterLayoutInfo2.mGlyphActor.Reset();
  characterLayoutInfo1 = characterLayoutInfo2;
  DALI_TEST_CHECK( !characterLayoutInfo1.mGlyphActor );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo;
  wordLayoutInfo.mSize = Vector2( 1.f, 1.f );
  wordLayoutInfo.mAscender = 1.f;
  wordLayoutInfo.mType = TextViewProcessor::ParagraphSeparator;

  TextViewProcessor::WordLayoutInfo wordLayoutInfo1;
  wordLayoutInfo1 = wordLayoutInfo;

  DALI_TEST_EQUALS( wordLayoutInfo1.mSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo1.mAscender, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo1.mType, TextViewProcessor::ParagraphSeparator, TEST_LOCATION );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo2( wordLayoutInfo );

  DALI_TEST_EQUALS( wordLayoutInfo2.mSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo2.mAscender, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( wordLayoutInfo2.mType, TextViewProcessor::ParagraphSeparator, TEST_LOCATION );

  TextViewProcessor::ParagraphLayoutInfo paragraphLayoutInfo;
  paragraphLayoutInfo.mSize = Vector2( 1.f, 1.f );
  paragraphLayoutInfo.mAscender = 1.f;
  paragraphLayoutInfo.mLineHeightOffset = 1.f;
  paragraphLayoutInfo.mNumberOfCharacters = 1u;

  TextViewProcessor::ParagraphLayoutInfo paragraphLayoutInfo1;
  paragraphLayoutInfo1 = paragraphLayoutInfo;

  DALI_TEST_EQUALS( paragraphLayoutInfo1.mSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo1.mAscender, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo1.mLineHeightOffset, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo1.mNumberOfCharacters, 1u, TEST_LOCATION );

  TextViewProcessor::ParagraphLayoutInfo paragraphLayoutInfo2( paragraphLayoutInfo );

  DALI_TEST_EQUALS( paragraphLayoutInfo2.mSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo2.mAscender, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo2.mLineHeightOffset, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( paragraphLayoutInfo2.mNumberOfCharacters, 1u, TEST_LOCATION );

  TextViewProcessor::TextLayoutInfo textLayoutInfo;
  textLayoutInfo.mWholeTextSize = Vector2( 1.f, 1.f );
  textLayoutInfo.mMaxWordWidth = 1.f;
  textLayoutInfo.mNumberOfCharacters = 1u;
  textLayoutInfo.mMaxItalicsOffset = 1.f;

  TextViewProcessor::TextLayoutInfo textLayoutInfo1;
  textLayoutInfo1 = textLayoutInfo;

  DALI_TEST_EQUALS( textLayoutInfo1.mWholeTextSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo1.mMaxWordWidth, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo1.mNumberOfCharacters, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo1.mMaxItalicsOffset, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  TextViewProcessor::TextLayoutInfo textLayoutInfo2( textLayoutInfo );

  DALI_TEST_EQUALS( textLayoutInfo2.mWholeTextSize, Vector2( 1.f, 1.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mMaxWordWidth, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mNumberOfCharacters, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mMaxItalicsOffset, 1.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewEqualityOperator(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewEqualityOperator : ");

  TextViewProcessor::TextInfoIndices indices;
  TextViewProcessor::TextInfoIndices indices1( 1u, 1u, 1u );

  DALI_TEST_CHECK( !( indices == indices1 ) );

  indices = indices1;

  DALI_TEST_CHECK( indices == indices1 );
  END_TEST;
}
