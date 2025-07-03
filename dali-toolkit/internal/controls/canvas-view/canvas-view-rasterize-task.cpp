/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "canvas-view-rasterize-task.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

#ifdef TRACE_ENABLED
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_VECTOR_ANIMATION_PERFORMANCE_MARKER, false);

#ifdef TRACE_ENABLED
uint64_t GetNanoseconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch    = std::chrono::steady_clock::now().time_since_epoch();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
  return static_cast<uint64_t>(duration.count());
}
#endif
} // namespace
CanvasRendererRasterizingTask::CanvasRendererRasterizingTask(CanvasRenderer canvasRenderer, CallbackBase* callback)
: AsyncTask(callback),
  mCanvasRenderer(canvasRenderer),
  mRasterizedSuccessed(false)
{
}

void CanvasRendererRasterizingTask::Process()
{
  mRasterizedSuccessed = Rasterize();
}

bool CanvasRendererRasterizingTask::IsReady()
{
  return true;
}

bool CanvasRendererRasterizingTask::IsRasterized()
{
  return mRasterizedSuccessed;
}

bool CanvasRendererRasterizingTask::Rasterize()
{
#ifdef TRACE_ENABLED
  uint64_t mStartTimeNanoSceonds = 0;
  uint64_t mEndTimeNanoSceonds   = 0;
#endif

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_CANVAS_VIEW_RASTERIZE_TASK", [&](std::ostringstream& oss)
                                          { mStartTimeNanoSceonds = GetNanoseconds(); });

  bool rasterized = DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.Rasterize();

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_CANVAS_VIEW_RASTERIZE_TASK", [&](std::ostringstream& oss)
                                        {
    mEndTimeNanoSceonds = GetNanoseconds();
    oss << std::fixed << std::setprecision(3);
    oss << "[";
    oss << "d:" << static_cast<float>(mEndTimeNanoSceonds - mStartTimeNanoSceonds) / 1000000.0f << "ms ";
    oss << "r:" << rasterized << "]"; });

  return rasterized;
}

Texture CanvasRendererRasterizingTask::GetRasterizedTexture()
{
  return mCanvasRenderer.GetRasterizedTexture();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
