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

#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-wobble-effect.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-wobble-effect-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

const std::string ScrollViewWobbleEffect::EFFECT_OVERSHOOT( "ScrollViewWobbleEffect::EFFECT_OVERSHOOT" );
const std::string ScrollViewWobbleEffect::EFFECT_TIME( "ScrollViewWobbleEffect::EFFECT_TIME" );

ScrollViewWobbleEffect ScrollViewWobbleEffect::New()
{
  return ScrollViewWobbleEffect(new Internal::ScrollViewWobbleEffect());
}

ScrollViewWobbleEffect::ScrollViewWobbleEffect()
{

}

ScrollViewWobbleEffect::ScrollViewWobbleEffect(Internal::ScrollViewWobbleEffect *impl)
: ScrollViewEffect(impl)
{
}

} // namespace Toolkit

} // namespace Dali
