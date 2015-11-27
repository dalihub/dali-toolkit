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

// CLASS HEADER
#include "module-loader.h"

// INTERNAL INCLUDES
#include <v8-utils.h>

namespace Dali
{

namespace V8Plugin
{

ModuleLoader::ModuleLoader()
{
}

ModuleLoader::~ModuleLoader()
{
  VectorBase::SizeType count =  mModules.Count();
  for( VectorBase::SizeType  i = 0; i < count ; ++i)
  {
    Module* module = mModules[i];
    delete module;
  }
  mModules.Clear();
}

bool ModuleLoader::CompileAndRun(v8::Isolate* isolate,
    const std::string& sourceCode,
    const std::string& sourceFileName )
{

  v8::HandleScope handleScope( isolate );
  v8::TryCatch tryCatch;

  // convert from string to v8 string
  v8::Local<v8::String> source = v8::String::NewFromUtf8( isolate, sourceCode.c_str());
  v8::Local<v8::String> file = v8::String::NewFromUtf8( isolate, sourceFileName.c_str());
  v8::ScriptOrigin origin(file);

  // Compile the script
  v8::Local<v8::Script> script = v8::Script::Compile( source, &origin);

  // See if an exception has been thrown
  if( tryCatch.HasCaught() || script.IsEmpty() )
  {
    // Print errors that happened during compilation.
    V8Utils::ReportException( isolate, &tryCatch );
    return false;
  }

  // Run the script
  v8::Local<v8::Value> result = script->Run();

  // in V8 test code ( they check for an exception and empty return object )
  if( tryCatch.HasCaught() || result.IsEmpty() )
  {
    // Print errors that happened during execution.
    V8Utils::ReportException( isolate, &tryCatch);
    return false;
  }

  return true;
}

bool ModuleLoader::ExecuteScript( v8::Isolate* isolate,
                                  const std::string& sourceCode,
                                  const std::string& sourceFileName )
{
  StoreScriptInfo(  sourceFileName );

  return CompileAndRun(isolate, sourceCode, sourceFileName );
}

bool ModuleLoader::ExecuteScriptFromFile( v8::Isolate* isolate,
                                          const std::string& fileName  )
{
  std::string contents;

  V8Utils::GetFileContents( fileName, contents );

  if( contents.empty() )
  {
    return false;
  }

  return ExecuteScript( isolate, contents, fileName );
}

/**
 * ### var module = require("moduleName");
 *
 *
 * There is no standard for modules or the 'require' keyword in JavaScript.<br />
 * However CommonJS have this: http://wiki.commonjs.org/wiki/Modules/1.1.1  ( used by Node.js).
 * <br /> <br />
 *
 * The concept behind 'require' keyword is simple, it allows you to include another
 * JavaScript file, which exports an API / function / constructor / singleton.
 *
 *
 *     // example_module.js
 *     exports.hello = function() { return "hello world" }
 *
 * <br />
 *
 *     // main.js
 *     var example = require( "example_module.js");
 *
 *     log( example.hello() );
 *
 *

 * ### Module writers guide:
 *
 *
 * #### Exporting as a namespace
 *
 * Example of using a namespace to export functions / objects.
 *
 *     // filesystem-helper.js
 *     exports.version = "FileSystem helper 1.0";
 *     exports.open = function() { }
 *     exports.close = function() { }
 *     exports.read = function() { }
 *     exports.write = function() { ... }
 *     exports.fileSize = function() {...}
 *
 * <br />
 *
 *     // main.js
 *     var fs = require( "filesystem-helper.js");
 *
 *     log( fs.version );
 *
 *     var file = fs.open("myfile.txt");
 *     var data = fs.read( file );
 *
 *
 *
 * #### Exporting as a function
 *
 * In this example we are using module.exports directly to change it
 * from an object literal with name-value pairs (exports object) to a function.
 *
 *     // my_first_module.js
 *     module.exports = function() {  log("helloWorld"); }
 *
 * <br />
 *
 *     // main.js
 *     var func = require("my_first_module.js");
 *     func();      // prints out hello-world
 *
 *
 * #### Exporting as a constructor
 *
 *
 *     // ImageView.js
 *     function ImageView( position, orientation, image, name )
 *     {
 *         this = new dali.Control("ImageView");
 *         this.image = image;
 *         this.position = position;
 *         this.orientation = orientation;
 *         this.name = name;
 *     }
 *     module.exports = ImageView;
 *
 * <br />
 *
 *     // main.js
 *
 *     var ImageView = require(" ImageView.js");
 *
 *     var imageView = new ImageView( position, orientation, image, "my first image actor");
 *
 * #### Exporting as a singleton
 *
 * By exporting a singleton you have an object which has shared state between
 * any modules using it.
 *
 * example:
 *
 *     // image-database.js
 *
 *     function ImageDatabase( )
 *     {
 *       this.addImage  = function() {  ... };
 *       this.removeImage = function() { ... };
 *       this.getImage = function()  { ...};
 *       this.getImageCount = function() { ... };
 *     }
 *
 *     module.exports = new ImageDatabase();
 *
 *
 * <br />
 *
 *     // main.js
 *
 *     var database = require('image-database.js');
 *
 *     database.addImage( myImage );
 *
 * <br />
 *
 *     // another-module.js
 *     var database = require('image-database.js');
 *
 *     // gets the same database object as main.js
 *
 *
 * The first call to require('image-database.js') will create the image database.
 * Further calls, will return the same instance, because require caches module.exports.
 * Otherwise it would have to recompile and run the module every time require is called.
 *
 * ## Notes
 *
 * #### Automatic wrapping of a module by DALi:
 *
 * The module is automatically wrapped in a function by DALi before being executed ( similar technique to Node.js). </br>
 * This is to prevent any functions / variables declared by the module entering the global namespace. </br>
 * Currently the module will have access to all DALi global functions, like log, require and the DALi API ( actors / stage etc).</br>
 *
 *
 *     // Parameters passed to the internally generated function
 *     // module = reference to current module
 *     // module.exports = defines what the module exports
 *     // exports = reference to module.exports
 *     // __filename = module filename
 *     // __dirname = module directory
 *
 *     function createModuleXYZ( exports ( === module.exports), module, __filename, __dirname )
 *     {
 *       //
 *       // Module code automatically inserted here.
 *       //
 *       log(" my first module ");
 *       var version = "1.3";      // this won't pollute global namespace
 *       exports.version = version;
 *       exports.logActorPosition = function( actorName )
 *       {
 *         var actor = dali.stage.getRootLayer().findChildByName(actorName );
 *         log( actor.x + "," + actor.y + "," + actor.z );
 *        }
 *       //
 *       // End module code
 *       //
 *
 *       return module.exports;
 *     }

 *
 * Initially module.exports is an object literal with name-value pairs ( exports object).
 * However it can be re-assigned to a constructor / function / singleton object as shown
 * in the examples above.
 *
 *
 *  ### Circular dependencies:
 *
 *  DALi JS supports circular dependencies as required by the CommonJS specification.
 *
 *  #### a.js
 *
 *
 *     export.version = "1.3"
 *     export.loaded = false;
 *     var bModule = require('b.js')
 *     export.loaded = true;
 *
 *  #### b.js
 *
 *     var aModule = require('a.js')
 *     log( "aModule version = " + aModule.version + ", aModule loaded = " + aModule.loaded );
 *
 *     //prints  aModule = 1.3, aModule loaded = false
 *
 *  #### main.js
 *
 *      var aModule = require("a.js");
 *
 *
 *  When b.js requires a.js, it is given everything that is exported from a.js, up to the point
 *  b.js is required by a.js.
 *
 * ### 'require' background
 *
 * There is alternative to module spec in CommonJS called RequireJs ( http://requirejs.org/docs/node.html) <br />
 * DALi JS tries to follows the CommonJS  specification (used by Node.js) as it
 * is supposed to be better suited to server side development. <br /><br />
 *
 * @method require
 * @for ModuleLoader
 *
 */
void ModuleLoader::Require(const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  std::string fileName = V8Utils::GetStringParameter( PARAMETER_0, found, isolate , args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "require missing module name");
    return;
  }

