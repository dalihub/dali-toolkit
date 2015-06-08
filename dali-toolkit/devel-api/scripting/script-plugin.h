#ifndef __DALI_SCRIPT_PLUGIN_H__
#define __DALI_SCRIPT_PLUGIN_H__

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

#include <dali/public-api/common/dali-common.h>
#include <string>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Abstract interface to provide scripting support.
 *
 * A plugin must export the following functions to create / destroy the object
 * CreateScriptPlugin()  // returns a pointer to a ScriptPlugin object
 * DestroyScriptPlugin() // destroys the plugin
 */
class DALI_IMPORT_API ScriptPlugin
{
public:


  /**
   * Function pointer called in toolkit to create a ScriptPlugin plugin instance.
   * @return Pointer to the newly created plugin object
   */
  typedef ScriptPlugin* Create();

  /**
   * Function pointer called in toolkit to Unload the plugin.
   * @param plugin  The plugin object created and returned by CreateScriptPluginPlugin().
   */
  typedef void Destroy(ScriptPlugin* plugin);

  /**
   * virtual Destructor.
   */
  virtual ~ScriptPlugin() {};

  /**
   * Set engine configuration flags
   * @param [in] flags string. Format dependent on the scripting engine.
   */
  virtual void SetFlags(const std::string& flags) = 0;

  /**
   * Exec buffer contents as a script
   * @param buffer script file contents
   * @param filename a nominal name for the buffer contents.
   * (NB filename extension may be used to disambiguate script language)
   */
  virtual void ExecuteBuffer(const std::string& buffer, const std::string& filename) = 0;

  /**
   * Exec file as a script
   * @param filename the filename to read and execute
   */
  virtual void ExecuteFile(const std::string& filename) = 0;

}; // class ScriptPlugin

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_SCRIPT_PLUGIN_H__
