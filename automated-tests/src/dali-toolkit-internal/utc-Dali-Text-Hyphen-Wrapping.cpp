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

namespace
{
const std::string DEFAULT_FONT_DIR("/resources/fonts");

struct LayoutTextData
{
  std::string          text;
  Size                 textArea;
  unsigned int         numberOfFonts;
  FontDescriptionRun*  fontDescriptions;
  unsigned int         numberOfLines;
  LineRun*             lines;
  Layout::Engine::Type layout;
  unsigned int         startIndex;
  unsigned int         numberOfGlyphs;
  Text::LineWrap::Mode wrapMode;
};

void Print(const LineRun& line)
{
  std::cout << "        glyph run, index : " << line.glyphRun.glyphIndex << ", num glyphs : " << line.glyphRun.numberOfGlyphs << std::endl;
  std::cout << "    character run, index : " << line.characterRun.characterIndex << ", num chars : " << line.characterRun.numberOfCharacters << std::endl;
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
                  data.wrapMode,
                  false,
                  Toolkit::DevelText::EllipsisPosition::END,
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  Vector<LineRun>& lines = textModel->mVisualModel->mLines;

  // 4) Compare the results.

  if(lines.Count() != data.numberOfLines)
  {
    std::cout << "  Different number of lines : " << lines.Count() << ", expected : " << data.numberOfLines << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < data.numberOfLines; ++index)
  {
    const LineRun& line         = *(lines.Begin() + index);
    const LineRun& expectedLine = *(data.lines + index);

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
  }

  return true;
}

} // namespace

int UtcDaliTextHyphenWrapping(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextHyphenWrapping");

  // Layout some lines of left to right text.

  const std::string fontFamily("TizenSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 13u;
  fontDescriptionRun1.familyLength                    = fontFamily.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun1);
  Size textArea(65.0f, 200.f);

  LineRun line1 =
    {
      {0u, 5u},
      {0u, 5u},
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  LineRun line2 =
    {
      {5u, 8u},
      {5u, 8u},
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      false,
      false};

  Vector<LineRun> lines;
  lines.PushBack(line1);
  lines.PushBack(line2);

  LayoutTextData data =
    {
      "Hi Experiment",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      2u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      13u,
      (Text::LineWrap::Mode)DevelText::LineWrap::HYPHENATION};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMixedWrapping(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMixedWrapping");

  // Layout some lines of left to right text.

  const std::string fontFamily("DejaVuSans");

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex     = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 13u;
  fontDescriptionRun1.familyLength                    = fontFamily.size();
  fontDescriptionRun1.familyName                      = new char[fontDescriptionRun1.familyLength];
  memcpy(fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength);
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined  = false;
  fontDescriptionRun1.slantDefined  = false;
  fontDescriptionRun1.sizeDefined   = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack(fontDescriptionRun1);
  Size textArea(72.0f, 200.f);

  LineRun line1 =
    {
      {0u, 3u},
      {0u, 3u},
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  LineRun line2 =
    {
      {3u, 6u},
      {3u, 6u},
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      false,
      false};
  LineRun line3 =
    {
      {9u, 4u},
      {9u, 4u},
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      0.f,
      false,
      false};

  Vector<LineRun> lines;
  lines.PushBack(line1);
  lines.PushBack(line2);
  lines.PushBack(line3);

  LayoutTextData data =
    {
      "Hi Experiment",
      textArea,
      1u,
      fontDescriptionRuns.Begin(),
      3u,
      lines.Begin(),
      Layout::Engine::MULTI_LINE_BOX,
      0u,
      13u,
      (Text::LineWrap::Mode)DevelText::LineWrap::MIXED};

  if(!LayoutTextTest(data))
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}