/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/property-input.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

void MoveActorConstraint( Vector3& current, const PropertyInputContainer& inputs )
{
  current += Vector3(inputs[0]->GetVector2());
}

void WrapActorConstraint( Vector3& position, const PropertyInputContainer& inputs )
{
  bool wrap = inputs[5]->GetBoolean();

  if(wrap)
  {
    const Vector2& min = inputs[3]->GetVector2();
    const Vector2& max = inputs[4]->GetVector2();

    const Vector3& anchor = inputs[1]->GetVector3();
    const Vector3 scale = inputs[0]->GetVector3();
    const Vector3 size = inputs[2]->GetVector3();

    if(fabsf(min.x - max.x) > Math::MACHINE_EPSILON_1)
    {
      // WRAP X (based on the position of the right side)
      float offsetX = (1.0f - anchor.x) * size.x * scale.x;
      position.x = WrapInDomain(position.x + offsetX, min.x, max.x) - offsetX;
    }

    if(fabsf(min.y - max.y) > Math::MACHINE_EPSILON_1)
    {
      // WRAP Y (based on the position of the bottom side)
      float offsetY = (1.0f - anchor.y) * size.y * scale.y;
      position.y = WrapInDomain(position.y + offsetY, min.y, max.y) - offsetY;
    }
  }
}

} // namespace Toolkit

} // namespace Dali

