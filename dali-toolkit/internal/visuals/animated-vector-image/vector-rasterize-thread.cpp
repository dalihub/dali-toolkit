/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-rasterize-thread.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <chrono>
#include <thread>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

} // unnamed namespace

VectorRasterizeThread::VectorRasterizeThread()
: mRasterizeTasks(),
  mConditionalWait(),
  mCompletedCallback(),
  mDestroyThread(false),
  mIsThreadStarted(false),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory())
{
}

VectorRasterizeThread::~VectorRasterizeThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    mDestroyThread = true;
    mConditionalWait.Notify(lock);
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::~VectorRasterizeThread: Join [%p]\n", this);

  Join();
}

void VectorRasterizeThread::SetCompletedCallback(CallbackBase* callback)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);

  mCompletedCallback = std::unique_ptr<CallbackBase>(callback);
}

void VectorRasterizeThread::AddTask(VectorAnimationTaskPtr task)
{
  // Lock while adding task to the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  if(!mIsThreadStarted)
  {
    Start();
    mIsThreadStarted = true;
  }

  if(mRasterizeTasks.end() == std::find(mRasterizeTasks.begin(), mRasterizeTasks.end(), task))
  {
    mRasterizeTasks.push_back(task);

    // wake up the animation thread
    mConditionalWait.Notify(lock);
  }
}

void VectorRasterizeThread::Run()
{
  SetThreadName("VectorRasterizeThread");
  mLogFactory.InstallLogFunction();

  while(!mDestroyThread)
  {
    Rasterize();
  }
}

void VectorRasterizeThread::Rasterize()
{
  VectorAnimationTaskPtr nextTask;
  {
    // Lock while popping task out from the queue
    ConditionalWait::ScopedLock lock(mConditionalWait);

    // conditional wait
    if(mRasterizeTasks.empty())
    {
      mConditionalWait.Wait(lock);
    }

    // pop out the next task from the queue
    if(!mRasterizeTasks.empty())
    {
      std::vector<VectorAnimationTaskPtr>::iterator next = mRasterizeTasks.begin();
      nextTask                                           = *next;
      mRasterizeTasks.erase(next);
    }
  }

  if(nextTask)
  {
    bool keepAnimation = nextTask->Rasterize();

    if(mCompletedCallback)
    {
      CallbackBase::Execute(*mCompletedCallback, nextTask, keepAnimation);
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
