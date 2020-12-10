/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/web-view/web-context.h>

namespace Dali
{
namespace Toolkit
{

WebContext::WebContext( Dali::WebEngineContext& context)
: mWebEngineContext( context )
{
}

WebContext::~WebContext()
{
}

Dali::WebEngineContext::CacheModel WebContext::GetCacheModel() const
{
  return mWebEngineContext.GetCacheModel();
}

void WebContext::SetCacheModel(Dali::WebEngineContext::CacheModel cacheModel )
{
  mWebEngineContext.SetCacheModel( cacheModel );
}

void WebContext::SetProxyUri( const std::string& uri )
{
  mWebEngineContext.SetProxyUri( uri );
}

void WebContext::SetCertificateFilePath( const std::string& certificatePath )
{
  mWebEngineContext.SetCertificateFilePath( certificatePath );
}

void WebContext::DisableCache( bool cacheDisabled )
{
  mWebEngineContext.DisableCache( cacheDisabled );
}

void WebContext::SetDefaultProxyAuth( const std::string& username, const std::string& password )
{
  mWebEngineContext.SetDefaultProxyAuth( username, password );
}

void WebContext::DeleteWebDatabase()
{
  mWebEngineContext.DeleteWebDatabase();
}

void WebContext::DeleteWebStorage()
{
  mWebEngineContext.DeleteWebStorage();
}

void WebContext::DeleteLocalFileSystem()
{
  mWebEngineContext.DeleteLocalFileSystem();
}

void WebContext::ClearCache()
{
  mWebEngineContext.ClearCache();
}

} // namespace Toolkit

} // namespace Dali
