#ifndef __DALI_TOOLKIT_TEXT_SELECTION_POPUP_H__
#define __DALI_TOOLKIT_TEXT_SELECTION_POPUP_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

class TextSelectionPopupCallbackInterface;

namespace Internal DALI_INTERNAL
{
class TextSelectionPopup;
}

/**
 * @brief A control which provides a Popup with a number of buttons
 *
 * The style of the pop can be set through style sheets, this includes the images for the buttons
 * A Show and Hide API is provided.
 *
 * If the buttons exceed the size constraints of the popup then it will offer scrolling.
 *
 *
 */
class DALI_IMPORT_API TextSelectionPopup : public Control
{
public:

  enum Buttons
  {
    CUT = 1u << 0,
    COPY = 1u << 1,
    PASTE = 1u << 2,
    SELECT = 1u << 3,
    SELECT_ALL = 1u << 4,
    CLIPBOARD = 1u << 5,
    NONE = 1u << 6,
  };

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextSelectionPopup class.
   */
  struct Property
  {
    enum
    {
      POPUP_MAX_SIZE =  PROPERTY_START_INDEX,   ///< name "popupMaxSize",                  maximum size the Popup can be,                  type VECTOR2
      POPUP_MIN_SIZE,                           ///< name "popupMinSize",                  minimum size the Popup can be,                  type VECTOR2
      OPTION_MAX_SIZE,                          ///< name "optionMaxSize",                 maximum size an option can be,                  type VECTOR2
      OPTION_MIN_SIZE,                          ///< name "optionMinSize",                 minimum size an option can be,                  type VECTOR2
      OPTION_DIVIDER_SIZE,                      ///< name "optionDividerSize",             size of the divider between options             type VECTOR2
      POPUP_CLIPBOARD_BUTTON_ICON_IMAGE,        ///< name "popupClipboardButtonImage",     The image to use as the popup clipboard icon,   type STRING
      POPUP_CUT_BUTTON_ICON_IMAGE,              ///< name "popupCutButtonImage",           The image to use as the popup cut icon,         type STRING
      POPUP_COPY_BUTTON_ICON_IMAGE,             ///< name "popupCopyButtonImage",          The image to use as the popup copy icon,        type STRING
      POPUP_PASTE_BUTTON_ICON_IMAGE,            ///< name "popupPasteButtonImage",         The image to use as the popup paste icon,       type STRING
      POPUP_SELECT_BUTTON_ICON_IMAGE,           ///< name "popupSelectButtonImage",        The image to use as the popup select icon,      type STRING
      POPUP_SELECT_ALL_BUTTON_ICON_IMAGE,       ///< name "popupSelectAllButtonImage",     The image to use as the popup select all icon,  type STRING
      POPUP_DIVIDER_COLOR,                      ///< name "popupDividerColor",             The color of the divider between options,       type VECTOR4
      POPUP_ICON_COLOR,                         ///< name "popupIconColor",                The color of the icons (if supplied),           type VECTOR4
      POPUP_PRESSED_COLOR,                      ///< name "popupPressedColor",             The color of the option when pressed,           type VECTOR4
      POPUP_PRESSED_IMAGE,                      ///< name "popupPressedImage",             The image to use for the option when pressed,   type STRING
      POPUP_FADE_IN_DURATION,                   ///< name "popupFadeInDuration",           The duration of the fade-in animation,          type FLOAT
      POPUP_FADE_OUT_DURATION,                  ///< name "popupFadeOutDuration",          The duration of the fade-out animation,         type FLOAT
    };
  };

  /**
   * Create the TextSelectionPopup control.
   * @param[in] callbackInterface The text popup callback interface which receives the button click callbacks.
   * @return A handle to the TextSelectionPopup control.
   */
  static TextSelectionPopup New( TextSelectionPopupCallbackInterface* callbackInterface );

  /**
   * @brief Creates an empty handle.
   */
  TextSelectionPopup();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextSelectionPopup( const TextSelectionPopup& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextSelectionPopup& operator=( const TextSelectionPopup& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextSelectionPopup();

  /**
   * @brief Downcast a handle to TextSelectionPopup.
   *
   * If the BaseHandle points is a TextSelectionPopup the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextSelectionPopup or an empty handle
   */
  static TextSelectionPopup DownCast( BaseHandle handle );

  /**
   * @brief Specify which buttons to show in Popup
   * @param[in] buttonsToEnable Buttons to enable
   */
  void EnableButtons( Toolkit::TextSelectionPopup::Buttons buttonsToEnable );

  /**
   * @brief Raises the toolbar's layer above the given @e target layer.
   *
   * @param[in] target The layer to get above of.
   */
  void RaiseAbove( Layer target );

  /**
   * @brief Show the Popup if not being shown
   */
  void ShowPopup();

  /**
   * @brief Hide the Popup if shown
   */
  void HidePopup();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextSelectionPopup( Internal::TextSelectionPopup& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextSelectionPopup( Dali::Internal::CustomActor* internal );

}; // Class TextSelectionPopup

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SELECTION_POPUP_H__
