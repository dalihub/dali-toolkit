#ifndef __DALI_TOOLKIT_WEBVIEWLITE_H__
#define __DALI_TOOLKIT_WEBVIEWLITE_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>

#include <dali/devel-api/adaptor-framework/web-engine-lite-plugin.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class WebViewLite;
}
}

/**
 * @brief WebViewLite class is used for Web.
 */
class WebViewLite: public BaseHandle
{
public:

  WebViewLite();

  ~WebViewLite();

  static WebViewLite New();

  WebViewLite( const WebViewLite& webViewLite );

  WebViewLite& operator=( const WebViewLite& webViewLite );

  static WebViewLite DownCast( BaseHandle handle );

  void CreateInstance(int width, int height, int windowX, int windowY, const std::string& locale, const std::string& timezoneID);

  void DestroyInstance();

  void LoadHtml(const std::string& path);

  Dali::WebEngineLitePlugin::WebEngineLiteSignalType& FinishedSignal();

private:

  WebViewLite( Internal::Adaptor::WebEngineLite* internal );

};

} // namespace Dali;

#endif
