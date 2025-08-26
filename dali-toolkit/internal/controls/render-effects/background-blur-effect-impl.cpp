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
#include <dali-toolkit/internal/controls/render-effects/background-blur-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace
{
// Default values
static constexpr float    BLUR_EFFECT_DOWNSCALE_FACTOR = 0.25f;
static constexpr uint32_t BLUR_EFFECT_BLUR_RADIUS      = 40u;

static constexpr uint32_t MINIMUM_GPU_ARRAY_SIZE = 2u; // GPU cannot handle array size smaller than 2.

static constexpr std::string_view UNIFORM_BLUR_STRENGTH_NAME("uAnimationRatio");
static constexpr std::string_view UNIFORM_BLUR_OPACITY_NAME("uOpacity");
static constexpr std::string_view UNIFORM_BLUR_OFFSET_DIRECTION_NAME("uOffsetDirection");
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

BackgroundBlurEffectImpl::BackgroundBlurEffectImpl()
: BackgroundBlurEffectImpl(BLUR_EFFECT_BLUR_RADIUS)
{
}

BackgroundBlurEffectImpl::BackgroundBlurEffectImpl(uint32_t blurRadius)
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(BLUR_EFFECT_DOWNSCALE_FACTOR),
  mBlurRadius(blurRadius),
  mInternalDownscaleFactor(mDownscaleFactor),
  mInternalBlurRadius(mBlurRadius),
  mSkipBlur(false),
  mBlurOnce(false)
{
  UpdateDownscaledBlurRadius();
}

BackgroundBlurEffectImpl::~BackgroundBlurEffectImpl()
{
}

BackgroundBlurEffectImplPtr BackgroundBlurEffectImpl::New()
{
  return BackgroundBlurEffectImpl::New(BLUR_EFFECT_BLUR_RADIUS);
}

BackgroundBlurEffectImplPtr BackgroundBlurEffectImpl::New(uint32_t blurRadius)
{
  BackgroundBlurEffectImplPtr handle = new BackgroundBlurEffectImpl(blurRadius);
  handle->Initialize();
  return handle;
}

OffScreenRenderable::Type BackgroundBlurEffectImpl::GetOffScreenRenderableType()
{
  return mSkipBlur ? OffScreenRenderable::NONE : OffScreenRenderable::BACKWARD;
}

void BackgroundBlurEffectImpl::GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward)
{
  tasks.clear();
  if(!isForward)
  {
    if(mSourceRenderTask)
    {
      // Re-initialize source actor of rendertask since it might be changed.
      // TODO : Should it be required always? Couldn't we skip it?
      ApplyRenderTaskSourceActor(mSourceRenderTask, GetOwnerControl());
      tasks.push_back(mSourceRenderTask);
    }
    if(mHorizontalBlurTask)
    {
      tasks.push_back(mHorizontalBlurTask);
    }
    if(mVerticalBlurTask)
    {
      tasks.push_back(mVerticalBlurTask);
    }
  }
}

void BackgroundBlurEffectImpl::SetBlurOnce(bool blurOnce)
{
  mBlurOnce = blurOnce;

  if(!mSkipBlur && IsActivated())
  {
    if(!mSourceRenderTask)
    {
      OnRefresh();
    }

    if(mBlurOnce)
    {
      mSourceRenderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
      mHorizontalBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
      mVerticalBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

      mVerticalBlurTask.FinishedSignal().Connect(this, &BackgroundBlurEffectImpl::OnRenderFinished);
    }
    else
    {
      mSourceRenderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
      mHorizontalBlurTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
      mVerticalBlurTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    }
  }
}

bool BackgroundBlurEffectImpl::GetBlurOnce() const
{
  return mBlurOnce;
}

