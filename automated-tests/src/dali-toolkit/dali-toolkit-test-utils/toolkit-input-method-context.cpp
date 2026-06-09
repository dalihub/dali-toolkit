/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "toolkit-input-method-context.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/input-method-options.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class RenderSurface;

class InputMethodContext : public Dali::BaseObject
{
public:
  using ActivatedSignalType              = Dali::InputMethodContext::ActivatedSignalType;
  using KeyboardEventSignalType          = Dali::Integration::InputMethodContext::KeyboardEventSignalType;
  using StatusChangedSignalType          = Dali::InputMethodContext::StatusChangedSignalType;
  using KeyboardResizedSignalType        = Dali::InputMethodContext::KeyboardResizedSignalType;
  using LanguageChangedSignalType        = Dali::InputMethodContext::LanguageChangedSignalType;
  using KeyboardTypeChangedSignalType    = Dali::InputMethodContext::KeyboardTypeChangedSignalType;
  using PrivateCommandReceivedSignalType = Dali::InputMethodContext::PrivateCommandReceivedSignalType;

public:
  static Dali::InputMethodContext Create();

  InputMethodContext();
  void Finalize();
  void ConnectCallbacks();
  void DisconnectCallbacks();
  void Activate();
  void Deactivate();
  void Reset();

  bool IsRestoreAfterFocusLostEnabled() const;
  bool SetRestoreAfterFocusLostEnabled(bool enabled);
  bool SetReturnKeyEnabled(bool enabled);
  bool IsReturnKeyEnabled() const;

  void         NotifyCursorPosition();
  void         SetCursorPosition(uint32_t cursorPosition);
  uint32_t     GetCursorPosition() const;
  void         SetSurroundingText(const Dali::String& text);
  Dali::String GetSurroundingText() const;
  void         NotifyTextInputMultiLine(bool multiLine);
  void         ApplyOptions(const Dali::Integration::InputMethodOptions& options);
  bool         FilterEventKey(const Dali::KeyEvent& keyEvent);
  void         GetPreeditStyle(Dali::Integration::InputMethodContext::PreEditAttributeDataContainer& attrs) const;
  void         SetPreeditStyle(Dali::Integration::InputMethodContext::PreeditStyle type);

  bool                                      SetInputPanelLayout(Dali::InputMethod::PanelLayout layout);
  Dali::InputMethod::PanelLayout      GetInputPanelLayout() const;
  bool                                      SetInputPanelReturnKeyType(Dali::InputMethod::ReturnKeyType action);
  Dali::InputMethod::ReturnKeyType     GetInputPanelReturnKeyType() const;
  bool                                      SetInputPanelAutoCapitalType(Dali::InputMethod::AutoCapitalType autoCapital);
  Dali::InputMethod::AutoCapitalType      GetInputPanelAutoCapitalType() const;
  bool                                      SetInputPanelLayoutVariation(Dali::InputMethod::PanelLayoutVariation variation);
  Dali::InputMethod::PanelLayoutVariation GetInputPanelLayoutVariation() const;

public: // Signals
  ActivatedSignalType& ActivatedSignal()
  {
    return mActivatedSignal;
  }
  KeyboardEventSignalType& EventReceivedSignal()
  {
    return mEventSignal;
  }
  KeyboardEventSignalType& KeyboardEventReceivedSignal()
  {
    return mKeyboardEventSignal;
  }
  StatusChangedSignalType& StatusChangedSignal()
  {
    return mKeyboardStatusSignal;
  }
  KeyboardResizedSignalType& ResizedSignal()
  {
    return mKeyboardResizeSignal;
  }
  LanguageChangedSignalType& LanguageChangedSignal()
  {
    return mKeyboardLanguageChangedSignal;
  }
  KeyboardTypeChangedSignalType& KeyboardTypeChangedSignal()
  {
    return mKeyboardTypeChangedSignal;
  }
  PrivateCommandReceivedSignalType& PrivateCommandReceivedSignal()
  {
    return mPrivateCommandReceivedSignal;
  }

protected:
  ~InputMethodContext() override;

private:
  void CreateContext();
  void DeleteContext();

private:
  InputMethodContext(const InputMethodContext&);
  InputMethodContext& operator=(InputMethodContext&);

private:
  uint32_t                                                            mIMFCursorPosition;
  Dali::String                                                        mSurroundingText;
  bool                                                                mRestoreAfterFocusLost : 1;
  bool                                                                mReturnKeyEnabled : 1;
  bool                                                                mIdleCallbackConnected : 1;
  Dali::Integration::InputMethodContext::PreEditAttributeDataContainer mPreeditAttrs;

