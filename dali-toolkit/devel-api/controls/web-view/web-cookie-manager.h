#ifndef DALI_TOOLKIT_WEB_COOKIE_MANAGER_H
#define DALI_TOOLKIT_WEB_COOKIE_MANAGER_H

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
#include <dali/devel-api/adaptor-framework/web-engine-cookie-manager.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
class WebEngineCookieManager;

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebCookieManager is a control for settings of WebView.
 *
 *
 * For working WebCookieManager, a WebView should be provided.
 *
 */
class DALI_TOOLKIT_API WebCookieManager
{
public:
  /**
   * @brief Creates a WebCookieManager.
   * @param[in] manager A #Dali::WebEngineCookieManager
   */
  WebCookieManager(Dali::WebEngineCookieManager& manager);

  /**
   * @brief Destructor.
   */
  virtual ~WebCookieManager() final;

  /**
   * @brief Sets @a policy as the cookie acceptance policy for @a manager.
   *
   * @details By default, only cookies set by the main document loaded are
   *          accepted.
   *
   * @param[in] policy A #Dali::WebEngineCookieManager::CookieAcceptPolicy
   */
  void SetCookieAcceptPolicy(Dali::WebEngineCookieManager::CookieAcceptPolicy policy);

  /**
   * @brief Gets the cookie acceptance policy. The default is Toolkit::WebCookieManager::CookieAcceptPolicy::NO_THIRD_PARTY.
   * @see Dali::WebEngineCookieManager::CookieAcceptPolicy
   */
  Dali::WebEngineCookieManager::CookieAcceptPolicy GetCookieAcceptPolicy() const;

  /**
   * @brief Deletes all the cookies of @a manager.
   */
  void ClearCookies();

  /**
   * @brief Sets the @a path where non-session cookies are stored persistently using
   *        @a storage as the format to read/write the cookies.
   *
   * @details Cookies are initially read from @a path/Cookies to create an initial
   *          set of cookies. Then, non-session cookies will be written to @a path/Cookies.
   *          By default, @a manager doesn't store the cookies persistently, so you need to
   *          call this method to keep cookies saved across sessions.
   *          If @a path does not exist it will be created.
   *
   * @param[in] path The path where to read/write Cookies
   * @param[in] storage The type of storage
   */
  void SetPersistentStorage(const std::string& path, Dali::WebEngineCookieManager::CookiePersistentStorage storage);

private:
  Dali::WebEngineCookieManager& mWebEngineCookieManager;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_COOKIE_MANAGER_H
