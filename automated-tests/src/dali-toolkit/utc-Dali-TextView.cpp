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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_text_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_text_view_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

const char* const PROPERTY_TEXT = "text";
const char* const PROPERTY_MULTILINE_POLICY = "multiline-policy";
const char* const PROPERTY_WIDTH_EXCEED_POLICY = "width-exceed-policy";
const char* const PROPERTY_HEIGHT_EXCEED_POLICY = "height-exceed-policy";
const char* const PROPERTY_LINE_JUSTIFICATION = "line-justification";
const char* const PROPERTY_FADE_BOUNDARY_LEFT = "fade-boundary-left";
const char* const PROPERTY_FADE_BOUNDARY_RIGHT = "fade-boundary-right";
const char* const PROPERTY_FADE_BOUNDARY_TOP = "fade-boundary-top";
const char* const PROPERTY_FADE_BOUNDARY_BOTTOM = "fade-boundary-bottom";
const char* const PROPERTY_LINE_HEIGHT_OFFSET = "line-height-offset";
const char* const PROPERTY_HORIZONTAL_ALIGNMENT = "horizontal-alignment";
const char* const PROPERTY_VERTICAL_ALIGNMENT = "vertical-alignment";

bool TestEqual( float x, float y )
{
  return !( fabsf( x - y ) > Math::MACHINE_EPSILON_1000 );
}

static bool gObjectCreatedCallBackCalled;
static unsigned int gNumberObjectCreated;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
  ++gNumberObjectCreated;
}

static bool gTextScrolled;
static Vector2 gScrollDelta;
static void TestTextScrolled( TextView textView, Vector2 scrollDelta )
{
  gTextScrolled = true;
  gScrollDelta = scrollDelta;
}

} // namespace


