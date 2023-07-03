#ifndef DALI_TOOLKIT_FONT_BITMAP_DEVEL_H
#define DALI_TOOLKIT_FONT_BITMAP_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <cstdint>
#include <string>

namespace Dali
{
namespace TextAbstraction
{
// Forward declarations
struct BitmapFont;
} // namespace TextAbstraction

namespace Toolkit
{
namespace DevelText
{
/**
 * @brief Struct that stores the needed info to create a bitmap glyph.
 */

struct DALI_TOOLKIT_API Glyph
{
  /**
   * @brief Default constructor.
   *
   * Initialize the members to its defaults.
   */
  Glyph();

  /**
   * @brief Constructor.
   *
   * Initialize the members with the given values.
   *
   * @param[in] url The url of the bitmap for that glyph.
   * @param[in] utf8 The utf8 codification of the glyph.
   * @param[in] ascender The ascender of the glyph.
   * @param[in] descender The descender of the glyph.
   */
  Glyph(const std::string& url, const std::string utf8, float ascender, float descender);

  /**
   * @brief Default destructor.
   */
  ~Glyph();

  std::string url;       ///< The url of the glyph.
  uint8_t     utf8[4];   ///< the glyph encoded in utf8
  float       ascender;  ///< The ascender. The distance from the base line to the top of the glyph.
  float       descender; ///< The descender. The distance from the base line to the bottom of the glyph.
};

/**
 * @brief Describes a bitmap font.
 */
struct DALI_TOOLKIT_API BitmapFontDescription
{
  /**
   * @brief Default constructor.
   *
   * Initialize the members to its defaults.
   */
  BitmapFontDescription();

  /**
   * @brief Default destructor.
   */
  ~BitmapFontDescription();

  std::vector<Glyph> glyphs;             ///< Vector of glyphs.
  std::string        name;               ///< Name of the font.
  float              underlinePosition;  ///< The position of the underline from the base line.
  float              underlineThickness; ///< The thickness of the underline.
  bool               isColorFont : 1;    ///< Whether the glyphs of this font have their own colors.
};

/**
 * @brief Creates a bitmap font with the given description.
 *
 * The @p bitmapFont output can be passed to the FontClient::GetFontId()
 * method to load the font and get an Id.
 *
 * @param[in] description The description of the bitmap font.
 * @param[out] bitmapFont A bitmap font.
 *
 * @note If the @e ascender and @e descender of the glyphs are zero, the @e descender value will be left as zero and all the bitmaps will be loaded to find out the @e ascender value.
 */
DALI_TOOLKIT_API void CreateBitmapFont(const BitmapFontDescription& description, TextAbstraction::BitmapFont& bitmapFont);

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UTILS_DEVEL_H
