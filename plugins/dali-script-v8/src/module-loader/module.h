#ifndef __DALI_V8PLUGIN_MODULE_H__
#define __DALI_V8PLUGIN_MODULE_H__

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
#include <string>

namespace Dali
{

namespace V8Plugin
{

/**
 *
 * Module class,
 * contains
 *
 */
class Module
{
public:

    Module(const std::string& path,
          const std::string& fileName,
          const std::string& moduleName,

        v8::Isolate* isolate,
        v8::Local<v8::Object>& exportsObject);

    ~Module();

    std::string mFileName;
    std::string mPath;
    std::string mModuleName;


    /**
     *  holds the exports object
     *  e.g.  exports.hello = function() {" hello world"}
     */
    v8::Persistent<v8::Object> mExportsObject;

    /*
     * Currently we allow the modules context to be released after it has been
     * compiled and run ( so we don't need to store it).
     * Data is shared between contexts which suggests
     * the module exports object can be kept alive if the context it was created in is deleted.
     * If this is not the case, then start to store the context.
     */
    // v8::Persistent<v8::Context> mContext.

private:

    Module();

    Module( const Module& );            ///< undefined copy constructor
    Module& operator=( const Module& ); ///< undefined assignment operator



};

} // namespace V8Plugin

} // namespace Dali

#endif // header MODULE_LOADER
