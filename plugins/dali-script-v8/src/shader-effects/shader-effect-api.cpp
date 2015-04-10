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
#include "shader-effect-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <shader-effects/shader-effect-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{

typedef  std::vector< std::string > HintsArray;


struct GeometryTypePair
{
  const char* name;
  GeometryType type;
};

const GeometryTypePair GeometryTypeTable[]=
{
    {"image",         GEOMETRY_TYPE_IMAGE },
    {"mesh",          GEOMETRY_TYPE_UNTEXTURED_MESH  },
    {"textured-mesh", GEOMETRY_TYPE_TEXTURED_MESH },
};

const unsigned int GeometryTypeTableCount = sizeof(GeometryTypeTable)/sizeof(GeometryTypeTable[0]);

struct GeometryHintPair
{
  const char* name;
  ShaderEffect::GeometryHints hint;
};

const GeometryHintPair GeometryHintTable[]=
{
    {"gridX",        ShaderEffect::HINT_GRID_X },
    {"gridY",        ShaderEffect::HINT_GRID_Y },
    {"grid",         ShaderEffect::HINT_GRID   },
    {"depthBuffer",  ShaderEffect::HINT_DEPTH_BUFFER },
    {"blending",     ShaderEffect::HINT_BLENDING },
    {"doesntModifyGeometry", ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY }
};

const unsigned int GeometryHintTableCount = sizeof(GeometryHintTable)/sizeof(GeometryHintTable[0]);




struct ShaderParameters
{
  ShaderParameters()
      : mType( GEOMETRY_TYPE_IMAGE),
        mHints( ShaderEffect::HINT_NONE )
  {
  }

  void SetGeometryType( v8::Isolate* isolate, const std::string& typeName )
  {
     for( unsigned int i = 0; i < GeometryTypeTableCount; ++i )
     {
       if( typeName == GeometryTypeTable[i].name )
       {
         mType = GeometryTypeTable[i].type;
         return;
       }
     }
     DALI_SCRIPT_EXCEPTION( isolate, "Geometry type not found\n");
  }

  ShaderEffect::GeometryHints GetGeometryHint( const std::string& hint )
  {
    for( unsigned int i = 0 ; i < GeometryHintTableCount; ++i )
    {
      if( hint == GeometryHintTable[i].name )
      {
        return GeometryHintTable[i].hint;
      }
    }
    return ShaderEffect::HINT_NONE;
  }

  void ProcessHintsArray( const HintsArray& hintsArray )
  {
    for( HintsArray::const_iterator iter  = hintsArray.begin(); iter != hintsArray.end() ; ++iter )
    {
      mHints= static_cast<ShaderEffect::GeometryHints>( mHints | GetGeometryHint( *iter ) );
    }
  }



  ShaderEffect NewShader()
  {
    return ShaderEffect::NewWithPrefix( mVertexPrefix ,
                              mVertex,
                              mFragmentPrefix,
                              mFragment,
                              mType,
                              mHints);
  }
  std::string mVertexPrefix;
  std::string mVertex;
  std::string mFragmentPrefix;
  std::string mFragment;
  GeometryType mType;
  ShaderEffect::GeometryHints mHints;
};

ShaderEffect GetShaderEffect( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  ShaderEffectWrapper* wrapper = static_cast< ShaderEffectWrapper *>(ptr);
  return wrapper->GetShaderEffect();
}

} // unnamed space

/**
 * Create a new ShaderEffect
 * @constructor
 * @for ShaderEffect
 * @method ShaderEffect
 * @param {Object} shaderOptions
 * @param {String} [shaderOptions.geometryType] Type of geometry to be rendered with the effect. "image", "text", "mesh", "textured-mesh", default is image.
 * @param {String} [shaderOptions.vertexShaderPrefix] This string will be inserted before the default uniforms for the vertex shader(ideal for #defines)
 * @param {String} [shaderOptions.vertexShader] VertexShader code for the effect. If not defined, the default version will be used
 * @param {String} [shaderOptions.fragmentShaderPrefix] This string will be inserted before the default uniforms for the fragment shader(ideal for #defines)
 * @param {String} [shaderOptions.fragmentShader] FragmentShader code for the effect. If not defined, the default version will be used
 * @param {Array}  [shaderOptions.geometryHints] Hints for rendering the geometry, e.g.  [ "gridX", "gridY", "grid","depthBuffer","blending","doesntModifyGeometry" ]
 * @return {Object} ShaderEffect
 * @example

      // this will match the default shaders for image
      var vertexShader = " void main() \
      { \
        gl_Position = uProjection * uModelView * vec4(aPosition, 1.0); \
        vTexCoord = aTexCoord; \
      }; "
     var fragShader = " void main() \
      { \
       gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\
      }; "
      var shaderOptions = {
            geometryType: "image",
            vertexShader: vertexShader,
            fragmentShader: fragShader
      };

      var shader = new dali.ShaderEffect(shaderOptions);

      imageActor.setShaderEffect( shader );

 *
 *
 *
 */
