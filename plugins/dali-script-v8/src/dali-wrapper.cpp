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
#include <stage/stage-wrapper.h>
#include <image/image-wrapper.h>
#include <animation/linear-constrainer-wrapper.h>
#include <animation/path-constrainer-wrapper.h>
#include <animation/path-wrapper.h>
#include <animation/animation-wrapper.h>
#include <events/pan-gesture-detector-wrapper.h>
#include <shader-effects/shader-effect-wrapper.h>
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
    { "TextActor",          ActorWrapper::NewActor },
    { "ImageActor",         ActorWrapper::NewActor },
    { "MeshActor",          ActorWrapper::NewActor },
    { "CameraActor",        ActorWrapper::NewActor },
    { "Layer",              ActorWrapper::NewActor },
    { "TextView",           ActorWrapper::NewActor },
    { "ResourceImage",      ImageWrapper::NewImage },
    { "BufferImage",        ImageWrapper::NewImage },
    { "NinePatchImage",     ImageWrapper::NewImage },
    { "FrameBufferImage",   ImageWrapper::NewImage },
    { "Animation",          AnimationWrapper::NewAnimation},
    { "ShaderEffect",       ShaderEffectWrapper::NewShaderEffect},
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

DaliWrapper::DaliWrapper()
:mIsolate( NULL )
{
}

DaliWrapper::~DaliWrapper()
{
  mInstanceCreated = false;
}

DaliWrapper& DaliWrapper::Get()
{
  if(!mInstanceCreated)
  {
    mWrapper = new DaliWrapper();
    mInstanceCreated = true;

    if(mWrapper)
    {
      mWrapper->Initialize();
    }
  }

  return *mWrapper;
}

void DaliWrapper::SetFlagsFromString(const std::string &flags)
{
  v8::V8::SetFlagsFromString(flags.c_str(), flags.size());
}

void DaliWrapper::Shutdown()
{
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

void DaliWrapper::CreateContext( )
{
  v8::HandleScope handleScope( mIsolate );

  // Create a  global JavaScript object so we can set built-in global functions, like Log.
  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New( mIsolate );

  // Add global objects ( functions/ values ) e.g. log function and V8_VERSION
  global->Set( v8::String::NewFromUtf8( mIsolate,  "log"),        v8::FunctionTemplate::New( mIsolate, V8Utils::Log) );
  global->Set( v8::String::NewFromUtf8( mIsolate,  "logError"),   v8::FunctionTemplate::New( mIsolate, V8Utils::LogError) );
  global->Set( v8::String::NewFromUtf8( mIsolate,  "require"),    v8::FunctionTemplate::New( mIsolate, DaliWrapper::Require));
  global->Set( v8::String::NewFromUtf8( mIsolate,  "V8_VERSION") ,v8::String::NewFromUtf8( mIsolate, v8::V8::GetVersion() ));

   // add the dali object to it, assume it won't be garbage collected until global is deleted
  global->Set(v8::String::NewFromUtf8( mIsolate, DALI_API_NAME) ,  NewDaliObjectTemplate( mIsolate ));


  // create a new context.
  // Isolate = isolated copy of the V8 including a heap manager, a garbage collector
  // Only 1 thread can access a single Isolate at a given time. However, multiple Isolates can be run in parallel.
  // Context = multiple contexts can exist in a given Isolate, and share data between contexts
  v8::Handle<v8::Context> context  = v8::Context::New( mIsolate, NULL, global);

  mGlobalObjectTemplate.Reset( mIsolate,  global);

  mContext.Reset( mIsolate, context);
}

void DaliWrapper::Initialize()
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
     CreateContext();
     v8::Local<v8::Context> context = v8::Local<v8::Context>::New(mIsolate, mContext);

     context->Enter();

     // Add the dali global object. Used for creating objects, and accessing constant values
     // e.g. var x =  new dali.ImageActor(), or var col = dali.COLOR_RED;

     v8::Local<v8::Object> daliObject = v8::Local<v8::Object>::Cast( context->Global()->Get( v8::String::NewFromUtf8( mIsolate, DALI_API_NAME)));

     v8::Local<v8::Object> stageObject = StageWrapper::WrapStage( mIsolate, Stage::GetCurrent() );
     daliObject->Set( v8::String::NewFromUtf8( mIsolate, "stage") , stageObject );

     // keyboard focus manager is a singleton
     v8::Local<v8::Object> keyboardObject = KeyboardFocusManagerWrapper::WrapKeyboardFocusManager( mIsolate,Toolkit::KeyboardFocusManager::Get() );
     daliObject->Set( v8::String::NewFromUtf8( mIsolate, "keyboardFocusManager") , keyboardObject );

     ConstantsWrapper::AddDaliConstants( mIsolate, daliObject);

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


  // add the data data directory,
  objTemplate->Set( v8::String::NewFromUtf8( isolate, "DALI_DATA_DIRECTORY"),
                      v8::String::NewFromUtf8( isolate, DALI_DATA_READ_ONLY_DIR));

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
  wrapper.mModuleLoader.Require( args, wrapper.mGlobalObjectTemplate );
}




} // namespace V8Plugin

} // namespace Dali
