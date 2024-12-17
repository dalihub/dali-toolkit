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

// CLASS HEADER
#include <dali-toolkit/internal/controls/render-effects/offscreen-rendering-context.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

// EXTERNAL INCLUDES
#include <integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
bool OffScreenRenderingContext::IsEnabled()
{
  return mCamera && mRenderer;
}

void OffScreenRenderingContext::Enable(Toolkit::Control control, DevelControl::OffScreenRenderingType type)
{
  if(type == DevelControl::OffScreenRenderingType::NONE)
  {
    return;
  }

  // Create resources
  if(!IsEnabled())
  {
    Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(control);
    if(DALI_UNLIKELY(!sceneHolder))
    {
      DALI_LOG_ERROR("Could not enable offscreen rendering. The control is not connected to a scene.\n");
      return;
    }
    mSceneHolder = sceneHolder;

    const Size size = control.GetProperty<Size>(Actor::Property::SIZE);

    mCamera = CameraActor::New(size);
    mCamera.SetInvertYAxis(true);
    mCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCamera.SetType(Camera::FREE_LOOK);
    mCamera.SetPerspectiveProjection(size);
    control.Add(mCamera);

    mFrameBuffer    = FrameBuffer::New(size.width, size.height, FrameBuffer::Attachment::DEPTH_STENCIL);
    Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, size.width, size.height);
    mFrameBuffer.AttachColorTexture(texture);

    RenderTaskList taskList = sceneHolder.GetRenderTaskList();
    mRenderTask             = taskList.CreateTask();
    mRenderTask.SetSourceActor(control);
    mRenderTask.SetCameraActor(mCamera);
    mRenderTask.SetExclusive(true);
    mRenderTask.SetInputEnabled(false);
    mRenderTask.SetFrameBuffer(mFrameBuffer);
    mRenderTask.SetClearEnabled(true);
    mRenderTask.SetClearColor(sceneHolder.GetBackgroundColor());

    mRenderer = CreateRenderer(SHADER_CONTROL_RENDERERS_VERT, SHADER_CONTROL_RENDERERS_FRAG);
    SetRendererTexture(mRenderer, mFrameBuffer);

    control.GetImplementation().SetCacheRenderer(mRenderer);
    control.GetImplementation().SetOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  }

  // Set refresh rate
  if(type == DevelControl::OffScreenRenderingType::REFRESH_ALWAYS)
  {
    mRenderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
  }
  else if(type == DevelControl::OffScreenRenderingType::REFRESH_ONCE)
  {
    mRenderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  }
}

void OffScreenRenderingContext::Disable(Toolkit::Control control)
{
  if(IsEnabled())
  {
    if(DALI_LIKELY(control))
    {
      control.GetImplementation().RemoveCacheRenderer();

      mCamera.Unparent();
      control.GetImplementation().SetOffScreenRenderableType(OffScreenRenderable::Type::NONE);
    }

    auto sceneHolder = mSceneHolder.GetHandle();
    if(DALI_LIKELY(sceneHolder))
    {
      RenderTaskList taskList = sceneHolder.GetRenderTaskList();
      taskList.RemoveTask(mRenderTask);
    }
    mSceneHolder.Reset();

    mFrameBuffer.Reset();
    mRenderTask.Reset();
    mCamera.Reset();
    mRenderer.Reset();
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
