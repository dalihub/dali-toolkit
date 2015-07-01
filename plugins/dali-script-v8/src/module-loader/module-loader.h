#ifndef __DALI_V8PLUGIN_MODULE_LOADER_H__
#define __DALI_V8PLUGIN_MODULE_LOADER_H__

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
#include <v8.h>
#include <string.h>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <module-loader/module.h>

namespace Dali
{

namespace V8Plugin
{

/**
 *
 * Responsible for executing JavaScript source code.
 * This includes loading other JavaScript files that may be referenced
 * inside JavaScript using the 'require' keyword.
 *
 * Notes:
 *
 * A module named with a forward slash is loaded as an absolute path:
 * E.g. require "/usr/apps/scripts/my_module.js"
 *
 * A module pre-fixed with a dot slash ( current directory) then the path is relative
 * to the calling scripts path. E.g.
 *
 * -- my_first.js --
 * require "./circle.js";   // load circle.js from same directory as my_first.js
 *
 * @TODO
 * ----------
 * Without a leading '/' or './' the module is loaded from the DALi modules directory or
 * the applications user defined module directory.
 * E.g. require "dali-spline.js"    // search dali-module directory first, then any user
 * defined module locations.
 * ------------
 *
 *
 */
class ModuleLoader
{
public:

  /**
   * Constructor
   */
  ModuleLoader();

  /**
   * non virtual destructor, not intended as a base class
   */
  ~ModuleLoader();


  /**
   * Execute a script from a file
   * @param[in] fileName file name
   * @return true on success, false on failure
   *
   */
  bool ExecuteScriptFromFile( v8::Isolate* isolate, const std::string& fileName );


  /**
   * Execute a script
   * @param[in] sourceCode source code to run
   * @param[in] sourceFileName source file name
   * @return true on success, false on failure
   */
  bool ExecuteScript( v8::Isolate* isolate,
                      const std::string& sourceCode,
                      const std::string& sourceFileName );


  /**
   * Implements JavaScript Require functionality
   */
  void Require(const v8::FunctionCallbackInfo< v8::Value >& args, v8::Persistent<v8::ObjectTemplate>& globalObjectTemplate );

private:

  /**
   * Compile and run the JavaScript code
   * @param[in] sourceCode source code to run
   * @param[in] sourceFileName source file name
   */
  bool CompileAndRun(v8::Isolate* isolate,
                     const std::string& sourceCode,
                     const std::string& sourceFileName );

  /**
   * Store information about the current script
   * @param[in] sourceFileName source file name
   */
  void StoreScriptInfo( const std::string& sourceFileName );

  /**
   * Store module information
   * @param[in] sourceFileName source file name
   * @return module object
   */
  Module* StoreModule( const std::string& path,
                    const std::string& fileName,
                    const std::string& moduleName,
                    v8::Isolate* isolate,
                    v8::Local<v8::Object>& moduleExportsObject );

  /**
   * Find a module
   * @param[in] moduleName module name
   * @return module
   */
  const Module* FindModule( const std::string& moduleName );

private:

  Dali::Vector< Module*> mModules; ///< vector of modules
  std::string mCurrentScriptPath;  ///< path of the current script being executed (via ExecuteScript).

};


} // namespace V8Plugin

} // namespace Dali

#endif // header MODULE_LOADER
