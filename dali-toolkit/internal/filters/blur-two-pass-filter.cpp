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
#include "blur-two-pass-filter.h"

// EXTERNAL INCLUDES
#include <sstream>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/images/texture-set-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali-toolkit/devel-api/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float DEFAULT_KERNEL0[] = { 12.0f/16.0f, 2.0f/16.0f, 2.0f/16.0f };

const float DEFAULT_KERNEL1[] = { 8.0f/16.0f, 2.75f/16.0f, 2.75f/16.0f, 1.25f/16.0f,
                                  1.25f/16.0f };

const float DEFAULT_KERNEL2[] = { 5.0f/16.0f, 2.75f/16.0f, 2.75f/16.0f, 1.75f/16.0f,
                                  1.75f/16.0f, 1.5f/16.0f, 1.5f/16.0f };

const float DEFAULT_KERNEL3[] = { 3.0f/16.0f, 2.0f/16.0f, 2.0f/16.0f, 2.0f/16.0f,
                                  2.0f/16.0f, 2.0f/16.0f, 2.0f/16.0f, 0.5f/16.0f,
                                  0.5f/16.0f };

const float DEFAULT_KERNEL4[] = { 2.0f/16.0f, 1.5f/16.0f, 1.5f/16.0f, 1.5f/16.0f,
                                  1.5f/16.0f, 1.0f/16.0f, 1.0f/16.0f, 1.0f/16.0f,
                                  1.0f/16.0f, 1.0f/16.0f, 1.0f/16.0f, 0.5f/16.0f,
                                  0.5f/16.0f, 0.5f/16.0f, 0.5f/16.0f };


std::string GetOffsetUniformName( int index )
{
  std::ostringstream oss;
  oss << "uSampleOffsets[" << index << "]";
  return oss.str();
}

std::string GetWeightUniformName( int index )
{
  std::ostringstream oss;
  oss << "uSampleWeights[" << index << "]";
  return oss.str();
}

const char* const BLUR_STRENGTH_UNIFORM_NAME( "uBlurStrength"  );
const char* const EFFECT_IMAGE_NAME( "sEffect" );

} // namespace


BlurTwoPassFilter::BlurTwoPassFilter()
: ImageFilter()
{
  // create blending actor and register the property in constructor
  // to make sure that GetBlurStrengthPropertyIndex() always returns a valid index
  mActorForBlending = Toolkit::ImageView::New();
  mBlurStrengthPropertyIndex = mActorForBlending.RegisterProperty( BLUR_STRENGTH_UNIFORM_NAME, 1.f );
}

BlurTwoPassFilter::~BlurTwoPassFilter()
{
}

void BlurTwoPassFilter::Enable()
{
  // create actor to render input with applied emboss effect
  mActorForInput = Toolkit::ImageView::New( mInputImage );
  mActorForInput.SetParentOrigin( ParentOrigin::CENTER );
  mActorForInput.SetSize( mTargetSize );

  // create internal offscreen for result of horizontal pass
  mImageForHorz = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat );
  // create an actor to render mImageForHorz for vertical blur pass
  mActorForHorz = Toolkit::ImageView::New( mImageForHorz );
  mActorForHorz.SetParentOrigin( ParentOrigin::CENTER );
  mActorForHorz.SetSize( mTargetSize );

  // create internal offscreen for result of the two pass blurred image
  mBlurredImage = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat );
  // create an actor to blend the blurred image and the input image with the given blur strength
  mActorForBlending.SetImage( mBlurredImage );
  mActorForBlending.SetParentOrigin( ParentOrigin::CENTER );
  mActorForBlending.SetSize( mTargetSize );

  // create custom shader effect
  if( !GetKernelSize() )
  {
    CreateKernel( DEFAULT_KERNEL4, sizeof(DEFAULT_KERNEL4)/sizeof(DEFAULT_KERNEL4[0]) );
  }
  int kernelSize( static_cast< int >(GetKernelSize()) );

  for( int i = 0; i < kernelSize; ++i )
  {
    const std::string offsetUniform( GetOffsetUniformName( i ) );
    const std::string weightUniform( GetWeightUniformName( i ) );

    mActorForInput.RegisterProperty( offsetUniform, Vector2(mKernel[i]) * Vector2::XAXIS );
    mActorForInput.RegisterProperty( weightUniform, mKernel[i].z );

    mActorForHorz.RegisterProperty( offsetUniform, Vector2(mKernel[i]) * Vector2::YAXIS );
    mActorForHorz.RegisterProperty( weightUniform, mKernel[i].z );
  }

  // Set up blur-two-pass custom shader
  Property::Value twoPassFragData;
  const auto& twoPassFragShader = GraphicsGetBuiltinShader( "BLUR_TWO_PASS_SHADER_FRAG" );
  EncodeBase64PropertyData( twoPassFragData, twoPassFragShader );

  Property::Value twoImageFragData;
  auto twoImageFragShader = GraphicsGetBuiltinShader( "BLUR_TWO_IMAGES_SHADER_FRAG" );
  EncodeBase64PropertyData( twoImageFragData, twoImageFragShader );

  Property::Map customShader;
  Property::Map specializationConstants;
  specializationConstants["NUM_SAMPLES"] = kernelSize;

  customShader[ Toolkit::DevelVisual::Shader::Property::SPECIALIZATION_CONSTANTS ] = specializationConstants;
  customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = twoPassFragData;

  Property::Map visualMap;
  visualMap[ Toolkit::Visual::Property::SHADER ] = customShader;
  mActorForInput.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );
  mActorForHorz.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );

  // Set up blend-two-image custom shader
  customShader.Clear();
  customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = twoImageFragData;
  visualMap[ Toolkit::Visual::Property::SHADER ] = customShader;
  mActorForBlending.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );

  mRootActor.Add( mActorForInput );
  mRootActor.Add( mActorForHorz );
  mRootActor.Add( mActorForBlending );

  // Add effect texture to blend-two-image custom shader
  TextureSet textureSet = mActorForBlending.GetRendererAt(0).GetTextures();
  if( textureSet )
  {
    TextureSetImage( textureSet, 1u, mInputImage );
  }

  SetupCamera();
  CreateRenderTasks();
}

