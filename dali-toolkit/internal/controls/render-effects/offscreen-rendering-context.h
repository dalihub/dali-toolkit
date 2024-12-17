#ifndef DALI_TOOLKIT_INTERNAL_OFFSCREEN_RENDERING_CONTEXT
#define DALI_TOOLKIT_INTERNAL_OFFSCREEN_RENDERING_CONTEXT

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

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/renderer.h>
#include <integration-api/adaptor-framework/scene-holder.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class OffScreenRenderingContext
{
public:
  OffScreenRenderingContext() = default;
  void Enable(Toolkit::Control control, DevelControl::OffScreenRenderingType type);
  void Disable(Toolkit::Control control);

private:
  bool IsEnabled();

private:
  RenderTask  mRenderTask;
  CameraActor mCamera;
  FrameBuffer mFrameBuffer;
  Renderer    mRenderer;

  WeakHandle<Integration::SceneHolder> mSceneHolder;
};
} // namespace Internal
} // namespace Toolkit
} // namespace Dali
#endif //DALI_TOOLKIT_INTERNAL_OFFSCREEN_RENDERING_CONTEXT
