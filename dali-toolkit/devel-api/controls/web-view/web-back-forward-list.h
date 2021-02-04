#ifndef DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_H
#define DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list-item.h>
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
  WebBackForwardList(const Dali::WebEngineBackForwardList& list);

  /**
   * @brief Destructor.
   */
  virtual ~WebBackForwardList() final;

  /**
   * @brief Returns the current item in the @a list.
   * @return The current item in list.
   */
  WebBackForwardListItem* GetCurrentItem();

  /**
   * @brief Returns the item at a given @a index relative to the current item.
   * @param[in] index The index of the item
   * @return The current item in list.
   */
  WebBackForwardListItem* GetItemAtIndex(uint32_t index);

  /**
   * @brief Returns the length of the back-forward list including the current
   *        item.
   * @return The length of the back-forward list including the current item,
   *         otherwise 0 in case of an error
   */
  uint32_t GetItemCount() const;

private:
  const Dali::WebEngineBackForwardList& mWebEngineBackForwardList;
  Dali::Toolkit::WebBackForwardListItem mWebBackForwardListItem;
};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_WEB_BACK_FORWARD_LIST_H
