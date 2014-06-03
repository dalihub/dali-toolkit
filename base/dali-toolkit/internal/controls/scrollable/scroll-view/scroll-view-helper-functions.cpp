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

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-helper-functions.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace ScrollViewHelperFunctions
{

bool IsStraightOnView( const Vector3& position )
{
  return ( fabsf(position.x) < Math::MACHINE_EPSILON_1 ) && ( fabsf( position.y ) < Math::MACHINE_EPSILON_1 );
}

void WrapPositionWithinDomain( Vector3& position, const Vector3& pageSize, const Vector3& min, const Vector3& max )
{
  if( fabsf( min.x - max.x ) > Math::MACHINE_EPSILON_1 )
  {
    // WRAP X (based on the position of the right side)
    position.x = WrapInDomain( position.x + pageSize.width, min.x, max.x ) - pageSize.width;
  }

  if( fabsf( min.y - max.y ) > Math::MACHINE_EPSILON_1 )
  {
    // WRAP Y (based on the position of the bottom side)
    position.y = WrapInDomain( position.y + pageSize.height, min.y, max.y ) - pageSize.height;
  }
}

bool IsOutsideView( const Vector3& position, const Vector3& pageSize )
{
  return ( fabsf( position.x ) >= pageSize.width ) || ( fabsf( position.y ) >= pageSize.height );
}

} // namespace ScrollViewHelperFunctions

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
