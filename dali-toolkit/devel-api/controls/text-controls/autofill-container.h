#ifndef DALI_TOOLKIT_AUTO_FILL_CONTAINER_H
#define DALI_TOOLKIT_AUTO_FILL_CONTAINER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/autofill-item.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class AutofillContainer;
}

/**
 * @brief AutofillContainer controls several text input boxes (Dali::Toolkit::TextField and Dali::Toolkit::TextEditor).
 *
 * It can make these editors a group of text boxes.
 */
class DALI_TOOLKIT_API AutofillContainer : public BaseHandle
{
public:

  // TODO : Need to update parameter and return value according to each Signal
  typedef Signal< void (AutofillContainer&) > AuthenticationSignalType;  ///< Authentication Signal Type
  typedef Signal< void (Control&) > ListShownSignalType;                 ///<  List Shown Signal Type

public:

  /**
   * @brief Creates the AutofillContainer.
   *
   * @param[in] name The AutofillContainer name
   * @return A handle to the AutofillContainer
   */
  static AutofillContainer New( const std::string& name );

  /**
   * @brief Creates an empty handle.
   */
  AutofillContainer();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from
   */
  AutofillContainer( const AutofillContainer& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from
   * @return A reference to this
   */
  AutofillContainer& operator=( const AutofillContainer& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AutofillContainer();

  /**
   * @brief Downcasts a handle to AutofillContainer.
   *
   * If the BaseHandle points is a AutofillContainer, the downcast returns a valid handle.
   * If not, the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a AutofillContainer or an empty handle
   */
  static AutofillContainer DownCast( BaseHandle handle );

  /**
   * @brief Adds Control and AutofillItem information to Autofill Container.
   *
   * @param[in] control The control to be added to Autofill Container
   * @param[in] propertyIndex The Property to be filled automatically of each Control
   * @param[in] id A unique ID that does not always change on each launching
   * @param[in] label An auxiliary means to guess heuristically what data is
   * @param[in] hint The Hint - id (username), name, password, phone, credit card number, organization, and so on
   * @param[in] isSensitive Whether this information is a sensitive data or not
   */
  void AddAutofillItem( Control control, Property::Index propertyIndex, const std::string& id, const std::string& label, Dali::AutofillItem::Hint hint, bool isSensitive );

  /**
   * @brief Removes Control and its AutofillItem information to Autofill Container.
   *
   * @param[in] control The control to be removed
   */
  void RemoveAutofillItem( Control control );

  /**
   * @brief Sets that a control is focused.
   *
   * @param focused The focused control
   */
  void SetFocusedControl( Toolkit::Control focused );

  /**
   * @brief Gets the focused control.
   *
   * @return The focused control
   */
  Toolkit::Control GetFocusedControl();

  /**
   * @brief Stores autofill data.
   */
  void SaveAutofillData();

  /**
   * @brief Sends a request for filling the data.
   */
  void RequestFillData();

  /**
   * @brief Gets the Autofill Service Name.
   *
   * @return Autofill Service Name
   */
  const std::string& GetAutofillServiceName() const;

  /**
   * @brief Gets the Autofill Service Message.
   *
   * @return Autofill Service Message
   */
  const std::string& GetAutofillServiceMessage() const;

  /**
   * @brief Gets the Autofill Service Image Path.
   *
   * @return Autofill Service Image Path
   */
  const std::string& GetAutofillServiceImagePath() const;

  /**
   * @brief Gets the number of list items. (The presentation text of Autofill)
   *
   * @return The number of list items
   */
  unsigned int GetListCount();

  /**
   * @brief Gets the list item of the index.
   *
   * @param[in] index The index for the list
   * @return The list item of the index
   */
  const std::string& GetListItem( unsigned int index ) const;

  /**
   * @brief Sets the selected item to fill out.
   *
   * @param[in] selected The selected item
   */
  void SetSelectedItem( const std::string& selected );

public:
  // Signals

  /**
   * @brief AutofillServiceShownSignal
   *
   * @return The signal containing the received data
   */
  AuthenticationSignalType& AutofillServiceShownSignal();

  /**
   * @brief AutofillListShownSignal
   *
   * @return The signal containing the received data
   */
  ListShownSignalType& AutofillListShownSignal();


public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL AutofillContainer( Internal::AutofillContainer* implementation );

};


} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_AUTO_FILL_CONTAINER_H
