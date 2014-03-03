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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-custom-effect.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-custom-effect-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

ScrollViewCustomEffect ScrollViewCustomEffect::New()
{
  return ScrollViewCustomEffect(new Internal::ScrollViewCustomEffect());
}

ScrollViewCustomEffect::ScrollViewCustomEffect()
{

}

ScrollViewCustomEffect::ScrollViewCustomEffect(Internal::ScrollViewCustomEffect *impl)
: ScrollViewEffect(impl)
{
}

ScrollViewCustomEffect ScrollViewCustomEffect::DownCast( BaseHandle handle )
{
  return ScrollViewCustomEffect( dynamic_cast<Internal::ScrollViewCustomEffect*>(handle.GetObjectPtr()) );
}

void ScrollViewCustomEffect::SetPageSpacing(const Vector2& spacing)
{
  GetImpl(*this).SetPageSpacing(spacing);
}

void ScrollViewCustomEffect::SetPageTranslation(const Vector3& translation)
{
  GetImpl(*this).SetPageTranslation(translation);
}

void ScrollViewCustomEffect::SetPageTranslation(const Vector3& translationIn, const Vector3& translationOut)
{
  GetImpl(*this).SetPageTranslation(translationIn, translationOut);
}

void ScrollViewCustomEffect::SetPageTranslationIn(const Vector3& translation)
{
  GetImpl(*this).SetPageTranslationIn(translation);
}

