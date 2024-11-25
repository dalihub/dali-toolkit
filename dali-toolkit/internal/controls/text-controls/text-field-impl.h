#ifndef DALI_TOOLKIT_INTERNAL_TEXT_FIELD_H
#define DALI_TOOLKIT_INTERNAL_TEXT_FIELD_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/devel-api/atspi-interfaces/editable-text.h>
#include <dali/devel-api/atspi-interfaces/hypertext.h>
#include <dali/devel-api/atspi-interfaces/text.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>
#include <dali-toolkit/internal/text/text-anchor-control-interface.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali-toolkit/internal/text/text-selectable-control-interface.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief A control which renders a short text string.
 */
class TextField : public Control, public Text::ControlInterface, public Text::EditableControlInterface, public Text::SelectableControlInterface, public Text::AnchorControlInterface
{
public:
  /**
   * @copydoc Dali::Toollkit::TextField::New()
   * @param[in] additionalBehaviour custom behavior flags for this TextField. Default is CONTROL_BEHAVIOUR_DEFAULT
   */
  static Toolkit::TextField New(ControlBehaviour additionalBehaviour = ControlBehaviour::CONTROL_BEHAVIOUR_DEFAULT);

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
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
   * @copydoc Dali::Toollkit::TextField::GetInputMethodContext()
   */
  InputMethodContext GetInputMethodContext();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  /**
   * @brief Gets text controller
   *
   * @return The text controller
   */
  Text::ControllerPtr GetTextController();

  /**
   * @copydoc TextField::TextChangedSignal()
   */
  Toolkit::TextField::TextChangedSignalType& TextChangedSignal();

  /**
   * @copydoc TextField::MaxLengthReachedSignal()
   */
  Toolkit::TextField::MaxLengthReachedSignalType& MaxLengthReachedSignal();

  /**
   * @copydoc TextField::TextChangedSignal()
   */
  Toolkit::TextField::InputStyleChangedSignalType& InputStyleChangedSignal();

  /**
   * @copydoc TextField::AnchorClickedSignal()
   */
  DevelTextField::AnchorClickedSignalType& AnchorClickedSignal();

  /**
   * @copydoc TextField::CursorPositionChangedSignal()
   */
  DevelTextField::CursorPositionChangedSignalType& CursorPositionChangedSignal();

  /**
   * @copydoc TextField::InputFilteredSignal()
   */
  DevelTextField::InputFilteredSignalType& InputFilteredSignal();

  /**
   * @copydoc TextField::SelectionChangedSignal()
   */
  DevelTextField::SelectionChangedSignalType& SelectionChangedSignal();

  /**
   * @copydoc TextField::SelectionClearedSignal()
   */
  DevelTextField::SelectionClearedSignalType& SelectionClearedSignal();

  /**
   * @copydoc TextField::SelectionStartedSignal()
   */
  DevelTextField::SelectionStartedSignalType& SelectionStartedSignal();

  /**
   * @brief Registers a new font variation property based on the provided tag.
   *
   * @param[in] tag A 4-character string representing the variation property tag.
   * @return The index of the registered variation property. Property::INVALID_INDEX if failed.
   */
  Dali::Property::Index RegisterFontVariationProperty(std::string tag);

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

