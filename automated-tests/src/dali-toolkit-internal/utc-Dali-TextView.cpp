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
#include <dali-toolkit/internal/controls/text-view/text-view-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-dbg.h>
#include <dali-toolkit/internal/controls/text-view/text-view-paragraph-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-word-processor.h>
#include <dali-toolkit/internal/controls/text-view/relayout-utilities.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Internal;

void dali_text_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_text_view_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

const Toolkit::Internal::TextView::LayoutParameters DEFAULT_LAYOUT_PARAMETERS;
const Toolkit::Internal::TextView::VisualParameters DEFAULT_VISUAL_PARAMETERS;

// Data structures used to create an 'experiment' in TET cases

struct SplitWordTest
{
  std::string description;
  std::string input;
  std::size_t position;
  std::string firstResult;
  std::string lastResult;
};

struct SplitParagraphTest
{
  std::string description;
  std::string input;
  std::size_t wordIndex;
  std::size_t characterIndex;
  float       lineHeightOffset;
  std::string firstResult;
  std::string lastResult;
};

struct MergeWordsTest
{
  std::string description;
  std::string inputFirst;
  std::string inputLast;
  std::string result;
};

struct MergeParagraphsTest
{
  std::string description;
  std::string inputFirst;
  std::string inputLast;
  float       lineHeightOffset;
  std::string result;
};

struct RemoveCharactersFromWordTest
{
  std::string description;
  std::string input;
  std::size_t position;
  std::size_t numberOfCharacters;
  std::string result;
};

struct RemoveWordsFromParagraphTest
{
  std::string description;
  std::string input;
  std::size_t wordIndex;
  std::size_t numberOfWords;
  float       lineHeightOffset;
  std::string result;
};

enum UpdateTextInfoOperation
{
  Insert,
  Remove,
  Replace
};

struct UpdateTextInfoTest
{
  std::string             description;
  UpdateTextInfoOperation operation;
  std::string             input;
  std::size_t             position;
  std::size_t             numberOfCharacters;
  std::string             inputText;
  float                   lineHeightOffset;
  std::string             result;
};

// Useful Print functions when something goes wrong.

void Print( const TextViewProcessor::CharacterLayoutInfo& character )
{
  std::cout << "             height : " << character.mHeight << std::endl;
  std::cout << "            advance : " << character.mAdvance << std::endl;
  std::cout << "            bearing : " << character.mBearing << std::endl;
  std::cout << "          mPosition : " << character.mPosition << std::endl;
  std::cout << "              mSize : " << character.mSize << std::endl;
  std::cout << "          mAscender : " << character.mAscender << std::endl;

  TextActor textActor = TextActor::DownCast( character.mGlyphActor );
  if( textActor )
  {
    std::cout << "[" << textActor.GetText() << "]";
  }
  else
  {
    std::cout << "{" << character.mStyledText.mText.GetText() << "}";
  }
}

void Print( const TextViewProcessor::WordLayoutInfo& word )
{
  std::cout << "[";
  std::cout << "              mSize : " << word.mSize << std::endl;
  std::cout << "          mAscender : " << word.mAscender << std::endl;
  std::cout << "              mType : " << word.mType << std::endl;
  std::cout << "mNumberOfCharacters : " << word.mCharactersLayoutInfo.size() << std::endl;
  std::cout << "[";
  for( TextViewProcessor::CharacterLayoutInfoContainer::const_iterator it = word.mCharactersLayoutInfo.begin(), endIt = word.mCharactersLayoutInfo.end(); it != endIt; ++it )
  {
    Print( *it );
  }
  std::cout << "]"; std::cout << std::endl;
  std::cout << "]"; std::cout << std::endl;
}

void Print( const TextViewProcessor::ParagraphLayoutInfo& paragraph )
{
  std::cout << "<";
  std::cout << "              mSize : " << paragraph.mSize << std::endl;
  std::cout << "          mAscender : " << paragraph.mAscender << std::endl;
  std::cout << "mNumberOfCharacters : " << paragraph.mNumberOfCharacters << std::endl;
  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator it = paragraph.mWordsLayoutInfo.begin(), endIt = paragraph.mWordsLayoutInfo.end(); it != endIt; ++it )
  {
    Print( *it );
  }
  std::cout << ">" << std::endl;
}

void Print( const TextViewProcessor::TextLayoutInfo& text )
{
  std::cout << "||";
  for( TextViewProcessor::ParagraphLayoutInfoContainer::const_iterator it = text.mParagraphsLayoutInfo.begin(), endIt = text.mParagraphsLayoutInfo.end(); it != endIt; ++it )
  {
    Print( *it );
  }
  std::cout << "||" << std::endl;
}

std::string GetText( const TextViewProcessor::CharacterLayoutInfo& character )
{
  return character.mStyledText.mText.GetText();
}

std::string GetText( const TextViewProcessor::WordLayoutInfo& word )
{
  std::string text;

  for( TextViewProcessor::CharacterLayoutInfoContainer::const_iterator it = word.mCharactersLayoutInfo.begin(), endIt = word.mCharactersLayoutInfo.end(); it != endIt; ++it )
  {
    text += GetText( *it );
  }

  return text;
}

std::string GetText( const TextViewProcessor::ParagraphLayoutInfo& paragraph )
{
  std::string text;

  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator it = paragraph.mWordsLayoutInfo.begin(), endIt = paragraph.mWordsLayoutInfo.end(); it != endIt; ++it )
  {
    text += GetText( *it );
  }

  return text;
}

// Test functions used to check if two data structures are equal.

bool TestEqual( float x, float y )
{
  return ( fabsf( x - y ) < 0.001f );
}

bool TestEqual( const TextViewProcessor::CharacterLayoutInfo& character1,
                const TextViewProcessor::CharacterLayoutInfo& character2 )
{
  if( !TestEqual( character1.mHeight, character2.mHeight ) )
  {
    return false;
  }
  if( !TestEqual( character1.mAdvance, character2.mAdvance ) )
  {
    return false;
  }
  if( !TestEqual( character1.mBearing, character2.mBearing ) )
  {
    return false;
  }

  if( !TestEqual( character1.mPosition.x, character2.mPosition.x ) )
  {
    return false;
  }
  if( !TestEqual( character1.mPosition.y, character2.mPosition.y ) )
  {
    return false;
  }

  if( !TestEqual( character1.mSize.x, character2.mSize.x ) )
  {
    return false;
  }
  if( !TestEqual( character1.mSize.y, character2.mSize.y ) )
  {
    return false;
  }

  if( !TestEqual( character1.mAscender, character2.mAscender ) )
  {
    return false;
  }

  if( character1.mGlyphActor && !character2.mGlyphActor )
  {
    return false;
  }

  if( !character1.mGlyphActor && character2.mGlyphActor )
  {
    return false;
  }

  std::string text1;
  std::string text2;
  TextStyle style1;
  TextStyle style2;

  TextActor textActor1 = TextActor::DownCast( character1.mGlyphActor );
  TextActor textActor2 = TextActor::DownCast( character2.mGlyphActor );
  if( textActor1 )
  {
    text1 = textActor1.GetText();
    style1 = textActor1.GetTextStyle();

    text2 = textActor2.GetText();
    style2 = textActor2.GetTextStyle();
  }

  if( text1 != text2 )
  {
    return false;
  }

  if( style1 != style2 )
  {
    return false;
  }

  text1 = character1.mStyledText.mText.GetText();
  style1 = character1.mStyledText.mStyle;

  text2 = character2.mStyledText.mText.GetText();
  style2 = character2.mStyledText.mStyle;

  if( text1 != text2 )
  {
    return false;
  }

  if( style1 != style2 )
  {
    std::cout << "  style1 : " << std::endl;
    TextViewProcessor::dbgPrint( style1 );

    std::cout << "  style2 : " << std::endl;
    TextViewProcessor::dbgPrint( style2 );
    return false;
  }

  return true;
}

