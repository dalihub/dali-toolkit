#ifndef DALI_SCENE3D_LOADER_SHADER_DEFINITION_OPTION_H_
#define DALI_SCENE3D_LOADER_SHADER_DEFINITION_OPTION_H_
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

// EXTERNAL INCLUDER
#include <dali/public-api/common/vector-wrapper.h>
#include <memory>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{

class DALI_SCENE3D_API ShaderDefinitionOption
{
public:
  enum class Type
  {
    GLTF_CHANNELS = 0,
    THREE_TEXTURE,
    BASE_COLOR_TEXTURE,
    METALLIC_ROUGHNESS_TEXTURE,
    NORMAL_TEXTURE,
    OCCLUSION,
    EMISSIVE,
    ALPHA_TEST,
    SUBSURFACE,
    SPECULAR,
    SPECULAR_COLOR,
    SKINNING,
    FLIP_UVS_VERTICAL,
    COLOR_ATTRIBUTE,
    VEC4_TANGENT,
    MORPH_POSITION,
    MORPH_NORMAL,
    MORPH_TANGENT,
    MORPH_VERSION_2_0
  };

public:
  /**
   * @brief Sets transparency option.
   */
  void SetTransparency();

  /**
   * @brief Adds new shader definition option.
   * If the option is already added, nothin is changed.
   *
   * @param[in] shaderDefinitionOptionType Option to be added,
   */
  void AddOption(Type shaderDefinitionOptionType);

  /**
   * @brief Retrieves current shader option hash
   *
   * @return Hash value of currently added options.
   */
  uint64_t GetOptionHash() const;

  /**
   * @brief Retrieves a list of define keywords.
   *
   * @param[out] defines A list of define keywords those are used in this option.
   */
  void GetDefines(std::vector<std::string>& defines) const;

  /**
   * @brief Retrieves a single shader define keyword of input type.
   *
   * @param[in] shaderDefinitionOptionType Shader definition option type to know its keyword.
   * @return string keyword of shader define.
   */
  static std::string_view GetDefineKeyword(Type shaderDefinitionOptionType);

private:
  uint64_t mOptionHash{0u};
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_SHADER_DEFINITION_OPTION_H_
