#ifndef __DALI_TOOLKIT_TEXT_CURSOR_HELPER_FUNCTIONS_H__
#define __DALI_TOOLKIT_TEXT_CURSOR_HELPER_FUNCTIONS_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/metrics.h>
#include <dali-toolkit/internal/text/visual-model-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct CursorInfo
{
  CursorInfo()
  : primaryPosition(),
    secondaryPosition(),
    lineOffset( 0.f ),
    lineHeight( 0.f ),
    primaryCursorHeight( 0.f ),
    secondaryCursorHeight( 0.f ),
    isSecondaryCursor( false )
  {}

  ~CursorInfo()
  {}

  Vector2 primaryPosition;       ///< The primary cursor's position (in text's coords).
  Vector2 secondaryPosition;     ///< The secondary cursor's position (in text's coords).
  float   lineOffset;            ///< The vertical offset where the line containing the cursor starts.
  float   lineHeight;            ///< The height of the line where the cursor is placed.
  float   primaryCursorHeight;   ///< The primary cursor's height.
  float   secondaryCursorHeight; ///< The secondary cursor's height.
  bool    isSecondaryCursor;     ///< Whether the secondary cursor is valid.
};

/**
 * @brief Retrieves the closest line for a given touch point.
 *
 * It returns the first line if the touch point is above the text and the last line if the touch point is below.
 *
 * @param[in] visualModel The visual model.
 * @param[in] visualY The touch point 'y' in text's coords.
 *
 * @return A line index.
 */
LineIndex GetClosestLine( VisualModelPtr visualModel,
                          float visualY );

/**
 * @brief Calculates the vertical line's offset for a given line.
 *
 * @pre @p lineIndex must be between 0 and the number of lines (both inclusive).
 *
 * @param[in] lines The laid-out lines.
 * @param[in] lineIndex Index to the line.
 *
 * @return The vertical offset of the given line.
 */
float CalculateLineOffset( const Vector<LineRun>& lines,
                           LineIndex lineIndex );

/**
 * @brief Retrieves the cursor's logical position for a given touch point x,y
 *
 * @param[in] visualModel The visual model.
 * @param[in] logicalModel The logical model.
 * @param[in] metrics A wrapper around FontClient used to get metrics.
 * @param[in] visualX The touch point 'x' in text's coords.
 * @param[in] visualY The touch point 'y' in text's coords.
 *
 * @return The logical cursor position (in characters). 0 is just before the first character, a value equal to the number of characters is just after the last character.
 */
CharacterIndex GetClosestCursorIndex( VisualModelPtr visualModel,
                                      LogicalModelPtr logicalModel,
                                      MetricsPtr metrics,
                                      float visualX,
                                      float visualY );


/**
 * @brief Calculates the cursor's position for a given character index in the logical order.
 *
 * It retrieves as well the line's height and the cursor's height and
 * if there is a valid alternative cursor, its position and height.
 *
 * @param[in] visualModel The visual model.
 * @param[in] logicalModel The logical model.
 * @param[in] metrics A wrapper around FontClient used to get metrics.
 * @param[in] logical The logical cursor position (in characters). 0 is just before the first character, a value equal to the number of characters is just after the last character.
 * @param[out] cursorInfo The line's height, the cursor's height, the cursor's position and whether there is an alternative cursor.
 */
void GetCursorPosition( VisualModelPtr visualModel,
                        LogicalModelPtr logicalModel,
                        MetricsPtr metrics,
                        CharacterIndex logical,
                        CursorInfo& cursorInfo );

/**
 * @brief Find the indices to the first and last characters of a word for the given touch point.
 *
 * @param[in] visualModel The visual model.
 * @param[in] logicalModel The logical model.
 * @param[in] metrics A wrapper around FontClient used to get metrics.
 * @param[in] visualX The touch point 'x' in text's coords.
 * @param[in] visualY The touch point 'y' in text's coords.
 * @param[out] startIndex Index to the first character of the selected word.
 * @param[out] endIndex Index to the last character of the selected word.
 *
 * @return @e true if the indices are found.
 */
bool FindSelectionIndices( VisualModelPtr visualModel,
                           LogicalModelPtr logicalModel,
                           MetricsPtr metrics,
                           float visualX,
                           float visualY,
                           CharacterIndex& startIndex,
                           CharacterIndex& endIndex );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_CURSOR_HELPER_FUNCTIONS_H__
