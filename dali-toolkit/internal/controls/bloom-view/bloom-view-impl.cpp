/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "bloom-view-impl.h"

// EXTERNAL INCLUDES
#include <sstream>
#include <iomanip>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/images/texture-set-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/controls/bloom-view/bloom-view.h>
#include <dali-toolkit/internal/controls/gaussian-blur-view/gaussian-blur-view-impl.h>

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
  return Toolkit::BloomView::New();
}

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::BloomView, Toolkit::Control, Create )
DALI_TYPE_REGISTRATION_END()

// default parameters
const float BLOOM_THRESHOLD_DEFAULT = 0.25f;
const float BLOOM_BLUR_STRENGTH_DEFAULT = 1.0f;
const float BLOOM_INTENSITY_DEFAULT = 1.0f;
const float IMAGE_INTENSITY_DEFAULT = 1.0f;
const float BLOOM_SATURATION_DEFAULT = 1.0f;
const float IMAGE_SATURATION_DEFAULT = 1.0f;

// gaussian blur
const unsigned int BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES = 5;
const float BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH = 1.5f;
const Pixel::Format BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT = Pixel::RGBA8888;
const float BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_FADE_IN = 1.0f;                                       // default, fully blurred
const float BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE = 0.5f;
const float BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE = 0.5f;

const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

const char* const BLOOM_BLUR_STRENGTH_PROPERTY_NAME = "BlurStrengthProperty";
const char* const BLOOM_THRESHOLD_PROPERTY_NAME = "uBloomThreshold";
const char* const RECIP_ONE_MINUS_BLOOM_THRESHOLD_PROPERTY_NAME = "uRecipOneMinusBloomThreshold";
const char* const BLOOM_INTENSITY_PROPERTY_NAME = "uBloomIntensity";
const char* const BLOOM_SATURATION_PROPERTY_NAME = "uBloomSaturation";
const char* const IMAGE_INTENSITY_PROPERTY_NAME = "uImageIntensity";
const char* const IMAGE_SATURATION_PROPERTY_NAME = "uImageSaturation";

///////////////////////////////////////////////////////
//
// Bloom shaders
//

const char* const BLOOM_EXTRACT_FRAGMENT_SOURCE =
  "varying mediump vec2 vTexCoord;\n"
  "uniform sampler2D sTexture;\n"
  "uniform lowp vec4 uColor;\n"
  "uniform mediump float uBloomThreshold;\n"
  "uniform mediump float uRecipOneMinusBloomThreshold;\n"
  "void main()\n"
  "{\n"
  "  mediump vec4 col;\n"
  "  col = texture2D(sTexture, vTexCoord);\n"
  "  col = (col - uBloomThreshold) * uRecipOneMinusBloomThreshold;\n" // remove intensities lower than the thresold and remap intensities above the threshold to [0..1]
  "  gl_FragColor = clamp(col, 0.0, 1.0);\n"
  "}\n";

const char* const COMPOSITE_FRAGMENT_SOURCE =
  "precision mediump float;\n"
  "varying mediump vec2 vTexCoord;\n"
  "uniform sampler2D sTexture;\n"
  "uniform sampler2D sEffect;\n"
  "uniform lowp vec4 uColor;\n"
  "uniform float uBloomIntensity;\n"
  "uniform float uImageIntensity;\n"
  "uniform float uBloomSaturation;\n"
  "uniform float uImageSaturation;\n"

  "vec4 ChangeSaturation(vec4 col, float sat)\n"
  "{\n"
  "  float grey = dot(col.rgb, vec3(0.3, 0.6, 0.1));\n"
  "  return mix(vec4(grey, grey, grey, 1.0), col, sat);\n"
  "}\n"

  "void main()\n"
  "{\n"
  "  mediump vec4 image;\n"
  "  mediump vec4 bloom;\n"
  "  image = texture2D(sTexture, vTexCoord);\n"
  "  bloom = texture2D(sEffect, vTexCoord);\n"
  "  image = ChangeSaturation(image, uImageSaturation) * uImageIntensity;\n"
  "  bloom = ChangeSaturation(bloom, uBloomSaturation) * uBloomIntensity;\n"
  "  image *= 1.0 - clamp(bloom, 0.0, 1.0);\n" // darken base where bloom is strong, to prevent excessive burn-out of result
  "  gl_FragColor = image + bloom;\n"
  "}\n";

} // namespace



