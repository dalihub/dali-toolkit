#ifndef DALI_TOOLKIT_TEXT_VIEW_MODEL_H
#define DALI_TOOLKIT_TEXT_VIEW_MODEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/internal/text/text-model-interface.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>

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
  ViewModel( const ModelInterface* const model );

  /**
   * @brief Virtual destructor.
   *
   * It's a default destructor.
   */
  virtual ~ViewModel();

  /**
   * @copydoc ModelInterface::GetControlSize()
   */
  virtual const Size& GetControlSize() const;

  /**
   * @copydoc ModelInterface::GetLayoutSize()
   */
  virtual const Size& GetLayoutSize() const;

  /**
   * @copydoc ModelInterface::GetScrollPosition()
   */
  virtual const Vector2& GetScrollPosition() const;

  /**
   * @copydoc ModelInterface::GetHorizontalAlignment()
   */
  virtual Text::HorizontalAlignment::Type GetHorizontalAlignment() const;

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  virtual Text::VerticalAlignment::Type GetVerticalAlignment() const;

  /**
 * @copydoc ModelInterface::GetVerticalLineAlignment()
 */
  virtual DevelText::VerticalLineAlignment::Type GetVerticalLineAlignment() const;

  /**
   * @copydoc ModelInterface::IsTextElideEnabled()
   */
  virtual bool IsTextElideEnabled() const;

  /**
   * @copydoc ModelInterface::GetNumberOfLines()
   */
  virtual Length GetNumberOfLines() const;

  /**
   * @copydoc ModelInterface::GetLines()
   */
  virtual const LineRun* const GetLines() const;

  /**
   * @copydoc ModelInterface::GetNumberOfScripts()
   */
  virtual Length GetNumberOfScripts() const;

  /**
   * @copydoc ModelInterface::GetScriptRuns()
   */
  virtual const ScriptRun* const GetScriptRuns() const;

  /**
   * @copydoc ModelInterface::GetNumberOfGlyphs()
   */
  virtual Length GetNumberOfGlyphs() const;

  /**
   * @copydoc ModelInterface::GetGlyphs()
   */
  virtual const GlyphInfo* const GetGlyphs() const;

  /**
   * @copydoc ModelInterface::GetLayout()
   */
  virtual const Vector2* const GetLayout() const;

  /**
   * @copydoc ModelInterface::GetColors()
   */
  virtual const Vector4* const GetColors() const;

  /**
   * @copydoc ModelInterface::GetColorIndices()
   */
  virtual const ColorIndex* const GetColorIndices() const;

  /**
   * @copydoc ModelInterface::GetDefaultColor()
   */
  virtual const Vector4& GetDefaultColor() const;

  /**
   * @copydoc ModelInterface::GetShadowOffset()
   */
  virtual const Vector2& GetShadowOffset() const;

  /**
   * @copydoc ModelInterface::GetShadowColor()
   */
  virtual const Vector4& GetShadowColor() const;

  /**
   * @copydoc ModelInterface::GetShadowBlurRadius()
   */
  virtual const float& GetShadowBlurRadius() const;

  /**
   * @copydoc ModelInterface::GetUnderlineColor()
   */
  virtual const Vector4& GetUnderlineColor() const;

  /**
   * @copydoc ModelInterface::IsUnderlineEnabled()
   */
  virtual bool IsUnderlineEnabled() const;

  /**
   * @copydoc ModelInterface::GetUnderlineHeight()
   */
  virtual float GetUnderlineHeight() const;

  /**
   * @copydoc ModelInterface::GetNumberOfUnderlineRuns()
   */
  virtual Length GetNumberOfUnderlineRuns() const;

  /**
   * @copydoc ModelInterface::GetUnderlineRuns()
   */
  virtual void GetUnderlineRuns( GlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns ) const;

  /**
   * @copydoc ModelInterface::GetOutlineColor()
   */
  virtual const Vector4& GetOutlineColor() const;

  /**
   * @copydoc ModelInterface::GetOutlineWidth()
   */
  virtual float GetOutlineWidth() const;

  /**
   * @copydoc ModelInterface::GetBackgroundColor()
   */
  virtual const Vector4& GetBackgroundColor() const;

  /**
   * @copydoc ModelInterface::IsBackgroundEnabled()
   */
  virtual bool IsBackgroundEnabled() const;

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
