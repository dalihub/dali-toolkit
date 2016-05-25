/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "check-box-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/images/resource-image.h>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>

#if defined(DEBUG_ENABLED)
  extern Debug::Filter* gLogButtonFilter;
#endif

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float ANIMATION_TIME( 0.26f );  // EFL checkbox tick time - Will be replaced by stylable tranisitions

BaseHandle Create()
{
  return Toolkit::CheckBoxButton::New();
}

TypeRegistration mType( typeid(Toolkit::CheckBoxButton), typeid(Toolkit::Button), Create );



}

Dali::Toolkit::CheckBoxButton CheckBoxButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< CheckBoxButton > internalCheckBoxButton = new CheckBoxButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::CheckBoxButton checkBoxButton( *internalCheckBoxButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalCheckBoxButton->Initialize();

  return checkBoxButton;
}

CheckBoxButton::CheckBoxButton()
: Button()
{
  SetTogglableButton( true );

  SetAnimationTime( ANIMATION_TIME );
}

CheckBoxButton::~CheckBoxButton()
{
}

void CheckBoxButton::FadeImageTo( Actor actor , float opacity )
{
  if( actor )
  {
    Dali::Animation transitionAnimation = GetTransitionAnimation();

    if( transitionAnimation )
    {
      transitionAnimation.AnimateTo( Property( actor, Actor::Property::COLOR_ALPHA ), opacity );
    }
  }
}

void CheckBoxButton::OnInitialize()
{
  Button::OnInitialize();
}

void CheckBoxButton::PrepareForTransitionIn( Actor actor )
{
  // Set Toolkit::Button::Property::SELECTED_VISUAL and Toolkit::Button::Property::UNSELECTED_VISUAL to opacity 0
  // Then get and start animation
}

void CheckBoxButton::PrepareForTransitionOut( Actor actor )
{
  // Set Toolkit::Button::Property::SELECTED_VISUAL and Toolkit::Button::Property::UNSELECTED_VISUAL to opacity 1
  // Then get and start animation
}

void CheckBoxButton::OnTransitionIn( Actor actor )
{
  // Only transition selected and unselected visual, background doesn't change.
  // Start Fade animation to 1
}

void CheckBoxButton::OnTransitionOut( Actor actor )
{
  // Only transition selected and unselected visual, background doesn't change.
  // Start Fade animation to 0
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