void BackgroundBlurEffectImpl::SetBlurRadius(uint32_t blurRadius)
{
  if(mBlurRadius != blurRadius)
  {
    if(!mSkipBlur && IsActivated())
    {
      OnDeactivate();
    }

    mBlurRadius = blurRadius;
    UpdateDownscaledBlurRadius();
    if(DALI_UNLIKELY(mSkipBlur))
    {
      return;
    }

    Renderer horizontalBlurRenderer = GaussianBlurAlgorithm::CreateRenderer(mDownscaledBlurRadius);
    mHorizontalBlurActor.RemoveRenderer(0u);
    mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);

    Renderer verticalBlurRenderer = GaussianBlurAlgorithm::CreateRenderer(mDownscaledBlurRadius);
    mVerticalBlurActor.RemoveRenderer(0u);
    mVerticalBlurActor.AddRenderer(verticalBlurRenderer);

    if(!mSkipBlur && IsActivated())
    {
      OnActivate();
    }
  }
}

uint32_t BackgroundBlurEffectImpl::GetBlurRadius() const
{
  return mBlurRadius;
}

void BackgroundBlurEffectImpl::SetBlurDownscaleFactor(float downscaleFactor)
{
  if(!Dali::Equals(mDownscaleFactor, downscaleFactor))
  {
    if(!mSkipBlur && IsActivated())
    {
      OnDeactivate();
    }

    mDownscaleFactor = downscaleFactor;
    UpdateDownscaledBlurRadius();
    if(DALI_UNLIKELY(mSkipBlur))
    {
      return;
    }

    Renderer horizontalBlurRenderer = GaussianBlurAlgorithm::CreateRenderer(mDownscaledBlurRadius);
    mHorizontalBlurActor.RemoveRenderer(0u);
    mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);

    Renderer verticalBlurRenderer = GaussianBlurAlgorithm::CreateRenderer(mDownscaledBlurRadius);
    mVerticalBlurActor.RemoveRenderer(0u);
    mVerticalBlurActor.AddRenderer(verticalBlurRenderer);

    if(!mSkipBlur && IsActivated())
    {
      OnActivate();
    }
  }
}

float BackgroundBlurEffectImpl::GetBlurDownscaleFactor() const
{
  return mDownscaleFactor;
}

void BackgroundBlurEffectImpl::AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    DALI_LOG_ERROR("Blur radius is too small. Blur animation will be ignored.\n");
    return;
  }

  if(mBlurOnce)
  {
    DALI_LOG_ERROR("This blur effect is set to render only once, so the animation will be ignored. Call SetBlurOnce(false) to render it every frame.\n");
    return;
  }

  fromValue = std::clamp(fromValue, 0.0f, 1.0f);
  toValue   = std::clamp(toValue, 0.0f, 1.0f);

  if(fromValue > toValue)
  {
    DALI_LOG_ERROR("Removing blur may require blur downscale factor updates for visual quality.\n");
  }

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, fromValue, AlphaFunction::BuiltinFunction::LINEAR);
  keyFrames.Add(1.0f, toValue, AlphaFunction::BuiltinFunction::LINEAR);

  Property::Index horizontalAnimationIndex = mHorizontalBlurActor.GetPropertyIndex(UNIFORM_BLUR_STRENGTH_NAME.data());
  animation.AnimateBetween(Property(mHorizontalBlurActor, horizontalAnimationIndex), keyFrames, alphaFunction, timePeriod);
  Property::Index verticalAnimationIndex = mVerticalBlurActor.GetPropertyIndex(UNIFORM_BLUR_STRENGTH_NAME.data());
  animation.AnimateBetween(Property(mVerticalBlurActor, verticalAnimationIndex), keyFrames, alphaFunction, timePeriod);
}