  /**
   * @copydoc Control::OnStyleChange()
   */
  void OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change) override;

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Control::OnInitialize()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Control::OnKeyInputFocusGained()
   */
  void OnKeyInputFocusGained() override;

  /**
   * @copydoc Control::OnKeyInputFocusLost()
   */
  void OnKeyInputFocusLost() override;

  /**
   * @copydoc Control::OnAccessibilityActivated()
   */
  bool OnAccessibilityActivated() override;

  /**
   * @copydoc Control::OnTap()
   */
  void OnTap(const TapGesture& tap) override;

  /**
   * @copydoc Control::OnPan()
   */
  void OnPan(const PanGesture& gesture) override;

  /**
   * @copydoc Control::OnLongPress()
   */
  void OnLongPress(const LongPressGesture& gesture) override;

  /**
   * @copydoc Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Control::OnPropertySet()
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnKeyEvent(const KeyEvent&)
   */
  bool OnKeyEvent(const KeyEvent& event) override;

  // From ControlInterface

  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

  // From EditableControlInterface

  /**
   * @copydoc Text::EditableControlInterface::TextChanged()
   */
  void TextInserted(unsigned int position, unsigned int length, const std::string& content) override;

  /**
   * @copydoc Text::EditableControlInterface::TextDeleted()
   */
  void TextDeleted(unsigned int position, unsigned int length, const std::string& content) override;

  /**
   * @copydoc Text::EditableControlInterface::CursorPositionChanged()
   */
  void CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition) override;

  /**
   * @copydoc Text::EditableControlInterface::TextChanged()
   */
  void TextChanged(bool immediate) override;

  /**
   * @copydoc Text::EditableControlInterface::MaxLengthReached()
   */
  void MaxLengthReached() override;

  /**
   * @copydoc Text::EditableControlInterface::InputStyleChanged()
   */
  void InputStyleChanged(Text::InputStyle::Mask inputStyleMask) override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectionChanged()
   */
  void SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd) override;

  /**
   * @copydoc Text::EditableControlInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, Toolkit::Text::DecorationType type, bool needsClipping) override;

  /**
   * @copydoc Text::EditableControlInterface::InputFiltered()
   */
  void InputFiltered(Toolkit::InputFilter::Property::Type type) override;

  /**
   * @copydoc Text::EditableControlInterface::GetControlBackgroundColor()
   */
  void GetControlBackgroundColor(Vector4& color) const override;

  // From SelectableControlInterface
public:
  /**
   * @copydoc Text::SelectableControlInterface::SetTextSelectionRange()
   */
  void SetTextSelectionRange(const uint32_t* start, const uint32_t* end) override;

  /**
   * @copydoc Text::SelectableControlInterface::GetTextSelectionRange()
   */
  Uint32Pair GetTextSelectionRange() const override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectWholeText()
   */
  void SelectWholeText() override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectNone()
   */
  void SelectNone() override;

  /**
   * @copydoc Text::SelectableControlInterface::SelectText()
   */
  void SelectText(const uint32_t start, const uint32_t end) override;

  /**
   * @copydoc Text::SelectableControlInterface::GetSelectedText()
   */
  string GetSelectedText() const override;

  /**
   * @copydoc Text::EditableControlInterface::IsEditable()
   */
  bool IsEditable() const override;

  /**
   * @copydoc Text::EditableControlInterface::SetEditable()
   */
  void SetEditable(bool editable) override;

  /**
   * @copydoc Dali::EditableControlInterface::CopyText()
   */
  string CopyText() override;

  /**
   * @copydoc Dali::EditableControlInterface::CutText()
   */
  string CutText() override;

  /**
   * @copydoc Text::EditableControlInterface::PasteText()
   */
  void PasteText() override;

  // From AnchorControlInterface

  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  void AnchorClicked(const std::string& href) override;

  /**
   * @brief Get the rendered size of a specific text range.
   * if the requested text is at multilines, multiple sizes will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple sizes will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to calculate size for.
   * @param[in] endIndex end index(included) of the text requested to calculate size for.
   * @return list of sizes of the reuested text.
   */
  Vector<Vector2> GetTextSize(const uint32_t startIndex, const uint32_t endIndex) const;

  /**
   * @brief Get the top/left rendered position of a specific text range.
   * if the requested text is at multilines, multiple positions will be returned for each text located in a separate line.
   * if a line contains characters with different directions, multiple positions will be returned for each block of contiguous characters with the same direction.
   *
   * @param[in] startIndex start index of the text requested to get position to.
   * @param[in] endIndex end index(included) of the text requested to get position to.
   * @return list of positions of the requested text.
   */
  Vector<Vector2> GetTextPosition(const uint32_t startIndex, const uint32_t endIndex) const;

  /**
   * @brief Get the line bounding rectangle.
   * if the requested index is out of range or the line is not yet rendered, a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] lineIndex line index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Rect<float> GetLineBoundingRectangle(const uint32_t lineIndex) const;

  /**
   * @brief Get the character bounding rectangle.
   * If the text is not yet rendered or the index > text.Count(); a rect of {0, 0, 0, 0} is returned.
   *
   * @param[in] charIndex character index to which we want to calculate the geometry for.
   * @return bounding rectangle.
   */
  Rect<float> GetCharacterBoundingRectangle(const uint32_t charIndex) const;

  /**
   * @brief Get the character index.
   * If the text is not yet rendered or the text is empty, -1 is returned.
   *
   * @param[in] visualX visual x position.
   * @param[in] visualY visual y position.
   * @return character index.
   */
  int GetCharacterIndexAtPosition(float visualX, float visualY) const;

  /**
   * @brief Gets the bounding box of a specific text range.
   *
   * @param[in] startIndex start index of the text requested to get bounding box to.
   * @param[in] endIndex end index(included) of the text requested to get bounding box to.
   * @return bounding box of the requested text.
   */
  Rect<> GetTextBoundingRectangle(uint32_t startIndex, uint32_t endIndex) const;

  /**
   * @brief Set the @p spannedText into current textField
   * the spanned text contains content (text) and  format (spans with ranges)
   * the text is copied into text-controller and the spans are applied on ranges
   *
   * @param[in] spannedText the text with spans.
   */
  void SetSpannedText(const Text::Spanned& spannedText);

  /**
   * @brief Set removing front inset to TextField.
   *
   * @param[in] remove Whether front inset of TextField has to be removed or not.
   */
  void SetRemoveFrontInset(const bool remove);

  /**
   * @brief Whether front inset of TextField is removed or not.
   *
   * @return True if the front inset of TextField is removed.
   */
  bool IsRemoveFrontInset() const;

  /**
   * @brief Set removing back inset to TextField.
   *
   * @param[in] remove Whether back inset of TextField has to be removed or not.
   */
  void SetRemoveBackInset(const bool remove);

  /**
   * @brief Whether back inset of TextField is removed or not.
   *
   * @return True if the back inset of TextField is removed.
   */
  bool IsRemoveBackInset() const;

