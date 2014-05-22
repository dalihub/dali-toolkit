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

// CLASS HEADER
#include "localized-control-factory-impl.h"

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <libintl.h>

using std::string;
using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{


}

LocalizedControlFactory::LocalizedControlFactory()
: mObjectEntries(),
  mSignalsConnected( false )
{
}

LocalizedControlFactory::~LocalizedControlFactory()
{
}

Dali::Toolkit::TextView LocalizedControlFactory::CreateLocalizedTextView( const std::string& textID, const std::string& textDomain, const std::string& textViewTheme )
{
  if( !mSignalsConnected )
  {
    Stage::GetCurrent().GetObjectRegistry().ObjectDestroyedSignal().Connect( this, &LocalizedControlFactory::OnObjectDestruction );
    Adaptor::Get().LanguageChangedSignal().Connect( this, &LocalizedControlFactory::OnLanguageChanged );
    mSignalsConnected = true;
  }

  const string& localizedText = dgettext(textDomain.c_str(), textID.c_str());
  Dali::Toolkit::TextView textView = Dali::Toolkit::TextView::New();
  textView.SetText(localizedText);

  LocalisedStringInfo info(textID, textDomain, textViewTheme);

  mObjectEntries[textView.GetObjectPtr()] = info;

  return textView;
}

void LocalizedControlFactory::OnObjectDestruction( const Dali::RefObject* objectPointer )
{
  if(!mObjectEntries.empty())
  {
    //Needs optimization. All the destructed objects are currently checked for existence in entries.
    mObjectEntries.erase(objectPointer);
  }
}

void LocalizedControlFactory::OnLanguageChanged( Dali::Adaptor& adaptor)
{
  if(!mObjectEntries.empty())
  {
    ObjectEntriesIterator iter = mObjectEntries.begin();
    ObjectEntriesIterator iterEnd = mObjectEntries.end();
    while(iter != iterEnd)
    {
      RefObject* refObjectPtr = const_cast<RefObject*> (iter->first);
      BaseHandle handle(static_cast<BaseObject*>(refObjectPtr));
      LocalisedStringInfo info = iter->second;

      const string& localizedText = dgettext(info.textDomain.c_str(), info.textID.c_str());

      Toolkit::TextView textView =  Dali::Toolkit::TextView::DownCast( handle );

      if(textView)
      {
        textView.SetText( localizedText );
      }
      else
      {
        DALI_ASSERT_ALWAYS(false && "Corrupt TextView internal pointer in entries!");
      }

      ++iter;
    }
  }
}



} // namespace Internal

} // namespace Toolkit

} // namespace Dali
