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
#include "cube-transition-cross-effect.h"

//INTERNAL INCLUDES
#include <dali-toolkit/internal/transition-effects/cube-transition-cross-effect-impl.h>

namespace Dali
{

namespace Toolkit
{

CubeTransitionCrossEffect::CubeTransitionCrossEffect( Internal::CubeTransitionCrossEffect& implementation )
: CubeTransitionEffect( implementation )
{
}

CubeTransitionCrossEffect::CubeTransitionCrossEffect( Dali::Internal::CustomActor* internal )
: CubeTransitionEffect( internal )
{
  VerifyCustomActorPointer< Internal::CubeTransitionCrossEffect >( internal );
}

CubeTransitionCrossEffect CubeTransitionCrossEffect::New( unsigned int numRows, unsigned int numColumns )
{
  return Internal::CubeTransitionCrossEffect::New( numRows, numColumns );
}

} // namespace Toolkit

} // namespace Dali
