/* Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <node.h>
#include <public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>
#include <integration-api/core.h>
#include <integration-api/adaptors/adaptor.h>
#include <public-api/adaptor-framework/window.h>
#include <devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-wrapper.h>
#include "javascript-application-options.h"

using namespace Dali;

namespace DaliNodeAddon
{

class DaliApplication
{
public:

    DaliApplication()
    : mInitialized( false),
      mSingletonService(SingletonService::New()),
      mAdaptor( NULL )
    {
    }

    ~DaliApplication()
    {
      mSingletonService.UnregisterAll();
      delete mAdaptor;
      mWindow.Reset();
    }

    bool Initialized() const
    {
      return mInitialized;
    }

    void Initialize(  ApplicationOptions options )
    {
      if( mInitialized )
      {
        return;
      }

      // 1. Create the window ( adaptor requires a window)
      const WindowOptions& window( options.window);

      mWindow = Window::New( window.positionSize, window.name, window.transparent );

      // 2. create the adaptor
      Adaptor* adaptor = &Adaptor::New( mWindow );

      // 3. start the adaptor
      adaptor->Start();

      // Set the view modes

      if( options.stereo.viewMode > Dali::MONO )
      {
        adaptor->SetStereoBase( options.stereo.stereoBase );
        adaptor->SetViewMode( options.stereo.viewMode );
      }

      // fire the scene create signal
      adaptor->NotifySceneCreated();

      mInitialized = true;
    }
private:

    bool mInitialized;
    SingletonService mSingletonService;
    Adaptor* mAdaptor;
    Window mWindow;

};

DaliApplication app;

void CreateDali(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);

  ApplicationOptions options;

  bool ok = GetApplicationOptions( args, options);
  if (!ok )
  {
    isolate->ThrowException( v8::Exception::TypeError( v8::String::NewFromUtf8(isolate, "Please check arguments passed to DALi require") ) );
    return;
  }

  app.Initialize( options );

  // the return value from calling the function   require('dali.js)(  options )
  // is the dali object
  args.GetReturnValue().Set( V8Plugin::DaliWrapper::CreateWrapperForNodeJS( isolate ) );
}


/**
 * We make module.exports a function so that the developer can pass to
 * parameters to DALi when it's 'required'
 * E.g
 *
 *
 * var window= {
 *        x:10,
 *        y:10,
 *        width:800,
 *        height: 600,
 *        transparent: false,
 *        name:'my-first-dali-app'
 * };
 *
 * var viewMode {
 *       'stereoscopic-mode':'stereo-vertical', // mono, stereo-horizontal, stereo-vertical, stereo-interlaced,
 *       'stereo-base': 65 // Distance in millimeters between left/right cameras typically between (50-70mm)
 * };
 *
 * var options= {
 *    'window': window,
 *    'view-mode': viewMode,
 *    'style-sheet': 'my-theme.json'
 * }
 *
 * var dali = require('dali.js')( options )
 *
 *
 */
void ExportDaliModule(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module)
{
  NODE_SET_METHOD(module, "exports", CreateDali);
}

} // namespace DaliNodeAddon

NODE_MODULE(dali, DaliNodeAddon::ExportDaliModule)