int UtcDaliTextViewNew(void)
{
  tet_infoline("UtcDaliTextViewNew: ");
  ToolkitTestApplication application;

  // Test default constructor.
  TextView view;

  DALI_TEST_CHECK( !view );

  // Test default initialization.
  view = TextView::New();

  DALI_TEST_CHECK( view );

  // Test copy constructor and asignment operator.
  TextView viewCopy1;

  viewCopy1 = view;

  DALI_TEST_CHECK( viewCopy1 );

  TextView viewCopy2( view );

  DALI_TEST_CHECK( viewCopy2 );

  // Test down cast.
  Actor actorView;

  actorView = view;

  TextView downCastView = TextView::DownCast( actorView );

  DALI_TEST_CHECK( downCastView );

  // Test constructor with a given text.

  const std::string text( "Hello world!" );

  const float DESCENDER = 8.0f;

  TextView view1 = TextView::New( text );

  DALI_TEST_EQUALS( view1.GetText(), text, TEST_LOCATION );

  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( text, styledText, true );

  TextView view2 = TextView::New( styledText );

  DALI_TEST_EQUALS( view2.GetText(), text, TEST_LOCATION );

  // Check the default Toolkit::TextView::CharacterLayoutInfo::CharacterLayoutInfo() to increase coverage.
  TextView::CharacterLayoutInfo characterLayoutInfo;

  DALI_TEST_EQUALS( characterLayoutInfo.mSize, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mPosition, Vector3::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mIsNewLineChar, false, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mIsRightToLeftCharacter, false, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mIsVisible, true, TEST_LOCATION );

  TextView::CharacterLayoutInfo characterLayoutInfo2( Size( 2.f, 2.f ),
                                                      Vector3( 3.f, 4.f, 5.f ),
                                                      true,
                                                      true,
                                                      false,
                                                      DESCENDER );

  characterLayoutInfo = characterLayoutInfo2;

  DALI_TEST_EQUALS( characterLayoutInfo.mSize, Size( 2.f, 2.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mPosition, Vector3( 3.f, 4.f, 5.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mIsNewLineChar, true, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mIsRightToLeftCharacter, true, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mIsVisible, false, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo.mDescender, DESCENDER , TEST_LOCATION );


  TextView::CharacterLayoutInfo characterLayoutInfo3( characterLayoutInfo );

  DALI_TEST_EQUALS( characterLayoutInfo3.mSize, Size( 2.f, 2.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo3.mPosition, Vector3( 3.f, 4.f, 5.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo3.mIsNewLineChar, true, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo3.mIsRightToLeftCharacter, true, TEST_LOCATION );
  DALI_TEST_EQUALS( characterLayoutInfo3.mIsVisible, false, TEST_LOCATION );

  // Check the default Toolkit::TextView::TextLayoutInfo::TextLayoutInfo() to increase coverage.

  TextView::TextLayoutInfo textLayoutInfo;
  DALI_TEST_EQUALS( textLayoutInfo.mCharacterLayoutInfoTable.size(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mCharacterLogicalToVisualMap.size(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mCharacterVisualToLogicalMap.size(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mTextSize, Size::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo.mScrollOffset, Vector2::ZERO, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  textLayoutInfo.mCharacterLayoutInfoTable.push_back( characterLayoutInfo );
  textLayoutInfo.mCharacterLogicalToVisualMap.push_back( 1 );
  textLayoutInfo.mCharacterVisualToLogicalMap.push_back( 1 );
  textLayoutInfo.mTextSize = Size( 10.f, 10.f );
  textLayoutInfo.mScrollOffset = Vector2( 5.f, 5.f );

  TextView::TextLayoutInfo textLayoutInfo2( textLayoutInfo );

  DALI_TEST_EQUALS( textLayoutInfo2.mCharacterLayoutInfoTable.size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mCharacterLogicalToVisualMap.size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mCharacterVisualToLogicalMap.size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mTextSize, Size( 10.f, 10.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo2.mScrollOffset, Vector2( 5.f, 5.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  TextView::TextLayoutInfo textLayoutInfo3;

  textLayoutInfo3 = textLayoutInfo2;

  DALI_TEST_EQUALS( textLayoutInfo3.mCharacterLayoutInfoTable.size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo3.mCharacterLogicalToVisualMap.size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo3.mCharacterVisualToLogicalMap.size(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo3.mTextSize, Size( 10.f, 10.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  DALI_TEST_EQUALS( textLayoutInfo3.mScrollOffset, Vector2( 5.f, 5.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    TextView view = TextView::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliTextViewSetAndGetText(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetText: ");
  ToolkitTestApplication application;

  TextView view = TextView::New();
  view.SetSnapshotModeEnabled( false ); // Disables offscreen rendering.

  std::string str( "Text with differing aCeNdEr and dEcEnDeR" );

  view.SetText( str );
  DALI_TEST_EQUALS( view.GetText(), str, TEST_LOCATION );

  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( str, styledText, true );

  view.SetText( styledText );
  DALI_TEST_EQUALS( view.GetText(), str, TEST_LOCATION );

  // Test the number of text actor created.

  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gNumberObjectCreated = 0u;
  registry.ObjectCreatedSignal().Connect(&TestCallback);

  // Following string should create three text-actors ([Hel], [lo wo] and [rld]).
  std::string text( "Hel<font size='10'>lo wo</font>rld!\n"
                    "\n" );

  view.SetMarkupProcessingEnabled( true ); // Enables markup processing.

  Stage::GetCurrent().Add( view );
  view.SetText( text );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( 3u, gNumberObjectCreated, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewSetStyleToCurrentText(void)
{
  tet_infoline("UtcDaliTextViewSetStyleToCurrentText: ");
  ToolkitTestApplication application;

  TextStyle style;
  style.SetItalics( true );

  const std::string text( "앞서 농식품부 주이석 검역검사본부\n"
                          "동물방역부장을 단장으로 하는\n"
                          "민관합동조사단은 지난달 30일부터\n"
                          "12일간의 현지 조사활동을 마치고\n"
                          "11일 새벽 귀국했습니다." );
  TextView view = TextView::New( text );

  bool fail = false;
  try
  {
    view.SetStyleToCurrentText( style );
  }
  catch( ... )
  {
    tet_printf( "Tet case fails\n" );
    fail = true;
    tet_result(TET_FAIL);
  }

  DALI_TEST_CHECK( !fail );
  END_TEST;
}

int UtcDaliTextViewSetAndGetLineHeight(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetLineHeight: ");

  ToolkitTestApplication application;

  const float lineHeightOffset( 9.f );

  TextView textView = TextView::New();

  textView.SetLineHeightOffset( PointSize( lineHeightOffset ) );

  DALI_TEST_EQUALS( float(textView.GetLineHeightOffset()), lineHeightOffset, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewSetAndGetFadeBoundary(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetFadeBoundary: ");

  ToolkitTestApplication application;

  TextView::FadeBoundary fadeBoundary( PixelSize( 0 ), PixelSize( 20 ), PixelSize( 0 ), PixelSize( 10 ) );

  TextView textView = TextView::New( "Hello world!" );

  textView.SetFadeBoundary( fadeBoundary );

  TextView::FadeBoundary fadeBoundary2 = textView.GetFadeBoundary();

  DALI_TEST_EQUALS( fadeBoundary.mLeft, fadeBoundary2.mLeft, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mRight, fadeBoundary2.mRight, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mTop, fadeBoundary2.mTop, TEST_LOCATION );
  DALI_TEST_EQUALS( fadeBoundary.mBottom, fadeBoundary2.mBottom, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewSetAndGetEllipsizeText(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetEllipsizeText: ");

  ToolkitTestApplication application;

  TextView textView = TextView::New( "Hello world!" );

  textView.SetEllipsizeText( std::string( "..." ) );

  DALI_TEST_EQUALS( std::string( "..." ), textView.GetEllipsizeText(), TEST_LOCATION );

  Toolkit::MarkupProcessor::StyledTextArray styledTextArray;

  GetStyledTextArray( std::string( "..." ), styledTextArray, true );

  textView.SetEllipsizeText( styledTextArray );

  DALI_TEST_EQUALS( std::string( "..." ), textView.GetEllipsizeText(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTextViewSetAndGetWidthExceedPolicy(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetWidthExceedPolicy: ");

  ToolkitTestApplication application;

  const TextView::ExceedPolicy EXCEED_POLICIES[] = { TextView::Original, TextView::Fade, TextView::Split, TextView::ShrinkToFit };
  const unsigned int NUM_EXCEED_POLICIES = sizeof( EXCEED_POLICIES ) / sizeof( unsigned int );

  TextView textView = TextView::New( "Hello world!" );

  for( unsigned int epIndex = 0; epIndex < NUM_EXCEED_POLICIES; ++epIndex )
  {
    textView.SetWidthExceedPolicy( EXCEED_POLICIES[epIndex] );

    DALI_TEST_EQUALS( textView.GetWidthExceedPolicy(), EXCEED_POLICIES[epIndex], TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTextViewSetAndGetHeightExceedPolicy(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetHeightExceedPolicy: ");

  ToolkitTestApplication application;

  const TextView::ExceedPolicy EXCEED_POLICIES[] = { TextView::Original, TextView::Fade, TextView::ShrinkToFit };
  const unsigned int NUM_EXCEED_POLICIES = sizeof( EXCEED_POLICIES ) / sizeof( unsigned int );

  TextView textView = TextView::New( "Hello world!" );

  for( unsigned int epIndex = 0; epIndex < NUM_EXCEED_POLICIES; ++epIndex )
  {
    textView.SetHeightExceedPolicy( EXCEED_POLICIES[epIndex] );

    DALI_TEST_EQUALS( textView.GetHeightExceedPolicy(), EXCEED_POLICIES[epIndex], TEST_LOCATION );
  }
  END_TEST;
}

/*
// Re-enable this test case when ALL layout options work in TextView, currently this breaks TCT
// output because too many warnings/errors are printed out
//int UtcDaliTextViewTestLayoutOptions01(void)
{
  tet_infoline("UtcDaliTextViewTestLayoutOptions01: ");

  ToolkitTestApplication application;

  const std::string text( "앞서 농식품부 주이석 검역검사본부\n"
                          "동물방역부장을 단장으로 하는\n"
                          "민관합동조사단은 지난달 30일부터\n"
                          "12일간의 현지 조사활동을 마치고\n"
                          "11일 새벽 귀국했습니다." );

  const TextView::MultilinePolicy MULTILINE_POLICIES[] = { TextView::SplitByNewLineChar, TextView::SplitByWord, TextView::SplitByChar };
  const TextView::ExceedPolicy EXCEED_WIDTH_POLICIES[] = { TextView::Original, TextView::Fade, TextView::Split, TextView::ShrinkToFit, TextView::EllipsizeEnd };
  const TextView::ExceedPolicy EXCEED_HEIGHT_POLICIES[] = { TextView::Original, TextView::Fade, TextView::ShrinkToFit };
  const Alignment::Type TEXT_ALIGNMENT[] = { static_cast<Alignment::Type>( Alignment::HorizontalLeft | Alignment::VerticalTop ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalLeft | Alignment::VerticalCenter ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalLeft | Alignment::VerticalBottom ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalCenter | Alignment::VerticalTop ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalCenter | Alignment::VerticalCenter ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalCenter | Alignment::VerticalBottom ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalRight | Alignment::VerticalTop ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalRight | Alignment::VerticalCenter ),
                                             static_cast<Alignment::Type>( Alignment::HorizontalRight | Alignment::VerticalBottom ) };
  const TextView::LineJustification LINE_JUSTIFICATION[] = { TextView::Left, TextView::Center, TextView::Right, TextView::Justified };

  const unsigned int NUM_MULTILINE_POLICIES = sizeof( MULTILINE_POLICIES ) / sizeof( unsigned int );
  const unsigned int NUM_WIDTH_EXCEED_POLICIES = sizeof( EXCEED_WIDTH_POLICIES ) / sizeof( unsigned int );
  const unsigned int NUM_HEIGHT_EXCEED_POLICIES = sizeof( EXCEED_HEIGHT_POLICIES ) / sizeof( unsigned int );
  const unsigned int NUM_TEXT_ALIGNMENT = sizeof( TEXT_ALIGNMENT ) / sizeof( unsigned int );
  const unsigned int NUM_LINE_JUSTIFICATION = sizeof( LINE_JUSTIFICATION ) / sizeof( unsigned int );

  TextView textView = TextView::New( text );
  textView.SetSnapshotModeEnabled( false ); // Disables offscreen rendering.

  Stage::GetCurrent().Add( textView );

  TextView::TextLayoutInfo textLayoutInfo;

  for( unsigned int mlpIndex = 0; mlpIndex < NUM_MULTILINE_POLICIES; ++mlpIndex )
  {
    textView.SetMultilinePolicy( MULTILINE_POLICIES[mlpIndex] );
    for( unsigned int ewpIndex = 0; ewpIndex < NUM_WIDTH_EXCEED_POLICIES; ++ewpIndex )
    {
      textView.SetWidthExceedPolicy( EXCEED_WIDTH_POLICIES[ewpIndex] );
      for( unsigned int ehpIndex = 0; ehpIndex < NUM_HEIGHT_EXCEED_POLICIES; ++ehpIndex )
      {
        textView.SetHeightExceedPolicy( EXCEED_HEIGHT_POLICIES[ehpIndex] );
        for( unsigned int taIndex = 0; taIndex < NUM_TEXT_ALIGNMENT; ++taIndex )
        {
          textView.SetTextAlignment( TEXT_ALIGNMENT[taIndex] );
          for( unsigned int ljIndex = 0; ljIndex < NUM_LINE_JUSTIFICATION; ++ljIndex )
          {
            textView.SetLineJustification( LINE_JUSTIFICATION[ljIndex] );

            try
            {
              textView.GetTextLayoutInfo( textLayoutInfo );

              application.SendNotification();
              application.Render();
            }
            catch( Dali::DaliException& e )
            {
              DALI_TEST_EQUALS( e.mCondition, "!\"TextView::CombineExceedPolicies() Invalid width and height exceed policies combination\"", TEST_LOCATION );
            }
            catch( ... )
            {
              tet_printf( "Tet case fails\n" );
              tet_printf( "      MultilinePolicy : %d\n", MULTILINE_POLICIES[mlpIndex] );
              tet_printf( "   Width ExceedPolicy : %d\n", EXCEED_WIDTH_POLICIES[ewpIndex] );
              tet_printf( "  Height ExceedPolicy : %d\n", EXCEED_HEIGHT_POLICIES[ehpIndex] );
              tet_printf( "        TextAlignment : %d\n", TEXT_ALIGNMENT[taIndex] );
              tet_printf( "    LineJustification : %d\n", LINE_JUSTIFICATION[ljIndex] );
              tet_result(TET_FAIL);
            }

            DALI_TEST_CHECK( LINE_JUSTIFICATION[ljIndex] == textView.GetLineJustification() );
          }
          DALI_TEST_CHECK( TEXT_ALIGNMENT[taIndex] == textView.GetTextAlignment() );
        }
        DALI_TEST_CHECK( EXCEED_HEIGHT_POLICIES[ehpIndex] == textView.GetHeightExceedPolicy() );
      }
      DALI_TEST_CHECK( EXCEED_WIDTH_POLICIES[ewpIndex] == textView.GetWidthExceedPolicy() );
    }
    DALI_TEST_CHECK( MULTILINE_POLICIES[mlpIndex] == textView.GetMultilinePolicy() );
  }
  END_TEST;
}
*/

int UtcDaliTextViewTestLayoutOptions02(void)
{
  tet_infoline("UtcDaliTextViewTestLayoutOptions02: ");
  ToolkitTestApplication application;

  // Check some configurations.

  TextView textView = TextView::New();
  textView.SetSnapshotModeEnabled( false ); // Disables offscreen rendering.
  textView.SetMarkupProcessingEnabled( true ); // Enables markup processing.

  Stage::GetCurrent().Add( textView );

  // SplitByWord and ShrinkToFit.
  // Centered alignment.
  // Centered justification.
  // Don't create a text actor per character.

  textView.SetMultilinePolicy( TextView::SplitByWord );
  textView.SetWidthExceedPolicy( TextView::ShrinkToFit );
  textView.SetHeightExceedPolicy( TextView::ShrinkToFit );
  textView.SetTextAlignment( static_cast<Alignment::Type>( Alignment::HorizontalCenter | Alignment::VerticalTop ) );
  textView.SetLineJustification( TextView::Center );
  textView.SetSize( 136.56252f, 100.f );

  textView.SetText( "Hel<font color='green'>lo wo</font>rld!" );

  application.SendNotification();
  application.Render();

  std::vector<Size> sizes;
  sizes.push_back( Size( 34.14063f, 11.380210f ) );              //
  sizes.push_back( Size( 56.90105f, 11.380210f ) );              //
  sizes.push_back( Size( 45.52084f, 11.380210f ) );              // By default characters have width and height values of 11.380210.
                                                                 // The result should be a line with the text 'Hello world' as shown below.
  std::vector<Vector3> positions;                                //  ____________
  positions.push_back( Vector3( 0.000008f, 11.380209f, 0.f ) );  // |Hello world!|
  positions.push_back( Vector3( 34.14063f, 11.380209f, 0.f ) );  //  ------------
  positions.push_back( Vector3( 91.04168f, 11.380209f, 0.f ) );  //

  DALI_TEST_CHECK( positions.size() == textView.GetChildCount() ); // Check text has two text-actors.

  for( std::size_t index = 0, num = textView.GetChildCount(); index < num; ++index )
  {
    const Vector3& size = textView.GetChildAt(index).GetCurrentSize();
    const Vector3& position = textView.GetChildAt(index).GetCurrentPosition();

    DALI_TEST_EQUALS( size.width, sizes[index].width, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( size.height, sizes[index].height, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( position.width, positions[index].width, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( position.height, positions[index].height, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }

  textView.SetSize( 50.f, 50.f );
  textView.SetTextAlignment( static_cast<Alignment::Type>( Alignment::HorizontalCenter | Alignment::VerticalCenter ) );
  textView.SetSizePolicy( Toolkit::Control::Fixed, Toolkit::Control::Fixed );
  textView.SetLineJustification( Toolkit::TextView::Left );

  application.SendNotification();
  application.Render();

  sizes.clear();
  sizes.push_back( Size( 24.999999f, 8.333333f ) );              //
  sizes.push_back( Size( 24.999999f, 8.333333f ) );              //
  sizes.push_back( Size( 16.666666f, 8.333333f ) );              // Longest word is 'world!' (6 characters x 11.380210) which doesn't fit in the 50x50 box.
  sizes.push_back( Size( 33.333332f, 8.333333f ) );              // The scale factor is 0.732265339, so the character size is 8.333333.
                                                                 // Text should be split in two lines, centered in the vertical dimension and fitted in the horizontal one.
  positions.clear();                                             // As shown below, the text is two lines and centered in the vertical dimension and
  positions.push_back( Vector3(  0.000008f, 25.223114f, 0.f ) ); // it should start in middle height (~25).
  positions.push_back( Vector3( 24.999999f, 25.223114f, 0.f ) ); //   ______
  positions.push_back( Vector3(  0.000006f, 33.556446f, 0.f ) ); //  |      |
  positions.push_back( Vector3( 16.666666f, 33.556446f, 0.f ) ); //  |Hello |
                                                                 //  |world!|
                                                                 //  |______|
                                                                 //

  DALI_TEST_CHECK( positions.size() == textView.GetChildCount() ); // Check text has two text-actors.

  for( std::size_t index = 0, num = textView.GetChildCount(); index < num; ++index )
  {
    const Vector3& size = textView.GetChildAt(index).GetCurrentSize();
    const Vector3& position = textView.GetChildAt(index).GetCurrentPosition();

    DALI_TEST_EQUALS( size.width, sizes[index].width, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( size.height, sizes[index].height, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( position.width, positions[index].width, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( position.height, positions[index].height, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }

  // TODO: Add more tests when TextView implementation is finished.
  END_TEST;
}

int UtcDaliTextViewInsertRemoveText(void)
{
  tet_infoline("UtcDaliTextViewInsertRemoveText: ");
  ToolkitTestApplication application;

  std::string text("Hello ");

  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( text, styledText, true );

  TextView view = TextView::New( "world!" );

  view.InsertTextAt( 0, styledText );

  DALI_TEST_EQUALS( view.GetText(), std::string("Hello world!"), TEST_LOCATION );

  view.RemoveTextFrom( 4, 5 );

  DALI_TEST_EQUALS( view.GetText(), std::string("Hellld!"), TEST_LOCATION );

  view.InsertTextAt( 0, "Hello " );

  DALI_TEST_EQUALS( view.GetText(), std::string("Hello Hellld!"), TEST_LOCATION );


  view.InsertTextAt( 0, "Hello " );
  view.InsertTextAt( 0, "Hello " );
  view.InsertTextAt( 0, "Hello " );
  view.InsertTextAt( 0, "Hello " );
  view.RemoveTextFrom( 4, 2 );
  view.RemoveTextFrom( 4, 2 );
  view.RemoveTextFrom( 4, 2 );
  view.RemoveTextFrom( 4, 2 );
  view.RemoveTextFrom( 4, 2 );
  view.SetText( "Hello world!" );

  DALI_TEST_EQUALS( view.GetText(), std::string("Hello world!"), TEST_LOCATION );

  view.ReplaceTextFromTo( 5, 1, "" );

  DALI_TEST_EQUALS( view.GetText(), std::string("Helloworld!"), TEST_LOCATION );

  view.ReplaceTextFromTo( 0, 11, styledText );

  DALI_TEST_EQUALS( view.GetText(), std::string("Hello "), TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewSnapshotEnable(void)
{
  tet_infoline("UtcDaliTextViewSnapshotEnable: ");
  ToolkitTestApplication application;

  // Avoids the frame buffer texture to throw an exception.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  TextView view = TextView::New();
  view.SetMarkupProcessingEnabled( true ); // Enables markup processing.
  view.SetText( "Hel<font color='green'>lo world!</font> This <font color='green'>is</font> a sna<font color='green'>psho</font>t test." );

  Stage::GetCurrent().Add( view );

  view.SetSnapshotModeEnabled( true );  // VCC. By default the snapshot mode should be enabled but it has been temporary disabled.
                                        // This line should be removed when text-view is set to use the snapshot mode by default.

  // Snapshot is enabled by default.
  DALI_TEST_CHECK( view.IsSnapshotModeEnabled() );

  application.SendNotification();
  application.Render();

  // TextView should have only two actors:
  // the root (Actor) and the image (ImageActor).

  DALI_TEST_EQUALS( view.GetChildCount(), 2u, TEST_LOCATION );

  view.SetSnapshotModeEnabled( false );
  DALI_TEST_CHECK( !view.IsSnapshotModeEnabled() );

  application.SendNotification();
  application.Render();

  // TextView should have one text-actor per word.

  DALI_TEST_EQUALS( view.GetChildCount(), 7u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewScroll(void)
{
  tet_infoline("UtcDaliTextViewScroll: ");
  ToolkitTestApplication application;

  // Avoids the frame buffer texture to throw an exception.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );

  TextView view = TextView::New( "Hello world! This is a scroll test." );
  view.SetSize( 100.f, 100.f );
  view.SetSnapshotModeEnabled( false );

  Stage::GetCurrent().Add( view );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !view.IsScrollEnabled() ); // Scroll should be disabled by default.

  view.SetScrollEnabled( true );
  view.ScrolledSignal().Connect( &TestTextScrolled );

  DALI_TEST_CHECK( view.IsScrollEnabled() );
  DALI_TEST_CHECK( view.IsSnapshotModeEnabled() ); // Scroll should enable snapshot mode.

  gTextScrolled = false;
  gScrollDelta = Vector2::ZERO;
  view.SetScrollPosition( Vector2( 400.f, 400.f ) );

  application.SendNotification();
  application.Render();

  const Vector2& scrollPosition = view.GetScrollPosition();
  DALI_TEST_EQUALS( scrollPosition, Vector2( 149.153656f, 0.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( gTextScrolled );
  DALI_TEST_EQUALS( gScrollDelta, Vector2( 149.153656f, 0.f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_CHECK( view.IsScrollPositionTrimmed() );
  END_TEST;
}

int UtcDaliTextViewSetProperty(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetText: ");
  ToolkitTestApplication application;

  TextView view = TextView::New( "Hello world!" );
  Stage::GetCurrent().Add( view );

  //Test multiline policy property
  view.SetProperty(view.GetPropertyIndex(PROPERTY_MULTILINE_POLICY), "SplitByNewLineChar");
  DALI_TEST_CHECK( Toolkit::TextView::SplitByNewLineChar == view.GetMultilinePolicy() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_MULTILINE_POLICY), "SplitByWord");
  DALI_TEST_CHECK( Toolkit::TextView::SplitByWord == view.GetMultilinePolicy() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_MULTILINE_POLICY), "SplitByChar");
  DALI_TEST_CHECK( Toolkit::TextView::SplitByChar == view.GetMultilinePolicy() );

  //Test width exceed policy property
  view.SetProperty(view.GetPropertyIndex(PROPERTY_WIDTH_EXCEED_POLICY), "Original");
  view.SetProperty(view.GetPropertyIndex(PROPERTY_HEIGHT_EXCEED_POLICY), "Original");
  DALI_TEST_CHECK( Toolkit::TextView::Original == view.GetWidthExceedPolicy() );
  DALI_TEST_CHECK( Toolkit::TextView::Original == view.GetHeightExceedPolicy() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_WIDTH_EXCEED_POLICY), "Fade");
  view.SetProperty(view.GetPropertyIndex(PROPERTY_HEIGHT_EXCEED_POLICY), "Fade");
  DALI_TEST_CHECK( Toolkit::TextView::Fade == view.GetWidthExceedPolicy() );
  DALI_TEST_CHECK( Toolkit::TextView::Fade == view.GetHeightExceedPolicy() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_WIDTH_EXCEED_POLICY), "ShrinkToFit");
  view.SetProperty(view.GetPropertyIndex(PROPERTY_HEIGHT_EXCEED_POLICY), "ShrinkToFit");
  DALI_TEST_CHECK( Toolkit::TextView::ShrinkToFit == view.GetWidthExceedPolicy() );
  DALI_TEST_CHECK( Toolkit::TextView::ShrinkToFit == view.GetHeightExceedPolicy() );

  //Test line justification property
  view.SetProperty(view.GetPropertyIndex(PROPERTY_LINE_JUSTIFICATION), "Left");
  DALI_TEST_CHECK( Toolkit::TextView::Left == view.GetLineJustification() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_LINE_JUSTIFICATION), "Center");
  DALI_TEST_CHECK( Toolkit::TextView::Center == view.GetLineJustification() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_LINE_JUSTIFICATION), "Right");
  DALI_TEST_CHECK( Toolkit::TextView::Right == view.GetLineJustification() );

  view.SetProperty(view.GetPropertyIndex(PROPERTY_LINE_JUSTIFICATION), "Justified");
  DALI_TEST_CHECK( Toolkit::TextView::Justified == view.GetLineJustification() );

  //Test fade boundary property
  unsigned int testValue = 23;
  PixelSize leftFadeBoundary(testValue);
  view.SetProperty(view.GetPropertyIndex(PROPERTY_FADE_BOUNDARY_LEFT), testValue);
  DALI_TEST_CHECK( leftFadeBoundary == view.GetFadeBoundary().mLeft );

  testValue = 26;
  PixelSize rightFadeBoundary(testValue);
  view.SetProperty(view.GetPropertyIndex(PROPERTY_FADE_BOUNDARY_RIGHT), testValue);
  DALI_TEST_CHECK( rightFadeBoundary == view.GetFadeBoundary().mRight );

  testValue = 2;
  PixelSize topFadeBoundary(testValue);
  view.SetProperty(view.GetPropertyIndex(PROPERTY_FADE_BOUNDARY_TOP), testValue);
  DALI_TEST_CHECK( topFadeBoundary == view.GetFadeBoundary().mTop );

  testValue = 11;
  PixelSize bottomFadeBoundary(testValue);
  view.SetProperty(view.GetPropertyIndex(PROPERTY_FADE_BOUNDARY_BOTTOM), testValue);
  DALI_TEST_CHECK( bottomFadeBoundary == view.GetFadeBoundary().mBottom );

  //Test Line height offset property
  float testOffsetValue = 14.04f;
  view.SetProperty(view.GetPropertyIndex(PROPERTY_LINE_HEIGHT_OFFSET), testOffsetValue);
  DALI_TEST_CHECK( PointSize(testOffsetValue) == view.GetLineHeightOffset() );

  //Test alignment property
  view.SetProperty(view.GetPropertyIndex(PROPERTY_HORIZONTAL_ALIGNMENT), "HorizontalLeft");
  view.SetProperty(view.GetPropertyIndex(PROPERTY_VERTICAL_ALIGNMENT), "VerticalTop");
  DALI_TEST_CHECK( (Toolkit::Alignment::HorizontalLeft | Toolkit::Alignment::VerticalTop) == view.GetTextAlignment() );
  END_TEST;
}

int UtcDaliTextViewSetSortModifier(void)
{
  tet_infoline("UtcDaliTextViewSetAndGetText: ");
  ToolkitTestApplication application;

  TextView view = TextView::New( "Hello world!" );
  Stage::GetCurrent().Add( view );

  view.SetSortModifier( 10.f );
  view.SetSnapshotModeEnabled( false );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( RenderableActor::DownCast(view.GetChildAt(0)).GetSortModifier(), 10.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTextViewUnderlineText(void)
{
  tet_infoline("UtcDaliTextViewUnderlineText: ");
  ToolkitTestApplication application;

  TextView textView = TextView::New();
  textView.SetSnapshotModeEnabled( false );
  textView.SetMarkupProcessingEnabled( true );
  textView.SetText( "<u><font size='10'>gg<font size='14'>gg<font size='18'>gg<font size='22'>gg</font>gg</font>gg</font>gg</font></u>" );

  textView.SetSize( 150.f, 100.f );

  Stage::GetCurrent().Add( textView );

  application.SendNotification();
  application.Render();

  std::vector<float> positions;
  positions.push_back( 6.448784f );
  positions.push_back( 9.862847f );
  positions.push_back( 13.276909f );
  positions.push_back( 16.690973f );
  positions.push_back( 13.276909f );
  positions.push_back( 9.862847f );
  positions.push_back( 6.448784f );

  for( std::size_t index = 0, num = textView.GetChildCount(); index < num; ++index )
  {
    TextStyle style = TextActor::DownCast( textView.GetChildAt(index) ).GetTextStyle();

    DALI_TEST_EQUALS( 4.17274f, style.GetUnderlineThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
    DALI_TEST_EQUALS( positions[index], style.GetUnderlinePosition(), Math::MACHINE_EPSILON_1000, TEST_LOCATION );
  }
  END_TEST;
}
