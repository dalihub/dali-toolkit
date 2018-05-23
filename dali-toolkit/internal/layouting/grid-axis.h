#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_AXIS_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_AXIS_H

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
#include <limits.h>

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class GridAxis;
using GridAxisPtr =IntrusivePtr<GridAxis>;

/* @brief This internal class houses the algorithm for computing the locations of grid lines;
 * along either the horizontal or vertical axis.
 *
 * A Grid layout uses two instances of this class
 * distinguished by the "horizontal" flag which is true for the horizontal axis and false
 * for the vertical one.
 */

class GridAxis
{

typedef Dali::Vector< unsigned int > LocationVector;

static const int UNDEFINED = INT_MIN;

public:
  static GridAxisPtr New();

public:

  /*
   * @brief Constructor
   */
 // GridAxis( unsigned int childCount );

/*
   * @brief Constructor
   */
  GridAxis( LayoutGroup& layoutGroup );

  /*
   * @brief Destructor
   */
  ~GridAxis();

  /*
   * @brief
   * @return
   */
  int GetMaxIndex();

  /*
   * @brief
   * @return
   */
  int CalculateMaxIndex();

  /*
   * @brief
   * @return
   */
  LocationVector GetLocations();

  /*
   * @brief
   * @param[in]
   */
  void ComputeLocations(LocationVector& a );

private:
  GridAxis( const GridAxis& other ) = delete;
  GridAxis& operator=( const GridAxis& other ) = delete;

private:

  const int NEW = 0;
  const int PENDING = 1;
  const int COMPLETE = 2;

  bool mHorizontal;

  int mDefinedCount = UNDEFINED;
  int maxIndex = UNDEFINED;

  // PackedMap<Spec, Bounds> groupBounds;
  // bool groupBoundsValid = false;

  // PackedMap<Interval, MutableInt> forwardLinks;
  // bool forwardLinksValid = false;

  // PackedMap<Interval, MutableInt> backwardLinks;
  // bool backwardLinksValid = false;

  // int[] leadingMargins;
  // bool leadingMarginsValid = false;

  // int[] trailingMargins;
  // bool trailingMarginsValid = false;

  // Arc[] arcs;
  // bool arcsValid = false;

  LayoutGroup& mLayoutGroup;

  unsigned int mChildCount;

  LocationVector mLocations;
  bool locationsValid = false;

  bool hasWeights;
  bool hasWeightsValid = false;
  Dali::Vector<unsigned int>  mDeltas;
};

} // namespace Internal

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_AXIS_H