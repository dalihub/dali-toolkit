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
#include <dali-toolkit/internal/controls/text-view/text-view-line-processor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-word-group-processor.h>
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

struct SplitWordGroupTest
{
  std::string description;
  std::string input;
  std::size_t wordPosition;
  std::size_t position;
  std::string firstResult;
  std::string lastResult;
};

struct SplitLineTest
{
  std::string description;
  std::string input;
  std::size_t groupPosition;
  std::size_t wordPosition;
  std::size_t position;
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

struct MergeWordGroupsTest
{
  std::string description;
  std::string inputFirst;
  std::string inputLast;
  std::string result;
};

struct MergeLinesTest
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

struct RemoveWordsFromGroupTest
{
  std::string description;
  std::string input;
  std::size_t wordIndex;
  std::size_t numberOfWords;
  std::string result;
};

struct RemoveGroupsFromLineTest
{
  std::string description;
  std::string input;
  std::size_t groupIndex;
  std::size_t numberOfGroups;
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

void Print( const TextViewProcessor::WordGroupLayoutInfo& wordGroup )
{
  std::cout << "(";
  std::cout << "              mSize : " << wordGroup.mSize << std::endl;
  std::cout << "          mAscender : " << wordGroup.mAscender << std::endl;
  std::cout << "         mDirection : " << wordGroup.mDirection << std::endl;
  std::cout << "mNumberOfCharacters : " << wordGroup.mNumberOfCharacters << std::endl;
  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator it = wordGroup.mWordsLayoutInfo.begin(), endIt = wordGroup.mWordsLayoutInfo.end(); it != endIt; ++it )
  {
    Print( *it );
  }
  std::cout << ")"; std::cout << std::endl;
}

void Print( const TextViewProcessor::LineLayoutInfo& line )
{
  std::cout << "<";
  std::cout << "              mSize : " << line.mSize << std::endl;
  std::cout << "          mAscender : " << line.mAscender << std::endl;
  std::cout << "mNumberOfCharacters : " << line.mNumberOfCharacters << std::endl;
  for( TextViewProcessor::WordGroupLayoutInfoContainer::const_iterator it = line.mWordGroupsLayoutInfo.begin(), endIt = line.mWordGroupsLayoutInfo.end(); it != endIt; ++it )
  {
    Print( *it );
  }
  std::cout << ">" << std::endl;
}

void Print( const TextViewProcessor::TextLayoutInfo& text )
{
  std::cout << "||";
  for( TextViewProcessor::LineLayoutInfoContainer::const_iterator it = text.mLinesLayoutInfo.begin(), endIt = text.mLinesLayoutInfo.end(); it != endIt; ++it )
  {
    Print( *it );
  }
  std::cout << "||" << std::endl;
}

void Print( const TextStyle& style )
{
  std::cout << " font name : " << style.GetFontName() << std::endl;
  std::cout << " : " << style.GetFontStyle() << std::endl;
  std::cout << " : " << style.GetFontPointSize() << std::endl;
  std::cout << " : " << style.GetWeight() << std::endl;
  std::cout << " : " << style.GetTextColor() << std::endl;
  std::cout << " : " << style.IsItalicsEnabled() << std::endl;
  std::cout << " : " << style.IsUnderlineEnabled() << std::endl;
  std::cout << " : " << style.IsShadowEnabled() << std::endl;
  std::cout << " : " << style.GetShadowColor() << std::endl;
  std::cout << " : " << style.GetShadowOffset() << std::endl;
  std::cout << " : " << style.IsGlowEnabled() << std::endl;
  std::cout << " : " << style.GetGlowColor() << std::endl;
  std::cout << " : " << style.GetGlowIntensity() << std::endl;
  std::cout << " : " << style.GetSmoothEdge() << std::endl;
  std::cout << " : " << style.IsOutlineEnabled() << std::endl;
  std::cout << " : " << style.GetOutlineThickness() << std::endl;
}

// Test functions used to check if two data structures are equal.

bool TestEqual( float x, float y )
{
  return ( fabsf( x - y ) < Math::MACHINE_EPSILON_1000 );
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

bool TestEqual( const TextViewProcessor::WordGroupLayoutInfo& group1,
                const TextViewProcessor::WordGroupLayoutInfo& group2 )
{

  if( group1.mNumberOfCharacters != group2.mNumberOfCharacters )
  {
    return false;
  }

  if( group1.mWordsLayoutInfo.size() != group2.mWordsLayoutInfo.size() )
  {
    return false;
  }

  if( !TestEqual( group1.mSize.x, group2.mSize.x ) )
  {
    return false;
  }
  if( !TestEqual( group1.mSize.y, group2.mSize.y ) )
  {
    return false;
  }

  if( !TestEqual( group1.mAscender, group2.mAscender ) )
  {
    return false;
  }

  if( group1.mDirection != group2.mDirection )
  {
    return false;
  }

  for( TextViewProcessor::WordLayoutInfoContainer::const_iterator it1 = group1.mWordsLayoutInfo.begin(), endIt1 = group1.mWordsLayoutInfo.end(),
         it2 = group2.mWordsLayoutInfo.begin(), endIt2 = group2.mWordsLayoutInfo.end();
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

bool TestEqual( const TextViewProcessor::LineLayoutInfo& line1,
                const TextViewProcessor::LineLayoutInfo& line2 )
{
  if( !TestEqual( line1.mSize.x, line2.mSize.x ) )
  {
    return false;
  }
  if( !TestEqual( line1.mSize.y, line2.mSize.y ) )
  {
    return false;
  }

  if( !TestEqual( line1.mAscender, line2.mAscender ) )
  {
    return false;
  }

  if( line1.mNumberOfCharacters != line2.mNumberOfCharacters )
  {
    return false;
  }

  if( line1.mWordGroupsLayoutInfo.size() != line2.mWordGroupsLayoutInfo.size() )
  {
    return false;
  }

  for( TextViewProcessor::WordGroupLayoutInfoContainer::const_iterator it1 = line1.mWordGroupsLayoutInfo.begin(), endIt1 = line1.mWordGroupsLayoutInfo.end(),
         it2 = line2.mWordGroupsLayoutInfo.begin(), endIt2 = line2.mWordGroupsLayoutInfo.end();
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

  if( text1.mLinesLayoutInfo.size() != text2.mLinesLayoutInfo.size() )
  {
    return false;
  }

  for( TextViewProcessor::LineLayoutInfoContainer::const_iterator it1 = text1.mLinesLayoutInfo.begin(), endIt1 = text1.mLinesLayoutInfo.end(),
         it2 = text2.mLinesLayoutInfo.begin(), endIt2 = text2.mLinesLayoutInfo.end();
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
  tet_printf( "%s", description.c_str() );

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

  if( !inputLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        inputWordLayout = *( *( *inputLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
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

  if( !firstResultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *firstResultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        firstResultWordLayout = *( *( *firstResultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
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

  if( !lastResultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *lastResultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        lastResultWordLayout = *( *( *lastResultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
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
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  if( !TestEqual( lastWordLayoutInfo, lastResultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}

/**
 * Splits the \e input group of words in two by the given \e wordPosition and \e position and checks the results with \e firstResult and \e lastResult.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Split the group of words from the beginning. (wordPosition 0 and position 0)".
 * @param input The input word.
 * @param wordPosition Index to the word within the group where to split the group.
 * @param position Where to split the word.
 * @param firstResult First part of the split group of words.
 * @param lastResult Last part of the split group of words.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestSplitWordGroup( const std::string& description,
                         const std::string& input,
                         const size_t wordPosition,
                         const size_t position,
                         const std::string& firstResult,
                         const std::string& lastResult,
                         const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create layout info for the input group of words.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  // Get the input group of words
  TextViewProcessor::WordGroupLayoutInfo inputWordGroupLayout;

  if( !inputLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      inputWordGroupLayout = *( *inputLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  // Create layout info for the first part of the result (after split the group of words)

  Toolkit::Internal::TextView::RelayoutData firstRelayoutData;
  TextViewProcessor::TextLayoutInfo& firstResultLayout( firstRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray firstResultStyledText;
  MarkupProcessor::GetStyledTextArray( firstResult, firstResultStyledText, true );

  TextViewProcessor::CreateTextInfo( firstResultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     firstRelayoutData );

  // Get the first result group of words
  TextViewProcessor::WordGroupLayoutInfo firstResultWordGroupLayout;

  if( !firstResultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *firstResultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      firstResultWordGroupLayout = *( *firstResultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  // Create layout info for the last part of the result (after split the group of words)

  Toolkit::Internal::TextView::RelayoutData lastRelayoutData;
  TextViewProcessor::TextLayoutInfo& lastResultLayout( lastRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray lastResultStyledText;
  MarkupProcessor::GetStyledTextArray( lastResult, lastResultStyledText, true );

  TextViewProcessor::CreateTextInfo( lastResultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     lastRelayoutData );

  // Get the last result group of words
  TextViewProcessor::WordGroupLayoutInfo lastResultWordGroupLayout;

  if( !lastResultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *lastResultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      lastResultWordGroupLayout = *( *lastResultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  // Split the group of words.

  TextViewProcessor::WordGroupLayoutInfo lastWordGroupLayoutInfo;

  TextViewProcessor::TextInfoIndices indices( 0, 0, wordPosition, position );
  SplitWordGroup( indices,
                  inputWordGroupLayout,
                  lastWordGroupLayoutInfo );

  // Test results
  if( !TestEqual( inputWordGroupLayout, firstResultWordGroupLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  if( !TestEqual( lastWordGroupLayoutInfo, lastResultWordGroupLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}

/**
 * Splits the \e input line in two by the given \e groupPosition, \e wordPosition and \e position and checks the results with \e firstResult and \e lastResult.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Split the line from the beginning. (groupPosition 0, wordPosition 0 and position 0)".
 * @param input The input word.
 * @param groupPosition Index to the group of words within the line where to split the line.
 * @param wordPosition Index to the word within the group where to split the group.
 * @param position Where to split the word.
 * @param lineHeightOffset Offset between lines.
 * @param firstResult First part of the split line.
 * @param lastResult Last part of the split line.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestSplitLine( const std::string& description,
                    const std::string& input,
                    const size_t groupPosition,
                    const size_t wordPosition,
                    const size_t position,
                    const float lineHeightOffset,
                    const std::string& firstResult,
                    const std::string& lastResult,
                    const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create layout info for the input line.
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

  // Get the input line
  TextViewProcessor::LineLayoutInfo inputLineLayout;

  if( !inputLayout.mLinesLayoutInfo.empty() )
  {
    inputLineLayout = *inputLayout.mLinesLayoutInfo.begin();
  }

  // Create layout info for the first part of the result (after split the line)

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

  // Get the first result line
  TextViewProcessor::LineLayoutInfo firstResultLineLayout;

  if( !firstResultLayout.mLinesLayoutInfo.empty() )
  {
    firstResultLineLayout = *firstResultLayout.mLinesLayoutInfo.begin();
  }

  // Create layout info for the last part of the result (after split the line)

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

  // Get the last result line
  TextViewProcessor::LineLayoutInfo lastResultLineLayout;

  if( !lastResultLayout.mLinesLayoutInfo.empty() )
  {
    lastResultLineLayout = *lastResultLayout.mLinesLayoutInfo.begin();
  }

  // Split the line.

  TextViewProcessor::LineLayoutInfo lastLineLayoutInfo;

  TextViewProcessor::TextInfoIndices indices( 0, groupPosition, wordPosition, position );
  SplitLine( indices,
             PointSize( lineHeightOffset ),
             inputLineLayout,
             lastLineLayoutInfo );

  // Test results
  if( !TestEqual( inputLineLayout, firstResultLineLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  if( !TestEqual( lastLineLayoutInfo, lastResultLineLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
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
  tet_printf( "%s", description.c_str() );

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

  if( !inputFirstLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputFirstLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        inputFirstWordLayout = *( *( *inputFirstLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
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

  if( !inputLastLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputLastLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        inputLastWordLayout = *( *( *inputLastLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
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

  if( !resultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *resultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        resultWordLayout = *( *( *resultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
    }
  }

  MergeWord( inputFirstWordLayout,
             inputLastWordLayout );

  if( !TestEqual( inputFirstWordLayout, resultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}

/**
 * Merges the \e inputFirst group of words and the \e inputLast group of words, and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param inputFirst The first part of the group of words.
 * @param inputLast The last part of the group of words.
 * @param result The merged group of word.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestMergeGroupsOfWords( const std::string& description, const std::string& inputFirst, const std::string& inputLast, const std::string& result, const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create layout info for the inputFirst group of word.
  Toolkit::Internal::TextView::RelayoutData firstRelayoutData;
  TextViewProcessor::TextLayoutInfo& inputFirstLayout( firstRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputFirstStyledText;
  MarkupProcessor::GetStyledTextArray( inputFirst, inputFirstStyledText, true );

  TextViewProcessor::CreateTextInfo( inputFirstStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     firstRelayoutData );

  // Get the input group of words.
  TextViewProcessor::WordGroupLayoutInfo inputFirstWordGroupLayout;

  if( !inputFirstLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputFirstLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      inputFirstWordGroupLayout = *( *inputFirstLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  // Create layout info for the inputLast group of words.
  Toolkit::Internal::TextView::RelayoutData lastRelayoutData;
  TextViewProcessor::TextLayoutInfo& inputLastLayout( lastRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputLastStyledText;
  MarkupProcessor::GetStyledTextArray( inputLast, inputLastStyledText, true );

  TextViewProcessor::CreateTextInfo( inputLastStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     lastRelayoutData );

  // Get the input group of words
  TextViewProcessor::WordGroupLayoutInfo inputLastWordGroupLayout;

  if( !inputLastLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputLastLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      inputLastWordGroupLayout = *( *inputLastLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  // Create layout info for the result group of words.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     resultRelayoutData );

  // Get the result word
  TextViewProcessor::WordGroupLayoutInfo resultWordGroupLayout;

  if( !resultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *resultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      resultWordGroupLayout = *( *resultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  MergeWordGroup( inputFirstWordGroupLayout,
                  inputLastWordGroupLayout );

  if( !TestEqual( inputFirstWordGroupLayout, resultWordGroupLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}

/**
 * Merges the \e inputFirst line and the \e inputLast line, and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param inputFirst The first part of the line.
 * @param inputLast The last part of the line.
 * @param lineHeightOffset Offset between lines.
 * @param result The merged line.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestMergeLines( const std::string& description, const std::string& inputFirst, const std::string& inputLast, const float lineHeightOffset, const std::string& result, const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create layout info for the inputFirst line.
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
  TextViewProcessor::LineLayoutInfo inputFirstLineLayout;

  if( !inputFirstLayout.mLinesLayoutInfo.empty() )
  {
    inputFirstLineLayout = *inputFirstLayout.mLinesLayoutInfo.begin();
  }

  // Create layout info for the inputLast line.
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
  TextViewProcessor::LineLayoutInfo inputLastLineLayout;

  if( !inputLastLayout.mLinesLayoutInfo.empty() )
  {
    inputLastLineLayout = *inputLastLayout.mLinesLayoutInfo.begin();
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
  TextViewProcessor::LineLayoutInfo resultLineLayout;

  if( !resultLayout.mLinesLayoutInfo.empty() )
  {
    resultLineLayout = *resultLayout.mLinesLayoutInfo.begin();
  }

  MergeLine( inputFirstLineLayout,
             inputLastLineLayout );

  if( !TestEqual( inputFirstLineLayout, resultLineLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}

/**
 * Removes from the \e input word the \e numberOfCharacters characters starting from the given \e position and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment. i.e. "Remove a whole group of characters. Merge".
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
  tet_printf( "%s", description.c_str() );

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

  if( !inputLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        inputWordLayout = *( *( *inputLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
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

  if( !resultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *resultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      const TextViewProcessor::WordGroupLayoutInfo& group( *line.mWordGroupsLayoutInfo.begin() );
      if( !group.mWordsLayoutInfo.empty() )
      {
        resultWordLayout = *( *( *resultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();
      }
    }
  }

  RemoveCharactersFromWord( position,
                            numberOfCharacters,
                            inputWordLayout );

  if( !TestEqual( inputWordLayout, resultWordLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}

/**
 * Removes from the \e input group of words the \e numberOfWords words starting from the given \e wordIndex and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param input The input group of words.
 * @param wordIndex Where to start to remove words.
 * @param numberOfWords The number of words to remove.
 * @param result The group of words without the removed words.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestRemoveWordsFromGroup( const std::string& description, const std::string& input, const std::size_t wordIndex, const std::size_t numberOfWords, const std::string& result, const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create layout info for the input group of words.
  Toolkit::Internal::TextView::RelayoutData relayoutData;
  TextViewProcessor::TextLayoutInfo& inputLayout( relayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray inputStyledText;
  MarkupProcessor::GetStyledTextArray( input, inputStyledText, true );

  TextViewProcessor::CreateTextInfo( inputStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData );

  // Get the input group of words
  TextViewProcessor::WordGroupLayoutInfo inputWordGroupLayout;

  if( !inputLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *inputLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      inputWordGroupLayout = *( *inputLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  // Create layout info for the result group of words.
  Toolkit::Internal::TextView::RelayoutData resultRelayoutData;
  TextViewProcessor::TextLayoutInfo& resultLayout( resultRelayoutData.mTextLayoutInfo );

  MarkupProcessor::StyledTextArray resultStyledText;
  MarkupProcessor::GetStyledTextArray( result, resultStyledText, true );

  TextViewProcessor::CreateTextInfo( resultStyledText,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     resultRelayoutData );

  // Get the result group of words.
  TextViewProcessor::WordGroupLayoutInfo resultWordGroupLayout;

  if( !resultLayout.mLinesLayoutInfo.empty() )
  {
    const TextViewProcessor::LineLayoutInfo& line( *resultLayout.mLinesLayoutInfo.begin() );
    if( !line.mWordGroupsLayoutInfo.empty() )
    {
      resultWordGroupLayout = *( *resultLayout.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();
    }
  }

  RemoveWordsFromWordGroup( wordIndex,
                            numberOfWords,
                            inputWordGroupLayout );

  if( !TestEqual( inputWordGroupLayout, resultWordGroupLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
    return false;
  }

  return true;
}


/**
 * Removes from the \e input line the \e numberOfGroups groups of words starting from the given \e groupIndex and checks the results with \e result.
 *
 * If the test fails it prints a short description and the line where this function was called.
 *
 * @param description Short description of the experiment.
 * @param input The input line.
 * @param groupIndex Where to start to remove groups of words
 * @param numberOfGroups The number of groups of words to remove.
 * @param lineHeightOffset Offset between lines.
 * @param result The line without the removed groups of words.
 * @param location Where this function has been called.
 *
 * @return \e true if the experiment is successful. Otherwise returns \e false.
 */
bool TestRemoveGroupsFromLine( const std::string& description, const std::string& input, const std::size_t groupIndex, const std::size_t numberOfGroups, const float lineHeightOffset, const std::string& result, const char* location )
{
  tet_printf( "%s", description.c_str() );

  // Create layout info for the input line.
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

  // Get the input line
  TextViewProcessor::LineLayoutInfo inputLineLayout;

  if( !inputLayout.mLinesLayoutInfo.empty() )
  {
    inputLineLayout = *inputLayout.mLinesLayoutInfo.begin();
  }

  // Create layout info for the result line.
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

  // Get the result line
  TextViewProcessor::LineLayoutInfo resultLineLayout;

  if( !resultLayout.mLinesLayoutInfo.empty() )
  {
    resultLineLayout = *resultLayout.mLinesLayoutInfo.begin();
  }

  RemoveWordGroupsFromLine( groupIndex,
                            numberOfGroups,
                            PointSize( lineHeightOffset ),
                            inputLineLayout );

  if( !TestEqual( inputLineLayout, resultLineLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );
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
                         const UpdateTextInfoOperation operation,
                         const std::string& input,
                         const std::size_t position,
                         const std::size_t numberOfCharacters,
                         const std::string& inputText,
                         const float lineHeightOffset,
                         const std::string& result,
                         const char* location )
{
  tet_printf( "%s", description.c_str() );

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
      tet_printf( "TestUpdateTextInfo: unknown update operation. %s", location );
      return false;
    }
  }

  if( !TestEqual( inputLayout, resultLayout ) )
  {
    tet_printf( "Fail. different layout info. %s", location );

    std::cout << "          result : "; Print( inputLayout );
    std::cout << " expected result : "; Print( resultLayout );
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
  style10.SetFontPointSize( PointSize( 10.f ) );
  TextStyle style12;
  style12.SetFontPointSize( PointSize( 0.f ) ); // point size is set to zero because is a default point size.

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

  // (new line character)
  wordLayout4.mSize = Size( 0.f, HEIGHT_12 );
  wordLayout4.mAscender = ASCENDER_12;
  wordLayout4.mType = TextViewProcessor::LineSeparator;
  layoutInfo12.mStyledText.mText = Text( "\n" );
  layoutInfo12.mSize.width = 0.f;
  wordLayout4.mCharactersLayoutInfo.push_back( layoutInfo12 ); // (new line char)

  // Groups

  TextViewProcessor::WordGroupLayoutInfo groupLayout1, groupLayout2;

  groupLayout1.mSize = Size( 5.f * WIDTH_10 + 7.f * WIDTH_12, HEIGHT_12 );
  groupLayout1.mAscender = ASCENDER_12;
  groupLayout1.mDirection = TextViewProcessor::LTR;
  groupLayout1.mNumberOfCharacters = 13;
  groupLayout1.mWordsLayoutInfo.push_back( wordLayout1 );
  groupLayout1.mWordsLayoutInfo.push_back( wordLayout2 );
  groupLayout1.mWordsLayoutInfo.push_back( wordLayout3 );
  groupLayout1.mWordsLayoutInfo.push_back( wordLayout4 );

  groupLayout2.mSize = Size( 0.f, HEIGHT_12 );
  groupLayout2.mAscender = ASCENDER_12;
  groupLayout2.mDirection = TextViewProcessor::LTR;
  groupLayout2.mNumberOfCharacters = 1;
  groupLayout2.mWordsLayoutInfo.push_back( wordLayout4 );

  // Lines

  TextViewProcessor::LineLayoutInfo lineLayout1, lineLayout2, lineLayout3;

  lineLayout1.mSize = Size( 5.f * WIDTH_10 + 7.f * WIDTH_12, HEIGHT_12 );
  lineLayout1.mAscender = ASCENDER_12;
  lineLayout1.mNumberOfCharacters = 13;
  lineLayout1.mWordGroupsLayoutInfo.push_back( groupLayout1 );

  lineLayout2.mSize = Size( 0.f, HEIGHT_12 );
  lineLayout2.mAscender = ASCENDER_12;
  lineLayout2.mNumberOfCharacters = 1;
  lineLayout2.mWordGroupsLayoutInfo.push_back( groupLayout2 );

  lineLayout3.mSize = Size( 0.f, HEIGHT_12 );

  // Text (layout)
  TextViewProcessor::TextLayoutInfo textLayout;

  textLayout.mWholeTextSize = Size( 5.f * WIDTH_10 + 7.f * WIDTH_12, 3.f * HEIGHT_12 );
  textLayout.mMaxWordWidth = 2.f * WIDTH_10 + 4.f * WIDTH_12;
  textLayout.mNumberOfCharacters = 14;
  textLayout.mLinesLayoutInfo.push_back( lineLayout1 );
  textLayout.mLinesLayoutInfo.push_back( lineLayout2 );
  textLayout.mLinesLayoutInfo.push_back( lineLayout3 );

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
  const std::size_t numberOfTests( 5 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
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
      std::string( "Remove from new line character to first character next line." ),
      Remove,
      std::string("Hello world\nhello world."),
      11,
      2,
      std::string(""),
      0.f,
      std::string("Hello worldello world."),
    },
    {
      std::string( "Replace style from new line character to first character next line." ),
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
    // * within the same group of characters.
    {
      std::string( "Remove within the same word, within the same group of characters" ),
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
    // * Remove whole word (merge lines)
    {
      std::string( "Remove within the same word, whole word (merge lines)" ),
      Remove,
      std::string("Hello <font size='30'>w</font>orl<font size='10'>d\nhello</font> world"),
      11,
      1,
      std::string(), // Not used.
      0.f,
      std::string( "Hello <font size='30'>w</font>orl<font size='10'>dhello</font> world" )
    },
    // * Remove whole group of words
    /* TODO check this when RTL text is working
    {
      std::string( "Remove within the same line, whole group of words (merge groups)" ),
      Remove,
      std::string("Hello world, שלום עולם, hello world"),
      10,
      15,
      std::string(), // Not used.
      0.f,
      std::string( "Hello worlello world" )
    },
    */
    // * Remove whole line
    {
      std::string( "Remove whole line" ),
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
      std::string( "Remove whole line" ),
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
  const std::size_t numberOfTests( 21 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
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

int UtcDaliTextViewSplitWordGroup(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitWordGroup : ");

  struct SplitWordGroupTest splitWordGroupTests[] =
  {
    {
      std::string( "Split word group, wordPosition 0, position 0." ),
      std::string( "<u><font size='10'>He<font size='12'>ll</font>oooo wooorld</font></u>" ),
      0,
      0,
      std::string( "" ),
      std::string( "<u><font size='10'>He<font size='12'>ll</font>oooo wooorld</font></u>" ),
    },
    {
      std::string( "Split word group, wordPosition 2, position 8." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font>" ),
      2,
      7,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font>" ),
      std::string( "" ),
    },
    {
      std::string( "Split word group, wordPosition 0, position 2." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font>" ),
      0,
      2,
      std::string( "<font size='10'>He</font>" ),
      std::string( "<font size='12'>ll</font><font size='10'>oooo wooorld</font>" ),
    },
    {
      std::string( "Split word group, wordPosition 0, position 3." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font>" ),
      0,
      3,
      std::string( "<font size='10'>He</font><font size='12'>l</font>" ),
      std::string( "<font size='12'>l</font><font size='10'>oooo wooorld</font>" ),
    },
    {
      std::string( "Split word group, wordPosition 0, position 4." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font>" ),
      0,
      4,
      std::string( "<font size='10'>He</font><font size='12'>ll</font>" ),
      std::string( "<font size='10'>oooo wooorld</font>" ),
    },
    {
      std::string( "Split word group, wordPosition 1, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font>" ),
      1,
      0,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo</font>" ),
      std::string( "<font size='10'> wooorld</font>" ),
    },
  };
  const std::size_t numberOfTests( 6 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const SplitWordGroupTest& test = splitWordGroupTests[index];

    if( !TestSplitWordGroup( test.description, test.input, test.wordPosition, test.position, test.firstResult, test.lastResult, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewSplitLine(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewSplitLine : ");

  struct SplitLineTest splitLineTests[] =
  {
    {
      std::string( "Split line, groupPosition 0, wordPosition 0, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      0,
      0,
      0,
      3.f,
      std::string( "" ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
    },
    {
      std::string( "Split line, groupPosition 2, wordPosition 2, position 4." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      2,
      2,
      4,
      0.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      std::string( "" ),
    },
    /* TODO check when RTL is working.
    {
      std::string( "Split line, groupPosition 1, wordPosition 2, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      1,
      2,
      0,
      0.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום" ),
      std::string( " עולם text text" ),
    },
    {
      std::string( "Split line, groupPosition 1, wordPosition 0, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      1,
      0,
      0,
      0.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> " ),
      std::string( "שלום עולם text text" ),
    },
    */
    {
      std::string( "Split line, groupPosition 2, wordPosition 0, position 0." ),
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם text text" ),
      2,
      0,
      0,
      6.f,
      std::string( "<font size='10'>He<font size='12'>ll</font>oooo wooorld</font> שלום עולם " ),
      std::string( "text text" ),
    },
  };
  const std::size_t numberOfTests( 3 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const SplitLineTest& test = splitLineTests[index];

    if( !TestSplitLine( test.description, test.input, test.groupPosition, test.wordPosition, test.position, test.lineHeightOffset, test.firstResult, test.lastResult, TEST_LOCATION ) )
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
  const std::size_t numberOfTests( 2 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
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
  // It test white spaces and new line characters can't be merged to other words.

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

  wordLayoutInfo01 = *( *( *textLayoutInfo01.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText02,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData02 );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo02;

  wordLayoutInfo02 = *( *( *textLayoutInfo02.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText03,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData03 );

  TextViewProcessor::WordLayoutInfo wordLayoutInfo03;

  wordLayoutInfo03 = *( *( *textLayoutInfo03.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin() ).mWordsLayoutInfo.begin();

  // Test MergeWord() asserts if white spaces or new line chars are merged.
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
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words.\"", TEST_LOCATION );
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
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words.\"", TEST_LOCATION );
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
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words.\"", TEST_LOCATION );
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
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words.\"", TEST_LOCATION );
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
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words.\"", TEST_LOCATION );
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
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWord(). ERROR: White spaces or new line characters can't be merged with other words.\"", TEST_LOCATION );
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

int UtcDaliTextViewMergeGroup01(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeGroup01 : ");

  struct MergeWordGroupsTest mergeWordGroupssTests[] =
  {
    {
      std::string( "Merge a void first group." ),
      std::string( "" ),
      std::string( "Hello world" ),
      std::string( "Hello world" ),
    },
    {
      std::string( "Merge a void last group." ),
      std::string( "Hello world" ),
      std::string( "" ),
      std::string( "Hello world" ),
    },
    {
      std::string( "Merge groups and merge last and first words." ),
      std::string( "Hello wor" ),
      std::string( "ld, hello world" ),
      std::string( "Hello world, hello world" ),
    },
    {
      std::string( "Merge groups and don't merge last and first words." ),
      std::string( "Hello world, " ),
      std::string( "hello world" ),
      std::string( "Hello world, hello world" )
    },
  };
  const std::size_t numberOfTests( 4 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const MergeWordGroupsTest& test = mergeWordGroupssTests[index];

    if( !TestMergeGroupsOfWords( test.description, test.inputFirst, test.inputLast, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewMergeGroup02(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeGroup02 : ");

  Toolkit::Internal::TextView::RelayoutData relayoutData01;
  Toolkit::Internal::TextView::RelayoutData relayoutData02;
  Toolkit::Internal::TextView::RelayoutData relayoutData03;
  TextViewProcessor::TextLayoutInfo& textLayoutInfo01( relayoutData01.mTextLayoutInfo );
  TextViewProcessor::TextLayoutInfo& textLayoutInfo02( relayoutData02.mTextLayoutInfo );
  TextViewProcessor::TextLayoutInfo& textLayoutInfo03( relayoutData03.mTextLayoutInfo );

  std::string text01( "Hello \n" );
  std::string text02( "world" );
  std::string text03( "السلام عليكم" );
  MarkupProcessor::StyledTextArray styledText01;
  MarkupProcessor::StyledTextArray styledText02;
  MarkupProcessor::StyledTextArray styledText03;
  MarkupProcessor::GetStyledTextArray( text01, styledText01, true );
  MarkupProcessor::GetStyledTextArray( text02, styledText02, true );
  MarkupProcessor::GetStyledTextArray( text03, styledText03, true );

  TextViewProcessor::CreateTextInfo( styledText01,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData01 );

  TextViewProcessor::WordGroupLayoutInfo wordGroupLayoutInfo01;

  wordGroupLayoutInfo01 = *( *textLayoutInfo01.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText02,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData02 );

  TextViewProcessor::WordGroupLayoutInfo wordGroupLayoutInfo02;

  wordGroupLayoutInfo02 = *( *textLayoutInfo02.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText03,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData03 );

  TextViewProcessor::WordGroupLayoutInfo wordGroupLayoutInfo03;

  wordGroupLayoutInfo03 = *( *textLayoutInfo03.mLinesLayoutInfo.begin() ).mWordGroupsLayoutInfo.begin();

  bool assert1 = false;
  bool assert2 = false;

  try
  {
    MergeWordGroup( wordGroupLayoutInfo01,
                    wordGroupLayoutInfo02 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWordGroup(). ERROR: A group of words can't be merged to another group which finishes with a new line character.\"", TEST_LOCATION );
    assert1 = true;
  }

  try
  {
    MergeWordGroup( wordGroupLayoutInfo03,
                    wordGroupLayoutInfo02 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeWordGroup(). ERROR: groups with different direction can't be merged.\"", TEST_LOCATION );
    assert2 = true;
  }

  if( assert1 && assert2 )
  {
    tet_result( TET_PASS );
  }
  else
  {
    tet_result( TET_FAIL );
  }
  END_TEST;
}

int UtcDaliTextViewMergeLine01(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeLine01 : ");

  struct MergeLinesTest mergeLinesTests[] =
  {
    {
      std::string( "Merge a void first line." ),
      std::string( "" ),
      std::string( "Hello world, this is a whole line" ),
      2.f,
      std::string( "Hello world, this is a whole line" )
    },
    {
      std::string( "Merge a void last line." ),
      std::string( "Hello world, this is a whole line" ),
      std::string( "" ),
      0.f,
      std::string( "Hello world, this is a whole line" )
    },
    /* TODO: check when RTL text is working.
    {
      std::string( "Merge lines and merge last and first groups" ),
      std::string( "Hello world, שלום" ),
      std::string( " עולם, hello world." ),
      6.f,
      std::string( "Hello world, שלום עולם, hello world." )
    },
    {
      std::string( "Merge lines and don't merge last and first words." ),
      std::string( "Hello world, " ),
      std::string( "שלום עולם, hello world." ),
      3.f,
      std::string( "Hello world, שלום עולם, hello world." )
    },
    */
    {
      std::string( "Merge lines. Don't merge words" ),
      std::string( "Hello world," ),
      std::string( " this is a whole line" ),
      0.f,
      std::string( "Hello world, this is a whole line" )
    },
    {
      std::string( "Merge lines. Merge words" ),
      std::string( "Hello world, th" ),
      std::string( "is is a whole line" ),
      0.f,
      std::string( "Hello world, this is a whole line" )
    },
  };
  const std::size_t numberOfTests( 4 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const MergeLinesTest& test = mergeLinesTests[index];

    if( !TestMergeLines( test.description, test.inputFirst, test.inputLast, test.lineHeightOffset, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewMergeLine02(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewMergeLine02 : ");

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

  TextViewProcessor::LineLayoutInfo lineLayoutInfo01;

  lineLayoutInfo01 = *textLayoutInfo01.mLinesLayoutInfo.begin();

  TextViewProcessor::CreateTextInfo( styledText02,
                                     DEFAULT_LAYOUT_PARAMETERS,
                                     relayoutData02 );

  TextViewProcessor::LineLayoutInfo lineLayoutInfo02;

  lineLayoutInfo02 = *textLayoutInfo02.mLinesLayoutInfo.begin();

  bool assert1 = false;

  try
  {
    MergeLine( lineLayoutInfo01,
               lineLayoutInfo02 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    DALI_TEST_EQUALS( e.mCondition, "!\"TextViewProcessor::MergeLine(). ERROR: A line can't be merged to another line which finishes with a new line character.\"", TEST_LOCATION );
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
  const std::size_t numberOfTests( 11 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
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

int UtcDaliTextViewRemoveWordsFromGroup(void)
{
  // Note: Currently RemoveWordsFromWordGroup() function is only used to remove a number of words from the beginning, or
  //       from a given index to the end. RemoveWordsFromWordGroup() doesn't merge words (if a white space is removed) so
  //       tehere isn't any TET case to cover these cases. To be done if needed.

  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewRemoveWordsFromGroup : ");
  struct RemoveWordsFromGroupTest removeWordsFromGroupTests[] =
  {
    {
      std::string( "Delete 0 words." ),
      std::string( "Hello world, hello world" ),
      3,
      0,
      std::string( "Hello world, hello world" ),
    },
    {
      std::string( "Delete some words in the middle. Don't merge words" ),
      std::string( "<font size='10'>Hel</font><font size='20'>lo wo</font><font size='30'>rld, hello world</font>" ),
      1,
      4,
      std::string( "<font size='10'>Hel</font><font size='20'>lo</font><font size='30'> world</font>" ),
    },
    {
      std::string( "Delete words up to the end" ),
      std::string( "<font size='10'>Hel</font><font size='20'>lo wo</font><font size='30'>rld, hello world</font>" ),
      5,
      2,
      std::string( "<font size='10'>Hel</font><font size='20'>lo wo</font><font size='30'>rld, hello</font>" ),
    },
    {
      std::string( "Delete words from the beginning." ),
      std::string( "Hello world, hello world" ),
      0,
      3,
      std::string( " hello world" ),
    },
  };
  const std::size_t numberOfTests( 4 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const RemoveWordsFromGroupTest& test = removeWordsFromGroupTests[index];

    if( !TestRemoveWordsFromGroup( test.description, test.input, test.wordIndex, test.numberOfWords, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}

int UtcDaliTextViewRemoveGroupsFromLine(void)
{
  // Note: Currently RemoveWordGroupsFromLine() function is only used to remove a number of group of words from the beginning, or
  //       from a given index to the end. RemoveWordGroupsFromLine() doesn't merge groups of words (if a whole group of words is removed) so
  //       tehere isn't any TET case to cover these cases. To be done if needed.

  ToolkitTestApplication application;

  tet_infoline("UtcDaliTextViewRemoveGroupsFromLine : ");
  struct RemoveGroupsFromLineTest removeGroupsFromLineTests[] =
  {
    {
      std::string( "Delete 0 groups of words." ),
      std::string( "Hello hello, שלום עולם hello hello" ),
      1,
      0,
      2.f,
      std::string( "Hello hello, שלום עולם hello hello" ),
    },
    {
      std::string( "Delete from the middle to the end." ),
      std::string( "Hello hello, שלום עולם hello hello" ),
      1,
      2,
      0.f,
      std::string( "Hello hello, " ),
    },
    {
      std::string( "Delete from the beginning to the middle." ),
      std::string( "Hello hello, שלום עולם hello hello" ),
      0,
      2,
      6.f,
      std::string( "hello hello" ),
    },
  };
  const std::size_t numberOfTests( 3 );

  for( std::size_t index = 0; index < numberOfTests; ++index )
  {
    const RemoveGroupsFromLineTest& test = removeGroupsFromLineTests[index];

    if( !TestRemoveGroupsFromLine( test.description, test.input, test.groupIndex, test.numberOfGroups, test.lineHeightOffset, test.result, TEST_LOCATION ) )
    {
      tet_result( TET_FAIL );
    }
  }

  tet_result( TET_PASS );
  END_TEST;
}
