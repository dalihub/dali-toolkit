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
#include "navigation-control-impl.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/size-negotiation/relayout-container.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/focus-manager.h>
#include <dali-toolkit/internal/controls/navigation-frame/navigation-tool-bar.h>
#include <dali-toolkit/internal/controls/navigation-frame/navigation-title-bar.h>
#include <dali-toolkit/public-api/focus-manager/focus-manager.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // to register type
{

BaseHandle Create()
{
  return Toolkit::NavigationControl::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::NavigationControl, Toolkit::Control, Create )

DALI_ACTION_REGISTRATION( Toolkit, NavigationControl, "push", ACTION_PUSH )
DALI_ACTION_REGISTRATION( Toolkit, NavigationControl, "pop",  ACTION_POP  )

DALI_TYPE_REGISTRATION_END()

}

NavigationControl::NavigationControl()
: Control( ControlBehaviour( REQUIRES_TOUCH_EVENTS ) ),
  mToolBar(NULL),
  mTitleBar(NULL),
  mOrientationAngle( 0 ),
  mOrientationAnimationDuration( 1.0f ),
  mOrientationAnimationAlphaFunc( AlphaFunction::EASE_OUT ),
  mItemPositionCoefficient( Vector3( 0.0f, 1.0f, 0.0f) ),
  mItemPushedSignal( ),
  mItemPoppedSignal( )
{
}

NavigationControl::~NavigationControl()
{
  // Clear all the items in the stack, forces their destruction before NavigationControl is destroyed.
  mItemStack.clear();
}

void NavigationControl::OnInitialize()
{
  //create layers for display background, current item, and bars respectively
  mBackgroundLayer = CreateLayer();
  mContentLayer = CreateLayer();
  mBarLayer = CreateLayer();
  mPopupLayer = CreateLayer();
}

void NavigationControl::OnControlChildAdd( Actor& child )
{
  Toolkit::Page page = Toolkit::Page::DownCast(child);

  // If it's a page then store it locally, Off stage.
  if(page)
  {
    mUnpushedItems.push_back(page);

    // Orphan it until needed later during "push".
    Self().Remove( child );
  }
}

Toolkit::NavigationControl NavigationControl::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< NavigationControl > internalNavigationControl = new NavigationControl();

  // Pass ownership to CustomActor handle
  Toolkit::NavigationControl navigationControl( *internalNavigationControl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalNavigationControl->Initialize();

  return navigationControl;
}

void NavigationControl::OnStageConnection()
{
  //only works when navigation control is already on stage!
  mContentLayer.RaiseAbove( mBackgroundLayer );
  mBarLayer.RaiseAbove( mContentLayer );
  mPopupLayer.RaiseAbove( mBarLayer );
  Self().SetSensitive(true);
  SetKeyInputFocus();
}

void NavigationControl::PushItem( Toolkit::Page page )
{
  // check the uninitialized item
  // check the duplicated push for the top item
  if(!page || page == mCurrentItem)
  {
    return;
  }

  if( mCurrentItem )
  {
    mContentLayer.Remove( mCurrentItem );
  }

  //push the new item into the stack and show it
  mItemStack.push_back(page);
  mCurrentItem = page;
  mContentLayer.Add(page);
  mCurrentItem.SetPositionInheritanceMode(USE_PARENT_POSITION_PLUS_LOCAL_POSITION);

  //set up the popup menu which would response to the KEY_MENU
  SetupPopupMenu();

  //Emit singal
  Toolkit::NavigationControl handle( GetOwner() );
  mItemPushedSignal.Emit(handle, page);
}

Toolkit::Page NavigationControl::PopItem()
{
  // cannot pop out the bottom-most item
  Toolkit::Page poppedItem;
  if(mItemStack.size() > 1)
  {
    // pop out the top item of the stack and show the new item right under the old one.
    mContentLayer.Remove(mCurrentItem);
    poppedItem = mItemStack.back();
    mItemStack.pop_back();
    mCurrentItem = mItemStack.back();
    mContentLayer.Add(mCurrentItem);

    //set up the popup menu which would response to the KEY_MENU
    SetupPopupMenu();
  }

  //Emit signal
  Toolkit::NavigationControl handle( GetOwner() );
  mItemPoppedSignal.Emit(handle, poppedItem);

  return poppedItem;
}

