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
  using ContentReceivedSignalType        = Dali::Integration::InputMethodContext::ContentReceivedSignalType;
  using StatusChangedSignalType          = Dali::InputMethodContext::StatusChangedSignalType;
  using KeyboardResizedSignalType        = Dali::InputMethodContext::KeyboardResizedSignalType;
  using LanguageChangedSignalType        = Dali::InputMethodContext::LanguageChangedSignalType;
  using KeyboardTypeChangedSignalType    = Dali::InputMethodContext::KeyboardTypeChangedSignalType;
  using PrivateCommandReceivedSignalType = Dali::InputMethodContext::PrivateCommandReceivedSignalType;
  using LegacyStatusChangedSignalType       = Dali::Integration::InputMethodContext::LegacyStatusChangedSignalType;
  using LegacyKeyboardTypeChangedSignalType = Dali::Integration::InputMethodContext::LegacyKeyboardTypeChangedSignalType;
  using LegacyKeyboardResizedSignalType     = Dali::Integration::InputMethodContext::LegacyKeyboardResizedSignalType;
  using LegacyLanguageChangedSignalType     = Dali::Integration::InputMethodContext::LegacyLanguageChangedSignalType;
  using LegacyContentReceivedSignalType     = Dali::Integration::InputMethodContext::LegacyContentReceivedSignalType;

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
  Dali::BoundsInteger GetInputPanelArea() const;
  bool                SetInputPanelUserData(const Dali::String& data);
  Dali::String        GetInputPanelUserData() const;
  Dali::InputMethodContext::State GetInputPanelState() const;
  bool                            SetInputPanelAutoShowEnabled(bool enabled);
  bool                            ShowInputPanel();
  bool                            HideInputPanel();
  Dali::InputMethodContext::KeyboardType GetKeyboardType() const;
  bool                                   SetInputPanelLanguageLocale(const Dali::String& locale);
  Dali::String                           GetInputPanelLanguageLocale() const;
  bool                                   SetTextPredictionEnabled(bool enabled);
  bool                                   IsTextPredictionEnabled() const;
  bool                                   SetFullScreenModeEnabled(bool enabled);
  bool                                   IsFullScreenModeEnabled() const;
  bool                                   SetInputPanelPosition(uint32_t x, uint32_t y);
  bool                                   SetInputPanelPositionAlign(int32_t x, int32_t y, Dali::InputMethodContext::InputPanelAlign align);

  void         NotifyCursorPosition();
  void         SetCursorPosition(uint32_t cursorPosition);
  uint32_t     GetCursorPosition() const;
  void         SetSurroundingText(const Dali::String& text);
  Dali::String GetSurroundingText() const;
  void         NotifyTextInputMultiLine(bool multiLine);
  Dali::Integration::InputMethodContext::TextDirection GetTextDirection() const;
  void         SetContentMimeTypes(const Dali::String& mimeTypes);
  void         ApplyOptions(const Dali::Integration::InputMethodOptions& options);
  bool         SetInputPanelLanguage(Dali::Integration::InputMethodContext::InputPanelLanguage language);
  Dali::Integration::InputMethodContext::InputPanelLanguage GetInputPanelLanguage() const;
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
  ContentReceivedSignalType& ContentReceivedSignal()
  {
    return mContentReceivedSignal;
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
  LegacyStatusChangedSignalType& LegacyStatusChangedSignal()
  {
    return mLegacyKeyboardStatusSignal;
  }
  LegacyKeyboardTypeChangedSignalType& LegacyKeyboardTypeChangedSignal()
  {
    return mLegacyKeyboardTypeChangedSignal;
  }
  LegacyKeyboardResizedSignalType& LegacyResizedSignal()
  {
    return mLegacyKeyboardResizeSignal;
  }
  LegacyLanguageChangedSignalType& LegacyLanguageChangedSignal()
  {
    return mLegacyKeyboardLanguageChangedSignal;
  }
  LegacyContentReceivedSignalType& LegacyContentReceivedSignal()
  {
    return mLegacyContentReceivedSignal;
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
  Dali::String                                                        mInputPanelUserData;
  Dali::String                                                        mLanguageLocale;
  Dali::String                                                        mContentMimeTypes;
  Dali::BoundsInteger                                                 mInputPanelArea;
  Dali::InputMethodContext::State                                     mInputPanelState;
  Dali::InputMethodContext::KeyboardType                              mKeyboardType;
  Dali::Integration::InputMethodContext::InputPanelLanguage           mInputPanelLanguage;
  bool                                                                mRestoreAfterFocusLost : 1;
  bool                                                                mReturnKeyEnabled : 1;
  bool                                                                mAutoShowEnabled : 1;
  bool                                                                mTextPredictionEnabled : 1;
  bool                                                                mFullScreenModeEnabled : 1;
  bool                                                                mIdleCallbackConnected : 1;
  Dali::Integration::InputMethodContext::PreEditAttributeDataContainer mPreeditAttrs;

  Dali::InputMethod::PanelLayout          mPanelLayout;
  Dali::InputMethod::ReturnKeyType         mButtonAction;
  Dali::InputMethod::AutoCapitalType          mAutoCapital;
  Dali::InputMethod::PanelLayoutVariation mPanelLayoutVariation;

  ActivatedSignalType              mActivatedSignal;
  KeyboardEventSignalType          mEventSignal;
  KeyboardEventSignalType          mKeyboardEventSignal;
  ContentReceivedSignalType        mContentReceivedSignal;
  StatusChangedSignalType          mKeyboardStatusSignal;
  KeyboardResizedSignalType        mKeyboardResizeSignal;
  LanguageChangedSignalType        mKeyboardLanguageChangedSignal;
  KeyboardTypeChangedSignalType    mKeyboardTypeChangedSignal;
  PrivateCommandReceivedSignalType mPrivateCommandReceivedSignal;
  LegacyStatusChangedSignalType       mLegacyKeyboardStatusSignal;
  LegacyKeyboardTypeChangedSignalType mLegacyKeyboardTypeChangedSignal;
  LegacyKeyboardResizedSignalType     mLegacyKeyboardResizeSignal;
  LegacyLanguageChangedSignalType     mLegacyKeyboardLanguageChangedSignal;
  LegacyContentReceivedSignalType     mLegacyContentReceivedSignal;

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
  mInputPanelUserData(),
  mLanguageLocale(),
  mContentMimeTypes(),
  mInputPanelArea(),
  mInputPanelState(Dali::InputMethodContext::State::HIDE),
  mKeyboardType(Dali::InputMethodContext::KeyboardType::SOFTWARE_KEYBOARD),
  mInputPanelLanguage(Dali::Integration::InputMethodContext::InputPanelLanguage::AUTOMATIC),
  mRestoreAfterFocusLost(false),
  mReturnKeyEnabled(true),
  mAutoShowEnabled(false),
  mTextPredictionEnabled(false),
  mFullScreenModeEnabled(false),
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

Dali::BoundsInteger InputMethodContext::GetInputPanelArea() const
{
  return mInputPanelArea;
}

bool InputMethodContext::SetInputPanelUserData(const Dali::String& data)
{
  mInputPanelUserData = data;
  return true;
}

Dali::String InputMethodContext::GetInputPanelUserData() const
{
  return mInputPanelUserData;
}

Dali::InputMethodContext::State InputMethodContext::GetInputPanelState() const
{
  return mInputPanelState;
}

bool InputMethodContext::SetInputPanelAutoShowEnabled(bool enabled)
{
  mAutoShowEnabled = enabled;
  return true;
}

bool InputMethodContext::ShowInputPanel()
{
  mInputPanelState = Dali::InputMethodContext::State::SHOW;
  return true;
}

bool InputMethodContext::HideInputPanel()
{
  mInputPanelState = Dali::InputMethodContext::State::HIDE;
  return true;
}

Dali::InputMethodContext::KeyboardType InputMethodContext::GetKeyboardType() const
{
  return mKeyboardType;
}

bool InputMethodContext::SetInputPanelLanguageLocale(const Dali::String& locale)
{
  mLanguageLocale = locale;
  return true;
}

Dali::String InputMethodContext::GetInputPanelLanguageLocale() const
{
  return mLanguageLocale;
}

bool InputMethodContext::SetTextPredictionEnabled(bool enabled)
{
  mTextPredictionEnabled = enabled;
  return true;
}

bool InputMethodContext::IsTextPredictionEnabled() const
{
  return mTextPredictionEnabled;
}

bool InputMethodContext::SetFullScreenModeEnabled(bool enabled)
{
  mFullScreenModeEnabled = enabled;
  return true;
}

bool InputMethodContext::IsFullScreenModeEnabled() const
{
  return mFullScreenModeEnabled;
}

bool InputMethodContext::SetInputPanelPosition(uint32_t x, uint32_t y)
{
  mInputPanelArea.x = static_cast<int32_t>(x);
  mInputPanelArea.y = static_cast<int32_t>(y);
  return true;
}

bool InputMethodContext::SetInputPanelPositionAlign(int32_t x, int32_t y, Dali::InputMethodContext::InputPanelAlign align)
{
  (void)align;
  mInputPanelArea.x = x;
  mInputPanelArea.y = y;
  return true;
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
  (void)multiLine;
}

Dali::Integration::InputMethodContext::TextDirection InputMethodContext::GetTextDirection() const
{
  return Dali::Integration::InputMethodContext::LEFT_TO_RIGHT;
}

void InputMethodContext::SetContentMimeTypes(const Dali::String& mimeTypes)
{
  mContentMimeTypes = mimeTypes;
}

void InputMethodContext::ApplyOptions(const Dali::Integration::InputMethodOptions& options)
{
  (void)options;
}

bool InputMethodContext::SetInputPanelLanguage(Dali::Integration::InputMethodContext::InputPanelLanguage language)
{
  mInputPanelLanguage = language;
  return true;
}

Dali::Integration::InputMethodContext::InputPanelLanguage InputMethodContext::GetInputPanelLanguage() const
{
  return mInputPanelLanguage;
}

bool InputMethodContext::FilterEventKey(const Dali::KeyEvent& keyEvent)
{
  (void)keyEvent;
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

InputMethodContext::InputMethodContext(const InputMethodContext& inputMethodContext)
: BaseHandle(inputMethodContext)
{
}

InputMethodContext& InputMethodContext::operator=(const InputMethodContext& inputMethodContext)
{
  if(*this != inputMethodContext)
  {
    BaseHandle::operator=(inputMethodContext);
  }
  return *this;
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
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelArea();
}

bool InputMethodContext::SetInputPanelUserData(const Dali::String& data)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelUserData(data);
}

Dali::String InputMethodContext::GetInputPanelUserData() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelUserData();
}

InputMethodContext::State InputMethodContext::GetInputPanelState() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelState();
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
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelAutoShowEnabled(enabled);
}

