#ifndef DALI_TOOLKIT_INTERNAL_TRANSITION_BASE_H
#define DALI_TOOLKIT_INTERNAL_TRANSITION_BASE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/transition/transition-base.h>
#include <dali-toolkit/public-api/transition/transition-set.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using TransitionBasePtr = IntrusivePtr<TransitionBase>;

class TransitionBase : public BaseObject
{
public:
  /**
   * @brief Create a new TransitionBase object.
   * @return A smart-pointer to the newly allocated TransitionBase.
   */
  static TransitionBasePtr New();

  /**
   * @copydoc Dali::Toolkit::TransitionBase::SetTimePeriod()
   */
  void SetTimePeriod(const Dali::TimePeriod& timePeriod);

  /**
   * @copydoc Dali::Toolkit::TransitionBase::GetTimePeriod()
   */
  Dali::TimePeriod GetTimePeriod() const;

  /**
   * @copydoc Dali::Toolkit::TransitionBase::SetAlphaFunction()
   */
  void SetAlphaFunction(AlphaFunction alphaFunction)
  {
    mAlphaFunction = alphaFunction;
  }

  /**
   * @copydoc Dali::Toolkit::TransitionBase::GetAlphaFunction()
   */
  AlphaFunction GetAlphaFunction() const
  {
    return mAlphaFunction;
  }

  /**
   * @copydoc Dali::Toolkit::TransitionBase::TransitionWithChild()
   */
  void TransitionWithChild(bool transitionWithChild);

  /**
   * @brief Run processes those are required done before size/position negotiation.
   * @param[in] animation animation for the transition
   */
  void PreProcess(Dali::Animation animation);

  /**
   * @brief Make property animation for Transition
   */
  void Play();

  /**
   * Emit the Finished signal
   */
  void TransitionFinished();

  /**
   * @brief Set this transition is appearing transition or not.
   * @param[in] appearingTransition True if this transition is appearing transition.
   */
  void SetAppearingTransition(bool appearingTransition)
  {
    mIsAppearingTransition = appearingTransition;
  }

  /**
   * @brief Returns whether this transition is appearing transition or not
   */
  bool IsAppearingTransition() const
  {
    return mIsAppearingTransition;
  }

  /**
   * @brief Returns whether this transition is a transition from a Control to another Control or effect to appearing or disappearing.
   */
  bool IsPairTransition() const
  {
    return mIsPairTransition;
  }

  /**
   * @brief Returns target which will be transition.
   */
  const Dali::Toolkit::Control GetTarget() const
  {
    return mTarget;
  }

protected:
  /**
   * @brief Set property map which will be used as a animation start properties.
   * @param[in] propertyMap propertyMap that will be used as a start value of transition.
   */
  void SetStartPropertyMap(const Property::Map& propertyMap)
  {
    mStartPropertyMap = propertyMap;
  }

  /**
   * @brief Set property map which will be used as a animation finish properties.
   * @param[in] propertyMap propertyMap that will be used as a finish value of transition.
   */
  void SetFinishPropertyMap(const Property::Map& propertyMap)
  {
    mFinishPropertyMap = propertyMap;
  }

  /**
   * @brief Retrieve animation.
   */
  Dali::Animation GetAnimation()
  {
    return mAnimation;
  }

  /**
   * @brief Set target which will be transition.
   * @param[in] target control that will be transition.
   */
  void SetTarget(Dali::Toolkit::Control target)
  {
    mTarget = target;
  }

  /**
   * @brief Returns whether this transition will be applied to children of target or not.
   */
  bool IsTransitionWithChild() const
  {
    return mTransitionWithChild;
  }

  /**
   * @brief Set whether this transition is a transition from a Control to another Control or effect to appearing or disappearing.
   * @param[in] pairTransition True if this transition is appearing transition.
   */
  void SetPairTransition(bool pairTransition)
  {
    mIsPairTransition = pairTransition;
  }

protected:
  /**
   * Construct a new TransitionBase.
   */
  TransitionBase();

  /**
   * Second-phase constructor.
   */
  void Initialize();

  /**
   * Destructor
   */
  ~TransitionBase() = default;

private:
  // Undefined
  TransitionBase(const TransitionBase&);

  // Undefined
  TransitionBase& operator=(const TransitionBase& rhs);

private:
  /**
   * @brief Makes property animation for transition.
   */
  void SetAnimation();

  /**
   * @brief Adds a property on an animation between sourceValue and destimationValue.
   * @param[in] target target control to be animated.
   * @param[in] index property index for animation.
   * @param[in] sourceValue source value of animation.
   * @param[in] destinationValue destination value of animation.
   */
  void AnimateBetween(Dali::Toolkit::Control target, Property::Index index, Property::Value sourceValue, Property::Value destinationValue);

  /**
   * @brief Copy target to make clone for the child Actors
   */
  void CopyTarget();

  /**
   * @brief Make pair of Property::Map to be used for transition animation.
   * Set the pair of Property::Map by using SetStartPropertyMap() and SetFinishPropertyMap(),
   * then the properties of mTarget will be animated between them during transition duration.
   * If the transition requires the information of world transform, let them be in this method.
   * World transform and World color can be retrieved by using GetWorldTransform() and GetWorldColor() methods.
   * And If it is needed to add additional custom animation than use GetAnimation() and add them.
   *
   * @note Do not set any properties in this methods.
   */
  virtual void OnPlay()
  {
  }

  /**
   * @brief If the transition is needed to do something after the transition is finished, let them be in this method.
   */
  virtual void OnFinished()
  {
  }

private:
  Dali::Toolkit::Control mTarget;              ///< Target that will be animated.
  Dali::Actor            mCopiedActor;         ///< Copied View that will replace mTarget during transition
  Dali::Animation        mAnimation;           ///< Property animations for the transition of mTarget
  AlphaFunction          mAlphaFunction;       ///< Alpha function that will applied for the property animation
  Property::Map          mStartPropertyMap;    ///< Start properties to be animated. (world transform)
  Property::Map          mFinishPropertyMap;   ///< Finish properties to be animated. (world transform)
  Property::Map          mOriginalPropertyMap; ///< Original properties of mTarget to be used to restore after the transition is finished.
  Dali::TimePeriod       mTimePeriod;          ///< TimePeriod of transition
  bool                   mTransitionWithChild; ///< True, if mTarget transition is inherit to its child Actors.
                                               ///< If this is false, the child Actors are moved to the child of mCopiedActor that will have original properties of target Actor during Transition.
  bool mMoveTargetChildren;                    ///< Flag, if mTransitionWithChild is false and mTarget has children than True.
  bool mIsAppearingTransition;                 ///< True, if this transition is appearing transition.
  bool mIsPairTransition;                      ///< True, if this transition is started from a Control to another Control.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TransitionBase& GetImplementation(Dali::Toolkit::TransitionBase& animation)
{
  DALI_ASSERT_ALWAYS(animation && "TransitionBase handle is empty");

  BaseObject& handle = animation.GetBaseObject();

  return static_cast<Internal::TransitionBase&>(handle);
}

inline const Internal::TransitionBase& GetImplementation(const Dali::Toolkit::TransitionBase& animation)
{
  DALI_ASSERT_ALWAYS(animation && "TransitionBase handle is empty");

  const BaseObject& handle = animation.GetBaseObject();

  return static_cast<const Internal::TransitionBase&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TRANSITION_BASE_H
