#ifndef DALI_TOOLKIT_TOOLKIT_INPUT_METHOD_CONTEXT_H
#define DALI_TOOLKIT_TOOLKIT_INPUT_METHOD_CONTEXT_H

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

// This test double replaces the adaptor public InputMethodContext header.
#define DALI_INPUT_METHOD_CONTEXT_H

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/adaptor-framework/input-method.h>
#include <dali/public-api/dali-adaptor-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class InputMethodContext;
}
} //namespace Internal DALI_INTERNAL

/**
 * @brief Test double for the public InputMethodContext handle.
 */
class InputMethodContext : public BaseHandle
{
public:
  enum State
  {
    DEFAULT = 0,
    SHOW,
    HIDE,
    WILL_SHOW
  };

  enum KeyboardType
  {
    SOFTWARE_KEYBOARD,
    HARDWARE_KEYBOARD
  };

  enum class InputPanelAlign
  {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    MIDDLE_LEFT,
    MIDDLE_CENTER,
    MIDDLE_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
  };

  using ActivatedSignalType              = Signal<void(InputMethodContext)>;
  using StatusChangedSignalType          = Signal<void(InputMethodContext, State)>;
  using KeyboardTypeChangedSignalType    = Signal<void(InputMethodContext, KeyboardType)>;
  using KeyboardResizedSignalType        = Signal<void(InputMethodContext)>;
  using LanguageChangedSignalType        = Signal<void(InputMethodContext)>;
  using PrivateCommandReceivedSignalType = Signal<void(InputMethodContext, const Dali::String&)>;

public:
  InputMethodContext();
  ~InputMethodContext();

  static InputMethodContext DownCast(BaseHandle handle);

  bool IsRestoreAfterFocusLostEnabled() const;
  bool SetRestoreAfterFocusLostEnabled(bool enabled);

  Dali::BoundsInteger GetInputPanelArea();
  bool                SetInputPanelUserData(const Dali::String& data);
  Dali::String        GetInputPanelUserData() const;
  State               GetInputPanelState() const;
  bool                SetReturnKeyEnabled(bool enabled);
  bool                IsReturnKeyEnabled() const;
  bool                SetInputPanelAutoShowEnabled(bool enabled);
  bool                ShowInputPanel();
  bool                HideInputPanel();
  KeyboardType        GetKeyboardType() const;
  bool                SetInputPanelLanguageLocale(const Dali::String& locale);
  Dali::String        GetInputPanelLanguageLocale() const;
  bool                SetTextPredictionEnabled(bool enabled);
  bool                IsTextPredictionEnabled() const;
  bool                SetFullScreenModeEnabled(bool enabled);
  bool                IsFullScreenModeEnabled() const;
  bool                SetInputPanelPosition(uint32_t x, uint32_t y);
  bool                SetInputPanelPositionAlign(int32_t x, int32_t y, InputPanelAlign align);

  bool                                      SetInputPanelLayout(Dali::InputMethod::PanelLayout layout);
  Dali::InputMethod::PanelLayout      GetInputPanelLayout() const;
  bool                                      SetInputPanelReturnKeyType(Dali::InputMethod::ReturnKeyType action);
  Dali::InputMethod::ReturnKeyType     GetInputPanelReturnKeyType() const;
  bool                                      SetInputPanelAutoCapitalType(Dali::InputMethod::AutoCapitalType autoCapital);
  Dali::InputMethod::AutoCapitalType      GetInputPanelAutoCapitalType() const;
  bool                                      SetInputPanelLayoutVariation(Dali::InputMethod::PanelLayoutVariation variation);
  Dali::InputMethod::PanelLayoutVariation GetInputPanelLayoutVariation() const;

  ActivatedSignalType&              ActivatedSignal();
  StatusChangedSignalType&          StatusChangedSignal();
  KeyboardResizedSignalType&        ResizedSignal();
  LanguageChangedSignalType&        LanguageChangedSignal();
  KeyboardTypeChangedSignalType&    KeyboardTypeChangedSignal();
  PrivateCommandReceivedSignalType& PrivateCommandReceivedSignal();

  explicit InputMethodContext(Internal::Adaptor::InputMethodContext* inputMethodContext);
};

} // namespace Dali DALI_IMPORT_API

#include <dali/integration-api/adaptor-framework/input-method-context-integ.h>

namespace Dali
{
namespace Integration
{
namespace InputMethodContext
{
namespace Test
{
void SetPreeditStyle(Dali::InputMethodContext context, PreeditStyle type);
} // namespace Test
} // namespace InputMethodContext
} // namespace Integration
} // namespace Dali

#endif // DALI_TOOLKIT_TOOLKIT_INPUT_METHOD_CONTEXT_H
