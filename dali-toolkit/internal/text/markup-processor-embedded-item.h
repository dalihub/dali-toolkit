#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_EMBEDDED_ITEM_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_EMBEDDED_ITEM_H

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

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Tag;
struct EmbeddedItem;

/**
 * @brief Retrieves the @e embedded @e item from the @p tag.
 *
 * @param[in] tag The embedded item tag and its attributes.
 * @param[in,out] embeddedItem The embedded item.
 */
void ProcessEmbeddedItem(const Tag& tag, EmbeddedItem& embeddedItem);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_EMBEDDED_ITEM_H
