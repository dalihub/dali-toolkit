/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the LayoutEngine methods.

//////////////////////////////////////////////////////////

namespace
{
const std::string DEFAULT_FONT_DIR("/resources/fonts");

struct LayoutTextData
{
  std::string          description;
  std::string          text;
  Size                 textArea;
  unsigned int         numberOfFonts;
  FontDescriptionRun*  fontDescriptions;
  Size                 layoutSize;
  unsigned int         totalNumberOfGlyphs;
  float*               positions;
  unsigned int         numberOfLines;
  LineRun*             lines;
  Layout::Engine::Type layout;
  unsigned int         startIndex;
  unsigned int         numberOfGlyphs;
  bool                 ellipsis : 1;
  bool                 updated : 1;
  float                characterSpacing;
};

void Print(const LineRun& line)
{
  std::cout << "        glyph run, index : " << line.glyphRun.glyphIndex << ", num glyphs : " << line.glyphRun.numberOfGlyphs << std::endl;
  std::cout << "    character run, index : " << line.characterRun.characterIndex << ", num chars : " << line.characterRun.numberOfCharacters << std::endl;
  std::cout << "                   width : " << floor(line.width) << std::endl;
  std::cout << "                ascender : " << line.ascender << std::endl;
  std::cout << "               descender : " << line.descender << std::endl;
  std::cout << "             extraLength : " << line.extraLength << std::endl;
  std::cout << "         alignmentOffset : " << line.alignmentOffset << std::endl;
  std::cout << "               direction : " << line.direction << std::endl;
  std::cout << "                ellipsis : " << line.ellipsis << std::endl;
}

bool LayoutTextTest(const LayoutTextData& data)
{
  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHindiRegular.ttf");

  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  if(0u != data.numberOfFonts)
  {
    fontDescriptionRuns.Insert(fontDescriptionRuns.End(),
                               data.fontDescriptions,
                               data.fontDescriptions + data.numberOfFonts);
  }

  LayoutOptions options;
  options.align = false;
  CreateTextModel(data.text,
                  data.textArea,
                  fontDescriptionRuns,
                  options,
                  layoutSize,
                  textModel,
                  metrics,
                  false,
                  LineWrap::WORD,
                  false,
                  Toolkit::DevelText::EllipsisPosition::END,
                  0.0f, // lineSpacing
                  data.characterSpacing);

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  // 2) Clear the layout.
  Vector<LineRun>& lines = visualModel->mLines;

  const Length     numberOfCharacters = logicalModel->mText.Count();
  const bool       isLastNewParagraph = (0u == numberOfCharacters) ? false : TextAbstraction::IsNewParagraph(*(logicalModel->mText.Begin() + (numberOfCharacters - 1u)));
  const GlyphIndex lastGlyphIndex     = data.startIndex + data.numberOfGlyphs - 1u;
  const bool       removeLastLine     = isLastNewParagraph && (lastGlyphIndex + 1u == visualModel->mGlyphs.Count());

  LineIndex startRemoveIndex = 0u;

  if(0u != lines.Count())
  {
    startRemoveIndex         = lines.Count();
    LineIndex endRemoveIndex = startRemoveIndex;
    ClearGlyphRuns(data.startIndex,
                   lastGlyphIndex + (removeLastLine ? 1u : 0u),
                   lines,
                   startRemoveIndex,
                   endRemoveIndex);

    // Update the character runs of the lines.
    const CharacterIndex* const glyphsToCharactersBuffer = visualModel->mGlyphsToCharacters.Begin();
    const Length* const         charactersPerGlyph       = visualModel->mCharactersPerGlyph.Begin();
    const CharacterIndex        startCharacterIndex      = *(glyphsToCharactersBuffer + data.startIndex);
    const CharacterIndex        lastCharacterIndex       = *(glyphsToCharactersBuffer + lastGlyphIndex) + *(charactersPerGlyph + lastGlyphIndex) - 1u;
    ClearCharacterRuns(startCharacterIndex,
                       lastCharacterIndex + (removeLastLine ? 1u : 0u),
                       lines,
                       startRemoveIndex,
                       endRemoveIndex);

    lines.Erase(lines.Begin() + startRemoveIndex,
                lines.Begin() + endRemoveIndex);
  }

  Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;

  glyphPositions.Erase(glyphPositions.Begin() + data.startIndex,
                       glyphPositions.Begin() + data.startIndex + data.numberOfGlyphs);

  // 3) Layout
  Layout::Engine engine;
  engine.SetMetrics(metrics);
  engine.SetLayout(data.layout);

  textModel->mHorizontalAlignment   = Text::HorizontalAlignment::BEGIN;
  textModel->mLineWrapMode          = LineWrap::WORD;
  textModel->mIgnoreSpacesAfterText = true;
  //textModel->mMatchSystemLanguageDirection = false;
  Layout::Parameters layoutParameters(data.textArea,
                                      textModel);

  layoutParameters.isLastNewParagraph = isLastNewParagraph;

  // The initial glyph and the number of glyphs to layout.
  layoutParameters.startGlyphIndex        = data.startIndex;
  layoutParameters.numberOfGlyphs         = data.numberOfGlyphs;
  layoutParameters.startLineIndex         = startRemoveIndex;
  layoutParameters.estimatedNumberOfLines = logicalModel->mParagraphInfo.Count();

  layoutSize = Vector2::ZERO;

  bool       isAutoScroll                   = false;
  bool       isAutoScrollMaxTextureExceeded = false;
  bool       isHiddenInputEnabled           = false;
  const bool updated                        = engine.LayoutText(layoutParameters,
                                         layoutSize,
                                         data.ellipsis,
                                         isAutoScroll,
                                         isAutoScrollMaxTextureExceeded,
                                         isHiddenInputEnabled,
                                         DevelText::EllipsisPosition::END);

  // 4) Compare the results.

  if(updated != data.updated)
  {
    std::cout << "  Different updated bool : " << updated << ", expected : " << data.updated << std::endl;
    return false;
  }

  if(layoutSize != data.layoutSize)
  {
    std::cout << "  Different layout size : " << layoutSize << ", expected : " << data.layoutSize << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < data.totalNumberOfGlyphs; ++index)
  {
    const Vector2& position = *(glyphPositions.Begin() + index);

    if(fabsf(std::round(position.x) - std::round(*(data.positions + 2u * index))) > Math::MACHINE_EPSILON_1000)
    {
      std::cout << "  Different position for glyph " << index << " x : " << position.x << ", expected : " << *(data.positions + 2u * index) << std::endl;
      return false;
    }
    if(fabsf(position.y - *(data.positions + 2u * index + 1u)) > Math::MACHINE_EPSILON_1000)
    {
      std::cout << "  Different position for glyph " << index << " y : " << position.y << ", expected : " << *(data.positions + 2u * index + 1u) << std::endl;
      return false;
    }
  }

  if(lines.Count() != data.numberOfLines)
  {
    std::cout << "  Different number of lines : " << lines.Count() << ", expected : " << data.numberOfLines << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < data.numberOfLines; ++index)
  {
    const LineRun& line         = *(lines.Begin() + index);
    const LineRun& expectedLine = *(data.lines + index);

    if(line.glyphRun.glyphIndex != expectedLine.glyphRun.glyphIndex)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }
    if(line.glyphRun.numberOfGlyphs != expectedLine.glyphRun.numberOfGlyphs)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }

