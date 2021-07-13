#ifndef DALI_TOOLKIT_TEXT_MODEL_H
#define DALI_TOOLKIT_TEXT_MODEL_H

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
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/text-model-interface.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
// Forward declarations.
class Model;

typedef IntrusivePtr<Model> ModelPtr;

/**
 * @brief Implementation class used to retrieve the text's model from the text-controller.
 */
class Model : public RefObject, public ModelInterface
{
public: // Constructor.
  /**
   * @brief Create a new instance of a text Model.
   *
   * @return A pointer to a new text Model.
   */
  static ModelPtr New();

public:
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
  HorizontalAlignment::Type GetHorizontalAlignment() const override;

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  VerticalAlignment::Type GetVerticalAlignment() const override;

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
   * @copydoc ModelInterface::IsMarkupBackgroundColorSet()
   */
  bool const IsMarkupBackgroundColorSet() const override;

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
   * @copydoc ModelInterface::IsMarkupProcessorEnabled()
   */
  bool IsMarkupProcessorEnabled() const override;

  /**
  * @copydoc ModelInterface::GetHyphens()
  */
  const GlyphInfo* GetHyphens() const override;

  /**
  * @copydoc ModelInterface::GetHyphens()
  */
  const Length* GetHyphenIndices() const override;

  /**
  * @copydoc ModelInterface::GetHyphens()
  */
  Length GetHyphensCount() const override;

private: // Private contructors & copy operator.
  /**
   * @brief Private constructor.
   */
  Model();

  // Undefined.
  Model(const Model& handle);

  // Undefined.
  Model& operator=(const Model& handle);

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Model();

public:
  LogicalModelPtr mLogicalModel; ///< Pointer to the logical model.
  VisualModelPtr  mVisualModel;  ///< Pointer to the visual model.
  /**
   * 0,0 means that the top-left corner of the layout matches the top-left corner of the UI control.
   * Typically this will have a negative value with scrolling occurs.
   */
  Vector2                                mScrollPosition;            ///< The text is offset by this position when scrolling.
  Vector2                                mScrollPositionLast;        ///< The last offset value of mScrollPosition
  HorizontalAlignment::Type              mHorizontalAlignment;       ///< The layout's horizontal alignment.
  VerticalAlignment::Type                mVerticalAlignment;         ///< The layout's vertical alignment.
  DevelText::VerticalLineAlignment::Type mVerticalLineAlignment;     ///< The layout's vertical line alignment.
  Text::LineWrap::Mode                   mLineWrapMode;              ///< The text wrap mode
  float                                  mAlignmentOffset;           ///< The alignment offset.
  bool                                   mElideEnabled : 1;          ///< Whether the text's elide is enabled.
  bool                                   mIgnoreSpacesAfterText : 1; ///< Whether ignoring spaces after text or not. Default is true.
  DevelText::MatchLayoutDirection        mMatchLayoutDirection;      ///< Whether to match text alignment with layout direction or not.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MODEL_H
