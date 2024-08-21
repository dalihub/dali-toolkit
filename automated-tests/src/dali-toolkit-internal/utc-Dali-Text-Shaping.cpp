/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/shaper.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following function.
// void ShapeText( const Vector<Character>& text,
//                 const Vector<LineBreakInfo>& lineBreakInfo,
//                 const Vector<ScriptRun>& scripts,
//                 const Vector<FontRun>& fonts,
//                 CharacterIndex startCharacterIndex,
//                 GlyphIndex startGlyphIndex,
//                 Length numberOfCharacters,
//                 Vector<GlyphInfo>& glyphs,
//                 Vector<CharacterIndex>& glyphToCharacterMap,
//                 Vector<Length>& charactersPerGlyph,
//                 Vector<GlyphIndex>& newParagraphGlyphs );

//////////////////////////////////////////////////////////

namespace
{
const std::string DEFAULT_FONT_DIR("/resources/fonts");

struct GlyphInfoData
{
  FontId     fontId;           ///< Identifies the font containing the glyph
  GlyphIndex index;            ///< Uniquely identifies a glyph for a given FontId
  float      width;            ///< The width of the glyph
  float      height;           ///< The height of the glyph
  float      xBearing;         ///< The distance from the cursor position to the leftmost border of the glyph
  float      yBearing;         ///< The distance from the baseline to the topmost border of the glyph
  float      advance;          ///< The distance to move the cursor for this glyph
  float      scaleFactor;      ///< The scaling applied (fixed-size fonts only)
  bool       isItalicRequired; ///< Whether the italic style is required.
  bool       isBoldRequired;   ///< Whether the bold style is required.
};

bool IsEqualGlyph(const GlyphInfoData& glyphData, const GlyphInfo& glyph)
{
  if(glyphData.fontId != glyph.fontId)
  {
    return false;
  }
  if(glyphData.index != glyph.index)
  {
    return false;
  }
  if(fabsf(glyphData.width - glyph.width) > Math::MACHINE_EPSILON_1000)
  {
    return false;
  }
  if(fabsf(glyphData.height - glyph.height) > Math::MACHINE_EPSILON_1000)
  {
    return false;
  }
  if(fabsf(glyphData.xBearing - glyph.xBearing) > Math::MACHINE_EPSILON_1000)
  {
    return false;
  }
  if(fabsf(glyphData.yBearing - glyph.yBearing) > Math::MACHINE_EPSILON_1000)
  {
    return false;
  }
  if(glyphData.advance >= 0.0f && fabsf(glyphData.advance - round(glyph.advance)) > Math::MACHINE_EPSILON_1000)
  {
    return false;
  }
  if(fabsf(glyphData.scaleFactor - glyph.scaleFactor) > Math::MACHINE_EPSILON_1000)
  {
    return false;
  }
  if(glyphData.isItalicRequired != glyph.isItalicRequired)
  {
    return false;
  }
  if(glyphData.isBoldRequired != glyph.isBoldRequired)
  {
    return false;
  }

  return true;
}

struct ShapeInfoData
{
  std::string                description;                        ///< Description of the test.
  std::string                text;                               ///< input text.
  uint32_t                   index;                              ///< The index from where to start to query the break info.
  uint32_t                   numberOfCharacters;                 ///< The requested number of characters.
  uint32_t                   expectedNumberOfGlyphs;             ///< The expected number of glyphs.
  GlyphInfoData*             glyphs;                             ///< The glyphs.
  CharacterIndex*            characterIndices;                   ///< The character index for each glyph.
  Length*                    charactersPerGlyph;                 ///< The characters per glyph.
  uint32_t                   expectedNumberOfNewParagraphGlyphs; ///< The expected number of glyphs.
  GlyphIndex*                newParagraphGlyphs;                 ///< Indices to the new paragraphs glyphs.
  bool                       markupProcessorEnabled;             //< Enable markup processor to use markup text.
  Vector<FontDescriptionRun> fontDescriptions;                   ///< Fonts which is used for text.
};

bool ShapeInfoTest(const ShapeInfoData& data)
{
  // 1) Create the model.
  ModelPtr   textModel;
  MetricsPtr metrics;
  Size       textArea(100.f, 60.f);
  Size       layoutSize;

  const Vector<FontDescriptionRun> fontDescriptions;
  const LayoutOptions              options;

  CreateTextModel(data.text,
                  textArea,
                  data.fontDescriptions,
                  options,
                  layoutSize,
                  textModel,
                  metrics,
                  data.markupProcessorEnabled,
                  LineWrap::WORD,
                  false,
                  Toolkit::DevelText::EllipsisPosition::END,
                  0.0f, // lineSpacing
                  0.0f  // characterSpacing
  );

  LogicalModelPtr logicalModel = textModel->mLogicalModel;
  VisualModelPtr  visualModel  = textModel->mVisualModel;

  // 2) Clear the model.

  Vector<GlyphInfo>&      glyphs             = visualModel->mGlyphs;
  Vector<CharacterIndex>& glyphToCharacter   = visualModel->mGlyphsToCharacters;
  Vector<Length>&         charactersPerGlyph = visualModel->mCharactersPerGlyph;
  Vector<GlyphIndex>&     charactersToGlyph  = visualModel->mCharactersToGlyph;
  Vector<Length>&         glyphsPerCharacter = visualModel->mGlyphsPerCharacter;

  // Get the glyph index.
  GlyphIndex glyphIndex = 0u;
  if(0u != visualModel->mCharactersToGlyph.Count())
  {
    glyphIndex = *(visualModel->mCharactersToGlyph.Begin() + data.index);

    const CharacterIndex lastCharacterIndex = data.index + data.numberOfCharacters - 1u;
    const Length         numberOfGlyphs     = *(visualModel->mCharactersToGlyph.Begin() + lastCharacterIndex) + *(visualModel->mGlyphsPerCharacter.Begin() + lastCharacterIndex) - glyphIndex;

    // Erase the glyph info from the text model.
    // Got from the ShapeText() function.
    glyphs.Erase(glyphs.Begin() + glyphIndex, glyphs.Begin() + glyphIndex + numberOfGlyphs);
    glyphToCharacter.Erase(glyphToCharacter.Begin() + glyphIndex, glyphToCharacter.Begin() + glyphIndex + numberOfGlyphs);
    charactersPerGlyph.Erase(charactersPerGlyph.Begin() + glyphIndex, charactersPerGlyph.Begin() + glyphIndex + numberOfGlyphs);

    // Got from the VisualModel::CreateCharacterToGlyphTable() and the VisualModel::CreateGlyphsPerCharacterTable() methods.
    charactersToGlyph.Erase(charactersToGlyph.Begin() + data.index,
                            charactersToGlyph.Begin() + data.index + data.numberOfCharacters);
    glyphsPerCharacter.Erase(glyphsPerCharacter.Begin() + data.index,
                             glyphsPerCharacter.Begin() + data.index + data.numberOfCharacters);

    // Update the glyph to character indices.
    for(Vector<CharacterIndex>::Iterator it    = glyphToCharacter.Begin() + glyphIndex,
                                         endIt = glyphToCharacter.End();
        it != endIt;
        ++it)
    {
      CharacterIndex& index = *it;
      index -= data.numberOfCharacters;
    }
  }

  // Reset the metrics got from the model as the ShapeText() function doesn't retrieve them.
  for(Vector<GlyphInfo>::Iterator it    = glyphs.Begin(),
                                  endIt = glyphs.End();
      it != endIt;
      ++it)
  {
    GlyphInfo& info  = *it;
    info.width       = 0.f;
    info.height      = 0.f;
    info.xBearing    = 0.f;
    info.yBearing    = 0.f;
    info.scaleFactor = 0.f;
  }

  // 3) Call the ShapeText() function.
  TextAbstraction::Shaping    shaping    = TextAbstraction::Shaping::Get();
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  Vector<GlyphIndex> newParagraphGlyphs;

  ShapeText(shaping,
            fontClient,
            logicalModel->mText,
            logicalModel->mLineBreakInfo,
            logicalModel->mScriptRuns,
            logicalModel->mFontRuns,
            data.index,
            glyphIndex,
            data.numberOfCharacters,
            glyphs,
            glyphToCharacter,
            charactersPerGlyph,
            newParagraphGlyphs);

  // Clear the advance of the new paragraph glyphs.
  for(Vector<GlyphIndex>::Iterator it    = newParagraphGlyphs.Begin(),
                                   endIt = newParagraphGlyphs.End();
      it != endIt;
      ++it)
  {
    GlyphInfo& info = *(glyphs.Begin() + *it);
    info.advance    = 0.f;
  }

  // 4) Compare the results.

  if(data.expectedNumberOfGlyphs != glyphs.Count())
  {
    std::cout << "  Different number of glyphs : " << glyphs.Count() << ", expected : " << data.expectedNumberOfGlyphs << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < data.expectedNumberOfGlyphs; ++index)
  {
    if(!IsEqualGlyph(data.glyphs[index], glyphs[index]))
    {
      std::cout << "  different glyph info, index : " << index << std::endl;

      const GlyphInfo& glyphInfo = glyphs[index];
      std::cout << "            fontId : " << glyphInfo.fontId << std::endl;
      std::cout << "             index : " << glyphInfo.index << std::endl;
      std::cout << "             width : " << glyphInfo.width << std::endl;
      std::cout << "            height : " << glyphInfo.height << std::endl;
      std::cout << "          xBearing : " << glyphInfo.xBearing << std::endl;
      std::cout << "          yBearing : " << glyphInfo.yBearing << std::endl;
      std::cout << "           advance : " << round(glyphInfo.advance) << std::endl;
      std::cout << "       scaleFactor : " << glyphInfo.scaleFactor << std::endl;
      std::cout << "  isItalicRequired : " << glyphInfo.isItalicRequired << std::endl;
      std::cout << "    isBoldRequired : " << glyphInfo.isBoldRequired << std::endl;

      std::cout << "  Expected : " << std::endl;
      const GlyphInfoData& expectedGlyphInfo = data.glyphs[index];
      std::cout << "            fontId : " << expectedGlyphInfo.fontId << std::endl;
      std::cout << "             index : " << expectedGlyphInfo.index << std::endl;
      std::cout << "             width : " << expectedGlyphInfo.width << std::endl;
      std::cout << "            height : " << expectedGlyphInfo.height << std::endl;
      std::cout << "          xBearing : " << expectedGlyphInfo.xBearing << std::endl;
      std::cout << "          yBearing : " << expectedGlyphInfo.yBearing << std::endl;
      std::cout << "           advance : " << expectedGlyphInfo.advance << std::endl;
      std::cout << "       scaleFactor : " << expectedGlyphInfo.scaleFactor << std::endl;
      std::cout << "  isItalicRequired : " << expectedGlyphInfo.isItalicRequired << std::endl;
      std::cout << "    isBoldRequired : " << expectedGlyphInfo.isBoldRequired << std::endl;

      return false;
    }
  }

  for(unsigned int index = 0u; index < data.expectedNumberOfGlyphs; ++index)
  {
    if(data.characterIndices[index] != glyphToCharacter[index])
    {
      std::cout << "  different character index, index : " << index << std::endl;
      return false;
    }
  }

  for(unsigned int index = 0u; index < data.expectedNumberOfGlyphs; ++index)
  {
    if(data.charactersPerGlyph[index] != charactersPerGlyph[index])
    {
      std::cout << "  different character per glyph, index : " << index << std::endl;
      return false;
    }
  }

  if(data.expectedNumberOfNewParagraphGlyphs != newParagraphGlyphs.Count())
  {
    std::cout << "  Different number of new paragraph glyphs : " << newParagraphGlyphs.Count() << ", expected : " << data.expectedNumberOfNewParagraphGlyphs << std::endl;
    return false;
  }

  for(unsigned int index = 0u; index < data.expectedNumberOfNewParagraphGlyphs; ++index)
  {
    if(data.newParagraphGlyphs[index] != newParagraphGlyphs[index])
    {
      std::cout << "  different new paragraph glyph, index : " << index << std::endl;
      return false;
    }
  }

  return true;
}

void LoadTextShapeFonts()
{
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/noto/NotoSansMalayalam-Regular.ttf");
}

void LoadSoftwareStylingFonts()
{
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/roboto/Roboto-Regular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/roboto/Roboto-Bold.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/roboto/Roboto-Italic.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/roboto/Roboto-BoldItalic.ttf");
}

void LoadEmojiFonts()
{
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.ClearCache();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  TextAbstraction::FontDescription fontDescriptionText;
  fontDescriptionText.path   = "";
  fontDescriptionText.family = "DejaVuSans";
  fontDescriptionText.width  = TextAbstraction::FontWidth::NONE;
  fontDescriptionText.weight = TextAbstraction::FontWeight::NORMAL;
  fontDescriptionText.slant  = TextAbstraction::FontSlant::NONE;
  fontClient.GetFontId(fontDescriptionText, TextAbstraction::FontClient::DEFAULT_POINT_SIZE);

  TextAbstraction::FontDescription fontDescriptionEmoji;
  fontDescriptionEmoji.path   = "";
  fontDescriptionEmoji.family = "NotoColorEmoji";
  fontDescriptionEmoji.width  = TextAbstraction::FontWidth::NONE;
  fontDescriptionEmoji.weight = TextAbstraction::FontWeight::NORMAL;
  fontDescriptionEmoji.slant  = TextAbstraction::FontSlant::NONE;
  fontClient.GetFontId(fontDescriptionEmoji, TextAbstraction::FontClient::DEFAULT_POINT_SIZE);
}

} // namespace

