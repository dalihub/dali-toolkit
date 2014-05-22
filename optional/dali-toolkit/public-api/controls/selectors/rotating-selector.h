#ifndef __DALI_TOOLKIT_CUSTOM_CHECK_ACTOR_H__
#define __DALI_TOOLKIT_CUSTOM_CHECK_ACTOR_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class RotatingSelector;
}

/**
 * RotatingSelector is a simple control to switch between two states (selected/unselected). A signal is emitted when the selector switches between
 * the two states. The control has two faces one behind the other, The control is rotated while switching between the two states
 */
class RotatingSelector : public Control
{
public:
  //Signal Names
  static const char* const SIGNAL_CHECKED;

public:

  /**
   * Create an uninitialized RotatingSelector; this can be initialized with RotatingSelector::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  RotatingSelector();

  /**
   * Copy constructor.
   */
  RotatingSelector( const RotatingSelector& rotatingSelector );

  /**
   * Assignment operator.
   */
  RotatingSelector& operator=( const RotatingSelector& rotatingSelector );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  ~RotatingSelector();

  /**
   * Create an initialized RotatingSelector.
   * @return A handle to a newly allocated Dali resource.
   */
  static RotatingSelector New(Actor& unSelectedActor, Actor& selectedActor);

  /**
   * Downcast an Object handle to RotatingSelector. If handle points to a RotatingSelector the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a RotatingSelector or an uninitialized handle
   */
  static RotatingSelector DownCast( BaseHandle handle );

  /**
   * Toggles the selection status of the selector.
   * @param[in] toggle true for selected and false for un selected.
   */
  void SetSelected( bool toggle );

  /**
   * Queries the selection status of the selector.
   * @return true if the selector is selected otherwise false
   */
  bool IsSelected() const;

  /**
   * Sets the actor to be displayed by the selector when it is in selected state
   * @param[in] selectedActor The actor to display
   */
  void SetSelectedActor( Actor& selectedActor );

  /**
   * Gets the actor to be displayed by the selector when it is in selected state
   * @return  A handle to the selected actor. If the selected actor has not been set, this handle will be invalid.
   */
  Actor GetSelectedActor();

  /**
   * Sets the actor to be displayed by the selector when it is in unselected state
   * @param[in] unSelectedActor The actor to display
   */
  void SetUnSelectedActor( Actor& unSelectedActor );

  /**
   * Gets the actor to be displayed by the selector when it is in unselected state
   * @return  A handle to Actor. If the unselected actor has not been set, this handle will be invalid.
   */
  Actor GetUnSelectedActor();

  /**
   * Sets whether the Selector is selectable
   * @param[in] selectable true to be able to toggle the selector false otherwise
   */
  void SetSelectable( bool selectable );

  /**
   * Queries whether the Selector is selectable
   * @return true if the selector is selectable, false otherwise
   */
  bool IsSelectable()const;

public: //Signals

  // RotatingSelector Toggled
  typedef SignalV2< void( RotatingSelector, bool ) > SelectedSignalV2;

  /**
   * Signal emitted when the rotating selector is in switched to a selected state.
   */
  SelectedSignalV2& SelectedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  RotatingSelector( Internal::RotatingSelector& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  RotatingSelector( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CUSTOM_CHECK_ACTOR_H__
