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
#include "spread-filter.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/devel-api/common/stage.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

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
  mActorForInput = Actor::New();
  mActorForInput.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mActorForInput.SetProperty( Actor::Property::SIZE, mTargetSize);
  // register properties as shader uniforms
  mActorForInput.RegisterProperty( SPREAD_UNIFORM_NAME, mSpread );
  mActorForInput.RegisterProperty( TEX_SCALE_UNIFORM_NAME, Vector2( 1.0f / mTargetSize.width, 0.0f ) );

  Renderer rendererForInput = CreateRenderer( BASIC_VERTEX_SOURCE, SHADER_SPREAD_FILTER_SHADER_FRAG );
  SetRendererTexture( rendererForInput, mInputTexture );
  mActorForInput.AddRenderer( rendererForInput );

  // create internal offscreen for result of horizontal pass
  mFrameBufferForHorz = FrameBuffer::New( mTargetSize.width, mTargetSize.height, FrameBuffer::Attachment::NONE );
  Texture textureForHorz = Texture::New( TextureType::TEXTURE_2D, mPixelFormat, unsigned(mTargetSize.width), unsigned(mTargetSize.height) );
  mFrameBufferForHorz.AttachColorTexture( textureForHorz );

  // create an actor to render mImageForHorz for vertical blur pass
  mActorForHorz = Actor::New();
  mActorForHorz.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  mActorForHorz.SetProperty( Actor::Property::SIZE, mTargetSize);
  // register properties as shader uniforms
  mActorForHorz.RegisterProperty( SPREAD_UNIFORM_NAME, mSpread );
  mActorForHorz.RegisterProperty( TEX_SCALE_UNIFORM_NAME, Vector2( 0.0f, 1.0f / mTargetSize.height ) );
  Renderer rendererForHorz = CreateRenderer( BASIC_VERTEX_SOURCE, SHADER_SPREAD_FILTER_SHADER_FRAG );
  SetRendererTexture( rendererForHorz, textureForHorz );
  mActorForHorz.AddRenderer( rendererForHorz );

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
    mActorForInput.SetProperty( Actor::Property::SIZE, mTargetSize);
  }
  if( mActorForHorz )
  {
    mActorForHorz.SetProperty( Actor::Property::SIZE, mTargetSize);
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
  mRenderTaskForHorz.SetFrameBuffer( mFrameBufferForHorz );
  mRenderTaskForHorz.SetCameraActor( mCameraActor );

  // use the internal buffer and perform a horizontal blur targeting the output buffer
  mRenderTaskForVert = taskList.CreateTask();
  mRenderTaskForVert.SetRefreshRate( mRefreshOnDemand ? RenderTask::REFRESH_ONCE : RenderTask::REFRESH_ALWAYS );
  mRenderTaskForVert.SetSourceActor( mActorForHorz );
  mRenderTaskForVert.SetExclusive(true);
  mRenderTaskForVert.SetInputEnabled( false );
  mRenderTaskForVert.SetClearEnabled( true );
  mRenderTaskForVert.SetClearColor( mBackgroundColor );
  mRenderTaskForVert.SetFrameBuffer( mOutputFrameBuffer );
  mRenderTaskForVert.SetCameraActor( mCameraActor );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
