#ifndef __DALI_TOOLKIT_SCROLL_COMPONENT_H__
#define __DALI_TOOLKIT_SCROLL_COMPONENT_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

class ScrollComponentImpl;
class ScrollConnector;

/**
 * Base class for scroll component handles.
 * Scroll-components such as scroll bars, indicators etc. are connected to scrollable containers via ScrollConnector.
 */
class DALI_IMPORT_API ScrollComponent : public Control
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
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ScrollComponent();

  /**
   * Downcast an Object handle to ScrollComponent. If handle points to a ScrollComponent the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollComponent or an uninitialized handle
   */
  static ScrollComponent DownCast( BaseHandle handle );

  /**
   * @brief Sets the scroll connector.
   *
   * @param[in] connector The scroll-connector used to connect with scrollable container.
   */
  void SetScrollConnector( ScrollConnector connector );

  /**
   * @brief Retrieve the scroll connector.
   *
   * @return The scroll-connector used to connect with a scrollable container.
   */
  ScrollConnector GetScrollConnector() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the implementation.
   * @param[in]  implementation The Control implementation.
   */
  DALI_INTERNAL ScrollComponent( ScrollComponentImpl& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL ScrollComponent( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCROLL_COMPONENT_H__
