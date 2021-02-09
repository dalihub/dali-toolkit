#ifndef DALI_TOOLKIT_TEXT_EMBEDDED_ITEM_H
#define DALI_TOOLKIT_TEXT_EMBEDDED_ITEM_H

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
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief An embedded item within the text.
 *
 * The @e url of the image is optional. If there is no image
 * the layout engine will use the @e width and @e height to
 * create a space inside the text. This gap can be filled later.
 *
 * A color blending mode can be set. The default is NONE, the
 * image will use its own color. If MULTIPLY is set, the color
 * of the image will be multiplied by the color of the text.
 */
struct EmbeddedItem
{
  CharacterIndex    characterIndex;    ///< The character's index of the embedded item within the string.
  char*             url;               ///< The url path of the image.
  Length            urlLength;         ///< The length of the url.
  unsigned int      width;             ///< The width of the item.
  unsigned int      height;            ///< The height of the item.
  ColorBlendingMode colorBlendingMode; ///< Whether the color of the image is multiplied by the color of the text.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EMBEDDED_ITEM_H