BloomView::BloomView()
  : Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) )
  , mBlurNumSamples(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES)
  , mBlurBellCurveWidth(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH)
  , mPixelFormat(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT)
  , mDownsampleWidthScale(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE)
  , mDownsampleHeightScale(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE)
  , mDownsampledWidth( 0.0f )
  , mDownsampledHeight( 0.0f )
  , mTargetSize(Vector2::ZERO)
  , mLastSize(Vector2::ZERO)
  , mChildrenRoot(Actor::New())
  , mInternalRoot(Actor::New() )
  , mBloomThresholdPropertyIndex(Property::INVALID_INDEX)
  , mBlurStrengthPropertyIndex(Property::INVALID_INDEX)
  , mBloomIntensityPropertyIndex(Property::INVALID_INDEX)
  , mBloomSaturationPropertyIndex(Property::INVALID_INDEX)
  , mImageIntensityPropertyIndex(Property::INVALID_INDEX)
  , mImageSaturationPropertyIndex(Property::INVALID_INDEX)
  , mActivated( false )
{
}

BloomView::BloomView( const unsigned int blurNumSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                    const float downsampleWidthScale, const float downsampleHeightScale)
  : Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) )
  , mBlurNumSamples(blurNumSamples)
  , mBlurBellCurveWidth(blurBellCurveWidth)
  , mPixelFormat(renderTargetPixelFormat)
  , mDownsampleWidthScale(downsampleWidthScale)
  , mDownsampleHeightScale(downsampleHeightScale)
  , mDownsampledWidth( 0.0f )
  , mDownsampledHeight( 0.0f )
  , mTargetSize(Vector2::ZERO)
  , mLastSize(Vector2::ZERO)
  , mChildrenRoot(Actor::New())
  , mInternalRoot(Actor::New())
  , mBloomThresholdPropertyIndex(Property::INVALID_INDEX)
  , mBlurStrengthPropertyIndex(Property::INVALID_INDEX)
  , mBloomIntensityPropertyIndex(Property::INVALID_INDEX)
  , mBloomSaturationPropertyIndex(Property::INVALID_INDEX)
  , mImageIntensityPropertyIndex(Property::INVALID_INDEX)
  , mImageSaturationPropertyIndex(Property::INVALID_INDEX)
  , mActivated( false )
{
}

BloomView::~BloomView()
{
}

