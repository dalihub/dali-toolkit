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
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace
{
// Default values
static constexpr float    BLUR_EFFECT_DOWNSCALE_FACTOR = 0.25f;
static constexpr uint32_t BLUR_EFFECT_BLUR_RADIUS      = 10u;

static constexpr float MINIMUM_DOWNSCALE_FACTOR = 0.1f;
static constexpr float MAXIMUM_DOWNSCALE_FACTOR = 1.0f;

static constexpr uint32_t MINIMUM_GPU_ARRAY_SIZE = 2u;   // GPU cannot handle array size smaller than 2.
static constexpr uint32_t MAXIMUM_BLUR_RADIUS    = 500u; ///< Maximum pixel radius for blur effect. (GL_MAX_FRAGMENT_UNIFORM_COMPONENTS(Usually 1024) - 19 (vertex shader used)) / 3 float

static constexpr float   MAXIMUM_BELL_CURVE_WIDTH            = 171.352f; ///< bell curve width for MAXIMUM_BLUR_RADIUS case
static constexpr int32_t MAXIMUM_BELL_CURVE_LOOP_TRIAL_COUNT = 50;

static constexpr std::string_view UNIFORM_BLUR_STRENGTH_NAME("uAnimationRatio");
static constexpr std::string_view UNIFORM_BLUR_OPACITY_NAME("uOpacity");

/**
  * @brief Calculates gaussian weight
  * @param[in] localOffset Input variable of gaussian distribution
  * @param[in] sigma Standard deviation of gaussian distribution, the width of the "bell"
  * @note Expected value of this gaussian distribution is 0.
  */
inline float CalculateGaussianWeight(float localOffset, float sigma)
{
  return (1.0f / (sigma * sqrt(2.0f * Dali::Math::PI))) * exp(-0.5f * (localOffset / sigma * localOffset / sigma));
}

/**
 * @brief
 * @return Downscaled(optimized) blur radius
 */
uint32_t FitBlurRadiusToValidRange(float& downscaleFactor, uint32_t& blurRadius)
{
  downscaleFactor = BLUR_EFFECT_DOWNSCALE_FACTOR;

  if(DALI_UNLIKELY(blurRadius > MAXIMUM_BLUR_RADIUS))
  {
    const uint32_t fixedBlurRadius      = MAXIMUM_BLUR_RADIUS;
    const float    fixedDownScaleFactor = Dali::Clamp(
      downscaleFactor * static_cast<float>(fixedBlurRadius) / static_cast<float>(blurRadius),
      MINIMUM_DOWNSCALE_FACTOR,
      MAXIMUM_DOWNSCALE_FACTOR);

    DALI_LOG_ERROR("Blur radius is out of bound: %u. Use %u and make downscale factor %f to %f.\n",
                   blurRadius,
                   fixedBlurRadius,
                   downscaleFactor,
                   fixedDownScaleFactor);

    downscaleFactor = fixedDownScaleFactor;
    blurRadius      = fixedBlurRadius;
  }
  return static_cast<uint32_t>(blurRadius * downscaleFactor);
}
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
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(BLUR_EFFECT_DOWNSCALE_FACTOR),
  mBlurRadius(BLUR_EFFECT_BLUR_RADIUS),
  mDownscaledBlurRadius(static_cast<uint32_t>(BLUR_EFFECT_BLUR_RADIUS * BLUR_EFFECT_DOWNSCALE_FACTOR)),
  mBellCurveWidth(Math::MACHINE_EPSILON_1),
  mSkipBlur(false),
  mBlurOnce(false)
{
}

BackgroundBlurEffectImpl::BackgroundBlurEffectImpl(uint32_t blurRadius)
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(BLUR_EFFECT_DOWNSCALE_FACTOR),
  mBlurRadius(blurRadius),
  mDownscaledBlurRadius(BLUR_EFFECT_BLUR_RADIUS),
  mBellCurveWidth(Math::MACHINE_EPSILON_1),
  mSkipBlur(false),
  mBlurOnce(false)
{
  mDownscaledBlurRadius = FitBlurRadiusToValidRange(mDownscaleFactor, mBlurRadius);
  if(DALI_UNLIKELY((mDownscaledBlurRadius >> 1) < MINIMUM_GPU_ARRAY_SIZE))
  {
    mSkipBlur = true;
    DALI_LOG_ERROR("Blur radius is too small. This blur will be ignored.\n");
  }
}

BackgroundBlurEffectImpl::~BackgroundBlurEffectImpl()
{
}

BackgroundBlurEffectImplPtr BackgroundBlurEffectImpl::New()
{
  BackgroundBlurEffectImplPtr handle = new BackgroundBlurEffectImpl();
  handle->Initialize();
  return handle;
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

  if(IsActivated()) // if false, no render task exists yet(nothing to do)
  {
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
    if(IsActivated())
    {
      OnDeactivate();
    }

    // Reinitialize blur parameters
    mBlurRadius           = blurRadius;
    mDownscaledBlurRadius = FitBlurRadiusToValidRange(mDownscaleFactor, mBlurRadius);

    mSkipBlur = false;
    if(DALI_UNLIKELY((mDownscaledBlurRadius >> 1) < MINIMUM_GPU_ARRAY_SIZE))
    {
      mSkipBlur = true;
      DALI_LOG_ERROR("Blur radius is too small. This blur will be ignored.\n");
    }

    OnInitialize();

    if(IsActivated())
    {
      OnActivate();
    }
  }
}

uint32_t BackgroundBlurEffectImpl::GetBlurRadius() const
{
  return mBlurRadius;
}

void BackgroundBlurEffectImpl::AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
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

  Property::Index horizontalAnimationIndex = mHorizontalBlurActor.GetPropertyIndex(UNIFORM_BLUR_STRENGTH_NAME.data());
  animation.AnimateBetween(Property(mHorizontalBlurActor, horizontalAnimationIndex), keyFrames, alphaFunction, timePeriod);
  Property::Index verticalAnimationIndex = mVerticalBlurActor.GetPropertyIndex(UNIFORM_BLUR_STRENGTH_NAME.data());
  animation.AnimateBetween(Property(mVerticalBlurActor, verticalAnimationIndex), keyFrames, alphaFunction, timePeriod);
}

