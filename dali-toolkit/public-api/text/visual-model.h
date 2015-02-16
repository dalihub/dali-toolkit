#ifndef __DALI_TOOLKIT_TEXT_VISUAL_MODEL_H__
#define __DALI_TOOLKIT_TEXT_VISUAL_MODEL_H__

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>
#include <dali-toolkit/public-api/text/text-definitions.h>

namespace Dali
{

struct Vector2;

namespace Toolkit
{

namespace Text
{

class VisualModel;
typedef IntrusivePtr<VisualModel> VisualModelPtr;

/**
 * @brief A visual text model contains layout specific information.
 *
 * This includes:
 * - A series of glyphs in visual order i.e. after the bidirectional reordering.
 * - The position of each glyph within a 2D bounding box.
 */
class VisualModel : public RefObject
{
public:

  /**
   * @brief Create a new instance of a VisualModel.
   *
   * @return A pointer to a new VisualModel.
   */
  static VisualModelPtr New();

  // Glyph interface.

  /**
   * @brief Replaces any glyphs previously set.
   *
   * @param[in] glyphs An array of glyphs in the visual order.
   * @param[in] characterIndices An array containing the first character in the logical model that each glyph relates to.
   * @param[in] charactersPerGlyph An array containing the number of characters per glyph.
   * @param[in] numberOfGlyphs The number of glyphs.
   */
  void SetGlyphs( const GlyphInfo* glyphs,
                  const CharacterIndex* characterIndices,
                  const Length* charactersPerGlyph,
                  Length numberOfGlyphs );

  /**
   * Retrieves the number of glyphs.
   *
   * @return The number of glyphs.
   */
  Length GetNumberOfGlyphs() const;

  /**
   * @brief Retrieves glyphs in the given buffer.
   *
   * The size of the @p glyphs buffer needs to be big enough to copy the @p numberOfGlyphs.
   * @param[out] glyphs Pointer to a buffer where the glyphs are copied.
   * @param[in] glyphIndex Index to the first glyph.
   * @param[in] numberOfGlyphs Number of glyphs to be copied.
   */
  void GetGlyphs( GlyphIndex glyphIndex,
                  GlyphInfo* glyphs,
                  Length numberOfGlyphs ) const;

  // Character <--> Glyph conversion

  /**
   * @brief Retrieves the first character in the logical model which a glyph represents.
   *
   * @note After shaping several characters may be represented by the same glyph.
   * Alternatively several glyphs may be required to display a character.
   * @param[in] glyphIndex The glyph index.
   * @return The character index.
   */
  CharacterIndex GetCharacterIndex( GlyphIndex glyphIndex ) const;

  /**
   * @brief Query the number of characters the glyph represents.
   *
   * @param[in] glyphIndex The glyph index.
   * @return The number of characters represented by the glyph.
   */
  Length GetCharactersPerGlyph( GlyphIndex glyphIndex ) const;

  /**
   * Retrieves the first glyph in the visual model which represents a given character.
   *
   * @note After shaping several characters may be represented by the same glyph.
   * Alternatively several glyphs may be required to display a character.
   * @param[in] characterIndex The character index.
   * @return The glyph index.
   */
  GlyphIndex GetGlyphIndex( CharacterIndex characterIndex ) const;

  // Position interface

  /**
   * @brief Replaces any glyph positions previously set.
   *
   * @param[in] glyphPositions An array of visual positions for each glyph.
   * @param[in] numberOfGlyphs The number of positions.
   */
  void SetGlyphPositions( const Vector2* glyphPositions,
                          Length numberOfGlyphs );

  /**
   * @brief Retrieves the glyph positions.
   *
   * @pre The size of the @p positions buffer needs to be big enough to copy the @p numberOfGlyphs positions.
   * @param[in] glyphIndex Index to the first glyph position.
   * @param[out] glyphPositions Pointer to a buffer where the glyph positions are copied.
   * @param[in] numberOfGlyphs The number of positions to be copied.
   */
  void GetGlyphPositions( GlyphIndex glyphIndex,
                          Vector2* glyphPositions,
                          Length numberOfGlyphs ) const;

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~VisualModel();

private:

  /**
   * @brief Private constructor.
   */
  VisualModel();

  // Undefined
  VisualModel( const VisualModel& handle );

  // Undefined
  VisualModel& operator=( const VisualModel& handle );

private:

  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_VISUAL_MODEL_H__
