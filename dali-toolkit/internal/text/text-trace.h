#ifndef DALI_TOOLKIT_INTERNAL_TEXT_TRACE_H
#define DALI_TOOLKIT_INTERNAL_TEXT_TRACE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include <dali/integration-api/trace.h>

#if defined(TRACE_ENABLED)
namespace DALI_NAMESPACE::Toolkit::Text::Internal
{
/**
 * Text scopes use constant tags, with no formatting or local time collection.
 * Keep the disabled path inline: the generic Tracer has out-of-line ctor/dtor
 * calls even when its filter is disabled. Capture the filter at begin so every
 * emitted begin is paired, including early returns.
 */
class TraceScope
{
public:
  TraceScope(Dali::Integration::Trace::Filter* filter, const char* tag)
  : mFilter(filter && filter->IsTraceEnabled() ? filter : nullptr),
    mTag(tag)
  {
    if(mFilter)
    {
      mFilter->BeginTrace(mTag);
    }
  }

  ~TraceScope()
  {
    End();
  }

  // End before optional effect publication or user callbacks, without changing
  // the render pipeline. The destructor still covers all earlier returns.
  void End()
  {
    if(mFilter)
    {
      mFilter->EndTrace(mTag);
      mFilter = nullptr;
    }
  }

  TraceScope(const TraceScope&)            = delete;
  TraceScope& operator=(const TraceScope&) = delete;

private:
  Dali::Integration::Trace::Filter* mFilter;
  const char*                       mTag;
};
} // namespace DALI_NAMESPACE::Toolkit::Text::Internal

#define DALI_TEXT_TRACE_SCOPE(filter, tag) Dali::Toolkit::Text::Internal::TraceScope textTraceScope(filter, tag)
#define DALI_TEXT_TRACE_END() textTraceScope.End()
#else
#define DALI_TEXT_TRACE_SCOPE(filter, tag)
#define DALI_TEXT_TRACE_END()
#endif

#endif // DALI_TOOLKIT_INTERNAL_TEXT_TRACE_H
