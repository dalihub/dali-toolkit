#ifndef DALI_TOOLKIT_WEB_CONTEXT_H
#define DALI_TOOLKIT_WEB_CONTEXT_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-context.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
class WebEngineSecurityOrigin;

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebContext is a control for context of WebView.
 *
 * For working WebContext, a WebEngineContext should be provided.
 *
 */
class DALI_TOOLKIT_API WebContext
{
public:
  /**
   * @brief Create a WebContext.
   *
   * @param[in] context The context of web engine.
   */
  WebContext(Dali::WebEngineContext& context);

  /**
   * @brief Destructor.
   */
  virtual ~WebContext() final;

  /**
   * @brief Return the cache model type.
   *
   * @return #Dali::WebEngineContext::CacheModel
   */
  Dali::WebEngineContext::CacheModel GetCacheModel() const;

  /**
   * @brief Request to set the cache model.
   *
   * @param[in] cacheModel The cache model
   */
  void SetCacheModel(Dali::WebEngineContext::CacheModel cacheModel);

  /**
   * @brief Set the given proxy URI to network backend of specific context.
   *
   * @param[in] uri The proxy URI to set
   */
  void SetProxyUri(const std::string& uri);

  /**
   * @brief Get the proxy URI from the network backend of specific context.
   *
   * @return current proxy URI or null string if it's not set
   */
  std::string GetProxyUri() const;

  /**
   * @brief Set the given proxy to network backend of specific context.
   *
   * @param[in] proxy URI to set
   * @param[in] bypass rule to set
   */
  void SetProxyBypassRule(const std::string& proxy, const std::string& bypass);

  /**
   * @brief Get the proxy bypass rule from the network backend of specific context.
   *
   * @return current proxy bypass rule or null string if it's not set
   */
  std::string GetProxyBypassRule() const;

  /**
   * @brief Add CA certificates to persistent NSS certificate database
   *
   * Function accepts a path to a CA certificate file, a path to a directory
   * containing CA certificate files, or a colon-seprarated list of those.
   * Certificate files should have *.crt extension.
   * Directories are traversed recursively.
   *
   * @param[in] certificatePath path to a CA certificate file(s), see above for details
   */
  void SetCertificateFilePath(const std::string& certificatePath);

  /**
   * @brief Get CA certifcate file path
   *
   * It returns an internal string and should not be modified.
   *
   * @return certificate_file path which is set during ewk_context_certificate_file_set or null string otherwise
   */
  std::string GetCertificateFilePath() const;

  /**
   * @brief Set a proxy auth credential to network backend of specific context.
   *
   * @param[in] username username to set
   * @param[in] password password to set
   */
  void SetDefaultProxyAuth(const std::string& username, const std::string& password);

  /**
   * @brief Requests for deleting all web databases.
   */
  void DeleteAllWebDatabase();

  /**
   * @brief Request for getting web database origins.
   *
   * @param[in] callback callback called after getting web database origins
   *
   * @return true if succeeded, false otherwise
   */
  bool GetWebDatabaseOrigins(Dali::WebEngineContext::WebEngineSecurityOriginAcquiredCallback callback);

  /**
   * @brief Request for deleting web databases for origin.
   *
   * @param[in] origin database origin
   *
   * @return true if succeeded, false otherwise
   */
  bool DeleteWebDatabase(Dali::WebEngineSecurityOrigin& origin);

  /**
   * @brief Gets list of origins that is stored in web storage db.
   *
   * @param[in] callback callback called after getting web storage origins
   *
   * @return true if succeeded, false otherwise
   */
  bool GetWebStorageOrigins(Dali::WebEngineContext::WebEngineSecurityOriginAcquiredCallback callback);

  /**
   * @brief Get list of origins that is stored in web storage db.
   *
   * @param[in] origin storage origin
   * @param[in] callback callback called after getting web storage origins
   *
   * @return true if succeeded, false otherwise
   */
  bool GetWebStorageUsageForOrigin(Dali::WebEngineSecurityOrigin& origin, Dali::WebEngineContext::WebEngineStorageUsageAcquiredCallback callback);

  /**
   * @brief Delete all web storage.
   *
   * @details This function does not ensure that all data will be removed.
   *          Should be used to extend free physical memory.
   */
  void DeleteAllWebStorage();

  /**
   * @brief Delete web storage db.
   *
   * @param[in] origin origin of db
   *
   * @return true if succeeded, false otherwise
   */
  bool DeleteWebStorage(Dali::WebEngineSecurityOrigin& origin);

