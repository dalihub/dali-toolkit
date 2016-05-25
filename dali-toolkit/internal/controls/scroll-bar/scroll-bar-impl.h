#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ScrollBar;

typedef IntrusivePtr<ScrollBar> ScrollBarPtr;

/**
 * ScrollBar is a UI component that can be added to the scrollable controls
 * indicating the current scroll position of the scrollable content.
 */
class ScrollBar : public Control
{

public:

  // Signals
  typedef Toolkit::ScrollBar::PanFinishedSignalType PanFinishedSignalType;
  typedef Toolkit::ScrollBar::ScrollPositionIntervalReachedSignalType ScrollPositionIntervalReachedSignalType;

public:

  /**
   * @copydoc Toolkit::ScrollBar::New()
   */
  static Toolkit::ScrollBar New(Toolkit::ScrollBar::Direction direction);

  /**
   * @copydoc Toolkit::ScrollBar::SetScrollPropertySource()
   */
  void SetScrollPropertySource( Handle handle, Property::Index propertyScrollPosition, Property::Index propertyMinScrollPosition, Property::Index propertyMaxScrollPosition, Property::Index propertyScrollContentSize );

  /**
   * @copydoc Toolkit::ScrollBar::SetScrollIndicator()
   */
  void SetScrollIndicator( Actor indicator );

  /**
   * @copydoc Toolkit::ScrollBar::GetScrollIndicator()
   */
  Actor GetScrollIndicator();

  /**
   * @copydoc Toolkit::ScrollBar::SetScrollPositionIntervals()
   */
  void SetScrollPositionIntervals( const Dali::Vector<float>& positions );

  /**
   * @copydoc Toolkit::ScrollBar::GetScrollPositionIntervals()
   */
  Dali::Vector<float> GetScrollPositionIntervals() const;

  /**
   * @copydoc Toolkit::ScrollBar::SetScrollDirection()
   */
  void SetScrollDirection( Toolkit::ScrollBar::Direction direction );

  /**
   * @copydoc Toolkit::ScrollBar::GetScrollDirection()
   */
  Toolkit::ScrollBar::Direction GetScrollDirection() const;

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorHeightPolicy()
   */
  void SetIndicatorHeightPolicy( Toolkit::ScrollBar::IndicatorHeightPolicy policy );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorHeightPolicy()
   */
  Toolkit::ScrollBar::IndicatorHeightPolicy GetIndicatorHeightPolicy() const;

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorFixedHeight()
   */
  void SetIndicatorFixedHeight( float height );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorFixedHeight()
   */
  float GetIndicatorFixedHeight() const;

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorShowDuration()
   */
  void SetIndicatorShowDuration( float durationSeconds );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorShowDuration()
   */
  float GetIndicatorShowDuration() const;

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorHideDuration()
   */
  void SetIndicatorHideDuration( float durationSeconds );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorHideDuration()
   */
  float GetIndicatorHideDuration() const;

  /**
   * @copydoc Toolkit::ScrollBar::ShowIndicator()
   */
  void ShowIndicator();

  /**
   * @copydoc Toolkit::ScrollBar::HideIndicator()
   */
 void HideIndicator();

 /**
  * @copydoc Toolkit::ScrollBar::PanFinishedSignal()
  */
 PanFinishedSignalType& PanFinishedSignal()
 {
   return mPanFinishedSignal;
 }

 /**
  * @copydoc Toolkit::ScrollBar::ScrollPositionIntervalReachedSignal()
  */
 ScrollPositionIntervalReachedSignalType& ScrollPositionIntervalReachedSignal()
 {
   return mScrollPositionIntervalReachedSignal;
 }

 /**
  * Connects a callback function with the object's signals.
  * @param[in] object The object providing the signal.
  * @param[in] tracker Used to disconnect the signal.
  * @param[in] signalName The signal to connect to.
  * @param[in] functor A newly allocated FunctorDelegate.
  * @return True if the signal was connected.
  * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
  */
 static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

 // Properties

 /**
  * Called when a property of an object of this type is set.
  * @param[in] object The object whose property is set.
  * @param[in] index The property index.
  * @param[in] value The new property value.
  */
 static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

 /**
  * Called to retrieve a property of an object of this type.
  * @param[in] object The object whose property is to be retrieved.
  * @param[in] index The property index.
  * @return The current value of the property.
  */
 static Property::Value GetProperty( BaseObject* object, Property::Index index );

private: // from Control

  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnPan
   */
  virtual void OnPan( const PanGesture& gesture );

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& size )
   */
  virtual void OnSizeSet( const Vector3& size );

