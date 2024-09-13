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
#include <dali-toolkit/internal/controls/render-effects/blur-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace
{
// Default values
static constexpr float    BLUR_EFFECT_DOWNSCALE_FACTOR = 0.4f;
static constexpr uint32_t BLUR_EFFECT_PIXEL_RADIUS     = 10u;
static constexpr int32_t  BLUR_EFFECT_ORDER_INDEX      = 101;

static constexpr float MINIMUM_DOWNSCALE_FACTOR = 0.1f;
static constexpr float MAXIMUM_DOWNSCALE_FACTOR = 1.0f;

static constexpr uint32_t MINIMUM_GPU_ARRAY_SIZE = 2u;   // GPU cannot handle array size smaller than 2.
static constexpr uint32_t MAXIMUM_BLUR_RADIUS    = 500u; ///< Maximum pixel radius for blur effect. (GL_MAX_FRAGMENT_UNIFORM_COMPONENTS(Usually 1024) - 19 (vertex shader used)) / 3 float

static constexpr float   MAXIMUM_BELL_CURVE_WIDTH            = 171.352f; ///< bell curve width for MAXIMUM_BLUR_RADIUS case
static constexpr int32_t MAXIMUM_BELL_CURVE_LOOP_TRIAL_COUNT = 50;

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

BlurEffectImpl::BlurEffectImpl(bool isBackground)
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(BLUR_EFFECT_DOWNSCALE_FACTOR),
  mPixelRadius(BLUR_EFFECT_PIXEL_RADIUS),
  mBellCurveWidth(Math::MACHINE_EPSILON_1),
  mSkipBlur(false),
  mIsBackground(isBackground)
{
}

BlurEffectImpl::BlurEffectImpl(float downscaleFactor, uint32_t blurRadius, bool isBackground)
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(downscaleFactor),
  mPixelRadius(blurRadius),
  mBellCurveWidth(Math::MACHINE_EPSILON_1),
  mSkipBlur(false),
  mIsBackground(isBackground)
{
  if(DALI_UNLIKELY(mDownscaleFactor < MINIMUM_DOWNSCALE_FACTOR || mDownscaleFactor > MAXIMUM_DOWNSCALE_FACTOR))
  {
    mDownscaleFactor = Dali::Clamp(mDownscaleFactor, MINIMUM_DOWNSCALE_FACTOR, MAXIMUM_DOWNSCALE_FACTOR);
  }

  if(DALI_UNLIKELY(blurRadius > MAXIMUM_BLUR_RADIUS))
  {
    const uint32_t fixedBlurRadius      = MAXIMUM_BLUR_RADIUS;
    const float    fixedDownScaleFactor = Dali::Clamp(
      mDownscaleFactor * static_cast<float>(fixedBlurRadius) / static_cast<float>(blurRadius),
      MINIMUM_DOWNSCALE_FACTOR,
      MAXIMUM_DOWNSCALE_FACTOR);

    DALI_LOG_ERROR("Blur radius is out of bound: %u. Use %u and make downscale factor %f to %f.\n",
                   blurRadius,
                   fixedBlurRadius,
                   mDownscaleFactor,
                   fixedDownScaleFactor);

    mDownscaleFactor = fixedDownScaleFactor;
    mPixelRadius     = fixedBlurRadius;
  }

  mPixelRadius = static_cast<uint32_t>(mPixelRadius * mDownscaleFactor);

  if(DALI_UNLIKELY((mPixelRadius >> 1) < MINIMUM_GPU_ARRAY_SIZE))
  {
    mSkipBlur = true;
    DALI_LOG_ERROR("Blur radius is too small. This blur will be ignored.\n");
  }
}

BlurEffectImpl::~BlurEffectImpl()
{
}

BlurEffectImplPtr BlurEffectImpl::New(bool isBackground)
{
  BlurEffectImplPtr handle = new BlurEffectImpl(isBackground);
  handle->Initialize();
  return handle;
}

BlurEffectImplPtr BlurEffectImpl::New(float downscaleFactor, uint32_t blurRadius, bool isBackground)
{
  BlurEffectImplPtr handle = new BlurEffectImpl(downscaleFactor, blurRadius, isBackground);
  handle->Initialize();
  return handle;
}

void BlurEffectImpl::OnInitialize()
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    return;
  }

  // Create CameraActors
  {
    mRenderFullSizeCamera = CameraActor::New();
    mRenderFullSizeCamera.SetInvertYAxis(true);
    mRenderFullSizeCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mRenderFullSizeCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mRenderFullSizeCamera.SetType(Dali::Camera::FREE_LOOK);
    mInternalRoot.Add(mRenderFullSizeCamera);

    mRenderDownsampledCamera = CameraActor::New();
    mRenderDownsampledCamera.SetInvertYAxis(true);
    mRenderDownsampledCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mRenderDownsampledCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK);
    mInternalRoot.Add(mRenderDownsampledCamera);
  }

  // Calculate bell curve width
  {
    const float epsilon     = 1e-2f / (mPixelRadius * 2);
    const float localOffset = (mPixelRadius * 2) - 1;

    float lowerBoundBellCurveWidth = Math::MACHINE_EPSILON_10000;
    float upperBoundBellCurveWidth = MAXIMUM_BELL_CURVE_WIDTH;

    int trialCount = 0;
    while(trialCount++ < MAXIMUM_BELL_CURVE_LOOP_TRIAL_COUNT && upperBoundBellCurveWidth - lowerBoundBellCurveWidth > Math::MACHINE_EPSILON_10000)
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

  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[BlurEffect:%p] mBellCurveWidth calculated! [mPixelRadius:%u][mBellCurveWidth:%f]\n", this, mPixelRadius, mBellCurveWidth);

  // Create blur actors
  {
    mInternalRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

    // shader
    std::ostringstream fragmentStringStream;
    fragmentStringStream << "#define NUM_SAMPLES " << (mPixelRadius >> 1) << "\n";
    fragmentStringStream << SHADER_BLUR_EFFECT_FRAG;
    std::string fragmentSource(fragmentStringStream.str());

    // Create an actor for performing a horizontal blur on the texture
    mHorizontalBlurActor = Actor::New();
    mHorizontalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    Renderer horizontalBlurRenderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
    horizontalBlurRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha
    mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);
    mInternalRoot.Add(mHorizontalBlurActor);

    // Create an actor for performing a vertical blur on the texture
    mVerticalBlurActor = Actor::New();
    mVerticalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    Renderer verticalBlurRenderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
    verticalBlurRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha
    mVerticalBlurActor.AddRenderer(verticalBlurRenderer);
    mInternalRoot.Add(mVerticalBlurActor);
  }
}

