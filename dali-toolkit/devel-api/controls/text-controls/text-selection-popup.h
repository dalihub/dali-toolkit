#ifndef DALI_TOOLKIT_TEXT_SELECTION_POPUP_H
#define DALI_TOOLKIT_TEXT_SELECTION_POPUP_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-map.h>

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
class DALI_TOOLKIT_API TextSelectionPopup : public Control
{
public:
  enum Buttons
  {
    CUT        = 1u << 0,
    COPY       = 1u << 1,
    PASTE      = 1u << 2,
    SELECT     = 1u << 3,
    SELECT_ALL = 1u << 4,
    CLIPBOARD  = 1u << 5,
    NONE       = 1u << 6,
  };

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000 ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextSelectionPopup class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The maximum size the Popup can be.
       * @details Name "popupMaxSize", type Vector2.
       */
      POPUP_MAX_SIZE = PROPERTY_START_INDEX,

      /**
       * @brief The minimum size the Popup can be.
       * @details Name "popupMinSize", type Vector2.
       */
      POPUP_MIN_SIZE,

      /**
       * @brief The maximum size an option can be.
       * @details Name "optionMaxSize", type Vector2.
       */
      OPTION_MAX_SIZE,

      /**
       * @brief The minimum size an option can be.
       * @details Name "optionMinSize", type Vector2.
       */
      OPTION_MIN_SIZE,

      /**
       * @brief The size of the divider between options.
       * @details Name "optionDividerSize", type Vector2.
       */
      OPTION_DIVIDER_SIZE,

      /**
       * @brief The padding of the divider between options.
       * @details Name "optionDividerPadding", type Vector4.
       */
      OPTION_DIVIDER_PADDING,

      /**
       * @brief The image to use as the popup clipboard icon.
       * @details Name "popupClipboardButtonImage", type string.
       */
      POPUP_CLIPBOARD_BUTTON_ICON_IMAGE,

      /**
       * @brief The image to use as the popup cut icon.
       * @details Name "popupCutButtonImage", type string.
       */
      POPUP_CUT_BUTTON_ICON_IMAGE,

      /**
       * @brief The image to use as the popup copy icon.
       * @details Name "popupCopyButtonImage", type string.
       */
      POPUP_COPY_BUTTON_ICON_IMAGE,

      /**
       * @brief The image to use as the popup paste icon.
       * @details Name "popupPasteButtonImage", type string.
       */
      POPUP_PASTE_BUTTON_ICON_IMAGE,

      /**
       * @brief The image to use as the popup select icon.
       * @details Name "popupSelectButtonImage", type string.
       */
      POPUP_SELECT_BUTTON_ICON_IMAGE,

      /**
       * @brief The image to use as the popup select all icon.
       * @details Name "popupSelectAllButtonImage", type string.
       */
      POPUP_SELECT_ALL_BUTTON_ICON_IMAGE,

      /**
       * @brief The color of the divider between options.
       * @details Name "popupDividerColor", type Vector4.
       */
      POPUP_DIVIDER_COLOR,

      /**
       * @brief The color of the icons (if supplied).
       * @details Name "popupIconColor", type Vector4.
       */
      POPUP_ICON_COLOR,

      /**
       * @brief The color of the option when pressed.
       * @details Name "popupPressedColor", type Vector4.
       */
      POPUP_PRESSED_COLOR,

      /**
       * @brief The corner radius of the option when pressed.
       * @details Name "popupPressedCornerRadius", type float.
       */
      POPUP_PRESSED_CORNER_RADIUS,

      /**
       * @brief The image to use for the option when pressed.
       * @details Name "popupPressedImage", type string.
       */
      POPUP_PRESSED_IMAGE,

      /**
       * @brief The duration of the fade-in animation.
       * @details Name "popupFadeInDuration", type float.
       */
      POPUP_FADE_IN_DURATION,

      /**
       * @brief The duration of the fade-out animation.
       * @details Name "popupFadeOutDuration", type float.
       */
      POPUP_FADE_OUT_DURATION,

      /**
       * @brief The popup background can have a separate border with a different color.
       * @details Name "backgroundBorder", type Property::Map.
       * @note Optional.
       */
      BACKGROUND_BORDER,

      /**
       * @brief The popup background.
       * @details Name "background", type Property::Map.
       * @note Optional.
       */
      BACKGROUND,

      /**
       * @brief The minimum size of popup label.
       * @details Name "labelMinimumSize", type Vector2.
       * @note Optional.
       */
      LABEL_MINIMUM_SIZE,

      /**
       * @brief The padding of popup label.
       * @details Name "labelPadding", type Vector4.
       * @note Optional.
       */
      LABEL_PADDING,

      /**
       * @brief The text visual map of popup label.
       * @details Name "labelTextVisual", type Property::Map.
       * @note Optional.
       */
      LABEL_TEXT_VISUAL,

      /**
       * @brief Whether the scroll-bar is enabled.
       * @details Name "enableScrollBar", type Property::BOOLEAN.
       * @note Optional.
       */
      ENABLE_SCROLL_BAR
    };
  };

  /**
   * Create the TextSelectionPopup control.
   * @param[in] callbackInterface The text popup callback interface which receives the button click callbacks.
   * @return A handle to the TextSelectionPopup control.
   */
  static TextSelectionPopup New(TextSelectionPopupCallbackInterface* callbackInterface);

  /**
   * @brief Creates an empty handle.
   */
  TextSelectionPopup();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextSelectionPopup(const TextSelectionPopup& handle);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextSelectionPopup& operator=(const TextSelectionPopup& handle);

  /**
   * @brief Move constructor.
   *
   * @param[in] handle The handle to move from.
   */
  TextSelectionPopup(TextSelectionPopup&& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] handle The handle to move from.
   * @return A reference to this.
   */
  TextSelectionPopup& operator=(TextSelectionPopup&& handle);

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
  static TextSelectionPopup DownCast(BaseHandle handle);

  /**
   * @brief Specify which buttons to show in Popup
   * @param[in] buttonsToEnable Buttons to enable
   */
  void EnableButtons(Toolkit::TextSelectionPopup::Buttons buttonsToEnable);

  /**
   * @brief Raises the toolbar's actor above the given @e target actor.
   *
   * @param[in] target The actor to get above of.
   */
  void RaiseAbove(Actor target);

  /**
   * @brief Show the Popup if not being shown
   */
  void ShowPopup();

  /**
   * @brief Hide the Popup if shown
   */
  void HidePopup();

  /**
   * @brief Used to set options of text selection popup
   *
   * @param[in] properties The text selection popup options
   */
  void SetProperties(const Dali::Property::Map& properties);

  /**
   * @brief Retrieve property map of text selection popup options
   *
   * @param[out] properties The text selection popup options
   */
  void GetProperties(Dali::Property::Map& properties);

public: // Not intended for application developers
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextSelectionPopup(Internal::TextSelectionPopup& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextSelectionPopup(Dali::Internal::CustomActor* internal);

}; // Class TextSelectionPopup

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SELECTION_POPUP_H
