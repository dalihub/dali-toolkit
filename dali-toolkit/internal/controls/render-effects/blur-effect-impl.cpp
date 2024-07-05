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
#include <dali/devel-api/common/stage.h>
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
static constexpr float    BLUR_EFFECT_DOWNSCALE_FACTOR    = 0.4f;
static constexpr uint32_t BLUR_EFFECT_PIXEL_RADIUS        = 5u;
static constexpr float    BLUR_EFFECT_BELL_CURVE_WIDTH    = 1.5f;
static constexpr int32_t  BLUR_EFFECT_ORDER_INDEX         = 101;
static constexpr float    BLUR_EFFECT_DIVIDE_ZERO_EPSILON = 0.001f;
} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
// mMultiplierForFraction and mDenominator are for CalculateGaussianWeight().
// The original equation,
//   (1.0f / sqrt(2.0f * Math::PI * mBellCurveWidth)) * exp(-(localOffset * localOffset) * (1.0f / (2.0f * mBellCurveWidth * mBellCurveWidth)));
// is simplified as below:
//   mDenominator * exp(-(localOffset * localOffset) * mMultiplierForFraction);

BlurEffectImpl::BlurEffectImpl(bool isBackground)
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(BLUR_EFFECT_DOWNSCALE_FACTOR),
  mPixelRadius(BLUR_EFFECT_PIXEL_RADIUS),
  mBellCurveWidth(BLUR_EFFECT_BELL_CURVE_WIDTH),
  mMultiplierForFraction(1.0f / (2.0f * mBellCurveWidth * mBellCurveWidth)),
  mDenominator(1.0f / sqrt(2.0f * Math::PI * mBellCurveWidth)),
  mIsActivated(false),
  mIsBackground(isBackground)
{
}

BlurEffectImpl::BlurEffectImpl(float downscaleFactor, uint32_t blurRadius, float bellCurveWidth, bool isBackground)
: RenderEffectImpl(),
  mInternalRoot(Actor::New()),
  mDownscaleFactor(downscaleFactor),
  mPixelRadius((blurRadius >> 2) + 1),
  mBellCurveWidth(std::max(bellCurveWidth, BLUR_EFFECT_DIVIDE_ZERO_EPSILON)),
  mMultiplierForFraction(1.0f / (2.0f * mBellCurveWidth * mBellCurveWidth)),
  mDenominator(1.0f / sqrt(2.0f * Math::PI * mBellCurveWidth)),
  mIsActivated(false),
  mIsBackground(isBackground)
{
  DALI_ASSERT_ALWAYS(downscaleFactor <= 1.0 && 0.0 < downscaleFactor);
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

BlurEffectImplPtr BlurEffectImpl::New(float downscaleFactor, uint32_t blurRadius, float bellCurveWidth, bool isBackground)
{
  BlurEffectImplPtr handle = new BlurEffectImpl(downscaleFactor, blurRadius, bellCurveWidth, isBackground);
  handle->Initialize();
  return handle;
}

void BlurEffectImpl::Initialize()
{
  mRenderFullSizeCamera = CameraActor::New();
  mRenderFullSizeCamera.SetInvertYAxis(true);
  mRenderFullSizeCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mRenderFullSizeCamera.SetNearClippingPlane(1.0f);
  mRenderFullSizeCamera.SetType(Dali::Camera::FREE_LOOK);
  mRenderFullSizeCamera.SetFieldOfView(Math::PI / 4.0f);
  mInternalRoot.Add(mRenderFullSizeCamera);

  mRenderDownsampledCamera = CameraActor::New();
  mRenderDownsampledCamera.SetInvertYAxis(true);
  mRenderDownsampledCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mRenderDownsampledCamera.SetNearClippingPlane(1.0f);
  mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK);
  mRenderDownsampledCamera.SetFieldOfView(Math::PI / 4.0f);
  mInternalRoot.Add(mRenderDownsampledCamera);

  //////////////////////////////////////////////////////
  // Create shaders

  std::ostringstream fragmentStringStream;
  fragmentStringStream << "#define NUM_SAMPLES " << mPixelRadius << "\n";
  fragmentStringStream << SHADER_BLUR_EFFECT_FRAG;
  std::string fragmentSource(fragmentStringStream.str());

  //////////////////////////////////////////////////////
  // Create actors
  mInternalRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  // Create an actor for performing a horizontal blur on the texture
  mHorizontalBlurActor = Actor::New();
  mHorizontalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Renderer horizontalBlurRenderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
  mHorizontalBlurActor.AddRenderer(horizontalBlurRenderer);
  mInternalRoot.Add(mHorizontalBlurActor);

  // Create an actor for performing a vertical blur on the texture
  mVerticalBlurActor = Actor::New();
  mVerticalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Renderer verticalBlurRenderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
  mVerticalBlurActor.AddRenderer(verticalBlurRenderer);
  mInternalRoot.Add(mVerticalBlurActor);
}

