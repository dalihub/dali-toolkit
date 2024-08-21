/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/async-text/async-text-module.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-module-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
AsyncTextModule::AsyncTextModule()
{
}

AsyncTextModule::~AsyncTextModule()
{
}

AsyncTextModule::AsyncTextModule(Internal::AsyncTextModule* implementation)
: BaseHandle(implementation)
{
}

AsyncTextModule AsyncTextModule::New()
{
  auto asyncTextModuleImpl = new Internal::AsyncTextModule();

  return AsyncTextModule(asyncTextModuleImpl);
}

void AsyncTextModule::ClearCache()
{
  GetImplementation(*this).ClearCache();
}

TextAbstraction::BidirectionalSupport& AsyncTextModule::GetBidirectionalSupport()
{
  return GetImplementation(*this).GetBidirectionalSupport();
}

TextAbstraction::FontClient& AsyncTextModule::GetFontClient()
{
  return GetImplementation(*this).GetFontClient();
}

TextAbstraction::Shaping& AsyncTextModule::GetShaping()
{
  return GetImplementation(*this).GetShaping();
}

TextAbstraction::Segmentation& AsyncTextModule::GetSegmentation()
{
  return GetImplementation(*this).GetSegmentation();
}

TextAbstraction::Hyphenation& AsyncTextModule::GetHyphenation()
{
  return GetImplementation(*this).GetHyphenation();
}

Text::MultilanguageSupport& AsyncTextModule::GetMultilanguageSupport()
{
  return GetImplementation(*this).GetMultilanguageSupport();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
