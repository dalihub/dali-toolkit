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
#include <dali-toolkit/internal/controls/text-view/text-processor.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Internal;

void dali_text_view_processor_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_text_view_processor_cleanup(void)
{
  test_return_value = TET_PASS;
}


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

struct SplitInParagraphsTest
{
  std::string inputText;

  std::size_t resultNumberOfParagraphs;
};

bool TestSplitInParagraphs( const SplitInParagraphsTest& test, const char* location )
{
  // Creates a styled text with the markup or plain string.
  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( test.inputText, styledText, true );

  std::vector<MarkupProcessor::StyledTextArray> paragraphs;

  TextProcessor::SplitInParagraphs( styledText,
                                    paragraphs );

  if( paragraphs.size() != test.resultNumberOfParagraphs )
  {
    tet_printf( "Fail. %s", location );
    tet_printf( "Different number of paragraphs, result %d, expected result %d", paragraphs.size(), test.resultNumberOfParagraphs );

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


int UtcDaliTextViewSplitInParagraphs(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitInParagraphs : ");

  struct SplitInParagraphsTest splitInParagraphsTest[] =
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
    const SplitInParagraphsTest& test = splitInParagraphsTest[index];

    if( !TestSplitInParagraphs( test, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewSplitInWords(void)
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
  END_TEST;
}

int UtcDaliTextViewBeginsRightToLeftCharacter(void)
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
  END_TEST;
}

int UtcDaliTextViewContainsRightToLeftCharacter(void)
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
  END_TEST;
}

int UtcDaliTextViewFindNearestWord(void)
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
  END_TEST;
}
