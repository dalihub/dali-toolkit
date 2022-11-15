#ifndef  TOOLKIT_TEXT_GEOMETRY_UTILS_H
#define  TOOLKIT_TEXT_GEOMETRY_UTILS_H

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


#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>



namespace TestTextGeometryUtils
{
void CheckGeometryResult(Vector<Vector2> positionsList, Vector<Vector2> sizeList, Vector<Vector2> expectedPositions, Vector<Vector2> expectedSizes);
void CheckRectGeometryResult(Rect<> rectGeometry, Rect<> expectedRectGeometry);
}

#endif // TOOLKIT_TEXT_GEOMETRY_UTILS_H