void BackgroundBlurEffectImpl::AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
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

  // Calculate bell curve width
  {
    const float epsilon     = 1e-2f / (mDownscaledBlurRadius * 2);
    const float localOffset = (mDownscaledBlurRadius * 2) - 1;

    float lowerBoundBellCurveWidth = Dali::Math::MACHINE_EPSILON_10000;
    float upperBoundBellCurveWidth = MAXIMUM_BELL_CURVE_WIDTH;

    int trialCount = 0;
    while(trialCount++ < MAXIMUM_BELL_CURVE_LOOP_TRIAL_COUNT && upperBoundBellCurveWidth - lowerBoundBellCurveWidth > Dali::Math::MACHINE_EPSILON_10000)
    {
      mBellCurveWidth = (lowerBoundBellCurveWidth + upperBoundBellCurveWidth) * 0.5f;
      if(CalculateGaussianWeight(localOffset, mBellCurveWidth) < epsilon)
      {
        lowerBoundBellCurveWidth = mBellCurveWidth;
      }
      else
      {
        upperBoundBellCurveWidth = mBellCurveWidth;
      }
    }
  }

  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[BlurEffect:%p] mBellCurveWidth calculated! [mBlurRadius:%u][mBellCurveWidth:%f]\n", this, mBlurRadius, mBellCurveWidth);

  // Create blur actors
  {
    mInternalRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

    // shader
    std::ostringstream fragmentStringStream;
    fragmentStringStream << "#define NUM_SAMPLES " << (mDownscaledBlurRadius >> 1) << "\n";
    fragmentStringStream << SHADER_BLUR_EFFECT_FRAG;
    std::string fragmentSource(fragmentStringStream.str());

    // Create an actor for performing a horizontal blur on the texture
    Renderer horizontalBlurRenderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
    horizontalBlurRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha

    if(mHorizontalBlurActor)
    {
      mHorizontalBlurActor.RemoveRenderer(0u);
      mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);
    }
    else
    {
      mHorizontalBlurActor = Actor::New();
      mHorizontalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);
      mInternalRoot.Add(mHorizontalBlurActor);

      mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
      mHorizontalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);
    }

    // Create an actor for performing a vertical blur on the texture
    Renderer verticalBlurRenderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
    verticalBlurRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha

    if(mVerticalBlurActor)
    {
      mVerticalBlurActor.RemoveRenderer(0u);
      mVerticalBlurActor.AddRenderer(verticalBlurRenderer);
    }
    else
    {
      mVerticalBlurActor = Actor::New();
      mVerticalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mVerticalBlurActor.AddRenderer(verticalBlurRenderer);
      mInternalRoot.Add(mVerticalBlurActor);

      mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_OPACITY_NAME.data(), 1.0f);
      mVerticalBlurActor.RegisterProperty(UNIFORM_BLUR_STRENGTH_NAME.data(), 1.0f);
    }
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

  // Get size
  Vector2 size = GetTargetSize();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[BlurEffect:%p] OnActivated! [ID:%d][size:%fx%f]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1, size.x, size.y);

  uint32_t downsampledWidth  = std::max(static_cast<uint32_t>(size.width * mDownscaleFactor), 1u);
  uint32_t downsampledHeight = std::max(static_cast<uint32_t>(size.height * mDownscaleFactor), 1u);

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
  SetShaderConstants(downsampledWidth, downsampledHeight);

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
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[BlurEffect:%p] OnDeactivated! [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

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

  mInputBackgroundFrameBuffer.Reset();
  mTemporaryFrameBuffer.Reset();
  mBlurredOutputFrameBuffer.Reset();

  Vector2  size              = GetTargetSize();
  uint32_t downsampledWidth  = std::max(static_cast<uint32_t>(size.width * mDownscaleFactor), 1u);
  uint32_t downsampledHeight = std::max(static_cast<uint32_t>(size.height * mDownscaleFactor), 1u);

  // Set size
  mCamera.SetPerspectiveProjection(size);
  mRenderDownsampledCamera.SetPerspectiveProjection(Size(downsampledWidth, downsampledHeight));
  mHorizontalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));
  mVerticalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));

  CreateFrameBuffers(ImageDimensions(downsampledWidth, downsampledHeight));
  SetShaderConstants(downsampledWidth, downsampledHeight);

  mSourceRenderTask.SetFrameBuffer(mInputBackgroundFrameBuffer);
  mHorizontalBlurTask.SetFrameBuffer(mTemporaryFrameBuffer);
  mVerticalBlurTask.SetFrameBuffer(mBlurredOutputFrameBuffer);
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

