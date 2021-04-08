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
   * @brief Toggle the cache to be enabled or disabled
   *
   * Function works asynchronously.
   * By default the cache is disabled resulting in not storing network data on disk.
   *
   * @param[in] cacheDisabled enable or disable cache
   */
  void DisableCache(bool cacheDisabled);

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
   * @brief Delete origin that is stored in web storage db.
   *
   * @param[in] origin origin of db
   *
   * @return true if succeeded, false otherwise
   */
  bool DeleteWebStorageOrigin(Dali::WebEngineSecurityOrigin& origin);

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

private:
  Dali::WebEngineContext& mWebEngineContext;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_CONTEXT_H
