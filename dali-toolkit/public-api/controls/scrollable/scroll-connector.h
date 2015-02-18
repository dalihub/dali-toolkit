#ifndef __DALI_TOOLKIT_SCROLL_CONNECTOR_H__
#define __DALI_TOOLKIT_SCROLL_CONNECTOR_H__

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
#include <dali/public-api/object/constrainable.h>

namespace Dali
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
 *   - The scrollable content size, which corresponds to the length of the scrollable content in the scrollable container in actor coordinates.
 *
 * The provider of the scrollable content is responsible for calling SetScrollDomain(). Scroll-bar components are then expected to
 * receive these values via the DOMAIN_CHANGED signal.
 *
 * The scroll-position property is accessible via GetScrollPositionObject(). This is intended to be shared by multiple controls
 * e.g. a list control may be scrolled by panning the list content, or indirectly by dragging a connected scroll-bar control.
 *
 * The overshoot property is intended for implementing 'end of list' style indicators. This property is expected to be in the range
 * -1 to 1, where -1 shows an attempt the scroll beyond the minimum limit, and 1 shows an attempt the scroll beyond the maximum limit.
 * Zero indicates normal scrolling i.e. when overshoot indicators should be hidden.
 *
 * Signals
 * | %Signal Name            | Method                             |
 * |-------------------------|------------------------------------|
 * | domain-changed          | @ref DomainChangedSignal()         |
 * | scroll-position-changed | @ref ScrollPositionChangedSignal() |
 */
class DALI_IMPORT_API ScrollConnector : public BaseHandle
{
public:

  static const char* const SCROLL_POSITION_PROPERTY_NAME; ///< This float "scroll-position" property is available via GetScrollPositionObject()
  static const char* const OVERSHOOT_PROPERTY_NAME;       ///< This float "overshoot" property is available via GetScrollPositionObject()

  static const Property::Index SCROLL_POSITION;           ///< The index of the "scroll-position" property
  static const Property::Index OVERSHOOT;                 ///< The index of the "overshoot" property

  // Signals
  typedef Signal< void ( float min, float max, float size ) > DomainChangedSignalType;
  typedef Signal< void ( float position ) > ScrollPositionChangedSignalType;

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
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ScrollConnector();

  /**
   * @brief Downcast a BaseHandle to ScrollConnector handle.
   * @return A handle to a ScrollConnector or an empty handle.
   */
  static ScrollConnector DownCast( BaseHandle handle );

  /**
   * @brief Set the scroll domain, corresponding to the start & end position, and size of the scrollable content in actor coordinates.
   * @param[in] min The minimum scroll position limit.
   * @param[in] max The maximum scroll position limit.
   * @param[in] length The length of the scrollable content in actor coordinates.
   */
  void SetScrollDomain( float min, float max, float length );

  /**
   * @brief Retrieve the min limit.
   * @return The minimum value.
   */
  float GetMinLimit() const;

  /**
   * @brief Retrieve the max limit.
   * @return The maximum value.
   */
  float GetMaxLimit() const;

  /**
   * @brief Retrieve the length of the scrollable content in actor coordinates.
   * @return The length of the scrollable content.
   */
  float GetContentLength() const;

  /**
   * @brief Set the scroll position.
   *
   * This will set the "scroll-position" property and emit the ScrollPositionChanged signal.
   *
   * @param[in] position The scroll position.
   */
  void SetScrollPosition( float position );

  /**
   * @brief Retrieve the scroll position.
   * @return The scroll position.
   */
  float GetScrollPosition() const;

  /**
   * @brief Signal emitted after the SetScrollPosition() method has been called.
   */
  ScrollConnector::ScrollPositionChangedSignalType& ScrollPositionChangedSignal();

  /**
   * @brief Signal emitted after the SetScrollDomain() method has been called.
   */
  ScrollConnector::DomainChangedSignalType& DomainChangedSignal();

  /**
   * @brief Retrieve the object which provides the "scroll-position" property.
   * @return The scroll-position object.
   */
  Constrainable GetScrollPositionObject() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in] impl The Control implementation.
   */
  explicit DALI_INTERNAL ScrollConnector( Internal::ScrollConnector* impl );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_CONNECTOR_H__
