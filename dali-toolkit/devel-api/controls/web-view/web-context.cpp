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
#include <dali-toolkit/devel-api/controls/web-view/web-context.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-security-origin.h>

namespace Dali
{
namespace Toolkit
{
WebContext::WebContext(Dali::WebEngineContext& context)
: mWebEngineContext(context)
{
}

WebContext::~WebContext()
{
}

Dali::WebEngineContext::CacheModel WebContext::GetCacheModel() const
{
  return mWebEngineContext.GetCacheModel();
}

void WebContext::SetCacheModel(Dali::WebEngineContext::CacheModel cacheModel)
{
  mWebEngineContext.SetCacheModel(cacheModel);
}

void WebContext::SetProxyUri(const std::string& uri)
{
  mWebEngineContext.SetProxyUri(uri);
}

std::string WebContext::GetProxyUri() const
{
  return mWebEngineContext.GetProxyUri();
}

void WebContext::SetProxyBypassRule(const std::string& proxy, const std::string& bypass)
{
  mWebEngineContext.SetProxyBypassRule(proxy, bypass);
}

std::string WebContext::GetProxyBypassRule() const
{
  return mWebEngineContext.GetProxyBypassRule();
}

void WebContext::SetCertificateFilePath(const std::string& certificatePath)
{
  mWebEngineContext.SetCertificateFilePath(certificatePath);
}

std::string WebContext::GetCertificateFilePath() const
{
  return mWebEngineContext.GetCertificateFilePath();
}

void WebContext::SetDefaultProxyAuth(const std::string& username, const std::string& password)
{
  mWebEngineContext.SetDefaultProxyAuth(username, password);
}

void WebContext::DeleteAllWebDatabase()
{
  mWebEngineContext.DeleteAllWebDatabase();
}

bool WebContext::GetWebDatabaseOrigins(Dali::WebEngineContext::WebEngineSecurityOriginAcquiredCallback callback)
{
  return mWebEngineContext.GetWebDatabaseOrigins(callback);
}

bool WebContext::DeleteWebDatabase(Dali::WebEngineSecurityOrigin& origin)
{
  return mWebEngineContext.DeleteWebDatabase(origin);
}

bool WebContext::GetWebStorageOrigins(Dali::WebEngineContext::WebEngineSecurityOriginAcquiredCallback callback)
{
  return mWebEngineContext.GetWebStorageOrigins(callback);
}

bool WebContext::GetWebStorageUsageForOrigin(Dali::WebEngineSecurityOrigin& origin, Dali::WebEngineContext::WebEngineStorageUsageAcquiredCallback callback)
{
  return mWebEngineContext.GetWebStorageUsageForOrigin(origin, callback);
}

void WebContext::DeleteAllWebStorage()
{
  mWebEngineContext.DeleteAllWebStorage();
}

bool WebContext::DeleteWebStorage(Dali::WebEngineSecurityOrigin& origin)
{
  return mWebEngineContext.DeleteWebStorage(origin);
}

void WebContext::DeleteLocalFileSystem()
{
  mWebEngineContext.DeleteLocalFileSystem();
}

void WebContext::ClearCache()
{
  mWebEngineContext.ClearCache();
}

bool WebContext::DeleteApplicationCache(Dali::WebEngineSecurityOrigin& origin)
{
  return mWebEngineContext.DeleteApplicationCache(origin);
}

void WebContext::GetFormPasswordList(Dali::WebEngineContext::WebEngineFormPasswordAcquiredCallback callback)
{
  mWebEngineContext.GetFormPasswordList(callback);
}

void WebContext::RegisterDownloadStartedCallback(Dali::WebEngineContext::WebEngineDownloadStartedCallback callback)
{
  mWebEngineContext.RegisterDownloadStartedCallback(callback);
}

void WebContext::RegisterMimeOverriddenCallback(Dali::WebEngineContext::WebEngineMimeOverriddenCallback callback)
{
  mWebEngineContext.RegisterMimeOverriddenCallback(callback);
}

void WebContext::EnableCache(bool cacheEnabled)
{
  mWebEngineContext.EnableCache(cacheEnabled);
}

bool WebContext::IsCacheEnabled() const
{
  return mWebEngineContext.IsCacheEnabled();
}

void WebContext::SetAppId(const std::string& appId)
{
  mWebEngineContext.SetAppId(appId);
}

bool WebContext::SetAppVersion(const std::string& appVersion)
{
  return mWebEngineContext.SetAppVersion(appVersion);
}

void WebContext::SetApplicationType(const Dali::WebEngineContext::ApplicationType applicationType)
{
  mWebEngineContext.SetApplicationType(applicationType);
}

void WebContext::SetTimeOffset(float timeOffset)
{
  mWebEngineContext.SetTimeOffset(timeOffset);
}

void WebContext::SetTimeZoneOffset(float timeZoneOffset, float daylightSavingTime)
{
  mWebEngineContext.SetTimeZoneOffset(timeZoneOffset, daylightSavingTime);
}

void WebContext::SetDefaultZoomFactor(float zoomFactor)
{
  mWebEngineContext.SetDefaultZoomFactor(zoomFactor);
}

float WebContext::GetDefaultZoomFactor() const
{
  return mWebEngineContext.GetDefaultZoomFactor();
}

void WebContext::RegisterUrlSchemesAsCorsEnabled(const std::vector<std::string>& schemes)
{
  mWebEngineContext.RegisterUrlSchemesAsCorsEnabled(schemes);
}

void WebContext::RegisterJsPluginMimeTypes(const std::vector<std::string>& mimeTypes)
{
  mWebEngineContext.RegisterJsPluginMimeTypes(mimeTypes);
}

bool WebContext::DeleteAllApplicationCache()
{
  return mWebEngineContext.DeleteAllApplicationCache();
}

bool WebContext::DeleteAllWebIndexedDatabase()
{
  return mWebEngineContext.DeleteAllWebIndexedDatabase();
}

void WebContext::DeleteFormPasswordDataList(const std::vector<std::string>& list)
{
  mWebEngineContext.DeleteFormPasswordDataList(list);
}

void WebContext::DeleteAllFormPasswordData()
{
  mWebEngineContext.DeleteAllFormPasswordData();
}

void WebContext::DeleteAllFormCandidateData()
{
  mWebEngineContext.DeleteAllFormCandidateData();
}

bool WebContext::FreeUnusedMemory()
{
  return mWebEngineContext.FreeUnusedMemory();
}

} // namespace Toolkit
} // namespace Dali
