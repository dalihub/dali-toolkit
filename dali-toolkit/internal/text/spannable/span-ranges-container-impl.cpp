
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/spannable/span-ranges-container-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <utility>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
struct SpanRangesContainer::Impl
{
  using SpanId = uint32_t;

  // Key : BaseSpan, Value : <Range, Added order>
  using SpanRangeContainer = std::map<Dali::Toolkit::Text::BaseSpan, std::pair<Dali::Toolkit::Text::Range, SpanId>>;

  SpanRangeContainer mSpanWithRanges{}; ///< The list of style-span
  SpanId             mSpanId{0};        ///< The global id for each added Span. It will be used when we determine order of span.
};

SpanRangesContainer::SpanRangesContainer()
{
  mImpl = std::make_unique<Impl>();
}

SpanRangesContainer::~SpanRangesContainer()
{
}

void SpanRangesContainer::AddSpan(const Dali::Toolkit::Text::BaseSpan& span, const Dali::Toolkit::Text::Range& range)
{
  mImpl->mSpanWithRanges.insert(std::make_pair(span, std::make_pair(range, mImpl->mSpanId++)));
}

void SpanRangesContainer::RemoveSpan(const Dali::Toolkit::Text::BaseSpan& span)
{
  mImpl->mSpanWithRanges.erase(span);
}

bool SpanRangesContainer::Contains(const Dali::Toolkit::Text::BaseSpan& span) const
{
  const auto it = mImpl->mSpanWithRanges.find(span);

  return it != mImpl->mSpanWithRanges.end();
}

void SpanRangesContainer::GetSpans(std::vector<Dali::Toolkit::Text::BaseSpan>& listOfSpans) const
{
  std::map<Impl::SpanId, Dali::Toolkit::Text::BaseSpan> reorderedListOfSpans;

  listOfSpans.reserve(listOfSpans.size() + mImpl->mSpanWithRanges.size());

  // At first, sort the list of spans ordered by added time.
  for(auto iter = mImpl->mSpanWithRanges.begin(), iterEnd = mImpl->mSpanWithRanges.end(); iter != iterEnd; ++iter)
  {
    reorderedListOfSpans.insert(std::make_pair(iter->second.second, iter->first));
  }

  // Retrieve result.
  for(auto iter = reorderedListOfSpans.begin(), iterEnd = reorderedListOfSpans.end(); iter != iterEnd; ++iter)
  {
    listOfSpans.push_back(iter->second);
  }
}

void SpanRangesContainer::GetSpansAndRanges(std::vector<Dali::Toolkit::Text::BaseSpan>& spans, std::vector<Dali::Toolkit::Text::Range>& ranges) const
{
  std::map<Impl::SpanId, std::pair<Dali::Toolkit::Text::BaseSpan, Dali::Toolkit::Text::Range>> reorderedListOfSpansWithRanges;

  spans.reserve(spans.size() + mImpl->mSpanWithRanges.size());
  ranges.reserve(ranges.size() + mImpl->mSpanWithRanges.size());

  // At first, sort the list of spans ordered by added time.
  for(auto iter = mImpl->mSpanWithRanges.begin(), iterEnd = mImpl->mSpanWithRanges.end(); iter != iterEnd; ++iter)
  {
    reorderedListOfSpansWithRanges.insert(std::make_pair(iter->second.second, std::make_pair(iter->first, iter->second.first)));
  }

  // Retrieve result.
  for(auto iter = reorderedListOfSpansWithRanges.begin(), iterEnd = reorderedListOfSpansWithRanges.end(); iter != iterEnd; ++iter)
  {
    spans.push_back(iter->second.first);
    ranges.push_back(iter->second.second);
  }
}
} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
