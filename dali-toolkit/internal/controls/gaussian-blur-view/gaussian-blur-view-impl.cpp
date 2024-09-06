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
#include "gaussian-blur-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>
#include <iomanip>
#include <sstream>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

// TODO:
// pixel format / size - set from JSON
// aspect ratio property needs to be able to be constrained also for cameras, not possible currently. Therefore changing aspect ratio of GaussianBlurView won't currently work
// default near clip value
// Manager object - re-use render targets if there are multiple GaussianBlurViews created

/////////////////////////////////////////////////////////
// IMPLEMENTATION NOTES

// As the GaussianBlurView actor changes size, the amount of pixels we need to blur changes. Therefore we need some way of doing this. However:-
// OnSetSize() does not get called when GaussianBlurView object size is modified using a Constraint.
// OnSizeAnimation() only gets called once per AnimateTo/By() and if an Animation has N such calls then only the final one will end up being used. Therefore we can't use
// OnSizeAnimation() to alter render target sizes.
// To get around the above problems, we use fixed sized render targets, from the last SetSize() call (which calls OnSetSize()), then we adjust the internal cameras / actors
// to take account of the changed GaussianBlurView object size, projecting to the unchanged render target sizes. This is done relative to the fixed render target / actor sizes
// by using constraints relative to the GaussianBlurView actor size.

// 2 modes:
// 1st mode, this control has a tree of actors (use Add() to add children) that are rendered and blurred.
// mRenderChildrenTask renders children to FB mRenderTargetForRenderingChildren
// mHorizontalBlurTask renders mHorizontalBlurActor Actor showing FB mRenderTargetForRenderingChildren into FB mRenderTarget2
// mVerticalBlurTask renders mVerticalBlurActor Actor showing FB mRenderTarget2 into FB mRenderTarget1
// mCompositeTask renders mCompositingActor Actor showing FB mRenderTarget1 into FB mBlurResultFrameBuffer
//
// 2nd mode, an image is blurred and rendered to a supplied target framebuffer
// mHorizontalBlurTask renders mHorizontalBlurActor Actor showing mUserInputImage into FB mRenderTarget2
// mVerticalBlurTask renders mVerticalBlurActor Actor showing mRenderTarget2 into FB mUserOutputRenderTarget
//
// Only this 2nd mode handles ActivateOnce

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
using namespace Dali;

BaseHandle Create()
{
  return Toolkit::GaussianBlurView::New();
}

DALI_TYPE_REGISTRATION_BEGIN(Toolkit::GaussianBlurView, Toolkit::Control, Create)
DALI_TYPE_REGISTRATION_END()

const unsigned int  GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES                = 5;
const float         GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH      = 1.5f;
const Pixel::Format GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT = Pixel::RGBA8888;
const float         GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH              = 1.0f; // default, fully blurred
const char* const   GAUSSIAN_BLUR_VIEW_STRENGTH_PROPERTY_NAME             = "GaussianBlurStrengthPropertyName";
const float         GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE     = 0.5f;
const float         GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE    = 0.5f;

const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

constexpr uint32_t MAXIMUM_SAMPLES_SIZE = 335u;

/**
  * @brief Calculates gaussian weight
  * @param[in] localOffset Input to the function
  */
inline float CalculateGaussianWeight(float localOffset, float sigma)
{
  return (1.0f / sqrt(2.0f * Dali::Math::PI * sigma)) * exp(-(localOffset / sigma * localOffset / sigma) * 0.5f);
}

} // namespace

GaussianBlurView::GaussianBlurView()
: Control(ControlBehaviour(DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS)),
  mPixelRadius(GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES),
  mBellCurveWidth(GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH),
  mPixelFormat(GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT),
  mDownsampleWidthScale(GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE),
  mDownsampleHeightScale(GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE),
  mDownsampledWidth(0.0f),
  mDownsampledHeight(0.0f),
  mBlurUserImage(false),
  mRenderOnce(false),
  mBackgroundColor(Color::BLACK),
  mTargetSize(Vector2::ZERO),
  mLastSize(Vector2::ZERO),
  mChildrenRoot(Actor::New()),
  mInternalRoot(Actor::New()),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mActivated(false)
{
}

