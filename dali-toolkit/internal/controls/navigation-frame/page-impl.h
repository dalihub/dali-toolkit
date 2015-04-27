#ifndef __DALI_TOOLKIT_INTERNAL_PAGE_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_PAGE_IMPL_H__
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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>

namespace Dali
{

namespace Toolkit
{

class Page;

namespace Internal
{

/**
 * Page object is a custom control which can be pushed into the stack of navigation control.
 * @see Dali::Toolkit::Page for more details
 */

class Page : public Control
{

public:

  /**
   * destructor
   */
  virtual ~Page();

  /**
   * Create an initialized Page.
   * @return A handle to a newly allocated Dali resource
   */
  static Toolkit::Page New();

  /**
   * @copydoc Dali::Toolkit::Page::SetTitle
   */
  void SetTitle(const std::string& title);

  /**
   * @copydoc Dali::Toolkit::Page::GetTitle
   */
  const std::string& GetTitle() const;

  /**
   * @copydoc Dali::Toolkit::Page::SetSubTitle
   */
  void SetSubTitle(const std::string& subtitle);

  /**
   * @copydoc Dali::Toolkit::Page::GetSubTitle
   */
  const std::string& GetSubTitle() const;

  /**
   * @copydoc Dali::Toolkit::Page::SetTitleIcon
   */
  void SetTitleIcon( Actor titleIcon);

  /**
   * @copydoc Dali::Toolkit::Page::GetTitleIcon
   */
  Actor GetTitleIcon() const;

  /**
   * @copydoc Dali::Toolkit::Page::AddControlToToolBar
   */
  bool AddControlToToolBar(Actor control, Toolkit::Alignment::Type alignment);

  /**
   * @copydoc Dali::Toolkit::Page::GetControlsOnToolBar
   */
  const Toolkit::Page::ControlOnBarContainer& GetControlsOnToolBar() const;

  /**
   * @copydoc Dali::Toolkit::Page::AddControlToTitleBar
   */
  bool AddControlToTitleBar(Actor control);

  /**
   * @copydoc Dali::Toolkit::Page::GetControlsOnTitleBar
   */
  const ActorContainer& GetControlsOnTitleBar() const;

  /**
   * @copydoc Dali::Toolkit::Page::SetPopupMenu
   */
  void SetPopupMenu( Toolkit::Popup popupMenu );

  /**
   * @copydoc Dali::Toolkit::Page::GetPopupMenu
   */
  Toolkit::Popup GetPopupMenu() const;

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

private:

  /**
   * Constructor.
   * It initializes the Page members
   */
  Page();

private:
  std::string        mTitle;
  std::string        mSubTitle;
  Actor              mTitleIcon;
  Toolkit::Popup     mPopupMenu;

  ActorContainer                        mTitleBarControls;
  Toolkit::Page::ControlOnBarContainer  mToolBarControls;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Page& GetImpl( Toolkit::Page& page )
{
  DALI_ASSERT_ALWAYS( page );

  Dali::RefObject& handle = page.GetImplementation();

  return static_cast<Toolkit::Internal::Page&>( handle );
}

inline const Toolkit::Internal::Page& GetImpl( const Toolkit::Page& page )
{
  DALI_ASSERT_ALWAYS( page );

  const Dali::RefObject& handle = page.GetImplementation();

  return static_cast<const Toolkit::Internal::Page&>( handle );
}

}// namespace Toolkit

} // namespace Dali


#endif /* __DALI_TOOLKIT_INTERNAL_PAGE_IMPL_H__*/
