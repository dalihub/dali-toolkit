/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/animated-property.h>

namespace Dali::Scene3D::Loader
{
void AnimatedProperty::Animate(Animation& anim, GetActor getActor)
{
  if(Actor actor = getActor(*this))
  {
    Property prop = GetProperty(actor);
    if(prop.propertyIndex != Property::INVALID_INDEX)
    {
      if(mKeyFrames)
      {
        anim.AnimateBetween(prop, mKeyFrames, mAlphaFunction, mTimePeriod);
      }
      else if(mValue)
      {
        if(mValue->mIsRelative)
        {
          anim.AnimateBy(prop, mValue->mValue, mAlphaFunction, mTimePeriod);
        }
        else
        {
          anim.AnimateTo(prop, mValue->mValue, mAlphaFunction, mTimePeriod);
        }
      }
    }
  }
}

} // namespace Dali::Scene3D::Loader