void BackgroundBlurEffectImpl::AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    DALI_LOG_ERROR("Blur radius is too small. Blur animation will be ignored.");
    return;
  }
  if(mBlurOnce)
  {
    DALI_LOG_ERROR("This blur effect is set to render only once, so the animation will be ignored. Call SetBlurOnce(false) to render it every frame.");
    return;
  }

  fromValue = std::clamp(fromValue, 0.0f, 1.0f);
  toValue   = std::clamp(toValue, 0.0f, 1.0f);

  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, fromValue, AlphaFunction::BuiltinFunction::LINEAR);
  keyFrames.Add(1.0f, toValue, AlphaFunction::BuiltinFunction::LINEAR);

  Property::Index horizontalAnimationIndex = mHorizontalBlurActor.GetPropertyIndex(UNIFORM_BLUR_OPACITY_NAME.data());
  animation.AnimateBetween(Property(mHorizontalBlurActor, horizontalAnimationIndex), keyFrames, alphaFunction, timePeriod);
  Property::Index verticalAnimationIndex = mVerticalBlurActor.GetPropertyIndex(UNIFORM_BLUR_OPACITY_NAME.data());
  animation.AnimateBetween(Property(mVerticalBlurActor, verticalAnimationIndex), keyFrames, alphaFunction, timePeriod);
}

void BackgroundBlurEffectImpl::OnInitialize()
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    return;
  }

  // Create blur actors
  {
    mInternalRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

    // Create an actor for performing a vertical blur on the texture
    Renderer horizontalBlurRenderer = GaussianBlurAlgorithm::CreateRenderer(mDownscaledBlurRadius);
    mHorizontalBlurActor            = Actor::New();
    mHorizontalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);

    mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
    mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);

    mInternalRoot.Add(mHorizontalBlurActor);

    // Create an actor for performing a vertical blur on the texture
    Renderer verticalBlurRenderer = GaussianBlurAlgorithm::CreateRenderer(mDownscaledBlurRadius);
    mVerticalBlurActor            = Actor::New();
    mVerticalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mVerticalBlurActor.AddRenderer(verticalBlurRenderer);

    mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
    mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);

    mInternalRoot.Add(mVerticalBlurActor);
  }
}

void BackgroundBlurEffectImpl::OnActivate()
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    return;
  }

  Toolkit::Control ownerControl = GetOwnerControl();
  DALI_ASSERT_ALWAYS(ownerControl && "Set the owner of RenderEffect before you activate.");

  // Reset animation properties
  mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
  mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);
  mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
  mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);

  // Get size
  Vector2 size = GetTargetSize();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[BackgroundBlurEffect:%p] OnActivated! [ID:%d][size:%fx%f] [radius:%u, scale:%f, downscaledRadius:%u=%u*%f]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1, size.x, size.y, mBlurRadius, mDownscaleFactor, mDownscaledBlurRadius, mInternalBlurRadius, mInternalDownscaleFactor);

  uint32_t downsampledWidth  = std::max(static_cast<uint32_t>(size.width * mInternalDownscaleFactor), 1u);
  uint32_t downsampledHeight = std::max(static_cast<uint32_t>(size.height * mInternalDownscaleFactor), 1u);

  // Set size
  if(!mCamera)
  {
    mCamera = CameraActor::New();
    mCamera.SetInvertYAxis(true);
    mCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCamera.SetType(Dali::Camera::FREE_LOOK);
    mInternalRoot.Add(mCamera);
  }
  mCamera.SetPerspectiveProjection(GetTargetSize());

  if(!mRenderDownsampledCamera)
  {
    mRenderDownsampledCamera = CameraActor::New();
    mRenderDownsampledCamera.SetInvertYAxis(true);
    mRenderDownsampledCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mRenderDownsampledCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK);
    mInternalRoot.Add(mRenderDownsampledCamera);
  }
  mRenderDownsampledCamera.SetPerspectiveProjection(Size(downsampledWidth, downsampledHeight));

  mHorizontalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));
  mVerticalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));

  // Set blur
  CreateFrameBuffers(ImageDimensions(downsampledWidth, downsampledHeight));
  CreateRenderTasks(GetSceneHolder(), ownerControl);

  // Reset shader constants
  auto& blurShader = GaussianBlurAlgorithm::GetGaussianBlurShader(mDownscaledBlurRadius);
  {
    Renderer renderer = mHorizontalBlurActor.GetRendererAt(0u);
    renderer.SetShader(blurShader);
    renderer.RegisterProperty(UNIFORM_BLUR_OFFSET_DIRECTION_NAME.data(), Vector2(1.0f / downsampledWidth, 0.0f));
  }
  {
    Renderer renderer = mVerticalBlurActor.GetRendererAt(0u);
    renderer.SetShader(blurShader);
    renderer.RegisterProperty(UNIFORM_BLUR_OFFSET_DIRECTION_NAME.data(), Vector2(0.0f, 1.0f / downsampledHeight));
  }

  // Inject blurred output to control
  Renderer renderer = GetTargetRenderer();
  renderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Dali::Toolkit::DepthIndex::BACKGROUND_EFFECT);
  ownerControl.AddRenderer(renderer);
  SetRendererTexture(renderer, mBlurredOutputFrameBuffer);

  ownerControl.Add(mInternalRoot);

  // Reorder render task
  // TODO : Can we remove this GetImplementation?
  GetImplementation(ownerControl).RequestRenderTaskReorder();
}

