#ifndef __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_COLOR_H__
#define __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_COLOR_H__

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

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Tag;
struct ColorRun;

/**
 * @brief Retrieves the color value from the tag and sets it to the color run.
 *
 * @param[in] tag The color tag and its attributes.
 * @param[in,out] colorRun The color run.
 */
void ProcessColorTag( const Tag& tag, ColorRun& colorRun );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_COLOR_H__
