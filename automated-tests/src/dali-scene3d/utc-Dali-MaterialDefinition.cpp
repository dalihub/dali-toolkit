/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/string-utils.h>
#include <fstream>
#include "dali-scene3d/integration-api/loader/material-definition.h"

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
const std::string RESOURCE_PATH = TEST_RESOURCE_DIR "/";

// Images of known, differing sizes.
const std::string IMAGE_64x64   = "broken.png";
const std::string IMAGE_100x100 = "mask.png";

/**
 * @brief Reads a whole file into a buffer, the way a model loader hands embedded images over.
 */
Dali::Vector<uint8_t> ReadFile(const std::string& filename)
{
  Dali::Vector<uint8_t> buffer;

  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if(file.is_open())
  {
    const auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    buffer.Resize(static_cast<uint32_t>(fileSize));
    file.read(reinterpret_cast<char*>(buffer.Begin()), fileSize);
  }

  return buffer;
}

/**
 * @brief Creates a texture stage which carries the encoded image in its buffer.
 */
MaterialDefinition::TextureStage MakeBufferStage(uint32_t semantic, const std::string& filename)
{
  return {semantic, TextureDefinition{ReadFile(RESOURCE_PATH + filename)}};
}

/**
 * @brief Creates a texture stage which refers to an image by its url.
 */
MaterialDefinition::TextureStage MakeUrlStage(uint32_t semantic, const std::string& filename)
{
  return {semantic, TextureDefinition{Integration::ToDaliString(filename)}};
}

/**
 * @brief Creates a material which only loads what its texture stages define.
 */
MaterialDefinition MakeMaterialDefinition(uint32_t flags, std::initializer_list<MaterialDefinition::TextureStage> stages)
{
  MaterialDefinition materialDefinition;
  materialDefinition.mFlags                        = flags;
  materialDefinition.mNeedAlbedoTexture            = false;
  materialDefinition.mNeedMetallicRoughnessTexture = false;
  materialDefinition.mNeedNormalTexture            = false;

  for(auto& stage : stages)
  {
    materialDefinition.mTextureStages.PushBack(stage);
  }

  return materialDefinition;
}

} // namespace

