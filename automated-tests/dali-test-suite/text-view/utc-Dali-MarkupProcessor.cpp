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
#include <dali-toolkit/public-api/markup-processor/markup-processor.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;

namespace
{

struct MarkupStringTest
{
  std::string input;
  std::string expectedResult;
};

bool TestMarkupString( const std::string& input, const std::string& expectedResult, std::string& result )
{
  Toolkit::MarkupProcessor::StyledTextArray styledTextArray;

  GetStyledTextArray( input, styledTextArray );
  GetMarkupString( styledTextArray, result );

  return expectedResult == result;
}

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
TEST_FUNCTION( UtcDaliMarkupProcessor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMarkupProcessorSetTextStyle01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMarkupProcessorSetTextStyle02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMarkupProcessorTestColors, POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for a method
static void UtcDaliMarkupProcessor()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliMarkupProcessor ");

  const std::string text1( "Text" );
  const std::string text2( "< font  face ='FreeSerif'  color= 'green' >t< / font >" );
  const std::string text3( "<  font face =  'FreeSerif' size=  '16' style = 'Bold' color='red'>< i><u >Styled< / u> Text< /i >< / font >< br / >" );
  const std::string text4( "<font face='FreeSerif' size='14' color='0xaadd8744'><b><u>Styled</u> Te<font size='20'>x</font>t</b></font>< br/>" );
  const std::string text5( "< shadow color   =   'blue' paramx =   '1' paramy = '0.75'  >Shadow< / shadow><br />" );
  const std::string text6( "<smooth     param=  '0.75'  >< glow   color =   'red' param  = '0.1'  >Glow</glow></smooth>< br />" );
  const std::string text7( "<font color='green''><   outline color = 'red'   paramx  =  '0.7' paramy  =  '0.7' >Outline< / outline  >< /font  ><  br  /  >" );
  const std::string text8( "<smooth param='0.75'>Smooth</smooth><  br /   >" );
  const std::string text9( "\\<" );
  const std::string text10( "\\>" );

  char crlf[2];
  crlf[0] = 0x0D;
  crlf[1] = 0x0A;
  const std::string text11( crlf, 2 );

  const std::string result1( text1 );
  const std::string result2( "<font face='FreeSerif' color='green'>t</font>" );
  const std::string result3( "<font face='FreeSerif' style='Bold' size='16' color='red'><i><u>Styled</u></i></font><font face='FreeSerif' style='Bold' size='16' color='red'><i> Text</i></font><br />" );
  const std::string result4( "<font face='FreeSerif' size='14' color='0xaadd8744'><b><u>Styled</u></b></font><font face='FreeSerif' size='14' color='0xaadd8744'><b> Te</b></font><font face='FreeSerif' size='20' color='0xaadd8744'><b>x</b></font><font face='FreeSerif' size='14' color='0xaadd8744'><b>t</b></font><br />" );
  const std::string result5( "<shadow color='blue' paramx='1' paramy='0.75'>Shadow</shadow><br />" );
  const std::string result6( "<smooth param='0.75'><glow color='red' param='0.1'>Glow</glow></smooth><br />" );
  const std::string result7( "<font color='green'><outline color='red' paramx='0.7' paramy='0.7'>Outline</outline></font><br />" );
  const std::string result8( "<smooth param='0.75'>Smooth</smooth><br />" );
  const std::string result9( text9 );
  const std::string result10( text10 );
  const std::string result11( "<br />" );

  std::string markupString;
  Toolkit::MarkupProcessor::StyledTextArray styledTextArray;

  GetStyledTextArray( text1, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result1, markupString, TEST_LOCATION );

  GetStyledTextArray( text2, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result2, markupString, TEST_LOCATION );

  GetStyledTextArray( text3, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result3, markupString, TEST_LOCATION );

  GetStyledTextArray( text4, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result4, markupString, TEST_LOCATION );

  GetStyledTextArray( text5, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result5, markupString, TEST_LOCATION );

  GetStyledTextArray( text6, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result6, markupString, TEST_LOCATION );

  GetStyledTextArray( text7, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result7, markupString, TEST_LOCATION );

  GetStyledTextArray( text8, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result8, markupString, TEST_LOCATION );

  GetStyledTextArray( text9, styledTextArray );
  GetMarkupString( styledTextArray, markupString );
  DALI_TEST_EQUALS( result9, markupString, TEST_LOCATION );

  GetStyledTextArray( text10, styledTextArray );
  GetMarkupString( styledTextArray, markupString );

  DALI_TEST_EQUALS( result10, markupString, TEST_LOCATION );

  GetStyledTextArray( text11, styledTextArray );
  GetMarkupString( styledTextArray, markupString );

  DALI_TEST_EQUALS( result11, markupString, TEST_LOCATION );
}

static void UtcDaliMarkupProcessorSetTextStyle01()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliMarkupProcessorSetTextStyle01 ");

