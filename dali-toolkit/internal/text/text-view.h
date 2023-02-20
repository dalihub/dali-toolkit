#ifndef DALI_TOOLKIT_TEXT_VIEW_H
#define DALI_TOOLKIT_TEXT_VIEW_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/text-view-interface.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief View provides an interface between the Text layout engine and rendering back-end.
 */
class View : public ViewInterface
{
public:
  /**
   * @brief Create a new instance of a View.
   */
  View();

  /**
   * @brief Virtual destructor.
   */
  virtual ~View();

  /**
   * @brief Set the visual model.
   *
   * @param[in] visualModel The visual model used by the View.
   */
  void SetVisualModel(VisualModelPtr visualModel);

  /**
   * @brief Set the logical model.
   *
   * @param[in] logicalModel The logical model used by the View.
   */
  void SetLogicalModel(LogicalModelPtr logicalModel);

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetControlSize()
   */
  const Vector2& GetControlSize() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetLayoutSize()
   */
  const Vector2& GetLayoutSize() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetNumberOfGlyphs()
   */
  Length GetNumberOfGlyphs() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetGlyphs()
   */
  virtual Length GetGlyphs(GlyphInfo* glyphs,
                           Vector2*   glyphPositions,
                           float&     minLineOffset,
                           GlyphIndex glyphIndex,
                           Length     numberOfGlyphs) const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetColors()
   */
  const Vector4* GetColors() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetColorIndices()
   */
  const ColorIndex* GetColorIndices() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetBackgroundColors()
   */
  const Vector4* GetBackgroundColors() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetBackgroundColorIndices()
   */
  const ColorIndex* GetBackgroundColorIndices() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::IsMarkupBackgroundColorSet()
   */
  bool IsMarkupBackgroundColorSet() const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetTextColor()
   */
  const Vector4& GetTextColor() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetShadowOffset()
   */
  const Vector2& GetShadowOffset() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetShadowColor()
   */
  const Vector4& GetShadowColor() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetUnderlineColor()
   */
  const Vector4& GetUnderlineColor() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::IsUnderlineEnabled()
   */
  bool IsUnderlineEnabled() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::IsMarkupUnderlineSet()
   */
  bool IsMarkupUnderlineSet() const;

  /**
  * @copydoc Dali::Toolkit::Text::ViewInterface::GetHyphens()
  */
  const GlyphInfo* GetHyphens() const override;

  /**
  * @copydoc Dali::Toolkit::Text::ViewInterface::GetHyphens()
  */
  const Length* GetHyphenIndices() const override;

  /**
  * @copydoc Dali::Toolkit::Text::ViewInterface::GetHyphens()
  */
  Length GetHyphensCount() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetUnderlineHeight()
   */
  float GetUnderlineHeight() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetUnderlineType()
   */
  Text::Underline::Type GetUnderlineType() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetDashedUnderlineWidth()
   */
  float GetDashedUnderlineWidth() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetDashedUnderlineGap()
   */
  float GetDashedUnderlineGap() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetNumberOfUnderlineRuns()
   */
  Length GetNumberOfUnderlineRuns() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetUnderlineRuns()
   */
  virtual void GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns,
                                UnderlineRunIndex   index,
                                Length              numberOfRuns) const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetOutlineColor()
   */
  const Vector4& GetOutlineColor() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetOutlineWidth()
   */
  uint16_t GetOutlineWidth() const override;

  /**
  * @copydoc Dali::Toolkit::Text::ViewInterface::GetEllipsisPosition()
  */
  DevelText::EllipsisPosition::Type GetEllipsisPosition() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetStartIndexOfElidedGlyphs()
   */
  GlyphIndex GetStartIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetEndIndexOfElidedGlyphs()
   */
  GlyphIndex GetEndIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetFirstMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetSecondMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetStrikethroughColor()
   */
  const Vector4& GetStrikethroughColor() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::IsStrikethroughEnabled()
   */
  bool IsStrikethroughEnabled() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::IsMarkupStrikethroughSet()
   */
  bool IsMarkupStrikethroughSet() const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetStrikethroughHeight()
   */
  float GetStrikethroughHeight() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetNumberOfStrikethroughRuns()
   */
  Length GetNumberOfStrikethroughRuns() const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetStrikethroughRuns()
   */
  void GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns,
                            StrikethroughRunIndex  index,
                            Length                 numberOfRuns) const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetNumberOfBoundedParagraphRuns()
   */
  virtual Length GetNumberOfBoundedParagraphRuns() const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetBoundedParagraphRuns()
   */
  virtual const Vector<BoundedParagraphRun>& GetBoundedParagraphRuns() const;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetCharacterSpacing()
   */
  float GetCharacterSpacing() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetTextBuffer()
   */
  const Character* GetTextBuffer() const override;

  /**
   * @copydoc Dali::Toolkit::Text::ViewInterface::GetGlyphsToCharacters()
   */
  const Vector<CharacterIndex>& GetGlyphsToCharacters() const override;

private:
  // Undefined
  View(const View& handle);

  // Undefined
  View& operator=(const View& handle);

private:
  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VIEW_H
