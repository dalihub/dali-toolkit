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
#include "canvas-view-rasterize-thread.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
CanvasRendererRasterizingTask::CanvasRendererRasterizingTask(CanvasView* canvasView, CanvasRenderer canvasRenderer)
: mCanvasView(canvasView),
  mCanvasRenderer(canvasRenderer),
  mRasterizedTexture()
{
}

bool CanvasRendererRasterizingTask::Rasterize()
{
  if(mCanvasRenderer && mCanvasRenderer.Rasterize())
  {
    return true;
  }
  return false;
}

CanvasView* CanvasRendererRasterizingTask::GetCanvasView() const
{
  return mCanvasView.Get();
}

Texture CanvasRendererRasterizingTask::GetRasterizedTexture()
{
  return mCanvasRenderer.GetRasterizedTexture();
}

CanvasViewRasterizeThread::CanvasViewRasterizeThread()
: mTrigger(new EventThreadCallback(MakeCallback(this, &CanvasViewRasterizeThread::ApplyRasterized))),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mProcessorRegistered(false),
  mRasterizationCompletedSignal()
{
}

CanvasViewRasterizeThread::~CanvasViewRasterizeThread()
{
  if(mProcessorRegistered && Adaptor::IsAvailable())
  {
    Adaptor::Get().UnregisterProcessor(*this);
  }
}

void CanvasViewRasterizeThread::TerminateThread(CanvasViewRasterizeThread*& thread)
{
  if(thread)
  {
    // add an empty task would stop the thread from conditional wait.
    thread->AddTask(CanvasRendererRasterizingTaskPtr());
    // stop the thread
    thread->Join();
    // delete the thread
    delete thread;
    thread = NULL;
  }
}

void CanvasViewRasterizeThread::AddTask(CanvasRendererRasterizingTaskPtr task)
{
  bool wasEmpty = false;

  {
    // Lock while adding task to the queue
    ConditionalWait::ScopedLock lock(mConditionalWait);
    wasEmpty = mRasterizeTasks.empty();
    if(!wasEmpty && task != NULL)
    {
      // Remove the tasks with the same renderer.
      // Older task which waiting to rasterize and apply the svg to the same renderer is expired.
      for(std::vector<CanvasRendererRasterizingTaskPtr>::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it)
      {
        if((*it) && (*it)->GetCanvasView() == task->GetCanvasView()) //Need
        {
          mRasterizeTasks.erase(it);
          break;
        }
      }
    }
    mRasterizeTasks.push_back(task);

    if(!mProcessorRegistered && Adaptor::IsAvailable())
    {
      Adaptor::Get().RegisterProcessor(*this);
      mProcessorRegistered = true;
    }
  }

  if(wasEmpty)
  {
    // wake up the image loading thread
    mConditionalWait.Notify();
  }
}

CanvasRendererRasterizingTaskPtr CanvasViewRasterizeThread::NextCompletedTask()
{
  // Lock while popping task out from the queue
  Mutex::ScopedLock lock(mMutex);

  if(mCompletedTasks.empty())
  {
    return CanvasRendererRasterizingTaskPtr();
  }

  std::vector<CanvasRendererRasterizingTaskPtr>::iterator next     = mCompletedTasks.begin();
  CanvasRendererRasterizingTaskPtr                        nextTask = *next;
  mCompletedTasks.erase(next);

  return nextTask;
}

void CanvasViewRasterizeThread::RemoveTask(CanvasView* canvasView)
{
  // Lock while remove task from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);
  if(!mRasterizeTasks.empty())
  {
    for(std::vector<CanvasRendererRasterizingTaskPtr>::iterator it = mRasterizeTasks.begin(), endIt = mRasterizeTasks.end(); it != endIt; ++it)
    {
      if((*it) && (*it)->GetCanvasView() == canvasView)
      {
        mRasterizeTasks.erase(it);
        break;
      }
    }
  }

  UnregisterProcessor();
}

CanvasRendererRasterizingTaskPtr CanvasViewRasterizeThread::NextTaskToProcess()
{
  // Lock while popping task out from the queue
  ConditionalWait::ScopedLock lock(mConditionalWait);

  // conditional wait
  while(mRasterizeTasks.empty())
  {
    mConditionalWait.Wait(lock);
  }

  // pop out the next task from the queue
  std::vector<CanvasRendererRasterizingTaskPtr>::iterator next     = mRasterizeTasks.begin();
  CanvasRendererRasterizingTaskPtr                        nextTask = *next;
  mRasterizeTasks.erase(next);

  return nextTask;
}

void CanvasViewRasterizeThread::AddCompletedTask(CanvasRendererRasterizingTaskPtr task)
{
  // Lock while adding task to the queue
  Mutex::ScopedLock lock(mMutex);
  mCompletedTasks.push_back(task);

  // wake up the main thread
  mTrigger->Trigger();
}

void CanvasViewRasterizeThread::Run()
{
  SetThreadName("CanvasViewThread");
  mLogFactory.InstallLogFunction();

  while(CanvasRendererRasterizingTaskPtr task = NextTaskToProcess())
  {
    if(task->Rasterize())
    {
      AddCompletedTask(task);
    }
  }
}

void CanvasViewRasterizeThread::ApplyRasterized()
{
  while(CanvasRendererRasterizingTaskPtr task = NextCompletedTask())
  {
    RasterizationCompletedSignal().Emit(task->GetRasterizedTexture()); // Here texture get
  }

  UnregisterProcessor();
}

void CanvasViewRasterizeThread::Process(bool postProcessor)
{
  ApplyRasterized();
}

CanvasViewRasterizeThread::RasterizationCompletedSignalType& CanvasViewRasterizeThread::RasterizationCompletedSignal()
{
  return mRasterizationCompletedSignal;
}

void CanvasViewRasterizeThread::UnregisterProcessor()
{
  if(mProcessorRegistered)
  {
    if(mRasterizeTasks.empty() && mCompletedTasks.empty() && Adaptor::IsAvailable())
    {
      Adaptor::Get().UnregisterProcessor(*this);
      mProcessorRegistered = false;
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
