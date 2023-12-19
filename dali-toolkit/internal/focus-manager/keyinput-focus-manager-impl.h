#ifndef DALI_TOOLKIT_INTERNAL_KEYINPUT_FOCUS_MANAGER_H
#define DALI_TOOLKIT_INTERNAL_KEYINPUT_FOCUS_MANAGER_H

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/object-registry.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Integration
{
class SceneHolder;

} // namespace Integration

namespace Toolkit
{
namespace Internal
{
class KeyInputFocusManager;

/**
 * @copydoc Toolkit::KeyInputFocusManager
 */
class KeyInputFocusManager : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  /**
   * Construct a new KeyInputFocusManager.
   */
  KeyInputFocusManager();

  /**
   * @copydoc Toolkit::SetFocus
   */
  void SetFocus(Toolkit::Control control);

  /**
   * @copydoc Toolkit::RemoveFocus
   */
  void RemoveFocus(Toolkit::Control control);

  /**
   * @copydoc Toolkit::GetCurrentFocusControl
   */
  Toolkit::Control GetCurrentFocusControl() const;

public:
  /**
   * @copydoc Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignal()
   */
  Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusChangedSignal();

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

protected:
  /**
   * Destructor
   */
  virtual ~KeyInputFocusManager();

private:
  /**
   * This will be called when a new scene holder is created
   * @param sceneHolder The new scene holder
   */
  void OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder);

  /**
   * Callback for the key event when no actor in the stage has gained the key input focus
   * @param[in] event The KeyEvent event.
   */
  bool OnKeyEvent(const KeyEvent& event);

  /**
   * Signal handler called when a focused Control is removed from Scene.
   * @param[in]  control  The control removed from the scene.
   */
  void OnFocusControlSceneDisconnection(Dali::Actor control);

  /**
    *  Recursively deliver events to the control and its parents, until the event is consumed or the stage is reached.
   * @param[in]  control  The control got KeyEvent.
   * @param[in]  event    The KeyEvent.
   * @return True if KeyEvent is consumed.
    */
  bool EmitKeyEventSignal(Toolkit::Control control, const KeyEvent& event);

  /**
   * Gets the current native window id
   */
  uint32_t GetCurrentWindowId() const;

private:
  // Undefined
  KeyInputFocusManager(const KeyInputFocusManager&);

  KeyInputFocusManager& operator=(const KeyInputFocusManager& rhs);

private:
  // The key input focus change signal
  Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalType mKeyInputFocusChangedSignal;

  SlotDelegate<KeyInputFocusManager> mSlotDelegate;

  Toolkit::Control mCurrentFocusControl; ///< The current focused control
  uint32_t mCurrentWindowId; ///< The native window id of current focused control
};

} // namespace Internal

inline Internal::KeyInputFocusManager& GetImpl(Dali::Toolkit::KeyInputFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::KeyInputFocusManager&>(handle);
}

inline const Internal::KeyInputFocusManager& GetImpl(const Dali::Toolkit::KeyInputFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::KeyInputFocusManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_KEYINPUT_FOCUS_MANAGER_H