void BackgroundBlurEffectImpl::OnDeactivate()
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    return;
  }

  auto ownerControl = GetOwnerControl();
  if(DALI_LIKELY(ownerControl))
  {
    Renderer renderer = GetTargetRenderer();
    ownerControl.RemoveRenderer(renderer);
  }
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[BackgroundBlurEffect:%p] OnDeactivated! [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

  mInternalRoot.Unparent();

  DestroyFrameBuffers();
  DestroyRenderTasks();
}

void BackgroundBlurEffectImpl::OnRefresh()
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    return;
  }

  // Reset animation properties
  mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
  mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);
  mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
  mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);

  DestroyFrameBuffers();

  Vector2  size              = GetTargetSize();
  uint32_t downsampledWidth  = std::max(static_cast<uint32_t>(size.width * mInternalDownscaleFactor), 1u);
  uint32_t downsampledHeight = std::max(static_cast<uint32_t>(size.height * mInternalDownscaleFactor), 1u);

  // Set size
  mCamera.SetPerspectiveProjection(size);
  mRenderDownsampledCamera.SetPerspectiveProjection(Size(downsampledWidth, downsampledHeight));
  mHorizontalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));
  mVerticalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));

  // Reset buffers and renderers
  CreateFrameBuffers(ImageDimensions(downsampledWidth, downsampledHeight));

  if(!mSourceRenderTask)
  {
    Toolkit::Control ownerControl = GetOwnerControl();
    ownerControl.Add(mInternalRoot);
    CreateRenderTasks(GetSceneHolder(), ownerControl);
    GetImplementation(ownerControl).RequestRenderTaskReorder();
  }
  else
  {
    mSourceRenderTask.SetFrameBuffer(mInputBackgroundFrameBuffer);
    mHorizontalBlurTask.SetFrameBuffer(mTemporaryFrameBuffer);
    mVerticalBlurTask.SetFrameBuffer(mBlurredOutputFrameBuffer);
  }

  {
    Renderer renderer = mHorizontalBlurActor.GetRendererAt(0);
    SetRendererTexture(renderer, mInputBackgroundFrameBuffer);
    renderer.RegisterProperty(UNIFORM_BLUR_OFFSET_DIRECTION_NAME.data(), Vector2(1.0f / downsampledWidth, 0.0f));
  }
  {
    Renderer renderer = mVerticalBlurActor.GetRendererAt(0);
    SetRendererTexture(renderer, mTemporaryFrameBuffer);
    renderer.RegisterProperty(UNIFORM_BLUR_OFFSET_DIRECTION_NAME.data(), Vector2(0.0f, 1.0f / downsampledHeight));
  }

  SetRendererTexture(GetTargetRenderer(), mBlurredOutputFrameBuffer);
}

