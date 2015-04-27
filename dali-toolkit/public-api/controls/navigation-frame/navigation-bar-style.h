#ifndef __DALI_TOOLKIT_NAVIGATION_BAR_STYLE_H__
#define __DALI_TOOLKIT_NAVIGATION_BAR_STYLE_H__

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

namespace Dali
{

namespace Toolkit
{

  /**
   * The basic information of a navigation bar style: background and size.
   * All the style metrics for tool bar and title bar are in pixels referring to a bar width of 'referenceWidth'.
   * To fit the bars into different window size, the NavigationControl scales the bar by the ratio of windowWidth and referenceWidth.
   */
  struct BasicNaviBarStyle
  {
    /**
     * Constructor
     */
    BasicNaviBarStyle ( Actor background,
                    int referenceWidth,
                    int height )
     : background( background ),
       referenceWidth( referenceWidth ),
       height( height )
     {
     }

    Actor background;              ///< bar background
    int   referenceWidth;          ///< the width of the bar, this value also used to calculate the scale to fit the bar into windows of different sizes
    int   height;                  ///< the height of the bar
  };

  /**
   * The tool bar locates in the bottom of the frame where other controls (such as PushButton ) could be placed.
   * NaviToolBarStyle provides the tool bar layout style, which customize the position and size of the controls placed on it.
   * Controls could be added into three groups: HorizontalLeft, HorizontalCenter or HorizontalRight.
   * The left and right groups can only have one control maximum each, while the central group can have multiple controls.
   * It is fine to have no control in every group.
   *               +----------------------------------------+
   *               | +-+        +-----+  +-----+        +-+ |
   *               | +-+        +-----+  +-----+        +-+ |
   *               +----------------------------------------+
   */
  struct NaviToolBarStyle : BasicNaviBarStyle
  {
    /**
     * Constructor
     */
    NaviToolBarStyle ( Actor background,
                       int referenceWidth,
                       int height,
                       int centralMaximum,
                       int centralMinimum,
                       int centralButtonHeight,
                       int centralButtonGap,
                       int sideButtonSize,
                       int hotizontalMargin )
    : BasicNaviBarStyle( background, referenceWidth, height),
      centralMaximum( centralMaximum ), centralMinimum( centralMinimum ),
      centralButtonHeight( centralButtonHeight ), centralButtonGap( centralButtonGap ),
      sideButtonSize( sideButtonSize ), hotizontalMargin( hotizontalMargin )
    {
    }

    int   centralMaximum;       ///< the maximum width of central button
    int   centralMinimum;       ///< the minimum width of central button
    int   centralButtonHeight;  ///< the height of the central button
    int   centralButtonGap;     ///< the gap width between central buttons
    int   sideButtonSize;       ///< the size of side buttons in the left and right groups: sideButtonSize*sideButtonSize
    int   hotizontalMargin;     ///< the horizontal margin width
  };

  /**
   * The title bar locates in the top of the frame where title, subtitle, title icon and other controls could be placed.
   * NaviTitleBarStyle provides the title bar layout style,
   * which customize the position and size of the components placed on it and the text style of the titles.
   * The title bar contains two groups: the left group includes title icon, title and subtitle (subtitle and title icon are not must);
   * while the right group can have multiple controls placed on, and it is also fine to have no control on it.
   *               +----------------------------------------+
   *               | +-+ Title                   +-+  +-+   |
   *               | +-+ Subtitle                +-+  +-+   |
   *               +----------------------------------------+
   */
  struct NaviTitleBarStyle : BasicNaviBarStyle
  {
    /**
     * Constructor
     */
    NaviTitleBarStyle( Actor background,
                       int referenceWidth,
                       int height,
                       int titleHeightWithoutSubtitle,
                       int titleHeightWithSubtitle,
                       int subtitleHeight,
                       int titleLeftMargin,
                       int titleBottomMargin,
                       int titleIconSize,
                       int buttonSize,
                       int buttonRightMargin,
                       int buttonBottomMargin,
                       int gapBetweenButtons )
    : BasicNaviBarStyle( background, referenceWidth, height),
      titleHeightWithoutSubtitle( titleHeightWithoutSubtitle ),
      titleHeightWithSubtitle( titleHeightWithSubtitle ), subtitleHeight( subtitleHeight ),
      titleLeftMargin( titleLeftMargin ), titleBottomMargin( titleBottomMargin ),
      titleIconSize( titleIconSize ), buttonSize( buttonSize ),
      buttonRightMargin( buttonRightMargin ), buttonBottomMargin( buttonBottomMargin ),
      gapBetweenButtons( gapBetweenButtons )
    {
    }

    int   titleHeightWithoutSubtitle;       ///< the height of the title when no subtitle exists
    int   titleHeightWithSubtitle;          ///< the height of the title when there is subtitle below
    int   subtitleHeight;                   ///< the height of the subtitle
    int   titleLeftMargin;                  ///< the Margin between title and the left edge of the bar
    int   titleBottomMargin;                ///< the Margin between title and the bottom edge of the bar
    int   titleIconSize;                    ///< the size of the title icon: titleIconSize*titleIconSize
    int   buttonSize;                       ///< the size of the buttons in the right group: buttonSize*buttonSize
    int   buttonRightMargin;                ///< the Margin between the button and the right edge of the bar
    int   buttonBottomMargin;               ///< the Margin between the button and the bottom edge of the bar
    int   gapBetweenButtons;                ///< the gap width between buttons
  };

} // namespace Toolkit
} // namespace Dali


#endif /* __DALI_TOOLKIT_NAVIGATION_BAR_STYLE_H__ */
