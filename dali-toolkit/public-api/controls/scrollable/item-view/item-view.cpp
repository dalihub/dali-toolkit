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

#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/item-view-impl.h>

namespace Dali
{

namespace Toolkit
{

ItemView::ItemView()
{
}

ItemView::ItemView(Internal::ItemView& implementation)
: Scrollable(implementation)
{
}

ItemView::ItemView( Dali::Internal::CustomActor* internal )
: Scrollable(internal)
{
  VerifyCustomActorPointer<Internal::ItemView>(internal);
}

ItemView::ItemView( const ItemView& itemView )
: Scrollable(itemView)
{
}

ItemView& ItemView::operator=( const ItemView& itemView )
{
  if( &itemView != this )
  {
    Control::operator=( itemView );
  }
  return *this;
}

ItemView ItemView::New(ItemFactory& factory)
{
  return Internal::ItemView::New(factory);
}

ItemView ItemView::DownCast( BaseHandle handle )
{
  return Control::DownCast<ItemView, Internal::ItemView>(handle);
}

ItemView::~ItemView()
{
}

unsigned int ItemView::GetLayoutCount() const
{
  return GetImpl(*this).GetLayoutCount();
}

void ItemView::AddLayout(ItemLayout& layout)
{
  GetImpl(*this).AddLayout(layout);
}

void ItemView::RemoveLayout(unsigned int layoutIndex)
{
  GetImpl(*this).RemoveLayout(layoutIndex);
}

ItemLayoutPtr ItemView::GetLayout(unsigned int layoutIndex) const
{
  return GetImpl(*this).GetLayout(layoutIndex);
}

ItemLayoutPtr ItemView::GetActiveLayout() const
{
  return GetImpl(*this).GetActiveLayout();
}

float ItemView::GetCurrentLayoutPosition(unsigned int itemId) const
{
  return GetImpl(*this).GetCurrentLayoutPosition(itemId);
}

void ItemView::ActivateLayout(unsigned int layoutIndex, Vector3 targetSize, float durationSeconds)
{
  GetImpl(*this).ActivateLayout(layoutIndex, targetSize, durationSeconds);
}

void ItemView::DeactivateCurrentLayout()
{
  GetImpl(*this).DeactivateCurrentLayout();
}

void ItemView::SetMinimumSwipeSpeed(float speed)
{
  GetImpl(*this).SetMinimumSwipeSpeed(speed);
}

float ItemView::GetMinimumSwipeSpeed() const
{
  return GetImpl(*this).GetMinimumSwipeSpeed();
}

void ItemView::SetMinimumSwipeDistance(float distance)
{
  GetImpl(*this).SetMinimumSwipeDistance(distance);
}

float ItemView::GetMinimumSwipeDistance() const
{
  return GetImpl(*this).GetMinimumSwipeDistance();
}

void ItemView::SetMouseWheelScrollDistanceStep(float step)
{
  GetImpl(*this).SetMouseWheelScrollDistanceStep(step);
}

float ItemView::GetMouseWheelScrollDistanceStep() const
{
  return GetImpl(*this).GetMouseWheelScrollDistanceStep();
}

void ItemView::SetAnchoring(bool enabled)
{
  GetImpl(*this).SetAnchoring(enabled);
}

bool ItemView::GetAnchoring() const
{
  return GetImpl(*this).GetAnchoring();
}

void ItemView::SetAnchoringDuration(float durationSeconds)
{
  GetImpl(*this).SetAnchoringDuration(durationSeconds);
}

float ItemView::GetAnchoringDuration() const
{
  return GetImpl(*this).GetAnchoringDuration();
}

void ItemView::ScrollToItem(unsigned int itemId, float durationSeconds)
{
  GetImpl(*this).ScrollToItem(itemId, durationSeconds);
}

void ItemView::SetRefreshInterval(unsigned int intervalMilliseconds)
{
  GetImpl(*this).SetRefreshInterval(intervalMilliseconds);
}

unsigned int ItemView::GetRefreshInterval() const
{
  return GetImpl(*this).GetRefreshInterval();
}

Actor ItemView::GetItem(unsigned int itemId) const
{
  return GetImpl(*this).GetItem(itemId);
}

unsigned int ItemView::GetItemId(Actor actor) const
{
  return GetImpl(*this).GetItemId(actor);
}

void ItemView::RemoveItem(unsigned int itemId, float durationSeconds)
{
  GetImpl(*this).RemoveItem(itemId, durationSeconds);
}

} // namespace Toolkit

} // namespace Dali
