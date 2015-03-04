/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "dali-script-v8.h"

// EXTERNAL INCLUDES
#include <iostream>
#include <fstream>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-script-v8.h>
#include <dali-wrapper.h>
#include <utils/v8-utils.h>

// The plugin factories
extern "C" DALI_EXPORT_API Dali::Toolkit::ScriptPlugin* CreateScriptPlugin(void)
{
  return new Dali::V8Plugin::DaliScriptV8;
}

extern "C" DALI_EXPORT_API void DestroyScriptPlugin(Dali::Toolkit::ScriptPlugin* plugin)
{
  delete plugin;
}

namespace Dali
{
namespace V8Plugin
{

DaliScriptV8::DaliScriptV8()
{
}

DaliScriptV8::~DaliScriptV8()
{
  DaliWrapper::Get().Shutdown();
}

void DaliScriptV8::SetFlags(const std::string& s)
{
  DaliWrapper::Get().SetFlagsFromString(s);
}

void DaliScriptV8::ExecuteBuffer(const std::string& buffer, const std::string& filename)
{
  DaliWrapper::Get().ExecuteBuffer(buffer, filename);
}

void DaliScriptV8::ExecuteFile(const std::string& filename)
{


  DaliWrapper::Get().ExecuteFile( filename );
}

}  // namespace V8Plugin

}  // namespace Dali

