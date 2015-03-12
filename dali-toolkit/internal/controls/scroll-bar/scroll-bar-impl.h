#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__

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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/property-notification.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-connector.h>
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
class ScrollBar : public ScrollComponentImpl
{

public:

  // Signals
  typedef Toolkit::ScrollBar::ScrollPositionNotifiedSignalType ScrollPositionNotifiedSignalType;

public:

  /**
   * @copydoc Toolkit::ScrollBar::ScrollBar()
   */
  ScrollBar();

  /**
   * @copydoc Toolkit::ScrollBar::~ScrollBar()
   */
  virtual ~ScrollBar();

  /**
   * @copydoc Toolkit::ScrollBar::New()
   */
  static Toolkit::ScrollBar New();

  /**
   * @copydoc Toolkit::ScrollComponentImpl::OnScrollConnectorSet()
   */
  void OnScrollConnectorSet( Toolkit::ScrollConnector connector );

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorImage()
   */
  void SetIndicatorImage( Image image );

  /**
   * @copydoc Toolkit::ScrollBar::GetScrollIndicator()
   */
  Actor GetScrollIndicator();

  /**
   * @copydoc Toolkit::ScrollBar::SetPositionNotifications()
   */
  void SetPositionNotifications( const std::vector<float>& positions );

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorHeightPolicy()
   */
  void SetIndicatorHeightPolicy( Toolkit::ScrollBar::IndicatorHeightPolicy policy );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorHeightPolicy()
   */
  Toolkit::ScrollBar::IndicatorHeightPolicy GetIndicatorHeightPolicy();

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorFixedHeight()
   */
  void SetIndicatorFixedHeight( float height );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorFixedHeight()
   */
  float GetIndicatorFixedHeight();

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorShowDuration()
   */
  void SetIndicatorShowDuration( float durationSeconds );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorShowDuration()
   */
  float GetIndicatorShowDuration();

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorHideDuration()
   */
  void SetIndicatorHideDuration( float durationSeconds );

  /**
   * @copydoc Toolkit::ScrollBar::GetIndicatorHideDuration()
   */
  float GetIndicatorHideDuration();

  /**
   * @copydoc Toolkit::ScrollBar::Show()
   */
  void Show();

  /**
   * @copydoc Toolkit::ScrollBar::Hide()
   */
 void Hide();

 /**
  * Signal emitted after the SetScrollDomain() method has been called.
  */
 ScrollPositionNotifiedSignalType& ScrollPositionNotifiedSignal()
 {
   return mScrollPositionNotifiedSignal;
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
  virtual void OnPan( PanGesture gesture );

  /**
   * @copydoc Toolkit::Control::OnControlSizeSet( const Vector3& size )
   */
  virtual void OnControlSizeSet( const Vector3& size );

private:

  /**
   * Apply constraints for background and indicator.
   * These constraints are based on values from the scroll connector.
   */
  void ApplyConstraints();

  /**
   * Callback when the start & end position and size of the scrollable content are changed.
   * @param[in] minPosition The minimum position.
   * @param[in] maxPosition The maximum position.
   * @param[in] contentSize The size of scrollable content.
   */
  void OnScrollDomainChanged(float minPosition, float maxPosition, float contentSize);

  /**
   * Callback when the current scroll position of the scrollable content goes above or
   * below the values specified by SetPositionNotifications().
   * @param[in] source the property notification that triggered this callback
   */
  void OnScrollPositionNotified(PropertyNotification& source);

  /**
   * Process the pan gesture per predefined timeout until the gesture is finished.
   * @return True if the timer should be kept running.
   */
  bool OnPanGestureProcessTick();

  /**
   * Handle SetProperty for scroll indicator height policy.
   * @param[in] propertyValue The new property value.
   */
  void OnIndicatorHeightPolicyPropertySet(Property::Value propertyValue);

private:

  Handle mScrollPositionObject;                               ///< From mScrollConnector

  ImageActor mIndicator;                                             ///< Image of scroll indicator.
  Animation mAnimation;                                              ///< Scroll indicator Show/Hide Animation.

  float mIndicatorShowDuration;                                     ///< The duration of scroll indicator show animation
  float mIndicatorHideDuration;                                     ///< The duration of scroll indicator hide animation

  float mScrollStart;                                               ///< Scroll Start position (start of drag)
  Vector3 mGestureDisplacement;                                      ///< Gesture Displacement.

  bool mIsPanning;                                                  ///< Whether the scroll bar is being panned.
  float mCurrentScrollPosition;                                     ///< The current scroll position updated by the pan gesture

  Toolkit::ScrollBar::IndicatorHeightPolicy mIndicatorHeightPolicy;  ///< The height policy of scroll indicator (variable or fixed)
  float mIndicatorFixedHeight;                                      ///< The fixed height of scroll indicator

  Timer mTimer;                                                      ///< The timer to process the pan gesture after the gesture is started.

  Property::Index mPropertyIndicatorPosition;                        ///< Indicatore Position ("indicator-position")

  PropertyNotification mPositionNotification;                        ///< Stores the property notification used for scroll position changes

  ScrollPositionNotifiedSignalType mScrollPositionNotifiedSignal;

  ActiveConstraint mIndicatorSizeConstraint;
  ActiveConstraint mIndicatorPositionConstraint;
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
