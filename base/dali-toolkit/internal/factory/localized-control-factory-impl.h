#ifndef __DALI_TOOLKIT_INTERNAL_LOCALIZED_CONTROL_FACTORY_H__
#define __DALI_TOOLKIT_INTERNAL_LOCALIZED_CONTROL_FACTORY_H__

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
#include <set>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/factory/localized-control-factory.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @copydoc Toolkit::LocalizedControlFactory
 */
class LocalizedControlFactory : public Dali::BaseObject, public ConnectionTracker
{
public:

  /**
   * Structure used to store/retrieve localisation info.
   */
  struct LocalisedStringInfo
  {
    LocalisedStringInfo()
    {
    }

    LocalisedStringInfo(std::string id, std::string domain, std::string theme)
    : textID(id),
      textDomain(domain),
      textViewTheme(theme)
    {

    }

    std::string textID;
    std::string textDomain;
    std::string textViewTheme;
  };

  typedef std::map< const Dali::RefObject*, LocalisedStringInfo > ObjectEntriesContainer;
  typedef ObjectEntriesContainer::iterator ObjectEntriesIterator;
  typedef ObjectEntriesContainer::const_iterator ObjectEntriesConstIterator;

  /**
   * Construct a new LocalizedControlFactory.
   */
  LocalizedControlFactory();

  /**
   * @copydoc Toolkit::LocalizedControlFactory::CreateLocalizedTextView
   */
  Dali::Toolkit::TextView CreateLocalizedTextView( const std::string& textID, const std::string& textDomain, const std::string& textViewTheme );

protected:

  /**
   * Destructor
   */
  virtual ~LocalizedControlFactory();

private:

  /**
   * Callback for Object destructed signal.
   * @param objectPointer Pointer to a RefObject
   */
  void OnObjectDestruction( const Dali::RefObject* objectPointer );

  /**
   * Callback for language changed signal.
   * @param adaptor Reference to a Dali::Adaptor instance
   */
  void OnLanguageChanged( Dali::Adaptor& adaptor);

private:

  // Undefined
  LocalizedControlFactory(const LocalizedControlFactory&);
  LocalizedControlFactory& operator=(const LocalizedControlFactory& rhs);

private:

  ObjectEntriesContainer mObjectEntries;
  bool mSignalsConnected:1;

};

} // namespace Internal

inline Internal::LocalizedControlFactory& GetImpl(Dali::Toolkit::LocalizedControlFactory& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::LocalizedControlFactory&>(handle);
}

inline const Internal::LocalizedControlFactory& GetImpl(const Dali::Toolkit::LocalizedControlFactory& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::LocalizedControlFactory&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_LOCALIZED_CONTROL_FACTORY_H__
