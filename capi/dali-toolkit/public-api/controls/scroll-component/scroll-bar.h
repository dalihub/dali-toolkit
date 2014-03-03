#ifndef __DALI_TOOLKIT_SCROLL_BAR_H__
#define __DALI_TOOLKIT_SCROLL_BAR_H__

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
#include <dali-toolkit/public-api/controls/scroll-component/scroll-component.h>
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
// Forward declarations

class ScrollBar;
}

/**
 * ScrollBar is a UI component that can be added to the sides of the scrollable controls
 * indicating the current scroll position.
 */
class ScrollBar : public ScrollComponent
{

public:

  /**
   * Create an uninitialized ScrollBar; this can be initialized with ScrollBar::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * or horizontally (false)
   */
  ScrollBar();

  /**
   * Copy constructor.
   */
  ScrollBar( const ScrollBar& scrollBar );

  /**
   * Assignment operator.
   */
  ScrollBar& operator=( const ScrollBar& scrollBar );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ScrollBar();

    /**
   * Create an initialized ScrollBar
   * @param[in] container Reference to the container of scroll bar
   * @param[in] vertical Whether ScrollBar should be oriented vertically (true)
   * or horizontally (false)
   * @return A pointer to the created ScrollBar.
   */
  static ScrollBar New(Scrollable& container, bool vertical);

  /**
   * Downcast an Object handle to ScrollBar. If handle points to a ScrollBar the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollBar or an uninitialized handle
   */
  static ScrollBar DownCast( BaseHandle handle );

  /**
   * Show ScrollBar
   */
  void Show();

  /**
   * Hide ScrollBar
   */
  void Hide();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ScrollBar( Internal::ScrollBar& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ScrollBar( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_BAR_H__
