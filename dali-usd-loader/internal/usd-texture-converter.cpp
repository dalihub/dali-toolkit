
/*
* Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-usd-loader/internal/usd-texture-converter.h>

// EXTERNAL INCLUDES
#include <filesystem>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/material-definition.h>
#include <dali-usd-loader/internal/utils.h>

using namespace Dali;
using namespace pxr;
using namespace Dali::Scene3D::Loader;

namespace Dali::Scene3D::Loader
{
namespace
{
#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_USD_TEXTURE_CONVERTER");
#endif
} // namespace

UsdTextureConverter::UsdTextureConverter() = default;

bool UsdTextureConverter::ConvertTexture(const UsdShadeMaterial& usdMaterial, const UsdShadeShader& usdUvTexture, MaterialDefinition& materialDefinition, const ImageMetadataMap& imageMetaDataMap, uint32_t semantic)
{
  bool    transformOffsetAuthored = false;
  GfVec2f uvTransformOffset(0.0f, 0.0f);
  float   uvTransformRotation = 0.0f;
  GfVec2f uvTransformScale(0.0f, 0.0f);

  bool textureProcessed = false;

  // Get all inputs (primvar reader, transform, etc.)
  std::vector<UsdShadeShader> deps = TraverseShaderInputs(usdUvTexture);
  deps.push_back(UsdShadeShader(usdUvTexture.GetPrim()));

  // Map for handling various shaders
  std::unordered_map<std::string, std::function<void(const UsdShadeShader&)>> shaderProcessors = {
    {"UsdPrimvarReader_float2", [&](const UsdShadeShader& d) { ProcessUvChannel(d); }},
    {"UsdTransform2d", [&](const UsdShadeShader& d) { transformOffsetAuthored = Process2DTransform(d, uvTransformOffset, uvTransformRotation, uvTransformScale); }},
    {"UsdUVTexture", [&](const UsdShadeShader& d) { textureProcessed = ProcessTextureAttributes(usdUvTexture, d, materialDefinition, imageMetaDataMap, semantic); }}};

  // Iterate over shader dependencies and process the shader IDs
  for(const auto& d : deps)
  {
    TfToken tokenId;
    if(d.GetShaderId(&tokenId))
    {
      std::string depsId = tokenId.GetString();

      // Use the shader ID to find the appropriate processor and call it
      if(shaderProcessors.find(depsId) != shaderProcessors.end())
      {
        shaderProcessors[depsId](d);
      }
    }
  }

  if(transformOffsetAuthored)
  {
    // @TODO: Process texture transform (similar to KHR_texture_transform) in the future.
  }

  return textureProcessed; // Return false if texture conversion fails
}

void UsdTextureConverter::ProcessUvChannel(const UsdShadeShader& shader)
{
  TfToken       tokenVarName("varname");
  UsdShadeInput shaderInput = shader.GetInput(tokenVarName);
  if(shaderInput)
  {
    std::string uvMapName;
    shaderInput.Get<std::string>(&uvMapName);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "UV Map Name: %s", uvMapName.c_str());
  }
}

bool UsdTextureConverter::Process2DTransform(const UsdShadeShader& shader, GfVec2f& uvTransformOffset, float& uvTransformRotation, GfVec2f& uvTransformScale)
{
  // Map for processing different 2D transform attributes (translation, scale, rotation)
  std::unordered_map<std::string, std::function<void(const UsdShadeInput&)>> transformProcessors = {
    {"translation", [&](const UsdShadeInput& input) { ProcessTransformTranslation(input, uvTransformOffset); }},
    {"scale", [&](const UsdShadeInput& input) { ProcessTransformScale(input, uvTransformScale); }},
    {"rotation", [&](const UsdShadeInput& input) { ProcessTransformRotation(input, uvTransformRotation); }}};

  bool transformOffsetAuthored = false;

  // Process each input for the transform
  for(auto input : shader.GetInputs())
  {
    std::string baseName = input.GetBaseName().GetString();
    if(transformProcessors.find(baseName) != transformProcessors.end())
    {
      TfToken       token(baseName);
      UsdShadeInput shaderInput = UsdShadeShader(shader).GetInput(token);

      if(shaderInput)
      {
        transformProcessors[baseName](shaderInput);
        transformOffsetAuthored = true;
      }
    }
  }

  return transformOffsetAuthored; // Return true if any transform offsets are authored
}

void UsdTextureConverter::ProcessTransformTranslation(const UsdShadeInput& input, GfVec2f& uvTransformOffset)
{
  input.Get<GfVec2f>(&uvTransformOffset);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "uvTransformOffset: %.7f, %.7f, ", uvTransformOffset[0], uvTransformOffset[1]);
}

void UsdTextureConverter::ProcessTransformScale(const UsdShadeInput& input, GfVec2f& uvTransformScale)
{
  input.Get<GfVec2f>(&uvTransformScale);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "uvTransformScale: %.7f, %.7f, ", uvTransformScale[0], uvTransformScale[1]);
}

void UsdTextureConverter::ProcessTransformRotation(const UsdShadeInput& input, float& uvTransformRotation)
{
  input.Get<float>(&uvTransformRotation);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "UV Transform Rotation: %.7f", uvTransformRotation);
}

bool UsdTextureConverter::ProcessTextureAttributes(const UsdShadeShader& usdUvTexture, const UsdShadeShader& shader, MaterialDefinition& materialDefinition, const ImageMetadataMap& imageMetaDataMap, uint32_t semantic)
{
  std::string    imagePath;
  UsdAssetBuffer imageBuffer;

  // Map for processing texture attributes (file, wrapS, wrapT, scale, bias, etc.)
  std::unordered_map<std::string, std::function<void(const UsdShadeInput&)>> textureProcessors = {
    {"file", [&](const UsdShadeInput& input) { ProcessTextureFile(input, imagePath, imageBuffer); }},
    {"wrapS", [&](const UsdShadeInput& input) { ProcessTextureWrap(input); }},
    {"wrapT", [&](const UsdShadeInput& input) { ProcessTextureWrap(input); }},
    {"scale", [&](const UsdShadeInput& input) { ProcessTextureScale(input); }},
    {"bias", [&](const UsdShadeInput& input) { ProcessTextureBias(input); }},
    {"st", [&](const UsdShadeInput& input) { ProcessTextureST(input); }},
    {"fallback", [&](const UsdShadeInput& input) { ProcessTextureFallback(input); }}};

  // Process each input in the USD UV texture shader
  std::vector<UsdShadeInput> inputs = usdUvTexture.GetInputs();
  for(auto input : inputs)
  {
    std::string baseName = input.GetBaseName().GetString();
    if(textureProcessors.find(baseName) != textureProcessors.end())
    {
      textureProcessors[baseName](input);
    }
  }

  // Process the image buffer or path after extracting the texture attributes
  return ProcessImageBuffer(materialDefinition, semantic, imagePath, imageBuffer, imageMetaDataMap);
}

void UsdTextureConverter::ProcessTextureFile(const UsdShadeInput& input, std::string& imagePath, UsdAssetBuffer& imageBuffer)
{
  SdfAssetPath fileInput;
  input.Get<SdfAssetPath>(&fileInput);

  std::string resolvedAssetPath = fileInput.GetResolvedPath();
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "File: %s", resolvedAssetPath.c_str());

  imagePath = ConvertImagePath(resolvedAssetPath);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Converted File Path: %s", imagePath.c_str());

  // Load the texture image data as a buffer
  imageBuffer = LoadAssetFileAsBuffer(resolvedAssetPath);
}

void UsdTextureConverter::ProcessTextureWrap(const UsdShadeInput& input)
{
  TfToken wrap;
  input.Get<TfToken>(&wrap);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "%s: %s, ", input.GetBaseName().GetText(), wrap.GetText());
}

void UsdTextureConverter::ProcessTextureScale(const UsdShadeInput& input)
{
  GfVec4f scale;
  input.Get<GfVec4f>(&scale);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "scale: %.7f, %.7f, %.7f, %.7f, ", scale[0], scale[1], scale[2], scale[3]);
}

void UsdTextureConverter::ProcessTextureBias(const UsdShadeInput& input)
{
  GfVec4f bias;
  input.Get<GfVec4f>(&bias);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "bias: %.7f, %.7f, %.7f, %.7f, ", bias[0], bias[1], bias[2], bias[3]);
}

void UsdTextureConverter::ProcessTextureST(const UsdShadeInput& input)
{
  GfVec2f st;
  input.Get<GfVec2f>(&st);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "st: %.7f, %.7f, ", st[0], st[1]);
}

void UsdTextureConverter::ProcessTextureFallback(const UsdShadeInput& input)
{
  GfVec4f fallback;
  input.Get<GfVec4f>(&fallback);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "fallback: %.7f, %.7f, %.7f, %.7f, ", fallback[0], fallback[1], fallback[2], fallback[3]);
}

bool UsdTextureConverter::ProcessImageBuffer(MaterialDefinition& materialDefinition, uint32_t semantic, const std::string& imagePath, UsdAssetBuffer& imageBuffer, const ImageMetadataMap& imageMetaDataMap)
{
  std::string imageFileName = std::filesystem::path(imagePath).filename().string();

  ImageMetadata metaData;
  if(!imageFileName.empty())
  {
    if(auto search = imageMetaDataMap.find(imageFileName); search != imageMetaDataMap.end())
    {
      metaData = search->second;
    }
  }

  if(imageBuffer.size() > 0)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Image Buffer Processed: semantic: %u", semantic);

    // If the image buffer is valid, push it to the material definition as a texture stage
    materialDefinition.mTextureStages.push_back({semantic, TextureDefinition{std::move(imageBuffer), SamplerFlags::DEFAULT, metaData.mMinSize, metaData.mSamplingMode}});
    materialDefinition.mFlags |= semantic;
    return true;
  }
  else if(!imagePath.empty())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Image Path Processed: semantic: %u, imagePath: %s", semantic, imagePath.c_str());

    // If we have a valid image path, push it to the material definition
    materialDefinition.mTextureStages.push_back({semantic, TextureDefinition{std::move(imagePath), SamplerFlags::DEFAULT, metaData.mMinSize, metaData.mSamplingMode}});
    materialDefinition.mFlags |= semantic;
    return true;
  }

  // If neither image buffer nor image path is available, return false
  return false;
}
} // namespace Dali::Scene3D::Loader
