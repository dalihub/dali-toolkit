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
#include <dali-toolkit/internal/controls/text-view/split-by-new-line-char-policies.h>
#include <dali-toolkit/internal/controls/text-view/text-view-impl.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-helper-functions.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-dbg.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Internal;

void dali_text_view_helper_and_debug_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_text_view_helper_and_debug_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{
// Data structures used to create an 'experiment' in TET cases

const Toolkit::Internal::TextView::LayoutParameters DEFAULT_LAYOUT_PARAMETERS;
const Toolkit::Internal::TextView::VisualParameters DEFAULT_VISUAL_PARAMETERS;

struct GetIndicesFromGlobalCharacterIndexTest
{
  std::string description;
  std::string input;
  std::size_t position;
  std::size_t paragraphIndex;
  std::size_t wordIndex;
  std::size_t characterIndex;
};

/**
 * Gets the paragraph, word, and character indices for a given text and a given position and checks the results with the given indices.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param input The input text.
 * @param position Global position of the character. i.e in a text with with 1000 characters, position could be any value from 0 to 1000.
 * @param resultParagraphIndex Index to the paragraph where the character is located.
 * @param resultWordIndex Index to the word within the paragraph where the character is located.
 * @param resultCharacterIndex Index to the character within the word where the character is located.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestGetIndicesFromGlobalCharacterIndex( const std::string& description,
                                             const std::string& input,
                                             const std::size_t position,
                                             const std::size_t resultParagraphIndex,
                                             const std::size_t resultWordIndex,
                                             const std::size_t resultCharacterIndex,
                                             const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create natural size, layout and text-actor info for the input word.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  TextViewProcessor::TextInfoIndices indices;
  TextViewProcessor::GetIndicesFromGlobalCharacterIndex( position,
                                                         inputLayout,
                                                         indices );

  if( indices.mParagraphIndex != resultParagraphIndex )
  {
    tet_printf( "Fail. different paragraph index. %s", location );
    return false;
  }
  if( indices.mWordIndex != resultWordIndex )
  {
    tet_printf( "Fail. different word index. %s", location );
    return false;
  }
  if( indices.mCharacterIndex != resultCharacterIndex )
  {
    tet_printf( "Fail. different character index. %s", location );
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////
} // namespace


int UtcDaliTextViewGetIndicesFromGlobalCharacterIndex(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewGetIndicesFromGlobalCharacterIndex : ");
  struct GetIndicesFromGlobalCharacterIndexTest getIndicesFromGlobalCharacterIndexTests[] =
  {
    {
      std::string( "Test position 0" ),
      std::string( "text te<font size='30'>xt text te</font>xt text\n"
                   "text t<font size='30'>ext טקסט טקסט te</font>xt\n"
                   "text text text text text\n"
                   "\n" ),
      0,
      0,
      0,
      0
    },
    {
      std::string( "Test position 76. (just after the last \\n)" ),
      std::string( "t<font size='30'>ext text te</font>xt text text\n"
                   "text text טקסט טקסט text\n"
                   "text text te<font size='30'>xt text</font> text\n"
                   "\n" ),
      76,
      4,
      0,
      0
    },
    {
      std::string( "Test position 73. (the last \\n)" ),
      std::string( "text te<font size='30'>xt text text </font>text\n"
                   "text text טק<font size='30'>סט טקס</font>ט text\n"
                   "text text text text text\n"
                   "\n" ),
      75,
      3,
      0,
      0
    },
    {
      std::string( "Test position 35. (first hebrew character)" ),
      std::string( "text text text text text\n"
                   "text text טקסט טקסט text\n"
                   "text text text text text\n"
                   "\n" ),
      35,
      1,
      4,
      0
    },
    {
      std::string( "Test position 3. (end of the first word)" ),
      std::string( "text te<font size='30'>xt text text text\n</font>"
                   "text text טק<font size='30'>סט טקסט </font>text\n"
                   "text te<font size='30'>xt text text</font> text\n"
                   "\n" ),
      3,
      0,
      0,
      3
    },
    {
      std::string( "Test position 33. (end of the second word of the second paragraph)" ),
      std::string( "text te<font size='30'>xt text text text\n</font>"
                   "text text טק<font size='30'>סט טקסט </font>text\n"
                   "text te<font size='30'>xt text text</font> text\n"
                   "\n" ),
      33,
      1,
      2,
      3
    },
    {
      std::string( "Test position 43. (last hebrew character)" ),
      std::string( "text te<font size='30'>xt text text text\n</font>"
                   "text text טק<font size='30'>סט טקסט </font>text\n"
                   "text te<font size='30'>xt text text</font> text\n"
                   "\n" ),
      43,
      1,
      6,
      3
    },
  };
  const std::size_t numberOfTests( 7 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const GetIndicesFromGlobalCharacterIndexTest& test = getIndicesFromGlobalCharacterIndexTests[index];

    if( !TestGetIndicesFromGlobalCharacterIndex( test.description, test.input, test.position, test.paragraphIndex, test.wordIndex, test.characterIndex, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewDebugCouts(void)
{
  /////////////////////////////////////////////////////
  // Text debug functions to not to penalize coverage
  /////////////////////////////////////////////////////

  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewDebugCouts : ");

  Toolkit::Internal::TextView::RelayoutData relayoutData;

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( std::string( "Hello world\nhello world" ), inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  Actor dummy = Actor::New();
  Toolkit::Internal::SplitByNewLineChar::Relayout( dummy,
                                                   Toolkit::Internal::TextView::RELAYOUT_ALL,
                                                   DEFAULT_LAYOUT_PARAMETERS,
                                                   DEFAULT_VISUAL_PARAMETERS,
                                                   relayoutData );

  TextViewProcessor::dbgPrint( relayoutData.mTextLayoutInfo );

  TextStyle textStyle;
  TextViewProcessor::dbgPrint( textStyle );

  TextViewProcessor::TextInfoIndices indices;
  TextViewProcessor::dbgPrint( indices );

  TextViewProcessor::dbgPrint( inputStyledText );

  tet_result( TET_PASS );
  END_TEST;
}
