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

// CLASS  HEADER
#include <dali-toolkit/internal/text/async-text/async-text-module-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace
{
} // namespace

namespace Text
{
namespace Internal
{

AsyncTextModule::AsyncTextModule()
: mBidirectionalSupport(),
  mFontClient(),
  mShaping(),
  mSegmentation(),
  mHyphenation(),
  mMultilanguageSupport()
{
  bool connnectLocaleChangedSignal = false;

  mBidirectionalSupport = TextAbstraction::BidirectionalSupport::New();
  mFontClient           = TextAbstraction::FontClient::New();
  mShaping              = TextAbstraction::Shaping::New();
  mSegmentation         = TextAbstraction::Segmentation::New();
  mHyphenation          = TextAbstraction::Hyphenation::New();
  mMultilanguageSupport = Text::MultilanguageSupport::New(connnectLocaleChangedSignal);

  mFontClient.InitDefaultFontDescription();
}

AsyncTextModule::~AsyncTextModule()
{
}

void AsyncTextModule::ClearCache()
{
  mFontClient.ClearCacheOnLocaleChanged();
  mFontClient.InitDefaultFontDescription();
  mMultilanguageSupport.ClearCache();
}

TextAbstraction::BidirectionalSupport& AsyncTextModule::GetBidirectionalSupport()
{
  return mBidirectionalSupport;
}

TextAbstraction::FontClient& AsyncTextModule::GetFontClient()
{
  return mFontClient;
}

TextAbstraction::Shaping& AsyncTextModule::GetShaping()
{
  return mShaping;
}

TextAbstraction::Segmentation& AsyncTextModule::GetSegmentation()
{
  return mSegmentation;
}

TextAbstraction::Hyphenation& AsyncTextModule::GetHyphenation()
{
  return mHyphenation;
}

Text::MultilanguageSupport& AsyncTextModule::GetMultilanguageSupport()
{
  return mMultilanguageSupport;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
