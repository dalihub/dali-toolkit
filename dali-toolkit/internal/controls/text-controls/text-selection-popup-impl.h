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
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/public-api/controls/text-controls/text-selection-popup.h>

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
enum PopupParts
{
  POPUP_BACKGROUND,
  POPUP_CLIPBOARD_BUTTON,
  POPUP_CUT_BUTTON_ICON,
  POPUP_COPY_BUTTON_ICON,
  POPUP_PASTE_BUTTON_ICON,
  POPUP_SELECT_BUTTON_ICON,
  POPUP_SELECT_ALL_BUTTON_ICON,
};

} // namespace

class TextSelectionPopup : public Control
{
public:

  enum Buttons
  {
    ButtonsCut,
    ButtonsCopy,
    ButtonsPaste,
    ButtonsSelect,
    ButtonsSelectAll,
    ButtonsClipboard,
    ButtonsEnumEnd
  };

  struct ButtonRequirement
  {
    ButtonRequirement()
    : id( ButtonsEnumEnd ),
      priority( 0u ),
      name(),
      caption(),
      icon(),
      enabled( false )
    {}

    ButtonRequirement( Buttons buttonId,
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

    Buttons id;
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

//  static inline bool ButtonPriorityCompare( ButtonRequirement a, ButtonRequirement b )
//  {
//    return a.priority < b.priority ? true : false;
//  }

  /**
   * @copydoc Dali::Toollkit::TextSelectionPopup::New()
   */
  static Toolkit::TextSelectionPopup New();

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

  void CreatePopup();

  void DestroyPopup();

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
 virtual void OnInitialize();

//  /**
//   * @copydoc Control::GetNaturalSize()
//   */
//  virtual Vector3 GetNaturalSize();
//
//  /**
//   * @copydoc Control::GetHeightForWidth()
//   */
//  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );
//
//  /**
//   * Received for single & double taps
//   */
//  virtual void OnTap( const TapGesture& tap );
//
//  /**
//   * @copydoc Text::ControlInterface::RequestTextRelayout()
//   */
//  virtual void RequestTextRelayout();

  /**
   * Set max size of Popup
   * @param[in] maxSize Size (Vector2)
   */
  void SetPopupMaxSize( const Size& maxSize );

  /**
   * Get Max size of Popup
   * @return Vector2 the max size of the Popup
   */
  const Dali::Vector2& GetPopupMaxSize() const;

  /**
   * @brief Sets the image for the given part of the Popup.
   *
   * @param[in] part  The part of the pop from the Enum PopupParts
   * @param[in] image The image to use.
   */
 void SetPopupImage( PopupParts part, Dali::Image image );

  /**
   * @brief Retrieves the image of the given part used by the popup
   *
   * @param[in] part The part of the popup
   * @return The image used for that part.
   */
  Dali::Image GetPopupImage( PopupParts part );

  void CreateOrderedListOfPopupOptions();

  void CreateBackground();

  void AddOption( Dali::Toolkit::TableView& parent, const std::string& name, const std::string& caption, const Image iconImage, bool finalOption, bool showIcons, bool showCaption, std::size_t& indexInTable  );

  void SetUpPopup();

  void AddPopupOptions( bool createTail, bool showIcons, bool showCaptions );

private: // Implementation

  /**
   * Construct a new TextField.
   */
  TextSelectionPopup();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextSelectionPopup();

private:

  // Undefined copy constructor and assignment operators
  TextSelectionPopup(const TextSelectionPopup&);
  TextSelectionPopup& operator=(const TextSelectionPopup& rhs);

private: // Data

  Dali::Toolkit::TableView mTableOfButtons;                          // Actor which holds all the buttons, sensitivity can be set on buttons via this actor
  Layer mStencilLayer;                                // Layer to enable clipping when buttons exceed popup

  // Images to be used by the Popup
  Image mBackgroundImage;
  Image mCutIconImage;
  Image mCopyIconImage;
  Image mPasteIconImage;
  Image mClipboardIconImage;
  Image mSelectIconImage;
  Image mSelectAllIconImage;

  ImageActor mBackground;                             // The background popup panel
  ImageActor mTail;                                   // The tail for the popup
  ImageActor mTailEffect;   //todo remove                          // the tail effect
  ImageActor mTailLine;     //todo remove                          // The border/outline around the tail

  Size mMaxSize;                                      // Max size of the Popup
  Size mVisiblePopUpSize;                             // Visible Size of popup excluding content that needs scrolling.
  Size mRequiredPopUpSize;                            // Total size of popup including any invisible margin

  Vector4 mNinePatchMargins;                          // Margins between the edge of the cropped image and the nine patch rect (left, right, top, bottom).

  Size mContentSize;                                  // Size of Content (i.e. Buttons)
  //Animation mAnimation;                               // Popup Hide/Show animation.

  std::vector<ButtonRequirement> mOrderListOfButtons; // List of buttons in the order to be displayed and a flag to indicate if needed.

  Vector4 mBackgroundColor;             // Color of the background of the text input popup
  Vector4 mBackgroundPressedColor;      // Color of the option background.
  Vector4 mLineColor;                   // Color of the line around the text input popup
  Vector4 mIconColor;                   // Color of the popup icon.
  Vector4 mIconPressedColor;            // Color of the popup icon when pressed.
  Vector4 mTextColor;                   // Color of the popup text.
  Vector4 mTextPressedColor;            // Color of the popup text when pressed.

  // Priority of Options/Buttons in the Cut and Paste pop-up, higher priority buttons are displayed first, left to right.
  std::size_t mSelectOptionPriority;    // Position of Select Button
  std::size_t mSelectAllOptionPriority; // Position of Select All button
  std::size_t mCutOptionPriority;       // Position of Cut button
  std::size_t mCopyOptionPriority;      // Position of Copy button
  std::size_t mPasteOptionPriority;     // Position of Paste button
  std::size_t mClipboardOptionPriority; // Position of Clipboard button

  bool mShowIcons; // Flag to show icons
  bool mShowCaptions; // Flag to show text captions

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
