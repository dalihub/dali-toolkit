#ifndef DALI_TOOLKIT_TEXT_ENUMERATIONS_DEVEL_H
#define DALI_TOOLKIT_TEXT_ENUMERATIONS_DEVEL_H

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
namespace DevelText
{
namespace TextDirection
{
enum Type
{
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT
};

} // namespace TextDirection

namespace VerticalLineAlignment
{
enum Type
{
  TOP,
  MIDDLE,
  BOTTOM
};

} // namespace VerticalLineAlignment
namespace LineWrap
{
enum Mode
{
  WORD,
  CHARACTER,
  HYPHENATION, // HYPHENATION mode will add hyphen and move part of the word to the next line.
  MIXED        // MIXEd mode will apply WORD mode, if failed try HYPHENATION mode, if failed try CHARACTER.
};

} // namespace LineWrap

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_TEXT_ENUMERATIONS_DEVEL_H
