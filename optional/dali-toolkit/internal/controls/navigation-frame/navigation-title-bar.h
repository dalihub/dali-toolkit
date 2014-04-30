#ifndef __DALI_TOOLKIT_INTERNAL_NAVIGATION_TITLE_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_NAVIGATION_TITLE_BAR_H__

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
 * The title bar locates in the top of the frame where title, subtitle, title icon and other controls could be placed.
 * The title bar contains two groups: the left group includes title icon, title and subtitle (subtitle and title icon are not must);
 * while the right group can have multiple controls placed on, and it is also fine to have no control on it.
 * If the current NavigationOtem provides no control for the bar at all and also has not set a title, the bar is hidden.
 *               +----------------------------------------+
 *               | +-+ Title                   +-+  +-+   |
 *               | +-+ Subtitle                +-+  +-+   |
 *               +----------------------------------------+
 */
class NavigationTitleBar : public NavigationBar
{
public:

  NavigationTitleBar(NavigationControl& naviControl,
                     Toolkit::NaviTitleBarStyle titleBarStylePortrait,
                     Toolkit::NaviTitleBarStyle titleBarStyleLandscape );

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
   * Set the fixed width and height to the cells of the layout
   * These sizes need to be updated when the orientation is changing.
   */
  void SetFixedSizes();

private:

  Toolkit::NaviTitleBarStyle        mStylePortrait;
  Toolkit::NaviTitleBarStyle        mStyleLandscape;
  const Toolkit::NaviTitleBarStyle* mCurrentStyle;

  Toolkit::TableView mButtonLayout;
  Toolkit::TableView mTitleLayout;
  Toolkit::TableView mTitleIconLayout;

  Toolkit::TextView  mTitle;
  Toolkit::TextView  mSubTitle;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_INTERNAL_NAVIGATION_TITLE_BAR_H__ */
