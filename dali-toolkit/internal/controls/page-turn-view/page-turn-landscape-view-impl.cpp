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
#include <dali-toolkit/internal/controls/page-turn-view/page-turn-landscape-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::PageTurnLandscapeView, Toolkit::PageTurnView, NULL)
DALI_TYPE_REGISTRATION_END()

} // namespace

PageTurnLandscapeView::PageTurnLandscapeView(PageFactory& pageFactory, const Vector2& viewPageSize)
: PageTurnView(pageFactory, viewPageSize)
{
}

PageTurnLandscapeView::~PageTurnLandscapeView()
{
}

Toolkit::PageTurnLandscapeView PageTurnLandscapeView::New(PageFactory& pageFactory, const Vector2& viewPageSize)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<PageTurnLandscapeView> internalPageTurnView = new PageTurnLandscapeView(pageFactory, viewPageSize);

  // Pass ownership to CustomActor
  Dali::Toolkit::PageTurnLandscapeView pageTurnView(*internalPageTurnView);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPageTurnView->Initialize();

  return pageTurnView;
}

void PageTurnLandscapeView::OnPageTurnViewInitialize()
{
  mTurnEffectShader.RegisterProperty(PROPERTY_TEXTURE_WIDTH, 2.f);
  mSpineEffectShader.RegisterProperty(PROPERTY_TEXTURE_WIDTH, 2.f);

  mControlSize = Vector2(mPageSize.width * 2.f, mPageSize.height);
  Self().SetProperty(Actor::Property::SIZE, mControlSize);
  mTurningPageLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
}

void PageTurnLandscapeView::OnAddPage(Actor newPage, bool isLeftSide)
{
  newPage.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
}

Vector2 PageTurnLandscapeView::SetPanPosition(const Vector2& gesturePosition)
{
  if(mPages[mIndex].isTurnBack)
  {
    return Vector2(mPageSize.width - gesturePosition.x, gesturePosition.y);
  }
  else
  {
    return Vector2(gesturePosition.x - mPageSize.width, gesturePosition.y);
  }
}

void PageTurnLandscapeView::SetPanActor(const Vector2& panPosition)
{
  if(panPosition.x > mPageSize.width && mCurrentPageIndex < mTotalPageCount)
  {
    mTurningPageIndex = mCurrentPageIndex;
  }
  else if(panPosition.x <= mPageSize.width && mCurrentPageIndex > 0)
  {
    mTurningPageIndex = mCurrentPageIndex - 1;
  }
  else
  {
    mTurningPageIndex = -1;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
