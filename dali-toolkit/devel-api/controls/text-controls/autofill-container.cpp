/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/text-controls/autofill-container.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/autofill-container-impl.h>

namespace Dali
{

namespace Toolkit
{

AutofillContainer AutofillContainer::New( const std::string& name )
{
  return Internal::AutofillContainer::New( name );
}

AutofillContainer::AutofillContainer()
{
}

AutofillContainer::AutofillContainer( const AutofillContainer& handle )
: BaseHandle(handle)
{
}

AutofillContainer& AutofillContainer::operator=( const AutofillContainer& handle )
{
  BaseHandle::operator=(handle);
  return *this;
}

AutofillContainer::~AutofillContainer()
{
}

AutofillContainer AutofillContainer::DownCast( BaseHandle handle )
{
  return AutofillContainer( dynamic_cast<Internal::AutofillContainer*>( handle.GetObjectPtr() ) );
}


void AutofillContainer::AddAutofillItem( Control control, Property::Index propertyIndex, const std::string& id, const std::string& label, Dali::AutofillItem::Hint hint, bool isSensitive )
{
  GetImpl(*this).AddAutofillItem( control, propertyIndex, id, label, hint, isSensitive );
}

void AutofillContainer::RemoveAutofillItem( Control control )
{
  GetImpl(*this).RemoveAutofillItem( control );
}

void AutofillContainer::SetFocusedControl( Toolkit::Control focused )
{
  GetImpl(*this).SetFocusedControl( focused );
}

Toolkit::Control AutofillContainer::GetFocusedControl()
{
  return GetImpl(*this).GetFocusedControl();
}

void AutofillContainer::SaveAutofillData()
{
  GetImpl(*this).SaveAutofillData();
}

void AutofillContainer::RequestFillData()
{
  GetImpl(*this).RequestFillData();
}

const std::string& AutofillContainer::GetAutofillServiceName() const
{
  return GetImpl(*this).GetAutofillServiceName();
}

const std::string& AutofillContainer::GetAutofillServiceMessage() const
{
  return GetImpl(*this).GetAutofillServiceMessage();
}

const std::string& AutofillContainer::GetAutofillServiceImagePath() const
{
  return GetImpl(*this).GetAutofillServiceImagePath();
}

unsigned int AutofillContainer::GetListCount()
{
  return GetImpl(*this).GetListCount();
}

const std::string& AutofillContainer::GetListItem( unsigned int index ) const
{
  return GetImpl(*this).GetListItem( index );
}

void AutofillContainer::SetSelectedItem( const std::string& selected )
{
  GetImpl(*this).SetSelectedItem( selected );
}

AutofillContainer::AutofillContainer( Internal::AutofillContainer* implementation )
: BaseHandle(implementation)
{
}

AutofillContainer::AuthenticationSignalType& AutofillContainer::AutofillServiceShownSignal()
{
  return GetImpl(*this).AutofillServiceShownSignal();
}

AutofillContainer::ListShownSignalType& AutofillContainer::AutofillListShownSignal()
{
  return GetImpl(*this).AutofillListShownSignal();
}

} // namespace Toolkit

} // namespace Dali