  /**
   * @brief Request for deleting all local file systems.
   */
  void DeleteLocalFileSystem();

  /**
   * @brief Requests to clear cache
   */
  void ClearCache();

  /**
   * @brief Request for deleting web application cache for origin.
   *
   * @param[in] origin application cache origin
   *
   * @return true if succeeded, false otherwise
   */
  bool DeleteApplicationCache(Dali::WebEngineSecurityOrigin& origin);

  /**
   * @brief Asynchronous request to get list of all password data.
   *
   * @param[in] callback callback called after getting form password
   */
  void GetFormPasswordList(Dali::WebEngineContext::WebEngineFormPasswordAcquiredCallback callback);

  /**
   * @brief Register callback for download started.
   *
   * @param[in] callback callback for download started
   */
  void RegisterDownloadStartedCallback(Dali::WebEngineContext::WebEngineDownloadStartedCallback callback);

  /**
   * @brief Register callback for mime overridden.
   *
   * @param[in] callback callback for mime overridden
   */
  void RegisterMimeOverriddenCallback(Dali::WebEngineContext::WebEngineMimeOverriddenCallback callback);

  /**
   * @brief Toggle the cache to be enabled or disabled
   *
   * @param[in] cacheEnabled enable or disable cache
   */
  void EnableCache(bool cacheEnabled);

  /**
   * @brief Query if the cache is enabled
   *
   * @return true is cache is enabled or false otherwise
   */
  bool IsCacheEnabled() const;

  /**
   * @brief Set application id for context.
   *
   * @param[in] appId application id
   */
  void SetAppId(const std::string& appId);

  /**
   * @brief Set application version for context.
   *
   * @param[in] appVersion application version
   *
   * @return true if successful, false otherwise
   */
  bool SetAppVersion(const std::string& appVersion);

  /**
   * @brief To declare application type
   *
   * @param[in] applicationType The Application_Type enum
   *
   */
  void SetApplicationType(const Dali::WebEngineContext::ApplicationType applicationType);

  /**
   * @brief Set time offset
   *
   * @param[in] timeOffset The value will be added to system time as offset
   */
  void SetTimeOffset(float timeOffset);

  /**
   * @brief Set timezone offset
   *
   * @param[in] timeZoneOffset offset for time zone.
   * @param[in] daylightSavingTime The value is for daylight saving time use.
   */
  void SetTimeZoneOffset(float timeZoneOffset, float daylightSavingTime);

  /**
   * @brief Set default zoom factor
   *
   * @param[in] zoomFactor default zoom factor
   */
  void SetDefaultZoomFactor(float zoomFactor);

  /**
   * @brief Get default zoom factor
   *
   * Gets default zoom factor for all pages opened with this context.
   *
   * @return default zoom factor or negative value on error
   */
  float GetDefaultZoomFactor() const;

  /**
   * @brief Register url schemes as CORS enabled
   *
   * @param[in] schemes The URL schemes list which will be added to web security policy
   *
   */
  void RegisterUrlSchemesAsCorsEnabled(const std::vector<std::string>& schemes);

  /**
   * @brief Register JS plugin mime types.
   *
   * @param[in] mimeTypes The MIME types will be checked by the renderer frame loader
   * to skip creating default frame for the object tags with the registered MIME type.
   */
  void RegisterJsPluginMimeTypes(const std::vector<std::string>& mimeTypes);

  /**
   * @brief Request for deleting all web application caches.
   *
   * @return @c true on success, otherwise @c false
   */
  bool DeleteAllApplicationCache();

  /**
   * @brief Request for deleting all web indexed databases.
   *
   * @return @c true on success, otherwise @c false
   */
  bool DeleteAllWebIndexedDatabase();

  /**
   * @brief Delete a given password data list
   *
   * @param[in] list List with Ewk_Password_Data
   */
  void DeleteFormPasswordDataList(const std::vector<std::string>& list);

  /**
   * @brief Delete whole password data from DB
   */
  void DeleteAllFormPasswordData();

  /**
   * @brief Delete all candidate form data from DB
   */
  void DeleteAllFormCandidateData();

  /**
   * @brief Notify low memory to free unused memory.
   *
   * @return @c true on success or @c false otherwise.
   */
  bool FreeUnusedMemory();

private:
  Dali::WebEngineContext& mWebEngineContext;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_CONTEXT_H
