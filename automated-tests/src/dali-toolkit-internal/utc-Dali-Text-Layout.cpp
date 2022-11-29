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
  std::string                       description;
  std::string                       text;
  Size                              textArea;
  unsigned int                      numberOfFonts;
  FontDescriptionRun*               fontDescriptions;
  Size                              layoutSize;
  unsigned int                      totalNumberOfGlyphs;
  float*                            positions;
  unsigned int                      numberOfLines;
  LineRun*                          lines;
  Layout::Engine::Type              layout;
  unsigned int                      startIndex;
  unsigned int                      numberOfGlyphs;
  bool                              ellipsis : 1;
  DevelText::EllipsisPosition::Type ellipsisPosition;
  bool                              updated : 1;
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
                  0.0f  // characterSpacing
  );

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
                                         data.ellipsisPosition);

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

    if(fabsf(std::round(position.x) - *(data.positions + 2u * index)) > Math::MACHINE_EPSILON_1000)
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

//////////////////////////////////////////////////////////

struct AlignData
{
  std::string                     description;
  std::string                     text;
  Size                            textArea;
  unsigned int                    numberOfFonts;
  FontDescriptionRun*             fontDescriptions;
  Text::HorizontalAlignment::Type horizontalAlignment;
  Text::VerticalAlignment::Type   verticalAlignment;
  unsigned int                    startIndex;
  unsigned int                    numberOfCharacters;
  unsigned int                    numberOfLines;
  float*                          lineOffsets;
  Dali::LayoutDirection::Type     layoutDirection;
  bool                            matchSystemLanguageDirection;
};

bool AlignTest(const AlignData& data)
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
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  // Call the Align method.
  Layout::Engine engine;
  engine.SetMetrics(metrics);

  float alignmentOffset = 0.f;
  engine.Align(data.textArea,
               data.startIndex,
               data.numberOfCharacters,
               data.horizontalAlignment,
               visualModel->mLines,
               alignmentOffset,
               data.layoutDirection,
               data.matchSystemLanguageDirection);

  // Compare results.
  if(data.numberOfLines != visualModel->mLines.Count())
  {
    std::cout << "  Different number of lines : " << visualModel->mLines.Count() << ", expected : " << data.numberOfLines << std::endl;
    return false;
  }

  const LineRun* const linesBuffer = visualModel->mLines.Begin();
  for(unsigned int index = 0u; index < data.numberOfLines; ++index)
  {
    const LineRun& line = *(linesBuffer + index);

    if(line.alignmentOffset != *(data.lineOffsets + index))
    {
      std::cout << "  different line offset for index " << index << " : " << line.alignmentOffset << ", expected : " << *(data.lineOffsets + index) << std::endl;
      return false;
    }
  }
  return true;
}

} // namespace

//////////////////////////////////////////////////////////
//
// UtcDaliTextLayoutSetGetLayout
// UtcDaliTextLayoutSetGetTextEllipsisEnabled
// UtcDaliTextLayoutSetGetHorizontalAlignment
// UtcDaliTextLayoutSetGetVerticalAlignment
// UtcDaliTextLayoutSetGetCursorWidth
// UtcDaliTextLayoutNoText
// UtcDaliTextLayoutSmallTextArea01
// UtcDaliTextLayoutSmallTextArea02
// UtcDaliTextLayoutMultilineText01
// UtcDaliTextLayoutMultilineText02
// UtcDaliTextLayoutMultilineText03
// UtcDaliTextLayoutMultilineText04
// UtcDaliTextLayoutMultilineText05
// UtcDaliTextLayoutMultilineText06
// UtcDaliTextUpdateLayout01
// UtcDaliTextUpdateLayout02
// UtcDaliTextUpdateLayout03
// UtcDaliTextLayoutEllipsis01
// UtcDaliTextLayoutEllipsis02
// UtcDaliTextLayoutEllipsis03
// UtcDaliTextLayoutEllipsis04
// UtcDaliTextLayoutEllipsis04
// UtcDaliTextReorderLayout01
// UtcDaliTextReorderLayout02
// UtcDaliTextReorderLayout03
// UtcDaliTextReorderLayout04
// UtcDaliTextAlign01
// UtcDaliTextAlign02
// UtcDaliTextAlign03
// UtcDaliTextAlign04
// UtcDaliTextAlign05
// UtcDaliTextAlign06
// UtcDaliTextAlign07
// UtcDaliTextAlign08
// UtcDaliTextAlign09
//
//////////////////////////////////////////////////////////

int UtcDaliTextLayoutSetGetLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetLayout");

  Layout::Engine engine;

  DALI_TEST_CHECK(Layout::Engine::SINGLE_LINE_BOX == engine.GetLayout());

  engine.SetLayout(Layout::Engine::MULTI_LINE_BOX);
  DALI_TEST_CHECK(Layout::Engine::MULTI_LINE_BOX == engine.GetLayout());

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetCursorWidth(void)
{
  ToolkitTestApplication application;
  tet_infoline(" ");

  Layout::Engine engine;

  DALI_TEST_EQUALS(0, engine.GetCursorWidth(), TEST_LOCATION);

  engine.SetCursorWidth(2);
  DALI_TEST_EQUALS(2, engine.GetCursorWidth(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutNoText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutNoText");

  Size           textArea(100.f, 60.f);
  Size           layoutSize = Vector2::ZERO;
  LayoutTextData data =
    {
      "No text",
      "",
      textArea,
      0u,
      NULL,
      layoutSize,
      0u,
      NULL,
      0u,
      NULL,
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      0u,
      false,
      DevelText::EllipsisPosition::END,
      false};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSmallTextArea01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSmallTextArea01");

  // Won't layout the text in multi-line if the width is too small.

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
  Size           textArea(1.f, 1.f);
  Size           layoutSize = Vector2::ZERO;
  LayoutTextData data =
    {
      "Layout text in a small area",
      "Hello world",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      0u,
      NULL,
      0u,
      NULL,
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      11u,
      false,
      DevelText::EllipsisPosition::END,
      false};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSmallTextArea02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSmallTextArea02");

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
  Size           textArea(1.f, 1.f);
  Size           layoutSize(81.f, 20.f);
  float          positions[] = {0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f};
  struct LineRun line =
    {
      {0u, 11u},
      {0u, 11u},
      81.f,
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
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText01");

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
  Size  layoutSize(98.f, 97.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f, 81.f, -0.f, 0.f, -13.f, 9.f, -9.f, 19.f, -9.f, 32.f, -9.f, 41.f, -2.f, 45.f, -12.f, 0.f, -12.f, 8.f, -9.f, 18.f, -9.f, 27.f, -9.f, 38.f, -9.f, 47.f, -11.f, 53.f, -0.f, 0.f, -12.f, 11.f, -12.f, 15.f, -12.f, 26.f, -9.f, 36.f, -9.f, 42.f, -9.f, 53.f, -9.f, 62.f, -11.f, 68.f, -0.f, 0.f, -12.f, 4.f, -12.f, 9.f, -9.f, 18.f, -9.f, 28.f, -9.f, 36.f, -0.f, 41.f, -9.f, 51.f, -12.f, 57.f, -0.f, 62.f, -11.f, 68.f, -9.f, 78.f, -9.f, 87.f, -11.f, 94.f, -2.f};
  struct LineRun line0 =
    {
      {0u, 12u},
      {0u, 12u},
      81.f,
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
      44.f,
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
      54.f,
      15.f,
      -4.f,
      6.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line3 =
    {
      {25u, 9u},
      {25u, 10u},
      68.f,
      15.f,
      -4.f,
      6.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line4 =
    {
      {34u, 14u},
      {35u, 14u},
      98.f,
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
      "Layout simple multi-line text",
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
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText02");

  // Layout some lines of bidirectional text.

  const std::string fontFamily1("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun1.familyLength                    = fontFamily1.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily1.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = false;

  const std::string fontFamily2("TizenSansHebrew");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex     = 17u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun2.familyLength                    = fontFamily2.size();
  fontDescriptionRun2.familyName                      = new char[fontDescriptionRun2.familyLength];
  memcpy(fontDescriptionRun2.familyName, fontFamily2.c_str(), fontDescriptionRun2.familyLength);
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined  = false;
  fontDescriptionRun2.slantDefined  = false;
  fontDescriptionRun2.sizeDefined   = false;

  const std::string fontFamily3("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun3;
  fontDescriptionRun3.characterRun.characterIndex     = 26u;
  fontDescriptionRun3.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun3.familyLength                    = fontFamily3.size();
  fontDescriptionRun3.familyName                      = new char[fontDescriptionRun3.familyLength];
  memcpy(fontDescriptionRun3.familyName, fontFamily3.c_str(), fontDescriptionRun3.familyLength);
  fontDescriptionRun3.familyDefined = true;
  fontDescriptionRun3.weightDefined = false;
  fontDescriptionRun3.widthDefined  = false;
  fontDescriptionRun3.slantDefined  = false;
  fontDescriptionRun3.sizeDefined   = false;

  const std::string fontFamily4("TizenSansHebrew");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun4;
  fontDescriptionRun4.characterRun.characterIndex     = 28u;
  fontDescriptionRun4.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun4.familyLength                    = fontFamily4.size();
  fontDescriptionRun4.familyName                      = new char[fontDescriptionRun4.familyLength];
  memcpy(fontDescriptionRun4.familyName, fontFamily4.c_str(), fontDescriptionRun4.familyLength);
  fontDescriptionRun4.familyDefined = true;
  fontDescriptionRun4.weightDefined = false;
  fontDescriptionRun4.widthDefined  = false;
  fontDescriptionRun4.slantDefined  = false;
  fontDescriptionRun4.sizeDefined   = false;

  const std::string fontFamily5("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun5;
  fontDescriptionRun5.characterRun.characterIndex     = 38u;
  fontDescriptionRun5.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun5.familyLength                    = fontFamily5.size();
  fontDescriptionRun5.familyName                      = new char[fontDescriptionRun5.familyLength];
  memcpy(fontDescriptionRun5.familyName, fontFamily5.c_str(), fontDescriptionRun5.familyLength);
  fontDescriptionRun5.familyDefined = true;
  fontDescriptionRun5.weightDefined = false;
  fontDescriptionRun5.widthDefined  = false;
  fontDescriptionRun5.slantDefined  = false;
  fontDescriptionRun5.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun1);
  fontDescriptionRuns.PushBack(fontDescriptionRun2);
  fontDescriptionRuns.PushBack(fontDescriptionRun3);
  fontDescriptionRuns.PushBack(fontDescriptionRun4);
  fontDescriptionRuns.PushBack(fontDescriptionRun5);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(81.f, 120.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f, 81.f, -0.f, //  0 .. 11
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, // 12 .. 21
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, // 22 .. 27
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, // 28 .. 37
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, // 38 .. 49
      4.f,
      -13.f,
      13.f,
      -9.f,
      23.f,
      -9.f,
      36.f,
      -9.f,
      0.f,
      -2.f, // 50 .. 54
    };
  struct LineRun line0 =
    {
      {0u, 12u},
      {0u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line1 =
    {
      {12u, 10u},
      {12u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line2 =
    {
      {22u, 6u},
      {22u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line3 =
    {
      {28u, 10u},
      {28u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line4 =
    {
      {38u, 12u},
      {38u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line5 =
    {
      {50u, 5u},
      {50u, 5u},
      45.f,
      15.f,
      -5.f,
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
      "Layout bidirectional text.",
      "Hello world demo שלום עולם.\n"
      "שלום עולם hello world demo.",
      textArea,
      5u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      55u,
      positions,
      6u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      55u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText03");

  // Layout a long word which doesn't fit in the width of the text area.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 29u;
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
  Size  textArea(100.f, 300.f);
  Size  layoutSize(98.f, 60.f);
  float positions[] =
    {
      0.f,
      -12.f,
      10.f,
      -9.f,
      20.f,
      -13.f,
      24.f,
      -13.f,
      27.f,
      -9.f,
      36.f,
      -9.f,
      48.f,
      -9.f,
      58.f,
      -9.f,
      65.f,
      -13.f,
      68.f,
      -13.f,
      77.f,
      -13.f,
      86.f,
      -9.f,
      0.f,
      -9.f,
      13.f,
      -9.f,
      23.f,
      -13.f,
      31.f,
      -9.f,
      41.f,
      -13.f,
      45.f,
      -13.f,
      48.f,
      -9.f,
      57.f,
      -9.f,
      69.f,
      -9.f,
      79.f,
      -9.f,
      85.f,
      -13.f,
      88.f,
      -13.f,
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -2.f,
    };
  struct LineRun line0 =
    {
      {0u, 12u},
      {0u, 12u},
      95.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line1 =
    {
      {12u, 12u},
      {12u, 12u},
      98.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line2 =
    {
      {24u, 5u},
      {24u, 5u},
      44.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);
  lines.PushBack(line2);

  LayoutTextData data =
    {
      "Layout a long word which doesn't fit in the width of the text area.",
      "Helloworlddemohelloworlddemo.",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      29u,
      positions,
      3u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      29u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText04");

  // Layout simple text ending with a \n. It has to add a void line at the end.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 13u;
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
  Size  textArea(100.f, 300.f);
  Size  layoutSize(84.f, 40.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f, 81.f, -2.f, 85.f, -12.f};
  struct LineRun line0 =
    {
      {0u, 13u},
      {0u, 13u},
      84.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line1 =
    {
      {13u, 0u},
      {13u, 0u},
      0.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);

  LayoutTextData data =
    {
      "Layout simple text ending with a \n.",
      "Hello world.\n",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      13u,
      positions,
      2u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      13u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText05");

  // Layout simple text with one character with a different font size.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 6u;
  fontDescriptionRun1.familyLength                    = fontFamily.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex     = 6u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 1u;
  fontDescriptionRun2.familyLength                    = fontFamily.size();
  fontDescriptionRun2.familyName                      = new char[fontDescriptionRun2.familyLength];
  memcpy(fontDescriptionRun2.familyName, fontFamily.c_str(), fontDescriptionRun2.familyLength);
  fontDescriptionRun2.size          = 1280u;
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined  = false;
  fontDescriptionRun2.slantDefined  = false;
  fontDescriptionRun2.sizeDefined   = true;

  FontDescriptionRun fontDescriptionRun3;
  fontDescriptionRun3.characterRun.characterIndex     = 7u;
  fontDescriptionRun3.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun3.familyLength                    = fontFamily.size();
  fontDescriptionRun3.familyName                      = new char[fontDescriptionRun3.familyLength];
  memcpy(fontDescriptionRun3.familyName, fontFamily.c_str(), fontDescriptionRun3.familyLength);
  fontDescriptionRun3.familyDefined = true;
  fontDescriptionRun3.weightDefined = false;
  fontDescriptionRun3.widthDefined  = false;
  fontDescriptionRun3.slantDefined  = false;
  fontDescriptionRun3.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun1);
  fontDescriptionRuns.PushBack(fontDescriptionRun2);
  fontDescriptionRuns.PushBack(fontDescriptionRun3);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(89.f, 53.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -14.f, 60.f, -9.f, 70.f, -9.f, 76.f, -13.f, 80.f, -13.f, 89.f, -0.f, 0.f, -13.f, 9.f, -9.f, 19.f, -9.f, 32.f, -9.f, 41.f, -2.f};
  struct LineRun line0 =
    {
      {0u, 12u},
      {0u, 12u},
      89.f,
      25.f,
      -8.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line1 =
    {
      {12u, 5u},
      {12u, 5u},
      44.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);

  LayoutTextData data =
    {
      "Layout simple text with one character with a different font size.",
      "Hello world demo.",
      textArea,
      3u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      17u,
      positions,
      2u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      17u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText06");

  const std::string fontFamily("TizenSansHebrew");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 10u;
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

  Size  textArea(64.f, 100.f);
  Size  layoutSize(34.f, 40.f);
  float positions[] =
    {
      27.f, -13.f, 18.f, -10.f, 9.f, -10.f, 4.f, -10.f, 0.f, -0.f, 22.f, -10.f, 17.f, -10.f, 12.f, -10.f, 4.f, -10.f, 0.f, -10.f};
  struct LineRun line0 =
    {
      {0u, 5u},
      {0u, 5u},
      32.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line1 =
    {
      {5u, 5u},
      {5u, 5u},
      34.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      true,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);

  LayoutTextData data =
    {
      "Layout right to left text that doesn't fit in the text area after reordering.",
      "לכאן שנורו", // If this text is laid-out ltr the width is 64. When reordered, the length is 66. This might cause alignment issues.
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      10u,
      positions,
      2u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      10u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText07");

  const std::string fontFamily("TizenSansHebrew");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 9u;
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

  Size  textArea(26.f, 100.f);
  Size  layoutSize(24.f, 60.f);
  float positions[] =
    {
      10.f, -10.f, 4.f, -10.f, 0.f, -10.f, 13.f, -10.f, 9.f, -10.f, 0.f, -13.f, 14.f, -10.f, 4.f, -10.f, 0.f, -10.f};
  struct LineRun line0 =
    {
      {0u, 3u},
      {0u, 3u},
      21.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line1 =
    {
      {3u, 3u},
      {3u, 3u},
      22.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line2 =
    {
      {6u, 3u},
      {6u, 3u},
      24.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      true,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line0);
  lines.PushBack(line1);
  lines.PushBack(line2);

  LayoutTextData data =
    {
      "Layout a single word of right to left text that doesn't fit in one single line. When layouting ltr a piece of word fits in the line but it doesn't after reordering.",
      "שנורולכאן", // If a piece of this text is laid-out ltr the width is 26. When reordered, the length is 27. This might cause alignment issues.
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      9u,
      positions,
      3u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      9u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextUpdateLayout01");

  // Layout some lines of bidirectional text. Update the paragraphs at the beginning.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 17u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 26u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun03.familyLength                    = fontLatin.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontLatin.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 28u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength                    = fontArabic.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 42u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 54u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun06.familyLength                    = fontHebrew.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontHebrew.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun07;
  fontDescriptionRun07.characterRun.characterIndex     = 64u;
  fontDescriptionRun07.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun07.familyLength                    = fontHebrew.size();
  fontDescriptionRun07.familyName                      = new char[fontDescriptionRun07.familyLength];
  memcpy(fontDescriptionRun07.familyName, fontHebrew.c_str(), fontDescriptionRun07.familyLength);
  fontDescriptionRun07.familyDefined = true;
  fontDescriptionRun07.weightDefined = false;
  fontDescriptionRun07.widthDefined  = false;
  fontDescriptionRun07.slantDefined  = false;
  fontDescriptionRun07.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun08;
  fontDescriptionRun08.characterRun.characterIndex     = 74u;
  fontDescriptionRun08.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun08.familyLength                    = fontLatin.size();
  fontDescriptionRun08.familyName                      = new char[fontDescriptionRun08.familyLength];
  memcpy(fontDescriptionRun08.familyName, fontLatin.c_str(), fontDescriptionRun08.familyLength);
  fontDescriptionRun08.familyDefined = true;
  fontDescriptionRun08.weightDefined = false;
  fontDescriptionRun08.widthDefined  = false;
  fontDescriptionRun08.slantDefined  = false;
  fontDescriptionRun08.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun09;
  fontDescriptionRun09.characterRun.characterIndex     = 92u;
  fontDescriptionRun09.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun09.familyLength                    = fontLatin.size();
  fontDescriptionRun09.familyName                      = new char[fontDescriptionRun09.familyLength];
  memcpy(fontDescriptionRun09.familyName, fontLatin.c_str(), fontDescriptionRun09.familyLength);
  fontDescriptionRun09.familyDefined = true;
  fontDescriptionRun09.weightDefined = false;
  fontDescriptionRun09.widthDefined  = false;
  fontDescriptionRun09.slantDefined  = false;
  fontDescriptionRun09.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun10;
  fontDescriptionRun10.characterRun.characterIndex     = 104u;
  fontDescriptionRun10.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun10.familyLength                    = fontArabic.size();
  fontDescriptionRun10.familyName                      = new char[fontDescriptionRun10.familyLength];
  memcpy(fontDescriptionRun10.familyName, fontArabic.c_str(), fontDescriptionRun10.familyLength);
  fontDescriptionRun10.familyDefined = true;
  fontDescriptionRun10.weightDefined = false;
  fontDescriptionRun10.widthDefined  = false;
  fontDescriptionRun10.slantDefined  = false;
  fontDescriptionRun10.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun11;
  fontDescriptionRun11.characterRun.characterIndex     = 118u;
  fontDescriptionRun11.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun11.familyLength                    = fontHebrew.size();
  fontDescriptionRun11.familyName                      = new char[fontDescriptionRun11.familyLength];
  memcpy(fontDescriptionRun11.familyName, fontHebrew.c_str(), fontDescriptionRun11.familyLength);
  fontDescriptionRun11.familyDefined = true;
  fontDescriptionRun11.weightDefined = false;
  fontDescriptionRun11.widthDefined  = false;
  fontDescriptionRun11.slantDefined  = false;
  fontDescriptionRun11.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun12;
  fontDescriptionRun12.characterRun.characterIndex     = 128u;
  fontDescriptionRun12.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun12.familyLength                    = fontLatin.size();
  fontDescriptionRun12.familyName                      = new char[fontDescriptionRun12.familyLength];
  memcpy(fontDescriptionRun12.familyName, fontLatin.c_str(), fontDescriptionRun12.familyLength);
  fontDescriptionRun12.familyDefined = true;
  fontDescriptionRun12.weightDefined = false;
  fontDescriptionRun12.widthDefined  = false;
  fontDescriptionRun12.slantDefined  = false;
  fontDescriptionRun12.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun13;
  fontDescriptionRun13.characterRun.characterIndex     = 145u;
  fontDescriptionRun13.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun13.familyLength                    = fontHebrew.size();
  fontDescriptionRun13.familyName                      = new char[fontDescriptionRun13.familyLength];
  memcpy(fontDescriptionRun13.familyName, fontHebrew.c_str(), fontDescriptionRun13.familyLength);
  fontDescriptionRun13.familyDefined = true;
  fontDescriptionRun13.weightDefined = false;
  fontDescriptionRun13.widthDefined  = false;
  fontDescriptionRun13.slantDefined  = false;
  fontDescriptionRun13.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun14;
  fontDescriptionRun14.characterRun.characterIndex     = 154u;
  fontDescriptionRun14.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun14.familyLength                    = fontLatin.size();
  fontDescriptionRun14.familyName                      = new char[fontDescriptionRun14.familyLength];
  memcpy(fontDescriptionRun14.familyName, fontLatin.c_str(), fontDescriptionRun14.familyLength);
  fontDescriptionRun14.familyDefined = true;
  fontDescriptionRun14.weightDefined = false;
  fontDescriptionRun14.widthDefined  = false;
  fontDescriptionRun14.slantDefined  = false;
  fontDescriptionRun14.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun15;
  fontDescriptionRun15.characterRun.characterIndex     = 156u;
  fontDescriptionRun15.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun15.familyLength                    = fontHebrew.size();
  fontDescriptionRun15.familyName                      = new char[fontDescriptionRun15.familyLength];
  memcpy(fontDescriptionRun15.familyName, fontHebrew.c_str(), fontDescriptionRun15.familyLength);
  fontDescriptionRun15.familyDefined = true;
  fontDescriptionRun15.weightDefined = false;
  fontDescriptionRun15.widthDefined  = false;
  fontDescriptionRun15.slantDefined  = false;
  fontDescriptionRun15.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun16;
  fontDescriptionRun16.characterRun.characterIndex     = 166u;
  fontDescriptionRun16.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun16.familyLength                    = fontLatin.size();
  fontDescriptionRun16.familyName                      = new char[fontDescriptionRun16.familyLength];
  memcpy(fontDescriptionRun16.familyName, fontLatin.c_str(), fontDescriptionRun16.familyLength);
  fontDescriptionRun16.familyDefined = true;
  fontDescriptionRun16.weightDefined = false;
  fontDescriptionRun16.widthDefined  = false;
  fontDescriptionRun16.slantDefined  = false;
  fontDescriptionRun16.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun17;
  fontDescriptionRun17.characterRun.characterIndex     = 178u;
  fontDescriptionRun17.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun17.familyLength                    = fontArabic.size();
  fontDescriptionRun17.familyName                      = new char[fontDescriptionRun17.familyLength];
  memcpy(fontDescriptionRun17.familyName, fontArabic.c_str(), fontDescriptionRun17.familyLength);
  fontDescriptionRun17.familyDefined = true;
  fontDescriptionRun17.weightDefined = false;
  fontDescriptionRun17.widthDefined  = false;
  fontDescriptionRun17.slantDefined  = false;
  fontDescriptionRun17.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);
  fontDescriptionRuns.PushBack(fontDescriptionRun07);
  fontDescriptionRuns.PushBack(fontDescriptionRun08);
  fontDescriptionRuns.PushBack(fontDescriptionRun09);
  fontDescriptionRuns.PushBack(fontDescriptionRun10);
  fontDescriptionRuns.PushBack(fontDescriptionRun11);
  fontDescriptionRuns.PushBack(fontDescriptionRun12);
  fontDescriptionRuns.PushBack(fontDescriptionRun13);
  fontDescriptionRuns.PushBack(fontDescriptionRun14);
  fontDescriptionRuns.PushBack(fontDescriptionRun15);
  fontDescriptionRuns.PushBack(fontDescriptionRun16);
  fontDescriptionRuns.PushBack(fontDescriptionRun17);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(100.f, 380.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f, 81.f, -0.f, //   0 ..  11
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, //  12 ..  21
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, //  22 ..  27
      58.f,
      -8.f,
      52.f,
      -6.f,
      45.f,
      -8.f,
      41.f,
      -7.f,
      39.f,
      -11.f,
      34.f,
      -0.f,
      30.f,
      -7.f,
      28.f,
      -11.f,
      23.f,
      -11.f,
      16.f,
      -8.f,
      15.f,
      -11.f,
      9.f,
      -11.f,
      4.f,
      -8.f,
      0.f,
      -0.f,
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f, //  28 ..  47
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f,
      59.f,
      -10.f,
      50.f,
      -13.f,
      46.f,
      -10.f,
      36.f,
      -10.f,
      32.f,
      -0.f, //  48 ..  58
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      0.f,
      -0.f, //  59 ..  63
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, //  64 ..  73
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, //  74 ..  85
      4.f,
      -13.f,
      13.f,
      -9.f,
      23.f,
      -9.f,
      36.f,
      -9.f,
      0.f,
      -2.f,
      0.f,
      -12.f, //  86 ..  91
      0.f,
      -13.f,
      8.f,
      -9.f,
      18.f,
      -13.f,
      22.f,
      -13.f,
      25.f,
      -9.f,
      34.f,
      -0.f,
      38.f,
      -9.f,
      50.f,
      -9.f,
      60.f,
      -9.f,
      66.f,
      -13.f,
      69.f,
      -13.f,
      79.f,
      -0.f, //  92 .. 103
      92.f,
      -8.f,
      86.f,
      -6.f,
      79.f,
      -8.f,
      75.f,
      -7.f,
      73.f,
      -11.f,
      68.f,
      -0.f,
      64.f,
      -7.f,
      62.f,
      -11.f,
      57.f,
      -11.f,
      50.f,
      -8.f,
      49.f,
      -11.f,
      43.f,
      -11.f,
      38.f,
      -8.f,
      34.f,
      -0.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      10.f,
      -10.f,
      0.f,
      -10.f,
      99.f,
      -0.f, // 104 .. 122
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -0.f, // 123 .. 127
      0.f,
      -12.f,
      10.f,
      -9.f,
      20.f,
      -13.f,
      24.f,
      -13.f,
      27.f,
      -9.f,
      36.f,
      -0.f,
      40.f,
      -9.f,
      52.f,
      -9.f,
      62.f,
      -9.f,
      69.f,
      -13.f,
      72.f,
      -13.f,
      81.f,
      -0.f, // 128 .. 139
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, // 140 .. 149
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, // 150 .. 155
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, // 156 .. 165
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, // 166 .. 177
      54.f,
      -8.f,
      49.f,
      -6.f,
      41.f,
      -8.f,
      37.f,
      -7.f,
      35.f,
      -11.f,
      31.f,
      -0.f,
      26.f,
      -7.f,
      24.f,
      -11.f,
      19.f,
      -11.f,
      12.f,
      -8.f,
      11.f,
      -11.f,
      5.f,
      -11.f,
      0.f,
      -8.f,
      0.f,
      -0.f, // 178 .. 191
    };

  struct LineRun line01 =
    {
      {0u, 12u},
      {0u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line02 =
    {
      {12u, 10u},
      {12u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line03 =
    {
      {22u, 6u},
      {22u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line04 =
    {
      {28u, 14u},
      {28u, 14u},
      63.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line05 =
    {
      {42, 12u},
      {42, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line06 =
    {
      {54u, 10u},
      {54u, 10u},
      70.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line07 =
    {
      {64u, 10u},
      {64u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line08 =
    {
      {74u, 12u},
      {74u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line09 =
    {
      {86u, 6u},
      {86u, 6u},
      45.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line10 =
    {
      {92u, 12u},
      {92u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line11 =
    {
      {104u, 19u},
      {104u, 19u},
      100.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line12 =
    {
      {123u, 5u},
      {123u, 5u},
      32.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line13 =
    {
      {128u, 12u},
      {128u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line14 =
    {
      {140u, 10u},
      {140u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line15 =
    {
      {150u, 6u},
      {150u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line16 =
    {
      {156u, 10u},
      {156u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line17 =
    {
      {166u, 12u},
      {166u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line18 =
    {
      {178u, 14u},
      {178u, 14u},
      63.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line19 =
    {
      {192u, 0u},
      {192u, 0u},
      0.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line01);
  lines.PushBack(line02);
  lines.PushBack(line03);
  lines.PushBack(line04);
  lines.PushBack(line05);
  lines.PushBack(line06);
  lines.PushBack(line07);
  lines.PushBack(line08);
  lines.PushBack(line09);
  lines.PushBack(line10);
  lines.PushBack(line11);
  lines.PushBack(line12);
  lines.PushBack(line13);
  lines.PushBack(line14);
  lines.PushBack(line15);
  lines.PushBack(line16);
  lines.PushBack(line17);
  lines.PushBack(line18);
  lines.PushBack(line19);

  LayoutTextData data =
    {
      "Layout bidirectional text.",
      "Hello world demo שלום עולם.\n"
      "مرحبا بالعالم hello world שלום עולם\n"
      "שלום עולם hello world demo.\n"
      "hello world مرحبا بالعالم שלום עולם\n"
      "Hello world demo שלום עולם.\n"
      "שלום עולם hello world مرحبا بالعالم\n",
      textArea,
      17u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      192u,
      positions,
      19u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      64u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextUpdateLayout02");

  // Layout some lines of bidirectional text. Update the paragraphs at the middle.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 17u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 26u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun03.familyLength                    = fontLatin.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontLatin.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 28u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength                    = fontArabic.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 42u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 54u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun06.familyLength                    = fontHebrew.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontHebrew.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun07;
  fontDescriptionRun07.characterRun.characterIndex     = 64u;
  fontDescriptionRun07.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun07.familyLength                    = fontHebrew.size();
  fontDescriptionRun07.familyName                      = new char[fontDescriptionRun07.familyLength];
  memcpy(fontDescriptionRun07.familyName, fontHebrew.c_str(), fontDescriptionRun07.familyLength);
  fontDescriptionRun07.familyDefined = true;
  fontDescriptionRun07.weightDefined = false;
  fontDescriptionRun07.widthDefined  = false;
  fontDescriptionRun07.slantDefined  = false;
  fontDescriptionRun07.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun08;
  fontDescriptionRun08.characterRun.characterIndex     = 74u;
  fontDescriptionRun08.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun08.familyLength                    = fontLatin.size();
  fontDescriptionRun08.familyName                      = new char[fontDescriptionRun08.familyLength];
  memcpy(fontDescriptionRun08.familyName, fontLatin.c_str(), fontDescriptionRun08.familyLength);
  fontDescriptionRun08.familyDefined = true;
  fontDescriptionRun08.weightDefined = false;
  fontDescriptionRun08.widthDefined  = false;
  fontDescriptionRun08.slantDefined  = false;
  fontDescriptionRun08.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun09;
  fontDescriptionRun09.characterRun.characterIndex     = 92u;
  fontDescriptionRun09.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun09.familyLength                    = fontLatin.size();
  fontDescriptionRun09.familyName                      = new char[fontDescriptionRun09.familyLength];
  memcpy(fontDescriptionRun09.familyName, fontLatin.c_str(), fontDescriptionRun09.familyLength);
  fontDescriptionRun09.familyDefined = true;
  fontDescriptionRun09.weightDefined = false;
  fontDescriptionRun09.widthDefined  = false;
  fontDescriptionRun09.slantDefined  = false;
  fontDescriptionRun09.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun10;
  fontDescriptionRun10.characterRun.characterIndex     = 104u;
  fontDescriptionRun10.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun10.familyLength                    = fontArabic.size();
  fontDescriptionRun10.familyName                      = new char[fontDescriptionRun10.familyLength];
  memcpy(fontDescriptionRun10.familyName, fontArabic.c_str(), fontDescriptionRun10.familyLength);
  fontDescriptionRun10.familyDefined = true;
  fontDescriptionRun10.weightDefined = false;
  fontDescriptionRun10.widthDefined  = false;
  fontDescriptionRun10.slantDefined  = false;
  fontDescriptionRun10.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun11;
  fontDescriptionRun11.characterRun.characterIndex     = 118u;
  fontDescriptionRun11.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun11.familyLength                    = fontHebrew.size();
  fontDescriptionRun11.familyName                      = new char[fontDescriptionRun11.familyLength];
  memcpy(fontDescriptionRun11.familyName, fontHebrew.c_str(), fontDescriptionRun11.familyLength);
  fontDescriptionRun11.familyDefined = true;
  fontDescriptionRun11.weightDefined = false;
  fontDescriptionRun11.widthDefined  = false;
  fontDescriptionRun11.slantDefined  = false;
  fontDescriptionRun11.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun12;
  fontDescriptionRun12.characterRun.characterIndex     = 128u;
  fontDescriptionRun12.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun12.familyLength                    = fontLatin.size();
  fontDescriptionRun12.familyName                      = new char[fontDescriptionRun12.familyLength];
  memcpy(fontDescriptionRun12.familyName, fontLatin.c_str(), fontDescriptionRun12.familyLength);
  fontDescriptionRun12.familyDefined = true;
  fontDescriptionRun12.weightDefined = false;
  fontDescriptionRun12.widthDefined  = false;
  fontDescriptionRun12.slantDefined  = false;
  fontDescriptionRun12.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun13;
  fontDescriptionRun13.characterRun.characterIndex     = 145u;
  fontDescriptionRun13.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun13.familyLength                    = fontHebrew.size();
  fontDescriptionRun13.familyName                      = new char[fontDescriptionRun13.familyLength];
  memcpy(fontDescriptionRun13.familyName, fontHebrew.c_str(), fontDescriptionRun13.familyLength);
  fontDescriptionRun13.familyDefined = true;
  fontDescriptionRun13.weightDefined = false;
  fontDescriptionRun13.widthDefined  = false;
  fontDescriptionRun13.slantDefined  = false;
  fontDescriptionRun13.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun14;
  fontDescriptionRun14.characterRun.characterIndex     = 154u;
  fontDescriptionRun14.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun14.familyLength                    = fontLatin.size();
  fontDescriptionRun14.familyName                      = new char[fontDescriptionRun14.familyLength];
  memcpy(fontDescriptionRun14.familyName, fontLatin.c_str(), fontDescriptionRun14.familyLength);
  fontDescriptionRun14.familyDefined = true;
  fontDescriptionRun14.weightDefined = false;
  fontDescriptionRun14.widthDefined  = false;
  fontDescriptionRun14.slantDefined  = false;
  fontDescriptionRun14.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun15;
  fontDescriptionRun15.characterRun.characterIndex     = 156u;
  fontDescriptionRun15.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun15.familyLength                    = fontHebrew.size();
  fontDescriptionRun15.familyName                      = new char[fontDescriptionRun15.familyLength];
  memcpy(fontDescriptionRun15.familyName, fontHebrew.c_str(), fontDescriptionRun15.familyLength);
  fontDescriptionRun15.familyDefined = true;
  fontDescriptionRun15.weightDefined = false;
  fontDescriptionRun15.widthDefined  = false;
  fontDescriptionRun15.slantDefined  = false;
  fontDescriptionRun15.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun16;
  fontDescriptionRun16.characterRun.characterIndex     = 166u;
  fontDescriptionRun16.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun16.familyLength                    = fontLatin.size();
  fontDescriptionRun16.familyName                      = new char[fontDescriptionRun16.familyLength];
  memcpy(fontDescriptionRun16.familyName, fontLatin.c_str(), fontDescriptionRun16.familyLength);
  fontDescriptionRun16.familyDefined = true;
  fontDescriptionRun16.weightDefined = false;
  fontDescriptionRun16.widthDefined  = false;
  fontDescriptionRun16.slantDefined  = false;
  fontDescriptionRun16.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun17;
  fontDescriptionRun17.characterRun.characterIndex     = 178u;
  fontDescriptionRun17.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun17.familyLength                    = fontArabic.size();
  fontDescriptionRun17.familyName                      = new char[fontDescriptionRun17.familyLength];
  memcpy(fontDescriptionRun17.familyName, fontArabic.c_str(), fontDescriptionRun17.familyLength);
  fontDescriptionRun17.familyDefined = true;
  fontDescriptionRun17.weightDefined = false;
  fontDescriptionRun17.widthDefined  = false;
  fontDescriptionRun17.slantDefined  = false;
  fontDescriptionRun17.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);
  fontDescriptionRuns.PushBack(fontDescriptionRun07);
  fontDescriptionRuns.PushBack(fontDescriptionRun08);
  fontDescriptionRuns.PushBack(fontDescriptionRun09);
  fontDescriptionRuns.PushBack(fontDescriptionRun10);
  fontDescriptionRuns.PushBack(fontDescriptionRun11);
  fontDescriptionRuns.PushBack(fontDescriptionRun12);
  fontDescriptionRuns.PushBack(fontDescriptionRun13);
  fontDescriptionRuns.PushBack(fontDescriptionRun14);
  fontDescriptionRuns.PushBack(fontDescriptionRun15);
  fontDescriptionRuns.PushBack(fontDescriptionRun16);
  fontDescriptionRuns.PushBack(fontDescriptionRun17);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(100.f, 380.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f, 81.f, -0.f, //   0 ..  11
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, //  12 ..  21
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, //  22 ..  27
      58.f,
      -8.f,
      52.f,
      -6.f,
      45.f,
      -8.f,
      41.f,
      -7.f,
      39.f,
      -11.f,
      34.f,
      -0.f,
      30.f,
      -7.f,
      28.f,
      -11.f,
      23.f,
      -11.f,
      16.f,
      -8.f,
      15.f,
      -11.f,
      9.f,
      -11.f,
      4.f,
      -8.f,
      0.f,
      -0.f,
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f, //  28 ..  47
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f,
      59.f,
      -10.f,
      50.f,
      -13.f,
      46.f,
      -10.f,
      36.f,
      -10.f,
      32.f,
      -0.f, //  48 ..  58
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      0.f,
      -0.f, //  59 ..  63
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, //  64 ..  73
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, //  74 ..  85
      4.f,
      -13.f,
      13.f,
      -9.f,
      23.f,
      -9.f,
      36.f,
      -9.f,
      0.f,
      -2.f,
      0.f,
      -12.f, //  86 ..  91
      0.f,
      -13.f,
      8.f,
      -9.f,
      18.f,
      -13.f,
      22.f,
      -13.f,
      25.f,
      -9.f,
      34.f,
      -0.f,
      38.f,
      -9.f,
      50.f,
      -9.f,
      60.f,
      -9.f,
      66.f,
      -13.f,
      69.f,
      -13.f,
      79.f,
      -0.f, //  92 .. 103
      92.f,
      -8.f,
      86.f,
      -6.f,
      79.f,
      -8.f,
      75.f,
      -7.f,
      73.f,
      -11.f,
      68.f,
      -0.f,
      64.f,
      -7.f,
      62.f,
      -11.f,
      57.f,
      -11.f,
      50.f,
      -8.f,
      49.f,
      -11.f,
      43.f,
      -11.f,
      38.f,
      -8.f,
      34.f,
      -0.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      10.f,
      -10.f,
      0.f,
      -10.f,
      99.f,
      -0.f, // 104 .. 122
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -0.f, // 123 .. 127
      0.f,
      -12.f,
      10.f,
      -9.f,
      20.f,
      -13.f,
      24.f,
      -13.f,
      27.f,
      -9.f,
      36.f,
      -0.f,
      40.f,
      -9.f,
      52.f,
      -9.f,
      62.f,
      -9.f,
      69.f,
      -13.f,
      72.f,
      -13.f,
      81.f,
      -0.f, // 128 .. 139
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, // 140 .. 149
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, // 150 .. 155
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, // 156 .. 165
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, // 166 .. 177
      54.f,
      -8.f,
      49.f,
      -6.f,
      41.f,
      -8.f,
      37.f,
      -7.f,
      35.f,
      -11.f,
      31.f,
      -0.f,
      26.f,
      -7.f,
      24.f,
      -11.f,
      19.f,
      -11.f,
      12.f,
      -8.f,
      11.f,
      -11.f,
      5.f,
      -11.f,
      0.f,
      -8.f,
      0.f,
      -0.f, // 178 .. 191
    };

  struct LineRun line01 =
    {
      {0u, 12u},
      {0u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line02 =
    {
      {12u, 10u},
      {12u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line03 =
    {
      {22u, 6u},
      {22u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line04 =
    {
      {28u, 14u},
      {28u, 14u},
      63.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      true,
      false};
  struct LineRun line05 =
    {
      {42u, 12u},
      {42u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line06 =
    {
      {54u, 10u},
      {54u, 10u},
      70.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line07 =
    {
      {64u, 10u},
      {64u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line08 =
    {
      {74u, 12u},
      {74u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      1.f,
      0.f,
      false,
      false};
  struct LineRun line09 =
    {
      {86u, 6u},
      {86u, 6u},
      45.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line10 =
    {
      {92u, 12u},
      {92u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line11 =
    {
      {104u, 19u},
      {104u, 19u},
      100.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line12 =
    {
      {123u, 5u},
      {123u, 5u},
      32.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line13 =
    {
      {128u, 12u},
      {128u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line14 =
    {
      {140u, 10u},
      {140u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line15 =
    {
      {150u, 6u},
      {150u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line16 =
    {
      {156u, 10u},
      {156u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line17 =
    {
      {166u, 12u},
      {166u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line18 =
    {
      {178u, 14u},
      {178u, 14u},
      63.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line19 =
    {
      {192u, 0u},
      {192u, 0u},
      0.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line01);
  lines.PushBack(line02);
  lines.PushBack(line03);
  lines.PushBack(line04);
  lines.PushBack(line05);
  lines.PushBack(line06);
  lines.PushBack(line07);
  lines.PushBack(line08);
  lines.PushBack(line09);
  lines.PushBack(line10);
  lines.PushBack(line11);
  lines.PushBack(line12);
  lines.PushBack(line13);
  lines.PushBack(line14);
  lines.PushBack(line15);
  lines.PushBack(line16);
  lines.PushBack(line17);
  lines.PushBack(line18);
  lines.PushBack(line19);

  LayoutTextData data =
    {
      "Layout bidirectional text.",
      "Hello world demo שלום עולם.\n"
      "مرحبا بالعالم hello world שלום עולם\n"
      "שלום עולם hello world demo.\n"
      "hello world مرحبا بالعالم שלום עולם\n"
      "Hello world demo שלום עולם.\n"
      "שלום עולם hello world مرحبا بالعالم\n",
      textArea,
      17u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      192u,
      positions,
      19u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      64u,
      64u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextUpdateLayout03");

  // Layout some lines of bidirectional text. Update the paragraphs at the middle.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 17u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 26u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun03.familyLength                    = fontLatin.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontLatin.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 28u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength                    = fontArabic.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 42u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 54u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun06.familyLength                    = fontHebrew.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontHebrew.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun07;
  fontDescriptionRun07.characterRun.characterIndex     = 64u;
  fontDescriptionRun07.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun07.familyLength                    = fontHebrew.size();
  fontDescriptionRun07.familyName                      = new char[fontDescriptionRun07.familyLength];
  memcpy(fontDescriptionRun07.familyName, fontHebrew.c_str(), fontDescriptionRun07.familyLength);
  fontDescriptionRun07.familyDefined = true;
  fontDescriptionRun07.weightDefined = false;
  fontDescriptionRun07.widthDefined  = false;
  fontDescriptionRun07.slantDefined  = false;
  fontDescriptionRun07.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun08;
  fontDescriptionRun08.characterRun.characterIndex     = 74u;
  fontDescriptionRun08.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun08.familyLength                    = fontLatin.size();
  fontDescriptionRun08.familyName                      = new char[fontDescriptionRun08.familyLength];
  memcpy(fontDescriptionRun08.familyName, fontLatin.c_str(), fontDescriptionRun08.familyLength);
  fontDescriptionRun08.familyDefined = true;
  fontDescriptionRun08.weightDefined = false;
  fontDescriptionRun08.widthDefined  = false;
  fontDescriptionRun08.slantDefined  = false;
  fontDescriptionRun08.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun09;
  fontDescriptionRun09.characterRun.characterIndex     = 92u;
  fontDescriptionRun09.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun09.familyLength                    = fontLatin.size();
  fontDescriptionRun09.familyName                      = new char[fontDescriptionRun09.familyLength];
  memcpy(fontDescriptionRun09.familyName, fontLatin.c_str(), fontDescriptionRun09.familyLength);
  fontDescriptionRun09.familyDefined = true;
  fontDescriptionRun09.weightDefined = false;
  fontDescriptionRun09.widthDefined  = false;
  fontDescriptionRun09.slantDefined  = false;
  fontDescriptionRun09.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun10;
  fontDescriptionRun10.characterRun.characterIndex     = 104u;
  fontDescriptionRun10.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun10.familyLength                    = fontArabic.size();
  fontDescriptionRun10.familyName                      = new char[fontDescriptionRun10.familyLength];
  memcpy(fontDescriptionRun10.familyName, fontArabic.c_str(), fontDescriptionRun10.familyLength);
  fontDescriptionRun10.familyDefined = true;
  fontDescriptionRun10.weightDefined = false;
  fontDescriptionRun10.widthDefined  = false;
  fontDescriptionRun10.slantDefined  = false;
  fontDescriptionRun10.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun11;
  fontDescriptionRun11.characterRun.characterIndex     = 118u;
  fontDescriptionRun11.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun11.familyLength                    = fontHebrew.size();
  fontDescriptionRun11.familyName                      = new char[fontDescriptionRun11.familyLength];
  memcpy(fontDescriptionRun11.familyName, fontHebrew.c_str(), fontDescriptionRun11.familyLength);
  fontDescriptionRun11.familyDefined = true;
  fontDescriptionRun11.weightDefined = false;
  fontDescriptionRun11.widthDefined  = false;
  fontDescriptionRun11.slantDefined  = false;
  fontDescriptionRun11.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun12;
  fontDescriptionRun12.characterRun.characterIndex     = 128u;
  fontDescriptionRun12.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun12.familyLength                    = fontLatin.size();
  fontDescriptionRun12.familyName                      = new char[fontDescriptionRun12.familyLength];
  memcpy(fontDescriptionRun12.familyName, fontLatin.c_str(), fontDescriptionRun12.familyLength);
  fontDescriptionRun12.familyDefined = true;
  fontDescriptionRun12.weightDefined = false;
  fontDescriptionRun12.widthDefined  = false;
  fontDescriptionRun12.slantDefined  = false;
  fontDescriptionRun12.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun13;
  fontDescriptionRun13.characterRun.characterIndex     = 145u;
  fontDescriptionRun13.characterRun.numberOfCharacters = 9u;
  fontDescriptionRun13.familyLength                    = fontHebrew.size();
  fontDescriptionRun13.familyName                      = new char[fontDescriptionRun13.familyLength];
  memcpy(fontDescriptionRun13.familyName, fontHebrew.c_str(), fontDescriptionRun13.familyLength);
  fontDescriptionRun13.familyDefined = true;
  fontDescriptionRun13.weightDefined = false;
  fontDescriptionRun13.widthDefined  = false;
  fontDescriptionRun13.slantDefined  = false;
  fontDescriptionRun13.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun14;
  fontDescriptionRun14.characterRun.characterIndex     = 154u;
  fontDescriptionRun14.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun14.familyLength                    = fontLatin.size();
  fontDescriptionRun14.familyName                      = new char[fontDescriptionRun14.familyLength];
  memcpy(fontDescriptionRun14.familyName, fontLatin.c_str(), fontDescriptionRun14.familyLength);
  fontDescriptionRun14.familyDefined = true;
  fontDescriptionRun14.weightDefined = false;
  fontDescriptionRun14.widthDefined  = false;
  fontDescriptionRun14.slantDefined  = false;
  fontDescriptionRun14.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun15;
  fontDescriptionRun15.characterRun.characterIndex     = 156u;
  fontDescriptionRun15.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun15.familyLength                    = fontHebrew.size();
  fontDescriptionRun15.familyName                      = new char[fontDescriptionRun15.familyLength];
  memcpy(fontDescriptionRun15.familyName, fontHebrew.c_str(), fontDescriptionRun15.familyLength);
  fontDescriptionRun15.familyDefined = true;
  fontDescriptionRun15.weightDefined = false;
  fontDescriptionRun15.widthDefined  = false;
  fontDescriptionRun15.slantDefined  = false;
  fontDescriptionRun15.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun16;
  fontDescriptionRun16.characterRun.characterIndex     = 166u;
  fontDescriptionRun16.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun16.familyLength                    = fontLatin.size();
  fontDescriptionRun16.familyName                      = new char[fontDescriptionRun16.familyLength];
  memcpy(fontDescriptionRun16.familyName, fontLatin.c_str(), fontDescriptionRun16.familyLength);
  fontDescriptionRun16.familyDefined = true;
  fontDescriptionRun16.weightDefined = false;
  fontDescriptionRun16.widthDefined  = false;
  fontDescriptionRun16.slantDefined  = false;
  fontDescriptionRun16.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun17;
  fontDescriptionRun17.characterRun.characterIndex     = 178u;
  fontDescriptionRun17.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun17.familyLength                    = fontArabic.size();
  fontDescriptionRun17.familyName                      = new char[fontDescriptionRun17.familyLength];
  memcpy(fontDescriptionRun17.familyName, fontArabic.c_str(), fontDescriptionRun17.familyLength);
  fontDescriptionRun17.familyDefined = true;
  fontDescriptionRun17.weightDefined = false;
  fontDescriptionRun17.widthDefined  = false;
  fontDescriptionRun17.slantDefined  = false;
  fontDescriptionRun17.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);
  fontDescriptionRuns.PushBack(fontDescriptionRun07);
  fontDescriptionRuns.PushBack(fontDescriptionRun08);
  fontDescriptionRuns.PushBack(fontDescriptionRun09);
  fontDescriptionRuns.PushBack(fontDescriptionRun10);
  fontDescriptionRuns.PushBack(fontDescriptionRun11);
  fontDescriptionRuns.PushBack(fontDescriptionRun12);
  fontDescriptionRuns.PushBack(fontDescriptionRun13);
  fontDescriptionRuns.PushBack(fontDescriptionRun14);
  fontDescriptionRuns.PushBack(fontDescriptionRun15);
  fontDescriptionRuns.PushBack(fontDescriptionRun16);
  fontDescriptionRuns.PushBack(fontDescriptionRun17);
  Size  textArea(100.f, 300.f);
  Size  layoutSize(100.f, 380.f);
  float positions[] =
    {
      0.f, -12.f, 10.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 52.f, -9.f, 62.f, -9.f, 69.f, -13.f, 72.f, -13.f, 81.f, -0.f, //   0 ..  11
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, //  12 ..  21
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, //  22 ..  27
      58.f,
      -8.f,
      52.f,
      -6.f,
      45.f,
      -8.f,
      41.f,
      -7.f,
      39.f,
      -11.f,
      34.f,
      -0.f,
      30.f,
      -7.f,
      28.f,
      -11.f,
      23.f,
      -11.f,
      16.f,
      -8.f,
      15.f,
      -11.f,
      9.f,
      -11.f,
      4.f,
      -8.f,
      0.f,
      -0.f,
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f, //  28 ..  47
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f,
      59.f,
      -10.f,
      50.f,
      -13.f,
      46.f,
      -10.f,
      36.f,
      -10.f,
      32.f,
      -0.f, //  48 ..  58
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      0.f,
      -0.f, //  59 ..  63
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, //  64 ..  73
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, //  74 ..  85
      4.f,
      -13.f,
      13.f,
      -9.f,
      23.f,
      -9.f,
      36.f,
      -9.f,
      0.f,
      -2.f,
      0.f,
      -12.f, //  86 ..  91
      0.f,
      -13.f,
      8.f,
      -9.f,
      18.f,
      -13.f,
      22.f,
      -13.f,
      25.f,
      -9.f,
      34.f,
      -0.f,
      38.f,
      -9.f,
      50.f,
      -9.f,
      60.f,
      -9.f,
      66.f,
      -13.f,
      69.f,
      -13.f,
      79.f,
      -0.f, //  92 .. 103
      92.f,
      -8.f,
      86.f,
      -6.f,
      79.f,
      -8.f,
      75.f,
      -7.f,
      73.f,
      -11.f,
      68.f,
      -0.f,
      64.f,
      -7.f,
      62.f,
      -11.f,
      57.f,
      -11.f,
      50.f,
      -8.f,
      49.f,
      -11.f,
      43.f,
      -11.f,
      38.f,
      -8.f,
      34.f,
      -0.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      10.f,
      -10.f,
      0.f,
      -10.f,
      99.f,
      -0.f, // 104 .. 122
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -0.f, // 123 .. 127
      0.f,
      -12.f,
      10.f,
      -9.f,
      20.f,
      -13.f,
      24.f,
      -13.f,
      27.f,
      -9.f,
      36.f,
      -0.f,
      40.f,
      -9.f,
      52.f,
      -9.f,
      62.f,
      -9.f,
      69.f,
      -13.f,
      72.f,
      -13.f,
      81.f,
      -0.f, // 128 .. 139
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      68.f,
      -10.f,
      59.f,
      -13.f,
      54.f,
      -10.f,
      45.f,
      -10.f,
      79.f,
      -0.f, // 140 .. 149
      23.f,
      -10.f,
      19.f,
      -10.f,
      10.f,
      -13.f,
      0.f,
      -10.f,
      32.f,
      -2.f,
      36.f,
      -12.f, // 150 .. 155
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f, // 156 .. 165
      4.f,
      -13.f,
      12.f,
      -9.f,
      22.f,
      -13.f,
      26.f,
      -13.f,
      29.f,
      -9.f,
      38.f,
      -0.f,
      42.f,
      -9.f,
      54.f,
      -9.f,
      64.f,
      -9.f,
      70.f,
      -13.f,
      73.f,
      -13.f,
      0.f,
      -0.f, // 166 .. 177
      54.f,
      -8.f,
      49.f,
      -6.f,
      41.f,
      -8.f,
      37.f,
      -7.f,
      35.f,
      -11.f,
      31.f,
      -0.f,
      26.f,
      -7.f,
      24.f,
      -11.f,
      19.f,
      -11.f,
      12.f,
      -8.f,
      11.f,
      -11.f,
      5.f,
      -11.f,
      0.f,
      -8.f,
      0.f,
      -0.f, // 178 .. 191
    };

  struct LineRun line01 =
    {
      {0u, 12u},
      {0u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line02 =
    {
      {12u, 10u},
      {12u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line03 =
    {
      {22u, 6u},
      {22u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line04 =
    {
      {28u, 14u},
      {28u, 14u},
      63.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line05 =
    {
      {42u, 12u},
      {42u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line06 =
    {
      {54u, 10u},
      {54u, 10u},
      70.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line07 =
    {
      {64u, 10u},
      {64u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line08 =
    {
      {74u, 12u},
      {74u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      1.f,
      0.f,
      false,
      false};
  struct LineRun line09 =
    {
      {86u, 6u},
      {86u, 6u},
      45.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line10 =
    {
      {92u, 12u},
      {92u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line11 =
    {
      {104u, 19u},
      {104u, 19u},
      100.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line12 =
    {
      {123u, 5u},
      {123u, 5u},
      32.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line13 =
    {
      {128u, 12u},
      {128u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line14 =
    {
      {140u, 10u},
      {140u, 10u},
      79.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line15 =
    {
      {150u, 6u},
      {150u, 6u},
      35.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line16 =
    {
      {156u, 10u},
      {156u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line17 =
    {
      {166u, 12u},
      {166u, 12u},
      79.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line18 =
    {
      {178u, 14u},
      {178u, 14u},
      63.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line19 =
    {
      {192u, 0u},
      {192u, 0u},
      0.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines;
  lines.PushBack(line01);
  lines.PushBack(line02);
  lines.PushBack(line03);
  lines.PushBack(line04);
  lines.PushBack(line05);
  lines.PushBack(line06);
  lines.PushBack(line07);
  lines.PushBack(line08);
  lines.PushBack(line09);
  lines.PushBack(line10);
  lines.PushBack(line11);
  lines.PushBack(line12);
  lines.PushBack(line13);
  lines.PushBack(line14);
  lines.PushBack(line15);
  lines.PushBack(line16);
  lines.PushBack(line17);
  lines.PushBack(line18);
  lines.PushBack(line19);

  LayoutTextData data =
    {
      "Layout bidirectional text.",
      "Hello world demo שלום עולם.\n"
      "مرحبا بالعالم hello world שלום עולם\n"
      "שלום עולם hello world demo.\n"
      "hello world مرحبا بالعالم שלום עולם\n"
      "Hello world demo שלום עולם.\n"
      "שלום עולם hello world مرحبا بالعالم\n",
      textArea,
      17u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      192u,
      positions,
      19u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      128u,
      64u,
      false,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis01");

  // Layout single-line LTR text with ellipsis.

  const std::string fontLatin("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 51u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);

  struct LineRun line01 =
    {
      {0u, 13u},
      {0u, 13u},
      93.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      true};
  Vector<LineRun> lines;
  lines.PushBack(line01);

  float positions[] =
    {
      0.f,
      -12.f,
      10.f,
      -9.f,
      20.f,
      -13.f,
      24.f,
      -13.f,
      27.f,
      -9.f,
      36.f,
      -0.f,
      40.f,
      -9.f,
      52.f,
      -9.f,
      62.f,
      -9.f,
      69.f,
      -13.f,
      72.f,
      -13.f,
      81.f,
      -0.f,
      85.f,
      -13.f,
    };

  Size textArea(100.f, 50.f);
  Size layoutSize(100.f, 20.f);

  LayoutTextData data =
    {
      "Layout single-line LTR text with ellipsis.",
      "Hello world demo hello world demo hello world demo.",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      13u,
      positions,
      1u,
      lines.Begin(),
      Layout::Engine::SINGLE_LINE_BOX,
      0u,
      51u,
      true,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis02");

  // Layout multi-line LTR text with ellipsis.

  const std::string fontLatin("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 51u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);

  struct LineRun line01 =
    {
      {0u, 12u},
      {0u, 12u},
      81.f,
      15.f,
      -5.f,
      4.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line02 =
    {
      {12u, 12u},
      {12u, 12u},
      95.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      true};
  Vector<LineRun> lines;
  lines.PushBack(line01);
  lines.PushBack(line02);

  float positions[] =
    {
      0.f,
      -12.f,
      10.f,
      -9.f,
      20.f,
      -13.f,
      24.f,
      -13.f,
      27.f,
      -9.f,
      36.f,
      -0.f,
      40.f,
      -9.f,
      52.f,
      -9.f,
      62.f,
      -9.f,
      69.f,
      -13.f,
      72.f,
      -13.f,
      81.f,
      -0.f,
      0.f,
      -13.f,
      9.f,
      -9.f,
      19.f,
      -9.f,
      32.f,
      -9.f,
      41.f,
      -0.f,
      46.f,
      -13.f,
      54.f,
      -9.f,
      64.f,
      -13.f,
      68.f,
      -13.f,
      71.f,
      -9.f,
      80.f,
      -0.f,
      84.f,
      -9.f,
    };

  Size textArea(100.f, 50.f);
  Size layoutSize(100.f, 40.f);

  LayoutTextData data =
    {
      "Layout multi-line LTR text with ellipsis.",
      "Hello world demo hello world demo hello world demo.",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      24u,
      positions,
      2u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      51u,
      true,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis03");

  // Layout single-line RTL text with ellipsis.

  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength                    = fontHebrew.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength                    = fontArabic.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun03.familyLength                    = fontHebrew.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontHebrew.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 34u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength                    = fontArabic.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength                    = fontHebrew.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  struct LineRun line01 =
    {
      {0u, 14u},
      {0u, 14u},
      98.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      true};
  Vector<LineRun> lines;
  lines.PushBack(line01);

  float positions[] =
    {
      87.f,
      -10.f,
      79.f,
      -13.f,
      74.f,
      -10.f,
      65.f,
      -10.f,
      60.f,
      -0.f,
      51.f,
      -10.f,
      47.f,
      -10.f,
      38.f,
      -13.f,
      29.f,
      -10.f,
      24.f,
      -0.f,
      17.f,
      -8.f,
      12.f,
      -6.f,
      4.f,
      -8.f,
      0.f,
      -7.f,
      78.f,
      -11.f,
      73.f,
      -0.f,
    };

  Size textArea(100.f, 50.f);
  Size layoutSize(100.f, 20.f);

  LayoutTextData data =
    {
      "Layout single-line RTL text with ellipsis.",
      "שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      16u,
      positions,
      1u,
      lines.Begin(),
      Layout::Engine::SINGLE_LINE_BOX,
      0u,
      72u,
      true,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis04");

  // Layout multi-line RTL text with ellipsis.

  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength                    = fontHebrew.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength                    = fontArabic.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun03.familyLength                    = fontHebrew.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontHebrew.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 34u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength                    = fontArabic.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength                    = fontHebrew.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  struct LineRun line01 =
    {
      {0u, 10u},
      {0u, 10u},
      70.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      false};
  struct LineRun line02 =
    {
      {10u, 19u},
      {10u, 19u},
      99.f,
      15.f,
      -5.f,
      5.f,
      0.f,
      0.f,
      false,
      true};
  Vector<LineRun> lines;
  lines.PushBack(line01);
  lines.PushBack(line02);

  float positions[] =
    {
      63.f,
      -10.f,
      54.f,
      -13.f,
      50.f,
      -10.f,
      41.f,
      -10.f,
      36.f,
      -0.f,
      27.f,
      -10.f,
      23.f,
      -10.f,
      14.f,
      -13.f,
      4.f,
      -10.f,
      0.f,
      -0.f,
      96.f,
      -8.f,
      91.f,
      -6.f,
      83.f,
      -8.f,
      79.f,
      -7.f,
      78.f,
      -11.f,
      73.f,
      -0.f,
      68.f,
      -7.f,
      67.f,
      -11.f,
      61.f,
      -11.f,
      55.f,
      -8.f,
      53.f,
      -11.f,
      47.f,
      -11.f,
      42.f,
      -8.f,
      38.f,
      -0.f,
      27.f,
      -10.f,
      18.f,
      -13.f,
      14.f,
      -10.f,
      4.f,
      -10.f,
      0.f,
      -0.f,
      92.f,
      -10.f,
      88.f,
      -10.f,
      79.f,
      -13.f,
      70.f,
      -10.f,
      65.f,
      -0.f,
    };

  Size textArea(100.f, 50.f);
  Size layoutSize(100.f, 40.f);

  LayoutTextData data =
    {
      "Layout multi-line RTL text with ellipsis.",
      "שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      34u,
      positions,
      2u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      72u,
      true,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis05");

  const std::string fontLatin("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 51u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);

  struct LineRun line01 =
    {
      {0u, 11u},
      {0u, 11u},
      80.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      true};
  Vector<LineRun> lines;
  lines.PushBack(line01);

  float positions[] =
    {
      0.f, -12.f};

  Size textArea(100.f, 19.f);
  Size layoutSize(100.f, 20.f);

  LayoutTextData data =
    {
      "Not enough height.",
      "Hello world",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      layoutSize,
      1u,
      positions,
      1u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      11u,
      true,
      DevelText::EllipsisPosition::END,
      true};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign01");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Begin alignment for the first paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::BEGIN,
      Text::VerticalAlignment::TOP,
      0u,
      22u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign02");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 33.f, 19.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Begin alignment for the mid paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::BEGIN,
      Text::VerticalAlignment::TOP,
      22u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign03");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Begin alignment for the last paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::BEGIN,
      Text::VerticalAlignment::TOP,
      48u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign04");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {9.f, 15.f, 0.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Center alignment for the first paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::CENTER,
      Text::VerticalAlignment::TOP,
      0u,
      22u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign05");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 14.f, 9.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Center alignment for the mid paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::CENTER,
      Text::VerticalAlignment::TOP,
      22u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign06");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 0.f, 0.f, 9.f, 16.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Center alignment for the last paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::CENTER,
      Text::VerticalAlignment::TOP,
      48u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign07");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {19.f, 30.f, 0.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "End alignment for the first paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::END,
      Text::VerticalAlignment::TOP,
      0u,
      22u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign08(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign08");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, -4.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "End alignment for the mid paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::END,
      Text::VerticalAlignment::TOP,
      22u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign09(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign09");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 0.f, 0.f, 19.f, 33.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "End alignment for the last paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::END,
      Text::VerticalAlignment::TOP,
      48u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      false};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign10(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign10");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Begin alignment for the first paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::END,
      Text::VerticalAlignment::TOP,
      0u,
      22u,
      6u,
      positions,
      Dali::LayoutDirection::RIGHT_TO_LEFT,
      true};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign11(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign11");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {19.f, 30.f, 33.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "End alignment for the last paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::END,
      Text::VerticalAlignment::TOP,
      0u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      true};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign12(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign12");

  // Calculate text alignment.

  const std::string fontLatin("TizenSans");
  const std::string fontHebrew("TizenSansHebrew");
  const std::string fontArabic("TizenSansArabic");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength                    = fontLatin.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength                    = fontHebrew.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength                    = fontArabic.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex     = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength                    = fontLatin.size();
  fontDescriptionRun04.familyName                      = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength);
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined  = false;
  fontDescriptionRun04.slantDefined  = false;
  fontDescriptionRun04.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex     = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength                    = fontLatin.size();
  fontDescriptionRun05.familyName                      = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength);
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined  = false;
  fontDescriptionRun05.slantDefined  = false;
  fontDescriptionRun05.sizeDefined   = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex     = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength                    = fontArabic.size();
  fontDescriptionRun06.familyName                      = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength);
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined  = false;
  fontDescriptionRun06.slantDefined  = false;
  fontDescriptionRun06.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun01);
  fontDescriptionRuns.PushBack(fontDescriptionRun02);
  fontDescriptionRuns.PushBack(fontDescriptionRun03);
  fontDescriptionRuns.PushBack(fontDescriptionRun04);
  fontDescriptionRuns.PushBack(fontDescriptionRun05);
  fontDescriptionRuns.PushBack(fontDescriptionRun06);

  float positions[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

  Size      textArea(100.f, 300.f);
  AlignData data =
    {
      "Begin alignment for the first paragraph.",
      "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
      textArea,
      6u,
      fontDescriptionRuns.Begin(),
      Text::HorizontalAlignment::BEGIN,
      Text::VerticalAlignment::TOP,
      48u,
      26u,
      6u,
      positions,
      Dali::LayoutDirection::LEFT_TO_RIGHT,
      true};

  if(!AlignTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetDefaultLineSpacing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetDefaultLineSpacing");

  Layout::Engine engine;
  DALI_TEST_EQUALS(0.f, engine.GetDefaultLineSpacing(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  engine.SetDefaultLineSpacing(10.f);
  DALI_TEST_EQUALS(10.f, engine.GetDefaultLineSpacing(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutGetGlyphMetrics(void)
{
  tet_infoline(" UtcDaliTextLayoutGetGlyphMetrics");

  // Test retrieving metrics from group of characters

  const std::string fontFamily("TizenSansHindi");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex     = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun01.familyLength                    = fontFamily.size();
  fontDescriptionRun01.familyName                      = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontFamily.c_str(), fontDescriptionRun01.familyLength);
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined  = false;
  fontDescriptionRun01.slantDefined  = false;
  fontDescriptionRun01.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex     = 0u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun02.familyLength                    = fontFamily.size();
  fontDescriptionRun02.familyName                      = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontFamily.c_str(), fontDescriptionRun02.familyLength);
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined  = false;
  fontDescriptionRun02.slantDefined  = false;
  fontDescriptionRun02.sizeDefined   = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex     = 0u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 2u;
  fontDescriptionRun03.familyLength                    = fontFamily.size();
  fontDescriptionRun03.familyName                      = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontFamily.c_str(), fontDescriptionRun03.familyLength);
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined  = false;
  fontDescriptionRun03.slantDefined  = false;
  fontDescriptionRun03.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns01;
  fontDescriptionRuns01.PushBack(fontDescriptionRun01);

  Vector<FontDescriptionRun> fontDescriptionRuns02;
  fontDescriptionRuns02.PushBack(fontDescriptionRun02);

  Vector<FontDescriptionRun> fontDescriptionRuns03;
  fontDescriptionRuns03.PushBack(fontDescriptionRun03);

  // Set a text area.
  Size textArea(100.f, 100.f);

  // Group: second glyph doesn't exceed the width of the first glyph
  float positions01[] = {0.f, -11.f};

  struct LineRun line01 =
    {
      {0u, 1u},
      {0u, 1u},
      11.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines01;
  lines01.PushBack(line01);

  Size layoutSize01 = Vector2(11.f, 20.f);

  // Group: second glyph doesn't exceed the width of the first glyph
  float positions02[] = {
    0.f,
    -11.f,
    7.f,
    -15.f,
  };

  struct LineRun line02 =
    {
      {0u, 2u},
      {0u, 2u},
      16.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines02;
  lines02.PushBack(line02);

  Size layoutSize02 = Vector2(16.f, 20.f);

  // Group: second glyph doesn't exceed the width of the first glyph
  float positions03[] = {0.f, -11.f, 2.f, -15.f};

  struct LineRun line03 =
    {
      {0u, 2u},
      {0u, 2u},
      11.f,
      15.f,
      -5.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  Vector<LineRun> lines03;
  lines03.PushBack(line03);

  Size layoutSize03 = Vector2(11.f, 20.f);

  /////////////////////////////

  struct LayoutTextData data[] =
    {
      {"Single glyph",
       "प",
       textArea,
       1u,
       fontDescriptionRuns01.Begin(),
       layoutSize01,
       1u,
       positions01,
       1u,
       lines01.Begin(),
       Layout::Engine::SINGLE_LINE_BOX,
       0u,
       1u,
       false,
       DevelText::EllipsisPosition::END,
       true},
      {"Group: second glyph exceeds the width of the first glyph",
       "पो",
       textArea,
       1u,
       fontDescriptionRuns02.Begin(),
       layoutSize02,
       2u,
       positions02,
       1u,
       lines02.Begin(),
       Layout::Engine::SINGLE_LINE_BOX,
       0u,
       2u,
       false,
       DevelText::EllipsisPosition::END,
       true},
      {"Group: second glyph doesn't exceed the width of the first glyph",
       "पे",
       textArea,
       1u,
       fontDescriptionRuns03.Begin(),
       layoutSize03,
       2u,
       positions03,
       1u,
       lines03.Begin(),
       Layout::Engine::SINGLE_LINE_BOX,
       0u,
       2u,
       false,
       DevelText::EllipsisPosition::END,
       true}};
  const unsigned int numberOfTests = sizeof(data) / sizeof(LayoutTextData);

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!LayoutTextTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
