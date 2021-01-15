/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/stage.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/devel-api/controls/bloom-view/bloom-view.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/controls/gaussian-blur-view/gaussian-blur-view-impl.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

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

} // namespace



BloomView::BloomView()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mBlurNumSamples(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES),
  mBlurBellCurveWidth(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH),
  mPixelFormat(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT),
  mDownsampleWidthScale(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE),
  mDownsampleHeightScale(BLOOM_GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE),
  mDownsampledWidth( 0.0f ),
  mDownsampledHeight( 0.0f ),
  mTargetSize(Vector2::ZERO),
  mLastSize(Vector2::ZERO),
  mChildrenRoot(Actor::New()),
  mInternalRoot(Actor::New() ),
  mBloomThresholdPropertyIndex(Property::INVALID_INDEX),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mBloomIntensityPropertyIndex(Property::INVALID_INDEX),
  mBloomSaturationPropertyIndex(Property::INVALID_INDEX),
  mImageIntensityPropertyIndex(Property::INVALID_INDEX),
  mImageSaturationPropertyIndex(Property::INVALID_INDEX),
  mActivated( false )
{
}

BloomView::BloomView( const unsigned int blurNumSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                    const float downsampleWidthScale, const float downsampleHeightScale)
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mBlurNumSamples(blurNumSamples),
  mBlurBellCurveWidth(blurBellCurveWidth),
  mPixelFormat(renderTargetPixelFormat),
  mDownsampleWidthScale(downsampleWidthScale),
  mDownsampleHeightScale(downsampleHeightScale),
  mDownsampledWidth( 0.0f ),
  mDownsampledHeight( 0.0f ),
  mTargetSize(Vector2::ZERO),
  mLastSize(Vector2::ZERO),
  mChildrenRoot(Actor::New()),
  mInternalRoot(Actor::New()),
  mBloomThresholdPropertyIndex(Property::INVALID_INDEX),
  mBlurStrengthPropertyIndex(Property::INVALID_INDEX),
  mBloomIntensityPropertyIndex(Property::INVALID_INDEX),
  mBloomSaturationPropertyIndex(Property::INVALID_INDEX),
  mImageIntensityPropertyIndex(Property::INVALID_INDEX),
  mImageSaturationPropertyIndex(Property::INVALID_INDEX),
  mActivated( false )
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
  mChildrenRoot.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mInternalRoot.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  //////////////////////////////////////////////////////
  // Create actors

  // Create an image view for rendering from the scene texture to the bloom texture
  mBloomExtractActor = Actor::New();
  mBloomExtractActor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  // Create an image view for compositing the result (scene and bloom textures) to output
  mCompositeActor = Actor::New();
  mCompositeActor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  // Create an image view for holding final result, i.e. the blurred image. This will get rendered to screen later, via default / user render task
  mTargetActor = Actor::New();
  mTargetActor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  // Create the Gaussian Blur object + render tasks
  // Note that we use mBloomExtractTarget as the source image and also re-use this as the gaussian blur final render target. This saves the gaussian blur code from creating it
  // render targets etc internally, so we make better use of resources
  // Note, this also internally creates the render tasks used by the Gaussian blur, this must occur after the bloom extraction and before the compositing
  mGaussianBlurView = Dali::Toolkit::GaussianBlurView::New(mBlurNumSamples, mBlurBellCurveWidth, mPixelFormat, mDownsampleWidthScale, mDownsampleHeightScale, true);
  mGaussianBlurView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );


  //////////////////////////////////////////////////////
  // Create cameras for the renders corresponding to the (potentially downsampled) render targets' size
  mRenderDownsampledCamera = CameraActor::New();
  mRenderDownsampledCamera.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER );
  mRenderDownsampledCamera.SetInvertYAxis( true );

  mRenderFullSizeCamera = CameraActor::New();
  mRenderFullSizeCamera.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER );
  mRenderFullSizeCamera.SetInvertYAxis( true );


  ////////////////////////////////
  // Connect to actor tree
  Self().Add( mChildrenRoot );
  Self().Add( mInternalRoot );
  mInternalRoot.Add( mBloomExtractActor );
  mInternalRoot.Add( mGaussianBlurView );
  mInternalRoot.Add( mCompositeActor );
  mInternalRoot.Add( mTargetActor );
  mInternalRoot.Add( mRenderDownsampledCamera );
  mInternalRoot.Add( mRenderFullSizeCamera );

  // bind properties for / set shader constants to defaults
  SetupProperties();

  DevelControl::SetAccessibilityConstructor( Self(), []( Dali::Actor actor ) {
    return std::unique_ptr< Dali::Accessibility::Accessible >(
      new Control::Impl::AccessibleImpl( actor, Dali::Accessibility::Role::ANIMATION ) );
  } );
}

