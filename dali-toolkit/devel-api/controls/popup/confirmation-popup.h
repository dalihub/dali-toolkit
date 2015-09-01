#ifndef __DALI_TOOLKIT_CONFIRMATION_POPUP_H__
#define __DALI_TOOLKIT_CONFIRMATION_POPUP_H__

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

// INTERNAL INCLUDES
#include "popup.h"

namespace Dali
{

namespace Toolkit
{

// Forward declarations
namespace Internal DALI_INTERNAL
{
class ConfirmationPopup;
}

/**
 * @brief The ConfirmationPopup widget provides a simple interface to the Popup widget in which to
 * create common use-case popups.
 *
 * ConfirmationPopup will automatically provide signals for 1 or 2 buttons.
 * These signals are dynamically created. The controls (typically PushButtons) must be named as per the example below.
 *
 * Please see the programming guide for a detailed description of the ConfirmationPopup including examples.
 *
 * Signals (these are dynamically created upon connect).
 * | %Signal Name          | Actor name to connect to | Property to set signal type (eg clicked) |
 * |-----------------------|--------------------------|------------------------------------------|
 * | control-signal-ok     | control-ok               | connect-signal-ok-selected               |
 * | control-signal-cancel | control-cancel           | connect-signal-cancel-selected           |
 */
class DALI_IMPORT_API ConfirmationPopup : public Popup
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1 + DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the ConfirmationPopup class.
   */
  struct Property
  {
    enum
    {
      CONNECT_SIGNAL_OK_SELECTED = PROPERTY_START_INDEX, ///< name "connect-signal-ok-selected",     type std::string
      CONNECT_SIGNAL_CANCEL_SELECTED                     ///< name "connect-signal-cancel-selected", type std::string
    };
  };

  /**
   * @brief An enumeration to use as indices to reference buttons.
   */
  enum ControlIndex
  {
    CONTROL_OK = 0,    ///< Index of control 1
    CONTROL_CANCEL,    ///< Index of control 2
  };

  /**
   * @brief Create an uninitialized ConfirmationPopup; this can be initialized with ConfirmationPopup::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ConfirmationPopup();

  /**
   * @brief Copy constructor.
   */
  ConfirmationPopup( const ConfirmationPopup& confirmationPopup );

  /**
   * @brief Assignment operator.
   */
  ConfirmationPopup& operator=( const ConfirmationPopup& confirmationPopup );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived types must not contain data or virtual methods.
   */
  ~ConfirmationPopup();

  /**
   * @brief Create an initialized ConfirmationPopup.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ConfirmationPopup New();

  /**
   * @brief Downcast an Object handle to ConfirmationPopup.
   *
   * If handle points to a ConfirmationPopup the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a ConfirmationPopup or an uninitialized handle
   */
  static ConfirmationPopup DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL ConfirmationPopup( Internal::ConfirmationPopup& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL ConfirmationPopup( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CONFIRMATION_POPUP_H__
