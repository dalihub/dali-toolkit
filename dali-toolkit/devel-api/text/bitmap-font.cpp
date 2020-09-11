/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/text/bitmap-font.h>

// EXTERNAL INCLUDE
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <cstring>

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/character-set-conversion.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelText
{
Glyph::Glyph()
: url{},
  utf8{},
  ascender{0.f},
  descender{0.f}
{
}

Glyph::Glyph(const std::string& url, const std::string utf8Character, float ascender, float descender)
: url{url},
  utf8{},
  ascender{ascender},
  descender{descender}
{
  DALI_ASSERT_DEBUG(utf8Character.size() <= 4u);

  std::copy(utf8Character.begin(), utf8Character.end(), utf8);
}

Glyph::~Glyph()
{
}

BitmapFontDescription::BitmapFontDescription()
: glyphs{},
  name{},
  underlinePosition{0.f},
  underlineThickness{1.f},
  isColorFont{false}
{
}

BitmapFontDescription::~BitmapFontDescription()
{
}

void CreateBitmapFont(const BitmapFontDescription& description, TextAbstraction::BitmapFont& bitmapFont)
{
  bitmapFont.glyphs.reserve(description.glyphs.size());
  bitmapFont.name               = description.name;
  bitmapFont.underlinePosition  = description.underlinePosition;
  bitmapFont.underlineThickness = description.underlineThickness;
  bitmapFont.isColorFont        = description.isColorFont;

  for(const auto& glyph : description.glyphs)
  {
    uint32_t c = 0u;
    Text::Utf8ToUtf32(glyph.utf8, Text::GetUtf8Length(glyph.utf8[0u]), &c);

    TextAbstraction::BitmapGlyph bitmapGlyph(glyph.url, c, glyph.ascender, glyph.descender);

    bitmapFont.glyphs.push_back(std::move(bitmapGlyph));
  }
}

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali
