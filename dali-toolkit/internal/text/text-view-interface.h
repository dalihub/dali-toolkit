#ifndef __DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H__
#define __DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

struct Vector2;

namespace Toolkit
{

namespace Text
{

/**
 * @brief Abstract interface to provide the information necessary displaying text.
 *
 * This includes:
 * - The font & glyph IDs needed to get bitmaps etc. from TextAbstraction
 * - The visual position of each glyph within the layout
 * - A window into the text layout e.g. which page of a document to view
 */
class ViewInterface
{
public:

  /**
   * @brief Constructor.
   */
  ViewInterface();

  /**
   * @brief Virtual destructor
   */
  virtual ~ViewInterface();

  /**
   * Retrieves the number of glyphs.
   *
   * @return The number of glyphs.
   */
  virtual Length GetNumberOfGlyphs() const = 0;

  /**
   * @brief Retrieves glyphs in the given buffer.
   *
   * The size of the @p glyphs buffer needs to be big enough to copy the @p numberOfGlyphs.
   * @param[out] glyphs Pointer to a buffer where the glyphs are copied.
   * @param[in] glyphIndex Index to the first glyph.
   * @param[in] numberOfGlyphs Number of glyphs to be copied.
   */
  virtual void GetGlyphs( GlyphInfo* glyphs,
                          GlyphIndex glyphIndex,
                          Length numberOfGlyphs ) const = 0;

  /**
   * @brief Retrieves the glyph positions.
   *
   * @pre The size of the @p positions buffer needs to be big enough to copy the @p numberOfGlyphs positions.
   * @param[out] glyphPositions Pointer to a buffer where the glyph positions are copied.
   * @param[in] glyphIndex Index to the first glyph position.
   * @param[in] numberOfGlyphs The number of positions to be copied.
   */
  virtual void GetGlyphPositions( Vector2* glyphPositions,
                                  GlyphIndex glyphIndex,
                                  Length numberOfGlyphs ) const = 0;

  /**
   * @brief Retrieves the text color
   *
   * @return The text color
   */
  virtual const Vector4& GetTextColor() const = 0;

  /**
   * @brief Retrieves the shadow offset, 0 indicates no shadow.
   *
   * @return The shadow offset.
   */
  virtual const Vector2& GetShadowOffset() const = 0;

  /**
   * @brief Retrieves the shadow color.
   *
   * @return The shadow color.
   */
  virtual const Vector4& GetShadowColor() const = 0;

  /**
   * @brief Retrieves the underline color.
   *
   * @return The underline color.
   */
  virtual const Vector4& GetUnderlineColor() const = 0;

  /**
   * @brief Returns whether is underline is enabled or not.
   *
   * @return The underline state.
   */
  virtual bool IsUnderlineEnabled() const = 0;

};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H__
