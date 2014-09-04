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

#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-twist-effect.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-twist-effect-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

const float ScrollViewTwistEffect::DEFAULT_MINIMUM_DISTANCE_FOR_SHRINK( 0.0f );

ScrollViewTwistEffect ScrollViewTwistEffect::New()
{
  return ScrollViewTwistEffect(new Internal::ScrollViewTwistEffect());
}

ScrollViewTwistEffect::ScrollViewTwistEffect()
{

}

ScrollViewTwistEffect::ScrollViewTwistEffect(Internal::ScrollViewTwistEffect *impl)
: ScrollViewEffect(impl)
{
}

ScrollViewTwistEffect ScrollViewTwistEffect::DownCast( BaseHandle handle )
{
  return ScrollViewTwistEffect( dynamic_cast<Internal::ScrollViewTwistEffect*>(handle.GetObjectPtr()) );
}

float ScrollViewTwistEffect::GetMinimumDistanceForShrink() const
{
  return GetImpl(*this).GetMinimumDistanceForShrink();
}

void ScrollViewTwistEffect::SetMinimumDistanceForShrink(float distance)
{
  GetImpl(*this).SetMinimumDistanceForShrink( distance );
}

void ScrollViewTwistEffect::EnableEffect(bool enableFlag)
{
  GetImpl(*this).EnableEffect(enableFlag);
}

void ScrollViewTwistEffect::ApplyToActor( Actor child,
                                          bool additionalEffects,
                                          const Vector2& angleSwing,
                                          float scaleAmount,
                                          float delayMin,
                                          float delayMax )
{
  GetImpl(*this).ApplyToActor( child,
                               additionalEffects,
                               angleSwing,
                               scaleAmount,
                               delayMin,
                               delayMax );
}

void ScrollViewTwistEffect::SetMaxSwingAngle(const Vector2& maxSwingAngle)
{
  GetImpl(*this).SetMaxSwingAngle(maxSwingAngle);
}

Vector2 ScrollViewTwistEffect::GetMaxSwingAngle() const
{
  return GetImpl( *this ).GetMaxSwingAngle();
}

void ScrollViewTwistEffect::SetSwingDropOff(const Vector2& dropOff, const Vector2& distance, AlphaFunction function)
{
  GetImpl(*this).SetSwingDropOff(dropOff, distance, function);
}

void ScrollViewTwistEffect::GetSwingDropOff( Vector2& dropOff, Vector2& distance, AlphaFunction& function ) const
{
  GetImpl(*this).GetSwingDropOff(dropOff, distance, function);
}

} // namespace Toolkit

} // namespace Dali
