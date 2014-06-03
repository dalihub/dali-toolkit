#ifndef __DALI_TOOLKIT_INTERNAL_NAVIGATION_TOOL_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_NAVIGATION_TOOL_BAR_H__

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

#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/public-api/controls/text-view/text-view.h>
#include <dali-toolkit/internal/controls/navigation-frame/navigation-control-impl.h>
#include <dali-toolkit/internal/controls/navigation-frame/navigation-bar.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The tool bar locates in the bottom of the frame where other controls (such as PushButton ) could be placed.
 * Controls could be added into three groups: HorizontalLeft, HorizontalCenter or HorizontalRight.
 * The left and right groups can only have one control maximum each, while the central group can have multiple controls.
 * It is also fine to have no control in each group.
 * If the current NavigationOtem provides no control for the bar at all, the bar is hidden.
 *               +----------------------------------------+
 *               | +-+        +-----+  +-----+        +-+ |
 *               | +-+        +-----+  +-----+        +-+ |
 *               +----------------------------------------+
 */
class NavigationToolBar : public NavigationBar
{
public:

  NavigationToolBar(NavigationControl& naviControl,
                    Toolkit::NaviToolBarStyle toolBarStylePortrait,
                    Toolkit::NaviToolBarStyle toolBarStyleLandscape );

protected:

  /**
   * @copydoc Toolkit::Internal::NavigationBar::Update
   */
  void Update( Toolkit::Page page );

  /**
   * @copydoc Toolkit::Internal::NavigationBar::OrientationUpdate
   */
  void OrientationUpdate( bool isPortrait );

private:

  /**
   * Set a control onto the navigation tool bar when the page is on the top.
   * @param[in] control The control on the navigation tool bar.
   * @param[in] alignment The position of the control, can be HorizontalLeft/HorizontalRight/HorizontalCenter.
   */
  void AddControl(Actor actor, Toolkit::Alignment::Type alignment);

  /**
   * Set the fixed width and height to the cells of the layout
   * These sizes need to be updated when the orientation is changing.
   */
  void SetFixedSizes();

private:

  Toolkit::NaviToolBarStyle         mStylePortrait;
  Toolkit::NaviToolBarStyle         mStyleLandscape;
  const Toolkit::NaviToolBarStyle*  mCurrentStyle;

  Toolkit::TableView                mLeftLayout;
  Toolkit::TableView                mRightLayout;
  Toolkit::TableView                mCentralLayout;

  int                               mNumCentralActor;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_NAVIGATION_TOOL_BAR_H__ */
