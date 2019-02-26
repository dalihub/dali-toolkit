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
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptors/adaptor.h>
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

constexpr auto LOOP_FOREVER = -1;
constexpr auto NANOSECONDS_PER_SECOND( 1e+9 );

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_VECTOR_ANIMATION" );
#endif

} // unnamed namespace

VectorRasterizeThread::VectorRasterizeThread( const std::string& url )
: mUrl( url ),
  mVectorRenderer(),
  mConditionalWait(),
  mMutex(),
  mResourceReadyTrigger(),
  mAnimationFinishedTrigger(),
  mPlayRange( 0.0f, 1.0f ),
  mPlayState( DevelImageVisual::PlayState::STOPPED ),
  mFrameDurationNanoSeconds( 0 ),
  mProgress( 0.0f ),
  mFrameRate( 60.0f ),
  mCurrentFrame( 0 ),
  mTotalFrame( 0 ),
  mStartFrame( 0 ),
  mEndFrame( 0 ),
  mWidth( 0 ),
  mHeight( 0 ),
  mLoopCount( LOOP_FOREVER ),
  mCurrentLoop( 0 ),
  mNeedRender( false ),
  mDestroyThread( false ),
  mResourceReady( false ),
  mLogFactory( Dali::Adaptor::Get().GetLogFactory() )
{
  mVectorRenderer = VectorAnimationRenderer::New( mUrl );
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

void VectorRasterizeThread::SetRenderer( Renderer renderer )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  mVectorRenderer.SetRenderer( renderer );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetRenderer\n" );
}

void VectorRasterizeThread::SetSize( uint32_t width, uint32_t height )
{
  if( mWidth != width || mHeight != height )
  {
    ConditionalWait::ScopedLock lock( mConditionalWait );
    mVectorRenderer.SetSize( width, height );

    mWidth = width;
    mHeight = height;

    mResourceReady = false;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetSize: width = %d, height = %d\n", width, height );
  }
}

void VectorRasterizeThread::PlayAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState != DevelImageVisual::PlayState::PLAYING )
  {
    mPlayState = DevelImageVisual::PlayState::PLAYING;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::PlayAnimation: Start\n" );
  }
}

void VectorRasterizeThread::StopAnimation()
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  if( mPlayState != DevelImageVisual::PlayState::STOPPED )
  {
    mPlayState = DevelImageVisual::PlayState::STOPPED;

    // Reset the current frame and the current loop
    mCurrentFrame = mStartFrame;
    mCurrentLoop = 0;

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

  if( !mResourceReady )
  {
    mNeedRender = true;
    mConditionalWait.Notify( lock );

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::RenderFrame: Render\n" );
  }
}

void VectorRasterizeThread::SetResourceReadyCallback( EventThreadCallback* callback )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mResourceReadyTrigger = std::unique_ptr< EventThreadCallback >( callback );
}

void VectorRasterizeThread::SetAnimationFinishedCallback( EventThreadCallback* callback )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );
  mAnimationFinishedTrigger = std::unique_ptr< EventThreadCallback >( callback );
}

void VectorRasterizeThread::SetLoopCount( int32_t count )
{
  if( mLoopCount != count )
  {
    ConditionalWait::ScopedLock lock( mConditionalWait );

    mLoopCount = count;

    // Reset progress
    mCurrentLoop = 0;
  }
}

int32_t VectorRasterizeThread::GetLoopCount() const
{
  return mLoopCount;
}

void VectorRasterizeThread::SetPlayRange( Vector2 range )
{
  // Make sure the range specified is between 0.0 and 1.0
  if( range.x >= 0.0f && range.x <= 1.0f && range.y >= 0.0f && range.y <= 1.0f )
  {
    Vector2 orderedRange( range );
    // If the range is not in order swap values
    if( range.x > range.y )
    {
      orderedRange = Vector2( range.y, range.x );
    }

    if( mPlayRange != orderedRange )
    {
      ConditionalWait::ScopedLock lock( mConditionalWait );

      mPlayRange = orderedRange;

      if( mTotalFrame != 0 )
      {
        mStartFrame = static_cast< uint32_t >( mPlayRange.x * mTotalFrame + 0.5f );
        mEndFrame = static_cast< uint32_t >( mPlayRange.y * mTotalFrame + 0.5f );

        // If the current frame is out of the range, change the current frame also.
        if( mStartFrame > mCurrentFrame )
        {
          mCurrentFrame = mStartFrame;

          mResourceReady = false;
        }
        else if( mEndFrame < mCurrentFrame )
        {
          mCurrentFrame = mEndFrame;

          mResourceReady = false;
        }
      }
    }
  }
}

Vector2 VectorRasterizeThread::GetPlayRange() const
{
  return mPlayRange;
}

void VectorRasterizeThread::SetCurrentProgress( float progress )
{
  ConditionalWait::ScopedLock lock( mConditionalWait );

  if( progress >= mPlayRange.x && progress <= mPlayRange.y )
  {
    mProgress = progress;

    if( mTotalFrame != 0 )
    {
      mCurrentFrame = static_cast< uint32_t >( mTotalFrame * progress + 0.5f );
    }

    mResourceReady = false;

    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::SetCurrentProgress: progress = %f (%d)\n", progress, mCurrentFrame );
  }
}

float VectorRasterizeThread::GetCurrentProgress() const
{
  return ( static_cast< float >( mCurrentFrame ) / static_cast< float >( mTotalFrame ) );
}

DevelImageVisual::PlayState VectorRasterizeThread::GetPlayState() const
{
  return mPlayState;
}

bool VectorRasterizeThread::IsResourceReady() const
{
  return mResourceReady;
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

  mCurrentFrame = std::max( static_cast< uint32_t >( mTotalFrame * mProgress + 0.5f ), mStartFrame );

  mFrameRate = mVectorRenderer.GetFrameRate();
  mFrameDurationNanoSeconds = NANOSECONDS_PER_SECOND / mFrameRate;

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::StartRender: Renderer is started [%d, %f fps]\n", mTotalFrame, mFrameRate );

  return true;
}

void VectorRasterizeThread::Rasterize()
{
  bool needRender, resourceReady;

  {
    ConditionalWait::ScopedLock lock( mConditionalWait );
    needRender = mNeedRender;
    resourceReady = mResourceReady;
  }

  auto currentFrameStartTime = std::chrono::system_clock::now();

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

          // Reset the current frame and the current loop
          mCurrentFrame = mStartFrame;
          mCurrentLoop = 0;

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

  if( needRender )
  {
    mNeedRender = false;
  }

  if( !resourceReady )
  {
    DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: Resource ready trigger\n" );

    mResourceReadyTrigger->Trigger();
    mResourceReady = true;
  }

  auto timeToSleepUntil = currentFrameStartTime + std::chrono::nanoseconds( mFrameDurationNanoSeconds );

#if defined(DEBUG_ENABLED)
  auto sleepDuration = std::chrono::duration_cast< std::chrono::milliseconds >( timeToSleepUntil - std::chrono::system_clock::now() );

  DALI_LOG_INFO( gVectorAnimationLogFilter, Debug::Verbose, "VectorRasterizeThread::Rasterize: [current = %d, sleep duration = %lld]\n", mCurrentFrame, sleepDuration.count() );
#endif

  std::this_thread::sleep_until( timeToSleepUntil );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
