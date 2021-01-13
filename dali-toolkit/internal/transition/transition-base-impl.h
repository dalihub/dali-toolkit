#ifndef DALI_INTERNAL_TRANSITION_BASE_H
#define DALI_INTERNAL_TRANSITION_BASE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
   * @copydoc Dali::Toolkit::TransitionBase::SetDuration()
   */
  void SetDuration(float seconds);

  /**
   * @copydoc Dali::Toolkit::TransitionBase::GetDuration()
   */
  float GetDuration() const;

  /**
   * @copydoc Dali::Toolkit::TransitionBase::SetDelay()
   */
  void SetDelay(float seconds);

  /**
   * @copydoc Dali::Toolkit::TransitionBase::GetDelay()
   */
  float GetDelay() const;

  /**
   * @copydoc Dali::Toolkit::TransitionBase::SetTimePeriod()
   */
  void SetTimePeriod(const Dali::TimePeriod& timePeriod);

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
   * @brief Request to play this transition.
   * @param[in] transitionSet transitionSet that this transitionBase is added.
   */
  void SetPlay(Dali::Toolkit::TransitionSet transitionSet);

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

protected:
  /**
   * @brief Set property map which will be used ad a animation start properties.
   * @param[in] propertyMap propertyMap that will be used as a start value of transition.
   */
  void SetStartPropertyMap(Property::Map propertyMap)
  {
    mStartPropertyMap = propertyMap;
  }

  /**
   * @brief Set property map which will be used ad a animation finish properties.
   * @param[in] propertyMap propertyMap that will be used as a finish value of transition.
   */
  void SetFinishPropertyMap(Property::Map propertyMap)
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
   * @brief Set target view which will be transition.
   * @param[in] targetView control that will be transition.
   */
  void SetTargetView(Dali::Toolkit::Control targetView)
  {
    mTargetView = targetView;
  }

  /**
   * @brief Gets world transform of input Actor.
   * @param[in] actor actor for get world transform.
   */
  Matrix GetWorldTransform(Dali::Actor actor);

  /**
   * @brief Gets world color of input Actor.
   * @param[in] actor actor for get world color.
   */
  Vector4 GetWorldColor(Dali::Actor actor);

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
   * @brief Computes and center position by using transform properties.
   * @param[in] anchorPoint anchorPoint of an actor.
   * @param[in] positionUsesAnchorPoint positionUsesAnchorPoint of an actor.
   * @param[in] size size of an actor.
   * @param[in] scale scale of an actor.
   * @param[in] orientation orientation of an actor.
   */
  Vector3 CalculateCenterPosition(
    const Vector3&    anchorPoint,
    const bool        positionUsesAnchorPoint,
    const Vector3&    size,
    const Vector3&    scale,
    const Quaternion& orientation);

  /**
   * @brief Makes property animation for transition.
   */
  void SetAnimation();

  /**
   * @brief Gets Properties of input control.
   * @param[in] control control to get properties.
   */
  Property::Map GetProperties(Dali::Toolkit::Control control);

  /**
   * @brief Adds a property on an animation between sourceValue and destimationValue.
   * @param[in] target target control to be animated.
   * @param[in] index property index for animation.
   * @param[in] sourceValue source value of animation.
   * @param[in] destinationValue destination value of animation.
   */
  void AnimateBetween(Dali::Toolkit::Control target, Property::Index index, Property::Value sourceValue, Property::Value destinationValue);

  /**
   * @brief Copy target view to make clone for the child Actors
   */
  void CopyTarget();

  /**
   * @brief Make animators for Transform.
   * If the transition requires the information of world transform, let them be in this method.
   *
   * @note Do not set any properties in this methods.
   */
  virtual void OnPlay()
  {
  }

  /**
   * @brief Make animators for Transform.
   * If the transition requires the information of world transform, let them be in this method.
   *
   * @note Do not set any properties in this methods.
   */
  virtual void OnFinished()
  {
  }

private:
  Dali::Toolkit::Control       mTargetView;           ///< Target View that will be animated.
  Dali::Actor                  mCopiedActor;          ///< Copied View that will replace mTargetView during transition
  Dali::Animation              mAnimation;            ///< Property animations for the transition of mTargetView
  Dali::Toolkit::TransitionSet mTransitionSet;        ///< Handle of TransitionSet
  AlphaFunction                mAlphaFunction;        ///< Alpha function that will applied for the property animation
  Property::Map                mStartPropertyMap;     ///< Start properties to be animated. (world transform)
  Property::Map                mFinishPropertyMap;    ///< Finish properties to be animated. (world transform)
  Property::Map                mOriginialPropertyMap; ///< Original properties of mTargetView to be used to restore after the transition is finished.
  float                        mDurationSeconds;      ///< Duration of transition (seconds)
  float                        mDelaySeconds;         ///< Delay to be started to play transition (seconds)
  bool                         mTransitionWithChild;  ///< True, if mTargetView transition is inherit to its child Actors.
                                                      ///< If this is false, the child Actors are moved to the child of mCopiedActor that will have original properties of target Actor during Transition.
  bool mMoveTargetChildren;                           ///< Flag, if mTransitionWithChild is false and mTargetView has children than True.
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

#endif // DALI_INTERNAL_TRANSITION_BASE_H
