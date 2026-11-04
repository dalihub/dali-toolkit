#ifndef DALI_TOOLKIT_TEXT_MODEL_FIXTURE_H
#define DALI_TOOLKIT_TEXT_MODEL_FIXTURE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <dali-toolkit/internal/text/text-model.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>
#include <dali/devel-api/text-abstraction/script.h>
#include <string>

namespace TextTest
{
using namespace Dali;
using namespace Dali::Toolkit::Text;

// Geometry is input to these tests, not the result of host font selection or
// shaping. In particular ink width, advance, and bearing intentionally differ.
inline ModelPtr MakeModel(const std::u32string& text = U"abcd", unsigned int columns = 4u, bool rtl = false)
{
  auto model = Model::New();
  auto& logical = *model->mLogicalModel;
  auto& visual = *model->mVisualModel;
  logical.mText.Resize(text.size());
  visual.mGlyphs.Resize(text.size());
  visual.mGlyphPositions.Resize(text.size());
  visual.mCharactersToGlyph.Resize(text.size());
  visual.mGlyphsToCharacters.Resize(text.size());
  visual.mCharactersPerGlyph.Resize(text.size(), 1u);
  visual.mGlyphsPerCharacter.Resize(text.size(), 1u);
  logical.mLineBreakInfo.Resize(text.size(), TextAbstraction::LINE_NO_BREAK);
  if(rtl) logical.mCharacterDirections.Resize(text.size(), true);
  if(!text.empty())
  {
    ScriptRun script;
    script.characterRun = {0u, static_cast<Length>(text.size())};
    script.script = rtl ? TextAbstraction::ARABIC : TextAbstraction::LATIN;
    logical.mScriptRuns.PushBack(script);
  }
  unsigned int start = 0u;
  float maxWidth = 0.f;
  for(unsigned int i = 0u; i < text.size(); ++i)
  {
    logical.mText[i] = text[i];
    auto& glyph = visual.mGlyphs[i];
    glyph.fontId = 0u;
    glyph.index = text[i];
    glyph.advance = text[i] == '\n' ? 0.f : 10.f;
    glyph.width = text[i] == '\n' ? 0.f : 7.f;
    glyph.height = 20.f;
    glyph.xBearing = 2.f;
    glyph.yBearing = 15.f;
    visual.mGlyphPositions[i] = Vector2((i - start) * 10.f + 2.f, -15.f);
    visual.mCharactersToGlyph[i] = i;
    visual.mGlyphsToCharacters[i] = i;
    if(text[i] == '\n' || i + 1u == text.size() || i - start + 1u == columns)
    {
      LineRun line{};
      line.glyphRun = {start, i - start + 1u};
      line.characterRun = {start, i - start + 1u};
      line.width = (i - start) * 10.f + glyph.advance;
      line.ascender = 15.f;
      line.descender = -5.f;
      line.direction = rtl;
      maxWidth = std::max(maxWidth, line.width);
      visual.mLines.PushBack(line);
      logical.mLineBreakInfo[i] = TextAbstraction::LINE_MUST_BREAK;
      start = i + 1u;
    }
  }
  visual.mControlSize = Vector2(100.f, 100.f);
  visual.SetLayoutSize(Vector2(maxWidth, visual.mLines.Count() * 20.f));
  model->mRemoveFrontInset = false;
  model->mRemoveBackInset = false;
  return model;
}
} // namespace TextTest
#endif