void BloomView::OnSizeSet(const Vector3& targetSize)
{
  mTargetSize = Vector2(targetSize);
  mChildrenRoot.SetProperty( Actor::Property::SIZE, targetSize);
  mCompositeActor.SetProperty( Actor::Property::SIZE, targetSize);
  mTargetActor.SetProperty( Actor::Property::SIZE, targetSize);

  // Children render camera must move when GaussianBlurView object is
  // resized. This is since we cannot change render target size - so we need
  // to remap the child actors' rendering accordingly so they still exactly
  // fill the render target. Note that this means the effective resolution of
  // the child render changes as the GaussianBlurView object changes size,
  // this is the trade off for not being able to modify render target size
  // Change camera z position based on GaussianBlurView actor height
  float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
  mRenderFullSizeCamera.SetProperty( Actor::Property::POSITION_Z,  mTargetSize.height * cameraPosConstraintScale);

  // if we have already activated the blur, need to update render target sizes now to reflect the new size of this actor
  if(mActivated)
  {
    Deactivate();
    Activate();
  }

  Control::OnSizeSet( targetSize );
}

void BloomView::OnChildAdd( Actor& child )
{
  if( child != mChildrenRoot && child != mInternalRoot)
  {
    mChildrenRoot.Add( child );
  }

  Control::OnChildAdd( child );
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
    mRenderDownsampledCamera.SetNearClippingPlane(1.0f);
    mRenderDownsampledCamera.SetAspectRatio(mDownsampledWidth / mDownsampledHeight);
    mRenderDownsampledCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor

    mRenderDownsampledCamera.SetProperty( Actor::Property::POSITION, Vector3( 0.0f, 0.0f, ( ( mDownsampledHeight * 0.5f ) / tanf( ARBITRARY_FIELD_OF_VIEW * 0.5f ) ) ) );

    // Create and place a camera for the children render, corresponding to its render target size
    mRenderFullSizeCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    mRenderFullSizeCamera.SetNearClippingPlane(1.0f);
    mRenderFullSizeCamera.SetAspectRatio(mTargetSize.width / mTargetSize.height);
    mRenderFullSizeCamera.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor

    float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
    mRenderFullSizeCamera.SetProperty( Actor::Property::POSITION, Vector3( 0.0f, 0.0f, mTargetSize.height * cameraPosConstraintScale ) );

    //////////////////////////////////////////////////////
    // Pass size change onto GaussianBlurView, so it matches
    mGaussianBlurView.SetProperty( Actor::Property::SIZE, mTargetSize );
    GetImpl(mGaussianBlurView).AllocateResources();

    mGaussianBlurView.SetProperty( Actor::Property::VISIBLE, true );

    //////////////////////////////////////////////////////
    // Create render targets

    // create off screen buffer of new size to render our child actors to
    mRenderTargetForRenderingChildren = FrameBuffer::New( mTargetSize.width, mTargetSize.height, FrameBuffer::Attachment::NONE );
    Texture textureForChildren = Texture::New( TextureType::TEXTURE_2D, mPixelFormat, unsigned(mTargetSize.width), unsigned(mTargetSize.height) );
    mRenderTargetForRenderingChildren.AttachColorTexture( textureForChildren );

    mBloomExtractTarget = FrameBuffer::New( mDownsampledWidth, mDownsampledHeight, FrameBuffer::Attachment::NONE );
    Texture texture = Texture::New( TextureType::TEXTURE_2D, mPixelFormat, unsigned(mDownsampledWidth), unsigned(mDownsampledHeight) );
    mBloomExtractTarget.AttachColorTexture( texture );

    FrameBuffer blurExtractTarget = FrameBuffer::New( mDownsampledWidth, mDownsampledHeight, FrameBuffer::Attachment::NONE );
    texture = Texture::New( TextureType::TEXTURE_2D, mPixelFormat, unsigned(mDownsampledWidth), unsigned(mDownsampledHeight) );
    blurExtractTarget.AttachColorTexture( texture );

    mOutputRenderTarget = FrameBuffer::New( mTargetSize.width, mTargetSize.height, FrameBuffer::Attachment::NONE );
    Texture outputTexture = Texture::New( TextureType::TEXTURE_2D, mPixelFormat, unsigned(mTargetSize.width), unsigned(mTargetSize.height) );
    mOutputRenderTarget.AttachColorTexture( outputTexture );

    //////////////////////////////////////////////////////
    // Point actors and render tasks at new render targets

    Renderer bloomRenderer = CreateRenderer( BASIC_VERTEX_SOURCE, SHADER_BLOOM_VIEW_EXTRACT_SHADER_FRAG );
    SetRendererTexture( bloomRenderer, mRenderTargetForRenderingChildren );
    mBloomExtractActor.AddRenderer( bloomRenderer );
    mBloomExtractActor.SetProperty( Actor::Property::SIZE, Vector2( mDownsampledWidth, mDownsampledHeight ) ); // size needs to match render target

    // set GaussianBlurView to blur our extracted bloom
    mGaussianBlurView.SetUserImageAndOutputRenderTarget( mBloomExtractTarget.GetColorTexture(), blurExtractTarget );

    // use the completed blur in the first buffer and composite with the original child actors render
    Renderer compositeRenderer = CreateRenderer( BASIC_VERTEX_SOURCE, SHADER_BLOOM_VIEW_COMPOSITE_SHADER_FRAG );
    SetRendererTexture( compositeRenderer, mRenderTargetForRenderingChildren );
    TextureSet textureSet = compositeRenderer.GetTextures();
    textureSet.SetTexture( 0u, mRenderTargetForRenderingChildren.GetColorTexture() );
    textureSet.SetTexture( 1u, blurExtractTarget.GetColorTexture() );
    mCompositeActor.AddRenderer( compositeRenderer );

    // set up target actor for rendering result, i.e. the blurred image
    Renderer targetRenderer = CreateRenderer( BASIC_VERTEX_SOURCE, BASIC_FRAGMENT_SOURCE );
    SetRendererTexture( targetRenderer, mOutputRenderTarget );
    mTargetActor.AddRenderer( targetRenderer );
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
  mRenderChildrenTask.SetFrameBuffer( mRenderTargetForRenderingChildren );

  // Extract the bright part of the image and render to a new buffer. Downsampling also occurs at this stage to save pixel fill, if it is set up.
  mBloomExtractTask = taskList.CreateTask();
  mBloomExtractTask.SetSourceActor( mBloomExtractActor );
  mBloomExtractTask.SetExclusive(true);
  mBloomExtractTask.SetInputEnabled( false );
  mBloomExtractTask.SetClearEnabled( true );
  mBloomExtractTask.SetCameraActor(mRenderDownsampledCamera);
  mBloomExtractTask.SetFrameBuffer( mBloomExtractTarget );

  // GaussianBlurView tasks must be created here, so they are executed in the correct order with respect to BloomView tasks
  GetImpl(mGaussianBlurView).CreateRenderTasks();

  // Use an image view displaying the children render and composite it with the blurred bloom buffer, targeting the output
  mCompositeTask = taskList.CreateTask();
  mCompositeTask.SetSourceActor( mCompositeActor );
  mCompositeTask.SetExclusive(true);
  mCompositeTask.SetInputEnabled( false );
  mCompositeTask.SetClearEnabled( true );
  mCompositeTask.SetCameraActor(mRenderFullSizeCamera);
  mCompositeTask.SetFrameBuffer( mOutputRenderTarget );
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

  // Reset children
  mBloomExtractActor.RemoveRenderer( 0u );
  mTargetActor.RemoveRenderer( 0u );
  mCompositeActor.RemoveRenderer( 0u );

  mGaussianBlurView.SetProperty( Actor::Property::VISIBLE, false );

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
  mBloomExtractActor.RegisterProperty( BLOOM_THRESHOLD_PROPERTY_NAME, BLOOM_THRESHOLD_DEFAULT );
  mBloomExtractActor.RegisterProperty( RECIP_ONE_MINUS_BLOOM_THRESHOLD_PROPERTY_NAME, 1.0f / (1.0f - BLOOM_THRESHOLD_DEFAULT) );

  // Register a property that the user can control to change the bloom threshold
  mBloomThresholdPropertyIndex = self.RegisterProperty(BLOOM_THRESHOLD_PROPERTY_NAME, BLOOM_THRESHOLD_DEFAULT);
  Property::Index shaderBloomThresholdPropertyIndex = mBloomExtractActor.GetPropertyIndex(BLOOM_THRESHOLD_PROPERTY_NAME);
  Constraint bloomThresholdConstraint = Constraint::New<float>( mBloomExtractActor, shaderBloomThresholdPropertyIndex, EqualToConstraint());
  bloomThresholdConstraint.AddSource( Source(self, mBloomThresholdPropertyIndex) );
  bloomThresholdConstraint.Apply();

  // precalc 1.0 / (1.0 - threshold) on CPU to save shader insns, using constraint to tie to the normal threshold property
  Property::Index shaderRecipOneMinusBloomThresholdPropertyIndex = mBloomExtractActor.GetPropertyIndex(RECIP_ONE_MINUS_BLOOM_THRESHOLD_PROPERTY_NAME);
  Constraint thresholdConstraint = Constraint::New<float>( mBloomExtractActor, shaderRecipOneMinusBloomThresholdPropertyIndex, RecipOneMinusConstraint());
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
  mCompositeActor.RegisterProperty( BLOOM_INTENSITY_PROPERTY_NAME, BLOOM_INTENSITY_DEFAULT );
  Property::Index shaderBloomIntensityPropertyIndex = mCompositeActor.GetPropertyIndex(BLOOM_INTENSITY_PROPERTY_NAME);
  Constraint bloomIntensityConstraint = Constraint::New<float>( mCompositeActor, shaderBloomIntensityPropertyIndex, EqualToConstraint());
  bloomIntensityConstraint.AddSource( Source(self, mBloomIntensityPropertyIndex) );
  bloomIntensityConstraint.Apply();


  ////////////////////////////////////////////
  // bloom saturation

  // Register a property that the user can control to fade the bloom saturation via internally hidden shader
  mBloomSaturationPropertyIndex = self.RegisterProperty(BLOOM_SATURATION_PROPERTY_NAME, BLOOM_SATURATION_DEFAULT);
  mCompositeActor.RegisterProperty( BLOOM_SATURATION_PROPERTY_NAME, BLOOM_SATURATION_DEFAULT );
  Property::Index shaderBloomSaturationPropertyIndex = mCompositeActor.GetPropertyIndex(BLOOM_SATURATION_PROPERTY_NAME);
  Constraint bloomSaturationConstraint = Constraint::New<float>( mCompositeActor, shaderBloomSaturationPropertyIndex, EqualToConstraint());
  bloomSaturationConstraint.AddSource( Source(self, mBloomSaturationPropertyIndex) );
  bloomSaturationConstraint.Apply();


  ////////////////////////////////////////////
  // image intensity

  // Register a property that the user can control to fade the image intensity via internally hidden shader
  mImageIntensityPropertyIndex = self.RegisterProperty(IMAGE_INTENSITY_PROPERTY_NAME, IMAGE_INTENSITY_DEFAULT);
  mCompositeActor.RegisterProperty( IMAGE_INTENSITY_PROPERTY_NAME, IMAGE_INTENSITY_DEFAULT );
  Property::Index shaderImageIntensityPropertyIndex = mCompositeActor.GetPropertyIndex(IMAGE_INTENSITY_PROPERTY_NAME);
  Constraint imageIntensityConstraint = Constraint::New<float>( mCompositeActor, shaderImageIntensityPropertyIndex, EqualToConstraint());
  imageIntensityConstraint.AddSource( Source(self, mImageIntensityPropertyIndex) );
  imageIntensityConstraint.Apply();


  ////////////////////////////////////////////
  // image saturation

  // Register a property that the user can control to fade the image saturation via internally hidden shader
  mImageSaturationPropertyIndex = self.RegisterProperty(IMAGE_SATURATION_PROPERTY_NAME, IMAGE_SATURATION_DEFAULT);
  mCompositeActor.RegisterProperty( IMAGE_SATURATION_PROPERTY_NAME, IMAGE_SATURATION_DEFAULT );
  Property::Index shaderImageSaturationPropertyIndex = mCompositeActor.GetPropertyIndex(IMAGE_SATURATION_PROPERTY_NAME);
  Constraint imageSaturationConstraint = Constraint::New<float>( mCompositeActor, shaderImageSaturationPropertyIndex, EqualToConstraint());
  imageSaturationConstraint.AddSource( Source(self, mImageSaturationPropertyIndex) );
  imageSaturationConstraint.Apply();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
