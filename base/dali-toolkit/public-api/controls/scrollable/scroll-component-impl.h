#ifndef __DALI_TOOLKIT_SCROLL_COMPONENT_IMPL_H__
#define __DALI_TOOLKIT_SCROLL_COMPONENT_IMPL_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-component.h>

#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-connector.h>

namespace Dali
{

namespace Toolkit
{

class ScrollComponentImpl;
typedef IntrusivePtr<ScrollComponentImpl> ScrollComponentPtr;

/**
 * Base class for scroll component implementations.
 * Scroll-components such as scroll bars, indicators etc. are connected to scrollable containers via ScrollConnector.
 */
class ScrollComponentImpl : public Internal::Control
{
public:

  /**
   * @brief Sets the scroll connector.
   *
   * @param[in] connector The scroll-connector used to connect with a scrollable container.
   */
  void SetScrollConnector( Toolkit::ScrollConnector connector );

  /**
   * @brief Retrieve the scroll connector.
   *
   * @return The scroll-connector used to connect with a scrollable container.
   */
  Toolkit::ScrollConnector GetScrollConnector() const;

  /**
   * Called when the scroll component is disconnected from a scrollable container.
   */
  virtual void OnDisconnect() {}

protected:

  /**
   * Construct a new ScrollComponentImpl.
   */
  ScrollComponentImpl();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollComponentImpl();

  /**
   * @brief Called when a scroll component is connected to a scrollable UI control.
   *
   * @param[in] oldConnector The previous connector, or an empty handle.
   */
  virtual void OnScrollConnectorSet( Toolkit::ScrollConnector oldConnector );

private:

  // Undefined
  ScrollComponentImpl(const ScrollComponentImpl&);

  // Undefined
  ScrollComponentImpl& operator=(const ScrollComponentImpl& rhs);

protected:

  Toolkit::ScrollConnector mScrollConnector;
};

// Helpers for public-api forwarding methods

inline Toolkit::ScrollComponentImpl& GetImpl(Toolkit::ScrollComponent& scrollComponent)
{
  DALI_ASSERT_ALWAYS(scrollComponent);

  Dali::RefObject& handle = scrollComponent.GetImplementation();

  return static_cast<Toolkit::ScrollComponentImpl&>(handle);
}

inline const Toolkit::ScrollComponentImpl& GetImpl(const Toolkit::ScrollComponent& scrollComponent)
{
  DALI_ASSERT_ALWAYS(scrollComponent);

  const Dali::RefObject& handle = scrollComponent.GetImplementation();

  return static_cast<const Toolkit::ScrollComponentImpl&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_COMPONENT_IMPL_H__
