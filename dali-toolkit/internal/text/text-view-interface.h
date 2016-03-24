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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

struct Vector2;
struct Vector4;

namespace Toolkit
{

namespace Text
{

struct GlyphRun;

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
   * @brief Retrieves the target size of the UI control.
   *
   * @return The text's size.
   */
  virtual const Vector2& GetControlSize() const = 0;

  /**
   * @brief Retrieves the text's actual size after it has been laid out.
   *
   * @return The text's size. Note that this may be larger than the control size,
   * in the case where text is scrolling/clipped.
   */
  virtual const Vector2& GetActualSize() const = 0;

  /**
   * Retrieves the number of glyphs.
   *
   * @return The number of glyphs.
   */
  virtual Length GetNumberOfGlyphs() const = 0;

  /**
   * @brief Retrieves glyphs and positions in the given buffers.
   *
   * @note The size of the @p glyphs and @p glyphPositions buffers need to be big enough to copy the @p numberOfGlyphs glyphs and positions.
   * @note The returned number of glyphs may be less than @p numberOfGlyphs if a line has ellipsis.
   *
   * @param[out] glyphs Pointer to a buffer where the glyphs are copied.
   * @param[out] glyphPositions Pointer to a buffer where the glyph's positions are copied.
   * @param[out] colors Pointer to a buffer where the glyph's colors are copied.
   * @param[in] glyphIndex Index to the first glyph.
   * @param[in] numberOfGlyphs Number of glyphs to be copied.
   *
   * @return The number of glyphs.
   */
  virtual Length GetGlyphs( GlyphInfo* glyphs,
                            Vector2* glyphPositions,
                            Vector4* glyphColors,
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
   * @brief Returns whether underline is enabled or not.
   *
   * @return The underline state.
   */
  virtual bool IsUnderlineEnabled() const = 0;

  /**
   * @brief Retrieves the underline height override
   *
   * @return Returns the override height for an underline, 0 indicates that adaptor will determine the height
   */
  virtual float GetUnderlineHeight() const = 0;

  /**
   * @brief Retrieves the number of underline runs.
   *
   * @return The number of underline runs.
   */
  virtual Length GetNumberOfUnderlineRuns() const = 0;

  /**
   * @brief Retrieves the underline runs.
   *
   * @param[out] underlineRuns Pointer to a buffer where the underline runs are copied.
   * @param[in] index Index of the first underline run to be copied.
   * @param[in] numberOfRuns Number of underline runs to be copied.
   */
  virtual void GetUnderlineRuns( GlyphRun* underlineRuns,
                                 UnderlineRunIndex index,
                                 Length numberOfRuns ) const = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H__
