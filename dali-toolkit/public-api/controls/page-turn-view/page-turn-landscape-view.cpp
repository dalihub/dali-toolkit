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
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-landscape-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-landscape-view-impl.h>

namespace Dali
{

namespace Toolkit
{

PageTurnLandscapeView::PageTurnLandscapeView()
: PageTurnView()
{
}

PageTurnLandscapeView::PageTurnLandscapeView( const PageTurnLandscapeView& pageTurnLandscapeView )
: PageTurnView( pageTurnLandscapeView )
{
}

PageTurnLandscapeView::PageTurnLandscapeView( Internal::PageTurnLandscapeView& implementation )
: PageTurnView( implementation )
{
}

PageTurnLandscapeView::PageTurnLandscapeView( Dali::Internal::CustomActor* internal )
: PageTurnView( internal )
{
  VerifyCustomActorPointer<Internal::PageTurnLandscapeView>( internal );
}

PageTurnLandscapeView& PageTurnLandscapeView::operator=( const PageTurnLandscapeView& pageTurnLandscapeView )
{
  if( &pageTurnLandscapeView != this)
  {
    PageTurnView::operator=( pageTurnLandscapeView );
  }
  return *this;
}

PageTurnLandscapeView::~PageTurnLandscapeView()
{
}

PageTurnLandscapeView PageTurnLandscapeView::New( PageFactory& pageFactory, const Vector2& pageSize )
{
  return Internal::PageTurnLandscapeView::New(pageFactory, pageSize);
}

PageTurnLandscapeView PageTurnLandscapeView::DownCast( BaseHandle handle )
{
  return Control::DownCast<PageTurnLandscapeView, Internal::PageTurnLandscapeView>( handle );
}

} // namespace Toolkit

} // namespace Dali
