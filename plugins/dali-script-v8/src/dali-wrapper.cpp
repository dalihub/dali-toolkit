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
#include "dali-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <dali/integration-api/debug.h>
#include <actors/actor-wrapper.h>
#include <controls/control-wrapper.h>
#include <stage/stage-wrapper.h>
#include <image/image-wrapper.h>
#include <animation/linear-constrainer-wrapper.h>
#include <animation/path-constrainer-wrapper.h>
#include <animation/path-wrapper.h>
#include <animation/animation-wrapper.h>
#include <controls/item-factory-wrapper.h>
#include <events/pan-gesture-detector-wrapper.h>
#include <object/property-buffer-wrapper.h>
#include <rendering/geometry-wrapper.h>
#include <rendering/material-wrapper.h>
#include <rendering/renderer-wrapper.h>
#include <rendering/shader-wrapper.h>
#include <rendering/sampler-wrapper.h>
#include <shared/object-template-helper.h>
#include <constants/constants-wrapper.h>
#include <toolkit/builder/builder-wrapper.h>
#include <toolkit/focus-manager/keyboard-focus-manager-wrapper.h>


namespace Dali
{

namespace V8Plugin
{

namespace
{
/**
 * This string defines how the global DALi object/namespace is used from JavaScript
 * E.g. new dali.Image or dali.stage.add( )
 */
const char* const DALI_API_NAME = "dali";

/**
 * lookup table for setting up function calls for creating Dali objects
 * e.g.  new dali.TextActor()
 */
const ApiFunction ConstructorFunctionTable[]=
{
    { "Rotation",           PropertyValueWrapper::NewRotation},
    { "Matrix",             PropertyValueWrapper::NewMatrix},
    { "Path",               PathWrapper::NewPath },
    { "PathConstrainer",    PathConstrainerWrapper::NewPathConstrainer},
    { "LinearConstrainer",  LinearConstrainerWrapper::NewLinearConstrainer},
    { "Actor",              ActorWrapper::NewActor },
    { "CameraActor",        ActorWrapper::NewActor },
    { "Layer",              ActorWrapper::NewActor },
    { "Control",            ControlWrapper::NewControl },
    { "ResourceImage",      ImageWrapper::NewImage },
    { "BufferImage",        ImageWrapper::NewImage },
    { "FrameBufferImage",   ImageWrapper::NewImage },
    { "Animation",          AnimationWrapper::NewAnimation},
    { "ItemFactory",        ItemFactoryWrapper::NewItemFactory},
    { "Shader",             ShaderWrapper::NewShader},
    { "Sampler",            SamplerWrapper::NewSampler},
    { "Material",           MaterialWrapper::NewMaterial},
    { "Geometry",           GeometryWrapper::NewGeometry},
    { "Renderer",           RendererWrapper::NewRenderer},
    { "PropertyBuffer",     PropertyBufferWrapper::NewPropertyBuffer},
    { "Builder",            BuilderWrapper::NewBuilder},
    { "PanGestureDetector", PanGestureDetectorWrapper::NewPanGestureDetector},

};

const unsigned int PropertyFunctionTableCount = sizeof(ConstructorFunctionTable)/sizeof(ConstructorFunctionTable[0]);

void FatalErrorCallback(const char* location, const char* message)
{
  DALI_LOG_ERROR("%s, %s \n",location,message);
  DALI_ASSERT_ALWAYS( 0 && "V8 fatal error");
}

#if defined(DEBUG_ENABLED)
// default to verbose logging
Integration::Log::Filter* gLogExecuteFilter( Integration::Log::Filter::New(Debug::Verbose, false, "EXECUTE_JAVASCRIPT") );
#endif
} // un-named name space



bool DaliWrapper::mInstanceCreated = false;
DaliWrapper* DaliWrapper::mWrapper = NULL;

DaliWrapper::DaliWrapper( RunMode runMode, v8::Isolate* isolate )
:mIsolate( isolate ),
 mRunMode(runMode)
{
}

DaliWrapper::~DaliWrapper()
{
  mInstanceCreated = false;
}

DaliWrapper& DaliWrapper::Get()
{
  if( !mInstanceCreated )
  {
    mWrapper = new DaliWrapper( RUNNING_STANDALONE, NULL );

    mInstanceCreated = true;

    mWrapper->InitializeStandAlone();

  }
  return *mWrapper;
}

v8::Local<v8::Object> DaliWrapper::CreateWrapperForNodeJS( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate);

