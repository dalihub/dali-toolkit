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
#include <dali-toolkit/devel-api/controls/bubble-effect/bubble-emitter.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/bubble-effect/bubble-emitter-impl.h>

namespace Dali
{
namespace Toolkit
{
BubbleEmitter::BubbleEmitter()
{
}

BubbleEmitter::~BubbleEmitter()
{
}

BubbleEmitter::BubbleEmitter(Internal::BubbleEmitter& implementation)
: Control(implementation)
{
}

BubbleEmitter::BubbleEmitter(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::BubbleEmitter>(internal);
}

BubbleEmitter BubbleEmitter::New(const Vector2& winSize,
                                 Dali::Texture  shapeTexture,
                                 unsigned int   maximumNumberOfBubble,
                                 const Vector2& bubbleSizeRange)
{
  return Internal::BubbleEmitter::New(winSize, shapeTexture, maximumNumberOfBubble, bubbleSizeRange);
}

BubbleEmitter::BubbleEmitter(const BubbleEmitter& handle) = default;

BubbleEmitter& BubbleEmitter::operator=(const BubbleEmitter& rhs) = default;

BubbleEmitter::BubbleEmitter(BubbleEmitter&& handle) = default;

BubbleEmitter& BubbleEmitter::operator=(BubbleEmitter&& rhs) = default;

BubbleEmitter BubbleEmitter::DownCast(BaseHandle handle)
{
  return Control::DownCast<BubbleEmitter, Internal::BubbleEmitter>(handle);
}

Actor BubbleEmitter::GetRootActor()
{
  return GetImpl(*this).GetRootActor();
}

void BubbleEmitter::SetBackground(Dali::Texture bgTexture, const Vector3& hsvDelta)
{
  GetImpl(*this).SetBackground(bgTexture, hsvDelta);
}

void BubbleEmitter::SetBubbleShape(Dali::Texture shapeTexture)
{
  GetImpl(*this).SetBubbleShape(shapeTexture);
}

void BubbleEmitter::SetBubbleScale(float scale)
{
  GetImpl(*this).SetBubbleScale(scale);
}

void BubbleEmitter::SetBubbleDensity(unsigned int density)
{
  GetImpl(*this).SetBubbleDensity(density);
}

void BubbleEmitter::EmitBubble(Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement)
{
  GetImpl(*this).EmitBubble(animation, emitPosition, direction, displacement);
}

void BubbleEmitter::Restore()
{
  GetImpl(*this).Restore();
}

} // namespace Toolkit

} // namespace Dali
