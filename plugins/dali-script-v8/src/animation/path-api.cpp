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
#include "path-api.h"

// INTERNAL INCLUDES
#include <animation/path-wrapper.h>
#include <object/property-value-wrapper.h>
#include <v8-utils.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{

Path GetPath( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  PathWrapper* wrapper = static_cast<PathWrapper *>( ptr );
  return wrapper->GetPath();
}

} // un-named namespace


/**
 * Automatic generation of control points. Generated control points which result in a smooth join between the splines of each segment.
 *
 * The generating algorithm is as follows:
 * For a given knot point K[N], find the vector that bisects K[N-1],[N] and [N],[N+1].
 * Calculate the tangent vector by taking the normal of this bisector.
 * The in control point is the length of the preceding segment back along this bisector multiplied by the curvature
 * The out control point is the length of the succeeding segment forward along this bisector multiplied by the curvature
 *
 * @method generateControlPoints
 * @for Path
 * @param {float} curvature curvature The curvature of the spline. 0 gives straight lines between the knots,
 * negative values means the spline contains loops, positive values up to
 * 0.5 result in a smooth curve, positive values between 0.5 and 1 result
 * in looped curves where the loops are not distinct (i.e. the curve appears
 * to be non-continuous), positive values higher than 1 result in looped curves.
 */
void PathApi::GenerateControlPoints( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  float curvature = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 1.0f /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }

  Path path = GetPath( isolate, args );
  path.GenerateControlPoints( curvature );
}


/**
 * Add a interpolation point.
 * @method addPoint
 * @for Path
 * @param {Vector3} interpolationPoint The new interpolation point to be added
 * @example
 *        path.addPoint([ 10,40,0] );
 */
void PathApi::AddPoint( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Vector3 value = V8Utils::GetVector3Parameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }

  Path path = GetPath( isolate, args );
  path.AddPoint( value );
}


/**
 * Add a control point.
 * @method addControlPoint
 * @for Path
 * @param {Vector3} controlPoint The new control point to be added
 * @example
 *        path.addControlPoint([ 10,40,0] );
 */
void PathApi::AddControlPoint( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Vector3 value = V8Utils::GetVector3Parameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }

  Path path = GetPath( isolate, args );
  path.AddControlPoint( value );
}

/**
 * Sample path at a given progress. Calculates position and tangent at that point of the curve
 * @method sample
 * @for Path
 * @param {float} Progress  A floating point value between 0.0 and 1.0.
 * @return {Object} { position: Vector3, tangent:Vector3 }
 */
void PathApi::Sample( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  float progress = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 0.0f /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }

  //Sample the path
  Path path = GetPath( isolate, args );
  Vector3 position, tangent;
  path.Sample( progress, position, tangent );

  //Create v8 object with the result
  v8::Local<v8::Object> sampleObject = v8::Object::New( isolate );

  Dali::Property::Value valuePosition( position );
  sampleObject->Set( v8::String::NewFromUtf8( isolate, "position"), PropertyValueWrapper::WrapDaliProperty( isolate, valuePosition));

  Dali::Property::Value valueTangent( tangent );
  sampleObject->Set( v8::String::NewFromUtf8( isolate, "tangent"), PropertyValueWrapper::WrapDaliProperty( isolate, valueTangent));

  args.GetReturnValue().Set( sampleObject );
}

} // namespace V8Plugin

} // namespace Dali
