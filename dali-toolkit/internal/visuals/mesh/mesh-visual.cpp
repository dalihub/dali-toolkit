/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

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
Texture LoadTexture(const char* imageUrl, bool generateMipmaps)
{
  Texture texture;

  Devel::PixelBuffer pixelBuffer = LoadImageFromFile(imageUrl);
  if(pixelBuffer)
  {
    texture             = Texture::New(TextureType::TEXTURE_2D, pixelBuffer.GetPixelFormat(), pixelBuffer.GetWidth(), pixelBuffer.GetHeight());
    PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer);
    texture.Upload(pixelData);

    if(generateMipmaps)
    {
      texture.GenerateMipmaps();
    }
  }

  return texture;
}
} // unnamed namespace

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
  NORMAL_INDEX  = 1u,
  GLOSS_INDEX   = 2u
};

//Shading mode
DALI_ENUM_TO_STRING_TABLE_BEGIN(SHADING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::MeshVisual::ShadingMode, TEXTURELESS_WITH_DIFFUSE_LIGHTING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::MeshVisual::ShadingMode, TEXTURED_WITH_SPECULAR_LIGHTING)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::MeshVisual::ShadingMode, TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING)
DALI_ENUM_TO_STRING_TABLE_END(SHADING_MODE)

//Shader properties
const char* const OBJECT_MATRIX_UNIFORM_NAME("uObjectMatrix");
const char* const STAGE_OFFSET_UNIFORM_NAME("uStageOffset");

} // unnamed namespace

MeshVisualPtr MeshVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  MeshVisualPtr meshVisualPtr(new MeshVisual(factoryCache));
  meshVisualPtr->SetProperties(properties);
  meshVisualPtr->Initialize();
  return meshVisualPtr;
}

MeshVisual::MeshVisual(VisualFactoryCache& factoryCache)
: Visual::Base(factoryCache, Visual::FittingMode::FIT_KEEP_ASPECT_RATIO, Toolkit::Visual::MESH),
  mShadingMode(Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING),
  mUseTexture(true),
  mUseMipmapping(true),
  mUseSoftNormals(true)
{
}

MeshVisual::~MeshVisual()
{
}

void MeshVisual::DoSetProperties(const Property::Map& propertyMap)
{
  for(Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter)
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue(iter);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      if(keyValue.first == OBJECT_URL_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::OBJECT_URL, keyValue.second);
      }
      else if(keyValue.first == MATERIAL_URL_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::MATERIAL_URL, keyValue.second);
      }
      else if(keyValue.first == TEXTURES_PATH_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::TEXTURES_PATH, keyValue.second);
      }
      else if(keyValue.first == SHADING_MODE_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::SHADING_MODE, keyValue.second);
      }
      else if(keyValue.first == USE_MIPMAPPING_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::USE_MIPMAPPING, keyValue.second);
      }
      else if(keyValue.first == USE_SOFT_NORMALS_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::USE_SOFT_NORMALS, keyValue.second);
      }
      else if(keyValue.first == LIGHT_POSITION_NAME)
      {
        DoSetProperty(Toolkit::MeshVisual::Property::LIGHT_POSITION, keyValue.second);
      }
    }
  }

  if(mMaterialUrl.empty())
  {
    mUseTexture = false;
  }

  if(mLightPosition == Vector3::ZERO)
  {
    // Default behaviour is to place the light directly in front of the object,
    // at a reasonable distance to light everything on screen.
    Stage stage = Stage::GetCurrent();

    mLightPosition = Vector3(stage.GetSize().width / 2, stage.GetSize().height / 2, stage.GetSize().width * 5);
  }
}

void MeshVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Toolkit::MeshVisual::Property::OBJECT_URL:
    {
      if(!value.Get(mObjectUrl))
      {
        DALI_LOG_ERROR("MeshVisual: property objectUrl is the wrong type, use STRING\n");
      }
      break;
    }
    case Toolkit::MeshVisual::Property::MATERIAL_URL:
    {
      if(!value.Get(mMaterialUrl))
      {
        DALI_LOG_ERROR("MeshVisual: property materialUrl is the wrong type, use STRING\n");
      }
      break;
    }
    case Toolkit::MeshVisual::Property::TEXTURES_PATH:
    {
      if(!value.Get(mTexturesPath))
      {
        mTexturesPath.clear();
      }
      break;
    }
    case Toolkit::MeshVisual::Property::SHADING_MODE:
    {
      Scripting::GetEnumerationProperty(value, SHADING_MODE_TABLE, SHADING_MODE_TABLE_COUNT, mShadingMode);
      break;
    }
    case Toolkit::MeshVisual::Property::USE_MIPMAPPING:
    {
      if(!value.Get(mUseMipmapping))
      {
        DALI_LOG_ERROR("MeshVisual: property useMipmapping is the wrong type, use BOOLEAN\n");
      }
      break;
    }
    case Toolkit::MeshVisual::Property::USE_SOFT_NORMALS:
    {
      if(!value.Get(mUseSoftNormals))
      {
        DALI_LOG_ERROR("MeshVisual: property useSoftNormals is the wrong type, use BOOLEAN\n");
      }
      break;
    }
    case Toolkit::MeshVisual::Property::LIGHT_POSITION:
    {
      if(!value.Get(mLightPosition))
      {
        mLightPosition = Vector3::ZERO;
        DALI_LOG_ERROR("MeshVisual: property lightPosition is the wrong type, use VECTOR3\n");
      }
      break;
    }
  }
}

void MeshVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void MeshVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);

  // Mesh loaded and ready to display
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void MeshVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::MESH);
  map.Insert(Toolkit::MeshVisual::Property::OBJECT_URL, mObjectUrl);
  map.Insert(Toolkit::MeshVisual::Property::MATERIAL_URL, mMaterialUrl);
  map.Insert(Toolkit::MeshVisual::Property::TEXTURES_PATH, mTexturesPath);
  map.Insert(Toolkit::MeshVisual::Property::SHADING_MODE, mShadingMode);
  map.Insert(Toolkit::MeshVisual::Property::USE_MIPMAPPING, mUseMipmapping);
  map.Insert(Toolkit::MeshVisual::Property::USE_SOFT_NORMALS, mUseSoftNormals);
  map.Insert(Toolkit::MeshVisual::Property::LIGHT_POSITION, mLightPosition);
}

void MeshVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void MeshVisual::OnInitialize()
{
  //Try to load the geometry from the file.
  if(!LoadGeometry())
  {
    SupplyEmptyGeometry();
    return;
  }

  //If a texture is used by the obj file, load the supplied material file.
  if(mObjLoader.IsTexturePresent() && !mMaterialUrl.empty())
  {
    if(!LoadMaterial())
    {
      SupplyEmptyGeometry();
      return;
    }
  }

  //Now that the required parts are loaded, create the geometry for the object.
  if(!CreateGeometry())
  {
    SupplyEmptyGeometry();
    return;
  }

  CreateShader();

  //Load the various texture files supplied by the material file.
  if(!LoadTextures())
  {
    SupplyEmptyGeometry();
    return;
  }

  mImpl->mRenderer = Renderer::New(mGeometry, mShader);
  mImpl->mRenderer.SetTextures(mTextureSet);
  mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::ON);
  mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON);

  //Register transform properties
  mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

void MeshVisual::SupplyEmptyGeometry()
{
  mGeometry        = Geometry::New();
  mShader          = Shader::New(SHADER_MESH_VISUAL_SIMPLE_SHADER_VERT, SHADER_MESH_VISUAL_SIMPLE_SHADER_FRAG);
  mImpl->mRenderer = Renderer::New(mGeometry, mShader);

  DALI_LOG_ERROR("Initialisation error in mesh visual.\n");
}

void MeshVisual::UpdateShaderUniforms()
{
  Stage stage  = Stage::GetCurrent();
  float width  = stage.GetSize().width;
  float height = stage.GetSize().height;

  Matrix scaleMatrix;
  scaleMatrix.SetIdentityAndScale(Vector3(1.0, -1.0, 1.0));

  mShader.RegisterProperty(STAGE_OFFSET_UNIFORM_NAME, Vector2(width, height) / 2.0f);
  mShader.RegisterProperty(LIGHT_POSITION_NAME, mLightPosition);
  mShader.RegisterProperty(OBJECT_MATRIX_UNIFORM_NAME, scaleMatrix);
}

void MeshVisual::CreateShader()
{
  if(mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING)
  {
    mShader = Shader::New(SHADER_MESH_VISUAL_NORMAL_MAP_SHADER_VERT, SHADER_MESH_VISUAL_NORMAL_MAP_SHADER_FRAG);
  }
  else if(mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_SPECULAR_LIGHTING)
  {
    mShader = Shader::New(SHADER_MESH_VISUAL_SHADER_VERT, SHADER_MESH_VISUAL_SHADER_FRAG);
  }
  else //Textureless
  {
    mShader = Shader::New(SHADER_MESH_VISUAL_SIMPLE_SHADER_VERT, SHADER_MESH_VISUAL_SIMPLE_SHADER_FRAG);
  }

  UpdateShaderUniforms();
}

