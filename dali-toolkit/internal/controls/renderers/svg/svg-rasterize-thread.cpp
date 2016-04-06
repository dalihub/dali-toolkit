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

// INTERNAL INCLUDES
#include "nanosvg/nanosvgrast.h"
#include "svg-renderer.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

RasterizingTask::RasterizingTask( SvgRenderer* svgRenderer, NSVGimage* parsedSvg, unsigned int width, unsigned int height )
: mSvgRenderer( svgRenderer ),
  mParsedSvg( parsedSvg ),
  mWidth( width ),
  mHeight( height )
{
}

void RasterizingTask::Rasterize( NSVGrasterizer* rasterizer )
{
  if( mWidth > 0u && mHeight > 0u )
  {
    float scaleX =  static_cast<float>( mWidth ) /  mParsedSvg->width;
    float scaleY =  static_cast<float>( mHeight ) /  mParsedSvg->height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    unsigned int bufferStride = mWidth*Pixel::GetBytesPerPixel( Pixel::RGBA8888 );

    unsigned char* buffer = new unsigned char [bufferStride*mHeight];
    nsvgRasterize(rasterizer, mParsedSvg, 0.f,0.f,scale,
        buffer, mWidth, mHeight,
        bufferStride );

    mPixelData = Dali::PixelData::New( buffer, mWidth, mHeight, Pixel::RGBA8888, Dali::PixelData::DELETE_ARRAY );
  }
}

SvgRenderer* RasterizingTask::GetSvgRenderer() const
{
  return mSvgRenderer.Get();
}

PixelDataPtr RasterizingTask::GetPixelData() const
{
  return mPixelData;
}

SvgRasterizeThread::SvgRasterizeThread( EventThreadCallback* trigger )
: mTrigger( trigger ),
  mIsThreadWaiting( false )
{
  mRasterizer = nsvgCreateRasterizer();
}

SvgRasterizeThread::~SvgRasterizeThread()
{

  nsvgDeleteRasterizer( mRasterizer );
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
        if( (*it) && (*it)->GetSvgRenderer() == task->GetSvgRenderer() )
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

void SvgRasterizeThread::RemoveTask( SvgRenderer* renderer )
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( !mRasterizeTasks.empty() )
  {
    for( std::vector< RasterizingTaskPtr >::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it )
    {
      if( (*it) &&  (*it)->GetSvgRenderer() == renderer )
      {
        mRasterizeTasks.erase( it );
        break;
      }
    }
  }
}

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

RasterizingTaskPtr SvgRasterizeThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  // Delete the image here to make sure that it is not used in the nsvgRasterize()
  if( !mDeleteSvg.Empty() )
  {
    for( Vector< NSVGimage* >::Iterator it = mDeleteSvg.Begin(), endIt = mDeleteSvg.End();
        it != endIt;
        ++it )
    {
      nsvgDelete( *it );
    }
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
  while( RasterizingTaskPtr task = NextTaskToProcess() )
  {
    task->Rasterize( mRasterizer );
    AddCompletedTask( task );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