GaussianBlurView::GaussianBlurView(const unsigned int  numSamples,
                                   const float         blurBellCurveWidth,
                                   const Pixel::Format renderTargetPixelFormat,
                                   const float         downsampleWidthScale,
                                   const float         downsampleHeightScale,
                                   bool                blurUserImage)
: Control(ControlBehaviour(DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS)),
  mPixelRadius(numSamples),
  mBellCurveWidth(blurBellCurveWidth),
  mPixelFormat(renderTargetPixelFormat),
  mDownsampleWidthScale(downsampleWidthScale),
  mDownsampleHeightScale(downsampleHeightScale),
  mDownsampledWidth(0.0f),
  mDownsampledHeight(0.0f),
  mBlurUserImage(blurUserImage),
  mRenderOnce(false),
  mBackgroundColor(Color::BLACK),
  mTargetSize(Vector2::ZERO),
  mLastSize(Vector2::ZERO),
  mChildrenRoot(Actor::New()),
  mInternalRoot(Actor::New()),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mActivated(false)
{
  if(mPixelRadius == 0)
  {
    // We always assume that pixel radius is positive.
    mPixelRadius = 1;
  }
  if(mPixelRadius > MAXIMUM_SAMPLES_SIZE)
  {
    float reduceFactor = static_cast<float>(MAXIMUM_SAMPLES_SIZE) / mPixelRadius;
    mDownsampleWidthScale *= reduceFactor;
    mDownsampleHeightScale *= reduceFactor;
    mBellCurveWidth *= reduceFactor;
    mPixelRadius = MAXIMUM_SAMPLES_SIZE;
  }
}

GaussianBlurView::~GaussianBlurView()
{
}

