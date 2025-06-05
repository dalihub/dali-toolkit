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
#include <dali-toolkit/internal/controls/render-effects/mask-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace
{
const uint32_t        maskSourceIndex            = 0u;
const uint32_t        maskTargetIndex            = 1u;
constexpr const char* UNIFORM_MASK_MODE_NAME     = "uMaskMode";
constexpr const char* UNIFORM_MASK_POSITION_NAME = "uMaskPosition";
constexpr const char* UNIFORM_MASK_SCALE_NAME    = "uMaskScale";
} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
#ifdef DEBUG_ENABLED
extern Debug::Filter* gRenderEffectLogFilter; ///< Define at render-effect-impl.cpp
#endif

MaskEffectImpl::MaskEffectImpl(Toolkit::Control maskControl)
: RenderEffectImpl(),
  mMaskControl(maskControl),
  mMaskMode(MaskEffect::MaskMode::ALPHA),
  mMaskPosition(Vector2(0.f, 0.f)),
  mMaskScale(Vector2(1.f, 1.f))
{
}

MaskEffectImpl::MaskEffectImpl(Toolkit::Control maskControl, MaskEffect::MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale)
: RenderEffectImpl(),
  mMaskControl(maskControl),
  mMaskMode(maskMode),
  mMaskPosition(maskPosition),
  mMaskScale(maskScale)
{
  if(mMaskScale.x < Math::MACHINE_EPSILON_100)
  {
    DALI_LOG_DEBUG_INFO("maskScale.x is less or equal to zero. Adjust to epsilon.\n");
    mMaskScale.x = Math::MACHINE_EPSILON_100;
  }

  if(mMaskScale.y < Math::MACHINE_EPSILON_100)
  {
    DALI_LOG_DEBUG_INFO("maskScale.y is less or equal to zero. Adjust to epsilon.\n");
    mMaskScale.y = Math::MACHINE_EPSILON_100;
  }
}

MaskEffectImpl::~MaskEffectImpl()
{
}

MaskEffectImplPtr MaskEffectImpl::New(Toolkit::Control maskControl)
{
  MaskEffectImplPtr handle = new MaskEffectImpl(maskControl);
  handle->Initialize();
  return handle;
}

MaskEffectImplPtr MaskEffectImpl::New(Toolkit::Control maskControl, MaskEffect::MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale)
{
  MaskEffectImplPtr handle = new MaskEffectImpl(maskControl, maskMode, maskPosition, maskScale);
  handle->Initialize();
  return handle;
}

OffScreenRenderable::Type MaskEffectImpl::GetOffScreenRenderableType()
{
  return OffScreenRenderable::FORWARD;
}

void MaskEffectImpl::GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward)
{
  tasks.clear();
  if(isForward)
  {
    if(mMaskTargetRenderTask)
    {
      tasks.push_back(mMaskTargetRenderTask);
    }
    if(mMaskSourceRenderTask)
    {
      tasks.push_back(mMaskSourceRenderTask);
    }
  }
}

void MaskEffectImpl::OnInitialize()
{
  // Create CameraActors
  mCamera = CameraActor::New();
  mCamera.SetInvertYAxis(true);
  mCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

  // renderer
  Renderer maskRenderer = GetTargetRenderer();
  Shader   shader       = Dali::Shader::New(BASIC_VERTEX_SOURCE, SHADER_MASK_EFFECT_FRAG);
  maskRenderer.SetShader(shader);
  maskRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha
}

void MaskEffectImpl::OnActivate()
{
  Toolkit::Control ownerControl = GetOwnerControl();
  DALI_ASSERT_ALWAYS(ownerControl && "Set the owner of RenderEffect before you activate.");

  ownerControl.Add(mCamera);
  Renderer maskRenderer = GetTargetRenderer();
  ownerControl.GetImplementation().SetCacheRenderer(maskRenderer);
  ownerControl.GetImplementation().RegisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);

  Vector2 size = GetTargetSize();
  mCamera.SetPerspectiveProjection(size);

  CreateFrameBuffers(ImageDimensions(size.x, size.y));
  CreateRenderTasks(ownerControl);
  SetShaderConstants(ownerControl);

  mMaskTargetRenderTask.SetScreenToFrameBufferMappingActor(ownerControl);

  TextureSet textureSet = TextureSet::New();

  Texture maskSourceTexture = mMaskSourceFrameBuffer.GetColorTexture();
  Texture maskTargetTexture = mMaskTargetFrameBuffer.GetColorTexture();

  textureSet.SetTexture(maskSourceIndex, maskSourceTexture);
  textureSet.SetTexture(maskTargetIndex, maskTargetTexture);

  maskRenderer.SetTextures(textureSet);
}