int UtcDaliMaterialDefinitionLoadRawCombineMetallicRoughness(void)
{
  ToolkitTestApplication application;
  tet_infoline("Combine separate metallic and roughness textures of the same size.");

  MaterialDefinition materialDefinition = MakeMaterialDefinition(
    MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS,
    {MakeBufferStage(MaterialDefinition::METALLIC, IMAGE_64x64),
     MakeBufferStage(MaterialDefinition::ROUGHNESS, IMAGE_64x64)});

  MaterialDefinition::RawData raw = materialDefinition.LoadRaw(Integration::ToDaliString(RESOURCE_PATH));

  // The two stages are replaced with a single combined one.
  DALI_TEST_EQUALS(raw.mTextures.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mSemantic, static_cast<uint32_t>(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS), TEST_LOCATION);

  PixelData combined = raw.mTextures[0].mPixels;
  DALI_TEST_CHECK(combined);
  DALI_TEST_EQUALS(combined.GetWidth(), 64u, TEST_LOCATION);
  DALI_TEST_EQUALS(combined.GetHeight(), 64u, TEST_LOCATION);
  DALI_TEST_EQUALS(combined.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  // The stage keeps the combined buffer, RGBA8888 sized.
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mTexture.mTextureBuffer.Size(), 64u * 64u * 4u, TEST_LOCATION);

  // Roughness goes into the G channel and metallic into the B channel, so R and A are cleared.
  Integration::PixelDataBuffer pixelDataBuffer = Integration::GetPixelDataBuffer(combined);
  DALI_TEST_CHECK(pixelDataBuffer.buffer != nullptr);
  DALI_TEST_EQUALS(static_cast<uint32_t>(pixelDataBuffer.buffer[0]), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(pixelDataBuffer.buffer[3]), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialDefinitionLoadRawCombineMetallicRoughnessDifferentSize(void)
{
  ToolkitTestApplication application;
  tet_infoline("Combine separate metallic and roughness textures, resizing the metallic one to match.");

  MaterialDefinition materialDefinition = MakeMaterialDefinition(
    MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS,
    {MakeBufferStage(MaterialDefinition::METALLIC, IMAGE_100x100),
     MakeBufferStage(MaterialDefinition::ROUGHNESS, IMAGE_64x64)});

  MaterialDefinition::RawData raw = materialDefinition.LoadRaw(Integration::ToDaliString(RESOURCE_PATH));

  DALI_TEST_EQUALS(raw.mTextures.Count(), 1u, TEST_LOCATION);

  // The combined texture takes the size of the roughness texture.
  PixelData combined = raw.mTextures[0].mPixels;
  DALI_TEST_CHECK(combined);
  DALI_TEST_EQUALS(combined.GetWidth(), 64u, TEST_LOCATION);
  DALI_TEST_EQUALS(combined.GetHeight(), 64u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mTexture.mTextureBuffer.Size(), 64u * 64u * 4u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialDefinitionLoadRawMetallicOnly(void)
{
  ToolkitTestApplication application;
  tet_infoline("A metallic texture without a roughness one is used as it is.");

  MaterialDefinition materialDefinition = MakeMaterialDefinition(
    MaterialDefinition::METALLIC,
    {MakeBufferStage(MaterialDefinition::METALLIC, IMAGE_100x100)});

  MaterialDefinition::RawData raw = materialDefinition.LoadRaw(Integration::ToDaliString(RESOURCE_PATH));

  DALI_TEST_EQUALS(raw.mTextures.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mSemantic, static_cast<uint32_t>(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS), TEST_LOCATION);

  PixelData metallic = raw.mTextures[0].mPixels;
  DALI_TEST_CHECK(metallic);
  DALI_TEST_EQUALS(metallic.GetWidth(), 100u, TEST_LOCATION);
  DALI_TEST_EQUALS(metallic.GetHeight(), 100u, TEST_LOCATION);

  const uint32_t bytesPerPixel = Pixel::GetBytesPerPixel(metallic.GetPixelFormat());
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mTexture.mTextureBuffer.Size(), 100u * 100u * bytesPerPixel, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialDefinitionLoadRawRoughnessOnly(void)
{
  ToolkitTestApplication application;
  tet_infoline("A roughness texture without a metallic one is used as it is.");

  MaterialDefinition materialDefinition = MakeMaterialDefinition(
    MaterialDefinition::ROUGHNESS,
    {MakeBufferStage(MaterialDefinition::ROUGHNESS, IMAGE_64x64)});

  MaterialDefinition::RawData raw = materialDefinition.LoadRaw(Integration::ToDaliString(RESOURCE_PATH));

  DALI_TEST_EQUALS(raw.mTextures.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mSemantic, static_cast<uint32_t>(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS), TEST_LOCATION);

  PixelData roughness = raw.mTextures[0].mPixels;
  DALI_TEST_CHECK(roughness);
  DALI_TEST_EQUALS(roughness.GetWidth(), 64u, TEST_LOCATION);
  DALI_TEST_EQUALS(roughness.GetHeight(), 64u, TEST_LOCATION);

  const uint32_t bytesPerPixel = Pixel::GetBytesPerPixel(roughness.GetPixelFormat());
  DALI_TEST_EQUALS(materialDefinition.mTextureStages[0].mTexture.mTextureBuffer.Size(), 64u * 64u * bytesPerPixel, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialDefinitionLoadRawMetallicRoughnessEmptyBufferN(void)
{
  ToolkitTestApplication application;
  tet_infoline("Metallic and roughness stages with no image buffer produce no texture.");

  MaterialDefinition materialDefinition = MakeMaterialDefinition(
    MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS,
    {{MaterialDefinition::METALLIC, TextureDefinition{}},
     {MaterialDefinition::ROUGHNESS, TextureDefinition{}}});

  MaterialDefinition::RawData raw = materialDefinition.LoadRaw(Integration::ToDaliString(RESOURCE_PATH));

  DALI_TEST_EQUALS(raw.mTextures.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(materialDefinition.mTextureStages.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialDefinitionLoadRawExtraTextures(void)
{
  ToolkitTestApplication application;
  tet_infoline("Load the extra texture stages - subsurface, occlusion, emissive, specular and specular colour.");

  MaterialDefinition materialDefinition = MakeMaterialDefinition(
    MaterialDefinition::ALBEDO | MaterialDefinition::SUBSURFACE | MaterialDefinition::OCCLUSION |
      MaterialDefinition::EMISSIVE | MaterialDefinition::SPECULAR | MaterialDefinition::SPECULAR_COLOR,
    {MakeUrlStage(MaterialDefinition::ALBEDO, IMAGE_64x64),
     MakeUrlStage(MaterialDefinition::SUBSURFACE, IMAGE_64x64),
     MakeUrlStage(MaterialDefinition::OCCLUSION, IMAGE_64x64),
     MakeUrlStage(MaterialDefinition::EMISSIVE, IMAGE_64x64),
     MakeUrlStage(MaterialDefinition::SPECULAR, IMAGE_64x64),
     MakeUrlStage(MaterialDefinition::SPECULAR_COLOR, IMAGE_100x100)});

  MaterialDefinition::RawData raw = materialDefinition.LoadRaw(Integration::ToDaliString(RESOURCE_PATH));

  // Albedo, then one texture per extra stage, in stage order.
  DALI_TEST_EQUALS(raw.mTextures.Count(), 6u, TEST_LOCATION);
  for(auto& texture : raw.mTextures)
  {
    DALI_TEST_CHECK(texture.mPixels);
  }
  DALI_TEST_EQUALS(raw.mTextures[5].mPixels.GetWidth(), 100u, TEST_LOCATION);

  END_TEST;
}
