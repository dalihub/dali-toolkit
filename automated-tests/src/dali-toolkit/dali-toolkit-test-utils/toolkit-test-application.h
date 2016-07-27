#ifndef __DALI_TOOLKIT_TEST_APPLICATION_H__
#define __DALI_TOOLKIT_TEST_APPLICATION_H__

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

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/adaptors/adaptor.h>
#include <toolkit-adaptor-impl.h>

namespace Dali
{

/**
 * Adds some functionality on top of TestApplication that is required by the Toolkit.
 */
class ToolkitTestApplication : public TestApplication
{
public:

  ToolkitTestApplication( size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                          size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                          float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                          float  verticalDpi   = DEFAULT_VERTICAL_DPI )
  : TestApplication( false, surfaceWidth, surfaceHeight, horizontalDpi, verticalDpi )
  {
    Initialize();

    // set the DPI value for font rendering
    Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
    if( fontClient )
    {
      fontClient.SetDpi( mDpi.x, mDpi.y );
    }
  }

  ~ToolkitTestApplication()
  {
    // Need to delete core before we delete the adaptor.
    delete mCore;
    mCore = NULL;
  }

  //ToolkitOrientation& GetOrientation()
  //{
  //return mOrientation;
  //}

  /**
   * @brief Creates an adaptor implementation for those controls like the
   * text-field and the text-editor which connects a callback to the idle signal.
   */
  void CreateAdaptor()
  {
    Adaptor::Get();
  }

  /**
   * @brief Executes the idle callbacks.
   *
   * Some controls like the text-field and the text-editor connect callbacks to the
   * idle signal.
   */
  void RunIdles()
  {
    if( Adaptor::IsAvailable() )
    {
      for( Vector<CallbackBase*>::Iterator it = Internal::Adaptor::Adaptor::mCallbacks.Begin(),
             endIt = Internal::Adaptor::Adaptor::mCallbacks.End();
           it != endIt;
           ++it )
      {
        CallbackBase* callback = *it;

        CallbackBase::Execute( *callback );
      }

      Internal::Adaptor::Adaptor::mCallbacks.Clear();
    }
  }

private:
  //ToolkitOrientation mOrientation;
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TEST_APPLICATION_H__