bool TestEqual( const TextViewProcessor::WordLayoutInfo& word1,
                const TextViewProcessor::WordLayoutInfo& word2 )
{
  if( !TestEqual( word1.mSize.x, word2.mSize.x ) )
  {
    return false;
  }
  if( !TestEqual( word1.mSize.y, word2.mSize.y ) )
  {
    return false;
  }

  if( !TestEqual( word1.mAscender, word2.mAscender ) )
  {
    return false;
  }

  if( word1.mType != word2.mType )
  {
    return false;
  }

  if( word1.mCharactersLayoutInfo.size() != word2.mCharactersLayoutInfo.size() )
  {
    return false;
  }

  for( TextViewProcessor::CharacterLayoutInfoContainer::const_iterator it1 = word1.mCharactersLayoutInfo.begin(), endIt1 = word1.mCharactersLayoutInfo.end(),
         it2 = word2.mCharactersLayoutInfo.begin(), endIt2 = word2.mCharactersLayoutInfo.end();
       ( it1 != endIt1 ) && ( it2 != endIt2 );
       ++it1, ++it2 )
  {
    if( !TestEqual( *it1, *it2 ) )
    {
      return false;
    }
  }

  return true;
}

bool TestEqual( const TextViewProcessor::ParagraphLayoutInfo& paragraph1,
                const TextViewProcessor::ParagraphLayoutInfo& paragraph2 )
{
  if( !TestEqual( paragraph1.mSize.x, paragraph2.mSize.x ) )
  {
    return false;
  }
  if( !TestEqual( paragraph1.mSize.y, paragraph2.mSize.y ) )
  {
    return false;
  }

  if( !TestEqual( paragraph1.mAscender, paragraph2.mAscender ) )
  {
    return false;
  }

  if( paragraph1.mNumberOfCharacters != paragraph2.mNumberOfCharacters )
  {
    return false;
  }

  if( paragraph1.mWordsLayoutInfo.size() != paragraph2.mWordsLayoutInfo.size() )
  {
    return false;
  }

  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator it1 = paragraph1.mWordsLayoutInfo.begin(), endIt1 = paragraph1.mWordsLayoutInfo.end(),
         it2 = paragraph2.mWordsLayoutInfo.begin(), endIt2 = paragraph2.mWordsLayoutInfo.end();
       ( it1 != endIt1 ) && ( it2 != endIt2 );
       ++it1, ++it2 )
  {
    if( !TestEqual( *it1, *it2 ) )
    {
      return false;
    }
  }

  return true;
}

bool TestEqual( const TextViewProcessor::TextLayoutInfo& text1,
                const TextViewProcessor::TextLayoutInfo& text2 )
{
  if( !TestEqual( text1.mWholeTextSize.x, text2.mWholeTextSize.x ) )
  {
    return false;
  }
  if( !TestEqual( text1.mWholeTextSize.y, text2.mWholeTextSize.y ) )
  {
    return false;
  }

  if( !TestEqual( text1.mMaxWordWidth, text2.mMaxWordWidth ) )
  {
    return false;
  }

  if( text1.mNumberOfCharacters != text2.mNumberOfCharacters )
  {
    return false;
  }

  if( text1.mParagraphsLayoutInfo.size() != text2.mParagraphsLayoutInfo.size() )
  {
    return false;
  }

  for( TextViewProcessor::ParagraphLayoutInfoContainer::const_iterator it1 = text1.mParagraphsLayoutInfo.begin(), endIt1 = text1.mParagraphsLayoutInfo.end(),
         it2 = text2.mParagraphsLayoutInfo.begin(), endIt2 = text2.mParagraphsLayoutInfo.end();
       ( it1 != endIt1 ) && ( it2 != endIt2 );
       ++it1, ++it2 )
  {
    if( !TestEqual( *it1, *it2 ) )
    {
      return false;
    }
  }

  return true;
}

