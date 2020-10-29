/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-thread.h>

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

VectorAnimationManager::VectorAnimationManager()
: mEventCallbacks(),
  mLifecycleObservers(),
  mVectorAnimationThread( nullptr ),
  mProcessorRegistered( false )
{
}

VectorAnimationManager::~VectorAnimationManager()
{
  for( auto&& iter : mEventCallbacks )
  {
    delete iter;
  }
  mEventCallbacks.clear();

  if( mProcessorRegistered )
  {
    Adaptor::Get().UnregisterProcessor( *this );
  }

  for( auto observer : mLifecycleObservers )
  {
    observer->VectorAnimationManagerDestroyed();
  }
}

void VectorAnimationManager::AddObserver( VectorAnimationManager::LifecycleObserver& observer )
{
  DALI_ASSERT_DEBUG( mLifecycleObservers.end() == std::find( mLifecycleObservers.begin(), mLifecycleObservers.end(), &observer));
  mLifecycleObservers.push_back( &observer );
}

void VectorAnimationManager::RemoveObserver( VectorAnimationManager::LifecycleObserver& observer)
{
  auto iterator=std::find(mLifecycleObservers.begin(), mLifecycleObservers.end(), &observer);
  if( iterator != mLifecycleObservers.end() )
  {
    mLifecycleObservers.erase(iterator);
  }
}

VectorAnimationThread& VectorAnimationManager::GetVectorAnimationThread()
{
  if( !mVectorAnimationThread )
  {
    mVectorAnimationThread = std::unique_ptr< VectorAnimationThread >( new VectorAnimationThread() );
    mVectorAnimationThread->Start();
  }
  return *mVectorAnimationThread;
}

void VectorAnimationManager::RegisterEventCallback( CallbackBase* callback )
{
  mEventCallbacks.push_back( callback );

  if( !mProcessorRegistered )
  {
    Adaptor::Get().RegisterProcessor( *this );
    mProcessorRegistered = true;
  }
}

void VectorAnimationManager::UnregisterEventCallback( CallbackBase* callback )
{
  auto iter = std::find( mEventCallbacks.begin(), mEventCallbacks.end(), callback );
  if( iter != mEventCallbacks.end() )
  {
    mEventCallbacks.erase( iter );

    if( mEventCallbacks.empty() )
    {
      if( Adaptor::IsAvailable() )
      {
        Adaptor::Get().UnregisterProcessor( *this );
        mProcessorRegistered = false;
      }
    }
  }
}

void VectorAnimationManager::Process()
{
  for( auto&& iter : mEventCallbacks )
  {
    CallbackBase::Execute( *iter );
    delete iter;
  }
  mEventCallbacks.clear();

  Adaptor::Get().UnregisterProcessor( *this );
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