  const std::string text1( "Text with no defined style" );
  const std::string result1( "<font color='green'><i>Text with no defined style</i></font>" );
  const std::string result2( "Text with <font color='green'><i>no defined</i></font> style" );

  std::string markupString;
  Toolkit::MarkupProcessor::StyledTextArray styledTextArray;

  GetStyledTextArray( text1, styledTextArray );

  TextStyle style;
  style.SetItalics( true );
  style.SetTextColor( Color::GREEN );

  SetTextStyle( styledTextArray, style );
  GetMarkupString( styledTextArray, markupString );

  DALI_TEST_EQUALS( result1, markupString, TEST_LOCATION );

  styledTextArray.clear();
  SetTextStyle( text1, styledTextArray, style );
  GetMarkupString( styledTextArray, markupString );

  DALI_TEST_EQUALS( result1, markupString, TEST_LOCATION );

  GetStyledTextArray( text1, styledTextArray );
  SetTextStyleToRange( styledTextArray, style, TextStyle::ALL, 0, text1.size() - 1 );
  GetMarkupString( styledTextArray, markupString );

  DALI_TEST_EQUALS( result1, markupString, TEST_LOCATION );

  GetStyledTextArray( text1, styledTextArray );
  SetTextStyleToRange( styledTextArray, style, TextStyle::ALL, 10, 19 );
  GetMarkupString( styledTextArray, markupString );

  DALI_TEST_EQUALS( result2, markupString, TEST_LOCATION );

  std::string plainString;
  GetPlainString( styledTextArray, plainString );

  DALI_TEST_EQUALS( text1, plainString, TEST_LOCATION );
}

static void UtcDaliMarkupProcessorSetTextStyle02()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliMarkupProcessorSetTextStyle02 ");

  Toolkit::MarkupProcessor::StyledTextArray styledTextArray;

  // Test style applied to and empty string doesn't crash

  TextStyle style;
  style.SetItalics( true );
  style.SetTextColor( Color::GREEN );

  bool fails = false;
  try
  {
    SetTextStyle( styledTextArray, style );
  }
  catch( ... )
  {
    fails = true;
  }

  DALI_TEST_CHECK( !fails );
}

