#ifndef __DALI_TOOLKIT_INTERNAL_CONFIRMATION_POPUP_H__
#define __DALI_TOOLKIT_INTERNAL_CONFIRMATION_POPUP_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/devel-api/signals/signal-delegate.h>

// INTERNAL INCLUDES
#include "popup-impl.h"
#include <dali-toolkit/devel-api/controls/popup/confirmation-popup.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

#define MAXIMUM_NUMBER_OF_CONTROLS 2

/**
 * ConfirmationPopup implementation class.
 *
 * \sa Dali::Toolkit::ConfirmationPopup
 */
class ConfirmationPopup : public Dali::Toolkit::Internal::Popup
{
public:

  /**
   * Create a new ConfirmationPopup.
   * @return A smart-pointer to the newly allocated ConfirmationPopup.
   */
  static Dali::Toolkit::ConfirmationPopup New();

protected:

  /**
   * Construct a new ConfirmationPopup.
   */
  ConfirmationPopup();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ConfirmationPopup();

public:

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] propertyIndex The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] propertyIndex The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

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

private:

  /**
   * This type houses a list of dynamically created signals.
   */
  typedef std::vector< std::pair< std::string, SignalDelegate* > > SignalContainerType;

private:

  /**
   * Sets the name of the signal to connect to within the specified actor.
   *
   * @param[in] controlNumber The index of the control.
   * @param[in] signalName The name of the signal to connect to.
   */
  void SetControlSignalName( const unsigned int controlNumber, const std::string& signalName );

  /**
   * Gets the name of the signal to connect to within the specified actor.
   *
   * @param[in] controlNumber The index of the control.
   * @return The name of the signal to connect to.
   */
  std::string GetControlSignalName( unsigned int controlNumber ) const;

  /**
   * @copydoc Control::GetControlSignal()
   */
  SignalDelegate* GetControlSignal( const std::string& signalName );

private:

  // Undefined
  ConfirmationPopup( const ConfirmationPopup& );

  // Undefined
  ConfirmationPopup& operator=( const ConfirmationPopup& );

private:

  // Properties:

  std::string mControlSignalNames[ MAXIMUM_NUMBER_OF_CONTROLS ]; ///< Stores the names of the signals to connect to per control.

  // Internal variables:

  SignalContainerType mControlSignals;                           ///< Stores the dynamically created signals.

};

} // namespace Internal


// Helpers for public-api forwarding methods

inline Toolkit::Internal::ConfirmationPopup& GetDerivedImplementation( Toolkit::ConfirmationPopup& popup )
{
  DALI_ASSERT_ALWAYS( popup );

  Dali::RefObject& handle = popup.GetImplementation();

  return static_cast<Toolkit::Internal::ConfirmationPopup&>( handle );
}

inline const Toolkit::Internal::ConfirmationPopup& GetDerivedImplementation( const Toolkit::ConfirmationPopup& popup )
{
  DALI_ASSERT_ALWAYS( popup );

  const Dali::RefObject& handle = popup.GetImplementation();

  return static_cast<const Toolkit::Internal::ConfirmationPopup&>( handle );
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_CONFIRMATION_POPUP_H__
