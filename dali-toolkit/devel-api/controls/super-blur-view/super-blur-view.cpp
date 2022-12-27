/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/super-blur-view/super-blur-view-impl.h>

namespace Dali
{
namespace Toolkit
{
SuperBlurView::SuperBlurView()
{
}

SuperBlurView SuperBlurView::New(unsigned int blurLevels)
{
  return Internal::SuperBlurView::New(blurLevels);
}

SuperBlurView::SuperBlurView(const SuperBlurView& handle) = default;

SuperBlurView& SuperBlurView::operator=(const SuperBlurView& rhs) = default;

SuperBlurView::SuperBlurView(SuperBlurView&& handle) = default;

SuperBlurView& SuperBlurView::operator=(SuperBlurView&& rhs) = default;

SuperBlurView::~SuperBlurView()
{
}

SuperBlurView SuperBlurView::DownCast(BaseHandle handle)
{
  return Control::DownCast<SuperBlurView, Internal::SuperBlurView>(handle);
}

SuperBlurView::SuperBlurView(Internal::SuperBlurView& implementation)
: Control(implementation)
{
}

SuperBlurView::SuperBlurView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::SuperBlurView>(internal);
}

void SuperBlurView::SetTexture(Texture texture)
{
  GetImpl(*this).SetTexture(texture);
}

Property::Index SuperBlurView::GetBlurStrengthPropertyIndex() const
{
  return GetImpl(*this).GetBlurStrengthPropertyIndex();
}

void SuperBlurView::SetBlurStrength(float blurStrength)
{
  GetImpl(*this).SetBlurStrength(blurStrength);
}

float SuperBlurView::GetCurrentBlurStrength() const
{
  return GetImpl(*this).GetCurrentBlurStrength();
}

SuperBlurView::SuperBlurViewSignal& SuperBlurView::BlurFinishedSignal()
{
  return GetImpl(*this).BlurFinishedSignal();
}

Texture SuperBlurView::GetBlurredTexture(unsigned int level)
{
  return GetImpl(*this).GetBlurredTexture(level);
}
} // namespace Toolkit

} // namespace Dali
