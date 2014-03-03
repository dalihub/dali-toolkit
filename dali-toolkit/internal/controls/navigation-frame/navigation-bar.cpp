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

// CLASS HEADER
#include "navigation-bar.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

NavigationBar::NavigationBar(NavigationControl& naviControl,
                             Toolkit::BasicNaviBarStyle barStylePortrait,
                             Toolkit::BasicNaviBarStyle barStyleLandscape )
:mInternalNavigationControl( naviControl ),
 mBasicStylePortrait( barStylePortrait ),
 mBasicStyleLandscape( barStyleLandscape ),
 mBasicCurrentStyle( &mBasicStylePortrait ),
 mIsPortrait( true ),
 mVisible( true )
{
  mInternalNavigationControl.ItemPushedSignal().Connect( this, &NavigationBar::OnItemPushed );
  mInternalNavigationControl.ItemPoppedSignal().Connect( this, &NavigationBar::OnItemPopped );

  mLayout = Toolkit::TableView::New(1,3);
  mInternalNavigationControl.GetBarLayer().Add(mLayout);
  mLayout.SetSize(mBasicCurrentStyle->referenceWidth, mBasicCurrentStyle->height);
  mLayout.SetFixedHeight(0, mBasicCurrentStyle->height);
  mLayout.SetDrawMode(DrawMode::OVERLAY);

  SetBackground( mBasicCurrentStyle->background );
}

NavigationBar::~NavigationBar()
{
}

void NavigationBar::ScaleStyleUpdate( Vector2 naviControlSize, int orientation )
{
  bool isPortrait( orientation == 0 || orientation == 180 );
  // change in orientation.
  if(mIsPortrait != isPortrait)
  {
    mIsPortrait = isPortrait;
    mBasicCurrentStyle = isPortrait ? &mBasicStylePortrait : &mBasicStyleLandscape;
    OrientationUpdate( isPortrait );
    mLayout.SetSize(mBasicCurrentStyle->referenceWidth, mBasicCurrentStyle->height);
    mLayout.SetFixedHeight(0, mBasicCurrentStyle->height);
    if(mBackground)
    {
      mBackground.SetSize(mBasicCurrentStyle->referenceWidth, mBasicCurrentStyle->height);
    }
  }

  mRelativeScale = naviControlSize.x / static_cast<float>( mBasicCurrentStyle->referenceWidth);
  mLayout.SetScale(mRelativeScale);
  mBarHeight = mBasicCurrentStyle->height * mRelativeScale;
  if(mBackground)
  {
    mBackground.SetScale(mRelativeScale);
  }
}

float NavigationBar::GetBarHeight() const
{
  if( mVisible )
  {
    return mBarHeight;
  }
  else
  {
    return 0.0f;
  }
}

void NavigationBar::SetBackground( Actor background )
{
  mBackground = background;
  mBackground.SetSize(mBasicCurrentStyle->referenceWidth, mBasicCurrentStyle->height);
  mInternalNavigationControl.GetBarLayer().Add( mBackground );
  mBackground.SetScale(mRelativeScale);
}

void NavigationBar::OnItemPushed( Toolkit::NavigationControl naviControl, Toolkit::Page naviItem )
{
  mCurrentItem = naviItem;
  Update( mCurrentItem );
}

void NavigationBar::OnItemPopped( Toolkit::NavigationControl naviControl, Toolkit::Page naviItem )
{
  mCurrentItem = mInternalNavigationControl.GetCurrentItem();
  Update( mCurrentItem );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
