 /*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uModelMatrix;\n
  uniform mediump mat4 uViewMatrix;\n
  uniform mediump mat4 uProjection;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 pixelArea;
  varying mediump vec2 vTexCoord;\n
  uniform lowp float uPixelAligned;\n
  \n
  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n
\n
  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
  }\n
\n
  void main()\n
  {\n
    mediump vec4 vertexPosition = uViewMatrix * uModelMatrix * ComputeVertexPosition();\n
    vec4 alignedVertexPosition = vertexPosition;\n
    alignedVertexPosition.xy = floor ( vertexPosition.xy );\n // Pixel alignment
    vertexPosition = uProjection * mix( vertexPosition, alignedVertexPosition, uPixelAligned );\n
    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER_NO_ATLAS = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
      gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * vec4( mixColor, 1.0 );\n
  }\n
);

const char* FRAGMENT_SHADER_ATLAS_CLAMP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    uniform lowp vec4 uColor;\n
    uniform lowp vec3 mixColor;\n
    uniform lowp float preMultipliedAlpha;\n
    \n
    void main()\n
    {\n
        mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );\n
        gl_FragColor = texture2D( sTexture, texCoord ) * uColor * vec4( mixColor, 1.0 );\n
     }\n
);

const char* FRAGMENT_SHADER_ATLAS_VARIOUS_WRAP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    // WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
    uniform lowp vec2 wrapMode;\n
    uniform lowp vec4 uColor;\n
    uniform lowp vec3 mixColor;\n
    uniform lowp float preMultipliedAlpha;\n
    \n
    mediump float wrapCoordinate( mediump vec2 range, mediump float coordinate, lowp float wrap )\n
    {\n
      mediump float coord;\n
      if( wrap > 1.5 )\n // REFLECT
        coord = 1.0-abs(fract(coordinate*0.5)*2.0 - 1.0);\n
      else \n// warp == 0 or 1
        coord = mix(coordinate, fract( coordinate ), wrap);\n
      return clamp( mix(range.x, range.y, coord), range.x, range.y );
    }\n
    \n
    void main()\n
    {\n
        mediump vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                      wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );\n
        gl_FragColor = texture2D( sTexture, texCoord ) * uColor * vec4( mixColor, 1.0 );\n
    }\n
);

} // unnamed namespace

ImageVisualShaderFactory::ImageVisualShaderFactory()
{
}

ImageVisualShaderFactory::~ImageVisualShaderFactory()
{
}

Shader ImageVisualShaderFactory::GetShader( VisualFactoryCache& factoryCache, bool atlasing, bool defaultTextureWrapping )
{
  Shader shader;
  if( atlasing )
  {
    if( defaultTextureWrapping )
    {
      shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP );
      if( !shader )
      {
        shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_CLAMP );
        shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
        factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP, shader );
      }
    }
    else
    {
      shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP );
      if( !shader )
      {
        shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_VARIOUS_WRAP );
        shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
        factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP, shader );
      }
    }
  }
  else
  {
    shader = factoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_NO_ATLAS );
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER, shader );
    }
  }

  return shader;
}

const char* ImageVisualShaderFactory::GetVertexShaderSource()
{
  return VERTEX_SHADER;
}

const char* ImageVisualShaderFactory::GetFragmentShaderSource()
{
  return FRAGMENT_SHADER_NO_ATLAS;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
