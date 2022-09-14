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
#include <dali-toolkit/internal/text/string-text/range-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
struct Range::Impl
{
  u_int32_t mStartIndex; /// The start index of range
  u_int32_t mEndIndex;   /// The end index of range
};

Dali::Toolkit::Text::Range Range::New(u_int32_t startIndex, u_int32_t endIndex)
{
  RangePtr object = new Range(startIndex, endIndex);

  Dali::Toolkit::Text::Range handle = Dali::Toolkit::Text::Range(object.Get());

  return handle;
}

Range::Range(u_int32_t startIndex, u_int32_t endIndex)
{
  mImpl = std::make_unique<Impl>();

  mImpl->mStartIndex = startIndex <= endIndex ? startIndex : endIndex;
  mImpl->mEndIndex   = startIndex >= endIndex ? startIndex : endIndex;
}

Range::~Range()
{
}

u_int32_t Range::GetStartIndex() const
{
  return mImpl->mStartIndex;
}

u_int32_t Range::GetEndIndex() const
{
  return mImpl->mEndIndex;
}

u_int32_t Range::GetNumberOfIndices() const
{
  return 1u + ((mImpl->mStartIndex <= mImpl->mEndIndex) ? (mImpl->mEndIndex - mImpl->mStartIndex) : (mImpl->mStartIndex - mImpl->mEndIndex));
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
