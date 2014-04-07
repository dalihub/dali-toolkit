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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

// Internal headers are allowed here
#include <dali-toolkit/internal/controls/text-view/relayout-utilities.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Internal;

namespace
{

const Toolkit::Internal::TextView::LayoutParameters DEFAULT_LAYOUT_PARAMETERS;

// Data structures used to create an 'experiment' in TET cases


bool TestEqual( float x, float y )
{
  return ( fabsf( x - y ) < Math::MACHINE_EPSILON_1000 );
}

//////////////////////////////////////////////////////////////////

struct CalculateSubLineLayoutTest
{
  std::string description;
  std::string inputLine;
  float parentWidth;
  std::size_t groupIndex;
  std::size_t wordIndex;
  std::size_t characterIndex;
  TextViewRelayout::HorizontalWrapType splitPolicy;
  float shrinkFactor;

  float resultLineLength;
  float resultMaxCharHeight;
  float resultMaxAscender;
};

bool TestCalculateSubLineLayout( const CalculateSubLineLayoutTest& test,  const char* location )
{
  tet_printf( "%s", test.description.c_str() );

  // Create styled text.
  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( test.inputLine, inputStyledText, true );

  // Create styled text layout info.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  // Prepare input parameters and the result structure and call the function to be tested.

  // Creaqte indices.
  TextViewProcessor::TextInfoIndices indices( 0, test.groupIndex, test.wordIndex, test.characterIndex );

  // Get the input line.
  TextViewProcessor::LineLayoutInfo inputLineLayout;

  if( !relayoutData.mTextLayoutInfo.mLinesLayoutInfo.empty() )
  {
    inputLineLayout = *relayoutData.mTextLayoutInfo.mLinesLayoutInfo.begin();
  }

  // Result struct.
  TextViewRelayout::SubLineLayoutInfo resultLayoutInfo;

  CalculateSubLineLayout( test.parentWidth,
                          indices,
                          inputLineLayout,
                          test.splitPolicy,
                          test.shrinkFactor,
                          resultLayoutInfo  );

  // Check results.
  if( !TestEqual( test.resultLineLength, resultLayoutInfo.mLineLength ) )
  {
    tet_printf( "Fail. different line length %f == %f. %s", test.resultLineLength, resultLayoutInfo.mLineLength, location );
    return false;
  }

  if( !TestEqual( test.resultMaxCharHeight, resultLayoutInfo.mMaxCharHeight ) )
  {
    tet_printf( "Fail. different max character height %f == %f. %s", test.resultMaxCharHeight, resultLayoutInfo.mMaxCharHeight, location );
    return false;
  }

  if( !TestEqual( test.resultMaxAscender, resultLayoutInfo.mMaxAscender ) )
  {
    tet_printf( "Fail. different max ascender %f == %f. %s", test.resultMaxAscender, resultLayoutInfo.mMaxAscender, location );
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////

struct AlignmentOffsetTest
{
  Toolkit::Alignment::Type alignment;
  float parentSize;
  float wholeTextSize;

  float resultOffset;
};

bool TestAlignmentOffset( const AlignmentOffsetTest& test, const char* location )
{
  float offset = 0.f;

  switch( test.alignment )
  {
    case Toolkit::Alignment::HorizontalLeft:
    case Toolkit::Alignment::HorizontalCenter:
    case Toolkit::Alignment::HorizontalRight:
    {
      offset = TextViewRelayout::CalculateXoffset( test.alignment, test.parentSize, test.wholeTextSize );
      break;
    }
    case Toolkit::Alignment::VerticalTop:
    case Toolkit::Alignment::VerticalCenter:
    case Toolkit::Alignment::VerticalBottom:
    {
      offset = TextViewRelayout::CalculateYoffset( test.alignment, test.parentSize, test.wholeTextSize );
      break;
    }
  }

  // Check results.
  if( !TestEqual( test.resultOffset, offset ) )
  {
    tet_printf( "Fail. different offset %f == %f. %s", test.resultOffset, offset, location );
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////

struct JustificationOffsetTest
{
  Toolkit::TextView::LineJustification justification;
  float wholeTextWidth;
  float lineLength;

  float resultOffset;
};

bool TestJustificationOffset( const JustificationOffsetTest& test, const char* location )
{
  float offset = TextViewRelayout::CalculateJustificationOffset( test.justification, test.wholeTextWidth, test.lineLength );

  // Check results.
  if( !TestEqual( test.resultOffset, offset ) )
  {
    tet_printf( "Fail. different offset %f == %f. %s", test.resultOffset, offset, location );
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////

struct CalculateVisibilityTest
{
  Vector3 position;
  Size size;
  Size parentSize;
  TextViewRelayout::VisibilityTestType type;

  bool resultVisible;
};

bool TestCalculateVisibility( const CalculateVisibilityTest& test, const char* location )
{
  if( test.resultVisible != TextViewRelayout::IsVisible( test.position, test.size, test.parentSize, test.type ) )
  {
    tet_printf( "Fail. different visibility. Type %d, %s", test.type, location );
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////

} // namespace

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliTextViewDefaultConstructorDestructor, POSITIVE_TC_IDX );  // Calls structs's default constructor and destructors and checks their default values.
TEST_FUNCTION( UtcDaliTextViewCalculateSubLineLayout, POSITIVE_TC_IDX );        // Checks the function which calculates the layout info of the portion of the line which fits on the text-view width.
TEST_FUNCTION( UtcDaliTextViewCalculateAlignmentOffsets, POSITIVE_TC_IDX );     // Checks the horizontal and vertical alignaments (for the whole text).
TEST_FUNCTION( UtcDaliTextViewCalculateJustificationOffsets, POSITIVE_TC_IDX ); // Checks the justification alignment (line per line).
TEST_FUNCTION( UtcDaliTextViewCalculateVisibility, POSITIVE_TC_IDX );           // Checks the text-actor visibility within the text-view with a rectangle intersection test.

TEST_FUNCTION( UtcDaliTextViewMiscelaneousAsserts, NEGATIVE_TC_IDX );           // Tests some strange asserts.

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliTextViewDefaultConstructorDestructor()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewDefaultConstructorDestructor : ");

  // Test RelayoutParameters defaults.
  TextViewRelayout::RelayoutParameters relayoutParameters;

  DALI_TEST_EQUALS( relayoutParameters.mPositionOffset, Vector3::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mLineSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mWordSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mCharacterSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mIndices.mLineIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mIndices.mGroupIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mIndices.mWordIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mIndices.mCharacterIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( relayoutParameters.mCharacterGlobalIndex, 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !relayoutParameters.mIsFirstCharacter );
  DALI_TEST_CHECK( !relayoutParameters.mIsFirstCharacterOfWord );
  DALI_TEST_CHECK( !relayoutParameters.mIsNewLine );
  DALI_TEST_CHECK( !relayoutParameters.mIsNewLineCharacter );
  DALI_TEST_CHECK( !relayoutParameters.mIsWhiteSpace );
  DALI_TEST_CHECK( !relayoutParameters.mIsVisible );

  // Test FadeParameter defaults
  TextViewRelayout::FadeParameters fadeParameters;

  DALI_TEST_EQUALS( fadeParameters.mRightFadeBoundary, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mRightFadeThreshold, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mRightFadeBoundaryOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mRightFadeThresholdOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mRightAlphaCoeficients, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mLeftFadeBoundary, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mLeftFadeThreshold, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mLeftFadeBoundaryOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mLeftFadeThresholdOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mLeftAlphaCoeficients, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mTopFadeBoundary, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mTopFadeThreshold, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mTopFadeBoundaryOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mTopFadeThresholdOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mTopAlphaCoeficients, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mBottomFadeBoundary, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mBottomFadeThreshold, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mBottomFadeBoundaryOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mBottomFadeThresholdOffset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeParameters.mBottomAlphaCoeficients, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_CHECK( !fadeParameters.mIsPartiallyVisible );

  // Test EllipsizeParameters defaults
  TextViewRelayout::EllipsizeParameters ellipsizeParameters;

  DALI_TEST_EQUALS( ellipsizeParameters.mPosition, Vector3::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( ellipsizeParameters.mLineDescender, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( ellipsizeParameters.mLineWidth, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( ellipsizeParameters.mEllipsizeBoundary, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( ellipsizeParameters.mFirstIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( ellipsizeParameters.mLastIndex, 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !ellipsizeParameters.mEllipsizeLine );
  DALI_TEST_CHECK( !ellipsizeParameters.mIsLineWidthFullyVisible );
  DALI_TEST_CHECK( !ellipsizeParameters.mIsLineHeightFullyVisible );
  DALI_TEST_CHECK( !ellipsizeParameters.mIsNextLineFullyVisibleHeight );
  DALI_TEST_CHECK( !ellipsizeParameters.mCreateEllipsizedTextActors );
  DALI_TEST_CHECK( !ellipsizeParameters.mLineFits );
  DALI_TEST_CHECK( !ellipsizeParameters.mWordFits );

  // Test UnderlineInfo defaults
  TextViewRelayout::UnderlineInfo underlineInfo;

  DALI_TEST_EQUALS( underlineInfo.mMaxHeight, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( underlineInfo.mMaxThickness, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( underlineInfo.mPosition, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  // Test TextUnderlineStatus defaults
  TextViewRelayout::TextUnderlineStatus textUnderlineStatus;

  DALI_TEST_CHECK( textUnderlineStatus.mUnderlineInfo.empty() );
  DALI_TEST_EQUALS( textUnderlineStatus.mCharacterGlobalIndex, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textUnderlineStatus.mLineGlobalIndex, 0u, TEST_LOCATION );
  DALI_TEST_CHECK( !textUnderlineStatus.mCurrentUnderlineStatus );

  // Test SubLineLayoutInfo defaults
  TextViewRelayout::SubLineLayoutInfo subLineLayoutInfo;

  DALI_TEST_EQUALS( subLineLayoutInfo.mLineLength, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( subLineLayoutInfo.mMaxCharHeight, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( subLineLayoutInfo.mMaxAscender, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
}

static void UtcDaliTextViewCalculateSubLineLayout()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewCalculateSubLineLayout : ");

  struct CalculateSubLineLayoutTest calculateSubLineLayoutTest[] =
  {
    //WrapByCharacter
    {
      "The line is wraped by character. All characters have the same size.",
      "Hello world", // input line
      100.f,         // parent width
      0,
      0,              // indices
      0,
      TextViewRelayout::WrapByCharacter, // split policy
      1.f,
      // results
      91.041672f, // line length. (only fits 8 characters 8x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by character. There are characters with different sizes.",
      "Hello <font size='14'>world</font>", // input line
      100.f,         // parent width
      0,
      0,              // indices
      0,
      TextViewRelayout::WrapByCharacter, // split policy
      1.f,
      // results
      94.835075f, // line length. (only fits 8 characters 6x11.38 + 2x13.27)
      13.276911f, // max character height
      11.949220f  // max ascender
    },
    {
      "The line is wraped by character. There are characters with different sizes. It calculates the layout for the second line.",
      "Hello <font size='14'>wo</font>rld hell<font size='14'>o world</font>", // input line
      100.f,         // parent width
      0,
      2,              // indices. The third character of the third word starts in a new line.
      2,
      TextViewRelayout::WrapByCharacter, // split policy
      1.f,
      // results
      91.041672f, // line length. (only fits 8 characters 8x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by character. There are characters with different sizes. It calculates the layout for the third line.",
      "Hello <font size='14'>wo</font>rld hell<font size='14'>o world</font>", // input line
      100.f,         // parent width
      0,
      4,              // indices. The fifth character of the fifth word starts in a new line.
      4,
      TextViewRelayout::WrapByCharacter, // split policy
      1.f,
      // results
      92.938377f, // line length. (only fits 8 characters 8x11.38)
      13.276911f, // max character height
      11.949220f  // max ascender
    },

    //WrapByWord
    {
      "The line is wraped by word. All characters have the same size.",
      "Hello world", // input line
      100.f,         // parent width
      0,
      0,              // indices. It shouldn't use the index character so 9999999 shouldn't make it crash.
      9999999,
      TextViewRelayout::WrapByWord, // split policy
      1.f,
      // results
      56.901047f, // line length. (only fits 5 characters 5x11.38, white space is not counted)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by word. There are characters with different sizes.",
      "Hell<font size='14'>o</font> world", // input line
      100.f,         // parent width
      0,
      0,              // indices.
      0,
      TextViewRelayout::WrapByWord, // split policy
      1.f,
      // results
      58.797747f, // line length. (only fits 5 characters 4x11.38 + 13.276911, white space is not counted)
      13.276911f, // max character height
      11.949220f  // max ascender
    },
    {
      "The line is wraped by word. There are characters with different sizes. It calculates the layout for the second line.",
      "Hello <font size='14'>wo</font>rld <font size='16'>hello world</font>", // input line
      100.f,         // parent width
      0,
      2,              // indices. The third word starts in a new line.
      0,
      TextViewRelayout::WrapByWord, // split policy
      1.f,
      // results
      60.694449f, // line length. (only fits 5 characters 2x13.276911 + 3x11.38)
      13.276911f, // max character height
      11.949220f  // max ascender
    },
    {
      "The line is wraped by word. The word doen't fit.",
      "Hello world", // input line
      40.f,          // parent width
      0,
      0,              // indices. The third word starts in a new line.
      0,
      TextViewRelayout::WrapByWord, // split policy
      1.f,
      // results
      0.f,        // line length. (The word doesn't fit)
      11.380209f, // max character height
      10.242188f  // max ascender
    },

    //WrapByWordAndSplit
    {
      "The line is wraped by word and by character. All characters have the same size. There is not a long word.",
      "Hello world hello world", // input line
      100.f,         // parent width
      0,
      0,              // indices.
      0,
      TextViewRelayout::WrapByWordAndSplit, // split policy
      1.f,
      // results
      56.901047f, // line length. (only fits 5 characters 5x11.38, white space is not counted)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by word and by character. All characters have the same size. There is a long word.",
      "Helloooooooo world", // input line
      100.f,         // parent width
      0,
      0,              // indices.
      0,
      TextViewRelayout::WrapByWordAndSplit, // split policy
      1.f,
      // results
      91.041672f, // line length. (only fits 8 characters 8x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by word and by character. There are characters with different sizes. There is a long word. It calculates the layout for the second line.",
      "Helloooooooo <font size='14'>world</font>", // input line
      100.f,         // parent width
      0,
      0,              // indices.
      8,
      TextViewRelayout::WrapByWordAndSplit, // split policy
      1.f,
      // results
      45.520836f, // line length. (only fits 8 characters 8x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by word and by character. There are characters with different sizes. There is a shrink factor.",
      "Helloooooooo<font size='14'> world</font>", // input line
      100.f,         // parent width
      0,
      0,              // indices.
      8,
      TextViewRelayout::WrapByWordAndSplit, // split policy
      0.7f,
      // results
      95.593755f, // line length. (only fits 12 characters 8x11.38)
      7.9661463f, // max character height
      7.169531f  // max ascender
    },

    //WrapByLineAndSplit
    {
      "The line is wraped by end of line and by character. All characters have the same size.",
      "Hello world", // input line
      100.f,         // parent width
      0,
      0,              // indices
      0,
      TextViewRelayout::WrapByLineAndSplit, // split policy
      1.f,
      // results
      91.041672f, // line length. (only fits 8 characters 8x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line fits in the width.",
      "Hello", // input line
      100.f,         // parent width
      0,
      0,             // indices
      0,
      TextViewRelayout::WrapByLineAndSplit, // split policy
      1.f,
      // results
      56.901047f, // line length. (only fits 5 characters 5x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
    {
      "The line is wraped by end of line and by character. All characters have the same size. It calculates the layout for the second line.",
      "Hello world, hello world", // input line
      100.f,         // parent width
      0,
      2,             // indices
      2,
      TextViewRelayout::WrapByLineAndSplit, // split policy
      1.f,
      // results
      91.041672f, // line length. (only fits 8 characters 8x11.38)
      11.380209f, // max character height
      10.242188f  // max ascender
    },
  };
  const std::size_t numberOfTests( 15 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const CalculateSubLineLayoutTest& test = calculateSubLineLayoutTest[index];

    if( !TestCalculateSubLineLayout( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewCalculateAlignmentOffsets()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewCalculateAlignmentOffsets : ");

  struct AlignmentOffsetTest alignmentOffsetTest[] =
  {
    {
      Toolkit::Alignment::HorizontalLeft,
      100.f,
      75.f,
      0.f
    },
    {
      Toolkit::Alignment::HorizontalCenter,
      100.f,
      75.f,
      12.5f
    },
    {
      Toolkit::Alignment::HorizontalRight,
      100.f,
      75.f,
      25.f
    },
    {
      Toolkit::Alignment::VerticalTop,
      100.f,
      75.f,
      0.f
    },
    {
      Toolkit::Alignment::VerticalCenter,
      100.f,
      75.f,
      12.5f
    },
    {
      Toolkit::Alignment::VerticalBottom,
      100.f,
      75.f,
      25.f
    }
  };
  const std::size_t numberOfTests( 6 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const AlignmentOffsetTest& test = alignmentOffsetTest[index];

    if( !TestAlignmentOffset( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewCalculateJustificationOffsets()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewCalculateJustificationOffsets : ");

  struct JustificationOffsetTest justificationOffsetTest[] =
  {
    {
      Toolkit::TextView::Left,
      100.f,
      75.f,
      0.f
    },
    {
      Toolkit::TextView::Justified,
      100.f,
      75.f,
      0.f
    },
    {
      Toolkit::TextView::Center,
      100.f,
      150.f,
      -25.f
    },
    {
      Toolkit::TextView::Right,
      100.f,
      75.f,
      25.f
    },
  };
  const std::size_t numberOfTests( 4 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const JustificationOffsetTest& test = justificationOffsetTest[index];

    if( !TestJustificationOffset( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}


static void UtcDaliTextViewCalculateVisibility()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewCalculateVisibility : ");

  struct CalculateVisibilityTest calculateVisibilityTest[] =
  {
    {
      Vector3( 0.f, 10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE,
      true
    },
    {
      Vector3( 10.f, 10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE,
      true
    },
    {
      Vector3( 0.f, 10.f, 0.f ),
      Size( 150.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE,
      false
    },
    {
      Vector3( 0.f, 10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE_WIDTH,
      true
    },
    {
      Vector3( 95.f, 10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE_WIDTH,
      false
    },
    {
      Vector3( 0.f, 10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE_HEIGHT,
      true
    },
    {
      Vector3( 0.f, 0.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::FULLY_VISIBLE_HEIGHT,
      false
    },
    {
      Vector3( -10.f, 10.f, 0.f ),
      Size( 150.f, 150.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::PARTIALLY_VISIBLE,
      true
    },
    {
      Vector3( -100.f, -100.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::PARTIALLY_VISIBLE,
      false
    },
    {
      Vector3( -10.f, 10.f, 0.f ),
      Size( 50.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::PARTIALLY_VISIBLE_WIDTH,
      true
    },
    {
      Vector3( 110.f, 10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::PARTIALLY_VISIBLE_WIDTH,
      false
    },
    {
      Vector3( 0.f, 20.f, 0.f ),
      Size( 10.f, 50.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::PARTIALLY_VISIBLE_HEIGHT,
      true
    },
    {
      Vector3( 0.f, -10.f, 0.f ),
      Size( 10.f, 10.f ),
      Size( 100.f, 100.f ),
      TextViewRelayout::PARTIALLY_VISIBLE_HEIGHT,
      false
    },
  };
  const std::size_t numberOfTests( 13 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const CalculateVisibilityTest& test = calculateVisibilityTest[index];

    if( !TestCalculateVisibility( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewMiscelaneousAsserts()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMiscelaneousAsserts : ");

  float offset = 0.f;

  bool assert1 = false;
  bool assert2 = false;
  try
  {
    offset = Toolkit::Internal::TextViewRelayout::CalculateXoffset( Toolkit::Alignment::VerticalTop, 100.f, 50.f );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewRelayout::CalculateXoffset: Wrong horizontal text alignment. Did you set a vertical one?\"", TEST_LOCATION );
    assert1 = true;
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }
  DALI_TEST_EQUALS( offset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  try
  {
    offset = Toolkit::Internal::TextViewRelayout::CalculateYoffset( Toolkit::Alignment::HorizontalRight, 100.f, 50.f );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewRelayout::CalculateXoffset: Wrong vertical text alignment. Did you set an horizontal one?\"", TEST_LOCATION );
    assert2 = true;
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }
  DALI_TEST_EQUALS( offset, 0.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( assert1 && assert2 );

}
