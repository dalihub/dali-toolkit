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

#include <dali-toolkit/public-api/controls/selectors/rotating-selector.h>
#include <dali-toolkit/internal/controls/selectors/rotating-selector-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const RotatingSelector::SIGNAL_CHECKED = "checked";

RotatingSelector::RotatingSelector()
{
}

RotatingSelector::RotatingSelector( const RotatingSelector& rotatingSelector )
: Control( rotatingSelector )
{
}

RotatingSelector& RotatingSelector::operator=( const RotatingSelector& rotatingSelector )
{
  if( &rotatingSelector != this )
  {
    Control::operator=( rotatingSelector );
  }

  return *this;
}


RotatingSelector::~RotatingSelector()
{
}

RotatingSelector RotatingSelector::New(Actor& unSelectedActor, Actor& selectedActor)
{
  return Internal::RotatingSelector::New(unSelectedActor, selectedActor);
}

RotatingSelector RotatingSelector::DownCast( BaseHandle handle )
{
  return Control::DownCast<RotatingSelector, Internal::RotatingSelector>(handle);
}

void RotatingSelector::SetSelected( bool checked )
{
  Dali::Toolkit::GetImpl( *this ).SetSelected( checked );
}

bool RotatingSelector::IsSelected() const
{
  return Dali::Toolkit::GetImpl( *this ).IsSelected();
}

void RotatingSelector::SetSelectedActor( Actor& selectedActor )
{
  Dali::Toolkit::GetImpl( *this ).SetSelectedActor( selectedActor );
}

Actor RotatingSelector::GetSelectedActor()
{
  return Dali::Toolkit::GetImpl( *this ).GetSelectedActor();
}

void RotatingSelector::SetUnSelectedActor( Actor& unSelectedActor )
{
  Dali::Toolkit::GetImpl( *this ).SetUnSelectedActor( unSelectedActor );
}

Actor RotatingSelector::GetUnSelectedActor()
{
  return Dali::Toolkit::GetImpl( *this ).GetUnSelectedActor();
}

void RotatingSelector::SetSelectable( bool selectable )
{
  Dali::Toolkit::GetImpl( *this ).SetSelectable( selectable );
}

bool RotatingSelector::IsSelectable()const
{
  return Dali::Toolkit::GetImpl( *this ).IsSelectable();
}

RotatingSelector::SelectedSignalV2& RotatingSelector::SelectedSignal()
{
  return Dali::Toolkit::GetImpl( *this ).SelectedSignal();
}

RotatingSelector::RotatingSelector( Internal::RotatingSelector& implementation )
: Control(implementation)
{
}

RotatingSelector::RotatingSelector( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::RotatingSelector>(internal);
}

} // namespace Toolkit

} // namespace Dali