private: // Implementation
  /**
   * @copydoc Dali::Toolkit::Text::Controller::(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Get a Property Map for the image used for the required Handle Image
   * @param[out] value the returned image property
   * @param[in] handleType the type of handle
   * @param[in] handleImageType the type of image for the given handleType
   */
  void GetHandleImagePropertyValue(Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType);

  /**
   * @brief Enable or disable clipping.
   */
  void EnableClipping();

  /**
   * @brief Callback when keyboard is shown/hidden.
   *
   * @param[in] keyboardShown True if keyboard is shown.
   */
  void KeyboardStatusChanged(bool keyboardShown);

  /**
   * @brief Callback when TextField is touched
   *
   * @param[in] actor TextField touched
   * @param[in] touch Touch information
   */
  bool OnTouched(Actor actor, const TouchEvent& touch);

  /**
   * @brief Emits TextChanged signal.
   */
  void EmitTextChangedSignal();

  /**
   * @brief Emits CursorPositionChanged signal.
   */
  void EmitCursorPositionChangedSignal();

  /**
   * @brief Emits SelectionChanged signal.
   */
  void EmitSelectionChangedSignal();

  /**
   * @brief Emits SelectionCleared signal.
   */
  void EmitSelectionClearedSignal();

  /**
   * @brief Emits SelectionStarted signal.
   */
  void EmitSelectionStartedSignal();

  /**
   * @brief Callback function for when the layout is changed.
   * @param[in] actor The actor whose layoutDirection is changed.
   * @param[in] type  The layoutDirection.
   */
  void OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type);

  /**
  * @brief Add a layer for active or cursor.
  * @param[in] layer The actor in which to store the layer.
  * @param[in] actor The new layer to add.
  */
  void AddLayer(Actor& layer, Actor& actor);

  /**
   * Construct a new TextField.
   *
   * @param[in] additionalBehaviour additional behaviour flags for this TextField
   */
  TextField(ControlBehaviour additionalBehaviour);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextField();

  // Undefined copy constructor and assignment operators
  TextField(const TextField&);
  TextField& operator=(const TextField& rhs);

  /**
   * @brief Resize actor to the given size.
   *
   * @param[in] actor The actor to be resized.
   * @param[in] size Size to change.
   */
  void ResizeActor(Actor& actor, const Vector2& size);

  /**
   * @brief Render view, create and attach actor(s) to this Text Field.
   */
  void RenderText(Text::Controller::UpdateTextType updateTextType);

  // Connection needed to re-render text, when a Text Field returns to the scene.
  void OnSceneConnect(Dali::Actor actor);

  // Needed to synchronize TextAnchor actors with Anchor objects in text's logical model
  void OnAccessibilityStatusChanged();

  /**
   * @brief Notifies when the font variation property changes to specific value.
   */
  void OnVariationPropertyNotify(PropertyNotification& source);

