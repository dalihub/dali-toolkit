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
#include "blur-two-pass-filter.h"

// EXTERNAL INCLUDES
#include <sstream>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES

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


const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

const char* BLUR_TWO_PASS_FRAGMENT_SOURCE =
{
 "precision highp float;\n"
 "uniform vec2 uSampleOffsets[NUM_SAMPLES];\n"
 "uniform float uSampleWeights[NUM_SAMPLES];\n"
 "void main()\n"
 "{\n"
 "  vec4 color = vec4(0.0);\n"
 "# ifdef DEBUG_RENDER\n"
 "  if( vTexCoord.s < 0.495 )\n"
 "  {\n"
 "# endif //def DEBUG_RENDER\n"
 "    for( int i = 0; i < NUM_SAMPLES; ++i )\n"
 "    {\n"
 "      color += texture2D( sTexture, vTexCoord + uSampleOffsets[i] ) * uSampleWeights[i];\n"
 "    }\n"
 "# ifdef DEBUG_RENDER\n"
 "  }\n"
 "  else if( vTexCoord.s > 0.505 )\n"
 "  {\n"
 "    color = texture2D( sTexture, vTexCoord );\n"
 "  }\n"
 "  else\n"
 "  {\n"
 "    color = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
 "  }\n"
 "# endif //def DEBUG_RENDER\n"
 "  gl_FragColor = color;\n"
 "}\n"
};

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

const char* BLEND_TWO_IMAGES_FRAGMENT_SOURCE =
{
 "precision highp float;\n"
 "uniform float uBlurStrength; \n "
 "void main()\n"
 "{\n"
 "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uBlurStrength"
 "               + texture2D( sEffect, vTexCoord )*(1.0-uBlurStrength); \n"
 "}\n"
};

std::string GetBlurStrengthUniformName()
{
  return "uBlurStrength";
}

} // namespace


BlurTwoPassFilter::BlurTwoPassFilter()
: ImageFilter()
{
  mShaderForBlending = ShaderEffect::New( "", BLEND_TWO_IMAGES_FRAGMENT_SOURCE );
  mShaderForBlending.SetUniform( GetBlurStrengthUniformName(), 1.f );
  mBlurStrengthPropertyIndex = mShaderForBlending.GetPropertyIndex( GetBlurStrengthUniformName() );
}

BlurTwoPassFilter::~BlurTwoPassFilter()
{
}

