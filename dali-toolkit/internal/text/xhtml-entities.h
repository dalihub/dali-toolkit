#ifndef DALI_TOOLKIT_TEXT_XHTML_ENTITIES_H
#define DALI_TOOLKIT_TEXT_XHTML_ENTITIES_H

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
/**
 * @brief Retrieves UTF8 entity code for corresponding XHTML named Entity.
 *
 * @param[in] markupText The XHTML named entity.
 * @param[int] len Length of markupText.
 *
 * @return pointer to UTF8 entity code if namedEntity found in table otherwise NULL
 */
const char* const NamedEntityToUtf8(const char* const markupText, unsigned int len);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_XHTML_ENTITIES_H
