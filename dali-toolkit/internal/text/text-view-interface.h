#ifndef DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H
#define DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>
#include <dali-toolkit/internal/text/character-spacing-glyph-run.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/internal/text/underlined-glyph-run.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{
struct Vector2;
struct Vector4;

namespace Toolkit
{
namespace Text
{
struct GlyphRun;
struct StrikethroughGlyphRun;

/**
 * @brief Abstract interface to provide the information necessary to display text.
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
   * @return The control's size.
   */
  virtual const Vector2& GetControlSize() const = 0;

  /**
   * @brief Retrieves the text's layout size.
   *
   * @return The text's size. Note that this may be larger than the control size,
   * in the case where text is scrolling/clipped.
   */
  virtual const Vector2& GetLayoutSize() const = 0;

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
   * @param[out] minLineOffset The minimum line offset.
   * @param[in] glyphIndex Index to the first glyph.
   * @param[in] numberOfGlyphs Number of glyphs to be copied.
   *
   * @return The number of glyphs.
   */
  virtual Length GetGlyphs(GlyphInfo* glyphs,
                           Vector2*   glyphPositions,
                           float&     minLineOffset,
                           GlyphIndex glyphIndex,
                           Length     numberOfGlyphs) const = 0;

  /**
   * @brief Retrieves the vector of colors.
   *
   * @return Pointer to the vector of colors.
   */
  virtual const Vector4* const GetColors() const = 0;

  /**
   * @brief Retrieves the vector of indices to the vector of colors.
   *
   * @return Pointer to a vector which stores for each glyph the index to the vector of colors.
   */
  virtual const ColorIndex* const GetColorIndices() const = 0;

  /**
   * @brief Retrieves the vector of background colors.
   *
   * @return Pointer to the vector of background colors.
   */
  virtual const Vector4* const GetBackgroundColors() const = 0;

  /**
   * @brief Retrieves the vector of indices to the vector of background colors.
   *
   * @return Pointer to a vector which stores for each glyph the index to the vector of background colors.
   */
  virtual const ColorIndex* const GetBackgroundColorIndices() const = 0;

  /**
   * @brief checks if there is background colors set using markup.
   *
   * @return boolean if there is background colors set using markup .
   */
  virtual bool const IsMarkupBackgroundColorSet() const = 0;

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
   * @brief Returns the hyphens glyph info.
   *
   * @return hyphens glyph info.
   */
  virtual const GlyphInfo* GetHyphens() const = 0;

  /**
   * @brief Returns the indices of the hyphen in the text.
   *
   * @return the hyphen indices.
   */
  virtual const Length* GetHyphenIndices() const = 0;

  /**
   * @brief Returns number of hyphens to add in text.
   *
   * @return number of hyphens.
   */
  virtual Length GetHyphensCount() const = 0;
  /**
   * @brief Retrieves the underline height override
   *
   * @return Returns the override height for an underline, 0 indicates that adaptor will determine the height
   */
  virtual float GetUnderlineHeight() const = 0;

  /**
   * @brief Retrieves the underline type override
   *
   * @return Returns the override type for an underline.
   */
  virtual Text::Underline::Type GetUnderlineType() const = 0;

  /**
   * @brief Retrieves the dashed underline width override.
   *
   * @return Returns the override width for the dashed underline.
   */
  virtual float GetDashedUnderlineWidth() const = 0;

  /**
   * @brief Retrieves the dashed underline gap override.
   *
   * @return Returns the override gap for the dashed underline.
   */
  virtual float GetDashedUnderlineGap() const = 0;

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
  virtual void GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns,
                                UnderlineRunIndex   index,
                                Length              numberOfRuns) const = 0;

  /**
   * @brief Retrieve the outline color.
   *
   * @return The outline color.
   */
  virtual const Vector4& GetOutlineColor() const = 0;

  /**
   * @brief Retrieves the width of an outline
   *
   * @return The width of the outline.
   */
  virtual uint16_t GetOutlineWidth() const = 0;

