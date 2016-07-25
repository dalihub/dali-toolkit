/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "mesh-visual.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <fstream>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/visual-string-constants.h>
#include <dali-toolkit/internal/controls/renderers/visual-data-impl.h>

namespace Dali
{

namespace
{
  /**
   * @brief Loads a texture from a file
   * @param[in] imageUrl The url of the file
   * @param[in] generateMipmaps Indicates whether to generate mipmaps for the texture
   * @return A texture if loading succeeds, an empty handle otherwise
   */
  Texture LoadTexture( const char* imageUrl, bool generateMipmaps )
  {
    Texture texture;
    Dali::BitmapLoader loader = Dali::BitmapLoader::New( imageUrl );
    loader.Load();
    PixelData pixelData = loader.GetPixelData();
    if( pixelData )
    {
      texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
      texture.Upload( pixelData );

      if( generateMipmaps )
      {
        texture.GenerateMipmaps();
      }
    }

    return texture;
  }
}// unnamed namespace

namespace Toolkit
{

namespace Internal
{

namespace
{

//Defines ordering of textures for shaders.
//All shaders, if including certain texture types, must include them in the same order.
//Within the texture set for the renderer, textures are ordered in the same manner.
enum TextureIndex
{
  DIFFUSE_INDEX = 0u,
  NORMAL_INDEX = 1u,
  GLOSS_INDEX = 2u
};

//Shader properties
const char * const OBJECT_MATRIX_UNIFORM_NAME( "uObjectMatrix" );
const char * const STAGE_OFFSET_UNIFORM_NAME( "uStageOffset" );

const char * const SHADER_TYPE_TEXTURELESS( "TEXTURELESS" );
const char * const SHADER_TYPE_DIFFUSE_TEXTURE( "DIFFUSE_TEXTURE" );
const char * const SHADER_TYPE_ALL_TEXTURES( "ALL_TEXTURES" );

//Shaders
//If a shader requires certain textures, they must be listed in order,
//as detailed in the TextureIndex enum documentation.

//A basic shader that doesn't use textures at all.
const char* SIMPLE_VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute highp vec3 aPosition;\n
  attribute highp vec3 aNormal;\n
  varying mediump vec3 vIllumination;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat4 uModelView;\n
  uniform mediump mat4 uViewMatrix;\n
  uniform mediump mat3 uNormalMatrix;
  uniform mediump mat4 uObjectMatrix;\n
  uniform mediump vec3 lightPosition;\n
  uniform mediump vec2 uStageOffset;\n

  void main()\n
  {\n
    vec4 normalisedVertexPosition = vec4( aPosition * min( uSize.x, uSize.y ), 1.0 );\n
    vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    //Illumination in Model-View space - Transform attributes and uniforms\n
    vec4 mvVertexPosition = uModelView * normalisedVertexPosition;\n
    vec3 normal = uNormalMatrix * mat3( uObjectMatrix ) * aNormal;\n

    vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );\n
    mvLightPosition = uViewMatrix * mvLightPosition;\n
    vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );\n

    float lightDiffuse = max( dot( vectorToLight, normal ), 0.0 );\n
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );\n

    gl_Position = vertexPosition;\n
  }\n
);

//Fragment shader corresponding to the texture-less shader.
const char* SIMPLE_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;\n
  varying mediump vec3 vIllumination;\n
  uniform lowp vec4 uColor;\n

  void main()\n
  {\n
    gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );\n
  }\n
);

