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
#include <dali-toolkit/devel-api/controls/web-view/web-back-forward-list.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list-item.h>
#include <dali/devel-api/adaptor-framework/web-engine-back-forward-list.h>

namespace Dali
{
namespace Toolkit
{

WebBackForwardList::WebBackForwardList( const Dali::WebEngineBackForwardList& list )
: mWebEngineBackForwardList( list )
, mWebBackForwardListItem( 0 )
{
}

WebBackForwardList::~WebBackForwardList()
{
}

WebBackForwardListItem* WebBackForwardList::GetCurrentItem()
{
  mWebBackForwardListItem = WebBackForwardListItem( &mWebEngineBackForwardList.GetCurrentItem() );
  return &mWebBackForwardListItem;
}

WebBackForwardListItem* WebBackForwardList::GetItemAtIndex( uint32_t index )
{
  mWebBackForwardListItem = WebBackForwardListItem( &mWebEngineBackForwardList.GetItemAtIndex( index ) );
  return &mWebBackForwardListItem;
}

uint32_t WebBackForwardList::GetItemCount() const
{
  return mWebEngineBackForwardList.GetItemCount();
}

} // namespace Toolkit

} // namespace Dali
