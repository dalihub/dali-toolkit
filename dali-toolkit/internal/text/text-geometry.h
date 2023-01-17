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

// EXTERNAL INCLUDES
#include <dali/public-api/math/rect.h>

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

/**
  * @brief Get the line bounding rectangle.
  * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
  *
  * @param[in] textModel text model containing line info.
  * @param[in] lineIndex line index to which we want to calculate the geometry for.
  * @return bounding rectangle.
  */
Rect<float> GetLineBoundingRect(ModelPtr textModel, const uint32_t lineIndex);

/**
  * @brief Get the left point of the line (x).
  *
  * @param[in] lineRun the requested line.
  * @return x point of the line.
  */
float GetLineLeft(const LineRun& lineRun);

/**
  * @brief Get the top point of the line (y).
  * if the requested line id the first; the top = 0, else the top = the heights of the previouse lines.
  *
  * @param[in] lines   the lines in the text controller.
  * @param[in] lineRun the requested line.
  * @return y point of the line.
  */
float GetLineTop(const Vector<LineRun>& lines, const LineRun& lineRun);

/**
  * @brief Get the rendered size and position of a specific character in a rectangle.
  * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
  * The Width  of the character = glyph.advance
  * The Height of the character = glyph.height
  * The Left (X) of the character = characterPosition.x - glyph.xBearing; Regardless of the language direction RTL or LTR, the x point is the left x-point of the glyph. 
  * The Top  (Y) of the character = characterPosition.y.
  * @param[in] textModel text model containing text info.
  * @param[in] charIndex character index to which we want to calculate the geometry for.
  * @return Rect {x, y, width, height}.
  */
Rect<> GetCharacterBoundingRect(ModelPtr textModel, const uint32_t charIndex);

/**
  * @brief Get the character index.
  * If the text is not yet rendered or the text is empty, -1 is returned.
  *
  * @param[in] textModel text model containing text info.
  * @param[in] visualX visual x position.
  * @param[in] visualY visual y position.
  * @return character index.
  */
int GetCharIndexAtPosition(ModelPtr textModel, float visualX, float visualY);

/**
  * @brief Get the left point of the character (x).
  *
  * @param[in] glyph the requested character glyph.
  * @param[in] characterPosition the position of the requested character.
  * @return x point of the character.
  */
float GetCharacterLeft(const GlyphInfo& glyph, const Vector2& characterPosition);

/**
  * @brief Get the top point of the character (y).
  *
  * @param[in] characterPosition the position of the requested character.
  * @return y point of the character.
  */
float GetCharacterTop(const Vector2& characterPosition);

/**
  * @brief Get the height of the character.
  *
  * @param[in] glyph the requested character glyph.
  * @return height of the character.
  */
float GetCharacterHeight(const GlyphInfo& glyph);

/**
  * @brief Get the left point of the character.
  *
  * @param[in] glyph the requested character glyph.
  * @return width of the character.
  */
float GetCharacterWidth(const GlyphInfo& glyph);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_GEOMETRY_H