    if(line.characterRun.characterIndex != expectedLine.characterRun.characterIndex)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }
    if(line.characterRun.numberOfCharacters != expectedLine.characterRun.numberOfCharacters)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }

    if(fabsf(floor(line.width) - expectedLine.width) > Math::MACHINE_EPSILON_1)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }

    if(fabsf(line.ascender - expectedLine.ascender) > Math::MACHINE_EPSILON_1)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }

    if(fabsf(line.descender - expectedLine.descender) > Math::MACHINE_EPSILON_1)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }

    if(fabsf(line.extraLength - expectedLine.extraLength) > Math::MACHINE_EPSILON_1)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }

    if(line.ellipsis != expectedLine.ellipsis)
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print(line);
      std::cout << "  expected" << std::endl;
      Print(expectedLine);
      return false;
    }
    // Do not compare the alignment offset as it's not calculated in the layout.
    // Do not compare the line direction as it's not set in the layout.
  }

  return true;
}

////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////
//
// UtcDaliTextCharacterSpacingSingleLineTextArea1
// UtcDaliTextCharacterSpacingSingleLineTextArea2
// UtcDaliTextCharacterSpacingSingleLineTextArea3
// UtcDaliTextCharacterSpacingMultilineText1
// UtcDaliTextCharacterSpacingMultilineText2
//
//////////////////////////////////////////////////////////