bool InputMethodContext::ShowInputPanel()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).ShowInputPanel();
}

bool InputMethodContext::HideInputPanel()
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).HideInputPanel();
}

InputMethodContext::KeyboardType InputMethodContext::GetKeyboardType() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetKeyboardType();
}

bool InputMethodContext::SetInputPanelLanguageLocale(const Dali::String& locale)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelLanguageLocale(locale);
}

Dali::String InputMethodContext::GetInputPanelLanguageLocale() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).GetInputPanelLanguageLocale();
}

bool InputMethodContext::SetTextPredictionEnabled(bool enabled)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetTextPredictionEnabled(enabled);
}

bool InputMethodContext::IsTextPredictionEnabled() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).IsTextPredictionEnabled();
}

bool InputMethodContext::SetFullScreenModeEnabled(bool enabled)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetFullScreenModeEnabled(enabled);
}

bool InputMethodContext::IsFullScreenModeEnabled() const
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).IsFullScreenModeEnabled();
}

bool InputMethodContext::SetInputPanelPosition(uint32_t x, uint32_t y)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelPosition(x, y);
}

bool InputMethodContext::SetInputPanelPositionAlign(int32_t x, int32_t y, InputPanelAlign align)
{
  return Internal::Adaptor::InputMethodContext::GetImplementation(*this).SetInputPanelPositionAlign(x, y, align);
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
  (void)actor;
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
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).GetTextDirection();
}