size_t NavigationControl::GetItemCount() const
{
  return mItemStack.size();
}

Toolkit::Page NavigationControl::GetItem(std::size_t index) const
{
  DALI_ASSERT_ALWAYS( index < mItemStack.size() );
  return mItemStack[index];
}

Toolkit::Page NavigationControl::GetCurrentItem() const
{
  return mCurrentItem;
}

void NavigationControl::SetBackground( Actor background)
{
  // It removes the old background
  if( mBackground )
  {
    mBackgroundLayer.Remove( mBackground );
  }
  mBackgroundLayer.Add( background );
  mBackground = background;
  mBackground.SetSize( mControlSize );
}

void NavigationControl::CreateNavigationToolBar(Toolkit::NaviToolBarStyle toolBarStylePortrait,
                                                Toolkit::NaviToolBarStyle toolBarStyleLandscape )
{
  // Set a navigation tool bar at the bottom of the navigation frame
  // the controls on the tool bar will update automatically when item is pushed or popped by responding to the signals
  mToolBar = new NavigationToolBar(*this, toolBarStylePortrait, toolBarStyleLandscape);
}

void NavigationControl::CreateNavigationTitleBar(Toolkit::NaviTitleBarStyle titleBarStylePortrait,
                                                 Toolkit::NaviTitleBarStyle titleBarStyleLandscape)
{
  // Set a navigation title bar at the top of the navigation frame
  // the tile/subtitle/titl icon/buttons will update automatically when item is pushed or popped by responding to the signals
  mTitleBar = new NavigationTitleBar(*this, titleBarStylePortrait, titleBarStyleLandscape);
}

void NavigationControl::OrientationChanged( int angle )
{
  if( mOrientationAngle != angle )
  {
    Vector2 targetSize = Vector2(GetSizeSet());

    // checking to see if changing from landscape -> portrait, or portrait -> landscape
    if( mOrientationAngle%180 != angle%180 )
    {
      targetSize = Vector2( targetSize.height, targetSize.width );
    }

    mOrientationAngle = angle;

    switch(angle)
    {
      case 0:
      {
        mItemPositionCoefficient = Vector3(0.0f, 1.0f, 0.0f);
        break;
      }
      case 90:
      {
        mItemPositionCoefficient = Vector3(1.0f, 0.0f, 0.0f);
        break;
      }
      case 180:
      {
        mItemPositionCoefficient = Vector3(0.0f, -1.0f, 0.0f);
        break;
      }
      case 270:
      {
        mItemPositionCoefficient = Vector3(-1.0f, 0.0f, 0.0f);
        break;
      }
      default:
      {
        DALI_ASSERT_ALWAYS(false);
        break;
      }
    }

    Actor self = Self();
    Animation animation = Animation::New( mOrientationAnimationDuration );
    animation.AnimateTo( Property( self, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( -angle ) ), Vector3::ZAXIS ), mOrientationAnimationAlphaFunc );
    animation.Play();

    self.SetSize( targetSize );

    RelayoutRequest();
  }
}

void NavigationControl::SetOrientationRotateAnimation( float duration, AlphaFunction alphaFunc)
{
  mOrientationAnimationDuration = duration;
  mOrientationAnimationAlphaFunc = alphaFunc;
}

Layer NavigationControl::GetBarLayer() const
{
  return mBarLayer;
}

void NavigationControl::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  const Vector2 setSize( size );

  if( mCurrentItem )
  {
    // always set the current item to fully occupy navigationControl space apart from the bars,
    // here the bars might be hidden if the current item does not need them
    float positionOffset = 0.0f;
    float sizeShrink = 0.0f;
    if(mTitleBar)
    {
      positionOffset += mTitleBar->GetBarHeight()*0.5f;
      sizeShrink += mTitleBar->GetBarHeight();
    }
    if(mToolBar)
    {
      positionOffset -= mToolBar->GetBarHeight()*0.5f;
      sizeShrink += mToolBar->GetBarHeight();
    }
    mCurrentItem.SetPosition( mItemPositionCoefficient * positionOffset);
    Vector2 itemSize( setSize.x, setSize.y-sizeShrink );

    container.Add( mCurrentItem, itemSize );
  }

  container.Add( mBarLayer, setSize );
  container.Add( mPopupLayer, setSize );
}

