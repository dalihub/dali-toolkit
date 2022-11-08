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
#include <dali-toolkit/devel-api/text/spannable-string.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spannable-string-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
SpannableString SpannableString::New(const std::string& text)
{
  return Internal::SpannableString::New(text);
}

SpannableString::SpannableString(Internal::SpannableString* internal)
: Spannable(internal)
{
}

SpannableString::SpannableString(const SpannableString& rhs) = default;

SpannableString::SpannableString(SpannableString&& rhs) = default;

SpannableString& SpannableString::operator=(const SpannableString& rhs) = default;

SpannableString& SpannableString::operator=(SpannableString&& rhs) = default;

SpannableString::~SpannableString() = default;

} // namespace Text

} // namespace Toolkit

} // namespace Dali
