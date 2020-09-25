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
#include "svg-rasterize-thread.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#ifdef NO_THORVG
#include <dali-toolkit/third-party/nanosvg/nanosvgrast.h>
#endif /* NO_THORVG */
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char * const UNITS("px");
}

#ifdef NO_THORVG
RasterizingTask::RasterizingTask( SvgVisual* svgRenderer, NSVGimage* parsedSvg, const VisualUrl& url, float dpi, unsigned int width, unsigned int height)
: mSvgVisual( svgRenderer ),
  mParsedSvg( parsedSvg ),
  mUrl( url ),
  mDpi( dpi ),
  mWidth( width ),
  mHeight( height )
{
  mRasterizer = nsvgCreateRasterizer();
}
#else /* NO_THORVG */
RasterizingTask::RasterizingTask( SvgVisual* svgRenderer, VectorImageRenderer vectorRenderer, const VisualUrl& url, float dpi, unsigned int width, unsigned int height, bool loaded)
: mSvgVisual( svgRenderer ),
  mVectorRenderer( vectorRenderer ),
  mUrl( url ),
  mDpi( dpi ),
  mWidth( width ),
  mHeight( height ),
  mLoaded( loaded )
{

}
#endif /* NO_THORVG */

RasterizingTask::~RasterizingTask()
{
#ifdef NO_THORVG
  nsvgDeleteRasterizer( mRasterizer );
#endif /* NO_THORVG */
}

void RasterizingTask::Load()
{
#ifdef NO_THORVG
  if( mParsedSvg != NULL)
  {
    return;
  }

  if( !mUrl.IsLocalResource() )
  {
    Dali::Vector<uint8_t> remoteBuffer;

    if( !Dali::FileLoader::DownloadFileSynchronously( mUrl.GetUrl(), remoteBuffer ))
    {
      DALI_LOG_ERROR("Failed to download file!\n");
      return;
    }

    remoteBuffer.PushBack( '\0' );
    mParsedSvg = nsvgParse( reinterpret_cast<char*>(remoteBuffer.begin()), UNITS, mDpi );
  }
#else /* NO_THORVG */
  if( !mLoaded &&  !mUrl.IsLocalResource() )
  {
    Dali::Vector<uint8_t> remoteBuffer;

    if( !Dali::FileLoader::DownloadFileSynchronously( mUrl.GetUrl(), remoteBuffer ))
    {
      DALI_LOG_ERROR("Failed to download file!\n");
      return;
    }

    remoteBuffer.PushBack( '\0' );
    char *data = reinterpret_cast<char*>(remoteBuffer.begin());
    if ( !mVectorRenderer.Load( data, remoteBuffer.Size()))
    {
      DALI_LOG_ERROR( "Failed to load data!\n" );
      return;
    }

    mLoaded = true;
  }
#endif /* NO_THORVG */
}

void RasterizingTask::Rasterize( )
{
#ifdef NO_THORVG
  if( mParsedSvg != NULL && mWidth > 0u && mHeight > 0u )
  {
    float scaleX = static_cast<float>( mWidth ) /  mParsedSvg->width;
    float scaleY = static_cast<float>( mHeight ) /  mParsedSvg->height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    unsigned int bufferStride = mWidth*Pixel::GetBytesPerPixel( Pixel::RGBA8888 );
    unsigned int bufferSize = bufferStride * mHeight;

    unsigned char* buffer = new unsigned char [bufferSize];
    nsvgRasterize(mRasterizer, mParsedSvg, 0.f,0.f,scale,
        buffer, mWidth, mHeight,
        bufferStride );

    mPixelData = Dali::PixelData::New( buffer, bufferSize, mWidth, mHeight, Pixel::RGBA8888, Dali::PixelData::DELETE_ARRAY );
  }
#else /* NO_THORVG */
  if ( mWidth <= 0u || mHeight <= 0u )
  {
    DALI_LOG_ERROR( "Size is zero!\n" );
    return;
  }

  Devel::PixelBuffer pixelBuffer = Devel::PixelBuffer::New( mWidth, mHeight, Dali::Pixel::RGBA8888 );
  mVectorRenderer.SetBuffer( pixelBuffer );
  {
    uint32_t defaultWidth, defaultHeight;
    mVectorRenderer.GetDefaultSize( defaultWidth, defaultHeight );

    float scaleX = static_cast<float>( mWidth ) / static_cast<float>( defaultWidth );
    float scaleY = static_cast<float>( mHeight ) / static_cast<float>( defaultHeight );
    float scale = scaleX < scaleY ? scaleX : scaleY;

    if ( !mVectorRenderer.Render( scale ) )
    {
      DALI_LOG_ERROR( "SVG Render Fail!\n" );
      return;
    }

    mPixelData = Devel::PixelBuffer::Convert( pixelBuffer );
    if ( !mPixelData )
    {
      DALI_LOG_ERROR( "Pixel Data is null\n" );
    }
  }
#endif /* NO_THORVG */
}

