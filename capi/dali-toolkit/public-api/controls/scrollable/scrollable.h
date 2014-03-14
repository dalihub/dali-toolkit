#ifndef __DALI_TOOLKIT_SCROLLABLE_H__
#define __DALI_TOOLKIT_SCROLLABLE_H__

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

/**
 * @addtogroup CAPI_DALI_TOOLKIT_SCROLLABLE_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Scrollable;
}

/**
 * @brief How axes/rotation or scale are clamped
 */
enum ClampState
{
  NotClamped,   ///< The quantity isn't clamped
  ClampedToMin, ///< The quantity is clamped to the min value
  ClampedToMax  ///< The quantity is clamped to the max value
};

/**
 * @brief A 2 dimensional clamp
 */
struct ClampState2
{
  ClampState x; ///< The clamp state of the x axis
  ClampState y; ///< The clamp state of the y axis
};

/**
 * @brief A 3 dimensional clamp
 */
struct ClampState3
{
  ClampState x; ///< The clamp state of the x axis
  ClampState y; ///< The clamp state of the y axis
  ClampState z; ///< The clamp state of the z axis
};

/**
 * @brief Base class for derived Scrollables that contains actors that can be scrolled manually
 * (via touch) or automatically.
 *
 * Scrollables such as ScrollView and ItemView can be derived from this class.
 */
class Scrollable : public Control
{
public:

  /**
   * @brief Clamp signal event's data
   */
  struct ClampEvent
  {
    ClampState3 scale;       ///< Clamp information for scale axes
    ClampState3 position;    ///< Clamp information for position axes
    ClampState rotation;     ///< Clamp information for rotation
  };

  /**
   * @brief Scroll component types
   */
  enum ScrollComponentType
  {
    HorizontalScrollBar,
    VerticalScrollBar,
    OvershootIndicator,
  };

  // Custom properties

  static const std::string SCROLL_RELATIVE_POSITION_PROPERTY_NAME;      ///< Property, name "scroll-relative-position", type VECTOR3
  static const std::string SCROLL_POSITION_MIN_PROPERTY_NAME;           ///< Property, name "scroll-position-min",      type VECTOR3
  static const std::string SCROLL_POSITION_MAX_PROPERTY_NAME;           ///< Property, name "scroll-position-max",      type VECTOR3
  static const std::string SCROLL_DIRECTION_PROPERTY_NAME;              ///< Property, name "scroll-direction",         type VECTOR2

  //Signal Names
  static const char* const SIGNAL_SCROLL_STARTED;   ///< "scroll-started";
  static const char* const SIGNAL_SCROLL_COMPLETED; ///< "scroll-completed";
  static const char* const SIGNAL_SCROLL_UPDATED;   ///< "scroll-updated";
  static const char* const SIGNAL_SCROLL_CLAMPED;   ///< "scroll-clamped";

public:

  typedef SignalV2< void ( const Vector3& ) > ScrollStartedSignalV2;   ///< ScrollStarted signal type
  typedef SignalV2< void ( const Vector3& ) > ScrollCompletedSignalV2; ///< ScrollCompleted signal type
  typedef SignalV2< void ( const Vector3& ) > ScrollUpdatedSignalV2;   ///< Scroll updated signal type
  typedef SignalV2< void ( const ClampEvent& ) > ScrollClampedSignalV2; ///< Scroll clamped signal type

  /**
   * @brief Signal emitted when the Scrollable has moved (whether by touch or animation).
   */
  ScrollStartedSignalV2& ScrollStartedSignal();

  /**
   * @brief Signal emitted when the Scrollable has moved (whether by touch or animation).
   */
  ScrollUpdatedSignalV2& ScrollUpdatedSignal();

  /**
   * @brief Signal emitted when the Scrollable has completed movement (whether by touch or animation).
   */
  ScrollCompletedSignalV2& ScrollCompletedSignal();

  /**
   * @brief Signal emitted when the Scrollable is pushing against a domain boundary
   * (in either position, scale, or rotation).
   *
   * @return The signal to connect to
   */
  ScrollClampedSignalV2& ScrollClampedSignal();

public:

  /**
   * @brief Creates an uninitialized Scrollable handle.
   */
  Scrollable();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   *
   * @param handle to copy from
   */
  Scrollable( const Scrollable& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] handle to copy from
   * @return A reference to this
   */
  Scrollable& operator=( const Scrollable& handle );

  /**
   * @brief Virtual destructor.
   *
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Scrollable();

  /**
   * @brief Downcast an Object handle to Scrollable.
   *
   * If handle points to a Scrollable the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a Scrollable or an uninitialized handle
   */
  static Scrollable DownCast( BaseHandle handle );

  /**
   * @brief Checks if a ScrollComponent has been enabled or not.
   *
   * @param[in] type The Scroll Component Type to check
   * @return True (if Enabled)
   */
  bool IsScrollComponentEnabled(Scrollable::ScrollComponentType type) const;

  /**
   * @brief Enables a ScrollComponent.
   *
   * @param[in] type The Scroll Component Type to enable
   */
  void EnableScrollComponent(Scrollable::ScrollComponentType type);

  /**
   * @brief Disables a ScrollComponent.
   *
   * @param[in] type The Scroll Component Type to disable
   */
  void DisableScrollComponent(Scrollable::ScrollComponentType type);

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  Scrollable(Internal::Scrollable& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Scrollable( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLLABLE_H__
