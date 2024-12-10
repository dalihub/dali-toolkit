#ifndef DALI_SCENE3D_LOADER_USD_MATERIAL_PROCESSOR_H
#define DALI_SCENE3D_LOADER_USD_MATERIAL_PROCESSOR_H
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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// Some TBB/USD headers produce compile warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#include <pxr/usd/usdShade/material.h>
#pragma GCC diagnostic pop

#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/material-definition.h>
#include <dali-usd-loader/internal/usd-texture-converter.h>

namespace Dali
{
class TextureSet;
}

namespace Dali::Scene3D::Loader
{
struct LoadResult;

/**
 * @class MaterialProcessor
 * @brief Responsible for processing USD materials and converting shader inputs to the MaterialDefinition objects.
 */
class UsdMaterialProcessor
{
public:
  using MaterialMap = std::map<std::string, int>;

  /**
   * @brief Constructor for MaterialProcessor.
   * @param stage The USD stage to traverse and process materials from.
   * @param materialMap The map of prim path and its corresponding material ID
   * @param imageMetaDataMap The map of image files to their meta data
   */
  UsdMaterialProcessor(const pxr::UsdStageRefPtr& stage, MaterialMap* materialMap, const ImageMetadataMap& imageMetaDataMap);

  /**
   * @brief Traverses the USD stage and processes all materials.
   * @param output The LoadResult object to store processed materials.
   */
  void TraverseMaterials(LoadResult& output);

private:
  /**
   * @brief Initializes the material definition with default values.
   * @param materialDefinition The material definition to be initialized.
   */
  void InitializeMaterialDefinition(MaterialDefinition& materialDefinition);

  /**
   * @brief Processes the opacity input of a material.
   * @param input The shader input for opacity.
   * @param materialDefinition The material definition to store the converted result.
   * return Whether the material has transparency.
   */
  bool ProcessOpacity(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition);

  /**
   * @brief Processes the opacity threshold input of a material.
   * @param input The shader input for opacity threshold.
   * return The threshold of the opacity.
   */
  float ProcessOpacityThreshold(const pxr::UsdShadeInput& input);

  /**
   * @brief Processes the ior (Index of Refraction) input of a material.
   * @param input The shader input for ior.
   * @param materialDefinition The material definition to store the converted result.
   */
  void ProcessIor(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition);

  /**
   * @brief Processes the diffuse color (albedo) input of a material.
   * @param input The shader input for diffuse color.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessDiffuseColor(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

  /**
   * @brief Processes the metallic input of a material.
   * @param input The shader input for metallic factor.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessMetallic(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

  /**
   * @brief Processes the roughness input of a material.
   * @param input The shader input for roughness factor.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessRoughness(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

  /**
   * @brief Processes the normal map input of a material.
   * @param input The shader input for normal map.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessNormal(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

  /**
   * @brief Processes the occlusion input of a material.
   * @param input The shader input for occlusion.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessOcclusion(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

  /**
   * @brief Processes the emissive color input of a material.
   * @param input The shader input for emissive color.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessEmissiveColor(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

  /**
   * @brief Processes the specular color input of a material.
   * @param input The shader input for specular color.
   * @param materialDefinition The material definition to store the converted result.
   * @param material The USD material.
   * @param uvTexture The USD UV texture shader.
   */
  void ProcessSpecularColor(const pxr::UsdShadeInput& input, MaterialDefinition& materialDefinition, const pxr::UsdShadeMaterial& material, const pxr::UsdShadeShader& uvTexture);

private:
  pxr::UsdStageRefPtr     mUsdStage;
  MaterialMap*            mMaterialMap;      ///< Maps prim paths to material IDs.
  const ImageMetadataMap& mImageMetaDataMap; ///< Maps image files to their meta data
  UsdTextureConverter     mUsdTextureConverter;
  using ShaderHandlerMap = std::unordered_map<MaterialDefinition::Flags, std::function<void(const pxr::UsdShadeInput&, MaterialDefinition&, const pxr::UsdShadeMaterial&, const pxr::UsdShadeShader&)>>;
  ShaderHandlerMap mShaderInputHandlers;
};
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_USD_MATERIAL_PROCESSOR_H