  Dali::InputMethod::PanelLayout          mPanelLayout;
  Dali::InputMethod::ReturnKeyType         mButtonAction;
  Dali::InputMethod::AutoCapitalType          mAutoCapital;
  Dali::InputMethod::PanelLayoutVariation mPanelLayoutVariation;

  ActivatedSignalType              mActivatedSignal;
  KeyboardEventSignalType          mEventSignal;
  KeyboardEventSignalType          mKeyboardEventSignal;
  StatusChangedSignalType          mKeyboardStatusSignal;
  KeyboardResizedSignalType        mKeyboardResizeSignal;
  LanguageChangedSignalType        mKeyboardLanguageChangedSignal;
  KeyboardTypeChangedSignalType    mKeyboardTypeChangedSignal;
  PrivateCommandReceivedSignalType mPrivateCommandReceivedSignal;

  static Dali::InputMethodContext mToolkitInputMethodContext;

public:
  inline static Internal::Adaptor::InputMethodContext& GetImplementation(Dali::InputMethodContext& inputMethodContext)
  {
    BaseObject& handle = inputMethodContext.GetBaseObject();
    return static_cast<Internal::Adaptor::InputMethodContext&>(handle);
  }

  inline static const Internal::Adaptor::InputMethodContext& GetImplementation(const Dali::InputMethodContext& inputMethodContext)
  {
    const BaseObject& handle = inputMethodContext.GetBaseObject();
    return static_cast<const Internal::Adaptor::InputMethodContext&>(handle);
  }
};

Dali::InputMethodContext Dali::Internal::Adaptor::InputMethodContext::mToolkitInputMethodContext;

Dali::InputMethodContext InputMethodContext::Create()
{
  if(!mToolkitInputMethodContext)
  {
    mToolkitInputMethodContext = Dali::InputMethodContext(new Dali::Internal::Adaptor::InputMethodContext());
  }
  return mToolkitInputMethodContext;
}

InputMethodContext::InputMethodContext()
: mIMFCursorPosition(0u),
  mSurroundingText(),
  mRestoreAfterFocusLost(false),
  mReturnKeyEnabled(true),
  mIdleCallbackConnected(false),
  mPreeditAttrs(),
  mPanelLayout(Dali::InputMethod::PanelLayout::NORMAL),
  mButtonAction(Dali::InputMethod::ReturnKeyType::DEFAULT),
  mAutoCapital(Dali::InputMethod::AutoCapitalType::SENTENCE),
  mPanelLayoutVariation(Dali::InputMethod::PanelLayoutVariation::NORMAL_NORMAL)
{
  CreateContext();
  ConnectCallbacks();
}

InputMethodContext::~InputMethodContext()
{
  DisconnectCallbacks();
  DeleteContext();
}

void InputMethodContext::Finalize()
{
}

void InputMethodContext::CreateContext()
{
}

void InputMethodContext::DeleteContext()
{
}

void InputMethodContext::ConnectCallbacks()
{
}

void InputMethodContext::DisconnectCallbacks()
{
}

void InputMethodContext::Activate()
{
}

void InputMethodContext::Deactivate()
{
}

void InputMethodContext::Reset()
{
}

bool InputMethodContext::IsRestoreAfterFocusLostEnabled() const
{
  return mRestoreAfterFocusLost;
}

bool InputMethodContext::SetRestoreAfterFocusLostEnabled(bool enabled)
{
  mRestoreAfterFocusLost = enabled;
  return true;
}

bool InputMethodContext::SetReturnKeyEnabled(bool enabled)
{
  mReturnKeyEnabled = enabled;
  return true;
}

bool InputMethodContext::IsReturnKeyEnabled() const
{
  return mReturnKeyEnabled;
}

void InputMethodContext::NotifyCursorPosition()
{
}

void InputMethodContext::SetCursorPosition(uint32_t cursorPosition)
{
  mIMFCursorPosition = cursorPosition;
}

uint32_t InputMethodContext::GetCursorPosition() const
{
  return mIMFCursorPosition;
}

void InputMethodContext::SetSurroundingText(const Dali::String& text)
{
  mSurroundingText = text;
}

Dali::String InputMethodContext::GetSurroundingText() const
{
  return mSurroundingText;
}

void InputMethodContext::NotifyTextInputMultiLine(bool multiLine)
{
}

void InputMethodContext::ApplyOptions(const Dali::Integration::InputMethodOptions& options)
{
  (void)options;
}

