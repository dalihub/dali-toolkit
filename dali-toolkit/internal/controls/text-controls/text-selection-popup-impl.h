#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_H__

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/layer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

enum PopupCustomisations
{
  POPUP_MAXIMUM_SIZE,
  POPUP_MINIMUM_SIZE,
  OPTION_MAXIMUM_SIZE,
  OPTION_MINIMUM_SIZE,
  OPTION_DIVIDER_SIZE
};

} // namespace

class TextSelectionPopup : public Control
{
public:

  struct ButtonRequirement
  {
    ButtonRequirement()
    : id( Toolkit::TextSelectionPopup::NONE ),
      priority( 0u ),
      name(),
      caption(),
      icon(),
      enabled( false )
    {}

    ButtonRequirement( Toolkit::TextSelectionPopup::Buttons buttonId,
                       std::size_t buttonPriority,
                       const std::string& buttonName,
                       const std::string& buttonCaption,
                       Dali::Image& buttonIcon,
                       bool buttonEnabled )
    : id( buttonId ),
      priority( buttonPriority ),
      name( buttonName ),
      caption( buttonCaption ),
      icon( buttonIcon ),
      enabled( buttonEnabled )
    {}

    Toolkit::TextSelectionPopup::Buttons id;
    std::size_t priority;
    std::string name;
    std::string caption;
    Dali::Image icon;
    bool enabled;
  };

  struct ButtonPriorityCompare
  {
      bool operator()( const ButtonRequirement& lhs, const ButtonRequirement& rhs ) const {
        return lhs.priority < rhs.priority;
      }
  };

  /**
   * @brief New constructor with provided buttons to enable.
   * @param[in] buttonsToEnable bit mask of buttons to enable
   * @param[in] callbackInterface The text popup callback interface which receives the button click callbacks.
   * @return A handle to the TextSelectionPopup control.
   */
  static Toolkit::TextSelectionPopup New( Toolkit::TextSelectionPopup::Buttons buttonsToEnable,
                                          TextSelectionPopupCallbackInterface* callbackInterface );

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

private: // Implementation

  /**
   * @brief When the cut button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnCutButtonPressed( Toolkit::Button button );

  /**
   * @brief When the copy button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnCopyButtonPressed( Toolkit::Button button );

  /**
   * @brief When the paste button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnPasteButtonPressed( Toolkit::Button button );

  /**
   * @brief When the select button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnSelectButtonPressed( Toolkit::Button button );

  /**
   * @brief When the select all button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnSelectAllButtonPressed( Toolkit::Button button );

  /**
   * @brief When the clipboard button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnClipboardButtonPressed( Toolkit::Button button );

  /**
   * @brief Method to set the dimension or dimension constraint on certain aspects of the Popup.
   *
   * @param[in] settingToCustomise The setting for the PopupCustomisations enum that can be customised
   * @param[in] dimension The size to customise with
   */
  void SetDimensionToCustomise( const PopupCustomisations& settingToCustomise, const Size& dimension );

  /**
   * @brief Method to get the dimension or dimension constraint on certain aspects of the Popup that was previously customised
   *
   * @param[in] setting The setting from the PopupCustomisations enum
   */
  Size GetDimensionToCustomise( const PopupCustomisations& setting );

  /**
   * @brief Sets the image for the given button of the Popup.
   *
   * @param[in] button  The button the image should be used for from the Buttons Enum.
   * @param[in] image The image to use.
   */
 void SetButtonImage( Toolkit::TextSelectionPopup::Buttons button, Dali::Image image );

  /**
   * @brief Retrieves the image of the given button used by the popup
   *
   * @param[in] button The button to get the image from
   * @return The image used for that button.
   */
  Dali::Image GetButtonImage( Toolkit::TextSelectionPopup::Buttons button );

  void CreateOrderedListOfPopupOptions();

  void AddOption( const ButtonRequirement& button, bool showDivider, bool showIcons, bool showCaption );

  std::size_t GetNumberOfEnabledOptions();

  void AddPopupOptionsToToolbar(  bool showIcons, bool showCaptions );

  void CreatePopup();

  /**
   * Construct a new TextField.
   */
  TextSelectionPopup( TextSelectionPopupCallbackInterface* callbackInterface );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextSelectionPopup();

private:

  // Undefined copy constructor and assignment operators
  TextSelectionPopup(const TextSelectionPopup&);
  TextSelectionPopup& operator=(const TextSelectionPopup& rhs);

private: // Data


  Dali::Toolkit::TextSelectionToolbar mToolbar;

  Dali::Toolkit::TableView mTableOfButtons;           // Actor which holds all the buttons, sensitivity can be set on buttons via this actor

  // Images to be used by the Popup buttons
  Image mCutIconImage;
  Image mCopyIconImage;
  Image mPasteIconImage;
  Image mClipboardIconImage;
  Image mSelectIconImage;
  Image mSelectAllIconImage;

  Size mMaxSize;                       // Maximum size of the Popup
  Size mMinSize;                       // Minimum size of the Popup

  Size mOptionMaxSize;                 // Maximum size of an Option button
  Size mOptionMinSize;                 // Minimum size of an Option button
  Size mOptionDividerSize;             // Size of divider line

  std::vector<ButtonRequirement> mOrderListOfButtons; // List of buttons in the order to be displayed and a flag to indicate if needed.

  Toolkit::TextSelectionPopup::Buttons mEnabledButtons; // stores enabled buttons
  Toolkit::TextSelectionPopupCallbackInterface* mCallbackInterface;

  Vector4 mDividerColor;                // Color of the divider between buttons
  Vector4 mIconColor;                   // Color of the popup icon.
  Vector4 mPressedColor;                // Color of the popup option when pressed.

  // Priority of Options/Buttons in the Cut and Paste pop-up, higher priority buttons are displayed first, left to right.
  std::size_t mSelectOptionPriority;    // Position of Select Button
  std::size_t mSelectAllOptionPriority; // Position of Select All button
  std::size_t mCutOptionPriority;       // Position of Cut button
  std::size_t mCopyOptionPriority;      // Position of Copy button
  std::size_t mPasteOptionPriority;     // Position of Paste button
  std::size_t mClipboardOptionPriority; // Position of Clipboard button

  bool mShowIcons:1; // Flag to show icons
  bool mShowCaptions:1; // Flag to show text captions

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextSelectionPopup& GetImpl( Toolkit::TextSelectionPopup& textSelectionPopup )
{
  DALI_ASSERT_ALWAYS( textSelectionPopup );

  Dali::RefObject& handle = textSelectionPopup.GetImplementation();

  return static_cast<Toolkit::Internal::TextSelectionPopup&>(handle);
}

inline const Toolkit::Internal::TextSelectionPopup& GetImpl( const Toolkit::TextSelectionPopup& textSelectionPopup )
{
  DALI_ASSERT_ALWAYS( textSelectionPopup );

  const Dali::RefObject& handle = textSelectionPopup.GetImplementation();

  return static_cast<const Toolkit::Internal::TextSelectionPopup&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_H__

