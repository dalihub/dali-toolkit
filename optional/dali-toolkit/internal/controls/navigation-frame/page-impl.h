#ifndef __DALI_TOOLKIT_INTERNAL_PAGE_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_PAGE_IMPL_H__
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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/dali.h>
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

class Page : public ControlImpl
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

private: // From ControlImpl

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Dali::CustomActorImpl::OnPropertySet()
   */
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue );

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

  Property::Index  mPropertyTitle;       ///< Property index for title.
  Property::Index  mPropertySubTitle;    ///< Property index for sub title.
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
