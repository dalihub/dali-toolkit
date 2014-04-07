#ifndef __DALI_TOOLKIT_SCROLL_CONNECTOR_H__
#define __DALI_TOOLKIT_SCROLL_CONNECTOR_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollConnector;
}

/**
 * ScrollConnector is used to connect scrollable containers with components such as scroll bars, indicators etc.
 *
 * This basically consists of:
 *   - A "scroll-position" property which controls which part of the scrollable content is visible
 *   - The minimum/maximum limits of the scroll position, which corresponds to the start & end points of the scollable list etc.
 *   - An "overshoot" property which shows any attempts to exceed the start & end points.
 *
 * The provider of the scrollable content is responsible for calling SetLimits(). Scroll-bar components are then expected to
 * receive these values via the LIMITS_CHANGED signal.
 *
 * The scroll-position property is accessible via GetScrollPositionObject(). This is intended to be shared by multiple controls
 * e.g. a list control may be scrolled by panning the list content, or indirectly by dragging a connected scroll-bar control.
 *
 * The overshoot property is intended for implementing 'end of list' style indicators. This property is expected to be in the range
 * -1 to 1, where -1 shows an attempt the scroll beyond the minimum limit, and 1 shows an attempt the scroll beyond the maximum limit.
 * Zero indicates normal scrolling i.e. when overshoot indicators should be hidden.
 */
class ScrollConnector : public BaseHandle
{
public:

  static const char* const SCROLL_POSITION_PROPERTY_NAME; ///< This float "scroll-position" property is available via GetScrollPositionObject()
  static const char* const OVERSHOOT_PROPERTY_NAME;       ///< This float "overshoot" property is available via GetScrollPositionObject()

  static const Property::Index SCROLL_POSITION;           ///< The index of the "scroll-position" property
  static const Property::Index OVERSHOOT;                 ///< The index of the "overshoot" property

  // Signals
  static const char* const LIMITS_CHANGED_SIGNAL_NAME;    ///< "limits-changed" signal name
  typedef SignalV2< void ( float min, float max ) > LimitsChangedSignalType;

  /**
   * Create a ScrollConnector.
   * @return A handle to a newly allocated ScrollConnector.
   */
  static ScrollConnector New();

  /**
   * Create an empty handle.
   */
  ScrollConnector();

  /**
   * Copy constructor.
   * @param handle A handle to copy.
   */
  ScrollConnector( const ScrollConnector& handle );

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using Dali::BaseHandle::operator=;

  /**
   * Virtual destructor.
   */
  virtual ~ScrollConnector();

  /**
   * Downcast a BaseHandle to ScrollConnector handle.
   * @return A handle to a ScrollConnector or an empty handle.
   */
  static ScrollConnector DownCast( BaseHandle handle );

  /**
   * Set The min/max values, corresponding to the start & end position of the scrollable content.
   * @param[in] min The minimum value.
   * @param[in] max The maximum value.
   */
  void SetLimits( float min, float max );

  /**
   * Retrieve the min limit.
   * @return The minimum value.
   */
  float GetMinLimit() const;

  /**
   * Retrieve the max limit.
   * @return The maximum value.
   */
  float GetMaxLimit() const;

  /**
   * Signal emitted after the SetLimits() method has been called.
   */
  ScrollConnector::LimitsChangedSignalType& LimitsChangedSignal();

  /**
   * Retrieve the object which provides the "scroll-position" property.
   * @return The scroll-position object.
   */
  Constrainable GetScrollPositionObject() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in] impl The Control implementation.
   */
  ScrollConnector( Internal::ScrollConnector* impl );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_CONNECTOR_H__