private:

  /**
   * Create the default indicator actor.
   */
  void CreateDefaultIndicatorActor();

  /**
   * Apply constraints for background and indicator.
   * These constraints are based on values from the scroll connector.
   */
  void ApplyConstraints();

  /**
   * Callback when the current scroll position of the scrollable content goes above or
   * below the values specified by SetScrollPositionIntervals().
   * @param[in] source the property notification that triggered this callback
   */
  void OnScrollPositionIntervalReached(PropertyNotification& source);

  /**
   * Process the pan gesture per predefined timeout until the gesture is finished.
   * @return True if the timer should be kept running.
   */
  bool OnPanGestureProcessTick();

  /**
   * Handle SetProperty for scroll direction.
   * @param[in] propertyValue The new property value.
   */
  void OnScrollDirectionPropertySet(Property::Value propertyValue);

  /**
   * Handle SetProperty for scroll indicator height policy.
   * @param[in] propertyValue The new property value.
   */
  void OnIndicatorHeightPolicyPropertySet(Property::Value propertyValue);

private:

  /**
   * Constructor.
   * It initializes ScrollBar members.
   */
  ScrollBar( Toolkit::ScrollBar::Direction direction );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollBar();

private:

  Actor mIndicator;                                                  ///< Image of scroll indicator.
  float mIndicatorShowAlpha;                                         ///< The alpha value when the indicator is fully shown
  Animation mAnimation;                                              ///< Scroll indicator Show/Hide Animation.

  Toolkit::ScrollBar::Direction mDirection;                          ///< The direction of scroll bar (vertical or horizontal)

  WeakHandleBase mScrollableObject;                                  ///< Object to be scrolled

  Property::Index mPropertyScrollPosition;                           ///< Index of scroll position property owned by the object to be scrolled
  Property::Index mPropertyMinScrollPosition;                        ///< Index of minimum scroll position property owned by the object to be scrolled
  Property::Index mPropertyMaxScrollPosition;                        ///< Index of maximum scroll position property owned by the object to be scrolled
  Property::Index mPropertyScrollContentSize;                        ///< Index of scroll content size property owned by the object to be scrolled

  float mIndicatorShowDuration;                                      ///< The duration of scroll indicator show animation
  float mIndicatorHideDuration;                                      ///< The duration of scroll indicator hide animation

  float mScrollStart;                                                ///< Scroll Start position (start of drag)
  Vector3 mGestureDisplacement;                                      ///< Gesture Displacement.

  float mCurrentScrollPosition;                                      ///< The current scroll position updated by the pan gesture

  Toolkit::ScrollBar::IndicatorHeightPolicy mIndicatorHeightPolicy;  ///< The height policy of scroll indicator (variable or fixed)
  float mIndicatorFixedHeight;                                       ///< The fixed height of scroll indicator
  float mIndicatorMinimumHeight;                                     ///< The minimum height for a variable size indicator
  float mIndicatorStartPadding;                                      ///< The padding at the start of the indicator
  float mIndicatorEndPadding;                                        ///< The padding at the end of the indicator

  Timer mContractDelayTimer;                                         ///< Timer guarantee contract delay time.
  Timer mPanProcessTimer;                                            ///< The timer to process the pan gesture after the gesture is started.

  Dali::Vector<float> mScrollPositionIntervals;                      ///< List of values to receive notification for when the current scroll position goes above or below them
  PropertyNotification mPositionNotification;                        ///< Stores the property notification used for scroll position changes

  PanFinishedSignalType mPanFinishedSignal;
  ScrollPositionIntervalReachedSignalType mScrollPositionIntervalReachedSignal;

  Constraint mIndicatorPositionConstraint;
  Constraint mIndicatorSizeConstraint;
  Constraint mScrollPositionInCurrentAxisConstraint;

  bool mIsPanning                 : 1;                               ///< Whether the scroll bar is being panned.
  bool mIndicatorFirstShow        : 1;                               ///< True if the indicator has never been shown
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ScrollBar& GetImpl(Toolkit::ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<Toolkit::Internal::ScrollBar&>(handle);
}

inline const Toolkit::Internal::ScrollBar& GetImpl(const Toolkit::ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  const Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<const Toolkit::Internal::ScrollBar&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__
