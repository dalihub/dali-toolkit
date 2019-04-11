#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_LOCATIONS_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_LOCATIONS_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

//EXTERNAL HEADERS
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <limits.h>

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class GridLocations;
using GridLocationsPtr = IntrusivePtr<GridLocations>;

/* @brief This internal class houses the algorithm for computing the locations and size of cells.
 *
 * A Grid layout uses two instances of this class
 * distinguished by the "horizontal" flag which is true for the horizontal axis and false
 * for the vertical one.
 */

class GridLocations : public RefObject
{

public:

  static const int UNDEFINED = INT_MIN;

  static const unsigned int HORIZONTAL = 0;
  static const unsigned int VERTICAL = 1;

  struct Cell
  {
    int xStart;
    int xEnd;
    int yTop;
    int yBottom;

    Cell( int x1, int x2, int y1, int y2): xStart(x1), xEnd(x2), yTop(y1), yBottom(y2){};
  };

  typedef std::vector< Cell > LocationVector;

public:

  static GridLocationsPtr New();

  /*
   * Uses the given parameters to calculate the x,y coordinates of each cell and cell size.
   */
  void CalculateLocations( int numberOfColumns, unsigned int availableWidth,
                           unsigned int availableHeight, unsigned int numberOfCells );

  LocationVector GetLocations();

private:

  GridLocations();
  ~GridLocations();
  GridLocations( const GridLocations& other ) = delete;
  GridLocations& operator=( const GridLocations& other ) = delete;

private:

  LocationVector mLocations;

};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_LOCATIONS_H
