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
#include <dali-toolkit/devel-api/controls/web-view-lite/web-view-lite.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/web-view-lite/web-view-lite-impl.h>

namespace Dali
{

namespace Toolkit
{

WebViewLite::WebViewLite()
{
}

WebViewLite::WebViewLite( const WebViewLite& webViewLite )
: Control( webViewLite )
{
}

WebViewLite& WebViewLite::operator=( const WebViewLite& webViewLite )
{
  if( &webViewLite != this )
  {
    Control::operator=( webViewLite );
  }

  return *this;
}

WebViewLite::~WebViewLite()
{
}

WebViewLite WebViewLite::New()
{
  return Internal::WebViewLite::New();
}

WebViewLite WebViewLite::DownCast( BaseHandle handle )
{
  return Control::DownCast< WebViewLite, Internal::WebViewLite >( handle );
}

void WebViewLite::CreateInstance(int width, int height, int windowX, int windowY, const std::string& locale, const std::string& timezoneID)
{
  Dali::Toolkit::GetImpl( *this ).CreateInstance(width, height, windowX, windowY, locale, timezoneID);
}

void WebViewLite::DestroyInstance()
{
  Dali::Toolkit::GetImpl( *this ).DestroyInstance();
}

void WebViewLite::LoadHtml(const std::string& path)
{
  Dali::Toolkit::GetImpl( *this ).LoadHtml(path);
}

WebViewLite::WebViewLiteSignalType& WebViewLite::FinishedSignal()
{
  return Dali::Toolkit::GetImpl( *this ).FinishedSignal();
}

WebViewLite::WebViewLite( Internal::WebViewLite& implementation )
: Control( implementation )
{
}

WebViewLite::WebViewLite( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer< Internal::WebViewLite >( internal );
}

} // namespace Toolkit

} // namespace Dali
