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
#include "script-impl.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include "script-plugin-proxy.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char* PLUGIN_FILE = "libdali-script-plugin-v8.so";
}

void Script::ExecuteFile( const std::string& filename )
{
  if( mPlugin )
  {
    mPlugin->ExecuteFile(filename);
  }
}

Script::Script(void) : mPlugin(NULL)
{
  ScriptPluginProxy *plugin = new ScriptPluginProxy( PLUGIN_FILE );

  if( mPlugin )
  {
    DALI_LOG_WARNING("Reloading script plugin %s, is this what you wanted to do?",PLUGIN_FILE);
    delete mPlugin;
    mPlugin = NULL;
  }

  if( plugin->IsInitialized() )
  {
    mPlugin = plugin;
  }
  else
  {
    delete plugin;
  }

}

Script::~Script()
{
  if( mPlugin )
  {
    delete mPlugin;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