ShaderEffect ShaderEffectApi::New(  v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  if( args[0]->IsObject() )
  {
    ShaderParameters shaderParams;

    v8::Local<v8::Object > obj = args[0]->ToObject();

    v8::Local<v8::Value> geometryTypeValue = obj->Get(v8::String::NewFromUtf8( isolate, "geometryType"));
    if( geometryTypeValue->IsString() )
    {
      std::string geometryTypeName = V8Utils::v8StringToStdString( geometryTypeValue );
     // printf(" geometry type found %s \n", geometryTypeName.c_str() );
      shaderParams.SetGeometryType( isolate, geometryTypeName );
    }

    v8::Local<v8::Value> vertexPrefixValue = obj->Get(v8::String::NewFromUtf8( isolate, "vertexShaderPrefix"));
    if( vertexPrefixValue->IsString() )
    {
      shaderParams.mVertexPrefix = V8Utils::v8StringToStdString( vertexPrefixValue );
    }

    v8::Local<v8::Value> fragmentPrefixValue = obj->Get(v8::String::NewFromUtf8( isolate, "fragmentShaderPrefix"));
    if( fragmentPrefixValue->IsString() )
    {
      shaderParams.mFragmentPrefix = V8Utils::v8StringToStdString( fragmentPrefixValue );
    }

    v8::Local<v8::Value> vertexValue = obj->Get(v8::String::NewFromUtf8( isolate, "vertexShader"));
    if( vertexValue->IsString() )
    {
      shaderParams.mVertex = V8Utils::v8StringToStdString( vertexValue );
    }

    v8::Local<v8::Value> fragmentValue = obj->Get(v8::String::NewFromUtf8( isolate, "fragmentShader"));
    if( fragmentValue->IsString() )
    {
      shaderParams.mFragment = V8Utils::v8StringToStdString( fragmentValue );
    }
    // Get any hints
    v8::Local<v8::Value> hints = obj->Get(v8::String::NewFromUtf8( isolate, "geometryHints"));
    if( hints->IsArray() )
    {
      HintsArray hintsArray;
      v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast( hints );
      for( uint32_t i=0; i <  array->Length(); ++i)
      {
        v8::Handle<v8::Value> entry = array->Get(  v8::Integer::New( isolate, i) );
        if( entry->IsString() )
        {
          std::string entryString = V8Utils::v8StringToStdString( entry );
          hintsArray.push_back( entryString );
        }
      }
      shaderParams.ProcessHintsArray( hintsArray );
    }

    return shaderParams.NewShader();
  }
  else
  {
    ShaderEffect effect;

    bool found( false );
    std::string typeName = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "string parameter missing" );
    }
    else
    {
      // create a new shader effect based on type, using the type registry.
      Dali::TypeInfo typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( typeName );
      if( typeInfo ) // handle, check if it has a value
      {
        Dali::BaseHandle handle = typeInfo.CreateInstance();
        if( handle )
        {
          effect = ShaderEffect::DownCast( handle );
        }
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate,"Unknown shader effect type");
      }
    }

    return effect;
  }
}

ShaderEffect ShaderEffectApi::GetShaderEffectFromParams( int paramIndex,
                          bool& found,
                          v8::Isolate* isolate,
                          const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::SHADER_EFFECT, isolate, args );
  if( wrappedObject )
  {
    found = true;
    ShaderEffectWrapper* wrapper = static_cast< ShaderEffectWrapper *>(wrappedObject);
    return wrapper->GetShaderEffect();
  }
  else
  {
    return ShaderEffect();
  }
}