void BlurTwoPassFilter::Enable()
{
  mCameraForBlur = CameraActor::New();
  mCameraForBlur.SetParentOrigin(ParentOrigin::CENTER);

  // create actor to render input with applied emboss effect
  mActorForInput = ImageActor::New( mInputImage );
  mActorForInput.SetParentOrigin( ParentOrigin::CENTER );
  mActorForInput.SetSize(mTargetSize);
  mActorForInput.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

  // create internal offscreen for result of horizontal pass
  mImageForHorz = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Image::UNUSED );

  // create an actor to render mImageForHorz for vertical blur pass
  mActorForHorz = ImageActor::New( mImageForHorz );
  mActorForHorz.SetParentOrigin( ParentOrigin::CENTER );
  mActorForHorz.SetSize(mTargetSize);
  mActorForHorz.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

  // create internal offscreen for result of the two pass blurred image
  mBlurredImage = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat, Image::UNUSED);

  // create an actor to blend the blurred image and the input image with the given blur strength
  mActorForBlending = ImageActor::New( mBlurredImage );
  mActorForBlending.SetParentOrigin( ParentOrigin::CENTER );
  mActorForBlending.SetSize(mTargetSize);
  mActorForBlending.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) );

  mRootActor.Add( mActorForInput );
  mRootActor.Add( mActorForHorz );
  mRootActor.Add( mActorForBlending );
  mRootActor.Add( mCameraForBlur );

  // create custom shader effect
  if( !GetKernelSize() )
  {
    CreateKernel( DEFAULT_KERNEL4, sizeof(DEFAULT_KERNEL4)/sizeof(DEFAULT_KERNEL4[0]) );
  }
  int kernelSize( static_cast< int >(GetKernelSize()) );

  std::ostringstream fragmentSource;
  if( mDebugRender )
  {
    fragmentSource << "#define DEBUG_RENDER\n";
  }
  fragmentSource << "#define NUM_SAMPLES " << kernelSize << "\n";
  fragmentSource << BLUR_TWO_PASS_FRAGMENT_SOURCE;
  mShaderForHorz = ShaderEffect::New( "", fragmentSource.str() );
  mActorForInput.SetShaderEffect( mShaderForHorz );
  mShaderForVert = ShaderEffect::New( "", fragmentSource.str() );
  mActorForHorz.SetShaderEffect( mShaderForVert );

  for( int i = 0; i < kernelSize; ++i )
  {
    const std::string offsetUniform( GetOffsetUniformName( i ) );
    const std::string weightUniform( GetWeightUniformName( i ) );

    mShaderForHorz.SetUniform( offsetUniform, Vector2(mKernel[i]) * Vector2::XAXIS );
    mShaderForHorz.SetUniform( weightUniform, mKernel[i].z );

    mShaderForVert.SetUniform( offsetUniform, Vector2(mKernel[i]) * Vector2::YAXIS );
    mShaderForVert.SetUniform( weightUniform, mKernel[i].z );
  }

  mActorForBlending.SetShaderEffect( mShaderForBlending );
  mShaderForBlending.SetEffectImage( mInputImage );

  SetupCamera();
  CreateRenderTasks();
}

void BlurTwoPassFilter::Disable()
{
  if( mRootActor )
  {
    if( mCameraForBlur )
    {
      mRootActor.Remove( mCameraForBlur );
      mCameraForBlur.Reset();
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

    if( mActorForBlending )
    {
      mRootActor.Remove( mActorForBlending );
      mActorForBlending.Reset();
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

Constrainable BlurTwoPassFilter::GetHandleForAnimateBlurStrength()
{
  return mShaderForBlending;
}

void BlurTwoPassFilter::SetupCamera()
{
  // Create and place a camera for the embossing render, corresponding to its render target size
  mCameraForBlur.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
  mCameraForBlur.SetNearClippingPlane(1.0f);
  mCameraForBlur.SetAspectRatio(mTargetSize.width / mTargetSize.height);
  mCameraForBlur.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
  mCameraForBlur.SetRotation(Quaternion(M_PI, Vector3::YAXIS));
  mCameraForBlur.SetPosition(0.0f, 0.0f, ((mTargetSize.height * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f)));
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
  mRenderTaskForHorz.SetCameraActor( mCameraForBlur );

  // use the internal buffer and perform a horizontal blur targeting the output buffer
  mRenderTaskForVert = taskList.CreateTask();
  mRenderTaskForVert.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForVert.SetSourceActor( mActorForHorz );
  mRenderTaskForVert.SetExclusive(true);
  mRenderTaskForVert.SetInputEnabled( false );
  mRenderTaskForVert.SetClearEnabled( true );
  mRenderTaskForVert.SetClearColor( mBackgroundColor );
  mRenderTaskForVert.SetTargetFrameBuffer( mBlurredImage );
  mRenderTaskForVert.SetCameraActor( mCameraForBlur );

  //Perform a blending between the blurred image and the input image
  mRenderTaskForBlending = taskList.CreateTask();
  mRenderTaskForBlending.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForBlending.SetSourceActor( mActorForBlending );
  mRenderTaskForBlending.SetExclusive(true);
  mRenderTaskForBlending.SetInputEnabled( false );
  mRenderTaskForBlending.SetClearEnabled( true );
  mRenderTaskForBlending.SetClearColor( mBackgroundColor );
  mRenderTaskForBlending.SetTargetFrameBuffer( mOutputImage );
  mRenderTaskForBlending.SetCameraActor( mCameraForBlur );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
