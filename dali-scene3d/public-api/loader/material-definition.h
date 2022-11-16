#ifndef DALI_SCENE3D_LOADER_MATERIAL_DEFINITION_H
#define DALI_SCENE3D_LOADER_MATERIAL_DEFINITION_H
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include "dali-scene3d/public-api/api.h"
#include "dali-scene3d/public-api/loader/environment-definition.h"
#include "dali-scene3d/public-api/loader/index.h"
#include "dali-scene3d/public-api/loader/utils.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h>
#include <cmath>
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/public-api/math/vector4.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/**
 * @brief Helper enum for encoding and decoding sampler states.
 */
struct DALI_SCENE3D_API SamplerFlags
{
  using Type = uint8_t;

  enum Values : Type
  {
    // Filter - 3 bits
    FILTER_NEAREST        = 0,
    FILTER_LINEAR         = NthBit(0),
    FILTER_MIPMAP_NEAREST = NthBit(1),
    FILTER_MIPMAP_LINEAR  = NthBit(2),

    // Wrap - 2 bits
    WRAP_REPEAT = 0,
    WRAP_CLAMP  = NthBit(0),
    WRAP_MIRROR = NthBit(1),

    // Layout - apply shift, then mask
    FILTER_MIN_BITS = 3,
    FILTER_MIN_MASK = NthBit(FILTER_MIN_BITS) - 1,

    FILTER_MAG_BITS  = 1,
    FILTER_MAG_SHIFT = FILTER_MIN_BITS,
    FILTER_MAG_MASK  = NthBit(FILTER_MAG_BITS) - 1,

    WRAP_S_BITS  = 2,
    WRAP_S_SHIFT = FILTER_MAG_SHIFT + FILTER_MAG_BITS,
    WRAP_S_MASK  = NthBit(WRAP_S_BITS) - 1,

    WRAP_T_BITS  = 2,
    WRAP_T_SHIFT = WRAP_S_SHIFT + WRAP_S_BITS,
    WRAP_T_MASK  = NthBit(WRAP_T_BITS) - 1,

    // Diagnostics
    MIPMAP_MASK = FILTER_MIPMAP_LINEAR | FILTER_MIPMAP_NEAREST,

    // Default
    DEFAULT = FILTER_LINEAR | (FILTER_LINEAR << FILTER_MAG_SHIFT) | (WRAP_REPEAT << WRAP_S_SHIFT) | (WRAP_REPEAT << WRAP_T_SHIFT), // LINEAR filters, REPEAT wraps
  };

  /**
   * @return SamplerFlags bit pattern calculated from the given Dali Sampler settings.
   */
  static Type Encode(FilterMode::Type minFilter, FilterMode::Type magFilter, WrapMode::Type wrapS, WrapMode::Type wrapT);

  /**
   * @brief Decodes the minification filter patter of @a flags into the corresponding FilterMode.
   */
  static FilterMode::Type GetMinFilter(Type flags);

  /**
   * @brief Decodes the magnification filter patter of @a flags into the corresponding FilterMode.
   */
  static FilterMode::Type GetMagFilter(Type flags);

  /**
   * @brief Decodes the horizontal wrap pattern of @a flags into the corresponding WrapMode.
   */
  static WrapMode::Type GetWrapS(Type flags);

  /**
   * @brief Decodes the vertical wrap pattern of @a flags into the corresponding WrapMode.
   */
  static WrapMode::Type GetWrapT(Type flags);

  /**
   * @brief Creates a Sampler with the settings encoded in @a flags.
   */
  static Sampler MakeSampler(Type flags);
};

/**
 * @brief Defines a texture from a combination of an image URI and its sampler definition.
 */
struct DALI_SCENE3D_API TextureDefinition
{
  std::string        mImageUri;
  SamplerFlags::Type mSamplerFlags;
  ImageDimensions    mMinImageDimensions;
  SamplingMode::Type mSamplingMode;

  TextureDefinition(const std::string& imageUri = "", SamplerFlags::Type samplerFlags = SamplerFlags::DEFAULT, ImageDimensions minImageDimensions = ImageDimensions(), SamplingMode::Type samplingMode = SamplingMode::BOX_THEN_LINEAR);
};

/**
 * @brief Defines a material with a number of texture stages, whether mipmappping
 *  is enabled, and an index of an environment (usually of all environments in a
 *  scene). Textures from the environment are added last when the DALi TextureSet
 *  is being created.
 */