Toolkit::GaussianBlurView GaussianBlurView::New()
{
  GaussianBlurView* impl = new GaussianBlurView();

  Dali::Toolkit::GaussianBlurView handle = Dali::Toolkit::GaussianBlurView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

Toolkit::GaussianBlurView GaussianBlurView::New(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat, const float downsampleWidthScale, const float downsampleHeightScale, bool blurUserImage)
{
  GaussianBlurView* impl = new GaussianBlurView(numSamples, blurBellCurveWidth, renderTargetPixelFormat, downsampleWidthScale, downsampleHeightScale, blurUserImage);

  Dali::Toolkit::GaussianBlurView handle = Dali::Toolkit::GaussianBlurView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void GaussianBlurView::SetUserImageAndOutputRenderTarget(Texture inputImage, FrameBuffer outputRenderTarget)
{
  // can only do this if the GaussianBlurView object was created with this parameter set
  DALI_ASSERT_ALWAYS(mBlurUserImage);

  mUserInputImage = inputImage;

  SetRendererTexture(mHorizontalBlurActor.GetRendererAt(0), inputImage);

  mUserOutputRenderTarget = outputRenderTarget;
}

FrameBuffer GaussianBlurView::GetBlurredRenderTarget() const
{
  if(!mUserOutputRenderTarget)
  {
    return mBlurResultFrameBuffer;
  }

  return mUserOutputRenderTarget;
}

void GaussianBlurView::SetBackgroundColor(const Vector4& color)
{
  mBackgroundColor = color;
}

Vector4 GaussianBlurView::GetBackgroundColor() const
{
  return mBackgroundColor;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void GaussianBlurView::OnInitialize()
{
  // root actor to parent all user added actors, needed to allow us to set that subtree as exclusive for our child render task
  mChildrenRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mInternalRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  //////////////////////////////////////////////////////
  // Create shaders

  std::ostringstream fragmentStringStream;
  fragmentStringStream << "#define NUM_SAMPLES " << (DALI_LIKELY(mPixelRadius > 1u) ? mPixelRadius : 2u) << "\n";
  fragmentStringStream << SHADER_GAUSSIAN_BLUR_VIEW_FRAG;
  std::string fragmentSource(fragmentStringStream.str());

  //////////////////////////////////////////////////////
  // Create actors

  // Create an actor for performing a horizontal blur on the texture
  mHorizontalBlurActor = Actor::New();
  mHorizontalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Renderer renderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
  mHorizontalBlurActor.AddRenderer(renderer);

  // Create an actor for performing a vertical blur on the texture
  mVerticalBlurActor = Actor::New();
  mVerticalBlurActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  renderer = CreateRenderer(BASIC_VERTEX_SOURCE, fragmentSource.c_str());
  mVerticalBlurActor.AddRenderer(renderer);

  // Register a property that the user can control to fade the blur in / out via the GaussianBlurView object
  Actor self                 = Self();
  mBlurStrengthPropertyIndex = self.RegisterProperty(GAUSSIAN_BLUR_VIEW_STRENGTH_PROPERTY_NAME, GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH);

  // Create an image view for compositing the blur and the original child actors render
  if(!mBlurUserImage)
  {
    mCompositingActor = Actor::New();
    mCompositingActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCompositingActor.SetProperty(Actor::Property::OPACITY, GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH); // ensure alpha is enabled for this object and set default value
    renderer = CreateRenderer(BASIC_VERTEX_SOURCE, BASIC_FRAGMENT_SOURCE);
    mCompositingActor.AddRenderer(renderer);

    Constraint blurStrengthConstraint = Constraint::New<float>(mCompositingActor, Actor::Property::COLOR_ALPHA, EqualToConstraint());
    blurStrengthConstraint.AddSource(Source(self, mBlurStrengthPropertyIndex));
    blurStrengthConstraint.Apply();

    // Create an image view for holding final result, i.e. the blurred image. This will get rendered to screen later, via default / user render task
    mTargetActor = Actor::New();
    mTargetActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    renderer = CreateRenderer(BASIC_VERTEX_SOURCE, BASIC_FRAGMENT_SOURCE);
    mTargetActor.AddRenderer(renderer);

    //////////////////////////////////////////////////////
    // Create cameras for the renders corresponding to the view size
    mRenderFullSizeCamera = CameraActor::New();
    mRenderFullSizeCamera.SetInvertYAxis(true);
    mRenderFullSizeCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

    //////////////////////////////////////////////////////
    // Connect to actor tree
    mInternalRoot.Add(mCompositingActor);
    mInternalRoot.Add(mTargetActor);
    mInternalRoot.Add(mRenderFullSizeCamera);
  }

  //////////////////////////////////////////////////////
  // Create camera for the renders corresponding to the (potentially downsampled) render targets' size
  mRenderDownsampledCamera = CameraActor::New();
  mRenderDownsampledCamera.SetInvertYAxis(true);
  mRenderDownsampledCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  //////////////////////////////////////////////////////
  // Connect to actor tree
  Self().Add(mChildrenRoot);
  mInternalRoot.Add(mHorizontalBlurActor);
  mInternalRoot.Add(mVerticalBlurActor);
  mInternalRoot.Add(mRenderDownsampledCamera);

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::FILLER);
}

void GaussianBlurView::OnSizeSet(const Vector3& targetSize)
{
  mTargetSize = Vector2(targetSize);

  mChildrenRoot.SetProperty(Actor::Property::SIZE, targetSize);

  if(!mBlurUserImage)
  {
    mCompositingActor.SetProperty(Actor::Property::SIZE, targetSize);
    mTargetActor.SetProperty(Actor::Property::SIZE, targetSize);

    // Children render camera must move when GaussianBlurView object is resized. This is since we cannot change render target size - so we need to remap the child actors' rendering
    // accordingly so they still exactly fill the render target. Note that this means the effective resolution of the child render changes as the GaussianBlurView object changes
    // size, this is the trade off for not being able to modify render target size
    // Change camera z position based on GaussianBlurView actor height
    float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
    mRenderFullSizeCamera.SetProperty(Actor::Property::POSITION_Z, mTargetSize.height * cameraPosConstraintScale);
  }

  // if we have already activated the blur, need to update render target sizes now to reflect the new size of this actor
  if(mActivated)
  {
    Deactivate();
    Activate();
  }

  Control::OnSizeSet(targetSize);
}

void GaussianBlurView::OnChildAdd(Actor& child)
{
  if(child != mChildrenRoot && child != mInternalRoot)
  {
    mChildrenRoot.Add(child);
  }

  Control::OnChildAdd(child);
}

void GaussianBlurView::OnChildRemove(Actor& child)
{
  mChildrenRoot.Remove(child);

  Control::OnChildRemove(child);
}

void GaussianBlurView::AllocateResources()
{
  mLastSize = mTargetSize;

  // get size of downsampled render targets
  mDownsampledWidth  = mTargetSize.width * mDownsampleWidthScale;
  mDownsampledHeight = mTargetSize.height * mDownsampleHeightScale;

  // Create and place a camera for the renders corresponding to the (potentially downsampled) render targets' size
  mRenderDownsampledCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
  // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
  mRenderDownsampledCamera.SetNearClippingPlane(1.0f);
  mRenderDownsampledCamera.SetAspectRatio(mDownsampledWidth / mDownsampledHeight);
  mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor

  mRenderDownsampledCamera.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, ((mDownsampledHeight * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f))));

  // setup for normal operation
  if(!mBlurUserImage)
  {
    // Create and place a camera for the children render, corresponding to its render target size
    mRenderFullSizeCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
    mRenderFullSizeCamera.SetNearClippingPlane(1.0f);
    mRenderFullSizeCamera.SetAspectRatio(mTargetSize.width / mTargetSize.height);
    mRenderFullSizeCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor

    float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
    mRenderFullSizeCamera.SetProperty(Actor::Property::POSITION, Vector3(0.0f, 0.0f, mTargetSize.height * cameraPosConstraintScale));

    // create offscreen buffer of new size to render our child actors to
    mRenderTargetForRenderingChildren = FrameBuffer::New(mDownsampledWidth, mDownsampledHeight, FrameBuffer::Attachment::NONE);
    Texture texture                   = Texture::New(TextureType::TEXTURE_2D, mPixelFormat, unsigned(mDownsampledWidth), unsigned(mDownsampledHeight));
    mRenderTargetForRenderingChildren.AttachColorTexture(texture);

    // Set actor for performing a horizontal blur
    SetRendererTexture(mHorizontalBlurActor.GetRendererAt(0), mRenderTargetForRenderingChildren);

    // Create offscreen buffer for vert blur pass
    mRenderTarget1 = FrameBuffer::New(mDownsampledWidth, mDownsampledHeight, FrameBuffer::Attachment::NONE);
    texture        = Texture::New(TextureType::TEXTURE_2D, mPixelFormat, unsigned(mDownsampledWidth), unsigned(mDownsampledHeight));
    mRenderTarget1.AttachColorTexture(texture);

    // use the completed blur in the first buffer and composite with the original child actors render
    SetRendererTexture(mCompositingActor.GetRendererAt(0), mRenderTarget1);

    // create offscreen buffer of new size to render composited result.
    mBlurResultFrameBuffer = FrameBuffer::New(mTargetSize.width, mTargetSize.height, FrameBuffer::Attachment::NONE);
    texture                = Texture::New(TextureType::TEXTURE_2D, mPixelFormat, unsigned(mTargetSize.width), unsigned(mTargetSize.height));
    mBlurResultFrameBuffer.AttachColorTexture(texture);

    // set up target actor for rendering result, i.e. the blurred image
    SetRendererTexture(mTargetActor.GetRendererAt(0), mBlurResultFrameBuffer);
  }

  // Create offscreen buffer for horiz blur pass
  mRenderTarget2  = FrameBuffer::New(mDownsampledWidth, mDownsampledHeight, FrameBuffer::Attachment::NONE);
  Texture texture = Texture::New(TextureType::TEXTURE_2D, mPixelFormat, unsigned(mDownsampledWidth), unsigned(mDownsampledHeight));
  mRenderTarget2.AttachColorTexture(texture);

  // size needs to match render target
  mHorizontalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(mDownsampledWidth, mDownsampledHeight));

  // size needs to match render target
  mVerticalBlurActor.SetProperty(Actor::Property::SIZE, Vector2(mDownsampledWidth, mDownsampledHeight));
  SetRendererTexture(mVerticalBlurActor.GetRendererAt(0), mRenderTarget2);

  // set gaussian blur up for new sized render targets
  SetShaderConstants();
}

