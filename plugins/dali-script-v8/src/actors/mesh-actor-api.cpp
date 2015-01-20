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
#include "mesh-actor-api.h"

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>



namespace Dali
{

namespace V8Plugin
{

namespace //unnamed name space
{

MeshActor GetMeshActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return MeshActor::DownCast( handleWrapper->mHandle );
}

} //unnamed name space

/***************************************
 * MESH ACTOR FUNCTIONS
 ****************************************/
/**
 * @constructor
 * @for MeshActor
 * @method MeshActor
 * @return {Object} MeshActor
 */
Actor MeshActorApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return MeshActor::New();
}
/**
 *
 * @for MeshActor
 * @method SetAffectedByLighting
 * @param {Boolean} true = yes affected by lighting
 */
void MeshActorApi::SetAffectedByLighting( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  MeshActor actor =  GetMeshActor( isolate, args );

  bool parameterFound( false );
  bool afftedByLight = V8Utils::GetBooleanParameter(  PARAMETER_0, parameterFound, isolate, args );
  if( !parameterFound )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter");
    return;
  }
  actor.SetAffectedByLighting( afftedByLight  );
}
/**
 *
 * @for MeshActor
 * @method IsAffectedByLighting
 * @return {Boolean} true = yes affected by lighting
 */
void MeshActorApi::IsAffectedByLighting( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  MeshActor actor =  GetMeshActor( isolate, args );

  args.GetReturnValue().Set(  v8::Boolean::New( isolate, actor.IsAffectedByLighting() ) );

}


} // namespace V8Plugin

} // namespace Dali
