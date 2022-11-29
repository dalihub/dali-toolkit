#ifndef DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H
#define DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H

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

// EXTERNAL INCLUDE
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDE
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/metrics.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

//DEVEL INCLUDE
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Layout
{
struct Parameters;

/**
 * @brief LayoutEngine is responsible for calculating the visual position of glyphs in layout.
 */
class Engine
{
public:
  enum Type
  {
    SINGLE_LINE_BOX,
    MULTI_LINE_BOX
  };

  /**
   * @brief Create a new instance of a LayoutEngine.
   */
  Engine();

  /**
   * @brief Virtual destructor.
   */
  ~Engine();

  /**
   * @brief Provide the wrapper around FontClient used to get metrics
   *
   * @param[in] metrics Used to get metrics
   */
  void SetMetrics(MetricsPtr& metrics);

  /**
   * @brief Choose the required layout.
   *
   * @param[in] layout The required layout.
   */
  void SetLayout(Type layout);

  /**
   * @brief Query the required layout.
   *
   * @return The required layout.
   */
  Type GetLayout() const;

  /**
   * @brief Sets the width of the cursor.
   *
   * @param[in] width The width of the cursor in pixels.
   */
  void SetCursorWidth(int width);

  /**
   * @brief Retrieves the width of the cursor.
   *
   * @return The width of the cursor in pixels.
   */
  int GetCursorWidth() const;

  /**
   * @brief Store the visual position of glyphs in the VisualModel.
   *
   * Builds the bidirectional info and reorders RTL lines.
   *
   * @param[in,out] layoutParameters The parameters needed to layout the text.
   * @param[out] layoutSize The size of the text after it has been laid-out.
   * @param[in] elideTextEnabled Whether the text elide is enabled.
   * @param[in,out] isAutoScrollEnabled If the isAutoScrollEnabled is true and the height of the text exceeds the boundaries of the control the text is elided and the isAutoScrollEnabled is set to false to disable the autoscroll
   * @param[in] isAutoScrollMaxTextureExceeded If isAutoScrollMaxTextureExceeded is true, enable ellipsis during auro scroll.
   * @param[in] isHiddenInputEnabled if isHiddenInputEnabled is true, hidden input feature is enabled.
   * @param[in] ellipsisPosition The location of the text ellipsis
   *
   * @return \e true if the text has been re-laid-out. \e false means the given width is too small to layout even a single character.
   */
  bool LayoutText(Parameters&                       layoutParameters,
                  Size&                             layoutSize,
                  bool                              elideTextEnabled,
                  bool&                             isAutoScrollEnabled,
                  bool                              isAutoScrollMaxTextureExceeded,
                  bool                              isHiddenInputEnabled,
                  DevelText::EllipsisPosition::Type ellipsisPosition);

  /**
   * @brief Aligns the laid out lines.
   *
   * @param[in] size The size of the container where the text is laid-out.
   * @param[in] startIndex Character index of the line from where the lines are aligned.
   * @param[in] numberOfCharacters The number of characters.
   * @param[in] horizontalAlignment The horizontal alignment.
   * @param[in,out] lines The laid-out lines.
   * @param[out] alignmentOffset The alignment offset.
   * @param[in] layoutDirection The direction of the layout.
   * @param[in] matchLayoutDirection Whether match align for layout direction or not.
   */
  void Align(const Size&                     size,
             CharacterIndex                  startIndex,
             Length                          numberOfCharacters,
             Text::HorizontalAlignment::Type horizontalAlignment,
             Vector<LineRun>&                lines,
             float&                          alignmentOffset,
             Dali::LayoutDirection::Type     layoutDirection,
             bool                            matchLayoutDirection);

  /**
   * @brief Sets the default line spacing.
   *
   * @param[in] lineSpacing The line spacing.
   */
  void SetDefaultLineSpacing(float lineSpacing);

  /**
   * @brief Retrieves the default line spacing.
   *
   * @return The line spacing.
   */
  float GetDefaultLineSpacing() const;

  /**
   * @brief Sets the default line size.
   *
   * @param[in] lineSize The line size.
   */
  void SetDefaultLineSize(float lineSize);

  /**
   * @brief Retrieves the default line size.
   *
   * @return The line size.
   */
  float GetDefaultLineSize() const;

private:
  // Undefined
  Engine(const Engine& handle);

  // Undefined
  Engine& operator=(const Engine& handle);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LAYOUT_ENGINE_H
