#ifndef DALI_SCENE3D_LOADER_USD_TEXTURE_CONVERTER_H
#define DALI_SCENE3D_LOADER_USD_TEXTURE_CONVERTER_H
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
#include <dali-scene3d/public-api/loader/load-scene-metadata.h>
#include <dali-usd-loader/internal/utils.h>

namespace Dali::Scene3D::Loader
{
class MaterialDefinition;

using ImageMetadataMap = std::unordered_map<std::string, ImageMetadata>;

/**
 * @class UsdTextureConverter
 * @brief Handles the conversion of textures for materials in USD models.
 *
 * The UsdTextureConverter class is responsible for processing various texture properties such as
 * file paths, UV transformations, and texture attributes (wrapS, scale, etc.). It encapsulates
 * the logic required to extract these properties from USD shaders and apply them to the
 * MaterialDefinition used in the USD loader.
 */
class DALI_INTERNAL UsdTextureConverter
{
public:
  /**
   * @brief Constructor for UsdTextureConverter.
   */
  UsdTextureConverter();

  /**
   * @brief Converts a texture from a USD shader and adds it to the material definition.
   *
   * This method processes the inputs and attributes of a USD texture shader (UsdShadeShader) and
   * converts the texture into a format usable by the application. It handles UV transformations,
   * texture file loading, and wraps various texture properties such as wrapS, scale, and bias.
   *
   * @param usdMaterial The USD material associated with the texture.
   * @param usdUvTexture The USD shader representing the UV texture.
   * @param materialDefinition The material definition to which the texture will be added.
   * @param semantic The semantic value used to identify the type of texture (e.g., albedo, normal map).
   * @param imageMetaDataMap The map of image files to their meta data
   * @return True if the texture was successfully converted and added to the material definition, false otherwise.
   */
  bool ConvertTexture(const pxr::UsdShadeMaterial& usdMaterial, const pxr::UsdShadeShader& usdUvTexture, MaterialDefinition& materialDefinition, const ImageMetadataMap& imageMetaDataMap, uint32_t semantic);

private:
  /**
   * @brief Processes the UV channel in the USD shader.
   *
   * This function extracts the "varname" input from the USD shader, which represents the UV map name.
   * The UV map name is used to map texture coordinates.
   *
   * @param shader The USD shader representing the UV channel.
   */
  void ProcessUvChannel(const pxr::UsdShadeShader& shader);

  /**
   * @brief Processes 2D transform attributes in a USD shader (translation, scale, rotation).
   *
   * The function maps shader inputs to respective UV transformation operations (translation, scale, and rotation),
   * and processes them by updating the corresponding UV transformation variables.
   *
   * @param shader The USD shader representing the 2D transform.
   * @param uvTransformOffset The UV offset vector to be updated.
   * @param uvTransformRotation The UV rotation value to be updated.
   * @param uvTransformScale The UV scale vector to be updated.
   * @return True if any transform offsets were authored, false otherwise.
   */
  bool Process2DTransform(const pxr::UsdShadeShader& shader, pxr::GfVec2f& uvTransformOffset, float& uvTransformRotation, pxr::GfVec2f& uvTransformScale);

  /**
   * @brief Processes the translation input in a 2D transform.
   * @param input The USD shader input for translation.
   * @param uvTransformOffset The UV offset vector to be updated.
   */
  void ProcessTransformTranslation(const pxr::UsdShadeInput& input, pxr::GfVec2f& uvTransformOffset);

  /**
   * @brief Processes the scale input in a 2D transform.
   * @param input The USD shader input for scale.
   * @param uvTransformScale The UV scale vector to be updated.
   */
  void ProcessTransformScale(const pxr::UsdShadeInput& input, pxr::GfVec2f& uvTransformScale);

  /**
   * @brief Processes the rotation input in a 2D transform.
   * @param input The USD shader input for rotation.
   * @param uvTransformRotation The UV rotation value to be updated.
   */
  void ProcessTransformRotation(const pxr::UsdShadeInput& input, float& uvTransformRotation);

  /**
   * @brief Processes various texture attributes such as file, wrapS, wrapT, etc., for a given texture.
   *
   * This function maps texture attributes to their respective processing functions, extracting relevant
   * properties and updating the material definition.
   *
   * @param usdUvTexture The USD shader representing the UV texture.
   * @param shader The USD shader representing the texture attributes.
   * @param materialDefinition The material definition to which the texture properties will be applied.
   * @param semantic The semantic value identifying the texture type.
   * @param imageMetaDataMap The map of image files to their meta data
   * @return True if texture attributes were successfully processed and added to the material definition, false otherwise.
   */
  bool ProcessTextureAttributes(const pxr::UsdShadeShader& usdUvTexture, const pxr::UsdShadeShader& shader, MaterialDefinition& materialDefinition, const ImageMetadataMap& imageMetaDataMap, uint32_t semantic);

  /**
   * @brief Processes the texture file input and retrieves the image path or buffer.
   * @param input The USD shader input representing the texture file.
   * @param imagePath The variable to store the resolved image path.
   * @param imageBuffer The variable to store the loaded image buffer.
   */
  void ProcessTextureFile(const pxr::UsdShadeInput& input, std::string& imagePath, UsdAssetBuffer& imageBuffer);

  /**
   * @brief Processes the texture wrapping (wrapS or wrapT) input.
   * @param input The USD shader input representing texture wrapping.
   */
  void ProcessTextureWrap(const pxr::UsdShadeInput& input);

  /**
   * @brief Processes the texture scale input.
   * @param input The USD shader input representing texture scale.
   */
  void ProcessTextureScale(const pxr::UsdShadeInput& input);

  /**
   * @brief Processes the texture bias input.
   * @param input The USD shader input representing texture bias.
   */
  void ProcessTextureBias(const pxr::UsdShadeInput& input);

  /**
   * @brief Processes the texture ST (UV) coordinates input.
   * @param input The USD shader input representing the texture UV coordinates.
   */
  void ProcessTextureST(const pxr::UsdShadeInput& input);

  /**
   * @brief Processes the fallback color for the texture input.
   * @param input The USD shader input representing the fallback color.
   */
  void ProcessTextureFallback(const pxr::UsdShadeInput& input);

  /**
   * @brief Processes the image buffer and/or path and adds the texture to the material definition.
   *
   * This method adds the loaded texture (either as a buffer or file path) to the material definition's
   * texture stages.
   *
   * @param materialDefinition The material definition to which the texture will be added.
   * @param semantic The semantic value used to identify the type of texture (e.g., albedo, normal map).
   * @param imagePath The file path of the texture image.
   * @param imageBuffer The image buffer of the texture.
   * @param imageMetaDataMap The map of image files to their meta data
   * @return True if the texture was successfully processed, false otherwise.
   */
  bool ProcessImageBuffer(MaterialDefinition& materialDefinition, uint32_t semantic, const std::string& imagePath, UsdAssetBuffer& imageBuffer, const ImageMetadataMap& imageMetaDataMap);
};
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_USD_TEXTURE_CONVERTER_H
