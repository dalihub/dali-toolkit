#ifndef __DALI_TOOLKIT_INTERNAL_NAVIGATION_CONTROL_H__
#define __DALI_TOOLKIT_INTERNAL_NAVIGATION_CONTROL_H__

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
#include <list>
#include <dali/public-api/actors/layer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/navigation-frame/navigation-control.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>
#include <dali-toolkit/internal/controls/navigation-frame/navigation-bar.h>
#include <dali-toolkit/public-api/controls/popup/popup.h>

namespace Dali
{

namespace Toolkit
{

class NavigationControl;
class Page;

namespace Internal
{

class NavigationBar;

/**
 * NavigationControl implements a controller than manages the navigation of hierarchical contents.
 * @see Dali::Toolkit::NavigationControl for more details.
 */
class NavigationControl : public Control
{
public:

  /**
   * Create an initialized NavigationControl.
   * @return A handle to a newly allocated Dali resource
   */
  static Toolkit::NavigationControl New();

  /**
   * @copydoc Dali::Toolkit::NavigationControl::PushItem()
   */
  void PushItem( Toolkit::Page page );

  /**
   * @copydoc Dali::Toolkit::NavigationControl::PopItem()
   */
  Toolkit::Page PopItem();

  /**
   * @copydoc Dali::Toolkit::NavigationControl::GetItemCount()
   */
  size_t GetItemCount() const;

  /**
   * @copydoc Dali::Toolkit::NavigationControl::GetItem()
   */
  Toolkit::Page GetItem(std::size_t index) const;

  /**
   * @copydoc Dali::Toolkit::NavigationControl::GetCurrentItem()
   */
  Toolkit::Page GetCurrentItem() const;

  /**
   * @copydoc Dali::Toolkit::NavigationControl::SetBackground()
   */
  void SetBackground( Actor background);

  /**
   * @copydoc Dali::Toolkit::NavigationControl::CreateNavigationToolBar()
   */
  void CreateNavigationToolBar( Toolkit::NaviToolBarStyle toolBarStylePortrait,
                                Toolkit::NaviToolBarStyle toolBarStyleLandscape );

  /**
   * @copydoc Dali::Toolkit::NavigationControl::CreateNavigationTitleBar()
   */
  void CreateNavigationTitleBar( Toolkit::NaviTitleBarStyle titleBarStylePortrait,
                                 Toolkit::NaviTitleBarStyle titleBarStyleLandscape );

  /**
   * @copydoc Dali::Toolkit::NavigationControl::OrientationChanged()
   */
  void OrientationChanged( int angle );

  /**
   * @copydoc Dali::Toolkit::NavigationControl::SetOrientationRotateAnimation()
   */
  void SetOrientationRotateAnimation( float duration, AlphaFunction alphaFunc);

  /**
   * Retrieve the layer for displaying navigation bar
   * @return The layer for navigation bar
   */
  Layer GetBarLayer() const;

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes);

public:

  /**
   * @copydoc Dali::Toolkit::NavigatinControl::ItemPushedSignal()
   */
  Toolkit::NavigationControl::ItemPushedSignalType& ItemPushedSignal();

  /**
   * @copydoc Dali::Toolkit::NavigatinControl::ItemPoppedSignal()
   */
  Toolkit::NavigationControl::ItemPoppedSignalType& ItemPoppedSignal();

private: // override functions from Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * From Control; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  virtual void OnControlChildAdd( Actor& child );

  /**
   * @copydoc Control::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @copydoc Control::OnRelaidOut()
   */
  virtual void OnRelaidOut( Vector2 size, ActorSizeContainer& container );

  /**
   * @copydoc Control::OnControlSizeSet
   */
  virtual void OnControlSizeSet( const Vector3& size );

  /**
   * @copydoc Control::OnKeyEvent()
   */
  virtual bool OnKeyEvent( const KeyEvent& event );

protected:

  /**
   * Constructor.
   * It initializes the NavigationControl members
   */
  NavigationControl();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NavigationControl();

private:

  // Undefined
  NavigationControl(const NavigationControl&);

  // Undefined
  NavigationControl& operator=(const NavigationControl& rhs);

  /**
   * Create a Layer and add it to the navigation control
   * @return The newly created layer
   */
  Layer CreateLayer();

  /**
   * Setup the pop up menu which would show when KEY_MENU is pressed
   * This function is called when pushing/popping item
   */
  void SetupPopupMenu();

  /**
   * Signal handler called when the user touches outside of pop up menu.
   */
  void OnPopupTouchedOutside();

public:
  std::list< Toolkit::Page >           mUnpushedItems;

private:

  std::vector< Toolkit::Page > mItemStack;
  Toolkit::Page                mCurrentItem;
  Vector2                      mControlSize;

  Layer                        mBackgroundLayer;
  Layer                        mBarLayer;
  Layer                        mContentLayer;
  Layer                        mPopupLayer;

  Actor                        mBackground;

  NavigationBar*               mToolBar;
  NavigationBar*               mTitleBar;

  int                          mOrientationAngle;
  float                        mOrientationAnimationDuration;
  AlphaFunction                mOrientationAnimationAlphaFunc;
  Vector3                      mItemPositionCoefficient;

  Toolkit::Popup               mPopupMenu;

private:
  Toolkit::NavigationControl::ItemPushedSignalType mItemPushedSignal;   ///< The signal to notify the item push
  Toolkit::NavigationControl::ItemPoppedSignalType mItemPoppedSignal;   ///< The signal to notify the item pop
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::NavigationControl& GetImpl( Toolkit::NavigationControl& navigationControl )
{
  DALI_ASSERT_ALWAYS( navigationControl );

  Dali::RefObject& handle = navigationControl.GetImplementation();

  return static_cast<Toolkit::Internal::NavigationControl&>( handle );
}

inline const Toolkit::Internal::NavigationControl& GetImpl( const Toolkit::NavigationControl& navigationControl )
{
  DALI_ASSERT_ALWAYS( navigationControl );

  const Dali::RefObject& handle = navigationControl.GetImplementation();

  return static_cast<const Toolkit::Internal::NavigationControl&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_NAVIGATION_CONTROL_H__ */
