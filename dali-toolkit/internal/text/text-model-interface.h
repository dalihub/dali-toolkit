#ifndef DALI_TOOLKIT_TEXT_MODEL_INTERFACE_H
#define DALI_TOOLKIT_TEXT_MODEL_INTERFACE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Interface class used to retrieve the text's model from the text-controller.
 */
class ModelInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~ModelInterface()
  {
  }

  /**
   * @brief Retrives the control's size.
   *
   * @return The control's size.
   */
  virtual const Size& GetControlSize() const = 0;

  /**
   * @brief Retrives the layout's size.
   *
   * @return The layout's size.
   */
  virtual const Size& GetLayoutSize() const = 0;

  /**
   * @brief Retrieves the text's scroll position.
   *
   * @return The scroll position.
   */
  virtual const Vector2& GetScrollPosition() const = 0;

  /**
   * @brief Retrieves the text's horizontal alignment.
   *
   * @return The horizontal alignment.
   */
  virtual HorizontalAlignment::Type GetHorizontalAlignment() const = 0;

  /**
   * @brief Retrieves the text's vertical alignment.
   *
   * @return The vertical alignment.
   */
  virtual VerticalAlignment::Type GetVerticalAlignment() const = 0;

  /**
   * @brief Retrieves the text's vertical line alignment.
   *
   * @return The vertical line alignment.
   */
  virtual DevelText::VerticalLineAlignment::Type GetVerticalLineAlignment() const = 0;

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
   * @brief Retrieves the number of laid-out lines.
   *
   * @return The number of laid-out lines.
   */
  virtual Length GetNumberOfLines() const = 0;

  /**
   * @brief Retrieves the laid-out lines.
   *
   * @return A pointer to the vector with the laid-out lines.
   */
  virtual const LineRun* const GetLines() const = 0;

  /**
   * @brief Retrieves the number of script runs.
   *
   * @return The number of script runs.
   */
  virtual Length GetNumberOfScripts() const = 0;

  /**
   * @brief Retrieves the script runs.
   *
   * @return A pointer to the vector with the runs of characters with the same script..
   */
  virtual const ScriptRun* const GetScriptRuns() const = 0;

  /**
   * @brief Retrieves the number of laid-out glyphs.
   *
   * @return The number of laid-out glyphs.
   */
  virtual Length GetNumberOfGlyphs() const = 0;

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
   * @brief Retrieves the laid-out glyphs.
   *
   * @return A pointer to the vector with the laid-out glyphs.
   */
  virtual const GlyphInfo* const GetGlyphs() const = 0;

  /**
   * @brief Retrieves the text layout.
   *
   * @return A pointer to the vector with the positions for each glyph.
   */
  virtual const Vector2* const GetLayout() const = 0;

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
   * @brief Retrieves the text's default color.
   *
   * @return The default color.
   */
  virtual const Vector4& GetDefaultColor() const = 0;

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
   * @brief Retrieve the shadow blur radius.
   *
   * @return The shadow blur radius.
   */
  virtual const float& GetShadowBlurRadius() const = 0;

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
  virtual void GetUnderlineRuns(GlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const = 0;

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
   * @brief Retrieves the background color.
   *
   * @return The background color.
   */
  virtual const Vector4& GetBackgroundColor() const = 0;

  /**
   * @brief Returns whether background is enabled or not.
   *
   * @return The background state.
   */
  virtual bool IsBackgroundEnabled() const = 0;

  /**
   * @brief Returns whether markup-processor is enabled or not.
   *
   * @return The markup-processor state.
   */
  virtual bool IsMarkupProcessorEnabled() const = 0;

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
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MODEL_INTERFACE_H
