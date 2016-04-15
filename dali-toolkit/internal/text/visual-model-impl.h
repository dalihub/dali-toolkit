#ifndef __DALI_TOOLKIT_TEXT_VISUAL_MODEL_IMPL_H__
#define __DALI_TOOLKIT_TEXT_VISUAL_MODEL_IMPL_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/color-run.h>

namespace Dali
{

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
   * @brief Creates the character to glyph conversion table.
   *
   * @pre The glyphs per character table needs to be created first.
   *
   * @param[in] startIndex The character from where the conversion table is created.
   * @param[in] startGlyphIndex The glyph from where the conversion table is created.
   * @param[in] numberOfCharacters The number of characters.
   */
  void CreateCharacterToGlyphTable( CharacterIndex startIndex,
                                    GlyphIndex startGlyphIndex,
                                    Length numberOfCharacters );

  /**
   * @brief Creates an array containing the number of glyphs per character.
   *
   * @param[in] startIndex The character from where the table is created.
   * @param[in] startGlyphIndex The glyph from where the conversion table is created.
   * @param[in] numberOfCharacters The number of characters.
   */
  void CreateGlyphsPerCharacterTable( CharacterIndex startIndex,
                                      GlyphIndex startGlyphIndex,
                                      Length numberOfCharacters );

  /**
   * @brief Retrieves glyphs in the given buffer.
   *
   * The size of the @p glyphs buffer needs to be big enough to copy the @p numberOfGlyphs.
   * @param[out] glyphs Pointer to a buffer where the glyphs are copied.
   * @param[in] glyphIndex Index to the first glyph.
   * @param[in] numberOfGlyphs Number of glyphs to be copied.
   */
  void GetGlyphs( GlyphInfo* glyphs,
                  GlyphIndex glyphIndex,
                  Length numberOfGlyphs ) const;

  // Position interface

  /**
   * @brief Retrieves the glyph positions.
   *
   * @pre The size of the @p positions buffer needs to be big enough to copy the @p numberOfGlyphs positions.
   * @param[out] glyphPositions Pointer to a buffer where the glyph positions are copied.
   * @param[in] glyphIndex Index to the first glyph position.
   * @param[in] numberOfGlyphs The number of positions to be copied.
   */
  void GetGlyphPositions( Vector2* glyphPositions,
                          GlyphIndex glyphIndex,
                          Length numberOfGlyphs ) const;

  // Line interface.

  /**
   * @brief Retrieves the number of lines and the index to the first line where the given range of glyphs is laid out.
   *
   * @param[in] glyphIndex Index to the first glyph.
   * @param[in] numberOfGlyphs The number of glyph.
   * @param[out] firstLine Index to the line containing the glyph index.
   * @param[out] numberOfLines The number of lines.
   */
  void GetNumberOfLines( GlyphIndex glyphIndex,
                         Length numberOfGlyphs,
                         LineIndex& firstLine,
                         Length& numberOfLines ) const;

  /**
   * @brief Retrieves the lines where the given range of glyphs is laid out.
   *
   * The size of the @p lines buffer needs to be big enough to copy the @p numberOfLines.
   *
   * @param[out] lines Pointer to a buffer where the lines are copied.
   * @param[in] glyphIndex Index to the first glyphs of the range.
   * @param[in] numberOfGlyphs Number of glyphs in the range.
   */
  void GetLinesOfGlyphRange( LineRun* lines,
                             GlyphIndex glyphIndex,
                             Length numberOfGlyphs ) const;

  /**
   * @brief Retrieves the line index where the character is laid-out.
   *
   * @param[in] characterIndex The character's index.
   *
   * @return The line index.
   */
  LineIndex GetLineOfCharacter( CharacterIndex characterIndex );

  // Underline runs

  /**
   * @brief Retrieves the underline runs.
   *
   * @param[out] underlineRuns Pointer to a buffer where the underline runs are copied.
   * @param[in] index Index of the first underline run to be copied.
   * @param[in] numberOfRuns Number of underline runs to be copied.
   */
  void GetUnderlineRuns( GlyphRun* underlineRuns,
                         UnderlineRunIndex index,
                         Length numberOfRuns ) const;

  // Size interface

  /**
   * @brief Sets the natural size.
   *
   * @param[in] size The text's natural size.
   */
  void SetNaturalSize( const Vector2& size  );

  /**
   * @brief Retrieves the natural size.
   *
   * @return The text's natural size.
   */
  const Vector2& GetNaturalSize() const;

