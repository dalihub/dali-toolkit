/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/web-view/web-cookie-manager.h>

namespace Dali
{
namespace Toolkit
{
WebCookieManager::WebCookieManager(Dali::WebEngineCookieManager& manager)
: mWebEngineCookieManager(manager)
{
}

WebCookieManager::~WebCookieManager()
{
}

void WebCookieManager::SetCookieAcceptPolicy(Dali::WebEngineCookieManager::CookieAcceptPolicy cookieAcceptPolicy)
{
  mWebEngineCookieManager.SetCookieAcceptPolicy(cookieAcceptPolicy);
}

Dali::WebEngineCookieManager::CookieAcceptPolicy WebCookieManager::GetCookieAcceptPolicy() const
{
  return mWebEngineCookieManager.GetCookieAcceptPolicy();
}

void WebCookieManager::ClearCookies()
{
  mWebEngineCookieManager.ClearCookies();
}

void WebCookieManager::SetPersistentStorage(const std::string& path, Dali::WebEngineCookieManager::CookiePersistentStorage storage)
{
  mWebEngineCookieManager.SetPersistentStorage(path, storage);
}

void WebCookieManager::ChangesWatch(Dali::WebEngineCookieManager::WebEngineCookieManagerChangesWatchCallback callback)
{
  mWebEngineCookieManager.ChangesWatch(callback);
}

} // namespace Toolkit

} // namespace Dali
