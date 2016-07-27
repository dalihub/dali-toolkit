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
#include "shader-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/shader-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * ## Shader API
 *
 * Shaders allows custom vertex and color transformations in the GPU
 *
 * ### Simple example
 *
 *```
 *    // this will match the default shaders for image
 *    var vertexShader = " void main() \
 *    { \
 *      gl_Position = uProjection * uModelView * vec4(aPosition, 1.0); \
 *      vTexCoord = aTexCoord; \
 *    }; "
 *   var fragShader = " void main() \
 *    { \
 *     gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\
 *    }; "
 *    var shaderOptions = {
 *          vertexShader: vertexShader,
 *          fragmentShader: fragShader
 *    };
 *
 *    var shader = new dali.Shader(shaderOptions);
 *```
 * @class Shader
 * @extends Handle
 */

namespace // unnamed namespace
{

typedef  std::vector< std::string > HintsArray;

struct ShaderHintPair
{
  const char* name;
  Shader::Hint::Value hint;
};

const ShaderHintPair ShaderHintTable[]=
{
    {"outputIsTransparent",    Shader::Hint::OUTPUT_IS_TRANSPARENT },
    {"modifiesGeometry",       Shader::Hint::MODIFIES_GEOMETRY },
};

const unsigned int ShaderHintTableCount = sizeof(ShaderHintTable)/sizeof(ShaderHintTable[0]);

struct ShaderParameters
{
  ShaderParameters()
      : mHints( Shader::Hint::NONE )
  {
  }

  Shader::Hint::Value GetShaderHint( const std::string& hint )
  {
    for( unsigned int i = 0 ; i < ShaderHintTableCount; ++i )
    {
      if( hint == ShaderHintTable[i].name )
      {
        return ShaderHintTable[i].hint;
      }
    }
    return Shader::Hint::NONE;
  }

  void ProcessHintsArray( const HintsArray& hintsArray )
  {
    for( HintsArray::const_iterator iter  = hintsArray.begin(); iter != hintsArray.end() ; ++iter )
    {
      mHints= static_cast<Shader::Hint::Value>( mHints | GetShaderHint( *iter ) );
    }
  }

  Shader NewShader()
  {
    return Shader::New( mVertex,
                         mFragment,
                         mHints);
  }

  std::string mVertex;
  std::string mFragment;
  Shader::Hint::Value mHints;
};

} // unnamed space

/**
 * Create a new Shader
 * @constructor
 * @for Shader
 * @method Shader
 * @param {Object} shaderOptions
 * @param {String} [shaderOptions.vertexShader] VertexShader code for the shader. If not defined, the default version will be used
 * @param {String} [shaderOptions.fragmentShader] FragmentShader code for the shader. If not defined, the default version will be used
 * @param {Array}  [shaderOptions.shaderHints] Hints for rendering/subdividing geometry, e.g.  [ "requiresSelfDepthTest", "outputIsTransparent", "outputIsOpaque","modifiesGeometry" ]
 * @return {Object} Shader
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
            vertexShader: vertexShader,
            fragmentShader: fragShader
      };

      var shader = new dali.Shader(shaderOptions);
 *
 */
Shader ShaderApi::New(  v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  ShaderParameters shaderParams;

  if( args[0]->IsObject() )
  {
    v8::Local<v8::Object > obj = args[0]->ToObject();

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
    v8::Local<v8::Value> hints = obj->Get(v8::String::NewFromUtf8( isolate, "shaderHints"));
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
  }

  return shaderParams.NewShader();
}

Shader ShaderApi::GetShaderFromParams( int paramIndex,
                          bool& found,
                          v8::Isolate* isolate,
                          const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::SHADER, isolate, args );
  if( wrappedObject )
  {
    found = true;
    ShaderWrapper* wrapper = static_cast< ShaderWrapper *>(wrappedObject);
    return wrapper->GetShader();
  }
  else
  {
    return Shader();
  }
}

} // namespace V8Plugin

} // namespace Dali