void BlurEffectImpl::Activate(Toolkit::Control ownerControl)
{
  DALI_ASSERT_ALWAYS(ownerControl && "Given empty owner control");

  if(mIsActivated)
  {
    if(ownerControl == GetOwnerControl())
    {
      return;
    }
    else
    {
      Deactivate();
    }
  }
  SetOwnerControl(ownerControl);
  mIsActivated = true;

  // Get input texture size
  Vector2 size = GetTargetSize();
  if(size == Vector2::ZERO)
  {
    size = ownerControl.GetNaturalSize();
    if(size == Vector2::ZERO)
    {
      return;
    }
  }
  DALI_ASSERT_ALWAYS(!(size.x < 0.0f || size.y < 0.0f || uint32_t(size.x) > Dali::GetMaxTextureSize() || uint32_t(size.y) > Dali::GetMaxTextureSize()) && "You need to pass a valid texture size.");

  uint32_t downsampledWidth  = static_cast<uint32_t>(size.width * mDownscaleFactor);
  uint32_t downsampledHeight = static_cast<uint32_t>(size.height * mDownscaleFactor);

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // Prepare resource
  // original texture output
  mInputBackgroundFrameBuffer    = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::NONE);
  Texture inputBackgroundTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mInputBackgroundFrameBuffer.AttachColorTexture(inputBackgroundTexture);

  // half-blurred output
  mTemporaryFrameBuffer    = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::NONE);
  Texture temporaryTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mTemporaryFrameBuffer.AttachColorTexture(temporaryTexture);

  // blurred output
  mSourceFrameBuffer    = FrameBuffer::New(downsampledWidth, downsampledHeight, FrameBuffer::Attachment::NONE);
  Texture sourceTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, downsampledWidth, downsampledHeight);
  mSourceFrameBuffer.AttachColorTexture(sourceTexture);

  // Add BlurActors
  mHorizontalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight)); // mTemporaryFrameBuffer
  mVerticalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(downsampledWidth, downsampledHeight));   // mSourceFrameBuffer

  // Add CameraActors
  float cameraPosConstraintScale = 0.5f / tanf(Math::PI / 4.0f * 0.5f);

  mRenderFullSizeCamera.SetAspectRatio(size.width / size.height);
  mRenderFullSizeCamera.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraPosConstraintScale * size.height));

  mRenderDownsampledCamera.SetAspectRatio(float(downsampledWidth) / float(downsampledHeight));
  mRenderDownsampledCamera.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, cameraPosConstraintScale * size.height * mDownscaleFactor));

  SetShaderConstants(downsampledWidth, downsampledHeight);

  // Prepare input texture
  mSourceRenderTask = taskList.CreateTask();
  if(mIsBackground)
  {
    mSourceRenderTask.SetSourceActor(Stage::GetCurrent().GetRootLayer());
    mSourceRenderTask.RenderUntil(ownerControl);
  }
  else
  {
    mSourceRenderTask.SetSourceActor(ownerControl);
  }
  mSourceRenderTask.SetOrderIndex(BLUR_EFFECT_ORDER_INDEX);
  mSourceRenderTask.SetCameraActor(mRenderFullSizeCamera);
  mSourceRenderTask.SetFrameBuffer(mInputBackgroundFrameBuffer);
  mSourceRenderTask.SetInputEnabled(false);
  mSourceRenderTask.SetExclusive(false);

  // Blur tasks
  SetRendererTexture(mHorizontalBlurActor.GetRendererAt(0), mInputBackgroundFrameBuffer);
  mHorizontalBlurTask = taskList.CreateTask();
  mHorizontalBlurTask.SetSourceActor(mHorizontalBlurActor);
  mHorizontalBlurTask.SetOrderIndex(BLUR_EFFECT_ORDER_INDEX + 1);
  mHorizontalBlurTask.SetExclusive(true);
  mHorizontalBlurTask.SetInputEnabled(false);
  mHorizontalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mHorizontalBlurTask.SetFrameBuffer(mTemporaryFrameBuffer);

  SetRendererTexture(mVerticalBlurActor.GetRendererAt(0), mTemporaryFrameBuffer);
  mVerticalBlurTask = taskList.CreateTask();
  mVerticalBlurTask.SetSourceActor(mVerticalBlurActor);
  mVerticalBlurTask.SetOrderIndex(BLUR_EFFECT_ORDER_INDEX + 2);
  mVerticalBlurTask.SetExclusive(true);
  mVerticalBlurTask.SetInputEnabled(false);
  mVerticalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mVerticalBlurTask.SetFrameBuffer(mSourceFrameBuffer);

  // Inject output to control
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

void BlurEffectImpl::Deactivate()
{
  mIsActivated = false;

  mInternalRoot.Unparent();

  mInputBackgroundFrameBuffer.Reset();
  mTemporaryFrameBuffer.Reset();
  mSourceFrameBuffer.Reset();

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  taskList.RemoveTask(mHorizontalBlurTask);
  taskList.RemoveTask(mVerticalBlurTask);
  taskList.RemoveTask(mSourceRenderTask);

  ClearOwnerControl();
}

void BlurEffectImpl::SetShaderConstants(float downsampledWidth, float downsampledHeight)
{
  std::vector<float> uvOffsets(mPixelRadius);
  std::vector<float> weights(mPixelRadius);

  // generate bell curve kernel
  unsigned int       halfSize = mPixelRadius * 2;
  std::vector<float> halfSideKernel(halfSize);

  halfSideKernel[0]  = CalculateGaussianWeight(0.0f);
  float totalWeights = halfSideKernel[0];
  for(unsigned int i = 1; i < halfSize; i++)
  {
    float w           = CalculateGaussianWeight(i);
    halfSideKernel[i] = w;
    totalWeights += w * 2.0f;
  }
  for(unsigned int i = 0; i < halfSize; i++)
  {
    halfSideKernel[i] /= totalWeights;
  }
  halfSideKernel[0] *= 0.5f;

  // compress kernel
  for(unsigned int i = 0; i < mPixelRadius; i++)
  {
    weights[i]   = halfSideKernel[2 * i] + halfSideKernel[2 * i + 1];
    uvOffsets[i] = 2.0f * i + halfSideKernel[2 * i + 1] / weights[i];
  }

  // set shader constants
  for(unsigned int i = 0; i < mPixelRadius; ++i)
  {
    mHorizontalBlurActor.RegisterProperty(GetSampleOffsetsPropertyName(i), Vector2(uvOffsets[i] / downsampledWidth, 0.0f));
    mHorizontalBlurActor.RegisterProperty(GetSampleWeightsPropertyName(i), weights[i]);

    mVerticalBlurActor.RegisterProperty(GetSampleOffsetsPropertyName(i), Vector2(0.0f, uvOffsets[i] / downsampledHeight));
    mVerticalBlurActor.RegisterProperty(GetSampleWeightsPropertyName(i), weights[i]);
  }

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
  DALI_ASSERT_ALWAYS(GetOwnerControl() && "You should first SetRenderEffect(), then set its background property map");

  Property::Map map    = GetOwnerControl().GetProperty<Property::Map>(Toolkit::Control::Property::BACKGROUND);
  Vector4       radius = Vector4::ZERO;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS].Get(radius);

  Renderer renderer = GetTargetRenderer();
  renderer.RegisterProperty("uRadius", radius);
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
