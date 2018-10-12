#ifndef DALI_TOOLKIT_TEXT_MODEL_H
#define DALI_TOOLKIT_TEXT_MODEL_H

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
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/text-model-interface.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

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
  virtual HorizontalAlignment::Type GetHorizontalAlignment() const;

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  virtual VerticalAlignment::Type GetVerticalAlignment() const;

  /**
   * @copydoc ModelInterface::GetVerticalLineAlignment()
   */
  virtual DevelText::VerticalLineAlignment::Type GetVerticalLineAlignment() const override;

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

private: // Private contructors & copy operator.

  /**
   * @brief Private constructor.
   */
  Model();

  // Undefined.
  Model( const Model& handle );

  // Undefined.
  Model& operator=( const Model& handle );

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Model();

public:
  LogicalModelPtr             mLogicalModel;        ///< Pointer to the logical model.
  VisualModelPtr              mVisualModel;         ///< Pointer to the visual model.
  /**
   * 0,0 means that the top-left corner of the layout matches the top-left corner of the UI control.
   * Typically this will have a negative value with scrolling occurs.
   */
  Vector2                                   mScrollPosition;          ///< The text is offset by this position when scrolling.
  Vector2                                   mScrollPositionLast;      ///< The last offset value of mScrollPosition
  HorizontalAlignment::Type                 mHorizontalAlignment;     ///< The layout's horizontal alignment.
  VerticalAlignment::Type                   mVerticalAlignment;       ///< The layout's vertical alignment.
  DevelText::VerticalLineAlignment::Type    mVerticalLineAlignment;   ///< The layout's vertical line alignment.
  Text::LineWrap::Mode                      mLineWrapMode;            ///< The text wrap mode
  float                                     mAlignmentOffset;         ///< The alignment offset.
  bool                                      mElideEnabled:1;          ///< Whether the text's elide is enabled.
  bool                                      mIgnoreSpacesAfterText:1; ///< Whether ignoring spaces after text or not. Default is true.
  bool                                      mMatchSystemLanguageDirection:1; ///< Whether match align for system language direction or not. Default is false.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MODEL_H
