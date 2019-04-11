/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/text-definitions.h>


namespace Dali
{

namespace Toolkit
{
using namespace Text;
namespace DevelText
{

Glyph::Glyph()
: url{},
  utf8{ 0u },
  ascender{ 0.f },
  descender{ 0.f }
{}

Glyph::~Glyph()
{}

BitmapFontDescription::BitmapFontDescription()
: glyphs{},
  name{},
  underlinePosition{ 0.f },
  underlineThickness{ 1.f }
{}

BitmapFontDescription::~BitmapFontDescription()
{}

void CreateBitmapFont( const BitmapFontDescription& description, TextAbstraction::BitmapFont& bitmapFont )
{
  bitmapFont.glyphs.reserve( description.glyphs.size() );
  bitmapFont.name = description.name;
  bitmapFont.underlinePosition = description.underlinePosition;
  bitmapFont.underlineThickness = description.underlineThickness;

  for( const auto& glyph : description.glyphs )
  {
    // 1) Convert to utf32
    Vector<Character> utf32;
    utf32.Resize( glyph.utf8.size() );

    const uint32_t numberOfCharacters = ( glyph.utf8.size() == 0 ) ? 0 :
        Text::Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( glyph.utf8.c_str() ),
                           glyph.utf8.size(),
                           &utf32[0u] );
    utf32.Resize( numberOfCharacters );

    TextAbstraction::BitmapGlyph bitmapGlyph( glyph.url, utf32[0u], glyph.ascender, glyph.descender );

    bitmapFont.glyphs.push_back( std::move( bitmapGlyph ) );
  }
}

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali
