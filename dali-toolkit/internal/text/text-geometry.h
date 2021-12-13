#ifndef DALI_TOOLKIT_TEXT_GEOMETRY_H
#define DALI_TOOLKIT_TEXT_GEOMETRY_H

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
#include <dali-toolkit/internal/text/text-model.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
   * @brief Get the rendered size & position of a specific text range.
   * if the requested text is at multilines, multiple sizes/positions will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] textModel text model containing text info.
   * @param[in] startIndex start index of the text requested to get position/size for.
   * @param[in] endIndex end index(included) of the text requested to get position/size for.
   * @param[in] sizesList list of sizes for the reuested text.
   * @param[in] positionsList list of positions for the requested text
   */
void GetTextGeometry(ModelPtr textModel, CharacterIndex startIndex, CharacterIndex endIndex, Vector<Vector2>& sizesList, Vector<Vector2>& positionsList);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_GEOMETRY_H