/**
 * Splits the \e input word in two by the given \e position and checks the results with \e firstResult and \e lastResult.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Split the word from the beginning. (position 0)".
 * @param input The input word.
 * @param position Where to split the word.
 * @param firstResult First part of the split word.
 * @param lastResult Last part of the split word.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestSplitWord( const std::string& description, const std::string& input, const size_t position, const std::string& firstResult, const std::string& lastResult, const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the input word.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  // Get the input word
  TextViewProcessor::WordLayoutInfo inputWordLayout;

  if( !inputLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *inputLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      inputWordLayout = *( *inputLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  // Create layout info for the first part of the result (after split the word)

  Toolkit::Internal::TextView::RelayoutData firstRelayoutData;
  TextViewProcessor::TextLayoutInfo& firstResultLayout( firstRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray firstResultStyledText;
  MarkupProcessor::GetStyledTextArray( firstResult, firstResultStyledText, true );

  TextViewProcessor::CreateTextInfo( firstResultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     firstRelayoutData );

  // Get the first result word
  TextViewProcessor::WordLayoutInfo firstResultWordLayout;

  if( !firstResultLayout.mParagraphsLayoutInfo.empty() )
  {
   const TextViewProcessor::ParagraphLayoutInfo& paragraph( *firstResultLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      firstResultWordLayout = *( *firstResultLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  // Create layout info for the last part of the result (after split the word)

  Toolkit::Internal::TextView::RelayoutData lastRelayoutData;
  TextViewProcessor::TextLayoutInfo& lastResultLayout( lastRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray lastResultStyledText;
  MarkupProcessor::GetStyledTextArray( lastResult, lastResultStyledText, true );

  TextViewProcessor::CreateTextInfo( lastResultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     lastRelayoutData );

  // Get the last result word
  TextViewProcessor::WordLayoutInfo lastResultWordLayout;

  if( !lastResultLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *lastResultLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      lastResultWordLayout = *( *lastResultLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  // Split the word.

  TextViewProcessor::WordLayoutInfo lastWordLayoutInfo;

  SplitWord( position,
             inputWordLayout,
             lastWordLayoutInfo );

  // Test results
  if( !TestEqual( inputWordLayout, firstResultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );
    return false;
  }

  if( !TestEqual( lastWordLayoutInfo, lastResultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );
    return false;
  }

  return true;
}

/**
 * Splits the \e input paragraph in two by the given \e wordIndex and \e characterIndex and checks the results with \e firstResult and \e lastResult.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Split the paragraph from the beginning. (wordIndex 0 and characterIndex 0)".
 * @param input The input word.
 * @param wordIndex Index to the word within the paragraph where to split it.
 * @param characterIndex Where to split the word.
 * @param lineHeightOffset Offset between lines.
 * @param firstResult First part of the split paragraph.
 * @param lastResult Last part of the split paragraph.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestSplitParagraph( const std::string& description,
                         const std::string& input,
                         size_t wordIndex,
                         size_t characterIndex,
                         float lineHeightOffset,
                         const std::string& firstResult,
                         const std::string& lastResult,
                         const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the input paragraph.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     relayoutData );

  // Get the input paragraph
  TextViewProcessor::ParagraphLayoutInfo inputParagraphLayout;

  if( !inputLayout.mParagraphsLayoutInfo.empty() )
  {
    inputParagraphLayout = *inputLayout.mParagraphsLayoutInfo.begin();
  }

  // Create layout info for the first part of the result (after split the paragraph)

  Toolkit::Internal::TextView::RelayoutData firstRelayoutData;
  TextViewProcessor::TextLayoutInfo& firstResultLayout( firstRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray firstResultStyledText;
  MarkupProcessor::GetStyledTextArray( firstResult, firstResultStyledText, true );

  TextViewProcessor::CreateTextInfo( firstResultStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     firstRelayoutData );

  // Get the first result paragraph
  TextViewProcessor::ParagraphLayoutInfo firstResultParagraphLayout;

  if( !firstResultLayout.mParagraphsLayoutInfo.empty() )
  {
    firstResultParagraphLayout = *firstResultLayout.mParagraphsLayoutInfo.begin();
  }

  // Create layout info for the last part of the result (after split the paragraph)

  Toolkit::Internal::TextView::RelayoutData lastRelayoutData;
  TextViewProcessor::TextLayoutInfo& lastResultLayout( lastRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray lastResultStyledText;
  MarkupProcessor::GetStyledTextArray( lastResult, lastResultStyledText, true );

  TextViewProcessor::CreateTextInfo( lastResultStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..."),
                                                                                    true ),
                                     lastRelayoutData );

  // Get the last result paragraph
  TextViewProcessor::ParagraphLayoutInfo lastResultParagraphLayout;

  if( !lastResultLayout.mParagraphsLayoutInfo.empty() )
  {
    lastResultParagraphLayout = *lastResultLayout.mParagraphsLayoutInfo.begin();
  }

  // Split the paragraph.

  TextViewProcessor::ParagraphLayoutInfo lastParagraphLayoutInfo;

  TextViewProcessor::TextInfoIndices indices( 0, wordIndex, characterIndex );

  SplitParagraph( indices,
                  PointSize( lineHeightOffset ),
                  inputParagraphLayout,
                  lastParagraphLayoutInfo );

  // Test results
  if( !TestEqual( inputParagraphLayout, firstResultParagraphLayout ) )
  {
    tet_printf( "Fail. different first layout info. %s\n", location );
    return false;
  }

  if( !TestEqual( lastParagraphLayoutInfo, lastResultParagraphLayout ) )
  {
    tet_printf( "Fail. different last layout info. %s\n", location );
    return false;
  }

  return true;
}

/**
 * Merges the \e inputFirst word and the \e inputLast word, and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Merge two words with same style".
 * @param inputFirst The first part of the word.
 * @param inputLast The last part of the word.
 * @param result The merged word.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestMergeWords( const std::string& description, const std::string& inputFirst, const std::string& inputLast, const std::string& result, const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the inputFirst word.
  Toolkit::Internal::TextView::RelayoutData firstRelayoutData;
  TextViewProcessor::TextLayoutInfo& inputFirstLayout( firstRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputFirstStyledText;
  MarkupProcessor::GetStyledTextArray( inputFirst, inputFirstStyledText, true );

  TextViewProcessor::CreateTextInfo( inputFirstStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     firstRelayoutData );

  // Get the input word
  TextViewProcessor::WordLayoutInfo inputFirstWordLayout;

  if( !inputFirstLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *inputFirstLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      inputFirstWordLayout = *( *inputFirstLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  // Create layout info for the inputLast word.
  Toolkit::Internal::TextView::RelayoutData lastRelayoutData;
  TextViewProcessor::TextLayoutInfo& inputLastLayout( lastRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputLastStyledText;
  MarkupProcessor::GetStyledTextArray( inputLast, inputLastStyledText, true );

  TextViewProcessor::CreateTextInfo( inputLastStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     lastRelayoutData );

  // Get the input word
  TextViewProcessor::WordLayoutInfo inputLastWordLayout;

  if( !inputLastLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *inputLastLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      inputLastWordLayout = *( *inputLastLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  // Create layout info for the result word.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     resultRelayoutData );

  // Get the result word
  TextViewProcessor::WordLayoutInfo resultWordLayout;

  if( !resultLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *resultLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      resultWordLayout = *( *resultLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  MergeWord( inputFirstWordLayout,
             inputLastWordLayout );

  if( !TestEqual( inputFirstWordLayout, resultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );
    return false;
  }

  return true;
}

/**
 * Merges the \e inputFirst paragraph and the \e inputLast paragraph, and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param inputFirst The first part of the paragraph.
 * @param inputLast The last part of the paragraph.
 * @param lineHeightOffset Offset between lines.
 * @param result The merged paragraph.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestMergeParagraphs( const std::string& description, const std::string& inputFirst, const std::string& inputLast, const float lineHeightOffset, const std::string& result, const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the inputFirst paragraph.
  Toolkit::Internal::TextView::RelayoutData firstRelayoutData;
  TextViewProcessor::TextLayoutInfo& inputFirstLayout( firstRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputFirstStyledText;
  MarkupProcessor::GetStyledTextArray( inputFirst, inputFirstStyledText, true );

  TextViewProcessor::CreateTextInfo( inputFirstStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     firstRelayoutData );

  // Get the input word
  TextViewProcessor::ParagraphLayoutInfo inputFirstParagraphLayout;

  if( !inputFirstLayout.mParagraphsLayoutInfo.empty() )
  {
    inputFirstParagraphLayout = *inputFirstLayout.mParagraphsLayoutInfo.begin();
  }

  // Create layout info for the inputLast paragraph.
  Toolkit::Internal::TextView::RelayoutData lastRelayoutData;
  TextViewProcessor::TextLayoutInfo& inputLastLayout( lastRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputLastStyledText;
  MarkupProcessor::GetStyledTextArray( inputLast, inputLastStyledText, true );

  TextViewProcessor::CreateTextInfo( inputLastStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     lastRelayoutData );

  // Get the input word
  TextViewProcessor::ParagraphLayoutInfo inputLastParagraphLayout;

  if( !inputLastLayout.mParagraphsLayoutInfo.empty() )
  {
    inputLastParagraphLayout = *inputLastLayout.mParagraphsLayoutInfo.begin();
  }

  // Create layout info for the result word.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     resultRelayoutData );

  // Get the result word
  TextViewProcessor::ParagraphLayoutInfo resultParagraphLayout;

  if( !resultLayout.mParagraphsLayoutInfo.empty() )
  {
    resultParagraphLayout = *resultLayout.mParagraphsLayoutInfo.begin();
  }

  MergeParagraph( inputFirstParagraphLayout,
                  inputLastParagraphLayout );

  if( !TestEqual( inputFirstParagraphLayout, resultParagraphLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );
    return false;
  }

  return true;
}

/**
 * Removes from the \e input word the \e numberOfCharacters characters starting from the given \e position and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Remove a whole word. Merge".
 * @param input The input word.
 * @param position Where to start to remove characters
 * @param numberOfCharacters The number of characters to remove.
 * @param result The word without the removed characters.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestRemoveCharactersFromWord( const std::string& description, const std::string& input, const std::size_t position, const std::size_t numberOfCharacters, const std::string& result, const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the input word.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  // Get the input word
  TextViewProcessor::WordLayoutInfo inputWordLayout;

  if( !inputLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *inputLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      inputWordLayout = *( *inputLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  // Create layout info for the result word.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     resultRelayoutData );

  // Get the result word
  TextViewProcessor::WordLayoutInfo resultWordLayout;

  if( !resultLayout.mParagraphsLayoutInfo.empty() )
  {
    const TextViewProcessor::ParagraphLayoutInfo& paragraph( *resultLayout.mParagraphsLayoutInfo.begin() );
    if( !paragraph.mWordsLayoutInfo.empty() )
    {
      resultWordLayout = *( *resultLayout.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
    }
  }

  RemoveCharactersFromWord( position,
                            numberOfCharacters,
                            inputWordLayout );

  if( !TestEqual( inputWordLayout, resultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );
    return false;
  }

  return true;
}

/**
 * Removes from the \e input paragraph the \e numberOfWords words starting from the given \e wordIndex and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param input The input paragraph.
 * @param wordIndex Index within the paragraph where to start to remove words.
 * @param numberOfWords The number of words to remove.
 * @param lineHeightOffset Offset between lines.
 * @param result The paragraph without the removed words.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestRemoveWordsFromParagraph( const std::string& description, const std::string& input, const std::size_t wordIndex, const std::size_t numberOfWords, const float lineHeightOffset, const std::string& result, const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the input paragraph.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     relayoutData );

  // Get the input paragraph
  TextViewProcessor::ParagraphLayoutInfo inputParagraphLayout;

  if( !inputLayout.mParagraphsLayoutInfo.empty() )
  {
    inputParagraphLayout = *inputLayout.mParagraphsLayoutInfo.begin();
  }

  // Create layout info for the result paragraph.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     resultRelayoutData );

  // Get the result paragraph
  TextViewProcessor::ParagraphLayoutInfo resultParagraphLayout;

  if( !resultLayout.mParagraphsLayoutInfo.empty() )
  {
    resultParagraphLayout = *resultLayout.mParagraphsLayoutInfo.begin();
  }

  RemoveWordsFromParagraph( wordIndex,
                            numberOfWords,
                            lineHeightOffset,
                            inputParagraphLayout );

  if( !TestEqual( inputParagraphLayout, resultParagraphLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );
    tet_printf( "            input : [%s]\n", input.c_str() );
    tet_printf( "           result : [%s]\n", GetText( resultParagraphLayout ).c_str() );
    tet_printf( "  expected result : [%s]\n\n", result.c_str() );

    Print(inputParagraphLayout); std::cout << std::endl << std::endl;
    Print(resultParagraphLayout); std::cout << std::endl;
    return false;
  }

  return true;
}

/**
 * Tests inserts, removes and updates operation in the given \e input text and checks with the given \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param operation Type of update operation (insert, remove, replace)
 * @param input The input text.
 * @param position Where to insert, remove or replace text.
 * @param numberOfCharacters Number of characters to remove or replace.
 * @param inputText Inserted or updated text.
 * @param lineHeightOffset Offset between lines.
 * @param result Expected result.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestUpdateTextInfo( const std::string& description,
                         UpdateTextInfoOperation operation,
                         const std::string& input,
                         std::size_t position,
                         std::size_t numberOfCharacters,
                         const std::string& inputText,
                         float lineHeightOffset,
                         const std::string& result,
                         const char* location )
{
  tet_printf( "%s\n", description.c_str() );

  // Create layout info for the input.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     relayoutData );

  // Create layout info for the result.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     Toolkit::Internal::TextView::LayoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                                    Toolkit::TextView::Original,
                                                                                    Toolkit::TextView::Original,
                                                                                    static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                                    Toolkit::TextView::Center,
                                                                                    PointSize( lineHeightOffset ),
                                                                                    std::string( "..." ),
                                                                                    true ),
                                     resultRelayoutData );

  // Choose operation and call appropiate UpdateTextInfo() method.
  const Toolkit::Internal::TextView::LayoutParameters layoutParameters( Toolkit::TextView::SplitByNewLineChar,
                                                                        Toolkit::TextView::Original,
                                                                        Toolkit::TextView::Original,
                                                                        static_cast<Toolkit::Alignment::Type>( Toolkit::Alignment::HorizontalCenter | Toolkit::Alignment::VerticalCenter ),
                                                                        Toolkit::TextView::Center,
                                                                        PointSize( lineHeightOffset ),
                                                                        std::string( "..." ),
                                                                        true );

  switch( operation )
  {
    case Insert:
    {
      MarkupProcessor::StyledTextArray inputStyledText;
      MarkupProcessor::GetStyledTextArray( inputText, inputStyledText, true );

      TextViewProcessor::UpdateTextInfo( position,
                                         inputStyledText,
                                         layoutParameters,
                                         relayoutData );
      break;
    }
    case Remove:
    {
      TextViewProcessor::UpdateTextInfo( position,
                                         numberOfCharacters,
                                         layoutParameters,
                                         relayoutData,
                                         TextViewProcessor::CLEAR_TEXT );
      break;
    }
    case Replace:
    {
      MarkupProcessor::StyledTextArray inputStyledText;
      MarkupProcessor::GetStyledTextArray( inputText, inputStyledText, true );

      TextViewProcessor::UpdateTextInfo( position,
                                         numberOfCharacters,
                                         inputStyledText,
                                         layoutParameters,
                                         relayoutData );
      break;
    }
    default:
    {
      tet_printf( "TestUpdateTextInfo: unknown update operation. %s\n", location );
      return false;
    }
  }

  if( !TestEqual( inputLayout, resultLayout ) )
  {
    tet_printf( "Fail. different layout info. %s\n", location );

    // std::cout << "          result : "; Print( inputLayout );
    // std::cout << " expected result : "; Print( resultLayout );
    return false;
  }

  return true;
}

} // namespace


int UtcDaliTextViewCreateTextInfo(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewCreateTextInfo : ");

  // Metrics for characters

  // Font size = 10
  //     size : [9.48351, 9.48351]
  //  advance : 9.48351
  //  bearing : 8.53516
  // ascender : 8.53516

  // Font size = 12
  //     size : [11.3802, 11.3802]
  //  advance : 11.3802
  //  bearing : 10.2422
  // ascender : 10.2422

  // Font size = 14
  //     size : [13.2769, 13.2769]
  //  advance : 13.2769
  //  bearing : 11.9492
  // ascender : 11.9492

  const float WIDTH_10( 9.48351f );
  const float HEIGHT_10( 9.48351f );
  const float ADVANCE_10( 9.48351f );
  const float BEARING_10( 8.53516f );
  const float ASCENDER_10( 8.53516f );

  const float WIDTH_12( 11.3802f );
  const float HEIGHT_12( 11.3802f );
  const float ADVANCE_12( 11.3802f );
  const float BEARING_12( 10.2422f );
  const float ASCENDER_12( 10.2422f );


  // Generate a text.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& textLayoutInfo( relayoutData.mTextLayoutInfo );

  std::string text( "Hel<font size='10'>lo wo</font>rld!\n"
                    "\n" );

  MarkupProcessor::StyledTextArray styledText;
  MarkupProcessor::GetStyledTextArray( text, styledText, true );

  TextViewProcessor::CreateTextInfo( styledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );


  // Build the text info with metric values.

  // Characters

  TextViewProcessor::CharacterLayoutInfo layoutInfo10; // ( [lo wo])
  layoutInfo10.mHeight = HEIGHT_10;
  layoutInfo10.mAdvance = ADVANCE_10;
  layoutInfo10.mBearing = BEARING_10;
  layoutInfo10.mSize = Size( WIDTH_10, HEIGHT_10 );
  layoutInfo10.mAscender = ASCENDER_10;
  TextViewProcessor::CharacterLayoutInfo layoutInfo12; // ( [Hel], [rld!] and [CR])
  layoutInfo12.mHeight = HEIGHT_12;
  layoutInfo12.mAdvance = ADVANCE_12;
  layoutInfo12.mBearing = BEARING_12;
  layoutInfo12.mSize = Size( WIDTH_12, HEIGHT_12 );
  layoutInfo12.mAscender = ASCENDER_12;

  TextStyle style10;
  style10.SetFontName( "" );
  style10.SetFontPointSize( PointSize( 10.f ) );
  TextStyle style12;
  style12.SetFontName( "" );

  layoutInfo12.mStyledText.mStyle = style12;
  layoutInfo10.mStyledText.mStyle = style10;

  // Words

  TextViewProcessor::WordLayoutInfo wordLayout1, wordLayout2, wordLayout3, wordLayout4;

  // Hello
  wordLayout1.mSize = Size( 3.f * WIDTH_12 + 2.f * WIDTH_10, HEIGHT_12 );
  wordLayout1.mAscender = ASCENDER_12;
  wordLayout1.mType = TextViewProcessor::NoSeparator;

  layoutInfo12.mStyledText.mText = Text( "H" );
  wordLayout1.mCharactersLayoutInfo.push_back( layoutInfo12 ); // H
  layoutInfo12.mStyledText.mText = Text( "e" );
  wordLayout1.mCharactersLayoutInfo.push_back( layoutInfo12 ); // e
  layoutInfo12.mStyledText.mText = Text( "l" );
  wordLayout1.mCharactersLayoutInfo.push_back( layoutInfo12 ); // l
  layoutInfo10.mStyledText.mText = Text( "l" );
  wordLayout1.mCharactersLayoutInfo.push_back( layoutInfo10 ); // l
  layoutInfo10.mStyledText.mText = Text( "o" );
  wordLayout1.mCharactersLayoutInfo.push_back( layoutInfo10 ); // o

  // (white space)
  wordLayout2.mSize = Size( WIDTH_10, HEIGHT_10 );
  wordLayout2.mAscender = ASCENDER_10;
  wordLayout2.mType = TextViewProcessor::WordSeparator;
  layoutInfo10.mStyledText.mText = Text( " " );
  wordLayout2.mCharactersLayoutInfo.push_back( layoutInfo10 ); // (white space)

  // world!
  wordLayout3.mSize = Size( 2.f * WIDTH_10 + 4.f * WIDTH_12, HEIGHT_12 );
  wordLayout3.mAscender = ASCENDER_12;
  wordLayout3.mType = TextViewProcessor::NoSeparator;
  layoutInfo10.mStyledText.mText = Text( "w" );
  wordLayout3.mCharactersLayoutInfo.push_back( layoutInfo10 ); // w
  layoutInfo10.mStyledText.mText = Text( "o" );
  wordLayout3.mCharactersLayoutInfo.push_back( layoutInfo10 ); // o
  layoutInfo12.mStyledText.mText = Text( "r" );
  wordLayout3.mCharactersLayoutInfo.push_back( layoutInfo12 ); // r
  layoutInfo12.mStyledText.mText = Text( "l" );
  wordLayout3.mCharactersLayoutInfo.push_back( layoutInfo12 ); // l
  layoutInfo12.mStyledText.mText = Text( "d" );
  wordLayout3.mCharactersLayoutInfo.push_back( layoutInfo12 ); // d
  layoutInfo12.mStyledText.mText = Text( "!" );
  wordLayout3.mCharactersLayoutInfo.push_back( layoutInfo12 ); // !

  // (new paragraph character)
  wordLayout4.mSize = Size( 0.f, HEIGHT_12 );
  wordLayout4.mAscender = ASCENDER_12;
  wordLayout4.mType = TextViewProcessor::ParagraphSeparator;
  layoutInfo12.mStyledText.mText = Text( "\n" );
  layoutInfo12.mSize.width = 0.f;
  wordLayout4.mCharactersLayoutInfo.push_back( layoutInfo12 ); // (new paragraph char)

  // Paragraphs

  TextViewProcessor::ParagraphLayoutInfo paragraphLayout1, paragraphLayout2, paragraphLayout3;

  paragraphLayout1.mSize = Size( 5.f * WIDTH_10 + 7.f * WIDTH_12, HEIGHT_12 );
  paragraphLayout1.mAscender = ASCENDER_12;
  paragraphLayout1.mNumberOfCharacters = 13;
  paragraphLayout1.mWordsLayoutInfo.push_back( wordLayout1 );
  paragraphLayout1.mWordsLayoutInfo.push_back( wordLayout2 );
  paragraphLayout1.mWordsLayoutInfo.push_back( wordLayout3 );
  paragraphLayout1.mWordsLayoutInfo.push_back( wordLayout4 );

  paragraphLayout2.mSize = Size( 0.f, HEIGHT_12 );
  paragraphLayout2.mAscender = ASCENDER_12;
  paragraphLayout2.mNumberOfCharacters = 1;
  paragraphLayout2.mWordsLayoutInfo.push_back( wordLayout4 );

  paragraphLayout3.mSize = Size( 0.f, HEIGHT_12 );

  // Text (layout)
  TextViewProcessor::TextLayoutInfo textLayout;

  textLayout.mWholeTextSize = Size( 5.f * WIDTH_10 + 7.f * WIDTH_12, 3.f * HEIGHT_12 );
  textLayout.mMaxWordWidth = 2.f * WIDTH_10 + 4.f * WIDTH_12;
  textLayout.mNumberOfCharacters = 14;
  textLayout.mParagraphsLayoutInfo.push_back( paragraphLayout1 );
  textLayout.mParagraphsLayoutInfo.push_back( paragraphLayout2 );
  textLayout.mParagraphsLayoutInfo.push_back( paragraphLayout3 );

  if(!TestEqual( textLayout, textLayoutInfo ))
  {
    std::cout << "Layout fails" << std::endl;
    Print(textLayout); std::cout << std::endl;
    Print(textLayoutInfo); std::cout << std::endl;
  }

  DALI_TEST_CHECK( TestEqual( textLayout, textLayoutInfo ) );
  END_TEST;
}

int UtcDaliTextViewSplitWord(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitWord : ");

  struct SplitWordTest splitWordTests[] =
  {
    {
      std::string( "Split word, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      0,
      std::string( "" ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
    },
    {
      std::string( "Split word, position 8." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      8,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      std::string( "" ),
    },
    {
      std::string( "Split word, position 2." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      2,
      std::string( "<font size='10'>He</font>" ),
      std::string( "<font size='12'>ll</font><font size='10'>oooo</font>" ),
    },
    {
      std::string( "Split word, position 3." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      3,
      std::string( "<font size='10'>He</font><font size='12'>l</font>" ),
      std::string( "<font size='12'>l</font><font size='10'>oooo</font>" ),
    },
    {
      std::string( "Split word, position 4." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      4,
      std::string( "<font size='10'>He</font><font size='12'>ll</font>" ),
      std::string( "<font size='10'>oooo</font>" ),
    },
  };
  const std::size_t numberOfTests( 5u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const SplitWordTest& test = splitWordTests[index];

    if( !TestSplitWord( test.description, test.input, test.position, test.firstResult, test.lastResult, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewUpdateTextInfo(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewUpdateTextInfo : ");

  struct UpdateTextInfoTest updateTextInfoTest[] =
  {
    // Remove operations

    {
      std::string( "Remove from new paragraph character to first character next paragraph." ),
      Remove,
      std::string("Hello world\nhello world."),
      11,
      2,
      std::string(""),
      0.f,
      std::string("Hello worldello world."),
    },
    {
      std::string( "Replace style from new paragraph character to first character next paragraph." ),
      Replace,
      std::string("Hello world\nhello world."),
      11,
      2,
      std::string("<b>\nh</b>"),
      0.f,
      std::string("Hello world<b>\nh</b>ello world."),
    },
    {
      std::string( "Remove from the beginning to the middle of last word." ),
      Remove,
      std::string("Hello world, hello world."),
      0,
      22,
      std::string(), // Not used.
      0.f,
      std::string("ld."),
    },
    {
      std::string( "Remove from the beginning to the middle of the text." ),
      Remove,
      std::string("Hello world hello world."),
      0,
      12,
      std::string(), // Not used.
      0.f,
      std::string("hello world."),
    },
    // Remove within the same word:
    // * within the same group of characters with same style.
    {
      std::string( "Remove within the same word, within the same group of characters with same style" ),
      Remove,
      std::string("Hello <font size='30'>world\nhello</font> world"),
      7,
      3,
      std::string(), // Not used.
      0.f,
      std::string( "Hello <font size='30'>wd\nhello</font> world" )
    },
    // * whole group of characters (merge adjacent group of characters)
    {
      std::string( "Remove within the same word, whole group of characters (merge adjacent group of characters)" ),
      Remove,
      std::string("Hello <font size='30'>w<font size='20'>orl</font>d\nhello</font> world"),
      7,
      3,
      std::string(), // Not used.
      0.f,
      std::string( "Hello <font size='30'>wd\nhello</font> world" )
    },
    // * whole group of characters (don't merge adjacent gtoup of characters)
    {
      std::string( "Remove within the same word, whole group of characters (don't merge adjacent gtoup of characters)" ),
      Remove,
      std::string("Hello <font size='30'>w</font>orl<font size='10'>d\nhello</font> world"),
      7,
      3,
      std::string(), // Not used.
      0.f,
      std::string( "Hello <font size='30'>w</font><font size='10'>d\nhello</font> world" )
    },
    // * Remove whole word (merge words)
    {
      std::string( "Remove within the same word, whole word (merge words)" ),
      Remove,
      std::string("Hello <font size='30'>w</font>orl<font size='10'>d\nhello</font> world"),
      5,
      1,
      std::string(), // Not used.
      0.f,
      std::string( "Hello<font size='30'>w</font>orl<font size='10'>d\nhello</font> world" )
    },
    // * Remove whole word (don't merge words)
    {
      std::string( "Remove within the same word, whole word (don't merge words)" ),
      Remove,
      std::string("Hello <font size='30'>w</font>orl<font size='10'>d\nhello</font> world"),
      6,
      5,
      std::string(), // Not used.
      0.f,
      std::string( "Hello <font size='10'>\nhello</font> world" )
    },
    // * Remove whole word (merge paragraphs)
    {
      std::string( "Remove within the same word, whole word (merge paragraphs)" ),
      Remove,
      std::string("Hello <font size='30'>w</font>orl<font size='10'>d\nhello</font> world"),
      11,
      1,
      std::string(), // Not used.
      0.f,
      std::string( "Hello <font size='30'>w</font>orl<font size='10'>dhello</font> world" )
    },
    // * Remove RTL text within LTR
    /* TODO check this when RTL text is working
    {
      std::string( "Remove within the same paragraph, RTL text within LTR." ),
      Remove,
      std::string("Hello world, שלום עולם, hello world"),
      10,
      15,
      std::string(), // Not used.
      0.f,
      std::string( "Hello worlello world" )
    },
    */
    // * Remove whole paragraph
    {
      std::string( "Remove whole paragraph" ),
      Remove,
      std::string("Hello world, hello world\n"
                  "Hello world, hello world\n"
                  "Hello world, hello world\n"
                  "Hello world, hello world\n"),
      25,
      25,
      std::string(), // Not used.
      0.f,
      std::string("Hello world, hello world\n"
                  "Hello world, hello world\n"
                  "Hello world, hello world\n"),
    },
    {
      std::string( "Remove whole paragraph" ),
      Remove,
      std::string("Hello world, hello world\n"
                  "H"),
      25,
      1,
      std::string(), // Not used.
      0.f,
      std::string("Hello world, hello world\n"),
    },


    // Insert operations
    {
      std::string( "insert some text" ),
      Insert,
      std::string("inpuext"),
      4,
      0,             // Not used
      std::string( "t t" ),
      0.f,
      std::string( "input text" )
    },
    {
      std::string( "Insert text at the end" ),
      Insert,
      std::string("touch "),
      6,
      0,
      std::string("me\nhello"),
      0.f,
      std::string("touch me\nhello")
    },

    // Replace operations.
    {
      std::string( "Replace style from the beginning to some point in the middle of the text." ),
      Replace,
      std::string( "Hello <font color='green'>world</font>" ),
      0,
      7,
      std::string( "<font color='red'>Hello w</font>" ),
      0.f,
      std::string( "<font color='red'>Hello w</font><font color='green'>orld</font>" )
    },
    {
      std::string( "Replace style from the middle of the text to the end." ),
      Replace,
      std::string( "Touch me\nhello" ),
      6,
      8,
      std::string( "<b>me\nhello</b>" ),
      0.f,
      std::string( "Touch <b>me\nhello</b>" )
    },
    {
      std::string( "Remove characters from text. Previous next test:Replace style from the middle of the text 1." ),
      Remove,
      std::string( "Touch me\nhello\nworld" ),
      6,
      8,
      std::string( "" ),
      0.f,
      std::string( "Touch \nworld" )
    },
    {
      std::string( "Insert styled text in the middle of a text. Previous: Replace style from the middle of the text 1." ),
      Insert,
      std::string( "Touch \nworld" ),
      6,
      0,
      std::string( "<b>me\nhello</b>" ),
      0.f,
      std::string( "Touch <b>me\nhello</b>\nworld" )
    },
    {
      std::string( "Replace style from the middle of the text 1." ),
      Replace,
      std::string( "Touch me\nhello\nworld" ),
      6,
      8,
      std::string( "<b>me\nhello</b>" ),
      0.f,
      std::string( "Touch <b>me\nhello</b>\nworld" )
    },
    {
      std::string( "Remove characters from text. Previous next test:Replace style from the middle of the text 2." ),
      Remove,
      std::string( "Touch me\nhello\nworld" ),
      6,
      9,
      std::string( "" ),
      0.f,
      std::string( "Touch world" )
    },
    {
      std::string( "Replace style from the middle of the text 2." ),
      Replace,
      std::string( "Touch me\nhello\nworld" ),
      6,
      9,
      std::string( "<b>me\nhello\n</b>" ),
      0.f,
      std::string( "Touch <b>me\nhello\n</b>world" )
    },
  };
  const std::size_t numberOfTests( 21u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const UpdateTextInfoTest& test = updateTextInfoTest[index];

    if( !TestUpdateTextInfo( test.description, test.operation, test.input, test.position, test.numberOfCharacters, test.inputText, test.lineHeightOffset, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewSplitParagraph(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitParagraph : ");

  struct SplitParagraphTest splitParagraphTests[] =
  {
    {
      std::string( "Split paragraph, wordPosition 0, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      0,
      0,
      3.f,
      std::string( "" ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
    },
    {
      std::string( "Split paragraph, wordPosition 10, position 4." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      10,
      4,
      0.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      std::string( "" ),
    },
    {
     std::string( "Split paragraph, wordPosition 2, position 4." ),
      std::string("<font size='10'>Hello </font>wor<font size='12'>ld, hello wo</font>rld"),
      2,
      4,
      0.f,
      std::string("<font size='10'>Hello </font>wor<font size='12'>l</font>"),
      std::string("<font size='12'>d, hello wo</font>rld")
    }
    /* TODO RTL
    {
      std::string( "Split paragraph, wordPosition 6, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      6,
      0,
      0.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום " ),
      std::string( "עולם text text" ),
    },
    {
      std::string( "Split paragraph, wordPosition 4, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      4,
      0,
      0.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> " ),
      std::string( "שלום עולם text text" ),
    },
    {
      std::string( "Split paragraph2, wordPosition 8, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      8,
      0,
      6.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם " ),
      std::string( "text text" ),
    },
    */
  };
  const std::size_t numberOfTests( 3u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const SplitParagraphTest& test = splitParagraphTests[index];

    if( !TestSplitParagraph( test.description,
                             test.input,
                             test.wordIndex,
                             test.characterIndex,
                             test.lineHeightOffset,
                             test.firstResult,
                             test.lastResult,
                             TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewMergeWord01(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeWord01 : ");

  struct MergeWordsTest mergeWordsTests[] =
  {
    {
      std::string( "Merge words with same style." ),
      std::string( "Hel" ),
      std::string( "lo" ),
      std::string( "Hello" ),
    },
    {
      std::string( "Merge words with different styles." ),
      std::string( "<font size='10>Hel</font>" ),
      std::string( "<font size='20'>lo</font>" ),
      std::string( "<font size='10'>Hel</font><font size='20'>lo</font>" )
    },
  };
  const std::size_t numberOfTests( 2u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const MergeWordsTest& test = mergeWordsTests[index];

    if( !TestMergeWords( test.description, test.inputFirst, test.inputLast, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewMergeWord02(void)
{
  // Negative test.
  // It test white spaces and new paragraph characters can't be merged to other words.

  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeWord02 : ");

  // Generate three words

  Toolkit::Internal::TextView::RelayoutData relayoutData01;
  Toolkit::Internal::TextView::RelayoutData relayoutData02;
  Toolkit::Internal::TextView::RelayoutData relayoutData03;
  TextViewProcessor::TextLayoutInfo& textLayoutInfo01( relayoutData01.mTextLayoutInfo );
  TextViewProcessor::TextLayoutInfo& textLayoutInfo02( relayoutData02.mTextLayoutInfo );
  TextViewProcessor::TextLayoutInfo& textLayoutInfo03( relayoutData03.mTextLayoutInfo );

  std::string text01( " " );
  std::string text02( "\n" );
  std::string text03( "a" );
  MarkupProcessor::StyledTextArray styledText01;
  MarkupProcessor::StyledTextArray styledText02;
  MarkupProcessor::StyledTextArray styledText03;
  MarkupProcessor::GetStyledTextArray( text01, styledText01, true );
  MarkupProcessor::GetStyledTextArray( text02, styledText02, true );
  MarkupProcessor::GetStyledTextArray( text03, styledText03, true );

  TextViewProcessor::CreateTextInfo( styledText01,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData01 );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo01;

  wordLayoutInfo01 = *( *textLayoutInfo01.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText02,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData02 );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo02;

  wordLayoutInfo02 = *( *textLayoutInfo02.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText03,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData03 );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo03;

  wordLayoutInfo03 = *( *textLayoutInfo03.mParagraphsLayoutInfo.begin() ).mWordsLayoutInfo.begin();

  // Test MergeWord() asserts if white spaces or new paragraph chars are merged.
  bool assert1 = false;
  bool assert2 = false;
  bool assert3 = false;
  bool assert4 = false;
  bool assert5 = false;
  bool assert6 = false;

  try
  {
    MergeWord( wordLayoutInfo01,
               wordLayoutInfo02 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words.\"", TEST_LOCATION );
    assert1 = true;
  }
  try
  {
    MergeWord( wordLayoutInfo01,
               wordLayoutInfo03 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words.\"", TEST_LOCATION );
    assert2 = true;
  }
  try
  {
    MergeWord( wordLayoutInfo02,
               wordLayoutInfo01 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words.\"", TEST_LOCATION );
    assert3 = true;
  }
  try
  {
    MergeWord( wordLayoutInfo02,
               wordLayoutInfo03 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words.\"", TEST_LOCATION );
    assert4 = true;
  }
  try
  {
    MergeWord( wordLayoutInfo03,
               wordLayoutInfo01 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words.\"", TEST_LOCATION );
    assert5 = true;
  }
  try
  {
    MergeWord( wordLayoutInfo03,
               wordLayoutInfo02 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new paragraph characters can't be merged with other words.\"", TEST_LOCATION );
    assert6 = true;
  }

  if( assert1 && assert2 && assert3 && assert4 && assert5 && assert6 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
  END_TEST;
}

int UtcDaliTextViewMergeParagraph01(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeParagraph01 : ");

  struct MergeParagraphsTest mergeParagraphsTests[] =
  {
    {
      std::string( "Merge a void first paragraph." ),
      std::string( "" ),
      std::string( "Hello world, this is a whole paragraph" ),
      2.f,
      std::string( "Hello world, this is a whole paragraph" )
    },
    {
      std::string( "Merge a void last paragraph." ),
      std::string( "Hello world, this is a whole paragraph" ),
      std::string( "" ),
      0.f,
      std::string( "Hello world, this is a whole paragraph" )
    },
    /* TODO RTL
    {
      std::string( "Merge paragraphs: last starting with RTL text and first ending with RTL" ),
      std::string( "Hello world, שלום" ),
      std::string( " עולם, hello world." ),
      6.f,
      std::string( "Hello world, שלום עולם, hello world." )
    },
    {
      std::string( "Merge paragraphs and don't merge last and first words." ),
      std::string( "Hello world, " ),
      std::string( "שלום עולם, hello world." ),
      3.f,
      std::string( "Hello world, שלום עולם, hello world." )
    },
    */
    {
      std::string( "Merge paragraphs. Don't merge words" ),
      std::string( "Hello world," ),
      std::string( " this is a whole paragraph" ),
      0.f,
      std::string( "Hello world, this is a whole paragraph" )
    },
    {
      std::string( "Merge paragraphs. Merge words" ),
      std::string( "Hello world, th" ),
      std::string( "is is a whole paragraph" ),
      0.f,
      std::string( "Hello world, this is a whole paragraph" )
    },
  };
  const std::size_t numberOfTests( 4u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const MergeParagraphsTest& test = mergeParagraphsTests[index];

    if( !TestMergeParagraphs( test.description, test.inputFirst, test.inputLast, test.lineHeightOffset, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewMergeParagraph02(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeParagraph02 : ");

  Toolkit::Internal::TextView::RelayoutData relayoutData01;
  Toolkit::Internal::TextView::RelayoutData relayoutData02;
  TextViewProcessor::TextLayoutInfo& textLayoutInfo01( relayoutData01.mTextLayoutInfo );
  TextViewProcessor::TextLayoutInfo& textLayoutInfo02( relayoutData02.mTextLayoutInfo );

  std::string text01( "Hello world\n" );
  std::string text02( "hello world" );
  MarkupProcessor::StyledTextArray styledText01;
  MarkupProcessor::StyledTextArray styledText02;
  MarkupProcessor::GetStyledTextArray( text01, styledText01, true );
  MarkupProcessor::GetStyledTextArray( text02, styledText02, true );

  TextViewProcessor::CreateTextInfo( styledText01,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData01 );

  TextViewProcessor::ParagraphLayoutInfo paragraphLayoutInfo01;

  paragraphLayoutInfo01 = *textLayoutInfo01.mParagraphsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText02,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData02 );

  TextViewProcessor::ParagraphLayoutInfo paragraphLayoutInfo02;

  paragraphLayoutInfo02 = *textLayoutInfo02.mParagraphsLayoutInfo.begin();

  bool assert1 = false;

  try
  {
    MergeParagraph( paragraphLayoutInfo01,
                    paragraphLayoutInfo02 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeParagraph(). ERROR: A paragraph can't be merged to another paragraph which finishes with a new paragraph character.\"", TEST_LOCATION );
    assert1 = true;
  }

  if( assert1 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
  END_TEST;
}

int UtcDaliTextViewRemoveCharactersFromWord(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeWord02 : ");

  struct RemoveCharactersFromWordTest removeCharactersFromWordTests[] =
  {
    {
      std::string( "Delete 0 characters." ),
      std::string( "Hello" ),
      3,
      0,
      std::string( "Hello" ),
    },
    {
      std::string( "Delete within the same group of characters. Starting from the beginning" ),
      std::string( "Hello" ),
      0,
      3,
      std::string( "lo" ),
    },
    {
      std::string( "Delete within the same group of characters. Somewhere in the middle" ),
      std::string( "Hello" ),
      2,
      2,
      std::string( "Heo" ),
    },
    {
      std::string( "Delete within the same group of characters. Starting somewhere in the middle to the end" ),
      std::string( "Hello" ),
      3,
      2,
      std::string( "Hel" ),
    },
    {
      std::string( "Delete within the same group of characters. Finish just before a new one." ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='30'>rld</font>" ),
      1,
      2,
      std::string( "<font size='10'>H</font><font size='20'>loWo</font><font size='30'>rld</font>" ),
    },
    {
      std::string( "Delete starting in one group of characters and finishing in a different one. No merge of groups." ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='30'>rld</font>" ),
      2,
      3,
      std::string( "<font size='10'>He</font><font size='20'>Wo</font><font size='30'>rld</font>" ),
    },
    {
      std::string( "Delete within the same group of characters. Starting just after a different one." ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='30'>rld</font>" ),
      7,
      2,
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='30'>d</font>" ),
    },
    {
      std::string( "Delete whole group of characters. No merge" ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='30'>rld</font>" ),
      3,
      4,
      std::string( "<font size='10'>Hel</font><font size='30'>rld</font>" ),
    },
    {
      std::string( "Delete whole group of characters and part of the adjacent ones. No merge" ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='30'>rld</font>" ),
      2,
      6,
      std::string( "<font size='10'>He</font><font size='30'>ld</font>" ),
    },
    {
      std::string( "Delete whole group of characters. Merge" ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='10'>rld</font>" ),
      3,
      4,
      std::string( "<font size='10'>Helrld</font>" ),
    },
    {
      std::string( "Delete whole group of characters and part of the adjacent ones. Merge" ),
      std::string( "<font size='10'>Hel</font><font size='20'>loWo</font><font size='10'>rld</font>" ),
      2,
      6,
      std::string( "<font size='10'>Held</font>" ),
    },
  };
  const std::size_t numberOfTests( 11u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const RemoveCharactersFromWordTest& test = removeCharactersFromWordTests[index];

    if( !TestRemoveCharactersFromWord( test.description, test.input, test.position, test.numberOfCharacters, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewRemoveWordsFromParagraph(void)
{
  // Note: Currently RemoveWordsFromParagraph() function is only used to remove a number of words from the beginning, or
  //       from a given index to the end.

  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewRemoveWordsFromParagraph : ");
  struct RemoveWordsFromParagraphTest removeWordsFromParagraphTest[] =
  {
    {
      std::string( "Delete 0 words." ),
      std::string( "Hello hello, שלום עולם hello hello" ),
      0,
      0,
      2.f,
      std::string( "Hello hello, שלום עולם hello hello" ),
    },
    {
      std::string( "Delete from the middle to the end." ),
      std::string( "Hello hello, שלום עולם hello hello" ),
      4,
      7,
      0.f,
      std::string( "Hello hello, " ),
    },
    {
      std::string( "Delete from the beginning to the middle." ),
      std::string( "Hello hello, שלום עולם hello hello" ),
      0,
      8,
      6.f,
      std::string( "hello hello" ),
    },
  };
  const std::size_t numberOfTests( 3u );

  for( std::size_t index = 0u; index < numberOfTests; ++index )
  {
    const RemoveWordsFromParagraphTest& test = removeWordsFromParagraphTest[index];

    if( !TestRemoveWordsFromParagraph( test.description, test.input, test.wordIndex, test.numberOfWords, test.lineHeightOffset, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}
