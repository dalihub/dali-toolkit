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
#include "canvas-view-rasterize-task.h"

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
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
  if(mCanvasRenderer && mCanvasRenderer.Rasterize())
  {
    return true;
  }
  return false;
}

Texture CanvasRendererRasterizingTask::GetRasterizedTexture()
{
  return mCanvasRenderer.GetRasterizedTexture();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