//////////////////////////////////////////////////////////

int UtcDaliTextShape(void)
{
  tet_infoline(" UtcDaliTextShape");

  struct GlyphInfoData glyphs02[] =
    {
      {1u, 276u, 0.f, 0.f, 0.f, 0.f, 11.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 317u, 0.f, 0.f, 0.f, 0.f, 12.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 312u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 298u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
    };

  CharacterIndex characterIndices02[]   = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u};
  Length         charactersPerGlyph02[] = {1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u};

  struct GlyphInfoData glyphs03[] =
    {
      {1u, 276u, 0.f, 0.f, 0.f, 0.f, 11.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 317u, 0.f, 0.f, 0.f, 0.f, 12.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 312u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 298u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 0u, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
      {1u, 298u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 307u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 0u, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
    };

  CharacterIndex characterIndices03[]   = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u};
  Length         charactersPerGlyph03[] = {1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u};
  CharacterIndex newParagraphGlyphs03[] = {11u, 16u};

  struct GlyphInfoData glyphs04[] =
    {
      {2u, 67u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f},
      {2u, 27u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f},
      {2u, 59u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {2u, 67u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f},
      {2u, 55u, 0.f, 0.f, 0.f, 0.f, 20.f, 0.f},
      {2u, 59u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {2u, 67u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f},
      {2u, 56u, 0.f, 0.f, 0.f, 0.f, 19.f, 0.f},
      {2u, 59u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {2u, 67u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f},
      {2u, 52u, 0.f, 0.f, 0.f, 0.f, 15.f, 0.f},
      {2u, 59u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
    };

  CharacterIndex characterIndices04[]   = {0u, 0u, 0u, 2u, 2u, 2u, 4u, 4u, 4u, 6u, 6u, 6u};
  Length         charactersPerGlyph04[] = {0u, 0u, 2u, 0u, 0u, 2u, 0u, 0u, 2u, 0u, 0u, 2u};

  struct GlyphInfoData glyphs05[] =
    {
      {1u, 280u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 312u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 307u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 303u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 310u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 313u, 0.f, 0.f, 0.f, 0.f, 7.f, 0.f},
      {1u, 315u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 307u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 298u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 306u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 312u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 313u, 0.f, 0.f, 0.f, 0.f, 7.f, 0.f},
      {1u, 303u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 314u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 295u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 307u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 314u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 0u, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
      {1u, 295u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 311u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 315u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 298u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 403u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f},
      {1u, 308u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 303u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 296u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 295u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 313u, 0.f, 0.f, 0.f, 0.f, 7.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 295u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 307u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 303u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 0u, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
      {1u, 310u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 309u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 313u, 0.f, 0.f, 0.f, 0.f, 7.f, 0.f},
      {1u, 313u, 0.f, 0.f, 0.f, 0.f, 7.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 303u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 312u, 0.f, 0.f, 0.f, 0.f, 6.f, 0.f},
      {1u, 295u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 297u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 315u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 308u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 298u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 303u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 295u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 308u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 299u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 297u, 0.f, 0.f, 0.f, 0.f, 8.f, 0.f},
      {1u, 315u, 0.f, 0.f, 0.f, 0.f, 9.f, 0.f},
      {1u, 307u, 0.f, 0.f, 0.f, 0.f, 14.f, 0.f},
      {1u, 4u, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f},
      {1u, 0u, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
    };

  CharacterIndex characterIndices05[]   = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 37u, 38u, 39u, 40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u, 48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 64u, 65u, 66u, 67u, 68u, 69u, 70u, 71u, 72u, 73u, 74u};
  Length         charactersPerGlyph05[] = {1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 2u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u};
  CharacterIndex newParagraphGlyphs05[] = {26u};
  CharacterIndex newParagraphGlyphs06[] = {49u};
  CharacterIndex newParagraphGlyphs07[] = {73u};

  Vector<FontDescriptionRun> fontDescriptions01;
  Vector<FontDescriptionRun> fontDescriptions02;
  Vector<FontDescriptionRun> fontDescriptions03;
  Vector<FontDescriptionRun> fontDescriptions04;
  Vector<FontDescriptionRun> fontDescriptions05;
  Vector<FontDescriptionRun> fontDescriptions06;

  const std::string fontFamily("TizenSans");
  const std::string fontFamilyMalayalam("Noto Sans Malayalam");

  FontDescriptionRun fontDescriptionRun01 =
    {
      {0u,
       11u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun01.familyLength = fontFamily.size();
  fontDescriptionRun01.familyName   = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontFamily.c_str(), fontDescriptionRun01.familyLength);

  fontDescriptions01.PushBack(fontDescriptionRun01);

  FontDescriptionRun fontDescriptionRun02 =
    {
      {0u,
       17u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun02.familyLength = fontFamily.size();
  fontDescriptionRun02.familyName   = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontFamily.c_str(), fontDescriptionRun02.familyLength);

  fontDescriptions02.PushBack(fontDescriptionRun02);

  FontDescriptionRun fontDescriptionRun03 =
    {
      {0u,
       8u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun03.familyLength = fontFamilyMalayalam.size();
  fontDescriptionRun03.familyName   = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontFamilyMalayalam.c_str(), fontDescriptionRun03.familyLength);

  fontDescriptions03.PushBack(fontDescriptionRun03);

  FontDescriptionRun fontDescriptionRun04 =
    {
      {0u,
       75u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun04.familyLength = fontFamily.size();
  fontDescriptionRun04.familyName   = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontFamily.c_str(), fontDescriptionRun04.familyLength);

  fontDescriptions04.PushBack(fontDescriptionRun04);

  FontDescriptionRun fontDescriptionRun05 =
    {
      {0u,
       75u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun05.familyLength = fontFamily.size();
  fontDescriptionRun05.familyName   = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontFamily.c_str(), fontDescriptionRun05.familyLength);

  fontDescriptions05.PushBack(fontDescriptionRun05);

  FontDescriptionRun fontDescriptionRun06 =
    {
      {0u,
       75u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun06.familyLength = fontFamily.size();
  fontDescriptionRun06.familyName   = new char[fontDescriptionRun06.familyLength];
  memcpy(fontDescriptionRun06.familyName, fontFamily.c_str(), fontDescriptionRun06.familyLength);

  fontDescriptions06.PushBack(fontDescriptionRun06);

  struct ShapeInfoData data[] =
    {
      {
        "Zero characters",
        "",
        0u,
        0u,
        0u,
        nullptr,
        nullptr,
        nullptr,
        0u,
        nullptr,
        false,
      },
      {"Latin script",
       "Hello world",
       0u,
       11u,
       11u,
       glyphs02,
       characterIndices02,
       charactersPerGlyph02,
       0u,
       nullptr,
       false,
       fontDescriptions01},
      {"Latin script. Some paragraphs.",
       "Hello world\ndemo\n",
       0u,
       17u,
       17u,
       glyphs03,
       characterIndices03,
       charactersPerGlyph03,
       2u,
       newParagraphGlyphs03,
       false,
       fontDescriptions02},
      {"Malayalam script. More glyphs than characters.",
       "ജോസോഹോവോ",
       0u,
       8u,
       12u,
       glyphs04,
       characterIndices04,
       charactersPerGlyph04,
       0u,
       nullptr,
       false,
       fontDescriptions03},
      {
        "Latin script with some paragraphs. Update initial paragraph.",
        "Lorem ipsum dolor sit amet\naeque definiebas ea mei\nposse iracundia ne cum.\n",
        0u,
        27u,
        74u,
        glyphs05,
        characterIndices05,
        charactersPerGlyph05,
        1u,
        newParagraphGlyphs05,
        false,
        fontDescriptions04,
      },
      {"Latin script with some paragraphs. Update mid paragraph.",
       "Lorem ipsum dolor sit amet\naeque definiebas ea mei\nposse iracundia ne cum.\n",
       27u,
       24u,
       74u,
       glyphs05,
       characterIndices05,
       charactersPerGlyph05,
       1u,
       newParagraphGlyphs06,
       false,
       fontDescriptions05},
      {"Latin script with some paragraphs. Update final paragraph.",
       "Lorem ipsum dolor sit amet\naeque definiebas ea mei\nposse iracundia ne cum.\n",
       51u,
       24u,
       74u,
       glyphs05,
       characterIndices05,
       charactersPerGlyph05,
       1u,
       newParagraphGlyphs07,
       false,
       fontDescriptions06},
    };
  const unsigned int numberOfTests = 7u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    LoadTextShapeFonts();

    if(!ShapeInfoTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextSoftwareStyling(void)
{
  tet_infoline(" UtcDaliTextSoftwareStyling");

  struct GlyphInfoData glyphs01[] =
    {
      {4u, 38u, 0.f, 0.f, 0.f, 0.f, 11.f, 0.f, true, true},
      {4u, 39u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, true, true},
      {4u, 40u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, true, true},
      {4u, 41u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, true, true},
    };
  struct GlyphInfoData glyphs02[] =
    {
      {1u, 38u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, false, false},
      {2u, 39u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, false, true},
      {3u, 40u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, true, false},
      {4u, 41u, 0.f, 0.f, 0.f, 0.f, 10.f, 0.f, true, true},
    };

  CharacterIndex characterIndices[]   = {0u, 1u, 2u, 3u};
  Length         charactersPerGlyph[] = {1u, 1u, 1u, 1u};

  Vector<FontDescriptionRun> fontDescriptions01;
  Vector<FontDescriptionRun> fontDescriptions02;

  const std::string fontFamily("Roboto");

  FontDescriptionRun fontDescriptionRun01 =
    {
      {0u,
       4u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::BOLD,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::ITALIC,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      true,
      false,
      true,
      false};
  fontDescriptionRun01.familyLength = fontFamily.size();
  fontDescriptionRun01.familyName   = new char[fontDescriptionRun01.familyLength];
  memcpy(fontDescriptionRun01.familyName, fontFamily.c_str(), fontDescriptionRun01.familyLength);

  fontDescriptions01.PushBack(fontDescriptionRun01);

  FontDescriptionRun fontDescriptionRun02 =
    {
      {0u,
       1u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      false,
      false};
  fontDescriptionRun02.familyLength = fontFamily.size();
  fontDescriptionRun02.familyName   = new char[fontDescriptionRun02.familyLength];
  memcpy(fontDescriptionRun02.familyName, fontFamily.c_str(), fontDescriptionRun02.familyLength);

  FontDescriptionRun fontDescriptionRun03 =
    {
      {1u,
       1u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::BOLD,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::NONE,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      true,
      false,
      false,
      false};
  fontDescriptionRun03.familyLength = fontFamily.size();
  fontDescriptionRun03.familyName   = new char[fontDescriptionRun03.familyLength];
  memcpy(fontDescriptionRun03.familyName, fontFamily.c_str(), fontDescriptionRun03.familyLength);

  FontDescriptionRun fontDescriptionRun04 =
    {
      {2u,
       1u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::NONE,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::ITALIC,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      false,
      false,
      true,
      false};
  fontDescriptionRun04.familyLength = fontFamily.size();
  fontDescriptionRun04.familyName   = new char[fontDescriptionRun04.familyLength];
  memcpy(fontDescriptionRun04.familyName, fontFamily.c_str(), fontDescriptionRun04.familyLength);

  FontDescriptionRun fontDescriptionRun05 =
    {
      {3u,
       1u},
      nullptr,
      0u,
      TextAbstraction::FontWeight::BOLD,
      TextAbstraction::FontWidth::NONE,
      TextAbstraction::FontSlant::ITALIC,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      true,
      false,
      true,
      false};
  fontDescriptionRun05.familyLength = fontFamily.size();
  fontDescriptionRun05.familyName   = new char[fontDescriptionRun05.familyLength];
  memcpy(fontDescriptionRun05.familyName, fontFamily.c_str(), fontDescriptionRun05.familyLength);

  fontDescriptions02.PushBack(fontDescriptionRun02);
  fontDescriptions02.PushBack(fontDescriptionRun03);
  fontDescriptions02.PushBack(fontDescriptionRun04);
  fontDescriptions02.PushBack(fontDescriptionRun05);

  struct ShapeInfoData data[] =
    {
      {"Latin script. Characters have same font description",
       "ABCD",
       0u,
       4u,
       4u,
       glyphs01,
       characterIndices,
       charactersPerGlyph,
       0u,
       nullptr,
       false,
       fontDescriptions01},
      {"Latin script. Each character has different font description.",
       "ABCD",
       0u,
       4u,
       4u,
       glyphs02,
       characterIndices,
       charactersPerGlyph,
       0u,
       nullptr,
       false,
       fontDescriptions02}};

  const unsigned int numberOfTests = 2u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    LoadSoftwareStylingFonts();

    if(!ShapeInfoTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextShapeEmojiSequences(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliTextShapeEmojiSequences");

  const std::string colorFontFamily("NotoColorEmoji");
  const std::string textFontFamily("DejaVuSans");

  LoadEmojiFonts();

  //Common attributes for font Descriptions
  CharacterRun    characterRun = {0u, 2u};
  FontWeight      weight       = TextAbstraction::FontWeight::NORMAL;
  FontWidth       width        = TextAbstraction::FontWidth::NORMAL;
  FontSlant       slant        = TextAbstraction::FontSlant::ITALIC;
  PointSize26Dot6 size         = TextAbstraction::FontClient::DEFAULT_POINT_SIZE;

  bool familyDefined = true;
  bool weightDefined = false;
  bool widthDefined  = false;
  bool slantDefined  = false;
  bool sizeDefined   = false;

  // variation selector 16 (Emoji)
  // Note : Non-scalable font's advance value is undefined. See https://github.com/harfbuzz/harfbuzz/discussions/3023. 2022-06-03 updated.
  struct GlyphInfoData glyphsVS16[] =
    {
      {2u, 74u, 0.f, 0.f, 0.f, 0.f, -1.0f /* Special value that we don't check it */, 0.f, false, false},
    };
  CharacterIndex characterIndicesVS16[]   = {0u, 1u};
  Length         charactersPerGlyphVS16[] = {2u};

  // variation selector 15 (Text)
  struct GlyphInfoData glyphsVS15[] =
    {
      {1u, 3842u, 0.f, 0.f, 0.f, 0.f, 14.0f, 0.f, false, false},
      {1u, 3u, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, false, false},
    };
  CharacterIndex characterIndicesVS15[]   = {0u, 0u};
  Length         charactersPerGlyphVS15[] = {0u, 2u};

  //Font Descriptions
  Vector<FontDescriptionRun> fontDescriptionsColorVS16 =
    CreateSingleFontDescription(characterRun, colorFontFamily, weight, width, slant, size, familyDefined, weightDefined, widthDefined, slantDefined, sizeDefined);

  Vector<FontDescriptionRun> fontDescriptionsColorVS15 =
    CreateSingleFontDescription(characterRun, colorFontFamily, weight, width, slant, size, familyDefined, weightDefined, widthDefined, slantDefined, sizeDefined);

  Vector<FontDescriptionRun> fontDescriptionsTextVS16 =
    CreateSingleFontDescription(characterRun, textFontFamily, weight, width, slant, size, familyDefined, weightDefined, widthDefined, slantDefined, sizeDefined);

  Vector<FontDescriptionRun> fontDescriptionsTextVS15 =
    CreateSingleFontDescription(characterRun, textFontFamily, weight, width, slant, size, familyDefined, weightDefined, widthDefined, slantDefined, sizeDefined);

  struct ShapeInfoData data[] =
    {
      {"EMOJI Sequence: Color Font with VS16",
       "&#x262a;&#xfe0f;",
       0u,
       2u,
       1u,
       glyphsVS16,
       characterIndicesVS16,
       charactersPerGlyphVS16,
       0u,
       nullptr,
       true,
       fontDescriptionsColorVS16},
      {"EMOJI Sequence: Color Font with VS15",
       "&#x262a;&#xfe0e;",
       0u,
       2u,
       2u,
       glyphsVS15,
       characterIndicesVS15,
       charactersPerGlyphVS15,
       0u,
       nullptr,
       true,
       fontDescriptionsColorVS15},
      {"EMOJI Sequence: Text Font with VS16",
       "&#x262a;&#xfe0f;",
       0u,
       2u,
       1u,
       glyphsVS16,
       characterIndicesVS16,
       charactersPerGlyphVS16,
       0u,
       nullptr,
       true,
       fontDescriptionsTextVS16},
      {"EMOJI Sequence: Text Font with VS15",
       "&#x262a;&#xfe0e;",
       0u,
       2u,
       2u,
       glyphsVS15,
       characterIndicesVS15,
       charactersPerGlyphVS15,
       0u,
       nullptr,
       true,
       fontDescriptionsTextVS15},
    };

  const unsigned int numberOfTests = 4u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    tet_infoline(data[index].description.c_str());
    if(!ShapeInfoTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