bool MeshVisual::CreateGeometry()
{
  //Determine if we need to use a simpler shader to handle the provided data
  if(!mUseTexture || !mObjLoader.IsDiffuseMapPresent())
  {
    mShadingMode = Toolkit::MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING;
  }
  else if(mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING && (!mObjLoader.IsNormalMapPresent() || !mObjLoader.IsSpecularMapPresent()))
  {
    mShadingMode = Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_SPECULAR_LIGHTING;
  }

  int objectProperties = 0;

  if(mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_SPECULAR_LIGHTING ||
     mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING)
  {
    objectProperties |= ObjLoader::TEXTURE_COORDINATES;
  }

  if(mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING)
  {
    objectProperties |= ObjLoader::TANGENTS | ObjLoader::BINORMALS;
  }

  //Create geometry with attributes required by shader.
  mGeometry = mObjLoader.CreateGeometry(objectProperties, mUseSoftNormals);

  if(mGeometry)
  {
    return true;
  }

  DALI_LOG_ERROR("Failed to load geometry in mesh visual.\n");
  return false;
}

bool MeshVisual::LoadGeometry()
{
  std::streampos     fileSize;
  Dali::Vector<char> fileContent;

  if(FileLoader::ReadFile(mObjectUrl, fileSize, fileContent, FileLoader::TEXT))
  {
    mObjLoader.ClearArrays();
    mObjLoader.LoadObject(fileContent.Begin(), fileSize);

    //Get size information from the obj loaded
    mSceneCenter = mObjLoader.GetCenter();
    mSceneSize   = mObjLoader.GetSize();

    return true;
  }

  DALI_LOG_ERROR("Failed to find object to load in mesh visual.\n");
  return false;
}

bool MeshVisual::LoadMaterial()
{
  std::streampos     fileSize;
  Dali::Vector<char> fileContent;

  if(FileLoader::ReadFile(mMaterialUrl, fileSize, fileContent, FileLoader::TEXT))
  {
    //Load data into obj (usable) form
    mObjLoader.LoadMaterial(fileContent.Begin(), fileSize, mDiffuseTextureUrl, mNormalTextureUrl, mGlossTextureUrl);
    return true;
  }

  DALI_LOG_ERROR("Failed to find texture set to load in mesh visual.\n");
  mUseTexture = false;
  return false;
}

bool MeshVisual::LoadTextures()
{
  mTextureSet = TextureSet::New();

  if(mShadingMode != Toolkit::MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING)
  {
    Sampler sampler = Sampler::New();
    if(mUseMipmapping)
    {
      sampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR_MIPMAP_LINEAR);
    }

    if(!mDiffuseTextureUrl.empty())
    {
      std::string imageUrl = mTexturesPath + mDiffuseTextureUrl;

      //Load textures
      Texture diffuseTexture = LoadTexture(imageUrl.c_str(), mUseMipmapping);
      if(diffuseTexture)
      {
        mTextureSet.SetTexture(DIFFUSE_INDEX, diffuseTexture);
        mTextureSet.SetSampler(DIFFUSE_INDEX, sampler);
      }
      else
      {
        DALI_LOG_ERROR("Failed to load diffuse map texture in mesh visual.\n");
        return false;
      }
    }

    if(!mNormalTextureUrl.empty() && (mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING))
    {
      std::string imageUrl = mTexturesPath + mNormalTextureUrl;

      //Load textures
      Texture normalTexture = LoadTexture(imageUrl.c_str(), mUseMipmapping);
      if(normalTexture)
      {
        mTextureSet.SetTexture(NORMAL_INDEX, normalTexture);
        mTextureSet.SetSampler(NORMAL_INDEX, sampler);
      }
      else
      {
        DALI_LOG_ERROR("Failed to load normal map texture in mesh visual.\n");
        return false;
      }
    }

    if(!mGlossTextureUrl.empty() && (mShadingMode == Toolkit::MeshVisual::ShadingMode::TEXTURED_WITH_DETAILED_SPECULAR_LIGHTING))
    {
      std::string imageUrl = mTexturesPath + mGlossTextureUrl;

      //Load textures
      Texture glossTexture = LoadTexture(imageUrl.c_str(), mUseMipmapping);
      if(glossTexture)
      {
        mTextureSet.SetTexture(GLOSS_INDEX, glossTexture);
        mTextureSet.SetSampler(GLOSS_INDEX, sampler);
      }
      else
      {
        DALI_LOG_ERROR("Failed to load gloss map texture in mesh visual.\n");
        return false;
      }
    }
  }
  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
