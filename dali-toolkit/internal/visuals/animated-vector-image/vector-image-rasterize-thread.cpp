/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-image-rasterize-thread.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptors/adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_VECTOR_ANIMATION" );
#endif

} // unnamed namespace

VectorRasterizeThread::VectorRasterizeThread( const std::string& url, Renderer renderer, uint32_t width, uint32_t height )
: mUrl( url ),
  mVectorRenderer(),
  mResourceReadyTrigger( NULL ),
  mCurrentFrame( 0 ),
  mTotalFrame( 0 ),
  mWidth( width ),
  mHeight( height ),
  mNeedRender( false ),
  mPlaying( false ),
  mPaused( false ),
  mDestroyThread( false ),
  mResourceReady( false ),
  mLogFactory( Dali::Adaptor::Get().GetLogFactory() )
{
  mVectorRenderer = VectorAnimationRenderer::New( mUrl, renderer, width, height );
}

VectorRasterizeThread::~VectorRasterizeThread()
{
  // Stop the thread
  {
    ConditionalWait::ScopedLock lock( mConditionalWait );
    mDestroyThread = true;
    mConditionalWait.Notify( lock );

    // This should be called in the main thread to stop waiting for the dequeuable buffer.
    mVectorRenderer.StopRender();
  }

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::~VectorRasterizeThread: Join\n" );

  Join();

  delete mResourceReadyTrigger;
}

void VectorRasterizeThread::Run()
{
  SetThreadName( "VectorImageThread" );
  mLogFactory.InstallLogFunction();

  //TODO: check the return value
  StartRender();

  while( IsThreadReady() )
  {
    Rasterize();
  }
}

void VectorRasterizeThread::StartAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( !mPlaying )
  {
    mPlaying = true;
    mPaused = false;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StartAnimation: Start\n" );
  }
}

void VectorRasterizeThread::StopAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlaying )
  {
    mPlaying = false;
    mPaused = false;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StopAnimation: Stop\n" );
  }
}

void VectorRasterizeThread::PauseAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlaying && !mPaused )
  {
    mPaused = true;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::PauseAnimation: Pause\n" );
  }
}

void VectorRasterizeThread::ResumeAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlaying && mPaused )
  {
    mPaused = false;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::ResumeAnimation: Resume\n" );
  }
}

void VectorRasterizeThread::RenderFrame()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mNeedRender = true;
  mConditionalWait.Notify( lock );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::RenderFrame: Render\n" );
}

void VectorRasterizeThread::SetResourceReadyCallback( EventThreadCallback* callback )
{
  mResourceReadyTrigger = callback;
}

bool VectorRasterizeThread::IsThreadReady()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( ( !mPlaying || mPaused ) && !mNeedRender && !mDestroyThread )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::IsThreadReady: Wait\n" );

    if( !mPlaying )
    {
      mCurrentFrame = 0;
    }

    mConditionalWait.Wait( lock );
  }

  // Keep the thread alive if this thread is NOT to be destroyed
  return !mDestroyThread;
}

bool VectorRasterizeThread::StartRender()
{
  //TODO: check the return value
  mVectorRenderer.StartRender();

  mTotalFrame = mVectorRenderer.GetTotalFrameNumber();

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StartRender: Renderer is started [%d]\n", mTotalFrame );

  return true;
}

void VectorRasterizeThread::Rasterize()
{
  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: [%d]\n", mCurrentFrame );

  // Rasterize
  mVectorRenderer.Render( mCurrentFrame );

  if( mPlaying && !mPaused )
  {
    mCurrentFrame++;

    if( mCurrentFrame >= mTotalFrame )
    {
      mCurrentFrame = 0;
    }
  }

  mNeedRender = false;

  if( !mResourceReady )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: Resource ready trigger\n" );

    mResourceReadyTrigger->Trigger();
    mResourceReady = true;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