private: // Data
  // Signals
  Toolkit::TextField::TextChangedSignalType                mTextChangedSignal;
  Toolkit::TextField::MaxLengthReachedSignalType           mMaxLengthReachedSignal;
  Toolkit::TextField::InputStyleChangedSignalType          mInputStyleChangedSignal;
  Toolkit::DevelTextField::AnchorClickedSignalType         mAnchorClickedSignal;
  Toolkit::DevelTextField::InputFilteredSignalType         mInputFilteredSignal;
  Toolkit::DevelTextField::CursorPositionChangedSignalType mCursorPositionChangedSignal;
  Toolkit::DevelTextField::SelectionChangedSignalType      mSelectionChangedSignal;
  Toolkit::DevelTextField::SelectionClearedSignalType      mSelectionClearedSignal;
  Toolkit::DevelTextField::SelectionStartedSignalType      mSelectionStartedSignal;

  // for Font Variations
  std::map<Dali::Property::Index, std::string> mVariationIndexMap;  // Stores [CustomPropertyIndex, tag].

  InputMethodContext               mInputMethodContext;
  Text::ControllerPtr              mController;
  Text::RendererPtr                mRenderer;
  Text::DecoratorPtr               mDecorator;
  Toolkit::Control                 mStencil;                  ///< For EXCEED_POLICY_CLIP
  std::vector<Actor>               mClippingDecorationActors; ///< Decoration actors which need clipping.
  std::vector<Toolkit::TextAnchor> mAnchorActors;
  Dali::InputMethodOptions         mInputMethodOptions;

  Actor mRenderableActor;
  Actor mActiveLayer;
  Actor mCursorLayer;
  Actor mBackgroundActor;

  float mAlignmentOffset;
  int   mRenderingBackend;
  int   mExceedPolicy;
  bool  mHasBeenStaged : 1;
  bool  mTextChanged : 1;           ///< If true, emits TextChangedSignal in next OnRelayout().
  bool  mCursorPositionChanged : 1; ///< If true, emits CursorPositionChangedSignal at the end of OnRelayout().
  bool  mSelectionChanged : 1;      ///< If true, emits SelectionChangedSignal at the end of OnRelayout().
  bool  mSelectionCleared : 1;      ///< If true, emits SelectionClearedSignal at the end of OnRelayout().

  //args for cursor position changed event
  unsigned int mOldPosition;

  //args for selection changed event
  uint32_t mOldSelectionStart;
  uint32_t mOldSelectionEnd;

  bool mSelectionStarted : 1; ///< If true, emits SelectionStartedSignal at the end of OnRelayout().

protected:
  struct PropertyHandler;

  /**
   * @brief This structure is to connect TextField with Accessible functions.
   */
  class TextFieldAccessible : public EditableTextControlAccessible
  {
  public:
    using EditableTextControlAccessible::EditableTextControlAccessible;

    /**
     * @copydoc Dali::Accessibility::Accessible::GetName()
     */
    std::string GetName() const override;

  protected:
    /**
     * @copydoc Dali::Toolkit::Internal::TextControlAccessible::GetTextAnchors()
     */
    const std::vector<Toolkit::TextAnchor>& GetTextAnchors() const override;

    /**
     * @copydoc Dali::Toolkit::Internal::TextControlAccessible::GetTextController()
     */
    Toolkit::Text::ControllerPtr GetTextController() const override;

    /**
     * @copydoc Dali::Toolkit::Internal::TextControlAccessible::GetSubstituteCharacter()
     */
    std::uint32_t GetSubstituteCharacter() const override;

    /**
     * @copydoc Dali::Toolkit::Internal::TextControlAccessible::IsHiddenInput()
     */
    bool IsHiddenInput() const override;

    /**
     * @copydoc Dali::Toolkit::Internal::EditableTextControlAccessible::RequestTextRelayout()
     */
    void RequestTextRelayout() override;
  };
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextField& GetImpl(Toolkit::TextField& textField)
{
  DALI_ASSERT_ALWAYS(textField);

  Dali::RefObject& handle = textField.GetImplementation();

  return static_cast<Toolkit::Internal::TextField&>(handle);
}

inline const Toolkit::Internal::TextField& GetImpl(const Toolkit::TextField& textField)
{
  DALI_ASSERT_ALWAYS(textField);

  const Dali::RefObject& handle = textField.GetImplementation();

  return static_cast<const Toolkit::Internal::TextField&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_FIELD_H