void BlurEffectImpl::OnActivate()
{
  if(DALI_UNLIKELY(mSkipBlur))
  {
    return;
  }

  Toolkit::Control ownerControl = GetOwnerControl();
  DALI_ASSERT_ALWAYS(ownerControl && "Set the owner of RenderEffect before you activate.");

  // Get size
  Vector2 size = GetTargetSizeForValidTexture();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[BlurEffect:%p] OnActivated! [ID:%d][size:%fx%f]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1, size.x, size.y);

  if(size == Vector2::ZERO)
  {
    return;
  }
  uint32_t downsampledWidth  = std::max(static_cast<uint32_t>(size.width * mDownscaleFactor), 1u);
  uint32_t downsampledHeight = std::max(static_cast<uint32_t>(size.height * mDownscaleFactor), 1u);

  // Set size
  mRenderFullSizeCamera.SetPerspectiveProjection(size);
  mRenderDownsampledCamera.SetPerspectiveProjection(Size(downsampledWidth, downsampledHeight));

  mHorizontalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));
  mVerticalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));

  // Keep sceneHolder as week handle.
  Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(ownerControl);
  if(DALI_UNLIKELY(!sceneHolder))
  {
    DALI_LOG_ERROR("BlurEffect Could not be activated due to ownerControl's SceneHolder is not exist\n");
    return;
  }
  mPlacementSceneHolder = sceneHolder;

  // Set blur
  CreateFrameBuffers(size, ImageDimensions(downsampledWidth, downsampledHeight));
  CreateRenderTasks(sceneHolder, ownerControl);
  SetShaderConstants(downsampledWidth, downsampledHeight);

  // Inject blurred output to control
  Renderer renderer = GetTargetRenderer();
  if(mIsBackground)
  {
    renderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Dali::Toolkit::DepthIndex::BACKGROUND - 3);
  }
  else
  {
    renderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Dali::Toolkit::DepthIndex::CONTENT);
  }
  ownerControl.AddRenderer(renderer);
  SetRendererTexture(renderer, mSourceFrameBuffer);

  ownerControl.Add(mInternalRoot);
}

void BlurEffectImpl::OnDeactivate()
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

  mInputBackgroundFrameBuffer.Reset();
  mTemporaryFrameBuffer.Reset();
  mSourceFrameBuffer.Reset();

  auto sceneHolder = mPlacementSceneHolder.GetHandle();
  if(DALI_LIKELY(sceneHolder))
  {
    RenderTaskList taskList = sceneHolder.GetRenderTaskList();
    taskList.RemoveTask(mHorizontalBlurTask);
    taskList.RemoveTask(mVerticalBlurTask);
    taskList.RemoveTask(mSourceRenderTask);
    mPlacementSceneHolder.Reset();
  }

  mHorizontalBlurTask.Reset();
  mVerticalBlurTask.Reset();
  mSourceRenderTask.Reset();
}

