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
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-slide-effect-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

const std::string ScrollViewSlideEffect::EFFECT_TIME( "ScrollViewSlideEffect::EFFECT_TIME" );
const std::string ScrollViewSlideEffect::EFFECT_REFERENCE( "ScrollViewSlideEffect::EFFECT_REFERENCE" );
const std::string ScrollViewSlideEffect::EFFECT_ACTIVE( "ScrollViewSlideEffect::EFFECT_ACTIVE" );

ScrollViewSlideEffect ScrollViewSlideEffect::New()
{
  return ScrollViewSlideEffect(new Internal::ScrollViewSlideEffect());
}

ScrollViewSlideEffect::ScrollViewSlideEffect()
{

}

ScrollViewSlideEffect::ScrollViewSlideEffect(Internal::ScrollViewSlideEffect *impl)
: ScrollViewEffect(impl)
{
}

bool ScrollViewSlideEffect::GetSlideDirection() const
{
  return GetImpl(*this).GetSlideDirection();
}

void ScrollViewSlideEffect::SetSlideDirection(bool vertical)
{
  GetImpl(*this).SetSlideDirection( vertical );
}

Vector3 ScrollViewSlideEffect::GetDelayReferenceOffset() const
{
  return GetImpl(*this).GetDelayReferenceOffset();
}

void ScrollViewSlideEffect::SetDelayReferenceOffset(const Vector3& offset)
{
  GetImpl(*this).SetDelayReferenceOffset(offset);
}

float ScrollViewSlideEffect::GetMaxDelayDuration() const
{
  return GetImpl(*this).GetMaxDelayDuration();
}

void ScrollViewSlideEffect::SetMaxDelayDuration(float offset)
{
  GetImpl(*this).SetMaxDelayDuration(offset);
}

ScrollViewSlideEffect ScrollViewSlideEffect::DownCast( BaseHandle handle )
{
  return ScrollViewSlideEffect( dynamic_cast<Internal::ScrollViewSlideEffect*>(handle.GetObjectPtr()) );
}

void ScrollViewSlideEffect::ApplyToActor( Actor child,
                                          float delayMin,
                                          float delayMax )
{
  GetImpl(*this).ApplyToActor( child,
                               delayMin,
                               delayMax );
}

} // namespace Toolkit

} // namespace Dali