  /**
   * @brief Sets the text's layout size.
   *
   * @param[in] size The text's size.
   */
  void SetLayoutSize( const Vector2& size );

  /**
   * @brief Retrieves the text's layout size.
   *
   * @return The text's size.
   */
  const Vector2& GetLayoutSize() const;

  /**
   * @brief Set the text's color
   *
   * @param[in] textColor The text's color
   */
  void SetTextColor( const Vector4& textColor );

  /**
   * @brief Retrieve the text's color
   *
   * @return The text's color
   */
  const Vector4& GetTextColor() const;

  /**
   * @brief Sets the text's shadow offset.
   *
   * @param[in] shadowOffset The shadow offset, 0,0 indicates no shadow.
   */
  void SetShadowOffset( const Vector2& shadowOffset );

  /**
   * @brief Retrieves the text's shadow offset.
   *
   * @return The text's shadow offset, 0,0 indicates no shadow.
   */
  const Vector2& GetShadowOffset() const;

  /**
   * @brief Sets the text's shadow color.
   *
   * @param[in] shadowColor The shadow color.
   */
  void SetShadowColor( const Vector4& shadowColor );

  /**
   * @brief Retrieves the text's shadow color.
   *
   * @return The text's shadow color.
   */
  const Vector4& GetShadowColor() const;

  /**
   * @brief Sets the text's underline color.
   *
   * @param[in] color The text's underline color.
   */
  void SetUnderlineColor( const Vector4& color );

  /**
   * @brief Retrieves the text's underline color.
   *
   * @return The text's underline color.
   */
  const Vector4& GetUnderlineColor() const;

  /**
   * @brief Sets the text underline flag.
   *
   * @param[in] enabled true if underlined.
   */
  void SetUnderlineEnabled( bool enabled );

  /**
   * @brief Returns whether the text is underlined or not.
   *
   * @return underline state.
   */
  bool IsUnderlineEnabled() const;

  /**
   * @brief Clear the caches.
   */
  void ClearCaches();

  /**
   * @brief Set the override used for underline height, 0 indicates height will be come from font metrics
   *
   * @param[in] height The height in pixels of the underline
   */
  void SetUnderlineHeight( float height );

  /**
   * @brief Retrieves the underline height override
   *
   * @return Returns the override height for an underline, 0 indicates that font metrics will determine the height
   */
  float GetUnderlineHeight() const;

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

public:

  Vector<GlyphInfo>      mGlyphs;               ///< For each glyph, the font's id, glyph's index within the font and glyph's metrics.
  Vector<CharacterIndex> mGlyphsToCharacters;   ///< For each glyph, the index of the first character.
  Vector<GlyphIndex>     mCharactersToGlyph;    ///< For each character, the index of the first glyph.
  Vector<Length>         mCharactersPerGlyph;   ///< For each glyph, the number of characters that form the glyph.
  Vector<Length>         mGlyphsPerCharacter;   ///< For each character, the number of glyphs that are shaped.
  Vector<Vector2>        mGlyphPositions;       ///< For each glyph, the position.
  Vector<LineRun>        mLines;                ///< The laid out lines.
  Vector<GlyphRun>       mUnderlineRuns;        ///< Runs of glyphs that are underlined.
  Vector<ColorGlyphRun>  mColorRuns;            ///< Runs of glyphs with the same color.

  Vector2                mControlSize;           ///< The size of the UI control the decorator is adding it's decorations to.
  Vector4                mTextColor;            ///< The text color
  Vector4                mShadowColor;          ///< Color of drop shadow
  Vector4                mUnderlineColor;       ///< Color of underline
  Vector2                mShadowOffset;         ///< Offset for drop shadow, 0 indicates no shadow
  float                  mUnderlineHeight;      ///< Fixed height for underline to override font metrics.

private:

  Size                   mNaturalSize;        ///< Size of the text with no line wrapping.
  Size                   mLayoutSize;         ///< Size of the laid-out text considering the layout properties set.

  // Caches to increase performance in some consecutive operations.
  LineIndex mCachedLineIndex; ///< Used to increase performance in consecutive calls to GetLineOfGlyph() or GetLineOfCharacter() with consecutive glyphs or characters.

public:

  bool                   mUnderlineEnabled:1;   ///< Underline enabled flag
  bool                   mUnderlineColorSet:1;  ///< Has the underline color been explicitly set?

};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_VISUAL_MODEL_IMPL_H__
