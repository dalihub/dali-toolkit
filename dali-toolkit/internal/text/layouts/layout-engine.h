#ifndef __DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H__
#define __DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H__

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

// EXTERNAL INCLUDE
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/metrics.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct LayoutParameters;

/**
 * @brief LayoutEngine is responsible for calculating the visual position of glyphs in layout.
 */
class LayoutEngine
{
public:

  enum Layout
  {
    SINGLE_LINE_BOX,
    MULTI_LINE_BOX
  };

  enum HorizontalAlignment
  {
    HORIZONTAL_ALIGN_BEGIN,
    HORIZONTAL_ALIGN_CENTER,
    HORIZONTAL_ALIGN_END
  };

  enum VerticalAlignment
  {
    VERTICAL_ALIGN_TOP,
    VERTICAL_ALIGN_CENTER,
    VERTICAL_ALIGN_BOTTOM
  };

  /**
   * @brief Create a new instance of a LayoutEngine.
   */
  LayoutEngine();

  /**
   * @brief Virtual destructor.
   */
  ~LayoutEngine();

  /**
   * @brief Provide the wrapper around FontClient used to get metrics
   *
   * @param[in] metrics Used to get metrics
   */
  void SetMetrics( MetricsPtr& metrics );

  /**
   * @brief Choose the required layout.
   *
   * @param[in] layout The required layout.
   */
  void SetLayout( Layout layout );

  /**
   * @brief Query the required layout.
   *
   * @return The required layout.
   */
  Layout GetLayout() const;

  /**
   * @brief Enable or disable the text ellipsis.
   *
   * @param[in] enabled Whether to enable the text ellipsis.
   */
  void SetTextEllipsisEnabled( bool enabled );

  /**
   * @return Whether the text ellipsis is enabled.
   */
  bool GetTextEllipsisEnabled() const;

  /**
   * @brief Choose the required text horizontal alignment.
   *
   * @param[in] alignment The required alignment.
   */
  void SetHorizontalAlignment( HorizontalAlignment alignment );

  /**
   * @brief Query the required text horizontal alignment.
   *
   * @return The required alignment.
   */
  HorizontalAlignment GetHorizontalAlignment() const;

  /**
   * @brief Choose the required text vertical alignment.
   *
   * @param[in] alignment The required alignment.
   */
  void SetVerticalAlignment( VerticalAlignment alignment );

  /**
   * @brief Query the required text vertical alignment.
   *
   * @return The required alignment.
   */
  VerticalAlignment GetVerticalAlignment() const;

  /**
   * @brief Sets the width of the cursor.
   *
   * @param[in] width The width of the cursor in pixels.
   */
  void SetCursorWidth( int width );

  /**
   * @brief Retrieves the width of the cursor.
   *
   * @return The width of the cursor in pixels.
   */
  int GetCursorWidth() const;

  /**
   * @brief Store the visual position of glyphs in the VisualModel.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[out] glyphPositions The positions of all the glyphs.
   * @param[out] lines The laid-out lines.
   * @param[out] layoutSize The size of the text after it has been laid-out.
   *
   * @return \e true if the text has been re-laid-out. \e false means the given width is too small to layout even a single character.
   */
  bool LayoutText( const LayoutParameters& layoutParameters,
                   Vector<Vector2>& glyphPositions,
                   Vector<LineRun>& lines,
                   Size& layoutSize );

  /**
   * @brief Re-lays out those lines with right to left characters.
   *
   * It doesn't change the phisical position of the glyphs in the model but sets their new position.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] startIndex Character index of the line from where the lines are reordered.
   * @param[in] numberOfCharacters The number of characters.
   * @param[in,out] glyphPositions The positions of all the glyphs.
   */
  void ReLayoutRightToLeftLines( const LayoutParameters& layoutParameters,
                                 CharacterIndex startIndex,
                                 Length numberOfCharacters,
                                 Vector<Vector2>& glyphPositions );

  /**
   * @brief Aligns the laid out lines.
   *
   * @param[in] size The size of the container where the text is laid-out.
   * @param[in] startIndex Character index of the line from where the lines are aligned.
   * @param[in] numberOfCharacters The number of characters.
   * @param[in,out] lines The laid-out lines.
   * @param[out] alignmentOffset The alignment offset.
   */
  void Align( const Size& size,
              CharacterIndex startIndex,
              Length numberOfCharacters,
              Vector<LineRun>& lines,
              float& alignmentOffset );

  /**
   * @brief Sets the default line spacing.
   *
   * @param[in] lineSpacing The line spacing.
   */
  void SetDefaultLineSpacing( float lineSpacing );

  /**
   * @brief Retrieves the default line spacing.
   *
   * @return The line spacing.
   */
  float GetDefaultLineSpacing() const;

private:

  // Undefined
  LayoutEngine( const LayoutEngine& handle );

  // Undefined
  LayoutEngine& operator=( const LayoutEngine& handle );

private:

  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H__
