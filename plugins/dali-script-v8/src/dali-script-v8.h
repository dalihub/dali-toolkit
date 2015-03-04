#ifndef __DALI_V8PLUGIN_SCRIPT_H__
#define __DALI_V8PLUGIN_SCRIPT_H__

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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/scripting/script-plugin.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace V8Plugin
{

/**
 * Executes JavaScript V8 scripts
 */
class  DaliScriptV8 : public Dali::Toolkit::ScriptPlugin
{
public: // Construction & Destruction

  /**
   * Constructor
   */
  DaliScriptV8();

  /**
   * Destructor
   */
  virtual ~DaliScriptV8();

public: // ScriptV8Plugin overrides

  /**
   * @copydoc Dali::Toolkit::ScriptPlugin::SetFlags()
   */
  virtual void SetFlags(const std::string& s);

  /**
   * @copydoc Dali::Toolkit::ScriptPlugin::ExecuteBuffer()
   */
  virtual void ExecuteBuffer(const std::string& buffer, const std::string& filename);

  /**
   * @copydoc Dali::Toolkit::ScriptPlugin::ExecuteFile()
   */
  virtual void ExecuteFile(const std::string& fileName);


};

}  // namespace V8Plugin

}  // namespace Dali

#endif // __FEEDBACK_PLUGIN_H__
