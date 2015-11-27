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
#include "image-load-thread.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

LoadingTask::LoadingTask(BitmapLoader loader, uint32_t packPositionX, uint32_t packPositionY, uint32_t width, uint32_t height )
: loader( loader ),
  packRect( packPositionX, packPositionY, width, height )
{
}

LoadQueue::LoadQueue()
{
}

LoadQueue::~LoadQueue()
{
}

LoadingTask* LoadQueue::NextTask()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  while( mTasks.Empty() )
  {
    mConditionalWait.Wait( lock );
  }

  Vector< LoadingTask* >::Iterator next = mTasks.Begin();
  LoadingTask* nextTask = *next;
  mTasks.Erase( next );

  return nextTask;
}

void LoadQueue::AddTask( LoadingTask* task )
{
  bool wasEmpty = false;

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock( mConditionalWait );
    wasEmpty = mTasks.Empty();
    mTasks.PushBack( task );
  }

  if( wasEmpty)
  {
    // wake up the image loading thread
    mConditionalWait.Notify();
  }
}

CompleteQueue::CompleteQueue(EventThreadCallback* trigger)
: mTrigger( trigger )
{}

CompleteQueue::~CompleteQueue()
{
  delete mTrigger;
}

LoadingTask* CompleteQueue::NextTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock( mMutex );

  if( mTasks.Empty() )
  {
    return NULL;
  }

  Vector< LoadingTask* >::Iterator next = mTasks.Begin();
  LoadingTask* nextTask = *next;
  mTasks.Erase( next );

  return nextTask;
}

void CompleteQueue::AddTask( LoadingTask* task )
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock( mMutex );
  mTasks.PushBack( task );

  // wake up the main thread
  mTrigger->Trigger();
}


ImageLoadThread::ImageLoadThread( LoadQueue& loadQueue, CompleteQueue& completeQueue )
: mLoadQueue( loadQueue ),
  mCompleteQueue( completeQueue )
{
}

ImageLoadThread::~ImageLoadThread()
{
}

void ImageLoadThread::Run()
{
  while( LoadingTask* task = mLoadQueue.NextTask() )
  {
    task->loader.Load();
    mCompleteQueue.AddTask( task );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
