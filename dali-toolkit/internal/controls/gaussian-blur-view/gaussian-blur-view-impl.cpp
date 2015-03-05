/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <sstream>
#include <iomanip>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/gaussian-blur-view/gaussian-blur-view.h>

// TODO:
// pixel format / size - set from JSON
// aspect ratio property needs to be able to be constrained also for cameras, not possible currently. Therefore changing aspect ratio of GaussianBlurView won't currently work
// default near clip value
// mChildrenRoot Add()/Remove() overloads - better solution
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
// mHorizBlurTask renders mImageActorHorizBlur Actor showing FB mRenderTargetForRenderingChildren into FB mRenderTarget2
// mVertBlurTask renders mImageActorVertBlur Actor showing FB mRenderTarget2 into FB mRenderTarget1
// mCompositeTask renders mImageActorComposite Actor showing FB mRenderTarget1 into FB mRenderTargetForRenderingChildren
//
// 2nd mode, an image is blurred and rendered to a supplied target framebuffer
// mHorizBlurTask renders mImageActorHorizBlur Actor showing mUserInputImage into FB mRenderTarget2
// mVertBlurTask renders mImageActorVertBlur Actor showing mRenderTarget2 into FB mUserOutputRenderTarget
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

TypeRegistration mType( typeid(Toolkit::GaussianBlurView), typeid(Toolkit::Control), Create );


const unsigned int GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES = 5;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH = 1.5f;
const Pixel::Format GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT = Pixel::RGBA8888;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH = 1.0f;                                       // default, fully blurred
const std::string GAUSSIAN_BLUR_VIEW_STRENGTH_PROPERTY_NAME("GaussianBlurStrengthPropertyName");
const float GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE = 0.5f;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE = 0.5f;

const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

const char* const GAUSSIAN_BLUR_FRAGMENT_SOURCE =
    "uniform mediump vec2 uSampleOffsets[NUM_SAMPLES];\n"
    "uniform mediump float uSampleWeights[NUM_SAMPLES];\n"

    "void main()\n"
    "{\n"
    "   mediump vec4 col;\n"
    "   col = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y) + uSampleOffsets[0]) * uSampleWeights[0];     \n"
    "   for (int i=1; i<NUM_SAMPLES; ++i)                                                                      \n"
    "   {                                                                                                      \n"
    "     col += texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y) + uSampleOffsets[i]) * uSampleWeights[i];  \n"
    "   }                                                                                                      \n"
    "   gl_FragColor = col;\n"
    "}\n";

} // namespace


GaussianBlurView::GaussianBlurView()
  : Control( CONTROL_BEHAVIOUR_NONE )
  , mNumSamples(GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES)
  , mBlurBellCurveWidth( 0.001f )
  , mPixelFormat(GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT)
  , mDownsampleWidthScale(GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE)
  , mDownsampleHeightScale(GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE)
  , mDownsampledWidth( 0.0f )
  , mDownsampledHeight( 0.0f )
  , mBlurUserImage( false )
  , mRenderOnce( false )
  , mBackgroundColor( Color::BLACK )
  , mTargetSize(Vector2::ZERO)
  , mLastSize(Vector2::ZERO)
  , mChildrenRoot(Actor::New())
  , mBlurStrengthPropertyIndex(Property::INVALID_INDEX)
{
  SetBlurBellCurveWidth(GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH);
}

GaussianBlurView::GaussianBlurView( const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                    const float downsampleWidthScale, const float downsampleHeightScale,
                                    bool blurUserImage)
  : Control( CONTROL_BEHAVIOUR_NONE )
  , mNumSamples(numSamples)
  , mBlurBellCurveWidth( 0.001f )
  , mPixelFormat(renderTargetPixelFormat)
  , mDownsampleWidthScale(downsampleWidthScale)
  , mDownsampleHeightScale(downsampleHeightScale)
  , mDownsampledWidth( 0.0f )
  , mDownsampledHeight( 0.0f )
  , mBlurUserImage( blurUserImage )
  , mRenderOnce( false )
  , mBackgroundColor( Color::BLACK )
  , mTargetSize(Vector2::ZERO)
  , mLastSize(Vector2::ZERO)
  , mChildrenRoot(Actor::New())
  , mBlurStrengthPropertyIndex(Property::INVALID_INDEX)
{
  SetBlurBellCurveWidth(blurBellCurveWidth);
}

GaussianBlurView::~GaussianBlurView()
{
}


Toolkit::GaussianBlurView GaussianBlurView::New()
{
  GaussianBlurView* impl = new GaussianBlurView();

  Dali::Toolkit::GaussianBlurView handle = Dali::Toolkit::GaussianBlurView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

Toolkit::GaussianBlurView GaussianBlurView::New(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                                const float downsampleWidthScale, const float downsampleHeightScale,
                                                bool blurUserImage)
{
  GaussianBlurView* impl = new GaussianBlurView( numSamples, blurBellCurveWidth, renderTargetPixelFormat,
                                                 downsampleWidthScale, downsampleHeightScale,
                                                 blurUserImage);

  Dali::Toolkit::GaussianBlurView handle = Dali::Toolkit::GaussianBlurView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////
// for creating a subtree for all user added child actors, so that we can have them exclusive to the mRenderChildrenTask and our other actors exclusive to our other tasks
// TODO: overloading Actor::Add()/Remove() not nice since breaks polymorphism. Need another method to pass ownership of added child actors to our internal actor root.
void GaussianBlurView::Add(Actor child)
{
  mChildrenRoot.Add(child);
}

void GaussianBlurView::Remove(Actor child)
{
  mChildrenRoot.Remove(child);
}

void GaussianBlurView::SetUserImageAndOutputRenderTarget(Image inputImage, FrameBufferImage outputRenderTarget)
{
  // can only do this if the GaussianBlurView object was created with this parameter set
  DALI_ASSERT_ALWAYS(mBlurUserImage);

  mUserInputImage = inputImage;
  mImageActorHorizBlur.SetImage( mUserInputImage );

  mUserOutputRenderTarget = outputRenderTarget;
}

FrameBufferImage GaussianBlurView::GetBlurredRenderTarget() const
{
  if(!mUserOutputRenderTarget)
  {
    return mRenderTargetForRenderingChildren;
  }

  return mUserOutputRenderTarget;
}

void GaussianBlurView::SetBackgroundColor( const Vector4& color )
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

/**
 * EqualToConstraintFloat
 *
 * f(current, property) = property
 */
struct EqualToConstraintFloat
{
  EqualToConstraintFloat(){}

  float operator()(const float current, const PropertyInput& property) {return property.GetFloat();}
};

void GaussianBlurView::OnInitialize()
{
  // root actor to parent all user added actors, needed to allow us to set that subtree as exclusive for our child render task
  mChildrenRoot.SetParentOrigin(ParentOrigin::CENTER);

  //////////////////////////////////////////////////////
  // Create shaders

  // horiz
  std::ostringstream horizFragmentShaderStringStream;
  horizFragmentShaderStringStream << "#define NUM_SAMPLES " << mNumSamples << "\n";
  horizFragmentShaderStringStream << GAUSSIAN_BLUR_FRAGMENT_SOURCE;
  mHorizBlurShader = ShaderEffect::New( "", horizFragmentShaderStringStream.str() );
  // vert
  std::ostringstream vertFragmentShaderStringStream;
  vertFragmentShaderStringStream << "#define NUM_SAMPLES " << mNumSamples << "\n";
  vertFragmentShaderStringStream << GAUSSIAN_BLUR_FRAGMENT_SOURCE;
  mVertBlurShader = ShaderEffect::New( "", vertFragmentShaderStringStream.str() );


  //////////////////////////////////////////////////////
  // Create actors

  // Create an ImageActor for performing a horizontal blur on the texture
  mImageActorHorizBlur = ImageActor::New();
  mImageActorHorizBlur.SetParentOrigin(ParentOrigin::CENTER);
  mImageActorHorizBlur.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME
  mImageActorHorizBlur.SetShaderEffect( mHorizBlurShader );

  // Create an ImageActor for performing a vertical blur on the texture
  mImageActorVertBlur = ImageActor::New();
  mImageActorVertBlur.SetParentOrigin(ParentOrigin::CENTER);
  mImageActorVertBlur.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME
  mImageActorVertBlur.SetShaderEffect( mVertBlurShader );

  // Register a property that the user can control to fade the blur in / out via the GaussianBlurView object
  mBlurStrengthPropertyIndex = Self().RegisterProperty(GAUSSIAN_BLUR_VIEW_STRENGTH_PROPERTY_NAME, GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH);

  // Create an ImageActor for compositing the blur and the original child actors render
  if(!mBlurUserImage)
  {
    mImageActorComposite = ImageActor::New();
    mImageActorComposite.SetParentOrigin(ParentOrigin::CENTER);
    mImageActorComposite.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME
    mImageActorComposite.SetOpacity(GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH); // ensure alpha is enabled for this object and set default value

    Constraint blurStrengthConstraint = Constraint::New<float>( Actor::Property::COLOR_ALPHA, ParentSource(mBlurStrengthPropertyIndex), EqualToConstraintFloat());
    mImageActorComposite.ApplyConstraint(blurStrengthConstraint);

    // Create an ImageActor for holding final result, i.e. the blurred image. This will get rendered to screen later, via default / user render task
    mTargetActor = ImageActor::New();
    mTargetActor.SetParentOrigin(ParentOrigin::CENTER);
    mTargetActor.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME


    //////////////////////////////////////////////////////
    // Create cameras for the renders corresponding to the view size
    mRenderFullSizeCamera = CameraActor::New();
    mRenderFullSizeCamera.SetParentOrigin(ParentOrigin::CENTER);


    //////////////////////////////////////////////////////
    // Connect to actor tree
    Self().Add( mImageActorComposite );
    Self().Add( mTargetActor );
    Self().Add( mRenderFullSizeCamera );
  }


  //////////////////////////////////////////////////////
  // Create camera for the renders corresponding to the (potentially downsampled) render targets' size
  mRenderDownsampledCamera = CameraActor::New();
  mRenderDownsampledCamera.SetParentOrigin(ParentOrigin::CENTER);


  //////////////////////////////////////////////////////
  // Connect to actor tree
  Self().Add( mChildrenRoot );
  Self().Add( mImageActorHorizBlur );
  Self().Add( mImageActorVertBlur );
  Self().Add( mRenderDownsampledCamera );
}


/**
 * ZrelativeToYconstraint
 *
 * f(current, property, scale) = Vector3(current.x, current.y, property.y * scale)
 */
struct ZrelativeToYconstraint
{
  ZrelativeToYconstraint( float scale )
    : mScale( scale )
  {}

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& property)
  {
    Vector3 v;

    v.x = current.x;
    v.y = current.y;
    v.z = property.GetVector3().y * mScale;

    return v;
  }

  float mScale;
};

void GaussianBlurView::OnControlSizeSet(const Vector3& targetSize)
{
  mTargetSize = Vector2(targetSize);

  mChildrenRoot.SetSize(targetSize);

  if( !mBlurUserImage )
  {
    mImageActorComposite.SetSize(targetSize);
    mTargetActor.SetSize(targetSize);

    // Children render camera must move when GaussianBlurView object is resized. This is since we cannot change render target size - so we need to remap the child actors' rendering
    // accordingly so they still exactly fill the render target. Note that this means the effective resolution of the child render changes as the GaussianBlurView object changes
    // size, this is the trade off for not being able to modify render target size
    // Change camera z position based on GaussianBlurView actor height
    float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
    mRenderFullSizeCamera.SetZ(mTargetSize.height * cameraPosConstraintScale);
  }


  // if we are already on stage, need to update render target sizes now to reflect the new size of this actor
  if(Self().OnStage())
  {
    AllocateResources();
  }
}

void GaussianBlurView::AllocateResources()
{
  // size of render targets etc is based on the size of this actor, ignoring z
  if(mTargetSize != mLastSize)
  {
    mLastSize = mTargetSize;

    // get size of downsampled render targets
    mDownsampledWidth = mTargetSize.width * mDownsampleWidthScale;
    mDownsampledHeight = mTargetSize.height * mDownsampleHeightScale;

    // Create and place a camera for the renders corresponding to the (potentially downsampled) render targets' size
    mRenderDownsampledCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
    mRenderDownsampledCamera.SetNearClippingPlane(1.0f);
    mRenderDownsampledCamera.SetAspectRatio(mDownsampledWidth / mDownsampledHeight);
    mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
    // Point the camera back into the scene
    mRenderDownsampledCamera.SetRotation(Quaternion(M_PI, Vector3::YAXIS));

    mRenderDownsampledCamera.SetPosition(0.0f, 0.0f, ((mDownsampledHeight * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f)));

    // setup for normal operation
    if(!mBlurUserImage)
    {
      // Create and place a camera for the children render, corresponding to its render target size
      mRenderFullSizeCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
      // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
      mRenderFullSizeCamera.SetNearClippingPlane(1.0f);
      mRenderFullSizeCamera.SetAspectRatio(mTargetSize.width / mTargetSize.height);
      mRenderFullSizeCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
      // Point the camera back into the scene
      mRenderFullSizeCamera.SetRotation(Quaternion(M_PI, Vector3::YAXIS));

      float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
      mRenderFullSizeCamera.SetPosition(0.0f, 0.0f, mTargetSize.height * cameraPosConstraintScale);

      // create offscreen buffer of new size to render our child actors to
      mRenderTargetForRenderingChildren = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Dali::Image::UNUSED );

      // Set ImageActor for performing a horizontal blur on the texture
      mImageActorHorizBlur.SetImage( mRenderTargetForRenderingChildren );

      // Create offscreen buffer for vert blur pass
      mRenderTarget1 = FrameBufferImage::New( mDownsampledWidth, mDownsampledHeight, mPixelFormat, Dali::Image::UNUSED );

      // use the completed blur in the first buffer and composite with the original child actors render
      mImageActorComposite.SetImage( mRenderTarget1 );

      // set up target actor for rendering result, i.e. the blurred image
      mTargetActor.SetImage(mRenderTargetForRenderingChildren);
    }

    // Create offscreen buffer for horiz blur pass
    mRenderTarget2 = FrameBufferImage::New( mDownsampledWidth, mDownsampledHeight, mPixelFormat, Dali::Image::UNUSED );

    // size needs to match render target
    mImageActorHorizBlur.SetSize(mDownsampledWidth, mDownsampledHeight);

    // size needs to match render target
    mImageActorVertBlur.SetImage( mRenderTarget2 );
    mImageActorVertBlur.SetSize(mDownsampledWidth, mDownsampledHeight);

    // set gaussian blur up for new sized render targets
    SetShaderConstants();
  }
}

