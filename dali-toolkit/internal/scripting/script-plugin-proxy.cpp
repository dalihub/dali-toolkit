/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "script-plugin-proxy.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <dlfcn.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScriptPluginProxy::ScriptPluginProxy( const std::string& sharedObjectName)
: mLibHandle(NULL),
  mCreatePluginFunctionPtr(NULL),
  mDestroyPluginFunctionPtr(NULL),
  mScriptingPlugin(NULL),
  mSharedObjectName(sharedObjectName),
  mIsInitialized(false)
{
  Initialize();
}

ScriptPluginProxy::~ScriptPluginProxy()
{
  UnInitialize();
}

void ScriptPluginProxy::SetFlags(const std::string& flags)
{
  if( mIsInitialized )
  {
    mScriptingPlugin->SetFlags( flags );
  }
}

void ScriptPluginProxy::ExecuteBuffer(const std::string &buffer, const std::string &filename)
{
  if( mIsInitialized )
  {
    mScriptingPlugin->ExecuteBuffer( buffer, filename );
  }
}

void ScriptPluginProxy::ExecuteFile(const std::string &filename)
{
  if( mIsInitialized )
  {
    mScriptingPlugin->ExecuteFile( filename );
  }
}

bool ScriptPluginProxy::IsInitialized() const
{
  return mIsInitialized;
};


void ScriptPluginProxy::Initialize()
{
  if( mIsInitialized )
  {
    return;
  }

  // dl library maintains link counts if you call this twice on the same library
  // (so its okay to do but we should close every handle we get too)
  mLibHandle = dlopen( mSharedObjectName.c_str(), RTLD_NOW | RTLD_GLOBAL );
  if( !mLibHandle )
  {
    DALI_LOG_ERROR( "Cannot load dali script plugin. %s\n", dlerror() );
    return;
  }

  // reset errors
  dlerror();

  // load plugin
  mCreatePluginFunctionPtr = reinterpret_cast<ScriptPlugin::Create*>( dlsym( mLibHandle, "CreateScriptPlugin" ) );
  if( !mCreatePluginFunctionPtr )
  {
    DALI_LOG_ERROR( "Cannot load symbol CreateScriptPlugin(). %s\n", dlerror() );
    return;
  }

  // reset errors
  dlerror();

  mDestroyPluginFunctionPtr = reinterpret_cast<ScriptPlugin::Destroy*>( dlsym( mLibHandle, "DestroyScriptPlugin" ) );
  if( !mDestroyPluginFunctionPtr )
  {
    DALI_LOG_ERROR( "Cannot load symbol:DestroyScriptPlugin(). %s\n", dlerror() );
    return;
  }

  // reset errors
  dlerror();

  mScriptingPlugin = mCreatePluginFunctionPtr();

  if( !mScriptingPlugin )
  {
    DALI_LOG_ERROR( "Call to function CreateFeedbackPlugin() failed\n" );
    return;
  }

  mIsInitialized = true;
}

void ScriptPluginProxy::UnInitialize()
{
  if( mScriptingPlugin )
  {
    mDestroyPluginFunctionPtr( mScriptingPlugin );
  }

  if( mLibHandle )
  {
    if( dlclose( mLibHandle ) )
    {
      DALI_LOG_ERROR( "Error closing dali plugin library: %s\n", dlerror() );
    }
  }
  mIsInitialized = false;
}
} // namespace Adaptor

} // namespace Internal

} // namespace Dali
