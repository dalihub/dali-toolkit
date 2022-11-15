
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

// EXTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/span-ranges-container-impl.h>
#include <map>

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
  std::map<Dali::Toolkit::Text::BaseSpan, Dali::Toolkit::Text::Range> mSpanWithRanges; ///< The list of style-span
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
  mImpl->mSpanWithRanges.insert(std::make_pair(span, range));
}

void SpanRangesContainer::RemoveSpan(const Dali::Toolkit::Text::BaseSpan& span)
{
  mImpl->mSpanWithRanges.erase(span);
}

bool SpanRangesContainer::Contains(const Dali::Toolkit::Text::BaseSpan& span) const
{
  std::map<Dali::Toolkit::Text::BaseSpan, Dali::Toolkit::Text::Range>::iterator it = mImpl->mSpanWithRanges.find(span);

  return it != mImpl->mSpanWithRanges.end();
}

void SpanRangesContainer::GetSpans(std::vector<Dali::Toolkit::Text::BaseSpan>& listOfSpans) const
{
  for(std::map<Dali::Toolkit::Text::BaseSpan, Dali::Toolkit::Text::Range>::iterator it = mImpl->mSpanWithRanges.begin();
      it != mImpl->mSpanWithRanges.end();
      ++it)
  {
    listOfSpans.push_back(it->first);
  }
}

void SpanRangesContainer::GetSpansAndRanges(std::vector<Dali::Toolkit::Text::BaseSpan>& spans, std::vector<Dali::Toolkit::Text::Range>& ranges) const
{
  for(std::map<Dali::Toolkit::Text::BaseSpan, Dali::Toolkit::Text::Range>::iterator it = mImpl->mSpanWithRanges.begin();
      it != mImpl->mSpanWithRanges.end();
      ++it)
  {
    spans.push_back(it->first);
    ranges.push_back(it->second);
  }
}
} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