void GaussianBlurView::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  if(!mBlurUserImage)
  {
    // create render task to render our child actors to offscreen buffer
    mRenderChildrenTask = taskList.CreateTask();
    mRenderChildrenTask.SetSourceActor( mChildrenRoot );
    mRenderChildrenTask.SetExclusive(true);
    mRenderChildrenTask.SetInputEnabled( false );
    mRenderChildrenTask.SetClearEnabled( true );
    mRenderChildrenTask.SetClearColor( mBackgroundColor );

    mRenderChildrenTask.SetCameraActor(mRenderFullSizeCamera);
    mRenderChildrenTask.SetTargetFrameBuffer( mRenderTargetForRenderingChildren );
  }

  // perform a horizontal blur targeting the second buffer
  mHorizBlurTask = taskList.CreateTask();
  mHorizBlurTask.SetSourceActor( mImageActorHorizBlur );
  mHorizBlurTask.SetExclusive(true);
  mHorizBlurTask.SetInputEnabled( false );
  mHorizBlurTask.SetClearEnabled( true );
  mHorizBlurTask.SetClearColor( mBackgroundColor );
  if( mRenderOnce && mBlurUserImage )
  {
    mHorizBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
  }

  // use the second buffer and perform a horizontal blur targeting the first buffer
  mVertBlurTask = taskList.CreateTask();
  mVertBlurTask.SetSourceActor( mImageActorVertBlur );
  mVertBlurTask.SetExclusive(true);
  mVertBlurTask.SetInputEnabled( false );
  mVertBlurTask.SetClearEnabled( true );
  mVertBlurTask.SetClearColor( mBackgroundColor );
  if( mRenderOnce && mBlurUserImage )
  {
    mVertBlurTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    mVertBlurTask.FinishedSignal().Connect( this, &GaussianBlurView::OnRenderTaskFinished );
  }

  // use the completed blur in the first buffer and composite with the original child actors render
  if(!mBlurUserImage)
  {
    mCompositeTask = taskList.CreateTask();
    mCompositeTask.SetSourceActor( mImageActorComposite );
    mCompositeTask.SetExclusive(true);
    mCompositeTask.SetInputEnabled( false );

    mCompositeTask.SetCameraActor(mRenderFullSizeCamera);
    mCompositeTask.SetTargetFrameBuffer( mRenderTargetForRenderingChildren );
  }

  mHorizBlurTask.SetCameraActor(mRenderDownsampledCamera);
  mVertBlurTask.SetCameraActor(mRenderDownsampledCamera);

  mHorizBlurTask.SetTargetFrameBuffer( mRenderTarget2 );
  if(mUserOutputRenderTarget)
  {
    mVertBlurTask.SetTargetFrameBuffer( mUserOutputRenderTarget );
  }
  else
  {
    mVertBlurTask.SetTargetFrameBuffer( mRenderTarget1 );
  }
}

