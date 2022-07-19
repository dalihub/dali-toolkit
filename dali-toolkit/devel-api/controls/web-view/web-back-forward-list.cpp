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

// CLASS HEADER
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-back-forward-list.h>

namespace Dali
{
namespace Toolkit
{
WebBackForwardList::WebBackForwardList(Dali::WebEngineBackForwardList& list)
: mWebEngineBackForwardList(list)
{
}

WebBackForwardList::~WebBackForwardList()
{
}

std::unique_ptr<Dali::WebEngineBackForwardListItem> WebBackForwardList::GetCurrentItem()
{
  return mWebEngineBackForwardList.GetCurrentItem();
}

std::unique_ptr<Dali::WebEngineBackForwardListItem> WebBackForwardList::GetPreviousItem()
{
  return mWebEngineBackForwardList.GetPreviousItem();
}

std::unique_ptr<Dali::WebEngineBackForwardListItem> WebBackForwardList::GetNextItem()
{
  return mWebEngineBackForwardList.GetNextItem();
}

std::unique_ptr<Dali::WebEngineBackForwardListItem> WebBackForwardList::GetItemAtIndex(uint32_t index)
{
  return mWebEngineBackForwardList.GetItemAtIndex(index);
}

uint32_t WebBackForwardList::GetItemCount() const
{
  return mWebEngineBackForwardList.GetItemCount();
}

std::vector<std::unique_ptr<Dali::WebEngineBackForwardListItem>> WebBackForwardList::GetBackwardItems(int limit)
{
  return mWebEngineBackForwardList.GetBackwardItems(limit);
}

std::vector<std::unique_ptr<Dali::WebEngineBackForwardListItem>> WebBackForwardList::GetForwardItems(int limit)
{
  return mWebEngineBackForwardList.GetForwardItems(limit);
}

} // namespace Toolkit

} // namespace Dali