/**
 * Set a shader effect image.
 * This image texture will be bound to the "sEffect" sampler
 * so it can be used in fragment shader for effects
 * @method setEffectImage
 * @for ShaderEffect
 * @param {Object} image
 * @example
 *
 *      shader.setEffectImage( image );
 *
 *      // example of a fragment shader than can use the effect image (sEffect and main texture sTexture)
 *
 *      "void main() \
 *       { \
 *        vec4 v4Color  = (texture2D(sTexture, vTexCoord) * uColor); \
 *        v4Color = v4Color*  texture2D(sEffect, vTexCoord);\
 *        gl_FragColor = v4Color; \"
 *      }";
 */
void ShaderEffectApi::SetEffectImage( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, " Image::SetEffectImage invalid params");
    return;
  }
  ShaderEffect effect =  GetShaderEffect( isolate, args );
  effect.SetEffectImage( image );
}


/**
 * Sets and registers a uniform property.
 * If name matches a uniform in the shader source, this value will be uploaded when rendering.
 * This uniform can then be animated / modified and the change will be made to the shader.
 * @method setUniform
 * @for ShaderEffect
 * @param {String} name
 * @param {Object} value must be a: float, vector2 vector3, vector4, matrix
 * @param {String} [uniformCoordinateType] The coordinate type of the uniform.
 * either "viewPortPosition" or "viewPortDirection"
 * @example

      var fragShader =
      " uniform lowp vec4 uColorShift; \
      \
       void main()             \
       {                  \

         gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor + uColorShift; \
       }"

      var shaderOptions = {
          geometryType: "image",
          fragmentShader: fragShader
      };

      // create a new shader effect
      var shader = new dali.ShaderEffect(shaderOptions);

      // add the color shift uniform so we can animate it
      // default the color shift to zero, so it has no effect
      shader.setUniform("uColorShift", [0.0, 0.0, 0.0, 0]);
 */
void ShaderEffectApi::SetUniform( const v8::FunctionCallbackInfo< v8::Value >& args )
{

  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ShaderEffect effect =  GetShaderEffect( isolate, args );

  bool found(false);
  std::string uniformName = V8Utils::GetStringParameter( PARAMETER_0 , found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "SetUniform: uniform name not found\n");
    return;
  }

  Property::Value propValue = V8Utils::GetPropertyValueParameter( PARAMETER_1 , found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "SetUniform: value not found\n");
    return;
  }

  std::string coordinateType = V8Utils::GetStringParameter( PARAMETER_2, found, isolate, args );
  ShaderEffect::UniformCoordinateType uniformCoordinateType( ShaderEffect::COORDINATE_TYPE_DEFAULT );
  if( found )
  {
    if( coordinateType == "viewPortPosition")
    {
      uniformCoordinateType = ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION;
    }
    else if ( coordinateType == "viewPortDirection ")
    {
      uniformCoordinateType = ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION;
    }
  }

  switch( propValue.GetType() )
  {
    case Property::FLOAT:
    {
      effect.SetUniform( uniformName, propValue.Get<float>(), uniformCoordinateType );
      break;
    }
    case Property::VECTOR2:
    {
      effect.SetUniform( uniformName, propValue.Get<Vector2>(), uniformCoordinateType );
      break;
    }
    case Property::VECTOR3:
    {
      effect.SetUniform( uniformName, propValue.Get<Vector3>(), uniformCoordinateType );
      break;
    }
    case Property::VECTOR4:
    {
      effect.SetUniform( uniformName, propValue.Get<Vector4>(), uniformCoordinateType );
      break;
    }
    case Property::MATRIX:
    {
      effect.SetUniform( uniformName, propValue.Get<Matrix>(), uniformCoordinateType );
      break;
    }
    case Property::MATRIX3:
    {
      effect.SetUniform( uniformName, propValue.Get<Matrix3>(), uniformCoordinateType );
      break;
    }
    default:
    {
      DALI_SCRIPT_EXCEPTION( isolate, "value type not recognised \n");
      break;
    }
  }
}


} // namespace V8Plugin

} // namespace Dali