void GaussianBlurView::RemoveRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  taskList.RemoveTask(mRenderChildrenTask);
  taskList.RemoveTask(mHorizBlurTask);
  taskList.RemoveTask(mVertBlurTask);
  taskList.RemoveTask(mCompositeTask);
}

void GaussianBlurView::OnStageDisconnection()
{
  // TODO: can't call this here, since SetImage() calls fails similarly to above
  // Need to fix the stage connection so this callback can be used arbitrarily. At that point we  can simplify the API by removing the need for Activate() / Deactivate()
  //Deactivate();
}

void GaussianBlurView::OnControlStageConnection()
{
  // TODO: can't call this here, since SetImage() calls fail to connect images to stage, since parent chain not fully on stage yet
  // Need to fix the stage connection so this callback can be used arbitrarily. At that point we  can simplify the API by removing the need for Activate() / Deactivate()
  //Activate();
}

void GaussianBlurView::Activate()
{
  // make sure resources are allocated and start the render tasks processing
  AllocateResources();
  CreateRenderTasks();
}

void GaussianBlurView::ActivateOnce()
{
  DALI_ASSERT_ALWAYS(mBlurUserImage); // Only works with blurring image mode.
  mRenderOnce = true;
  Activate();
}

void GaussianBlurView::Deactivate()
{
  // stop render tasks processing
  // Note: render target resources are automatically freed since we set the Image::Unused flag
  RemoveRenderTasks();
  mRenderOnce = false;
}

