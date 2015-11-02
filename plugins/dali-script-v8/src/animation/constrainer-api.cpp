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
#include "constrainer-api.h"

//EXTERNAL INCLUDES
#include <cfloat> //For FLT_MAX

// INTERNAL INCLUDES
#include <animation/linear-constrainer-wrapper.h>
#include <animation/path-constrainer-wrapper.h>
#include <object/property-value-wrapper.h>
#include <v8-utils.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{

PathConstrainer GetPathConstrainer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  PathConstrainerWrapper* wrapper = static_cast<PathConstrainerWrapper *>( ptr );
  return wrapper->GetPathConstrainer();
}

LinearConstrainer GetLinearConstrainer( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  LinearConstrainerWrapper* wrapper = static_cast<LinearConstrainerWrapper *>( ptr );
  return wrapper->GetLinearConstrainer();
}

bool GetApplyParameters( const v8::FunctionCallbackInfo< v8::Value >& args, Actor& targetActor, Property::Index& targetPropertyIndex,
                         Actor& sourceActor, Property::Index& sourcePropertyIndex, Vector2& range, Vector2& wrap )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( args[0]->IsObject() )
  {
    bool found(false);

    v8::Local<v8::Object> obj = args[0]->ToObject();
    v8::Local<v8::Value> member = obj->Get( v8::String::NewFromUtf8( isolate, "target" ) );

    //Get target actor
    if( member->IsObject() )
    {
      v8::Local<v8::Object> targetActorObject = member->ToObject();
      targetActor = V8Utils::GetActorFromObject( isolate, found, targetActorObject );
      if( !targetActor )
      {
        DALI_SCRIPT_EXCEPTION( isolate, "Target actor not found" );
        return false;
      }
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Target actor not specified" );
      return false;
    }

    //Get source actor
    member = obj->Get( v8::String::NewFromUtf8( isolate, "source" ) );
    if( member->IsObject() )
    {
      v8::Local<v8::Object> sourceActorObject = member->ToObject();
      sourceActor = V8Utils::GetActorFromObject( isolate, found, sourceActorObject );
      if( !sourceActor )
      {
        DALI_SCRIPT_EXCEPTION( isolate, "Source actor not found" );
        return false;
      }
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Source actor not specified" );
      return false;
    }

    //Get target property
    member = obj->Get( v8::String::NewFromUtf8( isolate, "targetProperty" ) );
    if( member->IsString() )
    {
      std::string propertyName = V8Utils::v8StringToStdString( member );
      targetPropertyIndex = targetActor.GetPropertyIndex( propertyName );
      if( targetPropertyIndex == Property::INVALID_INDEX )
      {
        targetPropertyIndex = targetActor.GetPropertyIndex( propertyName );

        if( targetPropertyIndex == Property::INVALID_INDEX )
        {
          DALI_SCRIPT_EXCEPTION( isolate, "Target property not found" );
          return false;
        }
      }
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Target property not specified" );
      return false;
    }

    //Get source property
    member = obj->Get( v8::String::NewFromUtf8( isolate, "sourceProperty" ) );
    if( member->IsString() )
    {
      std::string propertyName = V8Utils::v8StringToStdString( member );
      sourcePropertyIndex = targetActor.GetPropertyIndex( propertyName );
      if( sourcePropertyIndex == Property::INVALID_INDEX )
      {
        sourcePropertyIndex = targetActor.GetPropertyIndex( propertyName );

        if( sourcePropertyIndex == Property::INVALID_INDEX )
        {
          DALI_SCRIPT_EXCEPTION( isolate, "Source property not found" );
          return false;
        }
      }
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Source property not specified" );
      return false;
    }

    //Get range
    member = obj->Get( v8::String::NewFromUtf8( isolate, "range" ) );
    if( member->IsObject() )
    {
      v8::Local<v8::Object> rangeObject = member->ToObject();
      Property::Value value = V8Utils::GetPropertyValueFromObject( found, isolate, rangeObject );
      value.Get( range );
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Range not specified" );
      return false;
    }

    //Get wrap range
    member = obj->Get( v8::String::NewFromUtf8( isolate, "wrap" ) );
    if( member->IsObject() )
    {
      v8::Local<v8::Object> wrapObject = member->ToObject();
      Property::Value value = V8Utils::GetPropertyValueFromObject( found, isolate, wrapObject );
      value.Get( wrap );
    }
    else
    {
      wrap =  Vector2(-FLT_MAX, FLT_MAX);
    }

    return true;
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Bad parameter (Object)" );
    return false;
  }
}

} // un-named namespace

/**
 * Apply the constraint
 * @method applyConstraint
 * @for PathConstrainer and LinearConstrainer
 * @param {Object}  Constraint
 * @param {Object}  Constraint.targetActor
 * @param {String}  Constraint.targetProperty
 * @param {String}  Constraint.sourceProperty
 * @param {Vector2} Constraint.range
 * @param {Vector2} Constraint.wrap
 *
 * @example
 *
 *        var constraintPosition = {  "target":targetActor,
 *                                    "targetProperty":"position",
 *                                    "source":sourceActor,
 *                                    "sourceProperty":"colorAlpha",
 *                                    "range":range
 *                                    "wrap":wrap
 *                                 };
 *        pathConstrainer.applyConstraint( constraintPosition );
 */
void ConstrainerApi::Apply( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Actor target, source;
  Property::Index targetPropertyIndex = Property::INVALID_INDEX;
  Property::Index sourcePropertyIndex = Property::INVALID_INDEX;

  Vector2 range, wrap;
  if( GetApplyParameters(args, target, targetPropertyIndex, source,  sourcePropertyIndex, range, wrap ) )
  {

    PathConstrainer pathConstrainer = GetPathConstrainer( isolate, args );
    if( pathConstrainer )
    {
      pathConstrainer.Apply( Property(target, targetPropertyIndex),
                             Property(source, sourcePropertyIndex),
                             range, wrap );
    }
    else
    {
      LinearConstrainer linearConstrainer = GetLinearConstrainer( isolate, args );
      if( linearConstrainer )
      {
        linearConstrainer.Apply( Property(target, targetPropertyIndex),
                                 Property(source, sourcePropertyIndex),
                                 range, wrap );
      }
    }
  }
}

/**
 * Remove the constraint
 * @method remove
 * @for PathConstrainer
 * @param {Object} Actor
 * @example
 *        pathConstrainer.remove( targetActor );
 */
void ConstrainerApi::Remove( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  //Get target actor
  bool found( false );
  Actor targetActor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 (Actor)" );
    return;
  }


  PathConstrainer pathConstrainer = GetPathConstrainer( isolate, args );
  if( pathConstrainer )
  {
    pathConstrainer.Remove(targetActor);
  }
  else
  {
    LinearConstrainer linearConstrainer = GetLinearConstrainer( isolate, args );
    if( linearConstrainer )
    {
      linearConstrainer.Remove(targetActor);
    }
  }
}

} // namespace V8Plugin

} // namespace Dali
