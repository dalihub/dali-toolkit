#ifndef __DALI_TOOLKIT_INTERNAL_KEYINPUT_FOCUS_MANAGER_H__
#define __DALI_TOOLKIT_INTERNAL_KEYINPUT_FOCUS_MANAGER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <string>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/object-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

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
  typedef Dali::Vector< Dali::BaseObject* > FocusStack;
  typedef FocusStack::Iterator FocusStackIterator;

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

  /**
   * @copydoc Toolkit::IsKeyboardListener
   */
  bool IsKeyboardListener(Toolkit::Control control) const;

public:

  /**
   * @copydoc Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignal()
   */
  Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalV2& KeyInputFocusChangedSignal();

  /**
   * @copydoc Toolkit::KeyInputFocusManager::UnhandledKeyEventSignal()
   */
  Toolkit::KeyInputFocusManager::UnhandledKeyEventSignalV2& UnhandledKeyEventSignal();

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

protected:

  /**
   * Destructor
   */
  virtual ~KeyInputFocusManager();

private:

  /**
   * Search for a control in the focus stack.
   * @param[in] control The control for which to search
   * @return An iterator to the control. If not found, this will equate to the
   * mFocusStack.End() iterator.
   */
  FocusStackIterator FindFocusControlInStack( Toolkit::Control control ) const;

  /**
   * Callback for the key event when no actor in the stage has gained the key input focus
   * @param[in] event The KeyEvent event.
   */
  void OnKeyEvent(const KeyEvent& event);

  /**
   * Signal handler called when a focused Control is removed from Stage.
   * @param[in]  control  The control removed from stage.
   */
  void OnFocusControlStageDisconnection( Dali::Actor control );

  /**
   * Signal handler called when an actor is destroyed.
   */
  void OnObjectDestroyed(const Dali::RefObject* object);

private:

  // Undefined
  KeyInputFocusManager(const KeyInputFocusManager&);

  KeyInputFocusManager& operator=(const KeyInputFocusManager& rhs);

private:

  // The key input focus change signal
  Toolkit::KeyInputFocusManager::KeyInputFocusChangedSignalV2 mKeyInputFocusChangedSignalV2;

  // The un-handled key event signal
  Toolkit::KeyInputFocusManager::UnhandledKeyEventSignalV2 mUnhandledKeyEventSignalV2;

  // Keyboard events are sent to the current focus actor, which will be the actor on the top of the focus actors stack.
  FocusStack mFocusStack;
  SlotDelegate< KeyInputFocusManager > mSlotDelegate;
  ObjectRegistry mObjectRegistry;
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

#endif // __DALI_TOOLKIT_INTERNAL_KEYINPUT_FOCUS_MANAGER_H__
