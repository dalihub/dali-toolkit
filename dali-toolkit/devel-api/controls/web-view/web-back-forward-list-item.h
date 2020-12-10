#ifndef DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_ITEM_H
#define DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_ITEM_H

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
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{

class WebEngineBackForwardListItem;

namespace Toolkit
{

/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebBackForwardListItem is a class for back-forward list item of WebView.
 *
 *
 * For working WebBackForwardListItem, a Dali::WebBackForwardListItem should be provided.
 *
 */
class DALI_TOOLKIT_API WebBackForwardListItem
{
public:
  /**
   * @brief Creates a WebBackForwardListItem.
   */
  WebBackForwardListItem( const Dali::WebEngineBackForwardListItem* item );

  /**
   * @brief Destructor.
   */
  virtual ~WebBackForwardListItem() final;

  /**
   * @brief Returns the URL of the item.
   *
   * @details The returned URL may differ from the original URL (For example,
   *          if the page is redirected).
   *
   * @return The URL of the @a item, otherwise "" in case of an error
   */
  std::string GetUrl() const;

  /**
   * @brief Returns the title of the item.
   *
   * @return The title of the @a item, otherwise "" in case of an error
   */
  std::string GetTitle() const;

  /**
   * @brief Returns the original URL of the item.
   *
   * @return The original URL of the @a item, otherwise "" in case of an error
   */
  std::string GetOriginalUrl() const;

private:

  const Dali::WebEngineBackForwardListItem* mWebEngineBackForwardListItem;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_ITEM_H
