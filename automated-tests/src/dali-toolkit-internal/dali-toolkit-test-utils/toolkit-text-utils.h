#ifndef DALI_TOOLKIT_TEXT_UTILS_H
#define DALI_TOOLKIT_TEXT_UTILS_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/metrics.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-model.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Some layout options.
 */
struct LayoutOptions
{
  LayoutOptions()
  : align{ true }
  {}

  bool align   : 1; ///< Whether to align the lines.
};

/**
 * @brief Creates and fills all the vectors of a text model: characters in utf32, segmentation info,
 * scripts, fonts, bidi info, glyphs, conversion tables, etc.
 *
 * @param[in] text The given text.
 * @param[in] textArea The area where to layout the text.
 * @param[in] fontDescriptions The fonts to be used.
 * @param[in] options Layout options.
 * @param[out] layoutSize The laid-out size.
 * @param[out] textModel Pointer to a text model instance.
 * @param[out] metrics Pointer to a wrapper around FontClient used to get metrics.
 * @param[in] markupProcessorEnabled Enable markup processor to use markup text.
 * @param[in] wrapMode Line wrap mode.
 * @param[in] ellipsisEnabled Whether the ellipsis layout option is enabled.
 * @param[in] ellipsisPosition Where is the location the text elide.
 * @param[in] lineSpacing The height of the line in points.
 */
void CreateTextModel( const std::string& text,
                      const Size& textArea,
                      const Vector<FontDescriptionRun>& fontDescriptions,
                      const LayoutOptions& options,
                      Size& layoutSize,
                      ModelPtr& textModel,
                      MetricsPtr& metrics,
                      bool markupProcessorEnabled,
                      LineWrap::Mode wrapMode,
                      bool ellipsisEnabled,
                      DevelText::EllipsisPosition::Type ellipsisPosition,
                      float lineSpacing);

/**
 * @brief Configures the text @p controller similarly to the one configured by the text-label.
 *
 * @param[in,out] The text controller to configure.
 */
void ConfigureTextLabel( ControllerPtr controller );

/**
 * @brief Configures the text @p controller similarly to the one configured by the text-field.
 *
 * @param[in,out] The text controller to configure.
 */
void ConfigureTextField( ControllerPtr controller );

/**
 * @brief Configures the text @p controller similarly to the one configured by the text-editor.
 *
 * @param[in,out] The text controller to configure.
 */
void ConfigureTextEditor( ControllerPtr controller );


/**
 * @brief Creates one FontDescriptionRun then add it to FontDescription list.
 *
 * @param[in] characterRun The initial character index and the number of characters of the run.
 * @param[in] fontFamilyName The font's family name.
 * @param[in] weight The font's weight.
 * @param[in] width The font's width.
 * @param[in] slant The font's slant.
 * @param[in] size Whether the font's family is defined.
 * @param[in] familyDefined Whether the font's weight is defined.
 * @param[in] weightDefined Whether the font's width is defined.
 * @param[in] widthDefined Whether the ellipsis layout option is enabled.
 * @param[in] slantDefined Whether the font's slant is defined.
 * @param[in] sizeDefined Whether the font's size is defined.

* @return vector contains one FontDescriptionRun.
 */
Vector<FontDescriptionRun> CreateSingleFontDescription(
                    const CharacterRun&         characterRun,
                    const std::string           fontFamilyName,
                    const FontWeight            weight,
                    const FontWidth             width,
                    const FontSlant             slant,
                    const PointSize26Dot6       size,
                    const bool                  familyDefined,
                    const bool                  weightDefined,
                    const bool                  widthDefined,
                    const bool                  slantDefined,
                    const bool                  sizeDefined);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UTILS_H