#ifdef NO_THORVG
NSVGimage* RasterizingTask::GetParsedImage() const
{
  return mParsedSvg;
}
#else /* NO_THORVG */
VectorImageRenderer RasterizingTask::GetVectorRenderer() const
{
  return mVectorRenderer;
}

bool RasterizingTask::IsLoaded() const
{
  return mLoaded;
}
#endif /* NO_THORVG */

SvgVisual* RasterizingTask::GetSvgVisual() const
{
  return mSvgVisual.Get();
}

PixelData RasterizingTask::GetPixelData() const
{
  return mPixelData;
}

SvgRasterizeThread::SvgRasterizeThread( EventThreadCallback* trigger )
: mTrigger( trigger ),
  mIsThreadWaiting( false )
{
}

SvgRasterizeThread::~SvgRasterizeThread()
{
  delete mTrigger;
}

void SvgRasterizeThread::TerminateThread( SvgRasterizeThread*& thread )
{
  if( thread )
  {
    // add an empty task would stop the thread from conditional wait.
    thread->AddTask( RasterizingTaskPtr() );
    // stop the thread
    thread->Join();
    // delete the thread
    delete thread;
    thread = NULL;
  }
}

void SvgRasterizeThread::AddTask( RasterizingTaskPtr task )
{
  bool wasEmpty = false;

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock( mConditionalWait );
    wasEmpty = mRasterizeTasks.empty();
    if( !wasEmpty && task != NULL)
    {
      // Remove the tasks with the same renderer.
      // Older task which waiting to rasterize and apply the svg to the same renderer is expired.
      for( std::vector< RasterizingTaskPtr >::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it )
      {
        if( (*it) && (*it)->GetSvgVisual() == task->GetSvgVisual() )
        {
          mRasterizeTasks.erase( it );
          break;
        }
      }
    }
    mRasterizeTasks.push_back( task );
  }

  if( wasEmpty)
  {
    // wake up the image loading thread
    mConditionalWait.Notify();
  }
}

RasterizingTaskPtr SvgRasterizeThread::NextCompletedTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock( mMutex );

  if( mCompletedTasks.empty() )
  {
    return RasterizingTaskPtr();
  }

  std::vector< RasterizingTaskPtr >::iterator next = mCompletedTasks.begin();
  RasterizingTaskPtr nextTask = *next;
  mCompletedTasks.erase( next );

  return nextTask;
}

void SvgRasterizeThread::RemoveTask( SvgVisual* visual )
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( !mRasterizeTasks.empty() )
  {
    for( std::vector< RasterizingTaskPtr >::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it )
    {
      if( (*it) &&  (*it)->GetSvgVisual() == visual )
      {
        mRasterizeTasks.erase( it );
        break;
      }
    }
  }
}

#ifdef NO_THORVG
void SvgRasterizeThread::DeleteImage( NSVGimage* parsedSvg )
{
  // Lock while adding image to the delete queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( mIsThreadWaiting ) // no rasterization is ongoing, save to delete
  {
    nsvgDelete( parsedSvg );
  }
  else // wait to delete until current rasterization completed.
  {
    mDeleteSvg.PushBack( parsedSvg );
  }
}
#else /* NO_THORVG */
void SvgRasterizeThread::DeleteImage( VectorImageRenderer vectorRenderer )
{
  // Lock while adding image to the delete queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( mIsThreadWaiting ) // no rasterization is ongoing, save to delete
  {
    // TODO: what?
  }
  else // wait to delete until current rasterization completed.
  {
    mDeleteSvg.PushBack( &vectorRenderer );
  }
}
#endif /* NO_THORVG */

RasterizingTaskPtr SvgRasterizeThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  // Delete the image here to make sure that it is not used in the nsvgRasterize()
  if( !mDeleteSvg.Empty() )
  {
#ifdef NO_THORVG
    for( Vector< NSVGimage* >::Iterator it = mDeleteSvg.Begin(), endIt = mDeleteSvg.End();
        it != endIt;
        ++it )
    {
      nsvgDelete( *it );
    }
#endif /* NO_THORVG */
    mDeleteSvg.Clear();
  }

  // conditional wait
  while( mRasterizeTasks.empty() )
  {
    mIsThreadWaiting = true;
    mConditionalWait.Wait( lock );
  }
  mIsThreadWaiting = false;

  // pop out the next task from the queue
  std::vector< RasterizingTaskPtr >::iterator next = mRasterizeTasks.begin();
  RasterizingTaskPtr nextTask = *next;
  mRasterizeTasks.erase( next );

  return nextTask;
}

void SvgRasterizeThread::AddCompletedTask( RasterizingTaskPtr task )
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock( mMutex );
  mCompletedTasks.push_back( task );

  // wake up the main thread
  mTrigger->Trigger();
}

void SvgRasterizeThread::Run()
{
  SetThreadName( "SVGThread" );
  while( RasterizingTaskPtr task = NextTaskToProcess() )
  {
    task->Load( );
    task->Rasterize( );
    AddCompletedTask( task );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
