/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/clipping/text-clipper.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/integration-api/debug.h>

namespace
{

// Currently on desktop machines 2k x 2k is the maximum frame buffer size, on target is 4k x 4k.
const float MAX_OFFSCREEN_RENDERING_SIZE = 2048.f;

} // namespace


namespace Dali
{

namespace Toolkit
{

namespace Text
{

ClipperPtr Clipper::New( const Vector2& size )
{
  ClipperPtr clipper( new Clipper() );

  // Second-phase init
  clipper->Initialize( size );

  return clipper;
}

Actor Clipper::GetRootActor() const
{
  return mOffscreenRootActor;
}

ImageActor Clipper::GetImageActor() const
{
  return mImageActor;
}

void Clipper::Refresh( const Vector2& size )
{
  const Size offscreenSize( std::min( MAX_OFFSCREEN_RENDERING_SIZE, size.width ),
                            std::min( MAX_OFFSCREEN_RENDERING_SIZE, size.height ) );

  const bool sizeChanged = offscreenSize != mCurrentOffscreenSize;

  if( sizeChanged )
  {
    // Reconfigure camera for current size.
    mOffscreenCameraActor.SetOrthographicProjection( offscreenSize );

    // Recreate frame buffer for offscreen rendering when the size changes.
    FrameBufferImage frameBufferImage = FrameBufferImage::New( offscreenSize.width,
                                                               offscreenSize.height,
                                                               Pixel::RGBA8888 );

    mImageActor.SetSize( offscreenSize );
    mImageActor.SetImage( frameBufferImage );
    mRenderTask.SetTargetFrameBuffer( frameBufferImage );

    // Stores current sizPe to avoid create new Dali resources if text changes.
    mCurrentOffscreenSize = offscreenSize;
  }

  mRenderTask.SetRefreshRate( RenderTask::REFRESH_ONCE );
}

void Clipper::Initialize( const Vector2& size )
{
  const Size offscreenSize( std::min( MAX_OFFSCREEN_RENDERING_SIZE, size.width ),
                            std::min( MAX_OFFSCREEN_RENDERING_SIZE, size.height ) );

  // Create a root actor and an image actor for offscreen rendering.
  mOffscreenRootActor = Layer::New();
  mOffscreenRootActor.SetColorMode( USE_OWN_COLOR );
  mOffscreenRootActor.SetPositionInheritanceMode( DONT_INHERIT_POSITION );
  mOffscreenRootActor.SetInheritScale( false );
  mOffscreenRootActor.SetDepthTestDisabled( true );
  mOffscreenRootActor.SetSize( offscreenSize );

  mImageActor = ImageActor::New();
  mImageActor.SetParentOrigin( ParentOrigin::CENTER );
  mImageActor.SetBlendFunc( BlendingFactor::ONE, BlendingFactor::ONE_MINUS_SRC_ALPHA,
                            BlendingFactor::ONE, BlendingFactor::ONE );
  mImageActor.SetScale( Vector3( 1.0f, -1.0f, 1.0f ) );
  mImageActor.SetSize( offscreenSize );

  // Creates a new camera actor.
  mOffscreenCameraActor = CameraActor::New();
  mOffscreenCameraActor.SetParentOrigin( ParentOrigin::CENTER );
  mOffscreenCameraActor.SetOrthographicProjection( offscreenSize );
  mOffscreenRootActor.Add( mOffscreenCameraActor ); // camera to shoot the offscreen text

  // Creates a new render task.
  mRenderTask = Stage::GetCurrent().GetRenderTaskList().CreateTask();
  mRenderTask.SetSourceActor( mOffscreenRootActor );
  mRenderTask.SetClearColor( Color::TRANSPARENT );
  mRenderTask.SetClearEnabled( true );
  mRenderTask.SetExclusive( true );
  mRenderTask.SetCameraActor( mOffscreenCameraActor );

  // Creates a frame buffer for offscreen rendering
  FrameBufferImage frameBufferImage = FrameBufferImage::New( offscreenSize.width,
                                                             offscreenSize.height,
                                                             Pixel::RGBA8888 );
  mImageActor.SetImage( frameBufferImage );
  mRenderTask.SetTargetFrameBuffer( frameBufferImage );

  // Stores current size to avoid create new Dali resources if text changes.
  mCurrentOffscreenSize = offscreenSize;
}

Clipper::Clipper()
{
}

Clipper::~Clipper()
{
  if( Stage::IsInstalled() )
  {
    UnparentAndReset( mOffscreenRootActor );
    UnparentAndReset( mImageActor );

    Stage::GetCurrent().GetRenderTaskList().RemoveTask( mRenderTask );
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