int UtcDaliTextCharacterSpacingSingleLineTextArea1(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSpacingSingleLineTextArea1");

  // Will layout the text in single line as it can be scrolled.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun.familyLength                    = fontFamily.size();
  fontDescriptionRun.familyName                      = new char[fontDescriptionRun.familyLength];
  memcpy(fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength);
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined  = false;
  fontDescriptionRun.slantDefined  = false;
  fontDescriptionRun.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun);
  Size textArea(1.f, 1.f);

  //******* characterSpacing = 0.0f *******//
  float positions[] =
   {
    0.f, -12.f, 10.f, -9.f, 19.f, -13.f, 23.f, -13.f, 26.f, -9.f, 35.f, -0.f, 39.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f
   };
  Size layoutSize(79.f, 20.f);
  //************************************//

  struct LineRun line =
    {
      {0u, 11u},
      {0u, 11u},
      79.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line);

  LayoutTextData data =
    {
      "Layout text in a small area",
      "Hello world",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      11u,
      positions,
      1u,
      lines.Begin(),
      Layout::Engine::SINGLE_LINE_BOX,
      0u,
      11u,
      false,
      true,
      0.0f};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSpacingSingleLineTextArea2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSpacingSingleLineTextArea2");

  // Will layout the text in single line as it can be scrolled.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun.familyLength                    = fontFamily.size();
  fontDescriptionRun.familyName                      = new char[fontDescriptionRun.familyLength];
  memcpy(fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength);
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined  = false;
  fontDescriptionRun.slantDefined  = false;
  fontDescriptionRun.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun);
  Size textArea(1.f, 1.f);

  //******* characterSpacing = 2.0f *******//
  float positions[] =
   {
     0.f, -12.f, 12.f, -9.f, 23.f, -13.f, 29.f, -13.f, 34.f, -9.f, 45.f, -0.f, 51.f, -9.f, 65.f, -9.f, 77.f, -9.f, 85.f, -13.f, 90.f, -13.f
   };
  Size layoutSize(99.f, 20.f);
  //************************************//

  struct LineRun line =
    {
      {0u, 11u},
      {0u, 11u},
      99.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line);

  LayoutTextData data =
    {
      "Layout text in a small area",
      "Hello world",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      11u,
      positions,
      1u,
      lines.Begin(),
      Layout::Engine::SINGLE_LINE_BOX,
      0u,
      11u,
      false,
      true,
      2.0f};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSpacingSingleLineTextArea3(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSpacingSingleLineTextArea3");

  // Will layout the text in single line as it can be scrolled.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun.familyLength                    = fontFamily.size();
  fontDescriptionRun.familyName                      = new char[fontDescriptionRun.familyLength];
  memcpy(fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength);
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined  = false;
  fontDescriptionRun.slantDefined  = false;
  fontDescriptionRun.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun);
  Size textArea(1.f, 1.f);

  //******* characterSpacing = 4.0f *******//
  float positions[] =
   {
     0.f, -12.f, 14.f, -9.f, 27.f, -13.f, 35.f, -13.f, 42.f, -9.f, 55.f, -0.f, 63.f, -9.f, 79.f, -9.f, 93.f, -9.f, 103.f, -13.f, 110.f, -13.f
   };
  Size layoutSize(119.f, 20.f);
  //************************************//

  struct LineRun line =
    {
      {0u, 11u},
      {0u, 11u},
      119.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line);

  LayoutTextData data =
    {
      "Layout text in a small area",
      "Hello world",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      11u,
      positions,
      1u,
      lines.Begin(),
      Layout::Engine::SINGLE_LINE_BOX,
      0u,
      11u,
      false,
      true,
      4.0f};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSpacingMultilineText1(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSpacingMultilineText1");

  // Layout some lines of left to right text.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun1.familyLength                    = fontFamily.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex     = 18u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 31u;
  fontDescriptionRun2.familyLength                    = fontFamily.size();
  fontDescriptionRun2.familyName                      = new char[fontDescriptionRun2.familyLength];
  memcpy(fontDescriptionRun2.familyName, fontFamily.c_str(), fontDescriptionRun2.familyLength);
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined  = false;
  fontDescriptionRun2.slantDefined  = false;
  fontDescriptionRun2.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun1);
  fontDescriptionRuns.PushBack(fontDescriptionRun2);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(96.f, 97.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 19.f, -13.f, 23.f, -13.f, 26.f, -9.f, 35.f, -0.f, 39.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f, 0.f, -13.f, 9.f, -9.f, 18.f, -9.f, 31.f, -9.f, 40.f, -2.f, 44.f, -12.f, 0.f, -12.f, 8.f, -9.f, 18.f, -9.f, 27.f, -9.f, 38.f, -9.f, 47.f, -11.f, 53.f, -0.f, 0.f, -12.f, 11.f, -12.f, 14.f, -12.f, 25.f, -9.f, 36.f, -9.f, 41.f, -9.f, 52.f, -9.f, 61.f, -11.f, 67.f, -0.f, 0.f, -12.f, 4.f, -12.f, 8.f, -9.f, 17.f, -9.f, 27.f, -9.f, 35.f, -0.f, 40.f, -9.f, 50.f, -12.f, 56.f, -0.f, 61.f, -11.f, 67.f, -9.f, 77.f, -9.f, 86.f, -11.f, 93.f, -2.f
    };
  struct LineRun line0 =
    {
      {0u, 12u},
      {0u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line1 =
    {
      {12u, 6u},
      {12u, 6u},
      43.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line2 =
    {
      {18u, 7u},
      {18u, 7u},
      53.f,
      15.f,
      -4.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line3 =
    {
      {25u, 9u},
      {25u, 10u},
      67.f,
      15.f,
      -4.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line4 =
    {
      {34u, 14u},
      {35u, 14u},
      96.f,
      15.f,
      -4.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);
  lines.PushBack(line2);
  lines.PushBack(line3);
  lines.PushBack(line4);

  LayoutTextData data =
    {
      "Layout simple multiline text",
      "Hello world demo.\n"
      "Layout different lines of text.",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      48u,
      positions,
      5u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      48u,
      false,
      true,
      0.0f};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSpacingMultilineText2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSpacingMultilineText2");

  // Layout some lines of left to right text.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun1.familyLength                    = fontFamily.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex     = 18u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 31u;
  fontDescriptionRun2.familyLength                    = fontFamily.size();
  fontDescriptionRun2.familyName                      = new char[fontDescriptionRun2.familyLength];
  memcpy(fontDescriptionRun2.familyName, fontFamily.c_str(), fontDescriptionRun2.familyLength);
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined  = false;
  fontDescriptionRun2.slantDefined  = false;
  fontDescriptionRun2.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun1);
  fontDescriptionRuns.PushBack(fontDescriptionRun2);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(99.f, 116.f);
  float positions[] =
    {
      0.f, -12.f, 12.f, -9.f, 23.f, -13.f, 29.f, -13.f, 34.f, -9.f, 45.f, -0.f, 51.f, -9.f, 65.f, -9.f, 77.f, -9.f, 85.f, -13.f, 90.f, -13.f, 101.f, -0.f, 0.f, -13.f, 11.f, -9.f, 22.f, -9.f, 37.f, -9.f, 48.f, -2.f, 54.f, -12.f, 0.f, -12.f, 10.f, -9.f, 22.f, -9.f, 33.f, -9.f, 46.f, -9.f, 57.f, -11.f, 65.f, -0.f, 0.f, -12.f, 13.f, -12.f, 18.f, -12.f, 31.f, -9.f, 44.f, -9.f, 51.f, -9.f, 64.f, -9.f, 75.f, -11.f, 83.f, -0.f, 0.f, -12.f, 6.f, -12.f, 12.f, -9.f, 23.f, -9.f, 35.f, -9.f, 45.f, -0.f, 52.f, -9.f, 64.f, -12.f, 72.f, -0.f, 0.f, -11.f, 8.f, -9.f, 20.f, -9.f, 31.f, -11.f, 40.f, -2.f
    };
  struct LineRun line0 =
    {
      {0u, 12u},
      {0u, 12u},
      99.f,
      15.f,
      -5.f,
      6.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line1 =
    {
      {12u, 6u},
      {12u, 6u},
      51.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line2 =
    {
      {18u, 7u},
      {18u, 7u},
      63.f,
      15.f,
      -4.f,
      7.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line3 =
    {
      {25u, 9u},
      {25u, 10u},
      81.f,
      15.f,
      -4.f,
      7.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line4 =
    {
      {34u, 9u},
      {35u, 9u},
      70.f,
      15.f,
      -4.f,
      7.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line5 =
    {
      {43u, 5u},
      {44u, 5u},
      43.f,
      15.f,
      -4.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);
  lines.PushBack(line2);
  lines.PushBack(line3);
  lines.PushBack(line4);
  lines.PushBack(line5);

  LayoutTextData data =
    {
      "Layout simple multiline text",
      "Hello world demo.\n"
      "Layout different lines of text.",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      48u,
      positions,
      6u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      48u,
      false,
      true,
      2.0f};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}
