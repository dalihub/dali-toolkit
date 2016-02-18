#ifndef __DALI_TOOLKIT_TEXT_MODEL_H__
#define __DALI_TOOLKIT_TEXT_MODEL_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Creates and fills all the vectors of a text model: characters in utf32, segmentation info,
 * scripts, fonts, bidi info, glyphs, conversion tables, etc.
 *
 * @param[in] text The given text.
 * @param[in] textArea The area where to layout the text.
 * @param[out] layoutSize The laid-out size.
 * @param[out] Pointer to a logical text model instance.
 * @param[out] Pointer to a visual text model instance.
 */
void CreateTextModel( const std::string& text,
                      const Size& textArea,
                      Size& layoutSize,
                      LogicalModelPtr logicalModel,
                      VisualModelPtr visualModel );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MODEL_H__
