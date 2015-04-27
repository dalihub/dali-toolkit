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

ScrollBase::ScrollBase()
: Scrollable(),
  mParent(NULL),
  mDelay(0.0f)
{
}

ScrollBase::ScrollBase( ControlBehaviour behaviourFlags )
: Scrollable( behaviourFlags ),
  mParent(NULL),
  mDelay(0.0f)
{
}

void ScrollBase::SetParent(ScrollBase *parent)
{
  mParent = parent;
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
