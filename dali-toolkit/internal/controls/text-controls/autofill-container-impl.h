#ifndef DALI_TOOLKIT_INTERNAL_AUTOFILL_CONTAINER_H
#define DALI_TOOLKIT_INTERNAL_AUTOFILL_CONTAINER_H

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
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/adaptor-framework/autofill-manager.h>
#include <map>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/text-controls/autofill-container.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @copydoc Toolkit::AutofillContainer
 */
class AutofillContainer : public Dali::BaseObject
{
public:

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::New()
   */
  static Toolkit::AutofillContainer New( const std::string& name );

  /**
   * @brief Initialize AutofillContainer
   */
  void Initialize( const std::string& name );

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::AddAutofillItem()
   */
  void AddAutofillItem( Dali::Toolkit::Control control, Dali::Property::Index propertyIndex, const std::string& id, const std::string& label, Dali::AutofillItem::Hint hint, bool isSensitive );

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::RemoveAutofillItem()
   */
  void RemoveAutofillItem( Dali::Toolkit::Control control );

  /**
   * @brief Gets AutofillGroup setting to AutofillContainer.
   *
   * @return The AutofillGroup instance
   */
  Dali::AutofillGroup GetAutofillGroup();

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::SetFocusedControl()
   */
  void SetFocusedControl( Toolkit::Control focused );

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::GetFocusedControl()
   */
  Toolkit::Control GetFocusedControl();

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::SaveAutofillData()
   */
  void SaveAutofillData();

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::RequestFillData()
   */
  void RequestFillData();

  /**
   * @brief Sets the Autofill Service Name
   *
   * @param serviceName Autofill Service Name
   */
  void SetAutofillServiceName( const std::string& serviceName );

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::GetAutofillServiceName()
   */
  const std::string& GetAutofillServiceName() const;

  /**
   * @brief Sets the Autofill Service Message
   *
   * @param serviceMessage Autofill Service Message
   */
  void SetAutofillServiceMessage( const std::string& serviceMessage );

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::GetAutofillServiceMessage()
   */
  const std::string& GetAutofillServiceMessage() const;

  /**
   * @brief Sets the Autofill Service Image Path
   *
   * @param serviceImagePath Autofill Service Image Path
   */
  void SetAutofillServiceImagePath( const std::string& serviceImagePath );

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::GetAutofillServiceImagePath()
   */
  const std::string& GetAutofillServiceImagePath() const;

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::GetListCount()
   */
  unsigned int GetListCount();

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::GetListItem()
   */
  const std::string& GetListItem( unsigned int index ) const;

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::SetSelectedItem()
   */
  void SetSelectedItem( const std::string& selected );

  /**
   * @brief Callback when Autofill Authentication information is recieved.
   */
  void OnAutofillAuthReceived();

  /**
   * @brief Callback when Autofill Fill Response is recieved.
   * @param[in] item The callback parameter
   */
  void OnDataFillReceived( Dali::AutofillItem item );

  /**
   * @brief Callback when the values to be filled are multiple.
   */
  void OnListReceived();

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::AutofillServiceShownSignal()
   */
  Toolkit::AutofillContainer::AuthenticationSignalType& AutofillServiceShownSignal();

  /**
   * @copydoc Dali::Toollkit::AutofillContainer::AutofillListShownSignal()
   */
  Toolkit::AutofillContainer::ListShownSignalType& AutofillListShownSignal();

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


private: // Implementation

  /**
   * Construct a new AutofillContainer.
   */
  AutofillContainer();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~AutofillContainer();

private:

  // Undefined copy constructor and assignment operators
  AutofillContainer(const AutofillContainer&);
  AutofillContainer& operator=(const AutofillContainer& rhs);

private: // Data

  Dali::AutofillManager mAutofillManager;
  Dali::AutofillGroup mAutofillGroup;
  std::vector<Dali::Toolkit::Control> mControlItemList; ///< The List of Control adding to AutofillContainer
  //std::map<Dali::Toolkit::Control, Dali::AutofillItem> mControlList;
  std::vector< std::pair< Dali::Toolkit::Control, Dali::AutofillItem > > mControlList;

  SlotDelegate< AutofillContainer > mSlotDelegate;

  Property::Index mPropertyIndex;  ///< The index of property registered by each control

  Toolkit::AutofillContainer::AuthenticationSignalType mAuthenticationEventSignal;
  Toolkit::AutofillContainer::ListShownSignalType mListEventSignal;

  std::string mAutofillServiceName;
  std::string mAutofillServiceMessage;
  std::string mAutofillServiceImagePath;

  Toolkit::Control mCurrentFocused;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::AutofillContainer& GetImpl( Toolkit::AutofillContainer& autofillContainer )
{
  DALI_ASSERT_ALWAYS(autofillContainer);

  Dali::BaseObject& handle = autofillContainer.GetBaseObject();

  return static_cast<Toolkit::Internal::AutofillContainer&>(handle);
}

inline const Toolkit::Internal::AutofillContainer& GetImpl( const Toolkit::AutofillContainer& autofillContainer )
{
  DALI_ASSERT_ALWAYS(autofillContainer);

  const Dali::BaseObject& handle = autofillContainer.GetBaseObject();

  return static_cast<const Toolkit::Internal::AutofillContainer&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_AUTOFILL_CONTAINER_H
