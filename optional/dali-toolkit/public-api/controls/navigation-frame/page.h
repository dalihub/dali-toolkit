#ifndef __DALI_TOOLKIT_PAGE_H__
#define __DALI_TOOLKIT_PAGE_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/popup/popup.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class Page;
}

/**
 * A Page is a custom control which can be pushed into the stack of navigation control.
 * It serves as the root of a navigation view.
 * It also carries the title/subtitle/buttons/icons information which would be shown on the navigation bars when the item is on the top of the stack.
 */
class Page : public Control
{

public:

  /**
   * The names of custom properties installed by this control.
   */
  // Property Names
  static const char* const PROPERTY_TITLE;       ///< name "title", type std::string
  static const char* const PROPERTY_SUB_TITLE;   ///< name "sub-title", type std::string

  /**
   * Structure to indicate a control on the navigation tool bar and its group (HorizontalLeft, HorizontalRight or HorizontalCenter)
   */
  struct ControlOnBar
  {
    /**
     * Constructor
     */
    ControlOnBar(Actor actor, Toolkit::Alignment::Type alignment)
    : control( actor ),
      alignment( alignment )
    {
    }

    Actor control;                            ///< The control actor
    Toolkit::Alignment::Type alignment;       ///< The alignment of the control actor
  };

  typedef std::vector< const ControlOnBar* > ControlOnBarContainer;

public:

  /**
   * Create a Page handle; this can be initialized with Page::New().
   * Calling member function with an uninitialized handle is not allowed.
   */
  Page();

  /**
   * Copy Constructor.
   */
  Page( const Page& handle );

  /**
   * Assignment operator.
   * Change this handle to point to another real object.
   */
  Page& operator=( const Page& handle );

  /**
   * Create an initialized Page.
   * @return A handle to a newly allocated Dali resource.
   */
  static Page New();

  /**
   * Downcast an object handle to Page.
   * If handle points to a Page, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @param[in] handle Handle to an object.
   * @return handle to a Page of an uninitialized handle.
   */
  static Page DownCast( BaseHandle handle );

  /**
   * Sets the Page's title.
   * The title will be displayed on the navigation title bar when the item is on the top of the stack.
   * @param[in] title The Page's title.
   */
  void SetTitle(const std::string& title);

  /**
   * Retrieve the title of the page.
   * @return The Page's title or "" when the item does not have a title.
   */
  const std::string& GetTitle() const;

  /**
   * Sets the Page's subtitle.
   * The subtitle will be displayed on the navigation title bar when the item is on the top of the stack.
   * @param[in] subtitle The Page's subtitle.
   */
  void SetSubTitle(const std::string& subtitle);

  /**
   * Retrieve the subtitle of the page.
   * @return The Page's subtitle or "" when the subtitle does not have a subtitle.
   */
  const std::string& GetSubTitle() const;

  /**
   * Sets the Page's title icon.
   * The icon will be displayed in front of the title on the navigation item bar when the item is on the top.
   * @param[in] titleIcon The Page's icon
   */
  void SetTitleIcon( Actor titleIcon);

  /**
   * Retrieve the title icon of the page.
   * @return The Page's icon or an empty handle when the item does not have title icon.
   */
  Actor GetTitleIcon() const;

  /**
   * Set a control onto the navigation tool bar when the item is on the top.
   * Only one control (the last set one) is valid for HorizontalLeft and HorizontalRight each.
   * Can have multiple controls for HorizontalCenter.
   * If the control is uninitialized or if the alignment has a value other from HorizontalLeft/HorizontalRight/HorizontalCenter, the control is not added.
   * @param[in] control The control on the navigation tool bar.
   * @param[in] alignment The position of the control, can be HorizontalLeft/HorizontalRight/HorizontalCenter.
   * @return true if add control successfully, false if fail to add the control
   */
  bool AddControlToToolBar(Actor control, Alignment::Type alignment);

  /**
   * Retrieve the controls that would be displayed on the navigation tool bar when the item is on the top.
   * @return the vector of tool bar controls associated with the current item.
   */
  const ControlOnBarContainer GetControlsOnToolBar() const;

  /**
   * Set a control onto the right part of the navigation title bar when the item is on the top.
   * If the control is uninitialized or if the alignment has a value other from HorizontalLeft/HorizontalRight/HorizontalCenter, the control is not added.
   * @param[in] control The control on the navigation title bar.
   * @return true if add control successfully, false if fail to add the control
   */
  bool AddControlToTitleBar(Actor control);

  /**
   * Retrieve the controls that would be displayed on the navigation title bar when the item is on the top.
   * @return the vector of title bar controls associate with the current item.
   */
  const ActorContainer GetControlsOnTitleBar() const;

  /**
   * Set the menu which would pop up when the KEY_MENU is pressed.
   * @param[in] popupMenu the popup menu connected to the KEY_MENU event
   */
  void SetPopupMenu( Toolkit::Popup popupMenu );

  /**
   * Get the menu which would pop up when the KEY_MENU is pressed.
   * @return The popup menu connected to the KEY_MENU event
   */
  Toolkit::Popup GetPopupMenu() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in] impl The Page implementation.
   */
  Page(Internal::Page& impl);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Page( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_PAGE_H__ */
