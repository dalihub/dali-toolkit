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

unsigned int GridAxis::GetLocations()
{
  if( locations == null )
  {
    int N = getCount() + 1;
    locations = new int[N];
  }
  if( !locationsValid )
  {
    computeLocations(locations);
    locationsValid = true;
  }
  return locations;
}

void GridAxis::ComputeLocations( unsigned int[] a )
{
  if( !hasWeights() ) 
  {
    solve(a);
  }
  else
  {
    solveAndDistributeSpace( a );
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