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
#include <dali-toolkit/devel-api/controls/web-view/web-form-repost-decision.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine-form-repost-decision.h>

namespace Dali
{
namespace Toolkit
{
WebFormRepostDecision::WebFormRepostDecision(std::shared_ptr<Dali::WebEngineFormRepostDecision> decision)
: mFormRepostDecision(std::move(decision))
{
}

WebFormRepostDecision::~WebFormRepostDecision()
{
}

void WebFormRepostDecision::Reply(bool allowed)
{
  if(mFormRepostDecision)
  {
    mFormRepostDecision->Reply(allowed);
  }
}

} // namespace Toolkit

} // namespace Dali
