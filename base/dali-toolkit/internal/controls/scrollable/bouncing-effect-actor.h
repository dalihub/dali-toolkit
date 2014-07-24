#ifndef __DALI_TOOLKIT_INTERNAL_BOUNCING_EFFECT_ACTOR_H__
#define __DALI_TOOLKIT_INTERNAL_BOUNCING_EFFECT_ACTOR_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief Creates a Dali::Actor to display the bouncing effect for overshoot
 *
 * Usage example:
 * @code
 *  // create the actor and get the property index for animation
 *  Property::Index bouncePropertyIndex = Property::INVALID_INDEX;
 *  Actor bounceActor = CreateBouncingEffectActor( bouncePropertyIndex );

 *  // set size and color
 *  bounceActor.SetSize(720.f, 42.f );
 *  bounceActor.SetColor( Vector4( 0.0,0.64f,0.85f,0.25f ) );
 *
 *  // add to stage
 *  bounceActor.SetParentOrigin(ParentOrigin::CENTER);
 *  Stage::GetCurrent().Add(bounceActor);

 *  // start the bouncing animation
 *  Animation anim = Animation::New(2.0f);
 *  anim.AnimateTo( Property( bounceActor, bouncePropertyIndex ), 1.f, AlphaFunctions::Sin );
 *  anim.Play();
 * @endcode
 *
 * @param[out] bouncePropertyIndex The property index which controls the bouncing
 * @return The actor which displays the bouncing effect
 */
Actor CreateBouncingEffectActor( Property::Index& bouncePropertyIndex);

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_INTERNAL_BOUNCING_EFFECT_ACTOR_H__ */