void MaskEffectImpl::OnDeactivate()
{
  Toolkit::Control control = GetOwnerControl();
  if(DALI_LIKELY(control))
  {
    control.GetImplementation().RemoveCacheRenderer();
    control.GetImplementation().UnregisterOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
  }

  mCamera.Unparent();
  mMaskTargetFrameBuffer.Reset();
  mMaskSourceFrameBuffer.Reset();

  auto sceneHolder = GetSceneHolder();
  if(DALI_LIKELY(sceneHolder))
  {
    RenderTaskList taskList = sceneHolder.GetRenderTaskList();
    taskList.RemoveTask(mMaskTargetRenderTask);
    mMaskTargetRenderTask.Reset();
    taskList.RemoveTask(mMaskSourceRenderTask);
    mMaskSourceRenderTask.Reset();
  }
}

void MaskEffectImpl::OnRefresh()
{
  mMaskTargetFrameBuffer.Reset();
  mMaskSourceFrameBuffer.Reset();

  Vector2 size = GetTargetSize();
  mCamera.SetPerspectiveProjection(size);
  CreateFrameBuffers(ImageDimensions(size.x, size.y));

  mMaskTargetRenderTask.SetFrameBuffer(mMaskTargetFrameBuffer);
  mMaskSourceRenderTask.SetFrameBuffer(mMaskSourceFrameBuffer);

  TextureSet textureSet = TextureSet::New();

  Texture maskSourceTexture = mMaskSourceFrameBuffer.GetColorTexture();
  Texture maskTargetTexture = mMaskTargetFrameBuffer.GetColorTexture();

  textureSet.SetTexture(maskSourceIndex, maskSourceTexture);
  textureSet.SetTexture(maskTargetIndex, maskTargetTexture);

  GetTargetRenderer().SetTextures(textureSet);
}

void MaskEffectImpl::CreateFrameBuffers(const ImageDimensions size)
{
  uint32_t width  = size.GetWidth();
  uint32_t height = size.GetHeight();

  mMaskTargetFrameBuffer     = FrameBuffer::New(width, height, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture mMaskTargetTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, width, height);
  mMaskTargetFrameBuffer.AttachColorTexture(mMaskTargetTexture);

  mMaskSourceFrameBuffer     = FrameBuffer::New(width, height, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture mMaskSourceTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, width, height);
  mMaskSourceFrameBuffer.AttachColorTexture(mMaskSourceTexture);
}

void MaskEffectImpl::CreateRenderTasks(Toolkit::Control ownerControl)
{
  RenderTaskList taskList = GetSceneHolder().GetRenderTaskList();

  mMaskTargetRenderTask = taskList.CreateTask();
  mMaskTargetRenderTask.SetCameraActor(mCamera);
  mMaskTargetRenderTask.SetExclusive(true);
  mMaskTargetRenderTask.SetInputEnabled(true);
  mMaskTargetRenderTask.SetSourceActor(ownerControl);
  mMaskTargetRenderTask.SetFrameBuffer(mMaskTargetFrameBuffer);
  mMaskTargetRenderTask.SetClearEnabled(true);
  mMaskTargetRenderTask.SetClearColor(Color::TRANSPARENT);

  mMaskSourceRenderTask = taskList.CreateTask();
  mMaskSourceRenderTask.SetCameraActor(mCamera);
  mMaskSourceRenderTask.SetExclusive(true);
  mMaskSourceRenderTask.SetInputEnabled(false);
  mMaskSourceRenderTask.SetSourceActor(mMaskControl.GetHandle());
  mMaskSourceRenderTask.SetFrameBuffer(mMaskSourceFrameBuffer);
  mMaskSourceRenderTask.SetClearEnabled(true);
  mMaskSourceRenderTask.SetClearColor(Color::TRANSPARENT);
}

void MaskEffectImpl::SetShaderConstants(Toolkit::Control ownerControl)
{
  ownerControl.RegisterProperty(UNIFORM_MASK_MODE_NAME, static_cast<float>(mMaskMode));

  Vector2 newMaskPosition;
  newMaskPosition.x = -mMaskPosition.x;
  newMaskPosition.y = -mMaskPosition.y;
  ownerControl.RegisterProperty(UNIFORM_MASK_POSITION_NAME, newMaskPosition);

  Vector2 newMaskScale;
  newMaskScale.x = 1.0f / std::max(Math::MACHINE_EPSILON_100, mMaskScale.x);
  newMaskScale.y = 1.0f / std::max(Math::MACHINE_EPSILON_100, mMaskScale.y);

  ownerControl.RegisterProperty(UNIFORM_MASK_SCALE_NAME, newMaskScale);
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
