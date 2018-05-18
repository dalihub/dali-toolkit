/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include "toolkit-web-view-lite.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class WebViewLite: public Dali::BaseObject
{
public:

  WebViewLite()
  {
  }

public:
  Dali::WebEngineLitePlugin::WebEngineLiteSignalType mFinishedSignal;

private:
};

inline WebViewLite& GetImplementation( Dali::WebEngineLite& webViewLite )
{
  DALI_ASSERT_ALWAYS(webViewLite && "WebViewLite handle is empty");
  BaseObject& handle = webViewLite.GetBaseObject();
  return static_cast< Internal::Adaptor::WebEngineLite& >( handle );
}

inline const WebViewLite& GetImplementation( const Dali::WebEngineLite& webViewLite )
{
  DALI_ASSERT_ALWAYS(webViewLite && "WebViewLite handle is empty");
  const BaseObject& handle = webViewLite.GetBaseObject();
  return static_cast< const Internal::Adaptor::WebEngineLite& >( handle );
}

} // namespace Adaptor

} // namespace Internal


/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

WebViewLite::WebViewLite()
{
}

WebViewLite::WebViewLite( Internal::Adaptor::WebEngineLite* internal )
: BaseHandle( internal )
{
}

WebViewLite::~WebViewLite()
{
}

WebViewLite WebViewLite::New()
{
  Internal::Adaptor::WebEngineLite* webViewLite = new Internal::Adaptor::WebEngineLite();

  return WebViewLite( webViewLite );
}

WebViewLite::WebViewLite( const WebViewLite& webViewLite )
: BaseHandle( webViewLite )
{
}

WebViewLite& WebViewLite::operator=( const WebViewLite& webViewLite )
{
  BaseHandle::operator=( webViewLite );
  return *this;
}

WebViewLite WebViewLite::DownCast( BaseHandle handle )
{
  WebViewLite webViewLite;
  return webViewLite;
}

void WebViewLite::CreateInstance(int width, int height, int windowX, int windowY, const std::sting& locale, const std::string& timezoneID)
{
  Internal::Adaptor::GetImplementation( *this ).CreateInstance(width, height, windowX, windowY, locale, timezoneID);
}

void WebViewLite::DestroyInstance()
{
  Internal::Adaptor::GetImplementation( *this ).DestroyInstance();
}

void WebViewLite::LoadHtml(const std::string& path)
{
  return Internal::Adaptor::GetImplementation( *this ).LoadHtml(path);
}

Dali::WebEngineLitePlugin::WebEngineLiteSignalType& WebViewLite::FinishedSignal()
{
  return Internal::Adaptor::GetImplementation( *this ).mFinishedSignal;
}

} // namespace Dali;