struct DALI_SCENE3D_API MaterialDefinition
{
  enum Flags : uint32_t
  {
    // Texture semantics
    ALBEDO         = NthBit(0),
    METALLIC       = NthBit(1),
    ROUGHNESS      = NthBit(2),
    NORMAL         = NthBit(3),
    EMISSIVE       = NthBit(4),
    OCCLUSION      = NthBit(5),
    SPECULAR       = NthBit(6),
    SPECULAR_COLOR = NthBit(7),
    SUBSURFACE     = NthBit(8), // Note: dli-only

    // Other binary options
    TRANSPARENCY  = NthBit(20),
    GLTF_CHANNELS = NthBit(21), // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#pbrmetallicroughnessmetallicroughnesstexture

    // Alpha cutoff - reserved from the 24th bit
    ALPHA_CUTOFF_BITS  = 8,
    ALPHA_CUTOFF_SHIFT = sizeof(uint32_t) * 8 - ALPHA_CUTOFF_BITS,
    ALPHA_CUTOFF_MASK  = (1 << ALPHA_CUTOFF_BITS) - 1,
  };

  /**
   * @brief A(n image based) texture that's used in a material.
   */
  struct TextureStage
  {
    uint32_t          mSemantic;
    TextureDefinition mTexture;
  };

  using Vector = std::vector<std::pair<MaterialDefinition, TextureSet>>;

  struct RawData
  {
    struct TextureData
    {
      PixelData          mPixels;
      SamplerFlags::Type mSamplerFlags;
    };

    std::vector<TextureData> mTextures;
  };

  MaterialDefinition() = default;

  MaterialDefinition(const MaterialDefinition&) = delete;
  MaterialDefinition& operator=(const MaterialDefinition&) = delete;

  MaterialDefinition(MaterialDefinition&&) = default;
  MaterialDefinition& operator=(MaterialDefinition&&) = default;

  /**
   * @brief Loads (or, in the case of solid color materials, creates) raw pixel data,
   *  which is then returned.
   * @note This may be called from any thread.
   */
  RawData LoadRaw(const std::string& imagesPath) const;

  /**
   * @brief Creates Textures from the pixel data in @a raw, gets the
   *  the cube maps from the iEnvironment'th element of @a environments,
   *  then creates a DALi TextureSet and returns it.
   * @note This must be called from the event thread.
   * @note The textures are added in the following order: 2D, cube maps.
   */
  TextureSet Load(const EnvironmentDefinition::Vector& environments, RawData&& raw) const;

  /**
   * @brief Checks if the given mask matches any of the textures defined.
   */
  bool CheckTextures(uint32_t flags) const;

  /**
   * @return The alpha test reference value.
   * @note A value of 0.f means no alpha testing.
   */
  float GetAlphaCutoff() const
  {
    return ((mFlags >> ALPHA_CUTOFF_SHIFT) & ALPHA_CUTOFF_MASK) / 255.f;
  }

  /**
   * @brief Encodes the alpha test reference @a value in flags.
   * @note A value of 0.f means no alpha testing.
   */
  void SetAlphaCutoff(float value)
  {
    DALI_ASSERT_DEBUG(value >= 0.f && value <= 1.f);
    mFlags |= static_cast<uint8_t>(std::round(value * 255.f)) << ALPHA_CUTOFF_SHIFT;
  }

public: // DATA
  uint32_t mFlags = 0x0;

  Index   mEnvironmentIdx      = 0;
  Vector4 mColor               = Color::WHITE;
  float   mMetallic            = 1.f;
  float   mRoughness           = 1.f;
  Vector4 mBaseColorFactor     = Vector4::ONE;
  float   mNormalScale         = 1.f;
  float   mOcclusionStrength   = 1.f;
  Vector3 mEmissiveFactor      = Vector3::ZERO;
  float   mDielectricSpecular  = 0.04f;
  float   mSpecularFactor      = 1.0f;
  Vector3 mSpecularColorFactor = Vector3::ONE;

  // For the glTF, each of albedo, metallicRoughness, normal textures are not essential.
  bool mNeedAlbedoTexture            = true;
  bool mNeedMetallicRoughnessTexture = true;
  bool mNeedNormalTexture            = true;
  bool mDoubleSided                  = false;

  std::vector<TextureStage> mTextureStages;
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif //DALI_SCENE3D_LOADER_MATERIAL_DEFINITION_H
