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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-rasterize-thread.h>

// EXTERNAL INCLUDES
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

constexpr auto LOOP_FOREVER = -1;

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_VECTOR_ANIMATION" );
#endif

} // unnamed namespace

VectorRasterizeThread::VectorRasterizeThread( const std::string& url, Renderer renderer, uint32_t width, uint32_t height )
: mUrl( url ),
  mVectorRenderer(),
  mConditionalWait(),
  mMutex(),
  mResourceReadyTrigger( NULL ),
  mAnimationFinishedTrigger( NULL ),
  mPlayRange( 0.0f, 1.0f ),
  mPlayState( DevelImageVisual::PlayState::STOPPED ),
  mCurrentFrame( 0 ),
  mTotalFrame( 0 ),
  mStartFrame( 0 ),
  mEndFrame( 0 ),
  mWidth( width ),
  mHeight( height ),
  mLoopCount( LOOP_FOREVER ),
  mCurrentLoop( 0 ),
  mNeedRender( false ),
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
  delete mAnimationFinishedTrigger;
}

void VectorRasterizeThread::Run()
{
  mLogFactory.InstallLogFunction();

  //TODO: check the return value
  StartRender();

  while( IsThreadReady() )
  {
    Rasterize();
  }
}

void VectorRasterizeThread::SetSize( uint32_t width, uint32_t height )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mVectorRenderer.SetSize( width, height );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetSize: width = %d, height = %d\n", width, height );
}

void VectorRasterizeThread::StartAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState != DevelImageVisual::PlayState::PLAYING )
  {
    if( mPlayState == DevelImageVisual::PlayState::STOPPED )
    {
      // Reset the current frame and the current loop
      mCurrentFrame = mStartFrame;
      mCurrentLoop = 0;
    }

    mPlayState = DevelImageVisual::PlayState::PLAYING;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StartAnimation: Start\n" );
  }
}

void VectorRasterizeThread::StopAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState != DevelImageVisual::PlayState::STOPPED )
  {
    mPlayState = DevelImageVisual::PlayState::STOPPED;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StopAnimation: Stop\n" );
  }
}

void VectorRasterizeThread::PauseAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState == DevelImageVisual::PlayState::PLAYING )
  {
    mPlayState = DevelImageVisual::PlayState::PAUSED;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::PauseAnimation: Pause\n" );
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
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mResourceReadyTrigger = callback;
}

void VectorRasterizeThread::SetAnimationFinishedCallback( EventThreadCallback* callback )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mAnimationFinishedTrigger = callback;
}

void VectorRasterizeThread::SetLoopCount( int16_t count )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  mLoopCount = count;

  // Reset progress
  mCurrentLoop = 0;
  mCurrentFrame = mStartFrame;
}

void VectorRasterizeThread::SetPlayRange( Vector2 range )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  mPlayRange = range;

  if( mTotalFrame != 0 )
  {
    mStartFrame = static_cast< uint32_t >( mPlayRange.x * mTotalFrame + 0.5f );
    mEndFrame = static_cast< uint32_t >( mPlayRange.y * mTotalFrame + 0.5f );
  }
}

DevelImageVisual::PlayState VectorRasterizeThread::GetPlayState()
{
  return mPlayState;
}

bool VectorRasterizeThread::IsThreadReady()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( mPlayState != DevelImageVisual::PlayState::PLAYING && !mNeedRender && !mDestroyThread )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::IsThreadReady: Wait\n" );

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

  mStartFrame = static_cast< uint32_t >( mPlayRange.x * mTotalFrame + 0.5f );
  mEndFrame = static_cast< uint32_t >( mPlayRange.y * mTotalFrame + 0.5f );

  mCurrentFrame = mStartFrame;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StartRender: Renderer is started [%d (%d, %d)]\n", mTotalFrame, mStartFrame, mEndFrame );

  return true;
}

void VectorRasterizeThread::Rasterize()
{
  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: [%d]\n", mCurrentFrame );

  // Rasterize
  mVectorRenderer.Render( mCurrentFrame );

  if( mPlayState == DevelImageVisual::PlayState::PLAYING )
  {
    if( ++mCurrentFrame >= mEndFrame )
    {
      if( mLoopCount < 0 )
      {
        // repeat forever
        mCurrentFrame = mStartFrame;
      }
      else
      {
        mCurrentLoop++;
        if( mCurrentLoop >= mLoopCount )
        {
          // Animation is finished
          mPlayState = DevelImageVisual::PlayState::STOPPED;

          mAnimationFinishedTrigger->Trigger();

          DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: Animation is finished\n" );
        }
        else
        {
          mCurrentFrame = mStartFrame;
        }
      }
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
