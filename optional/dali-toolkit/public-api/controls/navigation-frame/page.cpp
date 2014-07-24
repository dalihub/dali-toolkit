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

// CLASS HEADER
#include "page.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/navigation-frame/page-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const Page::PROPERTY_TITLE = "title";
const char* const Page::PROPERTY_SUB_TITLE = "sub-title";

Page::Page()
{
}

Page::Page( const Page& handle )
: Control(handle)
{
}

Page& Page::operator=( const Page& handle )
{
  if( &handle != this )
  {
    CustomActor::operator=( handle );
  }
  return *this;
}

Page Page::New()
{
  return Internal::Page::New();
}

Page Page::DownCast( BaseHandle handle )
{
  return Control::DownCast<Page, Internal::Page>(handle);
}

Page::Page(Internal::Page& impl)
: Control(impl)
{
}

Page::Page( Dali::Internal::CustomActor* internal )
: Control( internal)
{
  VerifyCustomActorPointer<Internal::Page>(internal);
}

void Page::SetTitle(const std::string& title)
{
  GetImpl( *this ).SetTitle(title);
}

const std::string& Page::GetTitle() const
{
  return GetImpl( *this ).GetTitle();
}

void Page::SetSubTitle(const std::string& subtitle)
{
  GetImpl( *this ).SetSubTitle(subtitle);
}

const std::string& Page::GetSubTitle() const
{
  return GetImpl( *this ).GetSubTitle();
}

void Page::SetTitleIcon( Actor titleIcon)
{
  GetImpl( *this ).SetTitleIcon(titleIcon);
}

Actor Page::GetTitleIcon() const
{
  return GetImpl( *this ).GetTitleIcon();
}

bool Page::AddControlToToolBar(Actor control, Alignment::Type alignment)
{
  return GetImpl( *this ).AddControlToToolBar(control, alignment);
}

const Page::ControlOnBarContainer Page::GetControlsOnToolBar() const
{
  return GetImpl( *this ).GetControlsOnToolBar();
}

bool Page::AddControlToTitleBar(Actor control)
{
  return GetImpl( *this ).AddControlToTitleBar(control);
}

const ActorContainer Page::GetControlsOnTitleBar() const
{
  return GetImpl( *this ).GetControlsOnTitleBar();
}

void Page::SetPopupMenu( Toolkit::Popup popupMenu )
{
  GetImpl( *this ).SetPopupMenu( popupMenu );
}

Toolkit::Popup Page::GetPopupMenu() const
{
  return GetImpl( *this ).GetPopupMenu();
}

} // namespace Toolkit

} // namespace Dali
