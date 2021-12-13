/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "test-text-geometry-utils.h"

namespace TestTextGeometryUtils
{

void CheckGeometryResult(Vector<Vector2> positionsList, Vector<Vector2> sizeList, Vector<Vector2> expectedPositions, Vector<Vector2> expectedSizes)
{
  unsigned int expectedCount = expectedSizes.Size();

  for(unsigned int i = 0; i < expectedCount; i++)
  {
    DALI_TEST_EQUALS((int)positionsList[i].x, (int)expectedPositions[i].x, TEST_LOCATION);
    DALI_TEST_EQUALS((int)positionsList[i].y, (int)expectedPositions[i].y, TEST_LOCATION);

    DALI_TEST_EQUALS((int)sizeList[i].x, (int)expectedSizes[i].x, TEST_LOCATION);
    DALI_TEST_EQUALS((int)sizeList[i].y, (int)expectedSizes[i].y, TEST_LOCATION);
  }
}

}