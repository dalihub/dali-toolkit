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

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class GridAxis;
using GridAxisPtr = IntrusivePtr<GridAxis>;

/* @brief This internal class houses the algorithm for computing the locations of grid lines;
 * along either the horizontal or vertical axis.
 * 
 * A Grid layout uses two instances of this class
 * distinguished by the "horizontal" flag which is true for the horizontal axis and false
 * for the vertical one.
 */

class GridAxis
{
public:
  static GridAxisPtr New();

public:

  GridAxis();

  ~GridAxis();

  unsigned int GetLocations()

  void ComputeLocations( unsigned int[] a );

private:
  GridAxis( const GridAxis& other ) = delete;
  GridAxis& operator=( const GridAxis& other ) = delete;


private:

  static final int NEW = 0;
  static final int PENDING = 1;
  static final int COMPLETE = 2;

  final boolean horizontal;

  int definedCount = UNDEFINED;
  int maxIndex = UNDEFINED;

  PackedMap<Spec, Bounds> groupBounds;
  boolean groupBoundsValid = false;

  PackedMap<Interval, MutableInt> forwardLinks;
  boolean forwardLinksValid = false;

  PackedMap<Interval, MutableInt> backwardLinks;
  boolean backwardLinksValid = false;

  int[] leadingMargins;
  boolean leadingMarginsValid = false;

  int[] trailingMargins;
  boolean trailingMarginsValid = false;

  Arc[] arcs;
  boolean arcsValid = false;

  int[] locations;
  boolean locationsValid = false;

  boolean hasWeights;
  boolean hasWeightsValid = false;
  int[] deltas;

  boolean orderPreserved = DEFAULT_ORDER_PRESERVED;

  MutableInt parentMin = new MutableInt(0);
  MutableInt parentMax = new MutableInt(-MAX_SIZE);

};

} // namespace Internal

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_GRID_AXIS_H