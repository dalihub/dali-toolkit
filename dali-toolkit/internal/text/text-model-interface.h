#ifndef DALI_TOOLKIT_TEXT_MODEL_INTERFACE_H
#define DALI_TOOLKIT_TEXT_MODEL_INTERFACE_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/layouts/layout-alignment.h>
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/text-definitions.h>

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
  {}

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
  virtual Layout::HorizontalAlignment GetHorizontalAlignment() const = 0;

  /**
   * @brief Retrieves the text's vertical alignment.
   *
   * @return The vertical alignment.
   */
  virtual Layout::VerticalAlignment GetVerticalAlignment() const = 0;

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
   * @brief Retrieves the number of laid-out glyphs.
   *
   * @return The number of laid-out glyphs.
   */
  virtual Length GetNumberOfGlyphs() const = 0;

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
   * @brief Retrieves the text's default color.
   *
   * @return The default color.
   */
  virtual const Vector4& GetDefaultColor() const = 0;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MODEL_INTERFACE_H
