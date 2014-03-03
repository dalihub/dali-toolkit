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

#include "navigation-tool-bar.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
//ToDo: use const variables instead of magic numbers

NavigationToolBar::NavigationToolBar( NavigationControl& naviControl,
                                      Toolkit::NaviToolBarStyle toolBarStylePortrait,
                                      Toolkit::NaviToolBarStyle toolBarStyleLandscape )
: NavigationBar(naviControl, toolBarStylePortrait, toolBarStyleLandscape),
  mStylePortrait( toolBarStylePortrait ),
  mStyleLandscape( toolBarStyleLandscape ),
  mCurrentStyle( &mStylePortrait),
  mNumCentralActor( 0 )
{
  // tool bar is located at the bottom of the frame
  mLayout.SetParentOrigin( Dali::ParentOrigin::BOTTOM_CENTER );
  mLayout.SetAnchorPoint( Dali::AnchorPoint::BOTTOM_CENTER );
  if(mBackground)
  {
    mBackground.SetParentOrigin( Dali::ParentOrigin::BOTTOM_CENTER );
    mBackground.SetAnchorPoint( Dali::AnchorPoint::BOTTOM_CENTER );
  }

  // layout of the left part, which contains only one control at CellPosition(1,1)
  mLeftLayout = Toolkit::TableView::New(3,3);
  mLayout.AddChild(mLeftLayout, Toolkit::TableView::CellPosition(0,0));

  // layout of the right part, which contains only one control at CellPosition(1,1)
  mRightLayout = Toolkit::TableView::New(3,3);
  mLayout.AddChild(mRightLayout, Toolkit::TableView::CellPosition(0,2));

  // layout of the central part, which contains multiples control, will add cells dynamically
  mCentralLayout = Toolkit::TableView::New(3,2);
  mLayout.AddChild(mCentralLayout, Toolkit::TableView::CellPosition(0,1));

  SetFixedSizes();
}

void NavigationToolBar::AddControl(Actor actor, Toolkit::Alignment::Type alignment)
{
  switch( alignment )
  {
    case Toolkit::Alignment::HorizontalLeft:   // can only have one control on the left side of the bar
    {
      mLeftLayout.RemoveChildAt(Toolkit::TableView::CellPosition(1,1));
      mLeftLayout.AddChild(actor, Toolkit::TableView::CellPosition(1,1));
      break;
    }
    case Toolkit::Alignment::HorizontalRight:  // can only have one control on the right side of the bar
    {
      mRightLayout.RemoveChildAt(Toolkit::TableView::CellPosition(1,1));
      mRightLayout.AddChild(actor, Toolkit::TableView::CellPosition(1,1));
      break;
    }
    case Toolkit::Alignment::HorizontalCenter: // can only have multiple controls on the central part of the bar
    {
      // already have button in central part
      if( mCentralLayout.GetChildAt(Toolkit::TableView::CellPosition(1,1)))
      {
        unsigned int columnIndex = mCentralLayout.GetColumns() ;
        mCentralLayout.InsertColumn( columnIndex-1 );
        mCentralLayout.SetFixedWidth( columnIndex-1, mCurrentStyle->centralButtonGap );

        mCentralLayout.InsertColumn( columnIndex );
        mCentralLayout.AddChild(actor, Toolkit::TableView::CellPosition( 1, columnIndex ) );
      }
      else // have no button in central part
      {
        mCentralLayout.InsertColumn( 1 );
        mCentralLayout.AddChild(actor, Toolkit::TableView::CellPosition(1,1));
      }
      mNumCentralActor++;
      break;
    }
    default:
      DALI_ASSERT_ALWAYS( false );
  }

}

void NavigationToolBar::Update( Toolkit::Page page )
{
  const Toolkit::Page::ControlOnBarContainer& controls = page.GetControlsOnToolBar();

  // if there is no control to place on the bar, hide the bar
  if( controls.empty() )
  {
    mVisible = false;
    mLayout.SetVisible(false);
    mBackground.SetVisible(false);
    return;
  }

  //clear central controls
  unsigned int numColumns = mCentralLayout.GetColumns() ;
  unsigned int idx = numColumns-2;
  while(idx > 0)
  {
    mCentralLayout.DeleteColumn(idx);
    idx--;
  }
  mNumCentralActor = 0;
  mLeftLayout.RemoveChildAt(Toolkit::TableView::CellPosition(1,1));
  mRightLayout.RemoveChildAt(Toolkit::TableView::CellPosition(1,1));

  Toolkit::Page::ControlOnBarContainer::const_iterator iter;

  for( iter = controls.begin(); iter != controls.end(); iter++ )
  {
    AddControl( (*iter)->control, (*iter)->alignment );
  }

  float buttonWidth = static_cast<float>( mCurrentStyle->centralMinimum );
  int length = mNumCentralActor * (mCurrentStyle->centralMinimum + mCurrentStyle->centralButtonGap) - mCurrentStyle->centralButtonGap;
  if(  length > mCurrentStyle->centralMaximum ) // shrink the width to make sure all the controls can be fit in
  {
    buttonWidth = static_cast<float>( mCurrentStyle->centralMaximum - (mNumCentralActor - 1) * mCurrentStyle->centralButtonGap )
                / static_cast<float>( mNumCentralActor );
  }
  numColumns = mCentralLayout.GetColumns();
  idx = 1;
  while(idx < numColumns-1 )
  {
    mCentralLayout.SetFixedWidth( idx, buttonWidth );
    idx += 2;
  }
  mVisible = true;
  mLayout.SetVisible(true);
  mBackground.SetVisible(true);
}

void NavigationToolBar::OrientationUpdate( bool isPortrait )
{
  mCurrentStyle = isPortrait ? &mStylePortrait : &mStyleLandscape;
  SetFixedSizes();
  Update( mCurrentItem );
}

void NavigationToolBar::SetFixedSizes()
{
  mLayout.SetFixedWidth(1, mCurrentStyle->centralMaximum);

  mLeftLayout.SetFixedWidth(0,mCurrentStyle->hotizontalMargin);
  mLeftLayout.SetFixedWidth(1,mCurrentStyle->sideButtonSize );
  mLeftLayout.SetFixedHeight(1,mCurrentStyle->sideButtonSize );

  mRightLayout.SetFixedWidth(2,mCurrentStyle->hotizontalMargin);
  mRightLayout.SetFixedWidth(1,mCurrentStyle->sideButtonSize );
  mRightLayout.SetFixedHeight(1,mCurrentStyle->sideButtonSize );

  mCentralLayout.SetFixedHeight(1,mCurrentStyle->centralButtonHeight );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
