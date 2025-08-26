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
#include <dali-toolkit/internal/controls/render-effects/offscreen-rendering-impl.h>

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
OffScreenRenderingImpl::OffScreenRenderingImpl(DevelControl::OffScreenRenderingType type)
: mType(type)
{
  Initialize();
}

void OffScreenRenderingImpl::SetType(DevelControl::OffScreenRenderingType type)
{
  mType = type;

  if(mRenderTask)
  {
    if(mType == DevelControl::OffScreenRenderingType::REFRESH_ALWAYS)
    {
      mRenderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    }
    else if(mType == DevelControl::OffScreenRenderingType::REFRESH_ONCE)
    {
      mRenderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    }
  }
}

OffScreenRenderable::Type OffScreenRenderingImpl::GetOffScreenRenderableType()
{
  return OffScreenRenderable::Type::FORWARD;
}

void OffScreenRenderingImpl::GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward)
{
}

Dali::Texture OffScreenRenderingImpl::GetTexture() const
{
  return mTexture;
}

void OffScreenRenderingImpl::OnInitialize()
{
}

void OffScreenRenderingImpl::OnActivate()
{
  if(mType == DevelControl::OffScreenRenderingType::NONE)
  {
    return;
  }

  if(!mCamera)
  {
    mCamera = CameraActor::New();
    mCamera.SetInvertYAxis(true);
    mCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCamera.SetType(Dali::Camera::FREE_LOOK);
  }
  mCamera.SetPerspectiveProjection(GetTargetSize());
  GetOwnerControl().Add(mCamera);

  CreateFrameBuffer();
  CreateRenderTask();
  SetType(mType);

  Renderer renderer = GetTargetRenderer();
  SetRendererTexture(renderer, mFrameBuffer);

  Toolkit::Control control = GetOwnerControl();
  control.AddCacheRenderer(renderer);
  control.GetImplementation().RegisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  mRenderTask.SetScreenToFrameBufferMappingActor(control);
}

void OffScreenRenderingImpl::OnDeactivate()
{
  Toolkit::Control control = GetOwnerControl();
  if(DALI_LIKELY(control))
  {
    Renderer renderer = GetTargetRenderer();
    control.RemoveCacheRenderer(renderer);
    control.GetImplementation().UnregisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);

    mCamera.Unparent();
  }

  DestroyFrameBuffer();
  DestroyRenderTask();
}

void OffScreenRenderingImpl::OnRefresh()
{
  DestroyFrameBuffer();

  mCamera.SetPerspectiveProjection(GetTargetSize());

  CreateFrameBuffer();
  SetRendererTexture(GetTargetRenderer(), mFrameBuffer);
  mRenderTask.SetFrameBuffer(mFrameBuffer);
}

void OffScreenRenderingImpl::CreateFrameBuffer()
{
  const Size size = GetTargetSize();

  mFrameBuffer    = FrameBuffer::New(size.width, size.height, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, size.width, size.height);
  mFrameBuffer.AttachColorTexture(texture);
}

void OffScreenRenderingImpl::DestroyFrameBuffer()
{
  mFrameBuffer.Reset();
}

void OffScreenRenderingImpl::CreateRenderTask()
{
  Toolkit::Control         control     = GetOwnerControl();
  Integration::SceneHolder sceneHolder = GetSceneHolder();
  RenderTaskList           taskList    = sceneHolder.GetRenderTaskList();

  mRenderTask = taskList.CreateTask();
  mRenderTask.SetSourceActor(control);
  mRenderTask.SetCameraActor(mCamera);
  mRenderTask.SetExclusive(true);
  mRenderTask.SetInputEnabled(true);
  mRenderTask.SetFrameBuffer(mFrameBuffer);
  mRenderTask.SetClearEnabled(true);
  mRenderTask.SetClearColor(Color::TRANSPARENT);
  mRenderTask.SetRenderPassTag(GetRenderPassTag());
  mRenderTask.FinishedSignal().Connect(this, &OffScreenRenderingImpl::OnRenderFinished);
}

void OffScreenRenderingImpl::DestroyRenderTask()
{
  auto sceneHolder = GetSceneHolder();
  if(DALI_LIKELY(sceneHolder))
  {
    RenderTaskList taskList = sceneHolder.GetRenderTaskList();
    taskList.RemoveTask(mRenderTask);
  }

  mRenderTask.Reset();
}

void OffScreenRenderingImpl::OnRenderFinished(Dali::RenderTask& task)
{
  mTexture = mFrameBuffer.GetColorTexture();

  GetOwnerControl().OffScreenRenderingFinishedSignal().Emit();
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
