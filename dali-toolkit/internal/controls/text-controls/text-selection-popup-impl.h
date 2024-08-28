#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/property-map.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
enum PopupCustomisations
{
  POPUP_MAXIMUM_SIZE,
  OPTION_MAXIMUM_SIZE,
  OPTION_MINIMUM_SIZE,
  OPTION_DIVIDER_SIZE
};

class TextSelectionPopup : public Control
{
public:
  struct ButtonRequirement
  {
    ButtonRequirement()
    : id(Toolkit::TextSelectionPopup::NONE),
      priority(0u),
      name(),
      caption(),
      enabled(false)
    {
    }

    ButtonRequirement(Toolkit::TextSelectionPopup::Buttons buttonId,
                      std::size_t                          buttonPriority,
                      const std::string&                   buttonName,
                      const std::string&                   buttonCaption,
                      bool                                 buttonEnabled)
    : id(buttonId),
      priority(buttonPriority),
      name(buttonName),
      caption(buttonCaption),
      enabled(buttonEnabled)
    {
    }

    Toolkit::TextSelectionPopup::Buttons id;
    std::size_t                          priority;
    std::string                          name;
    std::string                          caption;
    bool                                 enabled;
  };

  struct ButtonPriorityCompare
  {
    bool operator()(const ButtonRequirement& lhs, const ButtonRequirement& rhs) const
    {
      return lhs.priority < rhs.priority;
    }
  };

  /**
   * @brief New constructor with provided buttons to enable.
   * @param[in] callbackInterface The text popup callback interface which receives the button click callbacks.
   * @return A handle to the TextSelectionPopup control.
   */
  static Toolkit::TextSelectionPopup New(TextSelectionPopupCallbackInterface* callbackInterface);

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  /**
   * @brief Toolkit::TextSelectionPopup::SetProperties()
   */
  void SetProperties(const Property::Map& properties);

  /**
   * @brief Toolkit::TextSelectionPopup::GetProperties()
   */
  void GetProperties(Property::Map& properties);

  /**
   * @copydoc Toolkit::EnableButtons
   */
  void EnableButtons(Toolkit::TextSelectionPopup::Buttons buttonsToEnable);

  /**
   * @copydoc Toolkit::TextSelectionPopup::RaiseAbove()
   */
  void RaiseAbove(Actor target);

  /**
   * @copydoc Toolkit::TextSelectionPopup::ShowPopup()
   */
  void ShowPopup();

  /**
   * @copydoc Toolkiut::TextSelectionPopup::HidePopup()
   */
  void HidePopup();

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  void OnInitialize() override;

private: // Implementation
  void HideAnimationFinished(Animation& animation);

  /**
   * @brief When the cut button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnCutButtonPressed(Toolkit::Button button);

  /**
   * @brief When the copy button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnCopyButtonPressed(Toolkit::Button button);

  /**
   * @brief When the paste button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnPasteButtonPressed(Toolkit::Button button);

  /**
   * @brief When the select button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnSelectButtonPressed(Toolkit::Button button);

  /**
   * @brief When the select all button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnSelectAllButtonPressed(Toolkit::Button button);

  /**
   * @brief When the clipboard button is pressed.
   * @param[in] button the button pressed
   * @return @e true to consume the event.
   */
  bool OnClipboardButtonPressed(Toolkit::Button button);

  /**
   * @brief Method to set the dimension or dimension constraint on certain aspects of the Popup.
   *
   * @param[in] settingToCustomise The setting for the PopupCustomisations enum that can be customised
   * @param[in] dimension The size to customise with
   */
  void SetDimensionToCustomise(const PopupCustomisations& settingToCustomise, const Size& dimension);

  /**
   * @brief Method to get the dimension or dimension constraint on certain aspects of the Popup that was previously customised
   *
   * @param[in] setting The setting from the PopupCustomisations enum
   */
  Size GetDimensionToCustomise(const PopupCustomisations& setting) const;

  /**
   * @brief Sets the image for the given button of the Popup.
   *
   * @param[in] button  The button the image should be used for from the Buttons Enum.
   * @param[in] image The image to use.
   */
  void SetButtonImage(Toolkit::TextSelectionPopup::Buttons button, const std::string& image);

  /**
   * @brief Retrieves the image of the given button used by the popup
   *
   * @param[in] button The button to get the image from
   * @return The image used for that button.
   */
  const std::string& GetButtonImage(Toolkit::TextSelectionPopup::Buttons button) const;

  /**
   * @brief Sets the image for the pressed state of a popup option.
   *
   * @param[in]  filename The image filename to use.
   */
  void SetPressedImage(const std::string& filename);

  /**
   * @brief Gets the image used for the pressed state of a popup option.
   *
   * @return     The image filename used.
   */
  std::string GetPressedImage() const;

  /**
   * Set option divider padding
   * @param[in] padding BEGIN END BOTTOM TOP
   */
  void SetOptionDividerPadding(const Padding& padding);