Toolkit::BloomView BloomView::New()
{
  BloomView* impl = new BloomView();

  Dali::Toolkit::BloomView handle = Dali::Toolkit::BloomView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

Toolkit::BloomView BloomView::New(const unsigned int blurNumSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                                const float downsampleWidthScale, const float downsampleHeightScale)
{
  BloomView* impl = new BloomView( blurNumSamples, blurBellCurveWidth, renderTargetPixelFormat, downsampleWidthScale, downsampleHeightScale);

  Dali::Toolkit::BloomView handle = Dali::Toolkit::BloomView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void BloomView::OnInitialize()
{
  // root actor to parent all user added actors, needed to allow us to set that subtree as exclusive for our child render task
  mChildrenRoot.SetParentOrigin( ParentOrigin::CENTER );
  mInternalRoot.SetParentOrigin( ParentOrigin::CENTER );

  //////////////////////////////////////////////////////
  // Create actors

  // Create an image view for rendering from the scene texture to the bloom texture
  mBloomExtractImageView = Toolkit::ImageView::New();
  mBloomExtractImageView.SetParentOrigin( ParentOrigin::CENTER );

  // Create an image view for compositing the result (scene and bloom textures) to output
  mCompositeImageView = Toolkit::ImageView::New();
  mCompositeImageView.SetParentOrigin( ParentOrigin::CENTER );

  // Create an image view for holding final result, i.e. the blurred image. This will get rendered to screen later, via default / user render task
  mTargetImageView = Toolkit::ImageView::New();
  mTargetImageView.SetParentOrigin( ParentOrigin::CENTER );

  // Create the Gaussian Blur object + render tasks
  // Note that we use mBloomExtractTarget as the source image and also re-use this as the gaussian blur final render target. This saves the gaussian blur code from creating it
  // render targets etc internally, so we make better use of resources
  // Note, this also internally creates the render tasks used by the Gaussian blur, this must occur after the bloom extraction and before the compositing
  mGaussianBlurView = Dali::Toolkit::GaussianBlurView::New(mBlurNumSamples, mBlurBellCurveWidth, mPixelFormat, mDownsampleWidthScale, mDownsampleHeightScale, true);
  mGaussianBlurView.SetParentOrigin( ParentOrigin::CENTER );


  //////////////////////////////////////////////////////
  // Create cameras for the renders corresponding to the (potentially downsampled) render targets' size
  mRenderDownsampledCamera = CameraActor::New();
  mRenderDownsampledCamera.SetParentOrigin(ParentOrigin::CENTER);
  mRenderDownsampledCamera.SetInvertYAxis( true );

  mRenderFullSizeCamera = CameraActor::New();
  mRenderFullSizeCamera.SetParentOrigin(ParentOrigin::CENTER);
  mRenderFullSizeCamera.SetInvertYAxis( true );


  ////////////////////////////////
  // Connect to actor tree
  Self().Add( mChildrenRoot );
  Self().Add( mInternalRoot );
  mInternalRoot.Add( mBloomExtractImageView );
  mInternalRoot.Add( mGaussianBlurView );
  mInternalRoot.Add( mCompositeImageView );
  mInternalRoot.Add( mTargetImageView );
  mInternalRoot.Add( mRenderDownsampledCamera );
  mInternalRoot.Add( mRenderFullSizeCamera );

  // bind properties for / set shader constants to defaults
  SetupProperties();
}

void BloomView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet( targetSize );

  mTargetSize = Vector2(targetSize);
  mChildrenRoot.SetSize(targetSize);
  mCompositeImageView.SetSize(targetSize);
  mTargetImageView.SetSize(targetSize);

  // Children render camera must move when GaussianBlurView object is
  // resized. This is since we cannot change render target size - so we need
  // to remap the child actors' rendering accordingly so they still exactly
  // fill the render target. Note that this means the effective resolution of
  // the child render changes as the GaussianBlurView object changes size,
  // this is the trade off for not being able to modify render target size
  // Change camera z position based on GaussianBlurView actor height
  float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
  mRenderFullSizeCamera.SetZ( mTargetSize.height * cameraPosConstraintScale);

  // if we have already activated the blur, need to update render target sizes now to reflect the new size of this actor
  if(mActivated)
  {
    Deactivate();
    Activate();
  }
}

void BloomView::OnChildAdd( Actor& child )
{
  Control::OnChildAdd( child );

  if( child != mChildrenRoot && child != mInternalRoot)
  {
    mChildrenRoot.Add( child );
  }
}

void BloomView::OnChildRemove( Actor& child )
{
  mChildrenRoot.Remove( child );

  Control::OnChildRemove( child );
}

void BloomView::AllocateResources()
{
  // size of render targets etc is based on the size of this actor, ignoring z
  if(mTargetSize != mLastSize || !mActivated)
  {
    mLastSize = mTargetSize;

    // get size of downsampled render targets
    mDownsampledWidth = mTargetSize.width * mDownsampleWidthScale;
    mDownsampledHeight = mTargetSize.height * mDownsampleHeightScale;


    //////////////////////////////////////////////////////
    // Create cameras

    // Create and place a camera for the renders corresponding to the (potentially downsampled) render targets' size
    mRenderDownsampledCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
    mRenderDownsampledCamera.SetNearClippingPlane(1.0f);
    mRenderDownsampledCamera.SetAspectRatio(mDownsampledWidth / mDownsampledHeight);
    mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor

    mRenderDownsampledCamera.SetPosition(0.0f, 0.0f, ((mDownsampledHeight * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f)));

    // Create and place a camera for the children render, corresponding to its render target size
    mRenderFullSizeCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    // TODO: how do we pick a reasonable value for near clip? Needs to relate to normal camera the user renders with, but we don't have a handle on it
    mRenderFullSizeCamera.SetNearClippingPlane(1.0f);
    mRenderFullSizeCamera.SetAspectRatio(mTargetSize.width / mTargetSize.height);
    mRenderFullSizeCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor

    float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
    mRenderFullSizeCamera.SetPosition(0.0f, 0.0f, mTargetSize.height * cameraPosConstraintScale);

    //////////////////////////////////////////////////////
    // Pass size change onto GaussianBlurView, so it matches
    mGaussianBlurView.SetSize(mTargetSize);
    GetImpl(mGaussianBlurView).AllocateResources();


    //////////////////////////////////////////////////////
    // Create render targets

    // create off screen buffer of new size to render our child actors to
    mRenderTargetForRenderingChildren = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat );
    mBloomExtractTarget = FrameBufferImage::New( mDownsampledWidth, mDownsampledHeight, mPixelFormat );
    FrameBufferImage mBlurExtractTarget = FrameBufferImage::New( mDownsampledWidth, mDownsampledHeight, mPixelFormat );
    mOutputRenderTarget = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat );


    //////////////////////////////////////////////////////
    // Point actors and render tasks at new render targets

    mBloomExtractImageView.SetImage( mRenderTargetForRenderingChildren );
    mBloomExtractImageView.SetSize(mDownsampledWidth, mDownsampledHeight); // size needs to match render target
    // Create shader used for extracting the bright parts of an image
    Property::Map customShader;
    customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = BLOOM_EXTRACT_FRAGMENT_SOURCE;
    Property::Map visualMap;
    visualMap.Insert( Toolkit::Visual::Property::SHADER, customShader );
    mBloomExtractImageView.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );

    // set GaussianBlurView to blur our extracted bloom
    mGaussianBlurView.SetUserImageAndOutputRenderTarget(mBloomExtractTarget, mBlurExtractTarget);

    // use the completed blur in the first buffer and composite with the original child actors render
    mCompositeImageView.SetImage( mRenderTargetForRenderingChildren );
    // Create shader used to composite bloom and original image to output render target
    customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = COMPOSITE_FRAGMENT_SOURCE;
    visualMap[ Toolkit::Visual::Property::SHADER ] = customShader;
    mCompositeImageView.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );
    TextureSet textureSet = mCompositeImageView.GetRendererAt(0).GetTextures();
    TextureSetImage( textureSet, 1u, mBlurExtractTarget );

    // set up target actor for rendering result, i.e. the blurred image
    mTargetImageView.SetImage(mOutputRenderTarget);
  }
}

