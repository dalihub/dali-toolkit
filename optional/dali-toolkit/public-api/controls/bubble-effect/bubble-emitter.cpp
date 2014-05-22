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

#include <dali-toolkit/public-api/controls/bubble-effect/bubble-emitter.h>

//INTERNAL INCLUDES
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

BubbleEmitter::BubbleEmitter( Internal::BubbleEmitter& implementation )
: Control( implementation )
{
}

BubbleEmitter::BubbleEmitter(Dali::Internal::CustomActor* internal)
: Control( internal )
{
  VerifyCustomActorPointer<Internal::BubbleEmitter>( internal );
}

BubbleEmitter BubbleEmitter::New( const Vector2& winSize,
                                  Image shapeImage,
                                  unsigned int maximumNumberOfBubble,
                                  const Vector2& bubbleSizeRange )
{
  return Internal::BubbleEmitter::New( winSize, shapeImage, maximumNumberOfBubble, bubbleSizeRange );
}

BubbleEmitter::BubbleEmitter( const BubbleEmitter& handle )
: Control( handle )
{
}

BubbleEmitter& BubbleEmitter::operator=( const BubbleEmitter& rhs )
{
  if( &rhs != this )
  {
    Control::operator=(rhs);
  }
  return *this;
}

BubbleEmitter BubbleEmitter::DownCast( BaseHandle handle )
{
  return Control::DownCast<BubbleEmitter, Internal::BubbleEmitter>( handle );
}

Actor BubbleEmitter::GetRootActor()
{
  return GetImpl(*this).GetRootActor();
}

void BubbleEmitter::SetBackground( Image bgImage, const Vector3& hsvDelta )
{
  GetImpl(*this).SetBackground( bgImage, hsvDelta );
}

void BubbleEmitter::SetShapeImage( Image shapeImage )
{
  GetImpl(*this).SetShapeImage( shapeImage );
}

void BubbleEmitter::SetBubbleScale( float scale )
{
  GetImpl(*this).SetBubbleScale( scale );
}

void BubbleEmitter::SetBubbleDensity( unsigned int density )
{
  GetImpl(*this).SetBubbleDensity( density );
}

void BubbleEmitter::SetBlendMode( bool enable )
{
  GetImpl(*this).SetBlendMode( enable );
}

void BubbleEmitter::EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  GetImpl(*this).EmitBubble( animation, emitPosition, direction, displacement );
}

void BubbleEmitter::StartExplosion( float duration, float multiple )
{
  GetImpl(*this).StartExplosion( duration, multiple );
}

void BubbleEmitter::Restore()
{
  GetImpl(*this).Restore();
}

} // namespace Toolkit

} // namespace Dali