bool InputMethodContext::FilterEventKey(const Dali::KeyEvent& keyEvent)
{
  return false;
}

void InputMethodContext::GetPreeditStyle(Dali::Integration::InputMethodContext::PreEditAttributeDataContainer& attrs) const
{
  attrs = mPreeditAttrs;
}

void InputMethodContext::SetPreeditStyle(Dali::Integration::InputMethodContext::PreeditStyle type)
{
  Dali::Integration::InputMethodContext::PreeditAttributeData data;
  data.preeditType = type;
  mPreeditAttrs.PushBack(data);
}

bool InputMethodContext::SetInputPanelLayout(Dali::InputMethod::PanelLayout layout)
{
  mPanelLayout = layout;
  return true;
}

Dali::InputMethod::PanelLayout InputMethodContext::GetInputPanelLayout() const
{
  return mPanelLayout;
}

bool InputMethodContext::SetInputPanelReturnKeyType(Dali::InputMethod::ReturnKeyType action)
{
  mButtonAction = action;
  return true;
}

Dali::InputMethod::ReturnKeyType InputMethodContext::GetInputPanelReturnKeyType() const
{
  return mButtonAction;
}

bool InputMethodContext::SetInputPanelAutoCapitalType(Dali::InputMethod::AutoCapitalType autoCapital)
{
  mAutoCapital = autoCapital;
  return true;
}

Dali::InputMethod::AutoCapitalType InputMethodContext::GetInputPanelAutoCapitalType() const
{
  return mAutoCapital;
}

bool InputMethodContext::SetInputPanelLayoutVariation(Dali::InputMethod::PanelLayoutVariation variation)
{
  mPanelLayoutVariation = variation;
  return true;
}

Dali::InputMethod::PanelLayoutVariation InputMethodContext::GetInputPanelLayoutVariation() const
{
  return mPanelLayoutVariation;
}
} // namespace Adaptor
} // namespace Internal

InputMethodContext::InputMethodContext()
{
}

InputMethodContext::~InputMethodContext()
{
}

InputMethodContext InputMethodContext::DownCast(BaseHandle handle)
{
  return InputMethodContext(dynamic_cast<Internal::Adaptor::InputMethodContext*>(handle.GetObjectPtr()));
}

bool InputMethodContext::IsRestoreAfterFocusLostEnabled() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).IsRestoreAfterFocusLostEnabled();
}

bool InputMethodContext::SetRestoreAfterFocusLostEnabled(bool enabled)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetRestoreAfterFocusLostEnabled(enabled);
}

Dali::BoundsInteger InputMethodContext::GetInputPanelArea()
{
  return Dali::BoundsInteger();
}

bool InputMethodContext::SetInputPanelUserData(const Dali::String& data)
{
  return true;
}

Dali::String InputMethodContext::GetInputPanelUserData() const
{
  return Dali::String();
}

InputMethodContext::State InputMethodContext::GetInputPanelState() const
{
  return DEFAULT;
}

bool InputMethodContext::SetReturnKeyEnabled(bool enabled)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetReturnKeyEnabled(enabled);
}

bool InputMethodContext::IsReturnKeyEnabled() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).IsReturnKeyEnabled();
}

bool InputMethodContext::SetInputPanelAutoShowEnabled(bool enabled)
{
  return true;
}

bool InputMethodContext::ShowInputPanel()
{
  return true;
}

bool InputMethodContext::HideInputPanel()
{
  return true;
}

InputMethodContext::KeyboardType InputMethodContext::GetKeyboardType() const
{
  return SOFTWARE_KEYBOARD;
}

bool InputMethodContext::SetInputPanelLanguageLocale(const Dali::String& locale)
{
  return false;
}

Dali::String InputMethodContext::GetInputPanelLanguageLocale() const
{
  return Dali::String();
}

bool InputMethodContext::SetTextPredictionEnabled(bool enabled)
{
  return true;
}

bool InputMethodContext::IsTextPredictionEnabled() const
{
  return false;
}

bool InputMethodContext::SetFullScreenModeEnabled(bool enabled)
{
  return true;
}

bool InputMethodContext::IsFullScreenModeEnabled() const
{
  return false;
}

bool InputMethodContext::SetInputPanelPosition(uint32_t x, uint32_t y)
{
  return true;
}

bool InputMethodContext::SetInputPanelPositionAlign(int32_t x, int32_t y, InputPanelAlign align)
{
  return false;
}

bool InputMethodContext::SetInputPanelLayout(Dali::InputMethod::PanelLayout layout)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelLayout(layout);
}