  /**
   * @brief Retrieves ellipsis position for text.
   *
   * @return The ellipsis position.
   */
  virtual DevelText::EllipsisPosition::Type GetEllipsisPosition() const = 0;

  /**
   * @brief Whether the text elide property is enabled.
   *
   * @return @e true if the text elide property is enabled, @e false otherwise.
   */
  virtual bool IsTextElideEnabled() const = 0;

  /**
   * @brief Retrieves the start index of laid-out glyphs.
   *
   * @return The start index of laid-out glyphs.
   */
  virtual GlyphIndex GetStartIndexOfElidedGlyphs() const = 0;

  /**
   * @brief Retrieves the end index of laid-out glyphs.
   *
   * @return The end index of laid-out glyphs.
   */
  virtual GlyphIndex GetEndIndexOfElidedGlyphs() const = 0;

  /**
   * @brief Retrieves the first middle index of elided glyphs, index before ellipsis of middle.
   *
   * @return The first middle index of elided glyphs, index before ellipsis of middle.
   */
  virtual GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const = 0;

  /**
   * @brief Retrieves the second middle index of elided glyphs, index of ellipsis of middle.
   *
   * @return The second middle index of elided glyphs, index of ellipsis of middle.
   */
  virtual GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const = 0;

  /**
   * @brief Retrieves the strikethrough color.
   *
   * @return The strikethrough color.
   */
  virtual const Vector4& GetStrikethroughColor() const = 0;

  /**
   * @brief Returns whether strikethrough is enabled or not.
   *
   * @return The strikethrough state.
   */
  virtual bool IsStrikethroughEnabled() const = 0;

  /**
   * @brief Retrieves the strikethrough height override
   *
   * @return Returns the override height for a strikethrough, 0 indicates that adaptor will determine the height
   */
  virtual float GetStrikethroughHeight() const = 0;

  /**
   * @brief Retrieves the number of strikethrough runs.
   *
   * @return The number of strikethrough runs.
   */
  virtual Length GetNumberOfStrikethroughRuns() const = 0;

  /**
   * @brief Retrieves the number of bounded paragraph runs.
   *
   * @return The number of bounded paragraph runs.
   */
  virtual Length GetNumberOfBoundedParagraphRuns() const = 0;

  /**
   * @brief Retrieves the reference for bounded paragraph runs.
   *
   * @return The reference for bounded paragraph runs.
   */
  virtual const Vector<BoundedParagraphRun>& GetBoundedParagraphRuns() const = 0;

  /**
   * @brief Retrieves the number of character-spacing glyph runs.
   *
   * @return The number of character-spacing glyph runs.
   */
  virtual Length GetNumberOfCharacterSpacingGlyphRuns() const = 0;

  /**
   * @brief Retrieves the reference for character-spacing glyph runs.
   *
   * @return The reference for character-spacing glyph runs.
   */
  virtual const Vector<CharacterSpacingGlyphRun>& GetCharacterSpacingGlyphRuns() const = 0;

  /**
   * @brief Retrieves the strikethrough runs.
   *
   * @param[out] strikethroughRuns Pointer to a buffer where the strikethrough runs are copied.
   * @param[in] index Index of the first strikethrough run to be copied.
   * @param[in] numberOfRuns Number of strikethrough runs to be copied.
   */
  virtual void GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns,
                                    StrikethroughRunIndex  index,
                                    Length                 numberOfRuns) const = 0;

  /**
   * @brief The spaces between characters in Pixels.
   *
   * @note A positive value will make the characters far apart (expanded) and a negative value will bring them closer (condensed).
   *
   * @return characterSpacing.
   */
  virtual const float GetCharacterSpacing() const = 0;

  /**
   * @brief The text buffer.
   *
   * @return The text buffer.
   */
  virtual const Character* GetTextBuffer() const = 0;

  /**
   * @brief The text Glyph to Characters Array.
   *
   * @return GetGlyphsToCharacters.
   */
  virtual const Vector<CharacterIndex>& GetGlyphsToCharacters() const = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VIEW_INTERFACE_H