void BackgroundBlurEffectImpl::OnRenderFinished(Dali::RenderTask& renderTask)
{
  mFinishedSignal.Emit();
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

void BackgroundBlurEffectImpl::ApplyRenderTaskSourceActor(RenderTask sourceRenderTask, const Toolkit::Control sourceControl)
{
  if(DALI_UNLIKELY(!sourceRenderTask || !sourceControl))
  {
    return;
  }

  bool        isExclusiveRequired = false;
  Dali::Actor sourceActor         = sourceControl;
  Dali::Actor stopperActor        = Dali::Actor(); // Give empty handle to invalidate previous render until option.

  stopperActor = sourceControl;
  while(sourceActor && sourceActor.GetParent())
  {
    sourceActor              = sourceActor.GetParent();
    Toolkit::Control control = Toolkit::Control::DownCast(sourceActor);
    if(control && GetImplementation(control).GetOffScreenRenderableType() == OffScreenRenderable::Type::FORWARD)
    {
      sourceActor         = GetImplementation(control).GetOffScreenRenderableSourceActor();
      isExclusiveRequired = GetImplementation(control).IsOffScreenRenderTaskExclusive();
      break;
    }
  }

  sourceRenderTask.SetExclusive(isExclusiveRequired);
  sourceRenderTask.SetSourceActor(sourceActor);
  sourceRenderTask.RenderUntil(stopperActor);
}

void BackgroundBlurEffectImpl::SetShaderConstants(uint32_t downsampledWidth, uint32_t downsampledHeight)
{
  const uint32_t sampleCount    = mDownscaledBlurRadius >> 1; // compression
  const uint32_t kernelSize     = sampleCount * 4 - 1;
  const uint32_t halfKernelSize = kernelSize / 2 + 1; // Gaussian curve is symmetric

  // Output: Gaussian kernel compressed to half size
  std::vector<float> uvOffsets(sampleCount);
  std::vector<float> weights(sampleCount);

  // Generate half size kernel
  std::vector<float> halfSideKernel(halfKernelSize);

  halfSideKernel[0]  = CalculateGaussianWeight(0.0f, mBellCurveWidth);
  float totalWeights = halfSideKernel[0];
  for(unsigned int i = 1; i < halfKernelSize; i++)
  {
    float w           = CalculateGaussianWeight(i, mBellCurveWidth);
    halfSideKernel[i] = w;
    totalWeights += w * 2.0f;
  }
  for(unsigned int i = 0; i < halfKernelSize; i++)
  {
    halfSideKernel[i] /= totalWeights;
  }
  halfSideKernel[0] *= 0.5f;

  // Compress kernel to half size
  for(unsigned int i = 0; i < sampleCount; i++)
  {
    weights[i]   = halfSideKernel[2 * i] + halfSideKernel[2 * i + 1];
    uvOffsets[i] = 2.0f * i + halfSideKernel[2 * i + 1] / weights[i];
  }

  // Set shader constants
  for(unsigned int i = 0; i < sampleCount; ++i)
  {
    mHorizontalBlurActor.RegisterProperty(GetSampleOffsetsPropertyName(i), Vector2(uvOffsets[i] / downsampledWidth, 0.0f));
    mHorizontalBlurActor.RegisterProperty(GetSampleWeightsPropertyName(i), weights[i]);

    mVerticalBlurActor.RegisterProperty(GetSampleOffsetsPropertyName(i), Vector2(0.0f, uvOffsets[i] / downsampledHeight));
    mVerticalBlurActor.RegisterProperty(GetSampleWeightsPropertyName(i), weights[i]);
  }
}

std::string BackgroundBlurEffectImpl::GetSampleOffsetsPropertyName(unsigned int index) const
{
  std::ostringstream oss;
  oss << "uSampleOffsets[" << index << "]";
  return oss.str();
}

std::string BackgroundBlurEffectImpl::GetSampleWeightsPropertyName(unsigned int index) const
{
  std::ostringstream oss;
  oss << "uSampleWeights[" << index << "]";
  return oss.str();
}

Dali::Toolkit::BackgroundBlurEffect::FinishedSignalType& BackgroundBlurEffectImpl::FinishedSignal()
{
  return mFinishedSignal;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
