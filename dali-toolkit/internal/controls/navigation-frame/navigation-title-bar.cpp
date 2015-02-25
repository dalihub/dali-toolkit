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

// CLASS HEADER
#include "navigation-title-bar.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

//ToDo: use const variables instead of magic numbers

NavigationTitleBar::NavigationTitleBar(NavigationControl& naviControl,
                                       Toolkit::NaviTitleBarStyle titleBarStylePortrait,
                                       Toolkit::NaviTitleBarStyle titleBarStyleLandscape )
: NavigationBar(naviControl, titleBarStylePortrait, titleBarStyleLandscape ),
  mStylePortrait( titleBarStylePortrait ),
  mStyleLandscape( titleBarStyleLandscape ),
  mCurrentStyle( &mStylePortrait )
{
  // title bar is located at the top of the frame
  mLayout.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
  mLayout.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  if(mBackground)
  {
    mBackground.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
    mBackground.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  }

  // button layout: three rows, controls will be put in the middle row, the top and bottom rows are just for margins
  mButtonLayout = Toolkit::TableView::New(3, 1);
  // title layout: fours rows, the top and bottom rows are just for margins
  // if subtitle exists, title in the second row and subtitle in the third row
  // if no subtitle, title will occupy both second and third row
  mTitleLayout= Toolkit::TableView::New( 4,1 );
  // title icon layout: the top row, the bottom row and the left column are all for margins
  mTitleIconLayout= Toolkit::TableView::New( 3,2 );
  SetFixedSizes();
}

void NavigationTitleBar::Update( Toolkit::Page page )
{
  const std::vector<Actor>& controls = page.GetControlsOnTitleBar();

  // if there is no control to place on the bar ano no title is set, hide the bar
  if(controls.empty() && page.GetTitle().empty())
  {
    mVisible = false;
    mLayout.SetVisible(false);
    mBackground.SetVisible(false);
    return;
  }

  if(mLayout.GetColumns() == 4)// | leftMargin | titleLayout(may have icon and subtitle) | buttonLayout | rightMargin |
  {
    //remove buttonLayout
    mLayout.DeleteColumn(2);
  }
  // remove titleLayout
  mLayout.RemoveChildAt( Toolkit::TableView::CellPosition(0,1) );
  //Remove the controls in the buttonLayout
  mButtonLayout.Resize(3,1);
  //remove titleIcon
  if(mTitleLayout.GetColumns() == 2)
  {
    mTitleLayout.DeleteColumn( 0 );
  }
  // remove title and subtitle
  mTitleLayout.RemoveChildAt( Toolkit::TableView::CellPosition(2,0) );
  mTitleLayout.RemoveChildAt( Toolkit::TableView::CellPosition(1,0) );

  // add controls at the right part of the bar(if exist)
  if(!controls.empty())
  {
    int numControls = controls.size();

    for( int index = 0; index < numControls; index++)
    {
      mButtonLayout.AddChild( controls[index], Toolkit::TableView::CellPosition(1, 2*index + 1) );
      mButtonLayout.SetFixedWidth (2*index, mCurrentStyle->gapBetweenButtons);
      mButtonLayout.SetFixedWidth (2*index+1, mCurrentStyle->buttonSize);
    }

    mLayout.InsertColumn(2);
    mLayout.SetFixedWidth(2, numControls * ( mCurrentStyle->buttonSize + mCurrentStyle->gapBetweenButtons) );
    mLayout.AddChild(mButtonLayout, Toolkit::TableView::CellPosition(0,2));
  }

  // insert title icon to the left of the title(if exist)
  if( page.GetTitleIcon() )
  {
    mTitleIconLayout.RemoveChildAt(Toolkit::TableView::CellPosition(1,0) );
    mTitleIconLayout.AddChild( page.GetTitleIcon(), Toolkit::TableView::CellPosition(1,0) );
    mTitleLayout.InsertColumn( 0 );
    mTitleLayout.SetFixedWidth( 0, mCurrentStyle->titleLeftMargin + mCurrentStyle->titleIconSize);
    mTitleLayout.AddChild( mTitleIconLayout, Toolkit::TableView::CellPosition(1,0,2,1) );
  }

  mLayout.AddChild( mTitleLayout, Toolkit::TableView::CellPosition(0,1) );

  mVisible = true;
  mLayout.SetVisible(true);
  mBackground.SetVisible(true);
}

void NavigationTitleBar::OrientationUpdate( bool isPortrait )
{
  mCurrentStyle = isPortrait ? &mStylePortrait : &mStyleLandscape;
  SetFixedSizes();
  Update( mCurrentItem );
}

void NavigationTitleBar::SetFixedSizes()
{
  mLayout.SetFixedWidth(0, mCurrentStyle->titleLeftMargin);
  mLayout.SetFixedWidth(2, mCurrentStyle->buttonRightMargin);

  mButtonLayout.SetFixedHeight(2, mCurrentStyle->buttonBottomMargin);
  mButtonLayout.SetFixedHeight(1, mCurrentStyle->buttonSize);

  mTitleLayout.SetFixedHeight( 3,mCurrentStyle->titleBottomMargin );
  mTitleLayout.SetFixedHeight( 2,mCurrentStyle->subtitleHeight );

  mTitleIconLayout.SetFixedWidth( 0, mCurrentStyle->titleIconSize );
  mTitleIconLayout.SetFixedHeight( 1, mCurrentStyle->titleIconSize );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
