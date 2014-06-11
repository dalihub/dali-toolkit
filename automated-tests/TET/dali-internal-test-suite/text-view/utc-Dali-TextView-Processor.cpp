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
#include <dali-toolkit/internal/controls/text-view/text-processor.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Internal;

namespace
{
// Data structures used to create an 'experiment' in TET cases

//////////////////////////////////////////////////////////////////

struct BeginsRightToLeftCharacterTest
{
  std::string description;
  std::string input;
  bool result;
};

bool TestBeginsRightToLeftCharacter( const std::string& description, const std::string& input, const bool result, const char* location )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( input, styledText, true );

  const bool ret = ( result == TextProcessor::BeginsRightToLeftCharacter( styledText ) );

  if( !ret )
  {
    tet_printf( "Fail. %s", location );
    tet_printf( "Input : %s", input.c_str() );
  }

  return ret;
}

//////////////////////////////////////////////////////////////////

struct ContainsRightToLeftCharacterTest
{
  std::string description;
  std::string input;
  bool result;
};

bool TestContainsRightToLeftCharacter( const std::string& description, const std::string& input, const bool result, const char* location )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( input, styledText, true );

  const bool ret = ( result == TextProcessor::ContainsRightToLeftCharacter( styledText ) );

  if( !ret )
  {
    tet_printf( "Fail. %s", location );
    tet_printf( "Input : %s", input.c_str() );
  }

  return ret;
}

//////////////////////////////////////////////////////////////////

struct FindNearestWordTest
{
  std::string description;
  std::string input;
  std::size_t offset;
  std::size_t start;
  std::size_t end;
};

bool TestFindNearestWord( const std::string& description, const std::string& input, const std::size_t offset, const std::size_t startResult, const std::size_t endResult, const char* location )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( input, styledText, true );

  std::size_t start;
  std::size_t end;
  TextProcessor::FindNearestWord( styledText, offset, start, end );

  const bool ret = ( start == startResult ) && ( end == endResult );

  if( !ret )
  {
    tet_printf( "Fail. %s", location );
    tet_printf( "Input : %s, offset %d, start %d, end %d", input.c_str(), offset, start, end );
  }

  return ret;
}

//////////////////////////////////////////////////////////////////

struct SplitInLinesTest
{
  std::string inputText;

  std::size_t resultNumberOfLines;
};

bool TestSplitInLines( const SplitInLinesTest& test, const char* location )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( test.inputText, styledText, true );

  std::vector<MarkupProcessor::StyledTextArray> lines;

  TextProcessor::SplitInLines( styledText,
                               lines );

  if( lines.size() != test.resultNumberOfLines )
  {
    tet_printf( "Fail. %s", location );
    tet_printf( "Different number of lines, result %d, expected result %d", lines.size(), test.resultNumberOfLines );

    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////

struct SplitInWordsTest
{
  std::string inputText;

  std::size_t resultNumberOfWords;
};

bool TestSplitInWords( const SplitInWordsTest& test, const char* location )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( test.inputText, styledText, true );

  std::vector<MarkupProcessor::StyledTextArray> words;

  TextProcessor::SplitInWords( styledText,
                               words );

  if( words.size() != test.resultNumberOfWords )
  {
    tet_printf( "Fail. %s", location );
    tet_printf( "Different number of words, result %d, expected result %d", words.size(), test.resultNumberOfWords );

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
TEST_FUNCTION( UtcDaliTextViewSplitInLines, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextViewSplitInWords, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextViewBeginsRightToLeftCharacter, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextViewContainsRightToLeftCharacter, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTextViewFindNearestWord, POSITIVE_TC_IDX );
// TEST_FUNCTION( , POSITIVE_TC_IDX );
// TEST_FUNCTION( , NEGATIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliTextViewSplitInLines()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitInLines : ");

  struct SplitInLinesTest splitInLinesTest[] =
  {
    {
      std::string( "Hello world\nhello world." ),
      2
    },
    {
      std::string( "Hello world\nhello world.\n\n" ),
      4
    }
  };
  const std::size_t numberOfTests( 2 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const SplitInLinesTest& test = splitInLinesTest[index];

    if( !TestSplitInLines( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewSplitInWords()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitInWords : ");

  struct SplitInWordsTest splitInWordsTest[] =
  {
    {
      std::string( "Hello world, hello word!" ),
      7
    },
  };
  const std::size_t numberOfTests( 1 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const SplitInWordsTest& test = splitInWordsTest[index];

    if( !TestSplitInWords( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewBeginsRightToLeftCharacter()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewBeginsRightToLeftCharacter : ");

  struct BeginsRightToLeftCharacterTest beginsRightToLeftCharacterTest[] =
  {
    {
      std::string( "Test if it begins with a right to left character. Should return false." ),
      std::string( "Hello world مرحبا العالم." ),
      false
    },
    {
      std::string( "Test if it begins with a right to left character. Should return true." ),
      std::string( "مرحبا العالم Hola mundo." ),
      true
    }
  };
  const std::size_t numberOfTests( 2 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const BeginsRightToLeftCharacterTest& test = beginsRightToLeftCharacterTest[index];

    if( !TestBeginsRightToLeftCharacter( test.description, test.input, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewContainsRightToLeftCharacter()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewContainsRightToLeftCharacter : ");

  struct ContainsRightToLeftCharacterTest containsRightToLeftCharacterTest[] =
  {
    {
      std::string( "Test if it contains a right to left character. Should return true." ),
      std::string( "Hello world مرحبا العالم." ),
      true
    },
    {
      std::string( "Test if it contains a right to left character. Should return true." ),
      std::string( "مرحبا العالم Hola mundo." ),
      true
    },
    {
      std::string( "Test if it contains a right to left character. Should return false." ),
      std::string( "Hello world." ),
      false
    },
    {
      std::string( "Test if it contains a right to left character. Should return true." ),
      std::string( "مرحبا العالم." ),
      true
    }
  };
  const std::size_t numberOfTests( 4 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const ContainsRightToLeftCharacterTest& test = containsRightToLeftCharacterTest[index];

    if( !TestContainsRightToLeftCharacter( test.description, test.input, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}

static void UtcDaliTextViewFindNearestWord()
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewFindNearestWord : ");

  struct FindNearestWordTest findNearestWordTest[] =
  {
    {
      std::string( "" ),
      std::string( "Hello world, hola mundo" ),
      0u,
      0u,
      5u
    },
    {
      std::string( "" ),
      std::string( "Hello world, hola mundo" ),
      7u,
      6u,
      12u
    },
    {
      std::string( "" ),
      std::string( "Hello world, hola mundo" ),
      11u,
      6u,
      12u
    },
    {
      std::string( "" ),
      std::string( "Hello world, hola mundo" ),
      23u,
      18u,
      23u
    },
    {
      std::string( "" ),
      std::string( "Hello world, hola mundo" ),
      5u,
      0u,
      5u
    },
    {
      std::string( "" ),
      std::string( "Hello world, hola mundo  مرحبا العالم" ),
      24u,
      25u,
      30u
    }
  };

  const std::size_t numberOfTests( 6 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const FindNearestWordTest& test = findNearestWordTest[index];

    if( !TestFindNearestWord( test.description, test.input, test.offset, test.start, test.end, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
}
