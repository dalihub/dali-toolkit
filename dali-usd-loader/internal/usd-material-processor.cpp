
/*
* Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-usd-loader/internal/usd-material-processor.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-usd-loader/internal/utils.h>

// EXTERNAL INCLUDES
#include <pxr/usd/usd/primRange.h>

using namespace Dali;
using namespace pxr;
using namespace Dali::Scene3D::Loader;

namespace Dali::Scene3D::Loader
{
namespace
{
#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_USD_MATERIAL_PROCESSOR");
#endif

// Map shader input names to the texture semantic flag
std::unordered_map<std::string, MaterialDefinition::Flags> SHADER_INPUT_NAME_TO_SEMANTIC_FLAG_MAP =
  {
    {"diffuseColor", MaterialDefinition::Flags::ALBEDO},
    {"metallic", MaterialDefinition::Flags::METALLIC},
    {"roughness", MaterialDefinition::Flags::ROUGHNESS},
    {"normal", MaterialDefinition::Flags::NORMAL},
    {"occlusion", MaterialDefinition::Flags::OCCLUSION},
    {"emissiveColor", MaterialDefinition::Flags::EMISSIVE},
    {"specularColor", MaterialDefinition::Flags::SPECULAR_COLOR}};
} // namespace

UsdMaterialProcessor::UsdMaterialProcessor(const UsdStageRefPtr& stage, MaterialMap* materialMap)
: mUsdStage(stage),
  mMaterialMap(materialMap)
{
  // Initialize the map of shader input handlers
  mShaderInputHandlers =
    {
      {MaterialDefinition::Flags::ALBEDO, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessDiffuseColor(input, materialDefinition, material, uvTexture); }},
      {MaterialDefinition::Flags::METALLIC, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessMetallic(input, materialDefinition, material, uvTexture); }},
      {MaterialDefinition::Flags::ROUGHNESS, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessRoughness(input, materialDefinition, material, uvTexture); }},
      {MaterialDefinition::Flags::NORMAL, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessNormal(input, materialDefinition, material, uvTexture); }},
      {MaterialDefinition::Flags::OCCLUSION, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessOcclusion(input, materialDefinition, material, uvTexture); }},
      {MaterialDefinition::Flags::EMISSIVE, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessEmissiveColor(input, materialDefinition, material, uvTexture); }},
      {MaterialDefinition::Flags::SPECULAR_COLOR, [&](const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture) { ProcessSpecularColor(input, materialDefinition, material, uvTexture); }}};
}

void UsdMaterialProcessor::TraverseMaterials(LoadResult& output)
{
  auto& outMaterials = output.mResources.mMaterials;

  int materialId = 0; // Initialize material ID counter

  // Traverse all prims (nodes) in the USD stage
  UsdPrimRange prims = mUsdStage->Traverse();
  for(auto prim : prims)
  {
    // Check if the current prim is a material
    if(prim.IsA<UsdShadeMaterial>())
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdShadeMaterial: %d: %s\n        ", materialId, prim.GetPrimPath().GetText());

      UsdShadeMaterial material = UsdShadeMaterial(prim);
      UsdShadeOutput   surf     = material.GetSurfaceOutput();

      // If no valid connected sources are found, skip this material
      if(surf.GetConnectedSources().size() == 0)
      {
        DALI_LOG_ERROR("No valid connected sources, ");
        continue;
      }

      UsdShadeConnectableAPI     previewSurface = surf.GetConnectedSources()[0].source;
      std::vector<UsdShadeInput> inputs         = previewSurface.GetInputs();

      // Initialize the material definition with default values
      MaterialDefinition materialDefinition;
      InitializeMaterialDefinition(materialDefinition);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mMaterialMap[%s] = %d, ", prim.GetPrimPath().GetAsString().c_str(), materialId);

      // Map the material path to the material ID
      (*mMaterialMap)[prim.GetPrimPath().GetAsString()] = materialId;
      materialId++;

      // Flags to track different material properties
      bool hasAlpha     = false;
      bool hasThreshold = false;

      float opacityThreshold(0.0f);

      // Sorted map to enforce processing order based on texture semantics
      std::map<MaterialDefinition::Flags, UsdShadeInput> shaderInputMap;

      for(const auto& input : inputs)
      {
        std::string baseName = input.GetBaseName();
        if(SHADER_INPUT_NAME_TO_SEMANTIC_FLAG_MAP.find(baseName) != SHADER_INPUT_NAME_TO_SEMANTIC_FLAG_MAP.end())
        {
          // Sort the shader input to match with the order of texture loading in MaterialDefinition.
          shaderInputMap[SHADER_INPUT_NAME_TO_SEMANTIC_FLAG_MAP[baseName]] = input;
        }
        else if(baseName == "opacity")
        {
          // Handle opacity input
          if(ProcessOpacity(input, materialDefinition))
          {
            hasAlpha = true;
          }

          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "hasAlpha: %d, ", hasAlpha);
        }
        else if(baseName == "opacityThreshold")
        {
          // Handle opacity threshold input (for alpha masking)
          opacityThreshold = ProcessOpacityThreshold(input);
          if(opacityThreshold > 0.0f)
          {
            hasThreshold = true;
          }
        }
        else if(baseName == "ior")
        {
          // Handle index of refraction (ior)
          ProcessIor(input, materialDefinition);
        }
      }

      // Process inputs in sorted order
      for(const auto& iter : shaderInputMap)
      {
        MaterialDefinition::Flags flag  = iter.first;
        UsdShadeInput             input = iter.second;

        // Check if the input has a connected texture source
        UsdShadeShader uvTexture;
        if(input.HasConnectedSource())
        {
          uvTexture = UsdShadeShader(input.GetConnectedSources()[0].source);
        }

        mShaderInputHandlers[flag](input, materialDefinition, material, uvTexture);
      }

      // Set alpha mode based on transparency and threshold values
      if(hasAlpha)
      {
        if(hasThreshold)
        {
          materialDefinition.mAlphaModeType = Scene3D::Material::AlphaModeType::MASK;
          materialDefinition.mIsMask        = true;
          materialDefinition.SetAlphaCutoff(std::min(1.f, std::max(0.f, opacityThreshold)));
        }
        else
        {
          materialDefinition.mAlphaModeType = Scene3D::Material::AlphaModeType::BLEND;
          materialDefinition.mIsOpaque      = false;
          materialDefinition.mFlags |= MaterialDefinition::TRANSPARENCY;
        }
      }

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: materialDefinition.mFlags: %u. needAlbedoTexture: %d, needMetallicRoughnessTexture: %d, needNormalTexture: %d\n", materialDefinition.mFlags, materialDefinition.mNeedAlbedoTexture, materialDefinition.mNeedMetallicRoughnessTexture, materialDefinition.mNeedNormalTexture);

      // Add the processed material to the output materials list
      outMaterials.emplace_back(std::move(materialDefinition), TextureSet());
    }
  }
}

void UsdMaterialProcessor::InitializeMaterialDefinition(MaterialDefinition& materialDefinition)
{
  materialDefinition.mFlags |= MaterialDefinition::GLTF_CHANNELS;

  materialDefinition.mBaseColorFactor     = Vector4::ONE;
  materialDefinition.mEmissiveFactor      = Vector3::ZERO;
  materialDefinition.mSpecularFactor      = 1.0f;
  materialDefinition.mSpecularColorFactor = Dali::Vector3::ONE;

  materialDefinition.mMetallic    = 1.0f;
  materialDefinition.mRoughness   = 1.0f;
  materialDefinition.mNormalScale = 1.0f;

  materialDefinition.mShadowAvailable = true;
  materialDefinition.mDoubleSided     = false;

  materialDefinition.mNeedAlbedoTexture            = false;
  materialDefinition.mNeedMetallicRoughnessTexture = false;
  materialDefinition.mNeedMetallicTexture          = false;
  materialDefinition.mNeedRoughnessTexture         = false;
  materialDefinition.mNeedNormalTexture            = false;
}

bool UsdMaterialProcessor::ProcessOpacity(const UsdShadeInput& input, MaterialDefinition& materialDefinition)
{
  UsdAttribute opacityAttr = input.GetAttr();
  float        opacity(1.0f);
  if(opacityAttr.HasAuthoredValue())
  {
    GetAttributeValue<float>(opacityAttr, opacity);
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "opacity: %f, ", opacity);

  // Set the alpha value in the base color factor
  materialDefinition.mBaseColorFactor.a = opacity;

  // Check if the material has transparency
  if(opacity < 1.0f || input.HasConnectedSource())
  {
    return true;
  }

  return false;
}

float UsdMaterialProcessor::ProcessOpacityThreshold(const UsdShadeInput& input)
{
  float opacityThreshold(0.0f);

  UsdAttribute opacityThresholdAttr = input.GetAttr();
  if(opacityThresholdAttr.HasAuthoredValue())
  {
    GetAttributeValue<float>(opacityThresholdAttr, opacityThreshold);
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "opacityThreshold: %.7f, ", opacityThreshold);

  return opacityThreshold;
}

void UsdMaterialProcessor::ProcessIor(const UsdShadeInput& input, MaterialDefinition& materialDefinition)
{
  UsdAttribute iorAttr = input.GetAttr();
  float        ior(1.5f);
  if(iorAttr.HasAuthoredValue())
  {
    GetAttributeValue<float>(iorAttr, ior);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ior: %.7f, ", ior);

    materialDefinition.mIor                = ior;
    materialDefinition.mDielectricSpecular = powf((materialDefinition.mIor - 1.0f) / (materialDefinition.mIor + 1.0f), 2.0f);
  }
}

void UsdMaterialProcessor::ProcessDiffuseColor(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process diffuse color (albedo)
  UsdAttribute diffuseAttr = input.GetAttr();
  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "diffuseColorTexture: ");
      materialDefinition.mNeedAlbedoTexture = mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::ALBEDO);
      if(materialDefinition.mNeedAlbedoTexture)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::ALBEDO, ");
      }
    }
  }
  else
  {
    GfVec3f diffuseColor(0.18f, 0.18f, 0.18f);
    if(diffuseAttr.HasAuthoredValue())
    {
      GetAttributeValue<GfVec3f>(diffuseAttr, diffuseColor);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "diffuseColor: %.7f, %.7f, %.7f, ", diffuseColor[0], diffuseColor[1], diffuseColor[2]);
    }

    // Set the base color factor of the material
    materialDefinition.mBaseColorFactor.r = diffuseColor[0];
    materialDefinition.mBaseColorFactor.g = diffuseColor[1];
    materialDefinition.mBaseColorFactor.b = diffuseColor[2];
  }
}

void UsdMaterialProcessor::ProcessMetallic(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process metallic input
  UsdAttribute metallicAttr = input.GetAttr();

  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "metallicTexture: ");
      materialDefinition.mNeedMetallicTexture = mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::METALLIC);
      if(materialDefinition.mNeedMetallicTexture)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::METALLIC, ");
      }
    }
  }
  else if(metallicAttr.HasAuthoredValue())
  {
    float metallicFactor(0.0f);
    GetAttributeValue<float>(metallicAttr, metallicFactor);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "metallicFactor: %.7f， ", metallicFactor);

    // Set the metallic factor of the material
    materialDefinition.mMetallic = metallicFactor;
  }
}

void UsdMaterialProcessor::ProcessRoughness(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process roughness input
  UsdAttribute roughnessAttr = input.GetAttr();
  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "roughnessTexture: ");
      materialDefinition.mNeedRoughnessTexture = mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::ROUGHNESS);
      if(materialDefinition.mNeedRoughnessTexture)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::MaterialDefinition::ROUGHNESS, ");
      }
    }
  }
  else if(roughnessAttr.HasAuthoredValue())
  {
    float roughnessFactor(0.5f);
    GetAttributeValue<float>(roughnessAttr, roughnessFactor);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "roughnessFactor: %.7f， ", roughnessFactor);

    // Set the roughness factor of the material
    materialDefinition.mRoughness = roughnessFactor;
  }
}

void UsdMaterialProcessor::ProcessNormal(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process normal map input
  UsdAttribute normalAttr = input.GetAttr();
  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normalTexture: ");
      materialDefinition.mNeedNormalTexture = mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::NORMAL);
      if(materialDefinition.mNeedNormalTexture)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::NORMAL, ");
      }
    }
  }
  else if(normalAttr.HasAuthoredValue())
  {
    GfVec3f normal;
    GetAttributeValue<GfVec3f>(normalAttr, normal);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normal: %.7f, %.7f, %.7f， ", normal[0], normal[1], normal[2]);
  }
}

void UsdMaterialProcessor::ProcessOcclusion(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process occlusion map input
  UsdAttribute occlusionAttr = input.GetAttr();
  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "occlusionTexture: ");
      if(mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::OCCLUSION))
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::OCCLUSION, ");
      }
    }
  }
  else if(occlusionAttr.HasAuthoredValue())
  {
    float occlusion(1.0f);
    GetAttributeValue<float>(occlusionAttr, occlusion);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "occlusion: %.7f， ", occlusion);
  }
}

void UsdMaterialProcessor::ProcessEmissiveColor(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process emissive color input
  UsdAttribute emissiveAttr = input.GetAttr();
  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "emissiveColorTexture: ");
      if(mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::EMISSIVE))
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::EMISSIVE, ");
        materialDefinition.mEmissiveFactor = Vector3::ONE;
      }

      // Handle emissive color scale
      UsdShadeInput scaleInput = uvTexture.GetInput(TfToken("scale"));
      if(scaleInput)
      {
        GfVec4d scale;
        scaleInput.Get<GfVec4d>(&scale);
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "emissiveColorScale: %.7f, %.7f, %.7f, %.7f, ", scale[0], scale[1], scale[2], scale[3]);
      }
    }
  }

  if(emissiveAttr.HasAuthoredValue())
  {
    GfVec3f emissiveFactor;
    GetAttributeValue<GfVec3f>(emissiveAttr, emissiveFactor);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "emissiveFactor: %.7f, %.7f, %.7f， ", emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);

    // Set the emissive factor of the material
    materialDefinition.mEmissiveFactor = Vector3(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);
  }
}

void UsdMaterialProcessor::ProcessSpecularColor(const UsdShadeInput& input, MaterialDefinition& materialDefinition, const UsdShadeMaterial& material, const UsdShadeShader& uvTexture)
{
  // Process specular color input
  UsdAttribute specularAttr = input.GetAttr();
  if(input.HasConnectedSource())
  {
    TfToken id;
    if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
    {
      // Convert the texture and associate it with the material
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "specularColorTexture: ");
      if(mUsdTextureConverter.ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::SPECULAR_COLOR))
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::SPECULAR_COLOR, ");
      }
    }
  }
  else if(specularAttr.HasAuthoredValue())
  {
    GfVec3f specularColor;
    GetAttributeValue<GfVec3f>(specularAttr, specularColor);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "specularColor: %.7f, %.7f, %.7f， ", specularColor[0], specularColor[1], specularColor[2]);

    // Set the specular color factor of the material
    materialDefinition.mSpecularColorFactor = Vector3(specularColor[0], specularColor[1], specularColor[2]);
  }
}
} // namespace Dali::Scene3D::Loader
