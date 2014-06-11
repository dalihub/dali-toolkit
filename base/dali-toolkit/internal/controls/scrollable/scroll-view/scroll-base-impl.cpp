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

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-base-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// ScrollBase
///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string ScrollBase::SCROLL_DOMAIN_OFFSET_PROPERTY_NAME( "domain-offset" );

ScrollBase::ScrollBase()
: Scrollable(),
  mParent(NULL),
  mPropertyTime(Property::INVALID_INDEX),
  mPropertyX(Property::INVALID_INDEX),
  mPropertyY(Property::INVALID_INDEX),
  mPropertyPrePosition(Property::INVALID_INDEX),
  mPropertyPosition(Property::INVALID_INDEX),
  mPropertyScale(Property::INVALID_INDEX),
  mPropertyOvershootX(Property::INVALID_INDEX),
  mPropertyOvershootY(Property::INVALID_INDEX),
  mPropertyWrap(Property::INVALID_INDEX),
  mPropertyPanning(Property::INVALID_INDEX),
  mPropertyScrolling(Property::INVALID_INDEX),
  mPropertyFinal(Property::INVALID_INDEX),
  mPropertyDomainOffset(Property::INVALID_INDEX),
  mPropertyPositionDelta(Property::INVALID_INDEX),
  mPropertyScrollStartPagePosition(Property::INVALID_INDEX),
  mDelay(0.0f)
{
}

void ScrollBase::SetParent(ScrollBase *parent)
{
  mParent = parent;
}

void ScrollBase::RegisterProperties()
{
  Actor self = Self();

  // Register common properties
  RegisterCommonProperties();

  // Register Scroll Properties.
  mPropertyTime = self.RegisterProperty(Toolkit::ScrollView::SCROLL_TIME_PROPERTY_NAME, 0.0f);
  mPropertyPrePosition = self.RegisterProperty(Toolkit::ScrollView::SCROLL_PRE_POSITION_PROPERTY_NAME, Vector3::ZERO);
  mPropertyPosition = self.RegisterProperty(Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME, Vector3::ZERO);
  mPropertyOvershootX = self.RegisterProperty(Toolkit::ScrollView::SCROLL_OVERSHOOT_X_PROPERTY_NAME, 0.0f);
  mPropertyOvershootY = self.RegisterProperty(Toolkit::ScrollView::SCROLL_OVERSHOOT_Y_PROPERTY_NAME, 0.0f);
  mPropertyFinal = self.RegisterProperty(Toolkit::ScrollView::SCROLL_FINAL_PROPERTY_NAME, Vector3::ZERO);
  mPropertyX = self.RegisterProperty(Toolkit::ScrollView::SCROLL_X_PROPERTY_NAME, 0.0f);
  mPropertyY = self.RegisterProperty(Toolkit::ScrollView::SCROLL_Y_PROPERTY_NAME, 0.0f);
  mPropertyScale = self.RegisterProperty(Toolkit::ScrollView::SCROLL_SCALE_PROPERTY_NAME, Vector3::ONE);
  mPropertyWrap = self.RegisterProperty(Toolkit::ScrollView::SCROLL_WRAP_PROPERTY_NAME, false);
  mPropertyPanning = self.RegisterProperty(Toolkit::ScrollView::SCROLL_PANNING_PROPERTY_NAME, false);
  mPropertyScrolling = self.RegisterProperty(Toolkit::ScrollView::SCROLL_SCROLLING_PROPERTY_NAME, false);
  mPropertyDomainOffset = self.RegisterProperty(SCROLL_DOMAIN_OFFSET_PROPERTY_NAME, Vector3::ZERO);
  mPropertyPositionDelta = self.RegisterProperty(Toolkit::ScrollView::SCROLL_POSITION_DELTA_PROPERTY_NAME, Vector3::ZERO);
  mPropertyScrollStartPagePosition = self.RegisterProperty(Toolkit::ScrollView::SCROLL_START_PAGE_POSITION_PROPERTY_NAME, Vector3::ZERO);
}

void ScrollBase::BindActor(Actor child)
{
  FindAndUnbindActor(child);

  ActorInfoPtr actorInfo(new ActorInfo(child));
  mBoundActors.push_back(actorInfo);

  // Apply all our constraints to this new child.
  ConstraintStack::iterator i;

  for(i = mConstraintStack.begin();i!=mConstraintStack.end();i++)
  {
    actorInfo->ApplyConstraint(*i);
  }
}

void ScrollBase::UnbindActor(Actor child)
{
  // Find the child in mBoundActors, and unparent it
  for (ActorInfoIter iter = mBoundActors.begin(); iter != mBoundActors.end(); ++iter)
  {
    ActorInfoPtr actorInfo = *iter;

    if( actorInfo->mActor == child )
    {
      mBoundActors.erase(iter);
      break;
    }
  }
}

void ScrollBase::FindAndUnbindActor(Actor child)
{
  // Since we don't know if and where child may have been bound
  // (as we cannot store such information inside the Actor), we
  // perform a search on all associated ScrollBases
  // This is done by recursively calling the parent of this ScrollBase
  // until reaching the top (at which point implementation may be
  // different as this is virtual)

  if(mParent) // continuously ascend until reaches root ScrollBase.
  {
    mParent->FindAndUnbindActor(child);
  }
}

void ScrollBase::ApplyConstraintToBoundActors(Constraint constraint)
{
  mConstraintStack.push_back(constraint);

  for(ActorInfoIter i = mBoundActors.begin();i != mBoundActors.end(); ++i)
  {
    (*i)->ApplyConstraint(constraint);
  }
}

void ScrollBase::RemoveConstraintsFromBoundActors()
{
  mConstraintStack.clear();

  for(ActorInfoIter i = mBoundActors.begin();i != mBoundActors.end(); ++i)
  {
    (*i)->RemoveConstraints();
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
