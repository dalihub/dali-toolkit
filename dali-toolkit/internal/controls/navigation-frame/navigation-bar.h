#ifndef __DALI_TOOLKIT_INTERNAL_NAVIGATION_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_NAVIGATION_BAR_H__

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>
#include <dali-toolkit/public-api/controls/navigation-frame/navigation-bar-style.h>
#include <dali-toolkit/internal/controls/navigation-frame/navigation-control-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Base class for different navigation bars such as tool bar, title bar.
 */
class NavigationBar : public Dali::RefObject, public ConnectionTracker
{

public:
  /**
   * Constructor
   * Pass in the navigationControl with which the bar associates and its style.
   */
  NavigationBar( NavigationControl& naviControl,
                 Toolkit::BasicNaviBarStyle barStylePortrait,
                 Toolkit::BasicNaviBarStyle barStyleLandscape);

  /**
   * Update the bar scale when the size of the navigation control is set / reset
   * Also Update the style when the orientation( portrait/landscape) is changed
   * @param[in] naviControlSize The size of the navigation control
   * @param[in] orientation The angle of the current orientation
   */
  void ScaleStyleUpdate( Vector2 naviControlSize, int orientation );

  /**
   * Retrieve the height of the bar
   * @return The height of the bar
   */
  float GetBarHeight() const;

private:

  /**
   * Set a background image and add it onto the NavigaionControl's bar layer.
   *
   */
  void SetBackground( Actor background );

  /**
   * Call the update function when it receives the page pushed signal.
   */
  void OnItemPushed( Toolkit::NavigationControl naviControl, Toolkit::Page naviItem );

  /**
   * Call the update function when it receives the page popped signal.
   */
  void OnItemPopped( Toolkit::NavigationControl naviControl, Toolkit::Page naviItem );

protected:

  /**
   * virtual destructor
   */
  virtual ~NavigationBar();

  /**
   * Given the current page, update the bar content.
   * @param[in] naviItem the item on the top of the navigation stack
   */
  virtual void Update( Toolkit::Page naviItem ) = 0;

  /**
   * update the bar style when the orientation is changed
   * @param[in] isPortrait Whether the current orientation is portrait mode
   */
  virtual void OrientationUpdate( bool isPortrait ) = 0;

private:

  // Undefined
  NavigationBar(const NavigationBar&);

  // Undefined
  NavigationBar& operator=(const NavigationBar& rhs);


protected:
  NavigationControl&                 mInternalNavigationControl;
  Toolkit::BasicNaviBarStyle         mBasicStylePortrait;
  Toolkit::BasicNaviBarStyle         mBasicStyleLandscape;
  const Toolkit::BasicNaviBarStyle*  mBasicCurrentStyle;
  float                              mRelativeScale;
  float                              mBarHeight;

  Toolkit::TableView                 mLayout;
  Actor                              mBackground;

  bool                               mIsPortrait;
  Toolkit::Page            mCurrentItem;

  bool                               mVisible;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_INTERNAL_NAVIGATION_BAR_H__ */
