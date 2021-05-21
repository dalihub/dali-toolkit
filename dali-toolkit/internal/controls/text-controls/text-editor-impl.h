#ifndef DALI_TOOLKIT_INTERNAL_TEXT_EDITOR_H
#define DALI_TOOLKIT_INTERNAL_TEXT_EDITOR_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/clipboard-event-notifier.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/scroll-bar/scroll-bar.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>
#include <dali-toolkit/internal/text/text-anchor-control-interface.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali-toolkit/internal/text/text-selectable-control-interface.h>
#include <dali-toolkit/internal/text/text-vertical-scroller.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief A control which renders a long text string with styles.
 */
class TextEditor : public Control, public Text::ControlInterface, public Text::EditableControlInterface, public Text::SelectableControlInterface, public Text::AnchorControlInterface
{
public:
  /**
   * @copydoc Dali::Toollkit::TextEditor::New()
   */
  static Toolkit::TextEditor New();

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
   * @copydoc Dali::Toollkit::TextEditor::GetInputMethodContext()
   */
  InputMethodContext GetInputMethodContext();

  /**
   * @copydoc Dali::Toollkit::TextEditor::MaxLengthReachedSignal()
   */
  DevelTextEditor::MaxLengthReachedSignalType& MaxLengthReachedSignal();

  /**
   * @copydoc Dali::Toollkit::TextEditor::AnchorClickedSignal()
   */
  DevelTextEditor::AnchorClickedSignalType& AnchorClickedSignal();

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
   * @copydoc TextEditor::TextChangedSignal()
   */
  Toolkit::TextEditor::TextChangedSignalType& TextChangedSignal();

  /**
   * @copydoc TextEditor::TextChangedSignal()
   */
  Toolkit::TextEditor::InputStyleChangedSignalType& InputStyleChangedSignal();

  /**
   * @copydoc TextEditor::ScrollStateChangedSignal()
   */
  Toolkit::TextEditor::ScrollStateChangedSignalType& ScrollStateChangedSignal();

private: // From Control
  /**
   * @copydoc Control::OnInitialize()
   */
  void OnInitialize() override;

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
   * @copydoc Text::EditableControlInterface::CaretMoved()
   */
  void CaretMoved(unsigned int position) override;

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
   * @copydoc Text::EditableControlInterface::AddDecoration()
   */
  void AddDecoration(Actor& actor, bool needsClipping) override;

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
   * @copydoc Dali::Toolkit::DevelTextEditor::ScrollBy()
   */
  void ScrollBy(Vector2 Scroll);

  /**
   * @brief Get Horizontal scroll position of TextEditor.
   *
   * @return Horizontal scroll position (in pixels) of TextEditor.
   */
  float GetHorizontalScrollPosition();

  /**
   * @brief Get Vertical scroll position of TextEditor.
   *
   * @return Vertical scroll position (in pixels) of TextEditor.
   */
  float GetVerticalScrollPosition();

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

  // From AnchorControlInterface

  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  void AnchorClicked(const std::string& href) override;

  Text::ControllerPtr getController();

private: // Implementation
  /**
   * @copydoc Dali::Toolkit::Text::Controller::(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent);

  /**
   * @brief Callback when Clipboard signals an item should be pasted
   * @param[in] clipboard handle to Clipboard Event Notifier
   */
  void OnClipboardTextSelected(ClipboardEventNotifier& clipboard);

  /**
   * @brief Get a Property Map for the image used for the required Handle Image
   * @param[out] value the returned image property
   * @param[in] handleType the type of handle
   * @param[in] handleImageType the type of image for the given handleType
   */
  void GetHandleImagePropertyValue(Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType);

  /**
   * @brief Callback when keyboard is shown/hidden.
   *
   * @param[in] keyboardShown True if keyboard is shown.
   */
  void KeyboardStatusChanged(bool keyboardShown);

  /**
   * @brief update scroll bar position
   *
   * If text scroll is occurred, create or update scroll bar position
   */
  void UpdateScrollBar();

  /**
   * @brief Callback when TextEditor is touched
   *
   * @param[in] actor TextEditor touched
   * @param[in] touch Touch information
   */
  bool OnTouched(Actor actor, const TouchEvent& touch);

  /**
   * @brief Callbacks called on idle.
   *
   * If there are notifications of change of input style on the queue, Toolkit::TextEditor::InputStyleChangedSignal() are emitted.
   */
  void OnIdleSignal();

  /**
   * @brief Emits TextChanged signal.
   */
  void EmitTextChangedSignal();

