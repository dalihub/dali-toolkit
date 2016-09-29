#ifndef __DALI_TOOLKIT_INTERNAL_NAVIGATION_CONTROL_H__
#define __DALI_TOOLKIT_INTERNAL_NAVIGATION_CONTROL_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/navigation-view/navigation-view.h>

namespace Dali
{

namespace Toolkit
{

class NavigationView;

namespace Internal
{

class NavigationBar;

/**
 * @brief
 *
 * NavigationView implements a controller than manages the navigation of hierarchical contents.
 */

class NavigationView : public Control
{
public:

  /**
   * Create an initialized NavigationView.
   * @return A handle to a newly allocated Dali resource
   */
  static Toolkit::NavigationView New();

  /**
   * @copydoc Dali::Toolkit::NavigationView::Push()
   */
  void Push( Actor& actor );

  /**
   * @copydoc Dali::Toolkit::NavigationView::Pop()
   */
  Actor Pop();

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] properties The properties with which to perform this action.
   * @return true if action has been accepted by this control
   */
  //static bool DoAction( BaseObject* object, const std::string& actionName, const Property::Map& properties );


private: // override functions from Control

  /**
   * @copydoc Control::OnStageConnection( int depth  )
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Control::OnRelayout()
   */
  //virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

protected:

  /**
   * Constructor.
   * It initializes the NavigationView members
   */
  NavigationView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NavigationView();

private:

  // Undefined
  NavigationView(const NavigationView&);

  // Undefined
  NavigationView& operator=(const NavigationView& rhs);

private:

  std::vector< Actor >         mContentStack;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::NavigationView& GetImpl( Toolkit::NavigationView& navigationView )
{
  DALI_ASSERT_ALWAYS( navigationView );

  Dali::RefObject& handle = navigationView.GetImplementation();

  return static_cast<Toolkit::Internal::NavigationView&>( handle );
}

inline const Toolkit::Internal::NavigationView& GetImpl( const Toolkit::NavigationView& navigationView )
{
  DALI_ASSERT_ALWAYS( navigationView );

  const Dali::RefObject& handle = navigationView.GetImplementation();

  return static_cast<const Toolkit::Internal::NavigationView&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_NAVIGATION_CONTROL_H__ */