  mInstanceCreated = true;

  mWrapper = new DaliWrapper( RUNNING_IN_NODE_JS, isolate );

  v8::Local<v8::Object> dali = mWrapper->CreateDaliObject();

  // As we running inside node, we already have an isolate and context
  return handleScope.Escape( dali );
}

v8::Local<v8::Object>  DaliWrapper::CreateDaliObject()
{
  v8::EscapableHandleScope handleScope( mIsolate  );

  // Create dali object used for creating objects, and accessing constant values
  // e.g. var x =  new dali.Actor(), or var col = dali.COLOR_RED;

  v8::Local<v8::ObjectTemplate> daliObjectTemplate = NewDaliObjectTemplate( mIsolate );

  // add dali.staqe
  v8::Local<v8::Object> stageObject = StageWrapper::WrapStage( mIsolate, Stage::GetCurrent() );
  daliObjectTemplate->Set( v8::String::NewFromUtf8( mIsolate, "stage") , stageObject );

  v8::Local<v8::Object> keyboardObject = KeyboardFocusManagerWrapper::WrapKeyboardFocusManager( mIsolate,Toolkit::KeyboardFocusManager::Get() );
  daliObjectTemplate->Set( v8::String::NewFromUtf8( mIsolate, "keyboardFocusManager") , keyboardObject );


  //create an instance of the template
  v8::Local<v8::Object> daliObject = daliObjectTemplate->NewInstance();

  ConstantsWrapper::AddDaliConstants( mIsolate, daliObject);

  daliObject->Set( v8::String::NewFromUtf8( mIsolate,  "V8_VERSION") ,v8::String::NewFromUtf8( mIsolate, v8::V8::GetVersion() ));

  return handleScope.Escape( daliObject  );
}


void DaliWrapper::SetFlagsFromString(const std::string &flags)
{
  v8::V8::SetFlagsFromString(flags.c_str(), flags.size());
}

void DaliWrapper::Shutdown()
{
  // if we're running inside node then we don't have ownership of the context
  if( mRunMode == RUNNING_IN_NODE_JS )
  {
    return;
  }

  DALI_LOG_WARNING("Destroying V8 DALi context\n");

  if( !mContext.IsEmpty())
  {
    v8::HandleScope handleScope( mIsolate );
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(mIsolate, mContext);
    context->Exit();   // exit the context
    mContext.Reset();  // destroys the context
  }
}

bool DaliWrapper::ExecuteBuffer(const std::string &sourceCode, const std::string &sourceFileName)
{
  return mModuleLoader.ExecuteScript( mIsolate,  sourceCode, sourceFileName );
}

bool DaliWrapper::ExecuteFile( const std::string& sourceFileName )
{
  DALI_LOG_INFO( gLogExecuteFilter, Debug::Verbose, "Executing source file %s \n",sourceFileName.c_str() );

  return mModuleLoader.ExecuteScriptFromFile( mIsolate,  sourceFileName );
}

GarbageCollectorInterface& DaliWrapper::GetDaliGarbageCollector()
{
  return mGarbageCollector;
}

void DaliWrapper::ApplyGlobalObjectsToContext( v8::Local<v8::Context> context )
{
  v8::HandleScope handleScope( mIsolate );

  // Add global objects ( functions/ values ) e.g. log function
  // create a console.log and console.error functions
  v8::Local<v8::ObjectTemplate> consoleObjectTemplate = v8::ObjectTemplate::New( mIsolate );
  consoleObjectTemplate->Set( v8::String::NewFromUtf8( mIsolate, "log"),   v8::FunctionTemplate::New( mIsolate, V8Utils::Log));
  consoleObjectTemplate->Set( v8::String::NewFromUtf8( mIsolate, "error"), v8::FunctionTemplate::New( mIsolate, V8Utils::LogError));

  context->Global()->Set( v8::String::NewFromUtf8( mIsolate, "console"), consoleObjectTemplate->NewInstance() );

  // add require functionality
  context->Global()->Set( v8::String::NewFromUtf8( mIsolate, "require"), v8::FunctionTemplate::New( mIsolate, DaliWrapper::Require)->GetFunction());

  // Create the Dali object
  // @todo consider forcing developers to perform require('dali') if we want to avoid polluting the global namespace
  v8::Local<v8::Object> daliObject = CreateDaliObject();

  // allow developers to require('dali'); // this is to maintain compatibility with node.js where dali is not part of the global namespace
  mModuleLoader.StorePreBuiltModule( mIsolate, daliObject, DALI_API_NAME );

  context->Global()->Set( v8::String::NewFromUtf8( mIsolate, DALI_API_NAME),daliObject );

}

void DaliWrapper::InitializeStandAlone()
{
  if( !mIsolate )
  {
    v8::V8::InitializeICU();

    v8::V8::Initialize();

    // default isolate removed from V8 version 3.27.1 and beyond.
    mIsolate = v8::Isolate::New();

    mIsolate->Enter();

    v8::V8::SetFatalErrorHandler( FatalErrorCallback );
  }

  // if context is null, create it and add dali object to the global object.
  if( mContext.IsEmpty())
  {
     v8::HandleScope handleScope( mIsolate );

     // create a new context.
     // Isolate = isolated copy of the V8 including a heap manager, a garbage collector
     // Only 1 thread can access a single Isolate at a given time. However, multiple Isolates can be run in parallel.
     // Context = multiple contexts can exist in a given Isolate, and share data between contexts
     v8::Local<v8::Context> context  = v8::Context::New( mIsolate );

     context->Enter();

     // Apply global objects like dali and console to the context
     ApplyGlobalObjectsToContext(context);

     mContext.Reset( mIsolate, context);
  }

  DALI_LOG_INFO( gLogExecuteFilter, Debug::Verbose, "V8 Library %s loaded \n", v8::V8::GetVersion() );
}


v8::Handle<v8::ObjectTemplate> DaliWrapper::NewDaliObjectTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  // create the template
  v8::Local< v8::ObjectTemplate > objTemplate = v8::ObjectTemplate::New( isolate );

  // Add some value properties ( a property can be a primitive value, an object or a function).
  objTemplate->Set( v8::String::NewFromUtf8( isolate, "BUILD"),
                    v8::String::NewFromUtf8( isolate, "Dali binary built on:" __DATE__ ", at " __TIME__));

#ifdef DALI_DATA_READ_ONLY_DIR
  // add the data data directory,
  objTemplate->Set( v8::String::NewFromUtf8( isolate, "DALI_DATA_DIRECTORY"),
                    v8::String::NewFromUtf8( isolate, DALI_DATA_READ_ONLY_DIR));
#endif
  // add our constructor functions
  ObjectTemplateHelper::InstallFunctions( isolate,
                                          objTemplate,
                                          ConstructorFunctionTable,
                                          PropertyFunctionTableCount,
                                          ObjectTemplateHelper::CONSTRUCTOR_FUNCTIONS);

  return handleScope.Escape( objTemplate );
}

void DaliWrapper::Require(const v8::FunctionCallbackInfo< v8::Value >& args)
{
  DaliWrapper& wrapper( DaliWrapper::Get() );
  wrapper.mModuleLoader.Require( args );
}


} // namespace V8Plugin

} // namespace Dali