void GaussianBlurView::SetBlurBellCurveWidth(float blurBellCurveWidth)
{
  // a value of zero leads to undefined Gaussian weights, do not allow user to do this
  mBlurBellCurveWidth = std::max( blurBellCurveWidth, 0.001f );
}

float GaussianBlurView::CalcGaussianWeight(float x)
{
  return (1.0f / sqrt(2.0f * Math::PI * mBlurBellCurveWidth)) * exp(-(x * x) / (2.0f * mBlurBellCurveWidth * mBlurBellCurveWidth));
}

void GaussianBlurView::SetShaderConstants()
{
  Vector2 *uvOffsets;
  float ofs;
  float *weights;
  float w, totalWeights;
  unsigned int i;

  uvOffsets = new Vector2[mNumSamples + 1];
  weights = new float[mNumSamples + 1];

  totalWeights = weights[0] = CalcGaussianWeight(0);
  uvOffsets[0].x = 0.0f;
  uvOffsets[0].y = 0.0f;

  for(i=0; i<mNumSamples >> 1; i++)
  {
    w = CalcGaussianWeight((float)(i + 1));
    weights[(i << 1) + 1] = w;
    weights[(i << 1) + 2] = w;
    totalWeights += w * 2.0f;

    // offset texture lookup to between texels, that way the bilinear filter in the texture hardware will average two samples with one lookup
    ofs = ((float)(i << 1)) + 1.5f;

    // get offsets from units of pixels into uv coordinates in [0..1]
    float ofsX = ofs / mDownsampledWidth;
    float ofsY = ofs / mDownsampledHeight;
    uvOffsets[(i << 1) + 1].x = ofsX;
    uvOffsets[(i << 1) + 1].y = ofsY;

    uvOffsets[(i << 1) + 2].x = -ofsX;
    uvOffsets[(i << 1) + 2].y = -ofsY;
  }

  for(i=0; i<mNumSamples; i++)
  {
    weights[i] /= totalWeights;
  }

  // set shader constants
  Vector2 xAxis(1.0f, 0.0f);
  Vector2 yAxis(0.0f, 1.0f);
  for (i = 0; i < mNumSamples; ++i )
  {
    mHorizBlurShader.SetUniform( GetSampleOffsetsPropertyName( i ), uvOffsets[ i ] * xAxis );
    mHorizBlurShader.SetUniform( GetSampleWeightsPropertyName( i ), weights[ i ] );

    mVertBlurShader.SetUniform( GetSampleOffsetsPropertyName( i ), uvOffsets[ i ] * yAxis );
    mVertBlurShader.SetUniform( GetSampleWeightsPropertyName( i ), weights[ i ] );
  }

  delete[] uvOffsets;
  delete[] weights;
}

std::string GaussianBlurView::GetSampleOffsetsPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumSamples );

  std::ostringstream oss;
  oss << "uSampleOffsets[" << index << "]";
  return oss.str();
}

std::string GaussianBlurView::GetSampleWeightsPropertyName( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mNumSamples );

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
  Toolkit::GaussianBlurView handle( GetOwner() );
  mFinishedSignal.Emit( handle );
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