void GaussianBlurView::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  if(!mBlurUserImage)
  {
    // create render task to render our child actors to offscreen buffer
    mRenderChildrenTask = taskList.CreateTask();
    mRenderChildrenTask.SetSourceActor(mChildrenRoot);
    mRenderChildrenTask.SetExclusive(true);
    mRenderChildrenTask.SetInputEnabled(false);
    mRenderChildrenTask.SetClearEnabled(true);
    mRenderChildrenTask.SetClearColor(mBackgroundColor);

    mRenderChildrenTask.SetCameraActor(mRenderFullSizeCamera);
    mRenderChildrenTask.SetFrameBuffer(mRenderTargetForRenderingChildren);

    if(mRenderOnce)
    {
      mRenderChildrenTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    }
  }

  // perform a horizontal blur targeting the second buffer
  mHorizontalBlurTask = taskList.CreateTask();
  mHorizontalBlurTask.SetSourceActor(mHorizontalBlurActor);
  mHorizontalBlurTask.SetExclusive(true);
  mHorizontalBlurTask.SetInputEnabled(false);
  mHorizontalBlurTask.SetClearEnabled(true);
  mHorizontalBlurTask.SetClearColor(mBackgroundColor);
  mHorizontalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mHorizontalBlurTask.SetFrameBuffer(mRenderTarget2);
  if(mRenderOnce || (mRenderOnce && mBlurUserImage))
  {
    mHorizontalBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  }

  // use the second buffer and perform a horizontal blur targeting the first buffer
  mVerticalBlurTask = taskList.CreateTask();
  mVerticalBlurTask.SetSourceActor(mVerticalBlurActor);
  mVerticalBlurTask.SetExclusive(true);
  mVerticalBlurTask.SetInputEnabled(false);
  mVerticalBlurTask.SetClearEnabled(true);
  mVerticalBlurTask.SetClearColor(mBackgroundColor);
  mVerticalBlurTask.SetCameraActor(mRenderDownsampledCamera);
  if(mUserOutputRenderTarget)
  {
    mVerticalBlurTask.SetFrameBuffer(mUserOutputRenderTarget);
  }
  else
  {
    mVerticalBlurTask.SetFrameBuffer(mRenderTarget1);
  }
  if(mRenderOnce || (mRenderOnce && mBlurUserImage))
  {
    mVerticalBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    mVerticalBlurTask.FinishedSignal().Connect(this, &GaussianBlurView::OnRenderTaskFinished);
  }

  // use the completed blur in the first buffer and composite with the original child actors render
  if(!mBlurUserImage)
  {
    mCompositeTask = taskList.CreateTask();
    mCompositeTask.SetSourceActor(mCompositingActor);
    mCompositeTask.SetExclusive(true);
    mCompositeTask.SetInputEnabled(false);

    mCompositeTask.SetCameraActor(mRenderFullSizeCamera);
    mCompositeTask.SetFrameBuffer(mBlurResultFrameBuffer);

    if(mRenderOnce)
    {
      mCompositeTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    }
  }
}

