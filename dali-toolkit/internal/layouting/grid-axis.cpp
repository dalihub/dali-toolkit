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
#include <algorithm>

// INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item.h>

namespace
{
const int UNDEFINED = INT_MIN;
}

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
  for( int i = 0; i < mChildCount; i++ )
  {
    LayoutItem c = getChildAt(i);
    LayoutParams params = getLayoutParams(c);
    Spec spec = mHorizontal ? params.columnSpec : params.rowSpec;
    Interval span = spec.span;
    result = max(result, span.min);
    result = max(result, span.max);
    result = max(result, span.size());
  }
  return result == -1 ? UNDEFINED : result;
}

GridAxis::LocationVector GridAxis::GetLocations()
{
  if( mLocations.Empty() )
  {
    int numberOfIndexes = std::max( mDefinedCount, GetMaxIndex() );
    int N =numberOfIndexes + 1;
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
  if( !hasWeights() )
  {
    solve( locations );
  }
  else
  {
    solveAndDistributeSpace( locations );
  }

  if( !orderPreserved )
  {
    // Solve returns the smallest solution to the constraint system for which all
    // values are positive. One value is therefore zero - though if the row/col
    // order is not preserved this may not be the first vertex. For consistency,
    // translate all the values so that they measure the distance from a[0]; the
    // leading edge of the parent. After this transformation some values may be
    // negative.
    int a0 = a[0];
    for( int i = 0, N = a.length; i < N; i++ )
    {
      a[i] = a[i] - a0;
    }
  }
}

} // namespace Internal

} // namespace Toolkit
} // namespace Dali