static void UtcDaliMarkupProcessorTestColors()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliMarkupProcessorTestColors  ");

  struct MarkupStringTest colorTests[] =
  {
    {
      std::string( "<font color='0xFF000000'>black</font>" ),
      std::string( "<font color='black'>black</font>" )
    },
    {
      std::string( "<font color='0xFFFFFFFF'>white</font>" ),
      std::string( "white" )
    },
    {
      std::string( "<font color='0xFFFF0000'>red</font>" ),
      std::string( "<font color='red'>red</font>" )
    },
    {
      std::string( "<font color='0xFF00FF00'>green</font>" ),
      std::string( "<font color='green'>green</font>" )
    },
    {
      std::string( "<font color='0xFF0000FF'>blue</font>" ),
      std::string( "<font color='blue'>blue</font>" )
    },
    {
      std::string( "<font color='0xFFFFFF00'>yellow</font>" ),
      std::string( "<font color='yellow'>yellow</font>" )
    },
    {
      std::string( "<font color='0xFFFF00FF'>magenta</font>" ),
      std::string( "<font color='magenta'>magenta</font>" )
    },
    {
      std::string( "<font color='0xFF00FFFF'>cyan</font>" ),
      std::string( "<font color='cyan'>cyan</font>" )
    },
    {
      std::string( "<font color='0x00000000'>transparent</font>" ),
      std::string( "<font color='transparent'>transparent</font>" )
    },
    {
      std::string( "<font color='#000000'>black</font>" ),
      std::string( "<font color='black'>black</font>" )
    },
    {
      std::string( "<font color='#FFFFFF'>white</font>" ),
      std::string( "white" )
    },
    {
      std::string( "<font color='#FF0000'>red</font>" ),
      std::string( "<font color='red'>red</font>" )
    },
    {
      std::string( "<font color='#00FF00'>green</font>" ),
      std::string( "<font color='green'>green</font>" )
    },
    {
      std::string( "<font color='#0000FF'>blue</font>" ),
      std::string( "<font color='blue'>blue</font>" )
    },
    {
      std::string( "<font color='#FFFF00'>yellow</font>" ),
      std::string( "<font color='yellow'>yellow</font>" )
    },
    {
      std::string( "<font color='#FF00FF'>magenta</font>" ),
      std::string( "<font color='magenta'>magenta</font>" )
    },
    {
      std::string( "<font color='#00FFFF'>cyan</font>" ),
      std::string( "<font color='cyan'>cyan</font>" )
    },
    {
      std::string( "<font color='#000'>black</font>" ),
      std::string( "<font color='black'>black</font>" )
    },
    {
      std::string( "<font color='#FFF'>white</font>" ),
      std::string( "white" )
    },
    {
      std::string( "<font color='#F00'>red</font>" ),
      std::string( "<font color='red'>red</font>" )
    },
    {
      std::string( "<font color='#0F0'>green</font>" ),
      std::string( "<font color='green'>green</font>" )
    },
    {
      std::string( "<font color='#00F'>blue</font>" ),
      std::string( "<font color='blue'>blue</font>" )
    },
    {
      std::string( "<font color='#FF0'>yellow</font>" ),
      std::string( "<font color='yellow'>yellow</font>" )
    },
    {
      std::string( "<font color='#F0F'>magenta</font>" ),
      std::string( "<font color='magenta'>magenta</font>" )
    },
    {
      std::string( "<font color='#0FF'>cyan</font>" ),
      std::string( "<font color='cyan'>cyan</font>" )
    },
    {
      std::string( "<font color='0x000000'>black</font>" ),
      std::string( "<font color='black'>black</font>" )
    },
    {
      std::string( "<font color='black'>black</font>" ),
      std::string( "<font color='black'>black</font>" )
    },
    {
      std::string( "<font color='white'>white</font>" ),
      std::string( "white" )
    },
    {
      std::string( "<font color='red'>red</font>" ),
      std::string( "<font color='red'>red</font>" )
    },
    {
      std::string( "<font color='0xFF00FF00'>green</font>" ),
      std::string( "<font color='green'>green</font>" )
    },
    {
      std::string( "<font color='blue'>blue</font>" ),
      std::string( "<font color='blue'>blue</font>" )
    },
    {
      std::string( "<font color='yellow'>yellow</font>" ),
      std::string( "<font color='yellow'>yellow</font>" )
    },
    {
      std::string( "<font color='magenta'>magenta</font>" ),
      std::string( "<font color='magenta'>magenta</font>" )
    },
    {
      std::string( "<font color='cyan'>cyan</font>" ),
      std::string( "<font color='cyan'>cyan</font>" )
    },
    {
      std::string( "<font color='transparent'>transparent</font>" ),
      std::string( "<font color='transparent'>transparent</font>" )
    },
    {
      std::string( "<outline color='white'>outline</outline>" ),
      std::string( "<outline color='white'>outline</outline>" )
    },
  };

  const std::size_t numberOfTests( 36 );

  bool fails = false;
  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const MarkupStringTest& test = colorTests[index];

    std::string result;
    if( !TestMarkupString( test.input, test.expectedResult, result ) )
    {
      TestMarkupString( test.input, test.expectedResult, result );
      tet_printf( "%s\n          input : %s\nexpected result : %s\n         result : %s\n", TEST_LOCATION, test.input.c_str(), test.expectedResult.c_str(), result.c_str() );

      fails = true;
    }
  }

  DALI_TEST_CHECK( !fails );
}
