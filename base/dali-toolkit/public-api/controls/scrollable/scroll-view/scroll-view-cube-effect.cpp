//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-cube-effect.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-cube-effect-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

ScrollViewCubeEffect ScrollViewCubeEffect::New()
{
  return ScrollViewCubeEffect(new Internal::ScrollViewCubeEffect());
}

ScrollViewCubeEffect::ScrollViewCubeEffect()
{

}

ScrollViewCubeEffect::ScrollViewCubeEffect(Internal::ScrollViewCubeEffect *impl)
: ScrollViewEffect(impl)
{
}

ScrollViewCubeEffect ScrollViewCubeEffect::DownCast( BaseHandle handle )
{
  return ScrollViewCubeEffect( dynamic_cast<Internal::ScrollViewCubeEffect*>(handle.GetObjectPtr()) );
}

void ScrollViewCubeEffect::ApplyToActor(Actor child,
                                        const Vector3& anchor,
                                        const Vector2& angleSwing,
                                        const Vector2& positionSwing)
{
  GetImpl(*this).ApplyToActor( child, anchor, angleSwing, positionSwing );
}

void ScrollViewCubeEffect::ApplyToActor(Actor child,
                                        Actor parentPage,
                                        const Vector3& anchor,
                                        const Vector2& angleSwing,
                                        const Vector2& positionSwing)
{
  GetImpl(*this).ApplyToActor( child, parentPage, anchor, angleSwing, positionSwing );
}

} // namespace Toolkit

} // namespace Dali
