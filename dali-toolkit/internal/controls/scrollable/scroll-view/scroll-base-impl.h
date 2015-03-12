#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_BASE_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_BASE_H__

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

// EXTERNAL INCLUDES
// TODO - Replace list with dali-vector.h
#include <list>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>

// INTERNAL INCLUDES

#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ScrollBase;

typedef IntrusivePtr<Actor>         ActorPtr;
typedef std::list<Constraint>               ConstraintStack;

/**
 * ScrollBase represents a set of properties (time, position
 * scale etc.) that constrain a set of actors.
 */
class ScrollBase : public Scrollable
{
public:

  struct ActorInfo : public Dali::RefObject
  {
    /**
     * ActorInfo constructor
     * @param[in] actor The actor that this ActorInfo represents.
     */
    ActorInfo(Actor actor)
    : mActor(actor)
    {
    }

    /**
     * ActorInfo destructor
     * removes scrollview-related constraints only.
     */
    ~ActorInfo()
    {
      RemoveConstraints();
    }

    /**
     * Apply a constraint to this actor
     * The constraint will be applied to the actor,
     * and the ActorInfo will keep track of this constraint.
     * @param[in] constraint The constraint to apply to the actor
     */
    void ApplyConstraint(Constraint constraint)
    {
      ActiveConstraint activeConstraint = mActor.ApplyConstraint( constraint );
      mConstraints.push_back( activeConstraint );
    }

    /**
     * Remove constraints from this actor.
     * All of the constraints that have been applied to the
     * actor via this ActorInfo will be removed.
     */
    void RemoveConstraints()
    {
      std::vector<ActiveConstraint>::iterator it = mConstraints.begin();
      std::vector<ActiveConstraint>::iterator end = mConstraints.end();
      for(;it!=end;++it)
      {
        mActor.RemoveConstraint(*it);
      }
      mConstraints.clear();
    }

    Actor mActor;                                     ///< The Actor that this ActorInfo represents.
    std::vector<ActiveConstraint> mConstraints;       ///< A list keeping track of constraints applied to the actor via this delegate.
  };

  typedef IntrusivePtr<ActorInfo> ActorInfoPtr;
  typedef std::vector<ActorInfoPtr> ActorInfoContainer;
  typedef ActorInfoContainer::iterator ActorInfoIter;
  typedef ActorInfoContainer::const_iterator ActorInfoConstIter;

public:

  /**
   * Sets the delay in seconds.
   * This delay affects the animation timing for all
   * Bound Actors.
   *
   * @param[in] delay The delay in seconds.
   */
  void SetDelay(float delay)
  {
    mDelay = delay;
  }

  /**
   * Gets the current delay in seconds.
   *
   * @return The delay in seconds.
   */
  float GetDelay() const
  {
    return mDelay;
  }

public:

  /**
   * Sets ScrollBase Parent
   *
   * @param[in] parent The parent that this ScrollBase belongs to.
   */
  void SetParent(ScrollBase *parent);

  /**
   * Bind Actor to this scroll view/group.
   * Once Bound, this scroll view/group will affect the actor (child)
   *
   * @param[in] child The actor to be bound.
   */
  void BindActor(Actor child);

  /**
   * Unbind Actor from this scroll view/group
   * Once Unbound, this scroll view/group will not affect the actor
   *
   * @note this does not remove the child from the ScrollView container
   *
   * @param[in] child The actor to be unbound
   */
  void UnbindActor(Actor child);

  /**
   * Searches associated ScrollBases for the Actor, and attempts to Unbind
   * systematically this Actor from the ScrollView or Groups attached.
   *
   * @param[in] child The actor to be unbound.
   */
  virtual void FindAndUnbindActor(Actor child);

  /**
   * Applies constraint to the bound actors within this ScrollView/Group only.
   *
   * @param[in] constraint, the constraint to apply to these bound actors and future
   * ones.
   */
  void ApplyConstraintToBoundActors(Constraint constraint);

  /**
   * Removes all constraints from the bound actors within this ScrollView/Group only.
   */
  void RemoveConstraintsFromBoundActors();

protected:

  static const char* const SCROLL_DOMAIN_OFFSET_PROPERTY_NAME;

protected:

  /**
   * Construct a new ScrollBase.
   */
  ScrollBase();

  /**
   * 2nd-phase initialization.
   */
  void RegisterProperties();

protected:

  ScrollBase *mParent;                              ///< Pointer to ScrollBase parent, if exists.
  Property::Index mPropertyTime;                    ///< Scroll Time (0 to animationDuration while animating, otherwise 0)
  Property::Index mPropertyPrePosition;             ///< Scroll Position ("scroll-position") [function of scroll-x, scroll-y]
  Property::Index mPropertyPosition;                ///< Scroll Position ("scroll-position") [function of scroll-pre-position]
  Property::Index mPropertyOvershootX;              ///< Scroll Overshoot ("scroll-overshoot-x") [function of scroll-pre-position, scroll-position]
  Property::Index mPropertyOvershootY;              ///< Scroll Overshoot ("scroll-overshoot-y") [function of scroll-pre-position, scroll-position]
  Property::Index mPropertyWrap;                    ///< Scroll Wrap ("scroll-wrap")
  Property::Index mPropertyPanning;                 ///< Whether we are panning
  Property::Index mPropertyScrolling;               ///< Whether we are scrolling
  Property::Index mPropertyFinal;                   ///< Scroll Final Position ("scroll-final") [scroll-position + f(scroll-overshoot)]
  Property::Index mPropertyDomainOffset;            ///< Scroll Domain Offset ("scroll-domain-offset") keeps track of scroll position as it wraps domains
  Property::Index mPropertyPositionDelta;           ///< Scroll Position Delta ("scroll-position-delta")
  Property::Index mPropertyScrollStartPagePosition; ///< Scroll Start Page Position ("scroll-start-page-position")

private:

  float mDelay;                             ///< delay in seconds.
  ConstraintStack mConstraintStack;         ///< The list of constraints to apply to any actors
  ActorInfoContainer mBoundActors;          ///< The list of actors that have been bound to this ScrollBase.

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_GROUP_H__
