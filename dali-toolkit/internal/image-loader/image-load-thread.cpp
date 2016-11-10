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

LoadingTask::LoadingTask(uint32_t id, BitmapLoader loader )
: loader( loader ),
  id( id )
{
}

ImageLoadThread::ImageLoadThread( EventThreadCallback* trigger )
: mTrigger( trigger )
{
}

ImageLoadThread::~ImageLoadThread()
{
  // add an empty task would stop the thread from conditional wait.
  AddTask( NULL );
  // stop the thread
  Join();

  delete mTrigger;
}

void ImageLoadThread::Run()
{
  while( LoadingTask* task = NextTaskToProcess() )
  {
    task->loader.Load();
    AddCompletedTask( task );
  }
}

void ImageLoadThread::AddTask( LoadingTask* task )
{
  bool wasEmpty = false;

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock( mConditionalWait );
    wasEmpty = mLoadQueue.Empty();
    mLoadQueue.PushBack( task );
  }

  if( wasEmpty )
  {
    // wake up the image loading thread
    mConditionalWait.Notify();
  }
}

LoadingTask* ImageLoadThread::NextCompletedTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock( mMutex );

  if( mCompleteQueue.Empty() )
  {
    return NULL;
  }

  Vector< LoadingTask* >::Iterator next = mCompleteQueue.Begin();
  LoadingTask* nextTask = *next;
  mCompleteQueue.Erase( next );

  return nextTask;
}

bool ImageLoadThread::CancelTask( uint32_t loadingTaskId )
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  for( Vector< LoadingTask* >::Iterator iter = mLoadQueue.Begin(); iter != mLoadQueue.End(); ++iter )
  {
    if( (*iter)->id == loadingTaskId )
    {
      delete (*iter);
      mLoadQueue.Erase( iter );
      return true;
    }
  }

  return false;
}


void ImageLoadThread::CancelAll()
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  for( Vector< LoadingTask* >::Iterator iter = mLoadQueue.Begin(); iter != mLoadQueue.End(); ++iter )
  {
    delete ( *iter );
  }
  mLoadQueue.Clear();
}

LoadingTask* ImageLoadThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock( mConditionalWait );

  while( mLoadQueue.Empty() )
  {
    mConditionalWait.Wait( lock );
  }

  Vector< LoadingTask* >::Iterator next = mLoadQueue.Begin();
  LoadingTask* nextTask = *next;
  mLoadQueue.Erase( next );

  return nextTask;
}

void ImageLoadThread::AddCompletedTask( LoadingTask* task )
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock( mMutex );
  mCompleteQueue.PushBack( task );

  // wake up the main thread
  mTrigger->Trigger();
}



} // namespace Internal

} // namespace Toolkit

} // namespace Dali