//Diffuse and specular illumination shader with albedo texture. Texture is index 0.
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute highp vec3 aPosition;\n
  attribute highp vec2 aTexCoord;\n
  attribute highp vec3 aNormal;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vIllumination;\n
  varying mediump float vSpecular;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat4 uModelView;
  uniform mediump mat4 uViewMatrix;\n
  uniform mediump mat3 uNormalMatrix;
  uniform mediump mat4 uObjectMatrix;\n
  uniform mediump vec3 lightPosition;\n
  uniform mediump vec2 uStageOffset;\n

  void main()
  {\n
    vec4 normalisedVertexPosition = vec4( aPosition * min( uSize.x, uSize.y ), 1.0 );\n
    vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    //Illumination in Model-View space - Transform attributes and uniforms\n
    vec4 mvVertexPosition = uModelView * normalisedVertexPosition;\n
    vec3 normal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aNormal );\n

    vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );\n
    mvLightPosition = uViewMatrix * mvLightPosition;\n
    vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );\n

    vec3 viewDirection = normalize( -mvVertexPosition.xyz );

    float lightDiffuse = dot( vectorToLight, normal );\n
    lightDiffuse = max( 0.0,lightDiffuse );\n
    vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );\n

    vec3 reflectDirection = reflect( -vectorToLight, normal );
    vSpecular = pow( max( dot( reflectDirection, viewDirection ), 0.0 ), 4.0 );

    vTexCoord = aTexCoord;\n
    gl_Position = vertexPosition;\n
  }\n
);

//Fragment shader corresponding to the diffuse and specular illumination shader with albedo texture
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vIllumination;\n
  varying mediump float vSpecular;\n
  uniform sampler2D sDiffuse;\n
  uniform lowp vec4 uColor;\n

  void main()\n
  {\n
    vec4 texture = texture2D( sDiffuse, vTexCoord );\n
    gl_FragColor = vec4( vIllumination.rgb * texture.rgb * uColor.rgb + vSpecular * 0.3, texture.a * uColor.a );\n
  }\n
);

//Diffuse and specular illumination shader with albedo texture, normal map and gloss map shader.
//Diffuse (albedo) texture is index 0, normal is 1, gloss is 2. They must be declared in this order.
const char* NORMAL_MAP_VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute highp vec3 aPosition;\n
  attribute highp vec2 aTexCoord;\n
  attribute highp vec3 aNormal;\n
  attribute highp vec3 aTangent;\n
  attribute highp vec3 aBiNormal;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vLightDirection;\n
  varying mediump vec3 vHalfVector;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump mat4 uModelView;
  uniform mediump mat4 uViewMatrix;\n
  uniform mediump mat3 uNormalMatrix;
  uniform mediump mat4 uObjectMatrix;\n
  uniform mediump vec3 lightPosition;\n
  uniform mediump vec2 uStageOffset;\n
  void main()
  {\n
    vec4 normalisedVertexPosition = vec4( aPosition * min( uSize.x, uSize.y ), 1.0 );\n
    vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n

    vec4 mvVertexPosition = uModelView * normalisedVertexPosition;\n

    vec3 tangent = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aTangent );
    vec3 binormal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aBiNormal );
    vec3 normal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aNormal );

    vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );\n
    mvLightPosition = uViewMatrix * mvLightPosition;\n
    vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );\n
    vLightDirection.x = dot( vectorToLight, tangent );
    vLightDirection.y = dot( vectorToLight, binormal );
    vLightDirection.z = dot( vectorToLight, normal );

    vec3 viewDirection = normalize( -mvVertexPosition.xyz );
    vec3 halfVector = normalize( viewDirection + vectorToLight );
    vHalfVector.x = dot( halfVector, tangent );
    vHalfVector.y = dot( halfVector, binormal );
    vHalfVector.z = dot( halfVector, normal );

    vTexCoord = aTexCoord;\n
    gl_Position = vertexPosition;\n
  }\n
);

//Fragment shader corresponding to the shader that uses all textures (diffuse, normal and gloss maps)
const char* NORMAL_MAP_FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  precision mediump float;\n
  varying mediump vec2 vTexCoord;\n
  varying mediump vec3 vLightDirection;\n
  varying mediump vec3 vHalfVector;\n
  uniform sampler2D sDiffuse;\n
  uniform sampler2D sNormal;\n
  uniform sampler2D sGloss;\n
  uniform lowp vec4 uColor;\n

  void main()\n
  {\n
    vec4 texture = texture2D( sDiffuse, vTexCoord );\n
    vec3 normal = normalize( texture2D( sNormal, vTexCoord ).xyz * 2.0 - 1.0 );\n
    vec4 glossMap = texture2D( sGloss, vTexCoord );\n

    float lightDiffuse = max( 0.0, dot( normal, normalize( vLightDirection ) ) );\n
    lightDiffuse = lightDiffuse * 0.5 + 0.5;\n

    float shininess = pow ( max ( dot ( normalize( vHalfVector ), normal ), 0.0 ), 16.0 )  ;

    gl_FragColor = vec4( texture.rgb * uColor.rgb * lightDiffuse + shininess * glossMap.rgb, texture.a * uColor.a );\n
  }\n
);

} // namespace