void ScrollViewCustomEffect::SetPageTranslationOut(const Vector3& translation)
{
  GetImpl(*this).SetPageTranslationOut(translation);
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunction(AlphaFunction func)
{
  GetImpl(*this).SetPageTranslateAlphaFunction(func);
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  GetImpl(*this).SetPageTranslateAlphaFunction(funcIn, funcOut);
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunctionIn(AlphaFunction func)
{
  GetImpl(*this).SetPageTranslateAlphaFunctionIn(func);
}

void ScrollViewCustomEffect::SetPageTranslateAlphaFunctionOut(AlphaFunction func)
{
  GetImpl(*this).SetPageTranslateAlphaFunctionOut(func);
}

void ScrollViewCustomEffect::SetGlobalPageRotation(float angle, const Vector3& axis)
{
  GetImpl(*this).SetGlobalPageRotation(angle, axis);
}

void ScrollViewCustomEffect::SetAngledOriginPageRotation(const Vector3& angle)
{
  GetImpl(*this).SetAngledOriginPageRotation(angle);
}

void ScrollViewCustomEffect::SetGlobalPageRotation(float angleIn, const Vector3& axisIn, float angleOut, const Vector3& axisOut)
{
  GetImpl(*this).SetGlobalPageRotation(angleIn, axisIn, angleOut, axisOut);
}

void ScrollViewCustomEffect::SetGlobalPageRotationIn(float angle, const Vector3& axis)
{
  GetImpl(*this).SetGlobalPageRotationIn(angle, axis);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOut(float angle, const Vector3& axis)
{
  GetImpl(*this).SetGlobalPageRotationOut(angle, axis);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOrigin(const Vector3& origin)
{
  GetImpl(*this).SetGlobalPageRotationOrigin(origin);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOrigin(const Vector3& originIn, const Vector3& originOut)
{
  GetImpl(*this).SetGlobalPageRotationOrigin(originIn, originOut);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOriginIn(const Vector3& origin)
{
  GetImpl(*this).SetGlobalPageRotationOriginIn(origin);
}

void ScrollViewCustomEffect::SetGlobalPageRotationOriginOut(const Vector3& origin)
{
  GetImpl(*this).SetGlobalPageRotationOriginOut(origin);
}

void ScrollViewCustomEffect::SetSwingAngle(const float angle, const Vector3& axis)
{
  GetImpl(*this).SetSwingAngle(angle, axis);
}

void ScrollViewCustomEffect::SetSwingAngle(float angleIn, const Vector3& axisIn, float angleOut, const Vector3& axisOut)
{
  GetImpl(*this).SetSwingAngle(angleIn, axisIn, angleOut, axisOut);
}

void ScrollViewCustomEffect::SetSwingAngleIn(float angle, const Vector3& axis)
{
  GetImpl(*this).SetSwingAngleIn(angle, axis);
}

void ScrollViewCustomEffect::SetSwingAngleOut(float angle, const Vector3& axis)
{
  GetImpl(*this).SetSwingAngleOut(angle, axis);
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunction(AlphaFunction func)
{
  GetImpl(*this).SetSwingAngleAlphaFunction(func);
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  GetImpl(*this).SetSwingAngleAlphaFunction(funcIn, funcOut);
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunctionIn(AlphaFunction func)
{
  GetImpl(*this).SetSwingAngleAlphaFunctionIn(func);
}

void ScrollViewCustomEffect::SetSwingAngleAlphaFunctionOut(AlphaFunction func)
{
  GetImpl(*this).SetSwingAngleAlphaFunctionOut(func);
}

void ScrollViewCustomEffect::SetSwingAnchor(const Vector3& anchor)
{
  GetImpl(*this).SetSwingAnchor(anchor);
}

void ScrollViewCustomEffect::SetSwingAnchor(const Vector3& anchorIn, const Vector3& anchorOut)
{
  GetImpl(*this).SetSwingAnchor(anchorIn, anchorOut);
}

void ScrollViewCustomEffect::SetSwingAnchorIn(const Vector3& anchor)
{
  GetImpl(*this).SetSwingAnchorIn(anchor);
}

void ScrollViewCustomEffect::SetSwingAnchorOut(const Vector3& anchor)
{
  GetImpl(*this).SetSwingAnchorOut(anchor);
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunction(AlphaFunction func)
{
  GetImpl(*this).SetSwingAnchorAlphaFunction(func);
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  GetImpl(*this).SetSwingAnchorAlphaFunction(funcIn, funcOut);
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunctionIn(AlphaFunction func)
{
  GetImpl(*this).SetSwingAnchorAlphaFunctionIn(func);
}

void ScrollViewCustomEffect::SetSwingAnchorAlphaFunctionOut(AlphaFunction func)
{
  GetImpl(*this).SetSwingAnchorAlphaFunctionOut(func);
}

void ScrollViewCustomEffect::SetOpacityThreshold(float thresh)
{
  GetImpl(*this).SetOpacityThreshold(thresh);
}

void ScrollViewCustomEffect::SetOpacityThreshold(float threshIn, float threshOut)
{
  GetImpl(*this).SetOpacityThreshold(threshIn, threshOut);
}

void ScrollViewCustomEffect::SetOpacityThresholdIn(float thresh)
{
  GetImpl(*this).SetOpacityThresholdIn(thresh);
}

void ScrollViewCustomEffect::SetOpacityThresholdOut(float thresh)
{
  GetImpl(*this).SetOpacityThresholdOut(thresh);
}

void ScrollViewCustomEffect::SetOpacityAlphaFunction(AlphaFunction func)
{
  GetImpl(*this).SetOpacityAlphaFunction(func);
}

void ScrollViewCustomEffect::SetOpacityAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut)
{
  GetImpl(*this).SetOpacityAlphaFunction(funcIn, funcOut);
}

void ScrollViewCustomEffect::SetOpacityAlphaFunctionIn(AlphaFunction func)
{
  GetImpl(*this).SetOpacityAlphaFunctionIn(func);
}

void ScrollViewCustomEffect::SetOpacityAlphaFunctionOut(AlphaFunction func)
{
  GetImpl(*this).SetOpacityAlphaFunctionOut(func);
}

void ScrollViewCustomEffect::ApplyToPage(Actor page, Vector3 pageSize)
{
  GetImpl(*this).ApplyToPage(page, pageSize);
}

} // namespace Toolkit

} // namespace Dali
