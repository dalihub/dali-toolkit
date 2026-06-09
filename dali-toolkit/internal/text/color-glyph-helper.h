/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#ifndef DALI_TOOLKIT_INTERNAL_TEXT_COLOR_GLYPH_HELPER_H
#define DALI_TOOLKIT_INTERNAL_TEXT_COLOR_GLYPH_HELPER_H

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
inline bool IsRenderableColorGlyph(TextAbstraction::FontClient& fontClient,
                                   TextAbstraction::FontId      fontId,
                                   TextAbstraction::GlyphIndex  glyphIndex)
{
  if(fontClient.IsRenderableColrV1Font(fontId))
  {
    return fontClient.IsRenderableColrV1Glyph(fontId, glyphIndex);
  }
  return fontClient.IsColorGlyph(fontId, glyphIndex);
}

inline bool IsColorGlyphBuffer(const TextAbstraction::GlyphBufferData& glyphBitmap)
{
  return glyphBitmap.isColorEmoji ||
         glyphBitmap.isColorBitmap ||
         Pixel::BGRA8888 == glyphBitmap.format;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_COLOR_GLYPH_HELPER_H