void BloomView::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // create render task to render our child actors to offscreen buffer
  mRenderChildrenTask = taskList.CreateTask();
  mRenderChildrenTask.SetSourceActor( mChildrenRoot );
  mRenderChildrenTask.SetExclusive(true);
  mRenderChildrenTask.SetInputEnabled( false );
  mRenderChildrenTask.SetClearEnabled( true );
  mRenderChildrenTask.SetCameraActor(mRenderFullSizeCamera); // use camera that covers render target exactly
  mRenderChildrenTask.SetTargetFrameBuffer( mRenderTargetForRenderingChildren );

  // Extract the bright part of the image and render to a new buffer. Downsampling also occurs at this stage to save pixel fill, if it is set up.
  mBloomExtractTask = taskList.CreateTask();
  mBloomExtractTask.SetSourceActor( mBloomExtractImageView );
  mBloomExtractTask.SetExclusive(true);
  mBloomExtractTask.SetInputEnabled( false );
  mBloomExtractTask.SetClearEnabled( true );
  mBloomExtractTask.SetCameraActor(mRenderDownsampledCamera);
  mBloomExtractTask.SetTargetFrameBuffer( mBloomExtractTarget );

  // GaussianBlurView tasks must be created here, so they are executed in the correct order with respect to BloomView tasks
  GetImpl(mGaussianBlurView).CreateRenderTasks();

  // Use an image view displaying the children render and composite it with the blurred bloom buffer, targeting the output
  mCompositeTask = taskList.CreateTask();
  mCompositeTask.SetSourceActor( mCompositeImageView );
  mCompositeTask.SetExclusive(true);
  mCompositeTask.SetInputEnabled( false );
  mCompositeTask.SetClearEnabled( true );
  mCompositeTask.SetCameraActor(mRenderFullSizeCamera);
  mCompositeTask.SetTargetFrameBuffer( mOutputRenderTarget );
}