void BackgroundBlurEffectImpl::CreateFrameBuffers(const ImageDimensions downsampledSize)
{
  uint32_t downsampledWidth  = downsampledSize.GetWidth();
  uint32_t downsampledHeight = downsampledSize.GetHeight();

  // buffer to draw input texture
  mInputBackgroundFrameBuffer    = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture inputBackgroundTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mInputBackgroundFrameBuffer.AttachColorTexture(inputBackgroundTexture);

  // buffer to draw half-blurred output
  mTemporaryFrameBuffer    = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture temporaryTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mTemporaryFrameBuffer.AttachColorTexture(temporaryTexture);

  // buffer to draw blurred output
  mBlurredOutputFrameBuffer = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture sourceTexture     = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mBlurredOutputFrameBuffer.AttachColorTexture(sourceTexture);
}

void BackgroundBlurEffectImpl::DestroyFrameBuffers()
{
  mInputBackgroundFrameBuffer.Reset();
  mTemporaryFrameBuffer.Reset();
  mBlurredOutputFrameBuffer.Reset();
}

void BackgroundBlurEffectImpl::CreateRenderTasks(Integration::SceneHolder sceneHolder, const Toolkit::Control sourceControl)
{
  RenderTaskList taskList = sceneHolder.GetRenderTaskList();

  // draw input texture
  mSourceRenderTask = taskList.CreateTask();
  mSourceRenderTask.SetCameraActor(mCamera);
  mSourceRenderTask.SetFrameBuffer(mInputBackgroundFrameBuffer);
  mSourceRenderTask.SetInputEnabled(false);
  mSourceRenderTask.SetRenderPassTag(GetRenderPassTag());

  ApplyRenderTaskSourceActor(mSourceRenderTask, sourceControl);

  // Clear inputBackgroundTexture as scene holder background.
  mSourceRenderTask.SetClearEnabled(true);
  mSourceRenderTask.SetClearColor(sceneHolder.GetBackgroundColor());

  // draw half-blurred output
  SetRendererTexture(mHorizontalBlurActor.GetRendererAt(0), mInputBackgroundFrameBuffer);
  mHorizontalBlurTask = taskList.CreateTask();
  mHorizontalBlurTask.SetSourceActor(mHorizontalBlurActor);
  mHorizontalBlurTask.SetExclusive(true);
  mHorizontalBlurTask.SetInputEnabled(false);
  mHorizontalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mHorizontalBlurTask.SetFrameBuffer(mTemporaryFrameBuffer);

  // Clear temporaryTexture as Transparent.
  mHorizontalBlurTask.SetClearEnabled(true);
  mHorizontalBlurTask.SetClearColor(Color::TRANSPARENT);

  // draw blurred output
  SetRendererTexture(mVerticalBlurActor.GetRendererAt(0), mTemporaryFrameBuffer);
  mVerticalBlurTask = taskList.CreateTask();
  mVerticalBlurTask.SetSourceActor(mVerticalBlurActor);
  mVerticalBlurTask.SetExclusive(true);
  mVerticalBlurTask.SetInputEnabled(false);
  mVerticalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mVerticalBlurTask.SetFrameBuffer(mBlurredOutputFrameBuffer);

  // Clear sourceTexture as Transparent.
  mVerticalBlurTask.SetClearEnabled(true);
  mVerticalBlurTask.SetClearColor(Color::TRANSPARENT);

  // Adjust refresh rate
  if(mBlurOnce)
  {
    mSourceRenderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    mHorizontalBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    mVerticalBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);

    mVerticalBlurTask.FinishedSignal().Connect(this, &BackgroundBlurEffectImpl::OnRenderFinished);
  }
  else
  {
    mSourceRenderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    mHorizontalBlurTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    mVerticalBlurTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
  }
}

void BackgroundBlurEffectImpl::DestroyRenderTasks()
{
  auto sceneHolder = GetSceneHolder();
  if(DALI_LIKELY(sceneHolder))
  {
    RenderTaskList taskList = sceneHolder.GetRenderTaskList();
    taskList.RemoveTask(mHorizontalBlurTask);
    taskList.RemoveTask(mVerticalBlurTask);
    taskList.RemoveTask(mSourceRenderTask);
  }

  mHorizontalBlurTask.Reset();
  mVerticalBlurTask.Reset();
  mSourceRenderTask.Reset();
}

