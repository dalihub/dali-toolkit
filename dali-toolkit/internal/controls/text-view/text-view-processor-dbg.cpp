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

// FILE HEADER
#include <dali-toolkit/internal/controls/text-view/text-view-processor-dbg.h>

// EXTERNAL INCLUDES
#include <iostream>

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

#if defined(DEBUG_ENABLED)
Debug::Filter* gTextViewProcessorLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXT_VIEW_PROCESSOR");
#endif

void dbgPrint( const WordLayoutInfo& word )
{
  std::cout << "       size " << word.mSize << std::endl;
  std::cout << "   ascender " << word.mAscender << std::endl;
  std::cout << "       type ";
  switch( word.mType )
  {
    case NoSeparator:
    {
      std::cout << "NoSeparator" << std::endl;
      break;
    }
    case ParagraphSeparator:
    {
      std::cout << "ParagraphSeparator" << std::endl;
      break;
    }
    case WordSeparator:
    {
      std::cout << "WordSeparator" << std::endl;
      break;
    }
  }
  std::cout << " first char " << word.mFirstCharacter << std::endl;
  std::cout << "   num char " << word.mCharactersLayoutInfo.size() << std::endl;
  for( CharacterLayoutInfoContainer::const_iterator characterIt = word.mCharactersLayoutInfo.begin(), endCharacterIt = word.mCharactersLayoutInfo.end();
       characterIt != endCharacterIt;
       ++characterIt )
  {
    const CharacterLayoutInfo& character( *characterIt );

    std::cout << "[" << character.mSize << std::endl;
    std::cout << " ascender " << character.mAscender << std::endl;
  }
}

void dbgPrint( const ParagraphLayoutInfo& paragraph )
{
  std::cout << "< ";
  std::cout << "  text : [" << paragraph.mText.GetText() << "]" << std::endl;
  std::cout << "  number of styles : " << paragraph.mTextStyles.Count() << std::endl;
  std::cout << paragraph.mSize;
  for( WordLayoutInfoContainer::const_iterator wordIt = paragraph.mWordsLayoutInfo.begin(), endWordIt = paragraph.mWordsLayoutInfo.end();
       wordIt != endWordIt;
       ++wordIt )
  {
    dbgPrint( *wordIt );
  }
  std::cout << " >";
  std::cout << std::endl;
}

void dbgPrint( const TextLayoutInfo& textInfo )
{
  std::cout << "||" << std::endl;
  std::cout << textInfo.mWholeTextSize;
  std::cout << textInfo.mNumberOfCharacters;
  for( ParagraphLayoutInfoContainer::const_iterator it = textInfo.mParagraphsLayoutInfo.begin(), endIt = textInfo.mParagraphsLayoutInfo.end();
       it != endIt;
       ++it )
  {
    dbgPrint( *it );
  }
  std::cout << "||" << std::endl;
}

void dbgPrint( const TextStyle& style )
{
  std::cout << "               font name : " << style.GetFontName() << std::endl;
  std::cout << "              font style : " << style.GetFontStyle() << std::endl;
  std::cout << "         font point size : " << style.GetFontPointSize() << std::endl;
  std::cout << "                  weight : " << style.GetWeight() << std::endl;
  std::cout << "              text color : " << style.GetTextColor() << std::endl;
  std::cout << "                 italics : " << style.IsItalicsEnabled() << std::endl;
  std::cout << "           italics angle : " << style.GetItalicsAngle() << std::endl;
  std::cout << "               underline : " << style.IsUnderlineEnabled() << std::endl;
  std::cout << "     underline thickness : " << style.GetUnderlineThickness() << std::endl;
  std::cout << "      underline position : " << style.GetUnderlinePosition() << std::endl;
  std::cout << "                  shadow : " << style.IsShadowEnabled() << std::endl;
  std::cout << "            shadow color : " << style.GetShadowColor() << std::endl;
  std::cout << "           shadow offset : " << style.GetShadowOffset() << std::endl;
  std::cout << "             shadow size : " << style.GetShadowSize() << std::endl;
  std::cout << "                    glow : " << style.IsGlowEnabled() << std::endl;
  std::cout << "              glow color : " << style.GetGlowColor() << std::endl;
  std::cout << "          glow intensity : " << style.GetGlowIntensity() << std::endl;
  std::cout << "             smooth edge : " << style.GetSmoothEdge() << std::endl;
  std::cout << "                 outline : " << style.IsOutlineEnabled() << std::endl;
  std::cout << "           outline color : " << style.GetOutlineColor() << std::endl;
  std::cout << "       outline thickness : " << style.GetOutlineThickness() << std::endl;
  std::cout << "                gradient : " << style.IsGradientEnabled() << std::endl;
  std::cout << "          gradient color : " << style.GetGradientColor() << std::endl;
  std::cout << "    gradient start point : " << style.GetGradientStartPoint() << std::endl;
  std::cout << "      gradient end point : " << style.GetGradientEndPoint() << std::endl;
  std::cout << "    is font name default : " << style.IsFontNameDefault() << std::endl;
  std::cout << "   is font style default : " << style.IsFontStyleDefault() << std::endl;
  std::cout << "    is font size default : " << style.IsFontSizeDefault() << std::endl;
  std::cout << "   is text color default : " << style.IsTextColorDefault() << std::endl;
  std::cout << "  is font weight default : " << style.IsFontWeightDefault() << std::endl;
  std::cout << "  is smooth edge default : " << style.IsSmoothEdgeDefault() << std::endl;
  std::cout << "      is italics default : " << style.IsItalicsDefault() << std::endl;
  std::cout << "    is underline default : " << style.IsUnderlineDefault() << std::endl;
  std::cout << "       is shadow default : " << style.IsShadowDefault() << std::endl;
  std::cout << "         is glow default : " << style.IsGlowDefault() << std::endl;
  std::cout << "      is outline default : " << style.IsOutlineDefault() << std::endl;
  std::cout << "     is gradient default : " << style.IsGradientDefault() << std::endl;
}

void dbgPrint( const TextInfoIndices& indices )
{
  std::cout << "          paragraph : " << indices.mParagraphIndex << std::endl;
  std::cout << "               word : " << indices.mWordIndex << std::endl;
  std::cout << "               char : " << indices.mCharacterIndex << std::endl;
  std::cout << "  char in paragraph : " << indices.mCharacterParagraphIndex << std::endl;
}

void dbgPrint( const MarkupProcessor::StyledTextArray& textArray )
{
  for( MarkupProcessor::StyledTextArray::const_iterator it = textArray.begin(), endIt = textArray.end(); it != endIt; ++it )
  {
    const MarkupProcessor::StyledText& text( *it );

    std::cout << text.mText.GetText();
  }
}

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