  /**
   * Get option divider padding
   * @return Padding
   */
  Padding GetOptionDividerPadding() const;

  /**
   * Set label padding
   * @param[in] padding BEGIN END BOTTOM TOP
   */
  void SetLabelPadding(const Padding& padding);

  /**
   * Get label padding
   * @return Padding
   */
  Padding GetLabelPadding() const;

  void CreateOrderedListOfPopupOptions();

  void AddOption(const ButtonRequirement& button, bool showDivider, bool showIcons, bool showCaption);

  std::size_t GetNumberOfEnabledOptions() const;

  void AddPopupOptionsToToolbar(bool showIcons, bool showCaptions);

  /**
   * Creates the background-border image
   *
   * @param[in] propertyMap The properties describing the background-border
   */
  void CreateBackgroundBorder(Property::Map& propertyMap);

  /**
   * Creates the background image
   *
   * @param[in] propertyMap The properties describing the background
   */
  void CreateBackground(Property::Map& propertyMap);

  /**
   * Construct a new TextField.
   */
  TextSelectionPopup(TextSelectionPopupCallbackInterface* callbackInterface);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextSelectionPopup();

protected:
  struct PropertyHandler;

private:
  // Undefined copy constructor and assignment operators
  TextSelectionPopup(const TextSelectionPopup&);
  TextSelectionPopup& operator=(const TextSelectionPopup& rhs);

private: // Data
  Dali::Toolkit::TextSelectionToolbar mToolbar;

  Dali::Toolkit::TableView mTableOfButtons; // Actor which holds all the buttons, sensitivity can be set on buttons via this actor

  // Images paths to be used by the Popup buttons
  std::string mCutIconImage;
  std::string mCopyIconImage;
  std::string mPasteIconImage;
  std::string mClipboardIconImage;
  std::string mSelectIconImage;
  std::string mSelectAllIconImage;

  Size    mPopupMaxSize;         // Maximum size of the Popup
  Size    mOptionMaxSize;        // Maximum size of an Option button
  Size    mOptionMinSize;        // Minimum size of an Option button
  Size    mOptionDividerSize;    // Size of divider line
  Padding mOptionDividerPadding; // Padding of divider line

  Vector2       mLabelMinimumSize; // Minimum size of label
  Padding       mLabelPadding;     // Padding of label
  Property::Map mLabelTextVisual;  // Text visual map of label
  bool          mEnableScrollBar;  // Enable scrollbar

  std::vector<ButtonRequirement> mOrderListOfButtons; // List of buttons in the order to be displayed and a flag to indicate if needed.

  Toolkit::TextSelectionPopup::Buttons          mEnabledButtons; // stores enabled buttons
  Toolkit::TextSelectionPopupCallbackInterface* mCallbackInterface;

  std::string mPressedImage;        // Image used for the popup option when pressed.
  Vector4     mPressedColor;        // Color of the popup option when pressed.
  float       mPressedCornerRadius; // Corner radius of the popup option when pressed.
  Vector4     mDividerColor;        // Color of the divider between buttons
  Vector4     mIconColor;           // Color of the popup icon.

  // Priority of Options/Buttons in the Cut and Paste pop-up, higher priority buttons are displayed first, left to right.
  std::size_t mSelectOptionPriority;    // Position of Select Button
  std::size_t mSelectAllOptionPriority; // Position of Select All button
  std::size_t mCutOptionPriority;       // Position of Cut button
  std::size_t mCopyOptionPriority;      // Position of Copy button
  std::size_t mPasteOptionPriority;     // Position of Paste button
  std::size_t mClipboardOptionPriority; // Position of Clipboard button
  float       mFadeInDuration;          // Duration of the animation to fade in the Popup
  float       mFadeOutDuration;         // Duration of the animation to fade out the Popup

  bool mShowIcons : 1;      // Flag to show icons
  bool mShowCaptions : 1;   // Flag to show text captions
  bool mPopupShowing : 1;   // Flag to indicate Popup showing
  bool mButtonsChanged : 1; // Flag to indicate the Popup Buttons have changed
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextSelectionPopup& GetImpl(Toolkit::TextSelectionPopup& textSelectionPopup)
{
  DALI_ASSERT_ALWAYS(textSelectionPopup);

  Dali::RefObject& handle = textSelectionPopup.GetImplementation();

  return static_cast<Toolkit::Internal::TextSelectionPopup&>(handle);
}

inline const Toolkit::Internal::TextSelectionPopup& GetImpl(const Toolkit::TextSelectionPopup& textSelectionPopup)
{
  DALI_ASSERT_ALWAYS(textSelectionPopup);

  const Dali::RefObject& handle = textSelectionPopup.GetImplementation();

  return static_cast<const Toolkit::Internal::TextSelectionPopup&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_POPUP_H
