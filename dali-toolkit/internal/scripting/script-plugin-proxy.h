#ifndef __DALI_INTERNAL_SCRIPT_PLUGIN_PROXY_H__
#define __DALI_INTERNAL_SCRIPT_PLUGIN_PROXY_H__

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

#include <dali-toolkit/devel-api/scripting/script-plugin.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief Proxy class to dynamically load, use and unload script plugin.
 *
 *
 */
class ScriptPluginProxy : public ScriptPlugin
{
public:

  /**
   * @brief Constructor
   * @param sharedObjectName so file to load
   */
  ScriptPluginProxy( const std::string& sharedObjectName);

  /**
   *  @brief destructor
   */
  virtual ~ScriptPluginProxy();

  /**
   * Set engine configuration flags
   * @param [in] flags string. Format dependent on the scripting engine.
   */
  virtual void SetFlags( const std::string& flags );

  /**
   *  @brief Exececute the buffer contents as a script
   * @param [in] buffer script file contents
   * @param [in] filename a nominal name for the buffer contents.
   * (NB filename extension may be used to disambiguate script language)
   */
  virtual void ExecuteBuffer( const std::string& buffer, const std::string& filename );

  /**
   * @brief  execture the file as a script
   * @param [in] filename the filename to read and execute
   */
  virtual void ExecuteFile( const std::string& fileName );

  /**
   * @brief check if the plugin is initialized
   * @return true if it's initialized
   */
  bool IsInitialized() const;

private:
  /**
   * @brief Dynamically loads the script plugin.
   */
  void Initialize();

  /**
   * @brief Unloads the script plugin.
   */
  void UnInitialize();

private:

  void* mLibHandle;
  ScriptPlugin::Create* mCreatePluginFunctionPtr;
  ScriptPlugin::Destroy* mDestroyPluginFunctionPtr;
  ScriptPlugin* mScriptingPlugin;
  std::string mSharedObjectName;
  bool mIsInitialized;

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCRIPT_PLUGIN_PROXY_H__
