#ifndef DALI_TOOLKIT_TEXT_LABEL_DEVEL_H
#define DALI_TOOLKIT_TEXT_LABEL_DEVEL_H

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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelTextLabel
{
namespace Property
{
enum Type
{
  TEXT                   = Dali::Toolkit::TextLabel::Property::TEXT,
  FONT_FAMILY            = Dali::Toolkit::TextLabel::Property::FONT_FAMILY,
  FONT_STYLE             = Dali::Toolkit::TextLabel::Property::FONT_STYLE,
  POINT_SIZE             = Dali::Toolkit::TextLabel::Property::POINT_SIZE,
  MULTI_LINE             = Dali::Toolkit::TextLabel::Property::MULTI_LINE,
  HORIZONTAL_ALIGNMENT   = Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT,
  VERTICAL_ALIGNMENT     = Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT,
  ENABLE_MARKUP          = Dali::Toolkit::TextLabel::Property::ENABLE_MARKUP,
  ENABLE_AUTO_SCROLL     = Dali::Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL,
  AUTO_SCROLL_SPEED      = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_SPEED,
  AUTO_SCROLL_LOOP_COUNT = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT,
  AUTO_SCROLL_GAP        = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_GAP,
  LINE_SPACING           = Dali::Toolkit::TextLabel::Property::LINE_SPACING,
  UNDERLINE              = Dali::Toolkit::TextLabel::Property::UNDERLINE,
  SHADOW                 = Dali::Toolkit::TextLabel::Property::SHADOW,
  EMBOSS                 = Dali::Toolkit::TextLabel::Property::EMBOSS,
  OUTLINE                = Dali::Toolkit::TextLabel::Property::OUTLINE,
  PIXEL_SIZE             = Dali::Toolkit::TextLabel::Property::PIXEL_SIZE,
  ELLIPSIS               = Dali::Toolkit::TextLabel::Property::ELLIPSIS,
  AUTO_SCROLL_LOOP_DELAY = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_DELAY,
  AUTO_SCROLL_STOP_MODE  = Dali::Toolkit::TextLabel::Property::AUTO_SCROLL_STOP_MODE,
  LINE_COUNT             = Dali::Toolkit::TextLabel::Property::LINE_COUNT,
  LINE_WRAP_MODE         = Dali::Toolkit::TextLabel::Property::LINE_WRAP_MODE,

  /**
     * @brief The direction of the layout.
     * @details Name "textDirection", type [Type](@ref Dali::Toolkit::DevelText::TextDirection::Type) (Property::INTEGER), Read/Write
     * @note The text direction can be changed only by replacing the text itself.
     * @see TextDirection::Type for supported values.
     */
  TEXT_DIRECTION,

  /**
     * @brief Alignment of text within area of single line
     * @details Name "verticalLineAlignment", type [Type](@ref Dali::Toolkit::DevelText::VerticalLineAlignment::Type) (Property::INTEGER), Read/Write
     * @note The default value is TOP
     * @see VerticalLineAlignment::Type for supported values
     */
  VERTICAL_LINE_ALIGNMENT,

  /**
     * @brief The default text background parameters.
     * @details Name "textBackground", type Property::MAP.
     * @note Use "textBackground" as property name to avoid conflict with Control's "background" property
     *
     * The background map contains the following keys:
     *
     * | %Property Name       | Type     | Required | Description                                                                                                        |
     * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
     * | enable               | BOOLEAN  | No       | True to enable the background or false to disable (the default value is false)                                     |
     * | color                | VECTOR4  | No       | The color of the background (the default value is Color::CYAN)                                                     |
     */
  BACKGROUND,

  /**
     * @brief Ignore spaces after text.
     * @details Name "ignoreSpacesAfterText", type (Property::BOLEAN), Read/Write
     * @note The default value is true
     */
  IGNORE_SPACES_AFTER_TEXT,

  /**
     * @brief Modifies the default text alignment to match the direction of the system language.
     * @details Name "matchSystemLanguageDirection", type (Property::BOLEAN), Read/Write
     * @note The default value is true
     *
     * If MATCH_SYSTEM_LANGUAGE_DIRECTION property set true, the default text alignment to match the direction of the system language.
     *
     * ex) Current system language direction LTR.
     *     TextLabel::New("Hello world \n  ﻡﺮﺤﺑﺍ. ");
     *     TextLabel::Property::HORIZONTAL_ALIGNMENT, "END"
     *
     * | TextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION                 |
     * |-----------------------------------------------------------------------
     * |               false               |          true (default)          |
     * |-----------------------------------|----------------------------------|
     * |                     Hello world   |                  Hello world     |
     * |   ﻡﺮﺤﺑﺍ.                     |                          ﻡﺮﺤﺑﺍ.      |
     *
     */
  MATCH_SYSTEM_LANGUAGE_DIRECTION,

  /**
     * @brief The text fit parameters.
     * @details Name "textFit", type Property::MAP.
     * @note The default value is false
     *
     * The textFit map contains the following keys:
     *
     * | %Property Name       | Type     | Required | Description                                                                                                        |
     * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
     * | enable               | BOOLEAN  | No       | True to enable the text fit or false to disable (the default value is false)                                     |
     * | minSize              | FLOAT    | No       | Minimum Size for text fit (the default value is 10.f)                                                     |
     * | maxSize              | FLOAT    | No       | Maximum Size for text fit (the default value is 100.f)                                                     |
     * | stepSize             | FLOAT    | No       | Step Size for font increase (the default value is 1.f)                                                     |
     * | fontSizeType         | STRING   | No       | The size type of font, You can choose between "pointSize" or "pixelSize". (the default value is "pointSize")                                                     |
     */
  TEXT_FIT,

  /**
     * @brief Sets the height of the line in points.
     * @details Name "lineSize", type Property::FLOAT.
     * @note If the font size is larger than the line size, it works with the font size.
     */
  MIN_LINE_SIZE,

  /**
     * @brief The type of rendering e.g. bitmap-based.
     * @details Name "renderingBackend", type Property::INT.
     */
  RENDERING_BACKEND,

  /**
   * @brief The font size scale for scaling the specified font size up or down.
   * @details name "fontSizeScale", type Property::FLOAT.
   * @note The default value is 1.0 which does nothing.
   * The given font size scale value is used for multiplying the specified font size before querying fonts.
   *
   * e.g. The rendering results of both cases are same.
   *  - fontSize: 15pt, fontSizeScale: 1.0
   *  - fontSize: 10pt, fontSizeScale: 1.5
   */
  FONT_SIZE_SCALE,

  /**
   * @brief True to enable the font size scale or false to disable.
   * @details Name "enableFontSizeScale", type Property::BOOLEAN.
   * @note The default value is true.
   * If false, font size scale is not apppied.
   */
  ENABLE_FONT_SIZE_SCALE,

  /**
  * @brief The enumerations used to specify whether to position the ellipsis at the END, START or MIDDLE of the text.
  * @details Name "EllipsisPosition", type [Type](@ref Dali::Toolkit::DevelText::EllipsisPosition::Type) (Property::INTEGER), or Property::STRING. Read/Write
  * @note Default is EllipsisPosition::END.
  * @see DevelText::EllipsisPosition
  */
  ELLIPSIS_POSITION,

  /**
   * @brief The default strikethrough parameters.
   * @details Name "strikethrough", type Property::MAP.
   *
   * The strikethrough map contains the following keys:
   *
   * | %Property Name       | Type     | Required | Description                                                                                                        |
   * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
   * | enable               | BOOLEAN  | No       | True to enable the strikethrough or false to disable (the default value is false)                                  |
   * | color                | VECTOR4  | No       | The color of the strikethrough (the default value is Color::BLACK)                                                 |
   * | height               | FLOAT    | No       | The height of the strikethrough (the default value is 0)                                                               |
   *
   */
  STRIKETHROUGH,

  /**
  * @brief The spaces between characters in Pixels.
  * @details Name "characterSpacing", type Property::FLOAT.
  * @note
  *   A positive value will make the characters far apart (expanded) and a negative value will bring them closer (condensed).
  *   The default value is 0.f which does nothing.
  */
  CHARACTER_SPACING,

  /**
   * @brief the relative height of the line (a factor that will be multiplied by text height).
   * @details Name "relativeLineSize", type Property::FLOAT.
   * @note If the value is less than 1, the lines could to be overlapped.
   */
  RELATIVE_LINE_SIZE,

  /**
   * @brief The anchor color that will be used by default in markup processing.
   * @details Name "anchorColor", type Property::VECTOR4.
   * @note If there is a color attribute in the anchor tag, the markup attribute takes precedence.
   */
  ANCHOR_COLOR,

  /**
   * @brief The anchor clicked color that will be used by default in markup processing.
   * @details Name "anchorClickedColor", type Property::VECTOR4.
   * @note If there is a color attribute in the anchor tag, the markup attribute takes precedence.
   */
  ANCHOR_CLICKED_COLOR,

  /**
   * @brief Whether to trim the xBearing of first glyph of the text.
   * @details Name "removeFrontInset", type Property::BOOLEAN.
   */
  REMOVE_FRONT_INSET,

  /**
   * @brief Whether to trim the advance of last glyph of the text.
   * @details Name "removeBackInset", type Property::BOOLEAN.
   */
  REMOVE_BACK_INSET,
};

} // namespace Property

struct FitOption
{
  FitOption(float pointSize = 0.0f, float minLineSize = 0.0f)
  : mPointSize(pointSize), mMinLineSize(minLineSize) {}

  float GetPointSize() const
  {
    return mPointSize;
  }
  float GetMinLineSize() const
  {
    return mMinLineSize;
  }
  void SetPointSize(float pointSize)
  {
    mPointSize = pointSize;
  }
  void SetMinLineSize(float minLineSize)
  {
    mMinLineSize = minLineSize;
  }

private:
  float mPointSize   = 0.0f;
  float mMinLineSize = 0.0f;
};

/**
 * @brief Get the rendered size of a specific text range.
 * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
 * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] startIndex start index of the text requested to calculate size for.
 * @param[in] endIndex end index(included) of the text requested to calculate size for.
 * @return list of sizes of the reuested text.
 */
DALI_TOOLKIT_API Vector<Vector2> GetTextSize(TextLabel textLabel, const uint32_t startIndex, const uint32_t endIndex);

/**
 * @brief Get the top/left rendered position of a specific text range.
 * if the requested text is at multilines, multiple positions will be returned for each text located in a separate line.
 * if a line contains characters with different directions, multiple positions will be returned for each block of contiguous characters with the same direction.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] startIndex start index of the text requested to get position to.
 * @param[in] endIndex end index(included) of the text requested to get position to.
 * @return list of positions of the requested text.
 */
DALI_TOOLKIT_API Vector<Vector2> GetTextPosition(TextLabel textLabel, const uint32_t startIndex, const uint32_t endIndex);

/**
 * @brief Gets the bounding box of a specific text range.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] startIndex start index of the text requested to get bounding box to.
 * @param[in] endIndex end index(included) of the text requested to get bounding box to.
 * @return bounding box of the requested text.
 */
DALI_TOOLKIT_API Rect<> GetTextBoundingRectangle(TextLabel textLabel, uint32_t startIndex, uint32_t endIndex);

/**
 * @brief Set text fit array to text label.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] enable Whether the text fit array is enabled or not.
 * @param[in] fitOptions list of the fit options.
 */
DALI_TOOLKIT_API void SetTextFitArray(TextLabel textLabel, const bool enable, std::vector<FitOption>& fitOptions);

/**
 * @brief Get the text fit array of text label.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @return list of the fit options.
 */
DALI_TOOLKIT_API std::vector<FitOption>& GetTextFitArray(TextLabel textLabel);

/**
 * @brief Whether the text fit array is enabled or not.
 *
 * @return True if the text fit array is enabled.
 */
DALI_TOOLKIT_API bool IsTextFitArrayEnabled(TextLabel textLabel);

/**
 * @brief Set removing front inset to text label.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] remove Whether front inset of text label has to be removed or not.
 */
DALI_TOOLKIT_API void SetRemoveFrontInset(TextLabel textLabel, const bool remove);

/**
 * @brief Whether front inset of text label is removed or not.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @return True if the front inset of text label is removed.
 */
DALI_TOOLKIT_API bool IsRemoveFrontInset(TextLabel textLabel);

/**
 * @brief Set removing back inset to text label.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @param[in] remove Whether back inset of text label has to be removed or not.
 */
DALI_TOOLKIT_API void SetRemoveBackInset(TextLabel textLabel, const bool remove);

/**
 * @brief Whether back inset of text label is removed or not.
 *
 * @param[in] textLabel The instance of TextLabel.
 * @return True if the back inset of text label is removed.
 */
DALI_TOOLKIT_API bool IsRemoveBackInset(TextLabel textLabel);

/**
 * @brief Anchor clicked signal type.
 *
 * @note Signal
 *  - const char* : href of clicked anchor.
 *  - uint32_t    : length of href.
 */
using AnchorClickedSignalType = Signal<void(TextLabel, const char*, uint32_t)>;

/**
 * @brief TextFit property changed signal type.
 */
using TextFitChangedSignalType = Signal<void(TextLabel)>;

/**
 * @brief This signal is emitted when the anchor is clicked.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextLabel textLabel, const char* href, uint32_t hrefLength);
 * @endcode
 * @param[in] textLabel The instance of TextLabel.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API AnchorClickedSignalType& AnchorClickedSignal(TextLabel textLabel);

/**
 * @brief This signal is emitted when the textfit property is changed.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(TextLabel textLabel);
 * @endcode
 * @param[in] textLabel The instance of TextLabel.
 * @return The signal to connect to.
 */
DALI_TOOLKIT_API TextFitChangedSignalType& TextFitChangedSignal(TextLabel textLabel);

} // namespace DevelTextLabel

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LABEL_DEVEL_H
