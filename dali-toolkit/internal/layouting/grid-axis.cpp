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

//CLASS HEADER
#include <dali-toolkit/internal/layouting/grid-axis.h>

// EXTERNAL HEADERS

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

// GridAxis::GridAxis( unsigned int childCount)
// : mChildCount( childCount )
// {
// }

GridAxis::GridAxis( LayoutGroup& layoutGroup )
: mLayoutGroup( layoutGroup )
{
}

int GridAxis::GetMaxIndex()
{
  if( maxIndex == UNDEFINED )
  {
    maxIndex = std::max( 0, CalculateMaxIndex() ); // use zero when there are no children
  }
  return maxIndex;
}

int GridAxis::CalculateMaxIndex()
{
  // the number Integer.MIN_VALUE + 1 comes up in undefined cells
  int result = -1;
  // for( unsigned int i = 0; i < mChildCount; i++ )
  // {
  //   LayoutItem c = getChildAt(i);
  //   LayoutParams params = getLayoutParams(c);

  //   // Spec defines either the horizontal or vertical characteristics
  //   // of a group of cells.
  //   // It defines grid lines which delimit the
  //   // axis bounds of the group of cells defined by the sepc.
  //   Spec spec = mHorizontal ? params.columnSpec : params.rowSpec;
  //   Interval span = spec.span;
  //   result = max(result, span.min);
  //   result = max(result, span.max);
  //   result = max(result, span.size());
  // }
  return result == -1 ? UNDEFINED : result;
}

GridAxis::LocationVector GridAxis::GetLocations()
{
  if( mLocations.Empty() )
  {
    //int numberOfIndexes = std::max( mDefinedCount, GetMaxIndex() );
    mLocations.PushBack( 0 );
  }
  if( !locationsValid )
  {
    ComputeLocations( mLocations );
    locationsValid = true;
  }
  return mLocations;
}

void GridAxis::ComputeLocations( GridAxis::LocationVector& locations  )
{

}

} // namespace Internal

} // namespace Toolkit
} // namespace Dali