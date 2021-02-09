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
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list-item.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list-item.h>

namespace Dali
{
namespace Toolkit
{
const std::string EMPTY_STRING;

WebBackForwardListItem::WebBackForwardListItem(const Dali::WebEngineBackForwardListItem* item)
: mWebEngineBackForwardListItem(item)
{
}

WebBackForwardListItem::~WebBackForwardListItem()
{
}

std::string WebBackForwardListItem::GetUrl() const
{
  return mWebEngineBackForwardListItem ? mWebEngineBackForwardListItem->GetUrl() : EMPTY_STRING;
}

std::string WebBackForwardListItem::GetTitle() const
{
  return mWebEngineBackForwardListItem ? mWebEngineBackForwardListItem->GetTitle() : EMPTY_STRING;
}

std::string WebBackForwardListItem::GetOriginalUrl() const
{
  return mWebEngineBackForwardListItem ? mWebEngineBackForwardListItem->GetOriginalUrl() : EMPTY_STRING;
}

} // namespace Toolkit

} // namespace Dali
