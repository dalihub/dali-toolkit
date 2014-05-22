#ifndef __DALI_TOOLKIT_LOCALIZED_CONTROL_FACTORY_H__
#define __DALI_TOOLKIT_LOCALIZED_CONTROL_FACTORY_H__

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

/**
 * @addtogroup CAPI_DALI_TOOLKIT_FACTORY_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class LocalizedControlFactory;
}

/**
 * @brief This class provides functionality for creating controls which have localized text.
 *
 * This class keeps track of objects created using its factory methods, and updates them
 * when the system language changes.
 *
 * Warning: If the developer calls SetText on the object to overwrite the managed TextView,
 * then it leads to an inconsistent state where the object will be overwritten with the
 * localized text when language/locale changes.
 */

class LocalizedControlFactory : public BaseHandle
{
public:

  /**
   * @brief Creates a localized TextView, which is automatically updated when the locale or language changes.
   *
   * @pre The LocalizedControlFactory has been initialized.
   *
   * @param textID The id of the localized text with which a platform request (gettext) for localized text can be made.
   * @param textDomain The text domain for the localized text. Eg "sys_string"
   * @param textViewTheme A string containing style info about various properties of TextView for different
   *        locale/language.
   * @return handle to a new localized TextView
   */
  static Dali::Toolkit::TextView CreateLocalizedTextView( const std::string& textID, const std::string& textDomain = "sys_string", const std::string& textViewTheme = "{}" );


private:

  /**
   * @brief Create a LocalizedControlFactory handle; this can be initialised with LocalizedControlFactory::New().
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  LocalizedControlFactory();

  /**
   * @brief Virtual destructor.
   */
  virtual ~LocalizedControlFactory();

  /**
   * @brief Get the singleton of LocalizedControlFactory object.
   *
   * @return A handle to the LocalizedControlFactory control.
   */
  static LocalizedControlFactory Get();

  /**
   * @brief Allows the creation of this Control from an Internal::LocalizedControlFactory pointer.
   *
   * @param[in]  impl  A pointer to the internal LocalizedControlFactory.
   */
  LocalizedControlFactory(Internal::LocalizedControlFactory *impl);
}; // class LocalizedControlFactory

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_LOCALIZED_CONTROL_FACTORY_H__
