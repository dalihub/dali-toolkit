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
#include "spread-filter.h"

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

const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

const char* const SPREAD_FRAGMENT_SOURCE =
{
 "precision highp float;\n"
 "uniform float uSpread;\n"
 "uniform vec2 uTexScale;\n"
 "void main()\n"
 "{\n"
 "  vec4 color = texture2D( sTexture, vTexCoord);\n"
 "# ifdef DEBUG_RENDER\n"
 "  if( vTexCoord.s < 0.495 )\n"
 "  {\n"
 "# endif //def DEBUG_RENDER\n"
 "    int spread = int(uSpread);\n"
 "    for( int i = 1; i <= spread; ++i )\n"
 "    {\n"
 "      vec2 offset = uTexScale * float(i);\n"
 "      color = max( texture2D( sTexture, vTexCoord + offset), color );\n"
 "      color = max( texture2D( sTexture, vTexCoord - offset), color );\n"
 "    }\n"
 "# ifdef DEBUG_RENDER\n"
 "  }\n"
 "  else if( vTexCoord.s <= 0.505 )\n"
 "  {\n"
 "    color = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
 "  }\n"
 "# endif //def DEBUG_RENDER\n"
 "  gl_FragColor = color;\n"
 "}\n"
};

} // namespace


SpreadFilter::SpreadFilter()
: ImageFilter(),
  mSpread(2)
{
}

SpreadFilter::~SpreadFilter()
{
}

void SpreadFilter::SetSpread( float spread )
{
  mSpread = spread;
}

void SpreadFilter::Enable()
{
  mCameraActor = CameraActor::New();
  mCameraActor.SetParentOrigin(ParentOrigin::CENTER);

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

  mRootActor.Add( mActorForInput );
  mRootActor.Add( mActorForHorz );
  mRootActor.Add( mCameraActor );

  std::ostringstream fragmentSource;
  if( mDebugRender )
  {
    fragmentSource << "#define DEBUG_RENDER\n";
  }
  fragmentSource << SPREAD_FRAGMENT_SOURCE;

  mShaderForHorz = ShaderEffect::New( "", fragmentSource.str() );
  mActorForInput.SetShaderEffect( mShaderForHorz );
  mShaderForHorz.SetUniform( "uSpread", mSpread );
  mShaderForHorz.SetUniform( "uTexScale", Vector2( 1.0f / mTargetSize.width, 0.0f ) );

  mShaderForVert = ShaderEffect::New( "", fragmentSource.str() );
  mActorForHorz.SetShaderEffect( mShaderForVert );
  mShaderForVert.SetUniform( "uSpread", mSpread );
  mShaderForVert.SetUniform( "uTexScale", Vector2( 0.0f, 1.0f / mTargetSize.height ) );

  SetupCamera();
  CreateRenderTasks();
}

void SpreadFilter::Disable()
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

    mRootActor.Reset();
  }
}

void SpreadFilter::Refresh()
{
  if( mRenderTaskForHorz )
  {
    mRenderTaskForHorz.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }

  if( mRenderTaskForVert )
  {
    mRenderTaskForVert.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  }
}

void SpreadFilter::SetSize( const Vector2& size )
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
}

void SpreadFilter::SetupCamera()
{
  // Create and place a camera for the embossing render, corresponding to its render target size
  mCameraActor.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
  mCameraActor.SetNearClippingPlane(1.0f);
  mCameraActor.SetAspectRatio(mTargetSize.width / mTargetSize.height);
  mCameraActor.SetType(Dali::Camera::FREE_LOOK); // camera orientation based solely on actor
  mCameraActor.SetPosition(0.0f, 0.0f, ((mTargetSize.height * 0.5f) / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f)));
}

void SpreadFilter::CreateRenderTasks()
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
  mRenderTaskForVert.SetTargetFrameBuffer( mOutputImage );
  mRenderTaskForVert.SetCameraActor( mCameraActor );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
