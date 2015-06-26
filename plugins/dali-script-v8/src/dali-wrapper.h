#ifndef __DALI_V8PLUGIN_DALI_WRAP_H__
#define __DALI_V8PLUGIN_DALI_WRAP_H__

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
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <garbage-collector/garbage-collector.h>

#include <module-loader/module-loader.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * Sets up JavaScript context and environment and allows execution of JavaScript in the context
 *
 *
 * Exceptions
 * ----------
 *
 * Execptions can be raised by calling DALI_SCRIPT_EXCEPTION()
 *
 * Expections break JavaScript execution with a printout but do not cause a c++ exception.
 * So always return from the calling function before attempting to run any further V8 code.
 *
 * Coding Style
 * ------------
 *
 *  Uses Tizen Web API coding style, which appears to be the same as this:
 *
 *  https://google-styleguide.googlecode.com/svn/trunk/javascriptguide.xml
 *
 * "In general, use
 *
 * functionNamesLikeThis,
 * variableNamesLikeThis,
 * ClassNamesLikeThis,
 * ConstructorsLikeThis,
 * EnumNamesLikeThis,
 * methodNamesLikeThis,
 * CONSTANT_VALUES_LIKE_THIS,
 * foo.namespaceNamesLikeThis.bar,
 * filenameslikethis.js. "
 */
class DALI_INTERNAL DaliWrapper
{

public:

  /**
   * @brief whether the wrapper is running in standalone using V8,
   * or inside Node.JS using V8
   */
  enum RunMode
  {
    RUNNING_STANDALONE,
    RUNNING_IN_NODE_JS
  };

  /**
   * @brief Constructor
   * @param[in] runMode whether the wrapper is running standalone or inside Node.JS
   * @param[in] isolate v8 isolate ( can be null if running standalone )
   */
  DaliWrapper( RunMode runMode, v8::Isolate* isolate );

  /**
   * non virtual destructor, not intended as a base class
   */
  ~DaliWrapper();

  /**
   * Gets the DaliWrapper singleton.
   *
   * @return the wrapper
   */
  static DaliWrapper& Get();

  /**
   * Intialize DaliWrapper for running inside NodeJS
   */
  static v8::Local<v8::Object> CreateWrapperForNodeJS( v8::Isolate* isolate);


  /**
   * Set V8 engine configuration flags
   *
   * @param[in] flags Configruation flags (See v8 documentation)
   */
  void SetFlagsFromString(const std::string& flags);

  /**
   * Delete the current V8 context
   */
  void Shutdown();

  /**
   * Excute the buffer in the v8 context
   *
   * @param[in] sourceCode The buffer containing javascript to execute
   * @param[in] sourceFileName Filename associated with the buffer (for error tracing)
   * @return true on success, false on failure
   */
  bool ExecuteBuffer(const std::string &sourceCode, const std::string &sourceFileName);


  /**
   * Excute the buffer in the v8 context
   *
   * @param[in] sourceFileName Filename associated with the buffer (for error tracing)
   * @return true on success, false on failure
   */
  bool ExecuteFile( const std::string& sourceFileName);


  /**
   * Get DALi's own garbage collector
   * @return garbage collector interface
   */
  GarbageCollectorInterface& GetDaliGarbageCollector();

private:

  /**
   * @brief Apply global objects like console.log and require() to the context
   */
  void ApplyGlobalObjectsToContext( v8::Local<v8::Context> context );

  /**
   * @brief Initialize DaliWrapper for running standalone
   * Creates a new isolate
   *
   */
  void InitializeStandAlone();

  /**
   * @brief create dali namespace/object
   */
  v8::Local<v8::Object> CreateDaliObject();

  /**
   * Create Dali ObjectTemplate
   * @return the Dali ObjectTemplate
   */
  static v8::Handle<v8::ObjectTemplate> NewDaliObjectTemplate( v8::Isolate* isolate );

  /**
   * Called when require keyword is found in a script
   */
  static void Require(const v8::FunctionCallbackInfo< v8::Value >& args);

  static bool mInstanceCreated;                                 ///< whether an instance has been created
  static DaliWrapper* mWrapper;                                 ///< static pointer to the wrapper

  GarbageCollector mGarbageCollector;                           ///< DALi garbage collector
  ModuleLoader mModuleLoader;                                   ///< Module loader
  v8::Persistent<v8::Context> mContext;                         ///< A sandboxed execution context with its own set of built-in objects and functions.
  v8::Isolate* mIsolate;                                        ///< represents an isolated instance of the V8 engine.
  RunMode mRunMode;
};


} // namespace V8Plugin

} // namespace Dali

#endif // header
