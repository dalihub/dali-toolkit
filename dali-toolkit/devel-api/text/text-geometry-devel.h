#ifndef DALI_TOOLKIT_TEXT_GEOMETRY_DEVEL_H
#define DALI_TOOLKIT_TEXT_GEOMETRY_DEVEL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace TextGeometry
{

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] editor the editor controller that contains the line.
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
   DALI_TOOLKIT_API Rect<float> GetLineBoundingRectangle(TextEditor editor, const uint32_t lineIndex);

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] label the label controller that contains the line.
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
   DALI_TOOLKIT_API Rect<float> GetLineBoundingRectangle(TextLabel label, const uint32_t lineIndex);

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] field the field controller that contains the line.
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
   DALI_TOOLKIT_API Rect<float> GetLineBoundingRectangle(TextField field, const uint32_t lineIndex);

  /**
   * @brief Get the character bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] label text model containing text info.
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
   DALI_TOOLKIT_API Rect<float> GetCharacterBoundingRectangle(TextLabel label, const uint32_t charIndex);

  /**
   * @brief Get the character bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] field text model containing text info.
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
   DALI_TOOLKIT_API Rect<float> GetCharacterBoundingRectangle(TextField field, const uint32_t charIndex);

  /**
   * @brief Get the character bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] editor text model containing text info.
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
   DALI_TOOLKIT_API Rect<float> GetCharacterBoundingRectangle(TextEditor editor, const uint32_t charIndex);

  /**
   * @brief Get the character index.
   * If the text is not yet rendered or the text is empty, -1 is returned.
   *
   * @param[in] label text model containing text info.
   * @param[in] visualX visual x position.
   * @param[in] visualY visual y position.
   * @return character index.
   */
   DALI_TOOLKIT_API int GetCharacterIndexAtPosition(TextLabel label, float visualX, float visualY);

  /**
   * @brief Get the character index.
   * If the text is not yet rendered or the text is empty, -1 is returned.
   *
   * @param[in] field text model containing text info.
   * @param[in] visualX visual x position.
   * @param[in] visualY visual y position.
   * @return character index.
   */
  DALI_TOOLKIT_API int GetCharacterIndexAtPosition(TextField field, float visualX, float visualY);

  /**
   * @brief Get the character index.
   * If the text is not yet rendered or the text is empty, -1 is returned.
   *
   * @param[in] editor text model containing text info.
   * @param[in] visualX visual x position.
   * @param[in] visualY visual y position.
   * @return character index.
   */
  DALI_TOOLKIT_API int GetCharacterIndexAtPosition(TextEditor editor, float visualX, float visualY);


} // namespace TextGeometry

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_TEXT_GEOMETRY_DEVEL_H