void BlurTwoPassFilter::Disable()
{
  if( mRootActor )
  {
    if( mCameraActor )
    {
      mRootActor.Remove( mCameraActor );
      mCameraActor.Reset();
    }

    if( mActorForInput )
    {
      mRootActor.Remove( mActorForInput );
      mActorForInput.Reset();
    }

    if( mActorForHorz )
    {
      mRootActor.Remove( mActorForHorz );
      mActorForHorz.Reset();
    }

    RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

    if( mRenderTaskForHorz )
    {
      taskList.RemoveTask(mRenderTaskForHorz);
    }
    if( mRenderTaskForVert )
    {
      taskList.RemoveTask(mRenderTaskForVert);
    }
    if( mRenderTaskForBlending )
    {
      taskList.RemoveTask(mRenderTaskForBlending);
    }

    mRootActor.Reset();
  }
}

void BlurTwoPassFilter::Refresh()
{
  if( mRenderTaskForHorz )
  {
    mRenderTaskForHorz.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }

  if( mRenderTaskForVert )
  {
    mRenderTaskForVert.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }

  if( mRenderTaskForBlending )
  {
    mRenderTaskForBlending.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }
}

void BlurTwoPassFilter::SetSize( const Vector2& size )
{
  mTargetSize = size;
  if( mActorForInput )
  {
    mActorForInput.SetSize(mTargetSize);
  }
  if( mActorForHorz )
  {
    mActorForHorz.SetSize(mTargetSize);
  }
  if( mActorForBlending )
  {
    mActorForBlending.SetSize(mTargetSize);
  }
}

Handle BlurTwoPassFilter::GetHandleForAnimateBlurStrength()
{
  return mActorForBlending;
}

void BlurTwoPassFilter::CreateRenderTasks()
{
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();

  // perform a horizontal blur targeting the internal buffer
  mRenderTaskForHorz = taskList.CreateTask();
  mRenderTaskForHorz.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForHorz.SetSourceActor( mActorForInput );
  mRenderTaskForHorz.SetExclusive(true);
  mRenderTaskForHorz.SetInputEnabled( false );
  mRenderTaskForHorz.SetClearEnabled( true );
  mRenderTaskForHorz.SetClearColor( mBackgroundColor );
  mRenderTaskForHorz.SetTargetFrameBuffer( mImageForHorz );
  mRenderTaskForHorz.SetCameraActor( mCameraActor );

  // use the internal buffer and perform a horizontal blur targeting the output buffer
  mRenderTaskForVert = taskList.CreateTask();
  mRenderTaskForVert.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForVert.SetSourceActor( mActorForHorz );
  mRenderTaskForVert.SetExclusive(true);
  mRenderTaskForVert.SetInputEnabled( false );
  mRenderTaskForVert.SetClearEnabled( true );
  mRenderTaskForVert.SetClearColor( mBackgroundColor );
  mRenderTaskForVert.SetTargetFrameBuffer( mBlurredImage );
  mRenderTaskForVert.SetCameraActor( mCameraActor );

  //Perform a blending between the blurred image and the input image
  mRenderTaskForBlending = taskList.CreateTask();
  mRenderTaskForBlending.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForBlending.SetSourceActor( mActorForBlending );
  mRenderTaskForBlending.SetExclusive(true);
  mRenderTaskForBlending.SetInputEnabled( false );
  mRenderTaskForBlending.SetClearEnabled( true );
  mRenderTaskForBlending.SetClearColor( mBackgroundColor );
  mRenderTaskForBlending.SetTargetFrameBuffer( mOutputImage );
  mRenderTaskForBlending.SetCameraActor( mCameraActor );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
