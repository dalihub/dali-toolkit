#ifndef DALI_SCENE3D_LOADERERERERER_RESOURCE_BUNDLE_H_
#define DALI_SCENE3D_LOADERERERERER_RESOURCE_BUNDLE_H_
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

// INTERNAL
#include "dali-scene3d/public-api/loader/environment-definition.h"
#include "dali-scene3d/public-api/loader/material-definition.h"
#include "dali-scene3d/public-api/loader/mesh-definition.h"
#include "dali-scene3d/public-api/loader/shader-definition.h"
#include "dali-scene3d/public-api/loader/skeleton-definition.h"

// EXTERNAL
#include <functional>
#include <memory>
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/public-api/rendering/shader.h"
#include "dali/public-api/rendering/texture-set.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/*
 * @brief The types of resources that .dli may define.
 */
struct DALI_SCENE3D_API ResourceType
{
  enum Value
  {
    Environment,
    Shader,
    Mesh,
    Material,
  };

  ResourceType() = delete;
};

/*
 * @return The string value corresponding to the given resource @a type.
 */
DALI_SCENE3D_API const char* GetResourceTypeName(ResourceType::Value type);

using ResourceRefCounts = std::vector<Vector<uint32_t>>;

/*
 * @brief Stores all resource definitions along with the DALi resources that
 *  could be created from them, directly indexible into with values from a dli
 *  document.
 */
class DALI_SCENE3D_API ResourceBundle
{
public:
  struct Options
  {
    using Type = uint8_t;

    enum Value : Type
    {
      None        = 0,
      ForceReload = NthBit(0), ///< Load resources [again] even if they were already loaded.
      KeepUnused  = NthBit(1)  ///<s Don't reset handles to resources that had a 0 reference count.
    };
  };

  using PathProvider = std::function<std::string(ResourceType::Value)>;

  ResourceBundle() = default;

  ResourceBundle(const ResourceBundle&) = delete;
  ResourceBundle& operator=(const ResourceBundle&) = delete;

  ResourceBundle(ResourceBundle&&) = default;
  ResourceBundle& operator=(ResourceBundle&&) = default;

  /*
   * @return A ResourceRefCounts object with the correct number of entries for
   *  all resource types (based on the various resource definition vectors),
   *  with all reference counts set to 0.
   */
  ResourceRefCounts CreateRefCounter() const;

  /*
   * @brief Based on a ResourceRefCounts, and more specifically the reference
   *  count of materials therein, it will calculate the reference count of
   *  environment maps.
   */
  void CountEnvironmentReferences(ResourceRefCounts& refCounts) const;

  /*
   * @brief Performs the loading of all resources based on their respective
   *  reference count in @a refCounts. Resources that had a non-zero ref count will be
   *  loaded unless we already have a handle to them (OR the ForceReload option was specified).
   *  Any handles we have to resources that come in with a zero ref count will be reset,
   *  UNLESS the KeepUnused option was specified.
   */
  void LoadResources(const ResourceRefCounts& refCounts,
                     PathProvider             pathProvider,
                     Options::Type            options = Options::None);

public: // DATA
  EnvironmentDefinition::Vector mEnvironmentMaps;
  ShaderDefinition::Vector      mShaders;
  MeshDefinition::Vector        mMeshes;
  MaterialDefinition::Vector    mMaterials;

  SkeletonDefinition::Vector mSkeletons;
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif //DALI_SCENE3D_LOADERERERERER_RESOURCE_BUNDLE_H_
