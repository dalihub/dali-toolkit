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
#include "web-view-lite-impl.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/adaptors/adaptor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/web-view-lite/web-view-lite.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::WebViewLite::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::WebViewLite, Toolkit::Control, Create );

DALI_SIGNAL_REGISTRATION( Toolkit, WebViewLite, "finished", FINISHED_SIGNAL )

DALI_TYPE_REGISTRATION_END()

} // anonymous namepsace

WebViewLite::WebViewLite()
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_DEFAULT | DISABLE_STYLE_CHANGE_SIGNALS ) )
{
  mWebViewLite = Dali::WebEngineLite::New();
}

WebViewLite::~WebViewLite()
{
}

Toolkit::WebViewLite WebViewLite::New()
{
  WebViewLite* impl = new WebViewLite();
  Toolkit::WebViewLite handle = Toolkit::WebViewLite( *impl );

  impl->Initialize();

  return handle;
}

void WebViewLite::OnInitialize()
{
  mWebViewLite.FinishedSignal().Connect( this, &WebViewLite::EmitFinishedSignal );
}

void WebViewLite::CreateInstance(int width, int height, int windowX, int windowY, const std::string& locale, const std::string& timezoneID)
{
  mWebViewLite.CreateInstance(width, height, windowX, windowY, locale, timezoneID);
}

void WebViewLite::DestroyInstance()
{
  mWebViewLite.DestroyInstance();
}

void WebViewLite::LoadHtml(const std::string& path)
{
  mWebViewLite.LoadHtml(path);
}

Dali::Toolkit::WebViewLite::WebViewLiteSignalType& WebViewLite::FinishedSignal()
{
  return mFinishedSignal;
}

void WebViewLite::EmitFinishedSignal()
{
  if ( !mFinishedSignal.Empty() )
  {
    Dali::Toolkit::WebViewLite handle( GetOwner() );
    mFinishedSignal.Emit( handle );
  }
}

bool WebViewLite::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::WebViewLite webViewLite = Toolkit::WebViewLite::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), FINISHED_SIGNAL ) )
  {
    webViewLite.FinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