void GaussianBlurView::RemoveRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  taskList.RemoveTask(mRenderChildrenTask);
  taskList.RemoveTask(mHorizontalBlurTask);
  taskList.RemoveTask(mVerticalBlurTask);
  taskList.RemoveTask(mCompositeTask);
}

void GaussianBlurView::Activate()
{
  if(!mActivated)
  {
    // make sure resources are allocated and start the render tasks processing
    Self().Add(mInternalRoot);
    AllocateResources();
    CreateRenderTasks();
    mActivated = true;
  }
}

void GaussianBlurView::ActivateOnce()
{
  Deactivate();
  mRenderOnce = true;
  Activate();
}

void GaussianBlurView::Deactivate()
{
  if(mActivated)
  {
    // stop render tasks processing
    // Note: render target resources are automatically freed since we set the Image::Unused flag
    mInternalRoot.Unparent();
    mRenderTargetForRenderingChildren.Reset();
    mBlurResultFrameBuffer.Reset();
    mRenderTarget1.Reset();
    mRenderTarget2.Reset();
    RemoveRenderTasks();
    mRenderOnce = false;
    mActivated  = false;
  }
}

void GaussianBlurView::SetShaderConstants()
{
  const uint32_t numSamples = DALI_LIKELY(mPixelRadius > 1u) ? mPixelRadius : 2u;

  std::vector<float> uvOffsets(numSamples);
  std::vector<float> weights(numSamples);

  // generate bell curve kernel
  unsigned int       halfSize = numSamples * 2;
  std::vector<float> halfSideKernel(halfSize);

  halfSideKernel[0]  = CalculateGaussianWeight(0.0f, mBellCurveWidth);
  float totalWeights = halfSideKernel[0];
  for(unsigned int i = 1; i < halfSize; i++)
  {
    float w           = CalculateGaussianWeight(i, mBellCurveWidth);
    halfSideKernel[i] = w;
    totalWeights += w * 2.0f;
  }
  for(unsigned int i = 0; i < halfSize; i++)
  {
    halfSideKernel[i] /= totalWeights;
  }
  halfSideKernel[0] *= 0.5f;

  // compress kernel
  for(unsigned int i = 0; i < numSamples; i++)
  {
    weights[i]   = halfSideKernel[2 * i] + halfSideKernel[2 * i + 1];
    uvOffsets[i] = 2.0f * i + halfSideKernel[2 * i + 1] / weights[i];
  }

  // set shader constants
  for(unsigned int i = 0; i < numSamples; ++i)
  {
    mHorizontalBlurActor.RegisterProperty(GetSampleOffsetsPropertyName(i), Vector2(uvOffsets[i] / mDownsampledWidth, 0.0f));
    mHorizontalBlurActor.RegisterProperty(GetSampleWeightsPropertyName(i), weights[i]);

    mVerticalBlurActor.RegisterProperty(GetSampleOffsetsPropertyName(i), Vector2(0.0f, uvOffsets[i] / mDownsampledHeight));
    mVerticalBlurActor.RegisterProperty(GetSampleWeightsPropertyName(i), weights[i]);
  }
}

std::string GaussianBlurView::GetSampleOffsetsPropertyName(unsigned int index) const
{
  std::ostringstream oss;
  oss << "uSampleOffsets[" << index << "]";
  return oss.str();
}

std::string GaussianBlurView::GetSampleWeightsPropertyName(unsigned int index) const
{
  std::ostringstream oss;
  oss << "uSampleWeights[" << index << "]";
  return oss.str();
}

Dali::Toolkit::GaussianBlurView::GaussianBlurViewSignal& GaussianBlurView::FinishedSignal()
{
  return mFinishedSignal;
}

void GaussianBlurView::OnRenderTaskFinished(Dali::RenderTask& renderTask)
{
  Toolkit::GaussianBlurView handle(GetOwner());
  mFinishedSignal.Emit(handle);
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
