#ifndef __DALI_TOOLKIT_SCROLL_BAR_INTERNAL_H__
#define __DALI_TOOLKIT_SCROLL_BAR_INTERNAL_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-component.h>
#include <dali-toolkit/public-api/controls/scrollable/scrollable.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
// Forward declarations

class ScrollBarInternal;
}

/**
 * ScrollBarInternal is a UI component that can be added to the sides of the scrollable controls
 * indicating the current scroll position.
 */
class ScrollBarInternal : public ScrollComponent
{

public:

  /**
   * Create an uninitialized ScrollBarInternal; this can be initialized with ScrollBarInternal::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * or horizontally (false)
   */
  ScrollBarInternal();

  /**
   * Copy constructor.
   */
  ScrollBarInternal( const ScrollBarInternal& scrollBar );

  /**
   * Assignment operator.
   */
  ScrollBarInternal& operator=( const ScrollBarInternal& scrollBar );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ScrollBarInternal();

    /**
   * Create an initialized ScrollBarInternal
   * @param[in] container Reference to the container of scroll bar
   * @param[in] vertical Whether ScrollBarInternal should be oriented vertically (true)
   * or horizontally (false)
   * @return A pointer to the created ScrollBarInternal.
   */
  static ScrollBarInternal New(Scrollable& container, bool vertical);

  /**
   * Downcast an Object handle to ScrollBarInternal. If handle points to a ScrollBarInternal the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollBarInternal or an uninitialized handle
   */
  static ScrollBarInternal DownCast( BaseHandle handle );

  /**
   * Show ScrollBarInternal
   */
  void Show();

  /**
   * Hide ScrollBarInternal
   */
  void Hide();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ScrollBarInternal( Internal::ScrollBarInternal& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ScrollBarInternal( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_BAR_INTERNAL_H__
