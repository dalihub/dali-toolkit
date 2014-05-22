/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace // unnamed namespace
{

BaseHandle Create()
{
  // empty handle as we cannot create ScrollComponent
  return BaseHandle();
}

TypeRegistration mType( typeid(Toolkit::ScrollComponent), typeid(Toolkit::Control), Create );

} // unnamed namespace

void ScrollComponentImpl::SetScrollConnector( Toolkit::ScrollConnector connector )
{
  if( mScrollConnector != connector )
  {
    Toolkit::ScrollConnector oldConnector = mScrollConnector;
    mScrollConnector = connector;

    // Notify derived classes
    OnScrollConnectorSet( oldConnector );
  }
}

Toolkit::ScrollConnector ScrollComponentImpl::GetScrollConnector() const
{
  return mScrollConnector;
}

ScrollComponentImpl::ScrollComponentImpl()
: ControlImpl(true/*requires touch*/)
{
}

ScrollComponentImpl::~ScrollComponentImpl()
{
}

void ScrollComponentImpl::OnScrollConnectorSet( Toolkit::ScrollConnector connector )
{
  // Do nothing (override in derived classes)
}

} // namespace Toolkit

} // namespace Dali
