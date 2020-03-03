/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "control-devel.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelControl
{
void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, depthIndex);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled, depthIndex);
}

void UnregisterVisual(Internal::Control& control, Dali::Property::Index index)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.UnregisterVisual(index);
}

Toolkit::Visual::Base GetVisual(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.GetVisual(index);
}

void EnableVisual(Internal::Control& control, Dali::Property::Index index, bool enable)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.EnableVisual(index, enable);
}

bool IsVisualEnabled(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.IsVisualEnabled(index);
}

Dali::Animation CreateTransition(Internal::Control& control, const Toolkit::TransitionData& handle)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.CreateTransition(handle);
}

void AddTransitions(Internal::Control&             control,
                    Dali::Animation                animation,
                    const Toolkit::TransitionData& transitionData)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.AddTransitions(animation, transitionData);
}

void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes)
{
  Internal::Control&       controlInternal = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoAction(visualIndex, actionId, attributes);
}

void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext)
{
  Internal::Control::Impl::Get(control).SetInputMethodContext(inputMethodContext);
}

VisualEventSignalType& VisualEventSignal(Control control)
{
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.VisualEventSignal();
}

static Toolkit::Internal::Control::Impl *GetControlImplementationIfAny( Dali::Actor actor)
{
  Dali::Toolkit::Control c = Toolkit::Control::DownCast( actor );
  if ( c )
  {
    auto &impl1 = Toolkit::Internal::GetImplementation( c );
    auto &impl2 = Toolkit::Internal::Control::Impl::Get( impl1 );
    return &impl2;
  }
  return nullptr;
}

Toolkit::DevelControl::AccessibilityActivateSignalType &AccessibilityActivateSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityActivateSignal;
}

Toolkit::DevelControl::AccessibilityReadingSkippedSignalType &AccessibilityReadingSkippedSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityReadingSkippedSignal;
}

Toolkit::DevelControl::AccessibilityReadingCancelledSignalType &AccessibilityReadingCancelledSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityReadingCancelledSignal;
}

Toolkit::DevelControl::AccessibilityReadingStoppedSignalType &AccessibilityReadingStoppedSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityReadingStoppedSignal;
}

Toolkit::DevelControl::AccessibilityGetNameSignalType &AccessibilityGetNameSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityGetNameSignal;
}

Toolkit::DevelControl::AccessibilityGetDescriptionSignalType &AccessibilityGetDescriptionSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityGetDescriptionSignal;
}

Toolkit::DevelControl::AccessibilityDoGestureSignalType &AccessibilityDoGestureSignal( Toolkit::Control control )
{
  auto ac = GetControlImplementationIfAny ( control );
  return ac->mAccessibilityDoGestureSignal;
}

void AppendAccessibilityRelation( Dali::Actor control, Actor destination, Dali::Accessibility::RelationType relation)
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    auto index = static_cast<Dali::Property::Array::SizeType>(relation);
    if (index >= ac->mAccessibilityRelations.size())
    {
        DALI_LOG_ERROR( "Relation index exceeds vector size." );
        return;
    }

    auto obj = ac->GetAccessibilityObject(destination);
    if (obj)
      ac->mAccessibilityRelations[index].push_back(obj->GetAddress());
  }
}

void RemoveAccessibilityRelation( Dali::Actor control, Actor destination, Dali::Accessibility::RelationType relation)
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    auto index = static_cast<Dali::Property::Array::SizeType>(relation);
    if (index >= ac->mAccessibilityRelations.size())
    {
        DALI_LOG_ERROR( "Relation index exceeds vector size." );
        return;
    }

    auto obj = ac->GetAccessibilityObject(destination);
    if (!obj)
      return;

    auto address = obj->GetAddress();

    auto &targets = ac->mAccessibilityRelations[index];
    for (auto i = 0u; i < targets.size(); ++i) {
        if (targets[i].ToString() == address.ToString())
        {
            targets[i] = targets.back();
            targets.erase(targets.end() - 1);
        }
    }
  }
}

std::vector<std::vector<Accessibility::Address>> GetAccessibilityRelations( Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    return ac->mAccessibilityRelations;
  }
  return {};
}

void ClearAccessibilityRelations( Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    for (auto &it : ac->mAccessibilityRelations)
      it.clear();
  }
}

void SetAccessibilityConstructor( Dali::Actor control,
        std::function< std::unique_ptr< Dali::Accessibility::Accessible >( Dali::Actor ) > constructor )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    ac->accessibilityConstructor = constructor;
  }
}

void AppendAccessibilityAttribute( Dali::Actor control, const std::string& key,
                                         const std::string value )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    ac->AppendAccessibilityAttribute( key, value );
  }
}

void RemoveAccessibilityAttribute( Dali::Actor control, const std::string& key )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    ac->RemoveAccessibilityAttribute( key );
  }
}

void ClearAccessibilityAttributes( Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    ac->ClearAccessibilityAttributes();
  }
}

void SetAccessibilityReadingInfoType( Dali::Actor control, const Dali::Accessibility::ReadingInfoTypes types )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    ac->SetAccessibilityReadingInfoType( types );
  }
}

Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    return ac->GetAccessibilityReadingInfoType();
  }
  return {};
}

bool ClearAccessibilityHighlight( Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    auto ptr = dynamic_cast<Dali::Accessibility::Component*>( ac );
    if( ptr )
      return ptr->ClearHighlight();
  }
  return false;
}

bool GrabAccessibilityHighlight( Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    auto ptr = dynamic_cast<Dali::Accessibility::Component*>( ac );
    if( ptr )
      return ptr->GrabHighlight();
  }
  return false;
}

Dali::Accessibility::States GetAccessibilityStates( Dali::Actor control )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    auto ptr = dynamic_cast<Dali::Accessibility::Component*>( ac );
    if(ptr)
      return ptr->GetStates();
  }
  return {};
}

void NotifyAccessibilityStateChange( Dali::Actor control, Dali::Accessibility::States states, bool doRecursive )
{
  if ( auto ac = GetControlImplementationIfAny ( control ) )
  {
    ac->GetAccessibilityObject()->NotifyAccessibilityStateChange( states, doRecursive );
  }
}

Dali::Accessibility::Accessible *GetBoundAccessibilityObject( Dali::Actor control )
{
  return Dali::Accessibility::Accessible::Get( control );
}

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali
