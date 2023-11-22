#ifndef DALI_SCENE3D_LOADER_SHADER_OPTION_H_
#define DALI_SCENE3D_LOADER_SHADER_OPTION_H_
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
#include <string>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
class DALI_SCENE3D_API ShaderOption
{
public:
  enum class Type
  {
    GLTF_CHANNELS = 0,          // 00001
    THREE_TEXTURE,              // 00002
    BASE_COLOR_TEXTURE,         // 00004
    METALLIC_ROUGHNESS_TEXTURE, // 00008
    NORMAL_TEXTURE,             // 00010
    OCCLUSION,                  // 00020
    EMISSIVE,                   // 00040
    ALPHA_TEST,                 // 00080
    SUBSURFACE,                 // 00100
    SPECULAR,                   // 00200
    SPECULAR_COLOR,             // 00400
    SKINNING,                   // 00800
    FLIP_UVS_VERTICAL,          // 01000
    COLOR_ATTRIBUTE,            // 02000
    VEC4_TANGENT,               // 04000
    MORPH_POSITION,             // 08000
    MORPH_NORMAL,               // 10000
    MORPH_TANGENT,              // 20000
    MORPH_VERSION_2_0           // 40000
  };

  struct MacroDefinition
  {
    std::string macro;
    std::string definition;
  };

  ShaderOption() = default;
  ShaderOption(const ShaderOption& rhs);
  ShaderOption& operator=(const ShaderOption& rhs);

  using HashType = uint64_t;

public:
  /**
   * @brief Sets transparency option.
   * @SINCE_2_2.33
   */
  void SetTransparency();

  /**
   * @brief Adds new shader definition option.
   *
   * If the option is already added, nothin is changed.
   * @SINCE_2_2.33
   * @param[in] shaderOptionType Option to be added,
   */
  void AddOption(Type shaderOptionType);

  /**
   * @brief Adds macro definitions for joints based on the number of joint sets.
   * @SINCE_2_2.52
   */
  void AddJointMacros(size_t numberOfJointSets);

  /**
   * @brief Enables empty preprocessor definitions to be defined to a value.
   * @SINCE_2_2.52
   */
  void AddMacroDefinition(std::string macro, std::string definition);

  /**
   * @brief Retrieves current shader option hash
   *
   * @SINCE_2_2.33
   * @return Hash value of currently added options.
   */
  HashType GetOptionHash() const;

  /**
   * @brief Retrieves a list of define keywords.
   *
   * @SINCE_2_2.33
   * @param[out] defines A list of define keywords those are used in this option.
   */
  void GetDefines(std::vector<std::string>& defines) const;

  /**
   * @brief Retrieves a single shader define keyword of input type.
   *
   * @SINCE_2_2.33
   * @param[in] shaderOptionType Shader definition option type to know its keyword.
   * @return string keyword of shader define.
   */
  static std::string_view GetDefineKeyword(Type shaderOptionType);

  /**
   * @brief Get any macro definitions.
   * @SINCE_2_2.52
   */
  const std::vector<MacroDefinition>& GetMacroDefinitions() const;

private:
  HashType mOptionHash{0u};

  std::vector<MacroDefinition> mMacros;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_SHADER_OPTION_H_
