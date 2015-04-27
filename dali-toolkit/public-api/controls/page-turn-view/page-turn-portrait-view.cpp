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
#include <dali-toolkit/public-api/controls/page-turn-view/page-turn-portrait-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-portrait-view-impl.h>

namespace Dali
{

namespace Toolkit
{

PageTurnPortraitView::PageTurnPortraitView()
: PageTurnView()
{
}

PageTurnPortraitView::PageTurnPortraitView( const PageTurnPortraitView& pageTurnPortraitView )
: PageTurnView( pageTurnPortraitView )
{
}

PageTurnPortraitView::PageTurnPortraitView( Internal::PageTurnPortraitView& implementation )
: PageTurnView( implementation )
{
}

PageTurnPortraitView::PageTurnPortraitView( Dali::Internal::CustomActor* internal )
: PageTurnView( internal )
{
  VerifyCustomActorPointer<Internal::PageTurnPortraitView>( internal );
}

PageTurnPortraitView& PageTurnPortraitView::operator=( const PageTurnPortraitView& pageTurnPortraitView )
{
  if( &pageTurnPortraitView != this)
  {
    PageTurnView::operator=( pageTurnPortraitView );
  }
  return *this;
}

PageTurnPortraitView::~PageTurnPortraitView()
{
}

PageTurnPortraitView PageTurnPortraitView::New( PageFactory& pageFactory, const Vector2& pageSize)
{
  return Internal::PageTurnPortraitView::New(pageFactory, pageSize);
}

PageTurnPortraitView PageTurnPortraitView::DownCast( BaseHandle handle )
{
  return Control::DownCast<PageTurnPortraitView, Internal::PageTurnPortraitView>( handle );
}

} // namespace Toolkit

} // namespace Dali
