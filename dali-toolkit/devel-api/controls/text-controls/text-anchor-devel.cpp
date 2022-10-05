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
#include <dali-toolkit/devel-api/controls/text-controls/text-anchor-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-anchor-impl.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
TextAnchor TextAnchor::New()
{
  return Internal::TextAnchor::New();
}

TextAnchor::TextAnchor()
{
}

TextAnchor::TextAnchor(const TextAnchor& handle) = default;

TextAnchor& TextAnchor::operator=(const TextAnchor& handle) = default;

TextAnchor::TextAnchor(TextAnchor&& handle) = default;

TextAnchor& TextAnchor::operator=(TextAnchor&& handle) = default;

TextAnchor::~TextAnchor()
{
}

TextAnchor TextAnchor::DownCast(BaseHandle handle)
{
  return Control::DownCast<TextAnchor, Internal::TextAnchor>(handle);
}

TextAnchor::TextAnchor(Internal::TextAnchor& implementation)
: Control(implementation)
{
}

TextAnchor::TextAnchor(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::TextAnchor>(internal);
}

} // namespace Toolkit

} // namespace Dali
