#ifndef __DALI_TOOLKIT_TEXT_LINE_RUN_H__
#define __DALI_TOOLKIT_TEXT_LINE_RUN_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief LineRun
 */
struct LineRun
{
  GlyphIndex glyphIndex;     ///< The initial glyph index.
  Length     numberOfGlyphs; ///< The number of characters of the run.
  // VCC More info to be added. i.e the size, ...
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LINE_RUN_H__
