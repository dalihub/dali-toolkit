#ifndef DALI_TOOLKIT_TEXT_UTILS_H
#define DALI_TOOLKIT_TEXT_UTILS_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
 */
void CreateTextModel( const std::string& text,
                      const Size& textArea,
                      const Vector<FontDescriptionRun>& fontDescriptions,
                      const LayoutOptions& options,
                      Size& layoutSize,
                      ModelPtr& textModel,
                      MetricsPtr& metrics,
                      bool markupProcessorEnabled );

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

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UTILS_H
