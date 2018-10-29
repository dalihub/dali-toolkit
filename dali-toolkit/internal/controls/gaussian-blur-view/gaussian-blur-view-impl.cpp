/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/devel-api/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>

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

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::GaussianBlurView, Toolkit::Control, Create )
DALI_TYPE_REGISTRATION_END()

const unsigned int GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES = 5;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH = 1.5f;
const Pixel::Format GAUSSIAN_BLUR_VIEW_DEFAULT_RENDER_TARGET_PIXEL_FORMAT = Pixel::RGBA8888;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_STRENGTH = 1.0f;                                       // default, fully blurred
const char* const GAUSSIAN_BLUR_VIEW_STRENGTH_PROPERTY_NAME = "GaussianBlurStrengthPropertyName";
const float GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE = 0.5f;
const float GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE = 0.5f;

const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;
} // namespace


GaussianBlurView::GaussianBlurView()
: Control( ControlBehaviour( DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mNumSamples( GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES ),
  mBlurBellCurveWidth( 0.001f ),
  mDownsampleWidthScale( GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_WIDTH_SCALE ),
  mDownsampleHeightScale( GAUSSIAN_BLUR_VIEW_DEFAULT_DOWNSAMPLE_HEIGHT_SCALE ),
  mDownsampledWidth( 0.0f ),
  mDownsampledHeight( 0.0f ),
  mBackgroundColor( Color::BLACK ),
  mTargetSize( Vector2::ZERO ),
  mLastSize( Vector2::ZERO ),
  mChildrenRoot( Actor::New() ),
  mInternalRoot( Actor::New() ),
  mRenderOnce( false ),
  mActivated( false )
{
  SetBlurBellCurveWidth(GAUSSIAN_BLUR_VIEW_DEFAULT_BLUR_BELL_CURVE_WIDTH);
}

GaussianBlurView::GaussianBlurView( const unsigned int numSamples,
                                    const float blurBellCurveWidth,
                                    const float downsampleWidthScale,
                                    const float downsampleHeightScale )
: Control( ControlBehaviour( DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS ) ),
  mNumSamples( GAUSSIAN_BLUR_VIEW_DEFAULT_NUM_SAMPLES ), // @todo: Store numSamples when Specialization Constants working
  mBlurBellCurveWidth( 0.001f ),
  mDownsampleWidthScale(downsampleWidthScale),
  mDownsampleHeightScale(downsampleHeightScale),
  mDownsampledWidth( 0.0f ),
  mDownsampledHeight( 0.0f ),
  mBackgroundColor( Color::BLACK ),
  mTargetSize(Vector2::ZERO),
  mLastSize( Vector2::ZERO ),
  mChildrenRoot(Actor::New()),
  mInternalRoot(Actor::New()),
  mRenderOnce( false ),
  mActivated( false )
{
  SetBlurBellCurveWidth(blurBellCurveWidth);
}

GaussianBlurView::~GaussianBlurView()
{
  RemoveRenderTasks();
  RemoveTextures();
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

Toolkit::GaussianBlurView GaussianBlurView::New( const unsigned int numSamples, const float blurBellCurveWidth,
                                                 const float downsampleWidthScale, const float downsampleHeightScale )
{
  GaussianBlurView* impl = new GaussianBlurView( numSamples, blurBellCurveWidth,
                                                 downsampleWidthScale, downsampleHeightScale );

  Dali::Toolkit::GaussianBlurView handle = Dali::Toolkit::GaussianBlurView( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

Toolkit::ImageView GaussianBlurView::GetBlurredImageView() const
{
  return mImageViewFinal;
}

void GaussianBlurView::SetBackgroundColor( const Vector4& color )
{
  mBackgroundColor = color;
}

Vector4 GaussianBlurView::GetBackgroundColor() const
{
  return mBackgroundColor;
}

void GaussianBlurView::SetBlurBellCurveWidth(float blurBellCurveWidth)
{
  // a value of zero leads to undefined Gaussian weights, do not allow user to do this
  mBlurBellCurveWidth = std::max( blurBellCurveWidth, 0.001f );
}

float GaussianBlurView::GetBlurBellCurveWidth()
{
  return mBlurBellCurveWidth;
}


void GaussianBlurView::Activate()
{
  if( ! mActivated )
  {
    // make sure resources are allocated and start the render tasks processing
    mChildrenRoot.SetVisible(true);
    AllocateResources();
  }
  mActivated = true;
}

void GaussianBlurView::Deactivate()
{
  if( mActivated )
  {
    FreeResources(); // Doesn't free the final image view or the scene child actors
  }
  // There is now no exclusive render task to prevent the scene being rendered to main fb, so make invisible instead.
  mChildrenRoot.SetVisible(false);
  mActivated = false;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void GaussianBlurView::OnInitialize()
{
  // root actor to parent all user added actors, needed to allow us to set that subtree as exclusive for our child render task
  mChildrenRoot.SetParentOrigin(ParentOrigin::CENTER);
  mChildrenRoot.SetName("ChildrenRoot");
  mInternalRoot.SetParentOrigin(ParentOrigin::CENTER);
  mInternalRoot.SetName("InternalRoot");

  //////////////////////////////////////////////////////
  // Create shaders

  Property::Map source;
  Property::Value value;
  Toolkit::EncodeBase64PropertyData( value, GraphicsGetBuiltinShaderId( SHADER_GAUSSIAN_BLUR_SHADER_FRAG ));
  source[Toolkit::Visual::Shader::Property::FRAGMENT_SHADER] = value;
  mCustomShader[ Toolkit::Visual::Property::SHADER ] = source;

  //////////////////////////////////////////////////////
  // Connect to actor tree
  Self().Add( mChildrenRoot );
  Self().Add( mInternalRoot );
}


void GaussianBlurView::OnSizeSet(const Vector3& targetSize)
{
  Vector2 newSize = Vector2(targetSize);

  if( mTargetSize != newSize )
  {
    mTargetSize = newSize;
    mChildrenRoot.SetSize(targetSize);
    mInternalRoot.SetSize(targetSize);

    if( mImageViewScene )
    {
      mImageViewScene.SetSize(targetSize);
    }
    if( mImageViewFinal )
    {
      mImageViewFinal.SetSize(targetSize);
    }

    // Children render camera must move when GaussianBlurView object is resized. This is since we
    // cannot change render target size - so we need to remap the child actors' rendering
    // accordingly so they still exactly fill the render target. Note that this means the effective
    // resolution of the child render changes as the GaussianBlurView object changes size, this is
    // the trade off for not being able to modify render target size Change camera z position based
    // on GaussianBlurView actor height
    if( mRenderFullSizeCamera )
    {
      float cameraPosConstraintScale = 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f);
      mRenderFullSizeCamera.SetZ(mTargetSize.height * cameraPosConstraintScale);
    }

    // if we have already activated the blur, need to update render target sizes now to reflect the
    // new size of this actor
    if( mActivated )
    {
      Deactivate();
      Activate();
    }
  }

  Control::OnSizeSet( targetSize );
}

void GaussianBlurView::OnChildAdd( Actor& child )
{
  if( child != mChildrenRoot && child != mInternalRoot)
  {
    mChildrenRoot.Add( child );
  }

  Control::OnChildAdd( child );
}

void GaussianBlurView::OnChildRemove( Actor& child )
{
  mChildrenRoot.Remove( child );

  Control::OnChildRemove( child );
}

void GaussianBlurView::AllocateResources()
{
  // size of render targets etc is based on the size of this actor, ignoring z
  // get size of downsampled render targets
  DALI_ASSERT_DEBUG( mTargetSize.width > 0 && mTargetSize.height > 0 && "Target size is zero ");

  mDownsampledWidth = mTargetSize.width * mDownsampleWidthScale;
  mDownsampledHeight = mTargetSize.height * mDownsampleHeightScale;
  auto downsampledSize = Vector2(mDownsampledWidth, mDownsampledHeight);

  mRenderDownsampledCamera = CreateCamera( mDownsampledWidth, mDownsampledHeight );
  mRenderFullSizeCamera = CreateCamera( mTargetSize.width, mTargetSize.height );
  mInternalRoot.Add( mRenderDownsampledCamera );
  mInternalRoot.Add( mRenderFullSizeCamera );

  // Set up render of scene
  Property::Map nullShader;
  mSceneOutput = CreateTexture( mTargetSize );
  mSceneOutputUrl = Dali::Toolkit::TextureManager::AddTexture( mSceneOutput );
  mSceneFramebuffer = CreateFramebuffer( mTargetSize, mSceneOutput );
  mRenderSceneTask = CreateRenderTask( mChildrenRoot, mRenderFullSizeCamera, mSceneFramebuffer );
  mImageViewScene = CreateImageView( mTargetSize, nullShader, Direction::HORIZONTAL, mSceneOutputUrl );
  mInternalRoot.Add( mImageViewScene );

  // Set up horizontal render with full size camera to downsized framebuffer to shrink scene
  mHorizontalOutput = CreateTexture( downsampledSize );
  mHorizontalOutputUrl = Dali::Toolkit::TextureManager::AddTexture( mHorizontalOutput );
  mHorizontalBlurFramebuffer = CreateFramebuffer( downsampledSize, mHorizontalOutput );
  mHorizontalBlurTask = CreateRenderTask( mImageViewScene, mRenderFullSizeCamera, mHorizontalBlurFramebuffer );
  mImageViewHorizontalBlur = CreateImageView( downsampledSize, mCustomShader, Direction::HORIZONTAL,
                                              mHorizontalOutputUrl );
  mInternalRoot.Add( mImageViewHorizontalBlur );

  // Setup vertical render with downsized camera to downsized framebuffer (matched sizes)
  mVerticalOutput = CreateTexture( downsampledSize );
  mVerticalOutputUrl = Dali::Toolkit::TextureManager::AddTexture( mVerticalOutput );
  mVerticalBlurFramebuffer = CreateFramebuffer( downsampledSize, mVerticalOutput );
  mVerticalBlurTask = CreateRenderTask( mImageViewHorizontalBlur, mRenderDownsampledCamera, mVerticalBlurFramebuffer );
  mImageViewVerticalBlur = CreateImageView( downsampledSize, mCustomShader, Direction::VERTICAL, mVerticalOutputUrl );
  mInternalRoot.Add( mImageViewVerticalBlur );

  // Setup final render with full sized camera to fullsize framebuffer to grow, reusing scene framebuffer
  mUpsampleTask = CreateRenderTask( mImageViewVerticalBlur, mRenderDownsampledCamera, mSceneFramebuffer );
  if( mImageViewFinal )
  {
    UnparentAndReset(mImageViewFinal);
  }
  mImageViewFinal = CreateImageView( mTargetSize, nullShader, Direction::HORIZONTAL, mSceneOutputUrl );
  mInternalRoot.Add( mImageViewFinal );
}


CameraActor GaussianBlurView::CreateCamera( float width, float height )
{
  auto camera = CameraActor::New(Vector2(width, height));
  camera.SetFieldOfView( ARBITRARY_FIELD_OF_VIEW );
  camera.SetNearClippingPlane( 1.0f );
  camera.SetAspectRatio( width / height );
  camera.SetType( Dali::Camera::FREE_LOOK );
  camera.SetPosition(0.0f, 0.0f, ((height * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f)));
  camera.SetParentOrigin( ParentOrigin::CENTER );
  return camera;
}

Texture GaussianBlurView::CreateTexture( Vector2 size )
{
  return Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888,
                       static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height) );
}

FrameBuffer GaussianBlurView::CreateFramebuffer( Vector2 targetSize, Texture texture )
{
  auto framebuffer = FrameBuffer::New(targetSize.width, targetSize.height, Pixel::RGBA8888);
  framebuffer.AttachColorTexture( texture );
  return framebuffer;
}

RenderTask GaussianBlurView::CreateRenderTask( Actor inputActor, CameraActor cameraActor, FrameBuffer framebuffer )
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  auto renderTask = taskList.CreateTask();
  renderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
  renderTask.SetCameraActor( cameraActor );
  renderTask.SetSourceActor( inputActor );
  renderTask.SetExclusive( true );
  renderTask.SetInputEnabled( false );
  renderTask.SetClearColor( mBackgroundColor );
  renderTask.SetClearEnabled( true );
  renderTask.SetFrameBuffer( framebuffer );
  return renderTask;
}

Toolkit::ImageView GaussianBlurView::CreateImageView( Vector2 targetSize,
                                                      Property::Map& customShader,
                                                      Direction blurDirection,
                                                      const std::string& url )
{
  auto image = Toolkit::ImageView::New( url );

  if( customShader.Count() > 0 )
  {
    image.SetProperty(Toolkit::ImageView::Property::IMAGE, customShader );
    SetShaderConstants( image, targetSize, blurDirection, 5);
  }

  image.SetParentOrigin( ParentOrigin::CENTER );
  image.SetSize( targetSize );
  return image;
}

void GaussianBlurView::FreeResources()
{
  // Frees/unreferences everything except mImageViewFinal.

  if( Stage::IsInstalled() )
  {
    RemoveRenderTasks();
    RemoveTextures();

    // Remove cameras
    UnparentAndReset( mRenderFullSizeCamera );
    UnparentAndReset( mRenderDownsampledCamera );

    // Remove all intermediate image views
    UnparentAndReset( mImageViewScene );
    UnparentAndReset( mImageViewHorizontalBlur );
    UnparentAndReset( mImageViewVerticalBlur );
  }

  mHorizontalOutputUrl.clear();
  mVerticalOutputUrl.clear();

  mHorizontalOutput.Reset();
  mVerticalOutput.Reset();
  mSceneOutput.Reset();

  mRenderOnce = false;
  mActivated = false;
}

void GaussianBlurView::RemoveRenderTasks()
{
  if( Stage::IsInstalled() )
  {
    Stage stage = Stage::GetCurrent();
    RenderTaskList taskList = stage.GetRenderTaskList();

    CameraActor null;
    if( mRenderSceneTask )
    {
      mRenderSceneTask.SetCameraActor(null);
      taskList.RemoveTask(mRenderSceneTask);
    }
    if( mHorizontalBlurTask )
    {
      mHorizontalBlurTask.SetCameraActor(null);
      taskList.RemoveTask(mHorizontalBlurTask);
    }
    if( mVerticalBlurTask )
    {
      mVerticalBlurTask.SetCameraActor(null);
      taskList.RemoveTask(mVerticalBlurTask);
    }
    if( mUpsampleTask )
    {
      mUpsampleTask.SetCameraActor(null);
      taskList.RemoveTask(mUpsampleTask);
    }

    mRenderSceneTask.Reset();
    mHorizontalBlurTask.Reset();
    mVerticalBlurTask.Reset();
    mUpsampleTask.Reset();

    // Remove framebuffers
    mSceneFramebuffer.Reset();
    mHorizontalBlurFramebuffer.Reset();
    mVerticalBlurFramebuffer.Reset();
  }
}

void GaussianBlurView::RemoveTextures()
{
  // Remove output textures
  Toolkit::TextureManager::RemoveTexture(mSceneOutputUrl);
  Toolkit::TextureManager::RemoveTexture(mHorizontalOutputUrl);
  Toolkit::TextureManager::RemoveTexture(mVerticalOutputUrl);
}

float GaussianBlurView::CalcGaussianWeight(float x)
{
  return (1.0f / sqrt(2.0f * Math::PI * mBlurBellCurveWidth)) * exp(-(x * x) / (2.0f * mBlurBellCurveWidth * mBlurBellCurveWidth));
}

void GaussianBlurView::SetShaderConstants(Actor actor, Vector2 targetSize, Direction direction, uint32_t numSamples)
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

    // offset texture lookup to between texels, that way the bilinear filter in the texture hardware
    // will average two samples with one lookup
    ofs = ((float)(i << 1)) + 1.5f;

    // get offsets from units of pixels into uv coordinates in [0..1]
    float ofsX = ofs / targetSize.width;
    float ofsY = ofs / targetSize.height;
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
    switch( direction )
    {
      case Direction::HORIZONTAL:
      {
        actor.RegisterProperty( GetSampleOffsetsPropertyName( i ), uvOffsets[ i ] * xAxis );
        actor.RegisterProperty( GetSampleWeightsPropertyName( i ), weights[ i ] );
        break;
      }
      case Direction::VERTICAL:
      {
        actor.RegisterProperty( GetSampleOffsetsPropertyName( i ), uvOffsets[ i ] * yAxis );
        actor.RegisterProperty( GetSampleWeightsPropertyName( i ), weights[ i ] );
        break;
      }
    }
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


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