void NavigationControl::OnControlSizeSet( const Vector3& size )
{
  if( mControlSize == Vector2(size) )
  {
    return;
  }
  mControlSize = Vector2(size);

  mBarLayer.SetSize(mControlSize);
  mPopupLayer.SetSize(mControlSize);

  if( mBackground )
  {
    mBackground.SetSize( mControlSize );
  }
  if( mToolBar )
  {
    mToolBar->ScaleStyleUpdate( mControlSize, mOrientationAngle );
  }
  if( mTitleBar )
  {
    mTitleBar->ScaleStyleUpdate( mControlSize, mOrientationAngle );
  }
}

bool NavigationControl::OnKeyEvent( const KeyEvent& event )
{
  bool consumed = false;

  if(event.state == KeyEvent::Down)
  {
    if(event.keyCode == 96 ) // F12 == for test
    //if( event.keyCode == Dali::DALI_KEY_BACK || event.keyCode == Dali::DALI_KEY_ESCAPE )
    {
      if( mPopupMenu && mPopupMenu.IsSensitive() ) // State:POPUP_SHOW
      {
        mPopupMenu.Hide();
        consumed = true;
      }
      else if(PopItem())
      {
        consumed = true;
      }
    }

    if( mPopupMenu && event.keyCode == 9)
    //if( mPopupMenu && ( event.keyCode == Dali::DALI_KEY_MENU  || event.keyCode == Dali::DALI_KEY_SEND ) )
    //Todo: replace with dali key enum after the mapping between X key definition and dali key enum is implemented in dali-adapto
    //if( mPopupMenu && event.keyPressedName == "XF86Send" )
    {
      if( !mPopupMenu.IsSensitive() ) // State: POPUP_HIDE
      {
        mPopupMenu.Show();
      }
      else // State:POPUP_SHOW
      {
        mPopupMenu.Hide();
      }
      consumed = true;
    }
  }

  return consumed;
}

Layer NavigationControl::CreateLayer()
{
  Layer layer = Layer::New();
  layer.SetPositionInheritanceMode(USE_PARENT_POSITION);
  Self().Add(layer);
  return layer;
}

void NavigationControl::SetupPopupMenu()
{
  if(mPopupMenu)
  {
    mPopupLayer.Remove( mPopupMenu );
  }
  mPopupMenu = mCurrentItem.GetPopupMenu();
  if( mPopupMenu )
  {
    mPopupLayer.Add( mPopupMenu );
    mPopupMenu.OutsideTouchedSignal().Connect(this, &NavigationControl::OnPopupTouchedOutside);
  }
}

void NavigationControl::OnPopupTouchedOutside()
{
  if( mPopupMenu )
  {
    mPopupMenu.Hide();
  }
}

Toolkit::NavigationControl::ItemPushedSignalType& NavigationControl::ItemPushedSignal()
{
  return mItemPushedSignal;
}

Toolkit::NavigationControl::ItemPoppedSignalType& NavigationControl::ItemPoppedSignal()
{
  return mItemPoppedSignal;
}

bool NavigationControl::DoAction( BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes )
{
  bool ret = false;

  Dali::BaseHandle handle( object );
  Toolkit::NavigationControl control = Toolkit::NavigationControl::DownCast( handle );
  DALI_ASSERT_ALWAYS( control );

  if( 0 == strcmp( actionName.c_str(), ACTION_PUSH ) )
  {
    for( PropertyValueConstIter iter = attributes.begin(); iter != attributes.end(); ++iter )
    {
      const Property::Value& value = *iter;

      DALI_ASSERT_ALWAYS( value.GetType() == Property::STRING );
      std::string itemName = value.Get<std::string>();

      for( std::list<Toolkit::Page>::iterator itemsIter = GetImpl( control ).mUnpushedItems.begin(); itemsIter != GetImpl( control ).mUnpushedItems.end(); ++itemsIter )
      {
        Toolkit::Page page = *itemsIter;
        if( page.GetName() == itemName )
        {
          GetImpl( control ).PushItem( page );
          ret = true;
          break;
        }
      }
    }
  }
  else if( 0 == strcmp( actionName.c_str(), ACTION_POP ) )
  {
    GetImpl( control ).PopItem();

    ret = true;
  }

  return ret;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
