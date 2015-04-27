#ifndef __DALI_TOOLKIT_NAVIGATION_CONTROL_H__
#define __DALI_TOOLKIT_NAVIGATION_CONTROL_H__

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
#include <dali/public-api/animation/alpha-function.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>
#include <dali-toolkit/public-api/controls/navigation-frame/navigation-bar-style.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class NavigationControl;
}

/**
 * NavigationControl implements a controller than manages the navigation of hierarchical contents.
 * NavigationControl holds views as its item which are organized in a stack.
 * New items get pushed on the top of the old. Only the topmost item is displayed in the view area at one time.
 * Its layout contains a title bar on the top, a tool bar in the bottom, and the content of top item in the middle.
 * The top item carries title/subtitle/buttons/icon information,
 * so with new item on the top, the NavigationControl will update the bars accordingly.
 * if no component is needed to place on the bar for the current item, the bar is hidden
 *                 +----------------------------------------+
 *                 |                                        |
 *                 | +-+ Title                   +-+  +-+   |  title bar
 *                 | +-+ Subtitle                +-+  +-+   |
 *                 +----------------------------------------+
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |              View Area                 |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 |                                        |
 *                 +----------------------------------------+
 *                 | +-+        +-----+  +-----+        +-+ |
 *                 | +-+        +-----+  +-----+        +-+ |  tool bar
 *                 +----------------------------------------+
 *
 * Actions
 * | %Action Name      | %NavigationControl method called |
 * |-------------------|----------------------------------|
 * | push              | %PushItem()                      |
 * | pop               | %PopItem()                       |
 */
class DALI_IMPORT_API NavigationControl : public Control
{

public:

  /**
   * Create a NavigationControl handle; this can be initialize with NavigationControl::New().
   * Calling member function with an uninitialized handle is not allowed.
   */
  NavigationControl();

  /**
   * Copy Constructor.
   */
  NavigationControl( const NavigationControl& handle );

 /**
  * Assignment operator.
  */
  NavigationControl& operator=( const NavigationControl& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~NavigationControl();

  /**
   * Create an initialized NavigationControl.
   * @return A handle to a newly allocated Dali resource.
   */
  static NavigationControl New();

  /**
   * Downcast an object handle to NavigationControl.
   * If handle points to a NavigationControl, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object.
   * @return handle to a NavigationControl of an uninitialized handle.
   */
  static NavigationControl DownCast( BaseHandle handle );

  /**
   * Push a new item to the top of the NavigationControl stack and show it.
   * @param[in] item A Page object.
   */
  void PushItem( Page item );

  /**
   * Pop an item that is on the top of the NavigationControl stack and make it disappear.
   * It doesnot pop out the last item in the stack.
   * It returns an uninitialized item handle if there is no item or only one item in the stack.
   * @return The Page popped out.
   */
  Page PopItem();

  /**
   * Query the number of items in the stack.
   * @return the number of items in the stack.
   */
  std::size_t GetItemCount() const;

  /**
   * Retrieve the index-th item in the stack
   * Here, the index is from zero to stack size minus one, the bottom-most item is with index zero
   * @pre There are more items in the stack than the parameter index plus one
   * @param[in] index The location index of the item in the stack
   * @return The index-th item in the navigation stack
   */
  Page GetItem(std::size_t index) const;

  /**
   * Retrieve the current top item.
   * @return the Page object which is on the top of the stack.
   */
  Page GetCurrentItem() const;

  /**
   * Sets a background image.
   * @param[in] background Actor with the navigation control background.
   */
  void SetBackground( Actor background);

  /**
   *Create a tool bar at the bottom of the navigation control.
   *@param[in] toolBarStylePortrait the given navigation tool bar style of Portrait orientation.
   *@param[in] toolBarStyleLandscape the given navigation tool bar style of Landscape orientation.
   */
  void CreateNavigationToolBar( NaviToolBarStyle toolBarStylePortrait, NaviToolBarStyle toolBarStyleLandscape );

  /**
   * Create a title bar at the top of the navigation control.
   * @param[in] titleBarStylePortrait the given navigation title bar style of Portrait orientation.
   * @param[in] titleBarStyleLandscape the given navigation title bar style of Landscape orientation.
   */
  void CreateNavigationTitleBar( NaviTitleBarStyle titleBarStylePortrait, NaviTitleBarStyle titleBarStyleLandscape);

  /**
   * Rotate all the contents to the new given orientation. This rotation is animated.
   * Also change the bar style from portrait to landscape style, or vice versa.
   * The application should invoke this function in call back of the orientation change signal if different orientations are required.
   * @param[in] angle The angle degree of the new orientation, this is one of four discrete values, in degrees clockwise: 0, 90, 180, & 270
   */
  void OrientationChanged( int angle );

  /**
   * Set the duration and the alpha function for the rotating animation in OrientationChanged function above.
   * Without calling this function, the default values are 1.0 and EaseOut respectively.
   * @param[in] duration The duration of the rotating animation when orientation changed.
   * @param[in] alphaFunc The alpha function of the rotating animation when orientation changed.
   */
  void SetOrientationRotateAnimation( float duration, AlphaFunction alphaFunc);

public: //Signal

  typedef Signal< void( NavigationControl, Page ) > ItemPushedSignalType;
  typedef Signal< void( NavigationControl, Page ) > ItemPoppedSignalType;

  /**
   * Signal emitted right after a new item is pushed into the navigation stack.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName(NavigationControl controller, Page pushedItem);
   * @endcode
   * @return The signal to connect to.
   */
  ItemPushedSignalType& ItemPushedSignal();

  /**
   * Signal emitted right after an item is popped out from the navigation stack.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName(NavigationControl controller, Page poppedItem);
   * @endcode
   * If attempt to pop the bottom-most item, the poppedItem in the callback will receive an uninitialized handle
   * The app can use this signal and check the poppedItem to be uninitialized to know the app window should be lower
   * @return The signal to connect to.
   */
  ItemPoppedSignalType& ItemPoppedSignal();



public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL NavigationControl( Internal::NavigationControl& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL NavigationControl( Dali::Internal::CustomActor* internal );

}; // class NavigationControl

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_NAVIGATION_CONTROL_H__ */
