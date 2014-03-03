#ifndef __DALI_TOOLKIT_SCROLL_COMPONENT_H__
#define __DALI_TOOLKIT_SCROLL_COMPONENT_H__

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
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollComponent;
}

/**
 * Base class for derived ScrollComponents
 * ScrollComponents such as ScrollBar are derived from this class.
 * To instantiate these ScrollBars and other derived components
 */
class ScrollComponent : public Control
{

public:
  /**
   * Create an uninitialized ScrollComponent; this can be initialized with ScrollComponent::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ScrollComponent();

  /**
   * Copy constructor.
   */
  ScrollComponent( const ScrollComponent& scrollComponent );

  /**
   * Assignment operator.
   */
  ScrollComponent& operator=( const ScrollComponent& scrollComponent );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ScrollComponent();

  /**
   * Downcast an Object handle to ScrollComponent. If handle points to a ScrollComponent the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollComponent or an uninitialized handle
   */
  static ScrollComponent DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ScrollComponent( Internal::ScrollComponent& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ScrollComponent( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_COMPONENT_H__
