#ifndef DALI_SCENE3D_LOADER_SHADER_DEFINITION_FACTORY_H_
#define DALI_SCENE3D_LOADER_SHADER_DEFINITION_FACTORY_H_
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
#include "dali-scene3d/public-api/loader/index.h"

// EXTERNAL INCLUDER
#include <memory>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
struct NodeDefinition;
class ResourceBundle;

class DALI_SCENE3D_API ShaderDefinitionFactory
{
public:
  ShaderDefinitionFactory();
  ~ShaderDefinitionFactory();

  /*
   * @brief Input for meshes and materials, output for shaders.
   */
  void SetResources(ResourceBundle& resources);

  /*
   * @brief Produces the index of a shader, which should be used to index into the shaders
   *  vector of the ResourceBundle which was provided for the factory. This shader will be
   *  created if one with the given settings hasn't been created by the factory yet (shaders
   *  already existing in the ResourceBundle are ignored), otherwise the index of the previously
   *  created shader will be returned.
   */
  Index ProduceShader(const NodeDefinition& nodeDef);

private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif //DALI_SCENE3D_LOADER_SHADER_DEFINITION_FACTORY_H_