void SetContentMimeTypes(Dali::InputMethodContext context, const Dali::String& mimeTypes)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).SetContentMimeTypes(mimeTypes);
}

void ApplyOptions(Dali::InputMethodContext context, const Dali::Integration::InputMethodOptions& options)
{
  Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).ApplyOptions(options);
}

bool SetInputPanelLanguage(Dali::InputMethodContext context, InputPanelLanguage language)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).SetInputPanelLanguage(language);
}

InputPanelLanguage GetInputPanelLanguage(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).GetInputPanelLanguage();
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

ContentReceivedSignalType& ContentReceivedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).ContentReceivedSignal();
}

LegacyStatusChangedSignalType& LegacyStatusChangedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).LegacyStatusChangedSignal();
}

LegacyKeyboardTypeChangedSignalType& LegacyKeyboardTypeChangedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).LegacyKeyboardTypeChangedSignal();
}

LegacyKeyboardResizedSignalType& LegacyResizedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).LegacyResizedSignal();
}

LegacyLanguageChangedSignalType& LegacyLanguageChangedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).LegacyLanguageChangedSignal();
}

LegacyContentReceivedSignalType& LegacyContentReceivedSignal(Dali::InputMethodContext context)
{
  return Dali::Internal::Adaptor::InputMethodContext::GetImplementation(context).LegacyContentReceivedSignal();
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
