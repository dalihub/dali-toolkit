#ifndef DALI_TOOLKIT_INTERNAL_TEXT_FIELD_H
#define DALI_TOOLKIT_INTERNAL_TEXT_FIELD_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/clipboard-event-notifier.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-editable-control-interface.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/rendering/text-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
/**
 * @brief A control which renders a short text string.
 */
class TextField : public Control, public Text::ControlInterface, public Text::EditableControlInterface
{
public:

  /**
   * @copydoc Dali::Toollkit::TextField::New()
   */
  static Toolkit::TextField New();

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
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
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * @copydoc TextField::TextChangedSignal()
   */
  Toolkit::TextField::TextChangedSignalType&  TextChangedSignal();

  /**
   * @copydoc TextField::MaxLengthReachedSignal()
   */
  Toolkit::TextField::MaxLengthReachedSignalType&  MaxLengthReachedSignal();

  /**
   * @copydoc TextField::TextChangedSignal()
   */
  Toolkit::TextField::InputStyleChangedSignalType& InputStyleChangedSignal();

  /**
   * @brief Called to select the whole texts.
   */
  void SelectWholeText();

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Control::OnStyleChange()
   */
  virtual void OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change );

  /**
   * @copydoc Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Control::OnKeyInputFocusGained()
   */
  virtual void OnKeyInputFocusGained();

  /**
   * @copydoc Control::OnKeyInputFocusLost()
   */
  virtual void OnKeyInputFocusLost();

  /**
   * @copydoc Control::OnTap()
   */
  virtual void OnTap( const TapGesture& tap );

  /**
   * @copydoc Control::OnPan()
   */
  virtual void OnPan( const PanGesture& gesture );

  /**
   * @copydoc Control::OnLongPress()
   */
  virtual void OnLongPress( const LongPressGesture& gesture );

  /**
   * @copydoc Control::OnStageConnection()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Dali::CustomActorImpl::OnKeyEvent(const KeyEvent&)
   */
  virtual bool OnKeyEvent(const KeyEvent& event);

// From ControlInterface

  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  virtual void RequestTextRelayout();

// From EditableControlInterface

  /**
   * @copydoc Text::ControlInterface::TextChanged()
   */
  virtual void TextChanged();

  /**
   * @copydoc Text::ControlInterface::MaxLengthReached()
   */
  virtual void MaxLengthReached();

  /**
   * @copydoc Text::ControlInterface::InputStyleChanged()
   */
  virtual void InputStyleChanged( Text::InputStyle::Mask inputStyleMask );

  /**
   * @copydoc Text::ControlInterface::AddDecoration()
   */
  virtual void AddDecoration( Actor& actor, bool needsClipping );

private: // Implementation

  /**
   * @copydoc Dali::Toolkit::Text::Controller::(InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
   */
  InputMethodContext::CallbackData OnInputMethodContextEvent( InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent );

  /**
   * @brief Callback when Clipboard signals an item should be pasted
   * @param[in] clipboard handle to Clipboard Event Notifier
   */
  void OnClipboardTextSelected( ClipboardEventNotifier& clipboard );

  /**
   * @brief Get a Property Map for the image used for the required Handle Image
   * @param[out] value the returned image property
   * @param[in] handleType the type of handle
   * @param[in] handleImageType the type of image for the given handleType
   */
  void GetHandleImagePropertyValue(  Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType );

  /**
   * @brief Enable or disable clipping.
   */
  void EnableClipping();

  /**
   * @brief Callback when keyboard is shown/hidden.
   *
   * @param[in] keyboardShown True if keyboard is shown.
   */
  void KeyboardStatusChanged( bool keyboardShown );

  /**
   * @brief Callback when TextField is touched
   *
   * @param[in] actor TextField touched
   * @param[in] touch Touch information
   */
  bool OnTouched( Actor actor, const TouchData& touch );

  /**
   * @brief Callbacks called on idle.
   *
   * If there are notifications of change of input style on the queue, Toolkit::TextField::InputStyleChangedSignal() are emitted.
   */
  void OnIdleSignal();

  /**
   * Construct a new TextField.
   */
  TextField();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextField();

  // Undefined copy constructor and assignment operators
  TextField(const TextField&);
  TextField& operator=(const TextField& rhs);

  /**
   * @brief Render view, create and attach actor(s) to this Text Field.
   */
  void RenderText( Text::Controller::UpdateTextType updateTextType );

  // Connection needed to re-render text, when a Text Field returns to the stage.
  void OnStageConnect( Dali::Actor actor );

private: // Data

  // Signals
  Toolkit::TextField::TextChangedSignalType mTextChangedSignal;
  Toolkit::TextField::MaxLengthReachedSignalType mMaxLengthReachedSignal;
  Toolkit::TextField::InputStyleChangedSignalType mInputStyleChangedSignal;

  InputMethodContext mInputMethodContext;
  Text::ControllerPtr mController;
  Text::RendererPtr mRenderer;
  Text::DecoratorPtr mDecorator;
  Toolkit::Control mStencil; ///< For EXCEED_POLICY_CLIP
  std::vector<Actor> mClippingDecorationActors;   ///< Decoration actors which need clipping.
  Dali::InputMethodOptions mInputMethodOptions;

  Actor mRenderableActor;
  Actor mActiveLayer;
  CallbackBase* mIdleCallback;

  float mAlignmentOffset;
  int mRenderingBackend;
  int mExceedPolicy;
  bool mHasBeenStaged:1;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextField& GetImpl( Toolkit::TextField& textField )
{
  DALI_ASSERT_ALWAYS(textField);

  Dali::RefObject& handle = textField.GetImplementation();

  return static_cast<Toolkit::Internal::TextField&>(handle);
}

inline const Toolkit::Internal::TextField& GetImpl( const Toolkit::TextField& textField )
{
  DALI_ASSERT_ALWAYS(textField);

  const Dali::RefObject& handle = textField.GetImplementation();

  return static_cast<const Toolkit::Internal::TextField&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_FIELD_H
