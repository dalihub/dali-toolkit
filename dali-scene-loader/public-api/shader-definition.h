#ifndef DALI_SCENE_LOADER_SHADER_DEFINITION_H
#define DALI_SCENE_LOADER_SHADER_DEFINITION_H
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

// INTERNAL INCLUDES
#include "dali-scene-loader/public-api/renderer-state.h"

// EXTERNAL INCLUDES
#include <memory>
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/public-api/rendering/shader.h"

namespace Dali
{
namespace SceneLoader
{
/*
 * @brief Defines a shader with paths to the files which define its
 *  vertex and fragment components, and a mapping of uniform names (which are
 *  used to refer to them in GLSL) to data.
 */
struct DALI_SCENE_LOADER_API ShaderDefinition
{
  using Vector = std::vector<std::pair<ShaderDefinition, Shader>>;

  struct RawData
  {
    std::string mVertexShaderSource;
    std::string mFragmentShaderSource;
  };

  /*
   * @brief Apply the defines values to shader.
   */
  static void ApplyDefine(std::string& shaderCode, const std::string& definevar);

  ShaderDefinition() = default;

  ShaderDefinition(const ShaderDefinition& other);
  ShaderDefinition& operator=(const ShaderDefinition& other);

  ShaderDefinition(ShaderDefinition&&) = default;
  ShaderDefinition& operator=(ShaderDefinition&&) = default;

  /*
   * @brief Attempts to load the source of the vertex and fragment shaders,
   *  then performs pre-processing of defines.
   * @note This may be called from any thread.
   */
  RawData LoadRaw(const std::string& shadersPath) const;

  /*
   * @brief Creates a DALi Shader from the sources in @a raw, traverses
   *  uniforms to get them to register their data against their name,
   *  then returns the Shader.
   * @note This must be called from the event thread.
   */
  Shader Load(RawData&& raw) const;

public: // DATA
  RendererState::Type mRendererState = RendererState::NONE;

  std::string              mVertexShaderPath;
  std::string              mFragmentShaderPath;
  std::vector<std::string> mDefines;
  std::vector<std::string> mHints;
  Property::Map            mUniforms;
  bool                     mUseBuiltInShader;
};

} // namespace SceneLoader
} // namespace Dali

#endif //DALI_SCENE_LOADER_SHADER_DEFINITION_H
