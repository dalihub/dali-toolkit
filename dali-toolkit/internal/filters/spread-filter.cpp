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
#include "spread-filter.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const char* const SPREAD_FRAGMENT_SOURCE =
{
 "precision highp float;\n"
 "varying mediump vec2 vTexCoord;\n"
 "uniform sampler2D sTexture;\n"
 "uniform int uSpread;\n"
 "uniform vec2 uTexScale;\n"
 "void main()\n"
 "{\n"
 "  vec4 color = texture2D( sTexture, vTexCoord);\n"
 "  for( int i = 1; i <= uSpread; ++i )\n"
 "  {\n"
 "    vec2 offset = uTexScale * float(i);\n"
 "    color = max( texture2D( sTexture, vTexCoord + offset), color );\n"
 "    color = max( texture2D( sTexture, vTexCoord - offset), color );\n"
 "  }\n"
 "  gl_FragColor = color;\n"
 "}\n"
};

const char* const SPREAD_UNIFORM_NAME( "uSpread" );
const char* const TEX_SCALE_UNIFORM_NAME( "uTexScale" );

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
  // create actor to render input with applied emboss effect
  mActorForInput = Toolkit::ImageView::New( mInputImage );
  mActorForInput.SetParentOrigin( ParentOrigin::CENTER );
  mActorForInput.SetSize(mTargetSize);
  // register properties as shader uniforms
  mActorForInput.RegisterProperty( SPREAD_UNIFORM_NAME, mSpread );
  mActorForInput.RegisterProperty( TEX_SCALE_UNIFORM_NAME, Vector2( 1.0f / mTargetSize.width, 0.0f ) );

  // create internal offscreen for result of horizontal pass
  mImageForHorz = FrameBufferImage::New( mTargetSize.width, mTargetSize.height, mPixelFormat );
  // create an actor to render mImageForHorz for vertical blur pass
  mActorForHorz = Toolkit::ImageView::New( mImageForHorz );
  mActorForHorz.SetParentOrigin( ParentOrigin::CENTER );
  mActorForHorz.SetSize(mTargetSize);
  // register properties as shader uniforms
  mActorForHorz.RegisterProperty( SPREAD_UNIFORM_NAME, mSpread );
  mActorForHorz.RegisterProperty( TEX_SCALE_UNIFORM_NAME, Vector2( 0.0f, 1.0f / mTargetSize.height ) );

  Property::Map customShader;
  customShader[ Toolkit::Visual::Shader::Property::FRAGMENT_SHADER ] = SPREAD_FRAGMENT_SOURCE;
  Property::Map visualMap;
  visualMap.Insert( Toolkit::DevelVisual::Property::SHADER, customShader );

  // set SPREAD custom shader
  mActorForInput.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );
  mActorForHorz.SetProperty( Toolkit::ImageView::Property::IMAGE, visualMap );

  mRootActor.Add( mActorForInput );
  mRootActor.Add( mActorForHorz );

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