void BlurEffectImpl::CreateFrameBuffers(const Vector2 size, const ImageDimensions downsampledSize)
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
  mSourceFrameBuffer    = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::DEPTH_STENCIL);
  Texture sourceTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mSourceFrameBuffer.AttachColorTexture(sourceTexture);
}

void BlurEffectImpl::CreateRenderTasks(Integration::SceneHolder sceneHolder, const Toolkit::Control sourceControl)
{
  RenderTaskList taskList = sceneHolder.GetRenderTaskList();

  // draw input texture
  mSourceRenderTask = taskList.CreateTask();
  if(mIsBackground)
  {
    mSourceRenderTask.SetSourceActor(sceneHolder.GetRootLayer());
    mSourceRenderTask.RenderUntil(sourceControl);
  }
  else
  {
    mSourceRenderTask.SetSourceActor(sourceControl);
  }
  mSourceRenderTask.SetOrderIndex(BLUR_EFFECT_ORDER_INDEX);
  mSourceRenderTask.SetCameraActor(mRenderFullSizeCamera);
  mSourceRenderTask.SetFrameBuffer(mInputBackgroundFrameBuffer);
  mSourceRenderTask.SetInputEnabled(false);
  mSourceRenderTask.SetExclusive(false);

  // Clear inputBackgroundTexture as scene holder background.
  mSourceRenderTask.SetClearEnabled(true);
  mSourceRenderTask.SetClearColor(sceneHolder.GetBackgroundColor());

  // draw half-blurred output
  SetRendererTexture(mHorizontalBlurActor.GetRendererAt(0), mInputBackgroundFrameBuffer);
  mHorizontalBlurTask = taskList.CreateTask();
  mHorizontalBlurTask.SetSourceActor(mHorizontalBlurActor);
  mHorizontalBlurTask.SetOrderIndex(BLUR_EFFECT_ORDER_INDEX + 1);
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
  mVerticalBlurTask.SetOrderIndex(BLUR_EFFECT_ORDER_INDEX + 2);
  mVerticalBlurTask.SetExclusive(true);
  mVerticalBlurTask.SetInputEnabled(false);
  mVerticalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mVerticalBlurTask.SetFrameBuffer(mSourceFrameBuffer);

  // Clear sourceTexture as Transparent.
  mVerticalBlurTask.SetClearEnabled(true);
  mVerticalBlurTask.SetClearColor(Color::TRANSPARENT);
}

Vector2 BlurEffectImpl::GetTargetSizeForValidTexture() const
{
  Vector2 size = GetTargetSize();
  if(size == Vector2::ZERO)
  {
    size = GetOwnerControl().GetNaturalSize();
  }

  if(size == Vector2::ZERO || size.x < 0.0f || size.y < 0.0f)
  {
    return Vector2::ZERO;
  }

  const uint32_t maxTextureSize = Dali::GetMaxTextureSize();
  if(uint32_t(size.x) > maxTextureSize || uint32_t(size.y) > maxTextureSize)
  {
    uint32_t denominator = std::max(size.x, size.y);
    size.x               = (size.x * maxTextureSize / denominator);
    size.y               = (size.y * maxTextureSize / denominator);
  }
  return size;
}

void BlurEffectImpl::SetShaderConstants(uint32_t downsampledWidth, uint32_t downsampledHeight)
{
  const uint32_t sampleCount    = mPixelRadius >> 1; // compression
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

  // Apply background properties
  if(mIsBackground)
  {
    SynchronizeBackgroundCornerRadius();
  }
}

std::string BlurEffectImpl::GetSampleOffsetsPropertyName(unsigned int index) const
{
  std::ostringstream oss;
  oss << "uSampleOffsets[" << index << "]";
  return oss.str();
}

std::string BlurEffectImpl::GetSampleWeightsPropertyName(unsigned int index) const
{
  std::ostringstream oss;
  oss << "uSampleWeights[" << index << "]";
  return oss.str();
}

void BlurEffectImpl::SynchronizeBackgroundCornerRadius()
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[BlurEffect:%p] Synchronize background corner radius\n", this);

  DALI_ASSERT_ALWAYS(GetOwnerControl() && "You should first SetRenderEffect(), then set its background property map");

  Property::Map map    = GetOwnerControl().GetProperty<Property::Map>(Toolkit::Control::Property::BACKGROUND);
  Vector4       radius = Vector4::ZERO;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS].Get(radius);

  Visual::Transform::Policy::Type policy{Visual::Transform::Policy::ABSOLUTE};
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY].Get(policy);

  Renderer renderer = GetTargetRenderer();
  renderer.RegisterProperty("uCornerRadius", radius);
  renderer.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(policy));
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
