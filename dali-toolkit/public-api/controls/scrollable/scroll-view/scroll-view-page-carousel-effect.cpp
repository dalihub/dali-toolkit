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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-page-carousel-effect.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-page-carousel-effect-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

ScrollViewPageCarouselEffect ScrollViewPageCarouselEffect::New()
{
  return ScrollViewPageCarouselEffect(new Internal::ScrollViewPageCarouselEffect());
}

ScrollViewPageCarouselEffect::ScrollViewPageCarouselEffect()
{

}

ScrollViewPageCarouselEffect::ScrollViewPageCarouselEffect(Internal::ScrollViewPageCarouselEffect *impl)
: ScrollViewEffect(impl)
{
}

ScrollViewPageCarouselEffect ScrollViewPageCarouselEffect::DownCast( BaseHandle handle )
{
  return ScrollViewPageCarouselEffect( dynamic_cast<Internal::ScrollViewPageCarouselEffect*>(handle.GetObjectPtr()) );
}

void ScrollViewPageCarouselEffect::ApplyToPage( Actor page, const Vector2& positionToPageSizeRatio )
{
  GetImpl(*this).ApplyToPage( page, positionToPageSizeRatio );
}

} // namespace Toolkit

} // namespace Dali
