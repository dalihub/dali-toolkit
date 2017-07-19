#ifndef DALI_TOOLKIT_TEXT_LAYOUT_WRAPMODE_H
#define DALI_TOOLKIT_TEXT_LAYOUT_WRAPMODE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

namespace Layout
{

/**
 * @brief Unit of wrapping for moving to next line
 *
 * If layout width too short to show full text,
 * WRAP_MODE_WORD mode will move word to next line,
 * +---------+
 * |HELLO    |
 * |WORLLD   |
 * +---------+
 *
 * but WRAP_MODE_CHARACTER mode will move character by character to next line
 * +---------+
 * |HELLO WOR|
 * |LD       |
 * +---------+
 */

namespace LineWrap {

enum Mode
{
  WORD,
  CHARACTER
};

} // namespace LineWrap

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LAYOUT_WRAPMODE_H