void BackgroundBlurEffectImpl::OnRenderFinished(Dali::RenderTask& renderTask)
{
  mFinishedSignal.Emit();

  DestroyFrameBuffers();
  DestroyRenderTasks();
  mInternalRoot.Unparent();
}

void BackgroundBlurEffectImpl::UpdateDownscaledBlurRadius()
{
  mInternalDownscaleFactor = mDownscaleFactor;
  mInternalBlurRadius      = mBlurRadius;
  mDownscaledBlurRadius    = GaussianBlurAlgorithm::GetDownscaledBlurRadius(mInternalDownscaleFactor, mInternalBlurRadius);

  mSkipBlur = false;
  if(DALI_UNLIKELY((mDownscaledBlurRadius >> 1) < MINIMUM_GPU_ARRAY_SIZE))
  {
    if(mInternalBlurRadius == 0u)
    {
      mSkipBlur = true;
      DALI_LOG_ERROR("Zero blur radius. This blur will be ignored.\n");
    }
    else
    {
      mDownscaledBlurRadius = MINIMUM_GPU_ARRAY_SIZE * 2;
      DALI_LOG_ERROR("Blur radius is too small. This blur will use minimum radius value.\n");
    }
  }
}

void BackgroundBlurEffectImpl::ApplyRenderTaskSourceActor(RenderTask sourceRenderTask, const Toolkit::Control sourceControl)
{
  if(DALI_UNLIKELY(!sourceRenderTask || !sourceControl))
  {
    return;
  }

  bool        isExclusiveRequired = false;
  bool        useUserSourceActor  = false;
  Dali::Actor userSourceActor     = mUserSourceActor.GetHandle();
  Dali::Actor sourceActor         = sourceControl;
  Dali::Actor stopperActor        = mUserStopperActor.GetHandle() ? mUserStopperActor.GetHandle() : Dali::Actor::DownCast(sourceControl);

  while(sourceActor && sourceActor.GetParent())
  {
    sourceActor = sourceActor.GetParent();

    if(userSourceActor == sourceActor)
    {
      useUserSourceActor = true;
    }

    Toolkit::Control control = Toolkit::Control::DownCast(sourceActor);
    if(control && (GetImplementation(control).GetOffScreenRenderableType() & OffScreenRenderable::Type::FORWARD) == OffScreenRenderable::Type::FORWARD)
    {
      sourceActor         = GetImplementation(control).GetOffScreenRenderableSourceActor();
      isExclusiveRequired = GetImplementation(control).IsOffScreenRenderTaskExclusive();
      break;
    }
  }

  // Use user defined source actor only if it is parent of sourceControl.
  if(useUserSourceActor)
  {
    sourceActor = userSourceActor;
  }

  sourceRenderTask.SetExclusive(isExclusiveRequired);
  sourceRenderTask.SetSourceActor(sourceActor);
  sourceRenderTask.RenderUntil(stopperActor);
}

Dali::Toolkit::BackgroundBlurEffect::FinishedSignalType& BackgroundBlurEffectImpl::FinishedSignal()
{
  return mFinishedSignal;
}

void BackgroundBlurEffectImpl::SetSourceActor(Dali::Actor sourceActor)
{
  mUserSourceActor = sourceActor;

  if(mSourceRenderTask)
  {
    // Re-initialize source actor of rendertask
    Toolkit::Control ownerControl = GetOwnerControl();
    ApplyRenderTaskSourceActor(mSourceRenderTask, ownerControl);
  }
}

void BackgroundBlurEffectImpl::SetStopperActor(Dali::Actor stopperActor)
{
  mUserStopperActor = stopperActor;

  if(mSourceRenderTask)
  {
    // Re-initialize stopper actor of rendertask
    Toolkit::Control ownerControl = GetOwnerControl();
    ApplyRenderTaskSourceActor(mSourceRenderTask, ownerControl);
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
