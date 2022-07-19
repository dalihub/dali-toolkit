#ifndef DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_H
#define DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-back-forward-list-item.h>
#include <vector>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
class WebEngineBackForwardList;

namespace Toolkit
{
class WebBackForwardListItem;

/**
 * @addtogroup dali_toolkit_controls_web_view
 * @{
 */

/**
 * @brief WebBackForwardList is a class for back-forward list item of WebView.
 *
 *
 * For working WebBackForwardList, a Dali::WebBackForwardList should be provided.
 *
 */
class DALI_TOOLKIT_API WebBackForwardList
{
public:
  /**
   * @brief Creates a WebBackForwardList.
   */
  WebBackForwardList(Dali::WebEngineBackForwardList& list);

  /**
   * @brief Destructor.
   */
  virtual ~WebBackForwardList() final;

  /**
   * @brief Returns the current item in the @a list.
   * @return The current item in list.
   */
  std::unique_ptr<Dali::WebEngineBackForwardListItem> GetCurrentItem();

  /**
   * @brief Returns the previous item in the @a list.
   * @return The previous item in list.
   */
  std::unique_ptr<Dali::WebEngineBackForwardListItem> GetPreviousItem();

  /**
   * @brief Returns the next item in the @a list.
   * @return The next item in list.
   */
  std::unique_ptr<Dali::WebEngineBackForwardListItem> GetNextItem();

  /**
   * @brief Returns the item at a given @a index relative to the current item.
   * @param[in] index The index of the item
   * @return The current item in list.
   */
  std::unique_ptr<Dali::WebEngineBackForwardListItem> GetItemAtIndex(uint32_t index);

  /**
   * @brief Returns the length of the back-forward list including the current
   *        item.
   * @return The length of the back-forward list including the current item,
   *         otherwise 0 in case of an error
   */
  uint32_t GetItemCount() const;

  /**
   * @brief Creates a list containing the items preceding the current item limited
   *        by @a limit.
   *
   * @details The WebEngineBackForwardListItem elements are located in the result
              list starting with the oldest one.\n
   *          If @a limit is equal to @c -1 all the items preceding the current
   *          item are returned.
   *
   * @param[in] limit The number of items to retrieve
   *
   * @return @c vector containing @c WebEngineBackForwardListItem elements,\n
   */
  std::vector<std::unique_ptr<Dali::WebEngineBackForwardListItem>> GetBackwardItems(int limit);

  /**
   * @brief Creates the list containing the items following the current item
   *        limited by @a limit.
   *
   * @details The @c WebEngineBackForwardListItem elements are located in the result
   *          list starting with the oldest one.\n
   *          If @a limit is equal to @c -1 all the items preceding the current
   *          item are returned.
   *
   * @param[in] limit The number of items to retrieve
   *
   * @return @c vector containing @c WebEngineBackForwardListItem elements,\n
   */
  std::vector<std::unique_ptr<Dali::WebEngineBackForwardListItem>> GetForwardItems(int limit);

private:
  Dali::WebEngineBackForwardList& mWebEngineBackForwardList;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_H