  // strip off any path / .js
  std::string moduleName;
  V8Utils::GetModuleName( fileName, moduleName );

  // see if the module already exists
  const Module* existingModule = FindModule( moduleName );
  if( existingModule )
  {
    // printf(" using existing module %s \n",moduleName.c_str() );
    args.GetReturnValue().Set( existingModule->mExportsObject );
    return;
  }

  std::string path = mCurrentScriptPath;  // path of top level script being executed
  std::string contents;
  V8Utils::GetFileContents(path + fileName, contents);

  // wrap the module in a function to protect global namespace.
  // the create function itself is global so we make it unique for each module
  // For reference nodeJs does this as an anonymous function, but we're calling it from native side
  // so need to pass parameters  / get a name for it.
  std::string functionName ="__createModule" +  moduleName;
  std::string source = "function " + functionName + "( exports, module, __filename, __directory)  { ";
  source+= contents;
  source+=" \n };";  // close the function

  CompileAndRun( isolate, source, fileName );

  // We need to create module object, so that the module can read / write properties to it

  v8::Local<v8::Object> moduleObject = v8::Object::New( isolate );
  v8::Local<v8::Object> exportsObject = v8::Object::New( isolate );
  moduleObject->Set( v8::String::NewFromUtf8( isolate, "exports"),  exportsObject );
  moduleObject->Set( v8::String::NewFromUtf8( isolate, "id"), v8::String::NewFromUtf8( isolate ,moduleName.c_str() ) );

