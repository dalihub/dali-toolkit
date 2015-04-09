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

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextLabel class.
   */
  struct Property
  {
    enum
    {

      POPUP_MAX_SIZE,                           ///< name "popup-max-size",                The max size the Popup can be,                  type VECTOR2
      POPUP_BACKGROUND_IMAGE,                   ///< name "popup-background-image",        The image to display for popup background       type STRING
      POPUP_CLIPBOARD_BUTTON_ICON_IMAGE,        ///< name "popup-clipboard-button-image",  The image to use as the popup clipboard icon,   type STRING
      POPUP_CUT_BUTTON_ICON_IMAGE,              ///< name "popup-cut-button-image",        The image to use as the popup cut icon,         type STRING
      POPUP_COPY_BUTTON_ICON_IMAGE,             ///< name "popup-copy-button-image",       The image to use as the popup copy icon,        type STRING
      POPUP_PASTE_BUTTON_ICON_IMAGE,            ///< name "popup-paste-button-image",      The image to use as the popup paste icon,       type STRING
      POPUP_SELECT_BUTTON_ICON_IMAGE,           ///< name "popup-select-button-image",     The image to use as the popup select icon,      type STRING
      POPUP_SELECT_ALL_BUTTON_ICON_IMAGE,       ///< name "popup-select-all-button-image", The image to use as the popup select all icon,  type STRING
    };
  };

  /**
   * Create the TextSelectionPopup control.
   * @return A handle to the TextSelectionPopup control.
   */
  static TextSelectionPopup New();

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

} // namepsace Dali

#endif // __DALI_TOOLKIT_TEXT_SELECTION_POPUP_H__