MeshVisual::MeshVisual( VisualFactoryCache& factoryCache )
: Visual( factoryCache ),
  mShaderType( ALL_TEXTURES ),
  mUseTexture( true ),
  mUseMipmapping( true ),
  mUseSoftNormals( true )
{
}

MeshVisual::~MeshVisual()
{
}

void MeshVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  Property::Value* objectUrl = propertyMap.Find( OBJECT_URL );
  if( !objectUrl || !objectUrl->Get( mObjectUrl ) )
  {
    DALI_LOG_ERROR( "Fail to provide object URL to the MeshVisual object.\n" );
  }

  Property::Value* materialUrl = propertyMap.Find( MATERIAL_URL );
  if( !materialUrl || !materialUrl->Get( mMaterialUrl ) || mMaterialUrl.empty() )
  {
    mUseTexture = false;
  }

  Property::Value* imagesUrl = propertyMap.Find( TEXTURES_PATH );
  if( !imagesUrl || !imagesUrl->Get( mTexturesPath ) )
  {
    //Default behaviour is to assume files are in the same directory,
    // or have their locations detailed in full when supplied.
    mTexturesPath.clear();
  }

  Property::Value* shaderType = propertyMap.Find( SHADER_TYPE );
  if( shaderType )
  {
    std::string shaderTypeString;
    if( shaderType->Get( shaderTypeString ) )
    {
      if( shaderTypeString == SHADER_TYPE_TEXTURELESS )
      {
        mShaderType = TEXTURELESS;
      }
      else if( shaderTypeString == SHADER_TYPE_DIFFUSE_TEXTURE )
      {
        mShaderType = DIFFUSE_TEXTURE;
      }
      else if( shaderTypeString == SHADER_TYPE_ALL_TEXTURES )
      {
        mShaderType = ALL_TEXTURES;
      }
      else
      {
        DALI_LOG_ERROR( "Unknown shader type provided to the MeshVisual object.\n");
      }
    }
  }

  Property::Value* useMipmapping = propertyMap.Find( USE_MIPMAPPING );
  if( useMipmapping )
  {
    useMipmapping->Get( mUseMipmapping );
  }

  Property::Value* useSoftNormals = propertyMap.Find( USE_SOFT_NORMALS );
  if( useSoftNormals )
  {
    useSoftNormals->Get( mUseSoftNormals );
  }

  Property::Value* lightPosition = propertyMap.Find( LIGHT_POSITION_UNIFORM_NAME );
  if( lightPosition )
  {
    if( !lightPosition->Get( mLightPosition ) )
    {
      DALI_LOG_ERROR( "Invalid value passed for light position in MeshRenderer object.\n" );
      mLightPosition = Vector3::ZERO;
    }
  }
  else
  {
    //Default behaviour is to place the light directly in front of the object,
    // at a reasonable distance to light everything on screen.
    Stage stage = Stage::GetCurrent();

    mLightPosition = Vector3( stage.GetSize().width / 2, stage.GetSize().height / 2, stage.GetSize().width * 5 );
  }
}

void MeshVisual::SetSize( const Vector2& size )
{
  Visual::SetSize( size );

  // ToDo: renderer responds to the size change
}

void MeshVisual::SetClipRect( const Rect<int>& clipRect )
{
  Visual::SetClipRect( clipRect );

  //ToDo: renderer responds to the clipRect change
}

void MeshVisual::SetOffset( const Vector2& offset )
{
  //ToDo: renderer applies the offset
}

void MeshVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();
}

void MeshVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, MESH_RENDERER );
  map.Insert( OBJECT_URL, mObjectUrl );
  map.Insert( MATERIAL_URL, mMaterialUrl );
  map.Insert( TEXTURES_PATH, mTexturesPath );

  std::string shaderTypeString;
  switch( mShaderType )
  {
    case ALL_TEXTURES:
    {
      shaderTypeString = SHADER_TYPE_ALL_TEXTURES;
      break;
    }

    case DIFFUSE_TEXTURE:
    {
      shaderTypeString = SHADER_TYPE_DIFFUSE_TEXTURE;
      break;
    }

    case TEXTURELESS:
    {
      shaderTypeString = SHADER_TYPE_TEXTURELESS;
      break;
    }
  }
  map.Insert( SHADER_TYPE, shaderTypeString );

  map.Insert( USE_MIPMAPPING, mUseMipmapping );
  map.Insert( USE_SOFT_NORMALS, mUseSoftNormals );
  map.Insert( LIGHT_POSITION_UNIFORM_NAME, mLightPosition );
}

void MeshVisual::InitializeRenderer()
{
  //Try to load the geometry from the file.
  if( !LoadGeometry() )
  {
    SupplyEmptyGeometry();
    return;
  }

  //If a texture is used by the obj file, load the supplied material file.
  if( mObjLoader.IsTexturePresent() && !mMaterialUrl.empty() )
  {
    if( !LoadMaterial() )
    {
      SupplyEmptyGeometry();
      return;
    }
  }

  //Now that the required parts are loaded, create the geometry for the object.
  if( !CreateGeometry() )
  {
    SupplyEmptyGeometry();
    return;
  }

  CreateShader();

  //Load the various texture files supplied by the material file.
  if( !LoadTextures() )
  {
    SupplyEmptyGeometry();
    return;
  }

  mImpl->mRenderer = Renderer::New( mGeometry, mShader );
  mImpl->mRenderer.SetTextures( mTextureSet );
  mImpl->mRenderer.SetProperty( Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON );
}

void MeshVisual::SupplyEmptyGeometry()
{
  mGeometry = Geometry::New();
  mShader = Shader::New( SIMPLE_VERTEX_SHADER, SIMPLE_FRAGMENT_SHADER );
  mImpl->mRenderer = Renderer::New( mGeometry, mShader );

  DALI_LOG_ERROR( "Initialisation error in mesh renderer.\n" );
}

void MeshVisual::UpdateShaderUniforms()
{
  Stage stage = Stage::GetCurrent();
  float width = stage.GetSize().width;
  float height = stage.GetSize().height;

  Matrix scaleMatrix;
  scaleMatrix.SetIdentityAndScale( Vector3( 1.0, -1.0, 1.0 ) );

  mShader.RegisterProperty( STAGE_OFFSET_UNIFORM_NAME, Vector2( width, height ) / 2.0f );
  mShader.RegisterProperty( LIGHT_POSITION_UNIFORM_NAME, mLightPosition );
  mShader.RegisterProperty( OBJECT_MATRIX_UNIFORM_NAME, scaleMatrix );
}

void MeshVisual::CreateShader()
{
  if( mShaderType == ALL_TEXTURES )
  {
    mShader = Shader::New( NORMAL_MAP_VERTEX_SHADER, NORMAL_MAP_FRAGMENT_SHADER );
  }
  else if( mShaderType == DIFFUSE_TEXTURE )
  {
    mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
  }
  else //Textureless
  {
    mShader = Shader::New( SIMPLE_VERTEX_SHADER, SIMPLE_FRAGMENT_SHADER );
  }

  UpdateShaderUniforms();
}