  // store the module exports object now, this is to allow for circular dependencies.
  // If this-module requires another module, which then requires this module ( creating a circle), it will be given an export object
  // which contains everything exported so far.
  Module* module = StoreModule( path, fileName, moduleName, isolate, exportsObject );

  v8::Local<v8::Context> currentContext =  isolate->GetCurrentContext();

  // get the CreateModule function
  v8::Local<v8::Function> createModule = v8::Local<v8::Function>::Cast(currentContext->Global()->Get(v8::String::NewFromUtf8( isolate, functionName.c_str() )));

  // add the arguments
  std::vector< v8::Local<v8::Value> > arguments;
  arguments.push_back( exportsObject );
  arguments.push_back( moduleObject );
  arguments.push_back( v8::String::NewFromUtf8( isolate, fileName.c_str() ));
  arguments.push_back( v8::String::NewFromUtf8( isolate, path.c_str() ));


  // call the CreateModule function
  createModule->Call( createModule, arguments.size(), &arguments[0]); //[0]

  // get the module.export object, the module writer may have re-assinged module.exports, so the exports object
  // no longer references it.
  v8::Local<v8::Value> moduleExportsValue = moduleObject->Get( v8::String::NewFromUtf8( isolate, "exports"));
  v8::Local<v8::Object>  moduleExports = moduleExportsValue->ToObject();

  // Re-store the export ( possible nothing happens, because exports hasn't been re-assigned).
  module->mExportsObject.Reset( isolate, moduleExports);

  args.GetReturnValue().Set( moduleExports );

}

void ModuleLoader::StorePreBuiltModule( v8::Isolate* isolate, v8::Local<v8::Object>& exportObject, const std::string& name )
{
  StoreModule( "", name, name, isolate, exportObject );
}

void ModuleLoader::StoreScriptInfo( const std::string& sourceFileName )
{
  V8Utils::GetFileDirectory( sourceFileName, mCurrentScriptPath);
}

Module* ModuleLoader::StoreModule( const std::string& path,
                                const std::string& fileName,
                                const std::string& moduleName,

                                v8::Isolate* isolate,
                                v8::Local<v8::Object>& moduleExportsObject )
{
  Module* module = new Module( path, fileName, moduleName, isolate, moduleExportsObject );
  mModules.PushBack( module );
  return module;

}

const Module* ModuleLoader::FindModule( const std::string& moduleName )
{
  VectorBase::SizeType count =  mModules.Count();
  for( VectorBase::SizeType  i = 0; i < count ; ++i)
  {
    const Module* module = mModules[i];
    if (module->mModuleName == moduleName )
    {
      return module;
    }
  }
  return NULL;
}


} // V8Plugin

} // Dali
