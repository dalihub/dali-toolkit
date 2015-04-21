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
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-view-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

PageTurnView::PageTurnView()
{
}

PageTurnView::PageTurnView( const PageTurnView& handle )
: Control( handle )
{
}

PageTurnView& PageTurnView::operator=( const PageTurnView& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

PageTurnView::~PageTurnView()
{
}

PageTurnView PageTurnView::DownCast( BaseHandle handle )
{
  return Control::DownCast<PageTurnView, Internal::PageTurnView>(handle);
}

PageTurnView::PageTurnView( Internal::PageTurnView& implementation )
: Control( implementation )
{
}

PageTurnView::PageTurnView( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::PageTurnView>(internal);
}

void PageTurnView::SetSpineShadowParameter( const Vector2& spineShadowParameter )
{
  Toolkit::GetImplementation( *this ).SetSpineShadowParameter( spineShadowParameter );
}

Vector2 PageTurnView::GetSpineShadowParameter()
{
  return Toolkit::GetImplementation( *this ).GetSpineShadowParameter();
}

void PageTurnView::GoToPage( unsigned int pageId )
{
  Toolkit::GetImplementation( *this ).GoToPage(pageId);
}

unsigned int PageTurnView::GetCurrentPage()
{
  return Toolkit::GetImplementation( *this ).GetCurrentPage();
}

PageTurnView::PageTurnSignal& PageTurnView::PageTurnStartedSignal()
{
  return Toolkit::GetImplementation( *this ).PageTurnStartedSignal();
}

PageTurnView::PageTurnSignal& PageTurnView::PageTurnFinishedSignal()
{
  return Toolkit::GetImplementation( *this ).PageTurnFinishedSignal();
}

PageTurnView::PagePanSignal& PageTurnView::PagePanStartedSignal()
{
  return Toolkit::GetImplementation( *this ).PagePanStartedSignal();
}

PageTurnView::PagePanSignal& PageTurnView::PagePanFinishedSignal()
{
  return Toolkit::GetImplementation( *this ).PagePanFinishedSignal();
}

Actor PageTurnView::EnterEditMode()
{
  return Toolkit::GetImplementation( *this ).EnterEditMode();
}

void PageTurnView::LeaveEditMode()
{
  Toolkit::GetImplementation( *this ).LeaveEditMode();
}

Actor PageTurnView::GetHitActor( Vector2& screenCoordinates, Vector2& actorCoordinates )
{
  return Toolkit::GetImplementation( *this ).GetHitActor( screenCoordinates, actorCoordinates );
}

void PageTurnView::RefreshAll()
{
  Toolkit::GetImplementation( *this ).RefreshAll();
}

void PageTurnView::RefreshCurrentPage()
{
  Toolkit::GetImplementation( *this ).RefreshCurrentPage();
}

} // namespace Toolkit

} // namespace Dali