bool MeshVisual::CreateGeometry()
{
  //Determine if we need to use a simpler shader to handle the provided data
  if( !mUseTexture || !mObjLoader.IsDiffuseMapPresent() )
  {
    mShaderType = TEXTURELESS;
  }
  else if( mShaderType == ALL_TEXTURES && (!mObjLoader.IsNormalMapPresent() || !mObjLoader.IsSpecularMapPresent()) )
  {
    mShaderType = DIFFUSE_TEXTURE;
  }

  int objectProperties = 0;

  if( mShaderType == DIFFUSE_TEXTURE ||
      mShaderType == ALL_TEXTURES )
  {
    objectProperties |= ObjLoader::TEXTURE_COORDINATES;
  }

  if( mShaderType == ALL_TEXTURES )
  {
    objectProperties |= ObjLoader::TANGENTS | ObjLoader::BINORMALS;
  }

  //Create geometry with attributes required by shader.
  mGeometry = mObjLoader.CreateGeometry( objectProperties, mUseSoftNormals );

  if( mGeometry )
  {
    return true;
  }

  DALI_LOG_ERROR( "Failed to load geometry in mesh renderer.\n" );
  return false;
}

bool MeshVisual::LoadGeometry()
{
  std::streampos fileSize;
  Dali::Vector<char> fileContent;

  if( FileLoader::ReadFile( mObjectUrl, fileSize, fileContent, FileLoader::TEXT ) )
  {
    mObjLoader.ClearArrays();
    mObjLoader.LoadObject( fileContent.Begin(), fileSize );

    //Get size information from the obj loaded
    mSceneCenter = mObjLoader.GetCenter();
    mSceneSize = mObjLoader.GetSize();

    return true;
  }

  DALI_LOG_ERROR( "Failed to find object to load in mesh renderer.\n" );
  return false;
}

bool MeshVisual::LoadMaterial()
{
  std::streampos fileSize;
  Dali::Vector<char> fileContent;

  if( FileLoader::ReadFile( mMaterialUrl, fileSize, fileContent, FileLoader::TEXT ) )
  {
    //Load data into obj (usable) form
    mObjLoader.LoadMaterial( fileContent.Begin(), fileSize, mDiffuseTextureUrl, mNormalTextureUrl, mGlossTextureUrl );
    return true;
  }

  DALI_LOG_ERROR( "Failed to find texture set to load in mesh renderer.\n" );
  mUseTexture = false;
  return false;
}

bool MeshVisual::LoadTextures()
{
  mTextureSet = TextureSet::New();

  if( mShaderType != TEXTURELESS )
  {
    Sampler sampler = Sampler::New();
    if( mUseMipmapping )
    {
      sampler.SetFilterMode( FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR_MIPMAP_LINEAR );
    }

    if( !mDiffuseTextureUrl.empty() )
    {
      std::string imageUrl = mTexturesPath + mDiffuseTextureUrl;

      //Load textures
      Texture diffuseTexture = LoadTexture( imageUrl.c_str(), mUseMipmapping );
      if( diffuseTexture )
      {
        mTextureSet.SetTexture( DIFFUSE_INDEX, diffuseTexture );
        mTextureSet.SetSampler( DIFFUSE_INDEX, sampler );
      }
      else
      {
        DALI_LOG_ERROR( "Failed to load diffuse map texture in mesh renderer.\n");
        return false;
      }
    }

    if( !mNormalTextureUrl.empty() && ( mShaderType == ALL_TEXTURES ) )
    {
      std::string imageUrl = mTexturesPath + mNormalTextureUrl;

      //Load textures
      Texture normalTexture = LoadTexture( imageUrl.c_str(), mUseMipmapping );
      if( normalTexture )
      {
        mTextureSet.SetTexture( NORMAL_INDEX, normalTexture );
        mTextureSet.SetSampler( NORMAL_INDEX, sampler );
      }
      else
      {
        DALI_LOG_ERROR( "Failed to load normal map texture in mesh renderer.\n");
        return false;
      }
    }

    if( !mGlossTextureUrl.empty() && ( mShaderType == ALL_TEXTURES ) )
    {
      std::string imageUrl = mTexturesPath + mGlossTextureUrl;

      //Load textures
      Texture glossTexture = LoadTexture( imageUrl.c_str(), mUseMipmapping );
      if( glossTexture )
      {
        mTextureSet.SetTexture( GLOSS_INDEX, glossTexture );
        mTextureSet.SetSampler( GLOSS_INDEX, sampler );
      }
      else
      {
        DALI_LOG_ERROR( "Failed to load gloss map texture in mesh renderer.\n");
        return false;
      }
    }
  }
  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
