#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_COMPONENTS_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_COMPONENTS_H__

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
#include <dali-toolkit/internal/controls/scroll-component/scroll-component.h>

#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ScrollComponent;

typedef IntrusivePtr<ScrollComponent> ScrollComponentPtr;


/**
 * Base class for derived ScrollComponents
 * ScrollComponents such as ScrollBar are derived from this class.
 * To instantiate these ScrollBars and other derived components.
 */
class ScrollComponent : public ControlImpl
{
public:

  /**
   * Create an initialized ScrollComponent
   * @param[in] scrollable reference to ScrollView implementation
   * @param[in] type the type of scroll component to create.
   * @return A pointer to the created ScrollComponent.
   */
  static Toolkit::ScrollComponent New(Toolkit::Scrollable& scrollable, Toolkit::Scrollable::ScrollComponentType type);

  /**
   * Called when the scroll component is disconnected from a Scrollable container.
   */
  virtual void OnDisconnect()
  {
  }

protected:

  /**
   * Construct a new ScrollComponent.
   */
  ScrollComponent();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollComponent();

private:

  // Undefined
  ScrollComponent(const ScrollComponent&);

  // Undefined
  ScrollComponent& operator=(const ScrollComponent& rhs);

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ScrollComponent& GetImpl(Toolkit::ScrollComponent& scrollComponent)
{
  DALI_ASSERT_ALWAYS(scrollComponent);

  Dali::RefObject& handle = scrollComponent.GetImplementation();

  return static_cast<Toolkit::Internal::ScrollComponent&>(handle);
}

inline const Toolkit::Internal::ScrollComponent& GetImpl(const Toolkit::ScrollComponent& scrollComponent)
{
  DALI_ASSERT_ALWAYS(scrollComponent);

  const Dali::RefObject& handle = scrollComponent.GetImplementation();

  return static_cast<const Toolkit::Internal::ScrollComponent&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_COMPONENTS_H__