void BloomView::RemoveRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  taskList.RemoveTask(mRenderChildrenTask);
  taskList.RemoveTask(mBloomExtractTask);

  GetImpl(mGaussianBlurView).RemoveRenderTasks();

  taskList.RemoveTask(mCompositeTask);
}

void BloomView::Activate()
{
  // make sure resources are allocated and start the render tasks processing
  AllocateResources();
  CreateRenderTasks();
  mActivated = true;
}

void BloomView::Deactivate()
{
  // stop render tasks processing
  // Note: render target resources are automatically freed since we set the Image::Unused flag
  RemoveRenderTasks();
  mRenderTargetForRenderingChildren.Reset();
  mBloomExtractTarget.Reset();
  mOutputRenderTarget.Reset();
  mActivated = false;
}

/**
 * RecipOneMinusConstraint
 *
 * f(current, property) = property
 */
struct RecipOneMinusConstraint
{
  RecipOneMinusConstraint(){}

  void operator()( float& current, const PropertyInputContainer& inputs )
  {
    current = 1.0f / ( 1.0f - inputs[0]->GetFloat() );
  }
};

// create properties and constraints to tie internal shader etc settings to BloomView object. User can therefore animate / set them via BloomView object without knowing about
// internal implementation classes
void BloomView::SetupProperties()
{
  CustomActor self = Self();


  ///////////////////////////////////////////
  // bloom threshold

  // set defaults, makes sure properties are registered with shader
  mBloomExtractImageView.RegisterProperty( BLOOM_THRESHOLD_PROPERTY_NAME, BLOOM_THRESHOLD_DEFAULT );
  mBloomExtractImageView.RegisterProperty( RECIP_ONE_MINUS_BLOOM_THRESHOLD_PROPERTY_NAME, 1.0f / (1.0f - BLOOM_THRESHOLD_DEFAULT) );

  // Register a property that the user can control to change the bloom threshold
  mBloomThresholdPropertyIndex = self.RegisterProperty(BLOOM_THRESHOLD_PROPERTY_NAME, BLOOM_THRESHOLD_DEFAULT);
  Property::Index shaderBloomThresholdPropertyIndex = mBloomExtractImageView.GetPropertyIndex(BLOOM_THRESHOLD_PROPERTY_NAME);
  Constraint bloomThresholdConstraint = Constraint::New<float>( mBloomExtractImageView, shaderBloomThresholdPropertyIndex, EqualToConstraint());
  bloomThresholdConstraint.AddSource( Source(self, mBloomThresholdPropertyIndex) );
  bloomThresholdConstraint.Apply();

  // precalc 1.0 / (1.0 - threshold) on CPU to save shader insns, using constraint to tie to the normal threshold property
  Property::Index shaderRecipOneMinusBloomThresholdPropertyIndex = mBloomExtractImageView.GetPropertyIndex(RECIP_ONE_MINUS_BLOOM_THRESHOLD_PROPERTY_NAME);
  Constraint thresholdConstraint = Constraint::New<float>( mBloomExtractImageView, shaderRecipOneMinusBloomThresholdPropertyIndex, RecipOneMinusConstraint());
  thresholdConstraint.AddSource( LocalSource(shaderBloomThresholdPropertyIndex) );
  thresholdConstraint.Apply();


  ////////////////////////////////////////////
  // bloom strength

  // Register a property that the user can control to fade the blur in / out via internal GaussianBlurView object
  mBlurStrengthPropertyIndex = self.RegisterProperty(BLOOM_BLUR_STRENGTH_PROPERTY_NAME, BLOOM_BLUR_STRENGTH_DEFAULT);
  Constraint blurStrengthConstraint = Constraint::New<float>( mGaussianBlurView, mGaussianBlurView.GetBlurStrengthPropertyIndex(), EqualToConstraint());
  blurStrengthConstraint.AddSource( Source(self, mBlurStrengthPropertyIndex) );
  blurStrengthConstraint.Apply();


  ////////////////////////////////////////////
  // bloom intensity

  // Register a property that the user can control to fade the bloom intensity via internally hidden shader
  mBloomIntensityPropertyIndex = self.RegisterProperty(BLOOM_INTENSITY_PROPERTY_NAME, BLOOM_INTENSITY_DEFAULT);
  mCompositeImageView.RegisterProperty( BLOOM_INTENSITY_PROPERTY_NAME, BLOOM_INTENSITY_DEFAULT );
  Property::Index shaderBloomIntensityPropertyIndex = mCompositeImageView.GetPropertyIndex(BLOOM_INTENSITY_PROPERTY_NAME);
  Constraint bloomIntensityConstraint = Constraint::New<float>( mCompositeImageView, shaderBloomIntensityPropertyIndex, EqualToConstraint());
  bloomIntensityConstraint.AddSource( Source(self, mBloomIntensityPropertyIndex) );
  bloomIntensityConstraint.Apply();


  ////////////////////////////////////////////
  // bloom saturation

  // Register a property that the user can control to fade the bloom saturation via internally hidden shader
  mBloomSaturationPropertyIndex = self.RegisterProperty(BLOOM_SATURATION_PROPERTY_NAME, BLOOM_SATURATION_DEFAULT);
  mCompositeImageView.RegisterProperty( BLOOM_SATURATION_PROPERTY_NAME, BLOOM_SATURATION_DEFAULT );
  Property::Index shaderBloomSaturationPropertyIndex = mCompositeImageView.GetPropertyIndex(BLOOM_SATURATION_PROPERTY_NAME);
  Constraint bloomSaturationConstraint = Constraint::New<float>( mCompositeImageView, shaderBloomSaturationPropertyIndex, EqualToConstraint());
  bloomSaturationConstraint.AddSource( Source(self, mBloomSaturationPropertyIndex) );
  bloomSaturationConstraint.Apply();


  ////////////////////////////////////////////
  // image intensity

  // Register a property that the user can control to fade the image intensity via internally hidden shader
  mImageIntensityPropertyIndex = self.RegisterProperty(IMAGE_INTENSITY_PROPERTY_NAME, IMAGE_INTENSITY_DEFAULT);
  mCompositeImageView.RegisterProperty( IMAGE_INTENSITY_PROPERTY_NAME, IMAGE_INTENSITY_DEFAULT );
  Property::Index shaderImageIntensityPropertyIndex = mCompositeImageView.GetPropertyIndex(IMAGE_INTENSITY_PROPERTY_NAME);
  Constraint imageIntensityConstraint = Constraint::New<float>( mCompositeImageView, shaderImageIntensityPropertyIndex, EqualToConstraint());
  imageIntensityConstraint.AddSource( Source(self, mImageIntensityPropertyIndex) );
  imageIntensityConstraint.Apply();


  ////////////////////////////////////////////
  // image saturation

  // Register a property that the user can control to fade the image saturation via internally hidden shader
  mImageSaturationPropertyIndex = self.RegisterProperty(IMAGE_SATURATION_PROPERTY_NAME, IMAGE_SATURATION_DEFAULT);
  mCompositeImageView.RegisterProperty( IMAGE_SATURATION_PROPERTY_NAME, IMAGE_SATURATION_DEFAULT );
  Property::Index shaderImageSaturationPropertyIndex = mCompositeImageView.GetPropertyIndex(IMAGE_SATURATION_PROPERTY_NAME);
  Constraint imageSaturationConstraint = Constraint::New<float>( mCompositeImageView, shaderImageSaturationPropertyIndex, EqualToConstraint());
  imageSaturationConstraint.AddSource( Source(self, mImageSaturationPropertyIndex) );
  imageSaturationConstraint.Apply();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
