#ifndef DALI_TOOLKIT_NAVIGATION_VIEW_H
#define DALI_TOOLKIT_NAVIGATION_VIEW_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/alpha-function.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class NavigationView;
}

/**
 * @brief NavigationView implements a view that manages the navigation of hierarchical contents.
 *
 * An Actor is pushed onto the NavigationView, itself and its children are added to the stage.
 * The actors currently shown are replaced.
 * When pop is called on the NavigationView, the current tree of Actors are removed and the previous set added back.
 * If pop is called on the last set of Actors then they remain, nothing is popped.
 */
class DALI_IMPORT_API NavigationView : public Control
{

public:

  /**
   * @brief Create a NavigationView handle; this can be initialize with NavigationView::New().
   *
   * @note Calling member function with an uninitialized handle is not allowed.
   */
  NavigationView();

  /**
   * @brief Copy Constructor.
   * @param[in] handle Handle to copy.
   */
  NavigationView( const NavigationView& handle );

 /**
  * @brief Assignment operator.
  * @param handle The handle to copy from.
  * @return reference to this
  */
  NavigationView& operator=( const NavigationView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~NavigationView();

  /**
   * @brief  Create an initialized NavigationView.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static NavigationView New();

  /**
   * @brief Downcast an object handle to NavigationView.
   *
   * @details If handle points to a NavigationView, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object.
   * @return handle to a NavigationView of an uninitialized handle.
   */
  static NavigationView DownCast( BaseHandle handle );

  /**
   * @brief Push a new actor tree to the top of the NavigationView stack and show it.
   * @param[in] item An actor tree.
   */
  void Push( Actor item );

  /**
   * @brief Pop the actor tree that is on the top of the NavigationView stack and make it disappear.
   *
   * @return The Actor tree popped out.
   *
   * @note It does not pop out the last item in the stack.
   * It returns an uninitialized item handle if there is no item or only one item in the stack.
   */
  Actor Pop();


public: // Not intended for application developers

/// @cond internal
  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL NavigationView( Internal::NavigationView& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL NavigationView( Dali::Internal::CustomActor* internal );
/// @endcond
}; // class NavigationView

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NAVIGATION_VIEW_H
