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

// CLASS HEADER
#include "cube-transition-effect.h"

//INTERNAL INCLUDES
#include <dali-toolkit/internal/transition-effects/cube-transition-effect-impl.h>

namespace Dali
{

namespace Toolkit
{

CubeTransitionEffect::CubeTransitionEffect()
{
}

CubeTransitionEffect::~CubeTransitionEffect()
{
}

CubeTransitionEffect::CubeTransitionEffect( Internal::CubeTransitionEffect& implementation )
: Control( implementation )
{
}

CubeTransitionEffect::CubeTransitionEffect( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::CubeTransitionEffect>(internal);
}

CubeTransitionEffect CubeTransitionEffect::DownCast( BaseHandle handle )
{
  return Control::DownCast<CubeTransitionEffect, Internal::CubeTransitionEffect>( handle );
}

void CubeTransitionEffect::SetTransitionDuration( float duration )
{
  GetImpl(*this).SetTransitionDuration( duration );
}

float CubeTransitionEffect::GetTransitionDuration() const
{
  return GetImpl(*this).GetTransitionDuration();
}

void CubeTransitionEffect::SetCubeDisplacement( float displacement )
{
  GetImpl(*this).SetCubeDisplacement( displacement );
}

float CubeTransitionEffect::GetCubeDisplacement() const
{
  return GetImpl(*this).GetCubeDisplacement();
}

bool CubeTransitionEffect::IsTransitioning()
{
  return GetImpl(*this).IsTransitioning();
}

void CubeTransitionEffect::SetCurrentImage( Image image )
{
  GetImpl(*this).SetCurrentImage( image );
}

void CubeTransitionEffect::SetTargetImage( Image image )
{
  GetImpl(*this).SetTargetImage( image );
}

void CubeTransitionEffect::StartTransition( bool toNextImage )
{
  GetImpl(*this).StartTransition( toNextImage );
}

void CubeTransitionEffect::StartTransition( Vector2 panPosition, Vector2 panDisplacement )
{
  GetImpl(*this).StartTransition( panPosition, panDisplacement );
}

void CubeTransitionEffect::PauseTransition()
{
  GetImpl(*this).PauseTransition();
}

void CubeTransitionEffect::ResumeTransition()
{
  GetImpl(*this).ResumeTransition();
}

void CubeTransitionEffect::StopTransition()
{
  GetImpl(*this).StopTransition();
}

CubeTransitionEffect::TransitionCompletedSignalType& CubeTransitionEffect::TransitionCompletedSignal()
{
  return GetImpl( *this ).TransitionCompletedSignal();
}

} // namespace Toolkit

} // namespace Dali
