#ifndef DALI_TOOLKIT_TEXT_LAYOUT_ALIGNMENT_H
#define DALI_TOOLKIT_TEXT_LAYOUT_ALIGNMENT_H

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

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Layout
{

/**
 * @brief Speficies the horizontal alignment.
 *
 * BEGIN is on the left for Left To Right languages whereas is right for Right To Left.
 * Similarly, END is on the right for Left To Right languages and left for Right To Left.
 */
enum HorizontalAlignment
{
  HORIZONTAL_ALIGN_BEGIN,
  HORIZONTAL_ALIGN_CENTER,
  HORIZONTAL_ALIGN_END
};

/**
 * @brief Speficies the vertical alignment.
 */
enum VerticalAlignment
{
  VERTICAL_ALIGN_TOP,
  VERTICAL_ALIGN_CENTER,
  VERTICAL_ALIGN_BOTTOM
};

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LAYOUT_ALIGNMENT_H
