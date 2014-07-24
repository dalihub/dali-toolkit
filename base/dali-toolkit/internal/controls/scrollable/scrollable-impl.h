#ifndef __DALI_TOOLKIT_INTERNAL_SCROLLABLE_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLLABLE_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>

#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class Scrollable;
typedef IntrusivePtr<Scrollable> ScrollablePtr;

/**
 * @copydoc Toolkit::Scrollable
 */
class Scrollable : public Control
{
public:
  static const std::string SCROLLABLE_CAN_SCROLL_VERTICAL;
  static const std::string SCROLLABLE_CAN_SCROLL_HORIZONTAL;

  /**
   * Create a new Scrollable.
   * @return A public handle to the newly allocated Scrollable.
   */
//  static Dali::Toolkit::Scrollable New();

public:

  /**
   * @copydoc Dali::Toolkit::Scrollable::IsScrollComponentEnabled(Scrollable::ScrollComponentType type)
   */
  bool IsScrollComponentEnabled(Toolkit::Scrollable::ScrollComponentType type) const;

  /**
   * @copydoc Dali::Toolkit::Scrollable::EnableScrollComponent(Scrollable::ScrollComponentType type)
   */
  void EnableScrollComponent(Toolkit::Scrollable::ScrollComponentType type);

  /**
   * @copydoc Dali::Toolkit::Scrollable::DisableScrollComponent(Scrollable::ScrollComponentType type)
   */
  void DisableScrollComponent(Toolkit::Scrollable::ScrollComponentType type);

  /**
   * Gets the size of the domain (minimum/maximum extents for each axis to scroll to)
   * @return the domain size
   */
  virtual Vector3 GetDomainSize() const = 0;

  /**
   * Adds actor as an Overlay to Scrollable
   * This method is called by Add-on UI components
   * such as scroll bars, page indicators.
   * @param[in] actor Actor to add as an overlay.
   */
  virtual void AddOverlay(Actor actor) = 0;

  /**
   * Removes overlay actor from Scrollable
   * This method is called by Add-on UI components
   * such as scroll bars, page indicators.
   * @param[in] actor Actor overlay to remove.
   */
  virtual void RemoveOverlay(Actor actor) = 0;

  /**
   * Retrieves current scroll position.
   * @returns The current scroll position.
   */
  virtual Vector3 GetCurrentScrollPosition() const = 0;

  /**
   * Scrolls Scrollable to position specified (contents will scroll to this position)
   * Position 0,0 is the origin. Increasing X scrolls contents left, while
   * increasing Y scrolls contents up.
   * @param[in] position The position to scroll to.
   * @param[in] duration The duration of the animation in seconds
   */
  virtual void ScrollTo(const Vector3 &position, float duration) = 0;

  /**
   * Set the color of the overshoot effect.
   * @parm[in] color The color of the overshoot effect
   */
  virtual void SetOvershootEffectColor( const Vector4& color ) = 0;

  /**
   * Retrieve the color of the overshoot effect.
   * @return The color of the overshoot effect.
   */
  Vector4 GetOvershootEffectColor() const;

private:

  /**
   * Temporary function to override EnableScrollComponent functionality for overshoot
   * Only ScrollView needs to override this as HQ has not requested disable functionality in ItemView
   * @param[in] enable true to enable, false to disable overshoot indicator
   */
  virtual void SetOvershootEnabled(bool enable) {}

public: //Signals

  /**
   * @copydoc Dali::Toolkit::Scrollable::ScrollStartedSignal()
   */
  Toolkit::Scrollable::ScrollStartedSignalV2& ScrollStartedSignal();

  /**
   * @copydoc Dali::Toolkit::Scrollable::ScrollUpdatedSignal()
   */
  Toolkit::Scrollable::ScrollUpdatedSignalV2& ScrollUpdatedSignal();

  /**
   * @copydoc Dali::Toolkit::Scrollable::ScrollCompletedSignal()
   */
  Toolkit::Scrollable::ScrollCompletedSignalV2& ScrollCompletedSignal();

  /**
   * @copydoc Dali::Toolkit::Scrollable::ScrollClampedSignal()
   */
  Toolkit::Scrollable::ScrollClampedSignalV2& ScrollClampedSignal();

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

  //properties

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

protected:

  /**
   * Construct a new Scrollable.
   */
  Scrollable();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Scrollable();

  /**
   * Register common properties
   */
  void RegisterCommonProperties();

private:

  /**
   * Gets position property.
   *
   * @return The current position
   */
  Vector3 GetPropertyPosition() const;

private:

  // Undefined
  Scrollable(const Scrollable&);

  // Undefined
  Scrollable& operator=(const Scrollable& rhs);

  /**
   * Helper to create an initialized ScrollComponent
   * @param[in] scrollable reference to ScrollView implementation
   * @param[in] type the type of scroll component to create.
   * @return A pointer to the created ScrollComponent.
   */
  Toolkit::ScrollComponent NewScrollComponent(Toolkit::Scrollable& scrollable, Toolkit::Scrollable::ScrollComponentType type);

protected:

  Vector4         mOvershootEffectColor;    ///<The color of the overshoot bouncing effect

  Property::Index mPropertyRelativePosition;///< Scroll Relative Position ("scroll-relative-position") [range from 0.0f - 1.0f in each axes]
  Property::Index mPropertyPositionMin;     ///< Scroll Domain Minimum ("position-min")
  Property::Index mPropertyPositionMax;     ///< Scroll Domain Maximum ("position-max")
  Property::Index mPropertyScrollDirection; ///< Scroll direction ("scroll-direction")
  Property::Index mPropertyCanScrollVertical;    ///< Whether the current scroll domain is large enough to scroll vertically
  Property::Index mPropertyCanScrollHorizontal;    ///< Whether the current scroll domain is large enough to scroll horizontally

  std::map<Toolkit::Scrollable::ScrollComponentType, ScrollComponentPtr> mComponent;  ///< ScrollComponent (such as a scrollbar/page indicator/status)

  Toolkit::Scrollable::ScrollStartedSignalV2 mScrollStartedSignalV2;
  Toolkit::Scrollable::ScrollUpdatedSignalV2 mScrollUpdatedSignalV2;
  Toolkit::Scrollable::ScrollCompletedSignalV2 mScrollCompletedSignalV2;
  Toolkit::Scrollable::ScrollClampedSignalV2 mScrollClampedSignalV2;

private:

  typedef std::map<Toolkit::Scrollable::ScrollComponentType, ScrollComponentPtr> ComponentContainer;
  typedef ComponentContainer::iterator ComponentIter;

  ComponentContainer mComponents;  ///< ScrollComponent (such as a scrollbar/page indicator/status)
  bool mOvershootEnabled:1;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Scrollable& GetImpl(Toolkit::Scrollable& scrollable)
{
  DALI_ASSERT_ALWAYS(scrollable);

  Dali::RefObject& handle = scrollable.GetImplementation();

  return static_cast<Toolkit::Internal::Scrollable&>(handle);
}

inline const Toolkit::Internal::Scrollable& GetImpl(const Toolkit::Scrollable& scrollable)
{
  DALI_ASSERT_ALWAYS(scrollable);

  const Dali::RefObject& handle = scrollable.GetImplementation();

  return static_cast<const Toolkit::Internal::Scrollable&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLLABLE_H__
