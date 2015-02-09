#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_OVERSHOOT_INDICATOR_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_OVERSHOOT_INDICATOR_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class Scrollable;
class ScrollOvershootEffect;
class ScrollOvershootEffectGradient;
class ScrollOvershootEffectRipple;
typedef IntrusivePtr<ScrollOvershootEffect> ScrollOvershootEffectPtr;
typedef IntrusivePtr<ScrollOvershootEffectGradient> ScrollOvershootEffectGradientPtr;
typedef IntrusivePtr<ScrollOvershootEffectRipple> ScrollOvershootEffectRipplePtr;

struct ScrollOvershootIndicator : public Dali::RefObject
{
public:

  /**
   * ScrollOvershootIndicator constructor.
   */
  ScrollOvershootIndicator();

  /**
   * Virtual destructor
   */
  virtual ~ScrollOvershootIndicator();

  /**
   * Attaches the scroll indicator to a scrollable actor
   *
   * &param[in] scrollable The scrollable actor to attach to
   */
  void AttachToScrollable(Scrollable& scrollable);

  /**
   * Detaches the scroll indicator from a scrollable actor
   *
   * &param[in] scrollable The scrollable actor to detach from
   */
  void DetachFromScrollable(Scrollable& scrollable);

  /**
   * Resets the indicator
   */
  void Reset();

  /**
   * Clears the overshoot
   */
  void ClearOvershoot();

  /**
   * Create an initialized ScrollOvershootIndicator
   *
   * @return A pointer to the created ScrollOvershootIndicator.
   */
  static ScrollOvershootIndicator* New();

  /**
   * Set the color of the overshoot effect.
   * @parm[in] color The color of the overshoot effect
   */
  void SetOvershootEffectColor( const Vector4& color );

private:
  ScrollOvershootEffectPtr mEffectX;                      ///< effect used for x-axis/horizontal display
  ScrollOvershootEffectPtr mEffectY;                      ///< effect used for y-axis/vertical display
};

/**
 * ScrollOvershootEffect is a derivable class, designed to allow the application programmer to create their own
 * overshoot effect and apply it with minimal implementation required
 */
struct ScrollOvershootEffect : public Dali::RefObject
{
public:
  /**
   * Create a new overshoot effect, passing in whether it is vertical or horizontal
   *
   * @param[in] vertical whether this effect is a vertical or horizontal one
   */
  ScrollOvershootEffect( bool vertical );

  /**
   * Virtual destructor
   */
  virtual ~ScrollOvershootEffect() {}

  /**
   * Returns if this is a vertical or horizontal overhoot effect
   *
   * @return true or false
   */
  bool IsVertical() const;

  /**
   * Applies the indicator effect, all derived effects must implement this function
   *
   * @param[in] scrollable the scrollable object to apply this effect to
   */
  virtual void Apply() = 0;

  /**
   * Removes the indicator effect, all derived effects must implement this function
   *
   * @param[in] scrollable the scrollable object to remove this effect from
   */
  virtual void Remove( Scrollable& scrollable ) = 0;

  /**
   * Resets this overshoot effect
   */
  virtual void Reset() = 0;

  /**
   * Sets up property notifications for overshoot values
   */
  virtual void UpdatePropertyNotifications() {}

  /**
   * @copydoc ScrollOvershootIndicator::SetOvershootEffectColor()
   */
  virtual void SetOvershootEffectColor( const Vector4& color ) = 0;

  /**
   * Sets shader overshoot value, either immediately of by animating over time
   *
   * @param[in] amount The amount to set overshoot to [-1.0f,1.0f]
   * @param[in] animate Whether to animate or set immediately
   */
  virtual void SetOvershoot(float amount, bool animate = true) = 0;

private:
  bool mVertical;                      ///< whether this is a vertical/horizontal effect
};

/**
 * ScrollOvershootEffectRipple creates an animated bounce effect at the end of the scrollable area if the user
 * attempts to scroll past it
 */
struct ScrollOvershootEffectRipple : public ScrollOvershootEffect, public ConnectionTracker
{
  enum AnimationState
  {
    AnimatingIn  = 0x01,  ///< animating overshoot to 0
    AnimatingOut = 0x02,  ///< animating overshoot to negative (overshoot image displays in +ve area of screen)
    AnimateBack  = 0x04,  ///< indicates that we need to animate overshoot back to zero immediately after it has finished animating in
  };

public:

  /**
   * Create a new gradient overshoot effect, passing in whether it is vertical or horizontal
   *
   * @param[in] vertical Whether this indicator is vertical or horizontal
   */
  ScrollOvershootEffectRipple( bool vertical, Scrollable& scrollable );

  /**
   * @copydoc ScrollOvershootEffect::Apply
   */
  virtual void Apply();

  /**
   * @copydoc ScrollOvershootEffect::Remove
   */
  virtual void Remove( Scrollable& scrollable );

  /**
   * @copydoc ScrollOvershootEffect::Reset
   */
  virtual void Reset();

  /**
   * @copydoc ScrollOvershootEffect::UpdatePropertyNotifications
   */
  void UpdatePropertyNotifications();

  /**
   * @copydoc ScrollOvershootEffect::SetOvershootEffectColor()
   */
  void SetOvershootEffectColor( const Vector4& color );

  /**
   * Updates the vibility of the overshoot image as well as updating its size, position and rotation
   * This function is called when animation starts and finishes
   *
   * @param[in] visible Whether to set the image visible or not
   */
  void UpdateVisibility( bool visible );

  /**
   * Informs overshoot effect to update image position and to animate effect overshoot value for a
   * positive overshoot value from scrollview
   *
   * @param[in] source the property notification that triggered this callback
   */
  void OnOvershootNotification(PropertyNotification& source);

  /**
   * @copydoc ScrollOvershootEffect::SetOvershoot()
   */
  void SetOvershoot(float amount, bool animate = true);

  /**
   * Connects to the animation finished signal of our overshoot animation
   *
   * @param[in] animation the animation instance that has finished
   */
  void OnOvershootAnimFinished(Animation& animation);

  /**
   * Creates a new ScrollOvershootEffectGradient objects and returns a pointer to it
   *
   * @param[in] vertical whether to create a vertical(true) or horizontal effect
   * @return a pointer to the new effect
   */
  static ScrollOvershootEffectRipplePtr New( bool vertical, Scrollable& scrollable );

private:
  Actor                 mOvershootOverlay;             ///< the actor which displays the overshoot effect
  Scrollable&           mAttachedScrollView;           ///< the actor that this indicator has been attached to
  Animation             mScrollOvershootAnimation;     ///< overshoot animation
  PropertyNotification  mOvershootIncreaseNotification;///< notification used to inform as overshoot increases
  PropertyNotification  mOvershootDecreaseNotification;///< notification used to inform as overshoot decreases
  Property::Index       mOvershootProperty;            ///< index of the overshoot property in the scrollable actor
  Property::Index       mEffectOvershootProperty;      ///< index of the effect's overshoot property
  float                mOvershoot;                    ///< last overshoot value as detected by notifications
  unsigned short      mAnimationStateFlags;          ///< contains flags indicating the current state of the overshoot animation
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_OVERSHOOT_INDICATOR_H__