Dali::InputMethod::PanelLayout InputMethodContext::GetInputPanelLayout() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelLayout();
}

bool InputMethodContext::SetInputPanelReturnKeyType(Dali::InputMethod::ReturnKeyType action)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelReturnKeyType(action);
}

Dali::InputMethod::ReturnKeyType InputMethodContext::GetInputPanelReturnKeyType() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelReturnKeyType();
}

bool InputMethodContext::SetInputPanelAutoCapitalType(Dali::InputMethod::AutoCapitalType autoCapital)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelAutoCapitalType(autoCapital);
}

Dali::InputMethod::AutoCapitalType InputMethodContext::GetInputPanelAutoCapitalType() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelAutoCapitalType();
}

bool InputMethodContext::SetInputPanelLayoutVariation(Dali::InputMethod::PanelLayoutVariation variation)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelLayoutVariation(variation);
}

Dali::InputMethod::PanelLayoutVariation InputMethodContext::GetInputPanelLayoutVariation() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelLayoutVariation();
}

InputMethodContext::ActivatedSignalType& InputMethodContext::ActivatedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).ActivatedSignal();
}

InputMethodContext::StatusChangedSignalType& InputMethodContext::StatusChangedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).StatusChangedSignal();
}

InputMethodContext::KeyboardResizedSignalType& InputMethodContext::ResizedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).ResizedSignal();
}

InputMethodContext::LanguageChangedSignalType& InputMethodContext::LanguageChangedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).LanguageChangedSignal();
}

InputMethodContext::KeyboardTypeChangedSignalType& InputMethodContext::KeyboardTypeChangedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).KeyboardTypeChangedSignal();
}

InputMethodContext::PrivateCommandReceivedSignalType& InputMethodContext::PrivateCommandReceivedSignal()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).PrivateCommandReceivedSignal();
}

InputMethodContext::InputMethodContext(Internal::Adaptor::InputMethodContext* impl)
: BaseHandle(impl)
{
}

namespace Integration
{
namespace InputMethodContext
{
Dali::InputMethodContext New()
{
  return Dali::Internal::Adaptor::InputMethodContext::Create();
}

Dali::InputMethodContext New(Dali::Actor actor)
{
  return Dali::Internal::Adaptor::InputMethodContext::Create();
}

void Finalize(Dali::InputMethodContext context)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).Finalize();
}

void Activate(Dali::InputMethodContext context)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).Activate();
}

void Deactivate(Dali::InputMethodContext context)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).Deactivate();
}

void Reset(Dali::InputMethodContext context)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).Reset();
}

void NotifyCursorPosition(Dali::InputMethodContext context)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).NotifyCursorPosition();
}

void SetCursorPosition(Dali::InputMethodContext context, uint32_t cursorPosition)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).SetCursorPosition(cursorPosition);
}

uint32_t GetCursorPosition(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).GetCursorPosition();
}

void SetSurroundingText(Dali::InputMethodContext context, const Dali::String& text)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).SetSurroundingText(text);
}

Dali::String GetSurroundingText(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).GetSurroundingText();
}

void NotifyTextInputMultiLine(Dali::InputMethodContext context, bool multiLine)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).NotifyTextInputMultiLine(multiLine);
}

TextDirection GetTextDirection(Dali::InputMethodContext context)
{
  return LEFT_TO_RIGHT;
}

void ApplyOptions(Dali::InputMethodContext context, const Dali::Integration::InputMethodOptions& options)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).ApplyOptions(options);
}

bool SetInputPanelLanguage(Dali::InputMethodContext context, InputPanelLanguage language)
{
  return true;
}

InputPanelLanguage GetInputPanelLanguage(Dali::InputMethodContext context)
{
  return InputPanelLanguage::AUTOMATIC;
}

bool FilterEventKey(Dali::InputMethodContext context, const Dali::KeyEvent& keyEvent)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).FilterEventKey(keyEvent);
}

void GetPreeditStyle(Dali::InputMethodContext context, PreEditAttributeDataContainer& attrs)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).GetPreeditStyle(attrs);
}

KeyboardEventSignalType& EventReceivedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).EventReceivedSignal();
}

KeyboardEventSignalType& KeyboardEventReceivedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).KeyboardEventReceivedSignal();
}

namespace Test
{
void SetPreeditStyle(Dali::InputMethodContext context, PreeditStyle type)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).SetPreeditStyle(type);
}
} // namespace Test
} // namespace InputMethodContext
} // namespace Integration
} // namespace Dali
