/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/node-definition.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/model-components/material-impl.h>
#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/internal/model-components/model-primitive-impl.h>
#include <dali-scene3d/public-api/loader/renderer-state.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace Dali
{
namespace
{
static constexpr std::string_view IBL_INTENSITY_STRING("uIblIntensity");
static constexpr std::string_view IBL_Y_DIRECTION("uYDirection");
static constexpr std::string_view IBL_MAXLOD("uMaxLOD");

static constexpr uint32_t MAX_NUMBER_OF_MATERIAL_TEXTURE = 7;
static constexpr uint32_t SEMANTICS[MAX_NUMBER_OF_MATERIAL_TEXTURE] =
  {
    Scene3D::Loader::MaterialDefinition::ALBEDO,
    Scene3D::Loader::MaterialDefinition::METALLIC | Scene3D::Loader::MaterialDefinition::ROUGHNESS,
    Scene3D::Loader::MaterialDefinition::NORMAL,
    Scene3D::Loader::MaterialDefinition::OCCLUSION,
    Scene3D::Loader::MaterialDefinition::EMISSIVE,
    Scene3D::Loader::MaterialDefinition::SPECULAR,
    Scene3D::Loader::MaterialDefinition::SPECULAR_COLOR,
};

static constexpr Scene3D::Material::TextureType TEXTURE_TYPES[MAX_NUMBER_OF_MATERIAL_TEXTURE] =
  {
    Scene3D::Material::TextureType::BASE_COLOR,
    Scene3D::Material::TextureType::METALLIC_ROUGHNESS,
    Scene3D::Material::TextureType::NORMAL,
    Scene3D::Material::TextureType::OCCLUSION,
    Scene3D::Material::TextureType::EMISSIVE,
    Scene3D::Material::TextureType::SPECULAR,
    Scene3D::Material::TextureType::SPECULAR_COLOR,
};

Vector4 GetTextureFactor(Scene3D::Loader::MaterialDefinition& materialDefinition, uint32_t semantic)
{
  Vector4 factor = Vector4::ONE;
  switch(semantic)
  {
    case Scene3D::Loader::MaterialDefinition::ALBEDO:
      factor = materialDefinition.mBaseColorFactor;
      break;
    case Scene3D::Loader::MaterialDefinition::METALLIC | Scene3D::Loader::MaterialDefinition::ROUGHNESS:
      factor = Vector4(materialDefinition.mMetallic, materialDefinition.mRoughness, 0.0f, 0.0f);
      break;
    case Scene3D::Loader::MaterialDefinition::NORMAL:
      factor.x = materialDefinition.mNormalScale;
      break;
    case Scene3D::Loader::MaterialDefinition::OCCLUSION:
      factor.x = materialDefinition.mOcclusionStrength;
      break;
    case Scene3D::Loader::MaterialDefinition::EMISSIVE:
      factor = materialDefinition.mEmissiveFactor;
      break;
    case Scene3D::Loader::MaterialDefinition::SPECULAR:
      factor.x = materialDefinition.mSpecularFactor;
      break;
    case Scene3D::Loader::MaterialDefinition::SPECULAR_COLOR:
      factor = materialDefinition.mSpecularColorFactor;
      break;
    default:
      break;
  }
  return factor;
}

} // namespace

namespace Scene3D
{
namespace Loader
{
bool NodeDefinition::Renderable::GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const
{
  return false;
}

void NodeDefinition::Renderable::RegisterResources(IResourceReceiver& receiver) const
{
  receiver.Register(ResourceType::Shader, mShaderIdx);
}

void NodeDefinition::Renderable::ReflectResources(IResourceReflector& reflector)
{
  reflector.Reflect(ResourceType::Shader, mShaderIdx);
}

void NodeDefinition::Renderable::OnCreate(const NodeDefinition& nodeDefinition, CreateParams& params, ModelNode& node) const
{
  DALI_ASSERT_DEBUG(mShaderIdx != INVALID_INDEX);
  auto&  resources = params.mResources;
  Shader shader    = resources.mShaders[mShaderIdx].second;

  static Geometry defaultGeometry = Geometry::New();
  Renderer        renderer        = Renderer::New(defaultGeometry, shader);

  RendererState::Apply(resources.mShaders[mShaderIdx].first.mRendererState, renderer);

  node.AddRenderer(renderer);
}

const char* NodeDefinition::ORIGINAL_MATRIX_PROPERTY_NAME = "originalMatrix";

ModelNode NodeDefinition::CreateModelNode(CreateParams& params)
{
  ModelNode node = ModelNode::New();
  mNodeId        = node.GetProperty<int32_t>(Dali::Actor::Property::ID);

  SetActorCentered(node);

  node.SetProperty(Actor::Property::NAME, mName);
  node.SetProperty(Actor::Property::POSITION, mPosition);
  node.SetProperty(Actor::Property::ORIENTATION, mOrientation);
  node.SetProperty(Actor::Property::SCALE, mScale);
  node.SetProperty(Actor::Property::SIZE, mSize);
  node.SetProperty(Actor::Property::VISIBLE, mIsVisible);

  node.RegisterProperty(ORIGINAL_MATRIX_PROPERTY_NAME, GetLocalSpace(), Property::AccessMode::READ_ONLY);

  node.SetProperty(Actor::Property::COLOR_MODE, ColorMode::USE_OWN_MULTIPLY_PARENT_COLOR);

  for(auto& renderable : mRenderables)
  {
    renderable->OnCreate(*this, params, node);
  }

  for(auto& e : mExtras)
  {
    node.RegisterProperty(e.mKey, e.mValue);
  }

  for(auto& c : mConstraints)
  {
    params.mConstrainables.push_back(ConstraintRequest{&c, node});
  }

  return node;
}

Matrix NodeDefinition::GetLocalSpace() const
{
  Matrix localSpace{false};
  localSpace.SetTransformComponents(mScale, mOrientation, mPosition);
  return localSpace;
}

std::string_view NodeDefinition::GetIblScaleFactorUniformName()
{
  return IBL_INTENSITY_STRING;
}

std::string_view NodeDefinition::GetIblYDirectionUniformName()
{
  return IBL_Y_DIRECTION;
}

std::string_view NodeDefinition::GetIblMaxLodUniformName()
{
  return IBL_MAXLOD;
}

bool NodeDefinition::GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const
{
  if(mRenderables.empty())
  {
    return false;
  }

  bool useModelExtents = false;
  for(auto& renderable : mRenderables)
  {
    Vector3 renderableMin(Vector3::ONE * MAXFLOAT), renderableMax(-Vector3::ONE * MAXFLOAT);
    if(!renderable->GetExtents(resources, renderableMin, renderableMax))
    {
      useModelExtents = false;
      break;
    }
    useModelExtents = true;
    min.x           = std::min(min.x, renderableMin.x);
    min.y           = std::min(min.y, renderableMin.y);
    min.z           = std::min(min.z, renderableMin.z);
    max.x           = std::max(max.x, renderableMax.x);
    max.y           = std::max(max.y, renderableMax.y);
    max.z           = std::max(max.z, renderableMax.z);
  }
  if(!useModelExtents)
  {
    // If the renderable node don't have mesh accessor, use size to compute extents.
    min = -mSize / 2.0f;
    max = mSize / 2.0f;
  }
  return true;
}

bool ModelRenderable::GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const
{
  auto&    mesh    = resources.mMeshes[mMeshIdx];
  uint32_t minSize = mesh.first.mPositions.mBlob.mMin.size();
  uint32_t maxSize = mesh.first.mPositions.mBlob.mMax.size();
  if(minSize == maxSize && minSize >= 2u && maxSize >= 2u)
  {
    min = Vector3(mesh.first.mPositions.mBlob.mMin[0], mesh.first.mPositions.mBlob.mMin[1], 0.0f);
    max = Vector3(mesh.first.mPositions.mBlob.mMax[0], mesh.first.mPositions.mBlob.mMax[1], 0.0f);
    if(minSize == 3u)
    {
      min.z = mesh.first.mPositions.mBlob.mMin[2];
      max.z = mesh.first.mPositions.mBlob.mMax[2];
    }
    return true;
  }
  return false;
}

void ModelRenderable::RegisterResources(IResourceReceiver& receiver) const
{
  Renderable::RegisterResources(receiver);
  receiver.Register(ResourceType::Mesh, mMeshIdx);
  receiver.Register(ResourceType::Material, mMaterialIdx);
}

void ModelRenderable::ReflectResources(IResourceReflector& reflector)
{
  Renderable::ReflectResources(reflector);
  reflector.Reflect(ResourceType::Mesh, mMeshIdx);
  reflector.Reflect(ResourceType::Material, mMaterialIdx);
}

void ModelRenderable::OnCreate(const NodeDefinition& nodeDefinition, NodeDefinition::CreateParams& params, ModelNode& node) const
{
  DALI_ASSERT_DEBUG(mMeshIdx != INVALID_INDEX);
  if(mShaderIdx == INVALID_INDEX)
  {
    Shader          shader          = params.mShaderManager->ProduceShader(params.mResources.mMaterials[mMaterialIdx].first, params.mResources.mMeshes[mMeshIdx].first);
    static Geometry defaultGeometry = Geometry::New();
    Renderer        renderer        = Renderer::New(defaultGeometry, shader);

    RendererState::Apply(params.mShaderManager->GetRendererState(params.mResources.mMaterials[mMaterialIdx].first), renderer);
    Internal::GetImplementation(node).UpdateShader(params.mShaderManager);
    node.AddRenderer(renderer);
  }
  else
  {
    Renderable::OnCreate(nodeDefinition, params, node);
  }

  auto& resources = params.mResources;
  auto& mesh      = resources.mMeshes[mMeshIdx];

  auto     renderer = node.GetRendererAt(node.GetRendererCount() - 1u);
  Geometry geometry = mesh.second.geometry;
  renderer.SetGeometry(geometry);

  TextureSet textures = resources.mMaterials[mMaterialIdx].second;
  // Set the blend shape texture.
  if(mesh.second.blendShapeGeometry)
  {
    TextureSet newTextureSet = TextureSet::New();
    newTextureSet.SetTexture(0u, mesh.second.blendShapeGeometry);

    const unsigned int numberOfTextures = textures.GetTextureCount();
    for(unsigned int index = 0u; index < numberOfTextures; ++index)
    {
      const unsigned int newIndex = index + 1u;
      newTextureSet.SetTexture(newIndex, textures.GetTexture(index));
      newTextureSet.SetSampler(newIndex, textures.GetSampler(index));
    }

    textures = newTextureSet;
  }
  renderer.SetTextures(textures);

  {
    mesh.first.mModelPrimitive = ModelPrimitive::New();
    auto primitive             = mesh.first.mModelPrimitive;
    GetImplementation(primitive).SetRenderer(renderer);

    Index    envIndex       = resources.mMaterials[mMaterialIdx].first.mEnvironmentIdx;
    uint32_t specularMipmap = resources.mEnvironmentMaps[envIndex].second.mSpecularMipmapLevels;
    GetImplementation(primitive).SetImageBasedLightTexture(resources.mEnvironmentMaps[envIndex].second.mDiffuse,
                                                           resources.mEnvironmentMaps[envIndex].second.mSpecular,
                                                           resources.mEnvironmentMaps[envIndex].first.mIblIntensity,
                                                           specularMipmap);

    bool hasPositions = false;
    bool hasNormals   = false;
    bool hasTangents  = false;
    mesh.first.RetrieveBlendShapeComponents(hasPositions, hasNormals, hasTangents);
    GetImplementation(primitive).SetBlendShapeOptions(hasPositions, hasNormals, hasTangents, mesh.first.mBlendShapeVersion);
    GetImplementation(primitive).SetBlendShapeGeometry(mesh.second.blendShapeGeometry);
    GetImplementation(primitive).SetSkinned(mesh.first.IsSkinned());
  }

  auto shader = renderer.GetShader();
  if(mesh.first.IsSkinned())
  {
    params.mSkinnables.push_back(SkinningShaderConfigurationRequest{mesh.first.mSkeletonIdx, shader, mesh.first.mModelPrimitive});
  }

  if(mesh.first.HasBlendShapes())
  {
    params.mBlendshapeRequests.push_back(BlendshapeShaderConfigurationRequest{nodeDefinition.mName, mMeshIdx, shader, mesh.first.mModelPrimitive});
  }

  auto& matDef = resources.mMaterials[mMaterialIdx].first;
  renderer.RegisterProperty("uColorFactor", matDef.mBaseColorFactor);
  renderer.RegisterProperty("uMetallicFactor", matDef.mMetallic);
  renderer.RegisterProperty("uRoughnessFactor", matDef.mRoughness);
  renderer.RegisterProperty("uDielectricSpecular", matDef.mDielectricSpecular);
  renderer.RegisterProperty("uSpecularFactor", matDef.mSpecularFactor);
  renderer.RegisterProperty("uSpecularColorFactor", matDef.mSpecularColorFactor);
  renderer.RegisterProperty("uNormalScale", matDef.mNormalScale);
  renderer.RegisterProperty("uEmissiveFactor", matDef.mEmissiveFactor);
  if(matDef.mFlags & MaterialDefinition::OCCLUSION)
  {
    renderer.RegisterProperty("uOcclusionStrength", matDef.mOcclusionStrength);
  }

  float opaque      = matDef.mIsOpaque ? 1.0f : 0.0f;
  float mask        = matDef.mIsMask ? 1.0f : 0.0f;
  float alphaCutoff = matDef.GetAlphaCutoff();

  renderer.RegisterProperty("uOpaque", opaque);
  renderer.RegisterProperty("uMask", mask);
  renderer.RegisterProperty("uAlphaThreshold", alphaCutoff);

  Index    envIndex       = matDef.mEnvironmentIdx;
  uint32_t specularMipmap = resources.mEnvironmentMaps[envIndex].second.mSpecularMipmapLevels;
  renderer.RegisterProperty(IBL_MAXLOD.data(), static_cast<float>(specularMipmap));
  renderer.RegisterProperty(IBL_INTENSITY_STRING.data(), resources.mEnvironmentMaps[envIndex].first.mIblIntensity);
  renderer.RegisterProperty(IBL_Y_DIRECTION.data(), resources.mEnvironmentMaps[envIndex].first.mYDirection);

  std::string lightCountPropertyName(Scene3D::Internal::Light::GetLightCountUniformName());
  renderer.RegisterProperty(lightCountPropertyName, 0);

  uint32_t maxLightCount = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
  for(uint32_t i = 0; i < maxLightCount; ++i)
  {
    std::string lightDirectionPropertyName(Scene3D::Internal::Light::GetLightDirectionUniformName());
    lightDirectionPropertyName += "[" + std::to_string(i) + "]";
    renderer.RegisterProperty(lightDirectionPropertyName, Vector3::ZAXIS);

    std::string lightColorPropertyName(Scene3D::Internal::Light::GetLightColorUniformName());
    lightColorPropertyName += "[" + std::to_string(i) + "]";
    renderer.RegisterProperty(lightColorPropertyName, Vector3(Color::WHITE));
  }

  node.SetProperty(Actor::Property::COLOR, mColor);

  // If user uses customshader, the properties of the shader could not be changed by Material.
  if(mShaderIdx == INVALID_INDEX)
  {
    matDef.mMaterial            = Material::New();
    auto     material           = matDef.mMaterial;
    uint32_t textureIndexOffset = (mesh.second.blendShapeGeometry) ? 1 : 0;
    uint32_t textureIndex       = 0;
    for(uint32_t i = 0; i < MAX_NUMBER_OF_MATERIAL_TEXTURE; ++i)
    {
      Internal::Material::TextureInformation textureInformation;
      if(matDef.CheckTextures(SEMANTICS[i]))
      {
        textureInformation.mTexture = textures.GetTexture(textureIndex + textureIndexOffset);
        textureInformation.mSampler = textures.GetSampler(textureIndex + textureIndexOffset);
        textureInformation.mUrl     = matDef.mTextureStages[textureIndex].mTexture.mDirectoryPath + matDef.mTextureStages[textureIndex].mTexture.mImageUri;
        textureIndex++;
      }
      textureInformation.mFactor = GetTextureFactor(matDef, SEMANTICS[i]);
      GetImplementation(material).SetTextureInformation(TEXTURE_TYPES[i], std::move(textureInformation));
    }
    material.SetProperty(Scene3D::Material::Property::ALPHA_MODE, matDef.mAlphaModeType);
    material.SetProperty(Scene3D::Material::Property::ALPHA_CUTOFF, matDef.GetAlphaCutoff());
    material.SetProperty(Scene3D::Material::Property::DOUBLE_SIDED, matDef.mDoubleSided);
    material.SetProperty(Scene3D::Material::Property::IOR, matDef.mIor);
    GetImplementation(mesh.first.mModelPrimitive).SetMaterial(material, false);
    GetImplementation(material).ResetFlag();
  }

  node.AddModelPrimitive(mesh.first.mModelPrimitive);
}

void ArcRenderable::OnCreate(const NodeDefinition& nodeDefinition, NodeDefinition::CreateParams& params, ModelNode& node) const
{
  ModelRenderable::OnCreate(nodeDefinition, params, node);

  node.RegisterProperty("antiAliasing", mAntiAliasing ? 1 : 0);
  node.RegisterProperty("arcCaps", mArcCaps);
  node.RegisterProperty("radius", mRadius);

  const float startAngleRadians = mStartAngleDegrees * Math::PI_OVER_180;
  Vector2     startPolar{std::cos(startAngleRadians), std::sin(startAngleRadians)};
  node.RegisterProperty("startAngle", startPolar);

  const float endAngleRadians = mEndAngleDegrees * Math::PI_OVER_180;
  Vector2     endPolar{std::cos(endAngleRadians), std::sin(endAngleRadians)};
  node.RegisterProperty("endAngle", endPolar);
}

void ArcRenderable::GetEndVectorWithDiffAngle(float startAngle, float diffAngle, Vector2& endVector)
{
  float endAngle = 0.f;

  if(diffAngle <= 0.001f)
  {
    // 0.001 is used to ensure is empty arc when startAngle = endAngle + 360 * N
    endAngle = startAngle + 0.001f;
  }
  else if(diffAngle >= 360.f)
  {
    endAngle = diffAngle + 359.99f;
  }
  else
  {
    endAngle = startAngle + 360.0f + diffAngle;
  }
  endVector.x = cosf(endAngle * Math::PI_OVER_180);
  endVector.y = sinf(endAngle * Math::PI_OVER_180);
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