  /**
   * @brief set RenderActor's position with new scrollPosition
   *
   * Apply updated scroll position or start scroll animation if VerticalScrollAnimation is enabled
   */
  void ApplyScrollPosition();

  /**
   * @brief Callback function for ScrollBar indicator animation finished signal
   *
   * Emit ScrollBarStateChanged Signal and toggle mScrollStarted flag to false
   */
  void OnScrollIndicatorAnimationFinished(Animation& animation);

  /**
   * Construct a new TextEditor.
   */
  TextEditor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextEditor();

  // Undefined copy constructor and assignment operators
  TextEditor(const TextEditor&);
  TextEditor& operator=(const TextEditor& rhs);

  /**
   * @brief Resize actor to the given size.
   *
   * @param[in] actor The actor to be resized.
   * @param[in] size Size to change.
   */
  void ResizeActor(Actor& actor, const Vector2& size);

  /**
   * @brief Render view, create and attach actor(s) to this text editor.
   */
  void RenderText(Text::Controller::UpdateTextType updateTextType);

  // Connection needed to re-render text, when a text editor returns to the scene.
  void OnSceneConnect(Dali::Actor actor);

private: // Data
  // Signals
  Toolkit::TextEditor::TextChangedSignalType           mTextChangedSignal;
  Toolkit::TextEditor::InputStyleChangedSignalType     mInputStyleChangedSignal;
  Toolkit::TextEditor::ScrollStateChangedSignalType    mScrollStateChangedSignal;
  Toolkit::DevelTextEditor::MaxLengthReachedSignalType mMaxLengthReachedSignal;
  Toolkit::DevelTextEditor::AnchorClickedSignalType    mAnchorClickedSignal;

  InputMethodContext            mInputMethodContext;
  Text::ControllerPtr           mController;
  Text::RendererPtr             mRenderer;
  Text::DecoratorPtr            mDecorator;
  Text::TextVerticalScrollerPtr mTextVerticalScroller;
  Toolkit::Control              mStencil;
  Toolkit::ScrollBar            mScrollBar;
  Dali::Animation               mAnimation; ///< Scroll indicator Show/Hide Animation.
  Dali::TimePeriod              mAnimationPeriod;
  std::vector<Actor>            mClippingDecorationActors; ///< Decoration actors which need clipping.
  Dali::InputMethodOptions      mInputMethodOptions;

  Actor         mRenderableActor;
  Actor         mActiveLayer;
  CallbackBase* mIdleCallback;

  float mAlignmentOffset;
  float mScrollAnimationDuration;
  float mLineSpacing;
  int   mRenderingBackend;
  bool  mHasBeenStaged : 1;
  bool  mScrollAnimationEnabled : 1;
  bool  mScrollBarEnabled : 1;
  bool  mScrollStarted : 1;
  bool  mTextChanged : 1; ///< If true, emits TextChangedSignal in next OnRelayout().

  struct AccessibleImpl : public DevelControl::AccessibleImpl,
                          public virtual Dali::Accessibility::Text,
                          public virtual Dali::Accessibility::EditableText
  {
    using DevelControl::AccessibleImpl::AccessibleImpl;

    std::string           GetName() override;
    std::string           GetText(size_t startOffset, size_t endOffset) override;
    size_t                GetCharacterCount() override;
    size_t                GetCaretOffset() override;
    bool                  SetCaretOffset(size_t offset) override;
    Accessibility::Range  GetTextAtOffset(size_t offset, Accessibility::TextBoundary boundary) override;
    Accessibility::Range  GetSelection(size_t selectionNum) override;
    bool                  RemoveSelection(size_t selectionNum) override;
    bool                  SetSelection(size_t selectionNum, size_t startOffset, size_t endOffset) override;
    bool                  CopyText(size_t startPosition, size_t endPosition) override;
    bool                  CutText(size_t startPosition, size_t endPosition) override;
    Accessibility::States CalculateStates() override;
  };
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextEditor& GetImpl(Toolkit::TextEditor& textEditor)
{
  DALI_ASSERT_ALWAYS(textEditor);

  Dali::RefObject& handle = textEditor.GetImplementation();

  return static_cast<Toolkit::Internal::TextEditor&>(handle);
}

inline const Toolkit::Internal::TextEditor& GetImpl(const Toolkit::TextEditor& textEditor)
{
  DALI_ASSERT_ALWAYS(textEditor);

  const Dali::RefObject& handle = textEditor.GetImplementation();

  return static_cast<const Toolkit::Internal::TextEditor&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_EDITOR_H
