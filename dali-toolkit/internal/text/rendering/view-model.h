#ifndef DALI_TOOLKIT_TEXT_VIEW_MODEL_H
#define DALI_TOOLKIT_TEXT_VIEW_MODEL_H

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
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/text-model-interface.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Responsible of creating and store temporary modifications of the text model.
 * i.e. The elide of text.
 */
class ViewModel : public ModelInterface
{
public:
  /**
   * @brief Constructor.
   *
   * Keeps the pointer to the text's model. It initializes all the members of the class to their defaults.
   *
   * @param[in] model Pointer to the text's model interface.
   */
  ViewModel(const ModelInterface* const model);

  /**
   * @brief Virtual destructor.
   *
   * It's a default destructor.
   */
  virtual ~ViewModel();

  /**
   * @copydoc ModelInterface::GetControlSize()
   */
  const Size& GetControlSize() const override;

  /**
   * @copydoc ModelInterface::GetLayoutSize()
   */
  const Size& GetLayoutSize() const override;

  /**
   * @copydoc ModelInterface::GetScrollPosition()
   */
  const Vector2& GetScrollPosition() const override;

  /**
   * @copydoc ModelInterface::GetHorizontalAlignment()
   */
  Text::HorizontalAlignment::Type GetHorizontalAlignment() const override;

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  Text::VerticalAlignment::Type GetVerticalAlignment() const override;

  /**
 * @copydoc ModelInterface::GetVerticalLineAlignment()
 */
  DevelText::VerticalLineAlignment::Type GetVerticalLineAlignment() const override;

  /**
   * @copydoc ModelInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfLines()
   */
  Length GetNumberOfLines() const override;

  /**
   * @copydoc ModelInterface::GetLines()
   */
  const LineRun* const GetLines() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfScripts()
   */
  Length GetNumberOfScripts() const override;

  /**
   * @copydoc ModelInterface::GetScriptRuns()
   */
  const ScriptRun* const GetScriptRuns() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfGlyphs()
   */
  Length GetNumberOfGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetGlyphs()
   */
  const GlyphInfo* const GetGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetLayout()
   */
  const Vector2* const GetLayout() const override;

  /**
   * @copydoc ModelInterface::GetColors()
   */
  const Vector4* const GetColors() const override;

  /**
   * @copydoc ModelInterface::GetColorIndices()
   */
  const ColorIndex* const GetColorIndices() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColors()
   */
  const Vector4* const GetBackgroundColors() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColorIndices()
   */
  const ColorIndex* const GetBackgroundColorIndices() const override;

  /**
   * @copydoc ModelInterface::GetDefaultColor()
   */
  const Vector4& GetDefaultColor() const override;

  /**
   * @copydoc ModelInterface::GetShadowOffset()
   */
  const Vector2& GetShadowOffset() const override;

  /**
   * @copydoc ModelInterface::GetShadowColor()
   */
  const Vector4& GetShadowColor() const override;

  /**
   * @copydoc ModelInterface::GetShadowBlurRadius()
   */
  const float& GetShadowBlurRadius() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineColor()
   */
  const Vector4& GetUnderlineColor() const override;

  /**
   * @copydoc ModelInterface::IsUnderlineEnabled()
   */
  bool IsUnderlineEnabled() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineHeight()
   */
  float GetUnderlineHeight() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfUnderlineRuns()
   */
  Length GetNumberOfUnderlineRuns() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineRuns()
   */
  void GetUnderlineRuns(GlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const override;

  /**
   * @copydoc ModelInterface::GetOutlineColor()
   */
  const Vector4& GetOutlineColor() const override;

  /**
   * @copydoc ModelInterface::GetOutlineWidth()
   */
  uint16_t GetOutlineWidth() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColor()
   */
  const Vector4& GetBackgroundColor() const override;

  /**
   * @copydoc ModelInterface::IsBackgroundEnabled()
   */
  bool IsBackgroundEnabled() const override;

  /**
   * @brief Does the text elide.
   *
   * It stores a copy of the visible glyphs and removes as many glyphs as needed
   * from the last visible line to add the ellipsis glyph.
   *
   * It stores as well a copy of the positions for each visible glyph.
   */
  void ElideGlyphs();

private:
  const ModelInterface* const mModel;            ///< Pointer to the text's model.
  Vector<GlyphInfo>           mElidedGlyphs;     ///< Stores the glyphs of the elided text.
  Vector<Vector2>             mElidedLayout;     ///< Stores the positions of each glyph of the elided text.
  bool                        mIsTextElided : 1; ///< Whether the text has been elided.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VIEW_MODEL_H
