#ifndef DALI_SCENE3D_LOADERERERER_RESOURCE_BUNDLE_H_
#define DALI_SCENE3D_LOADERERERER_RESOURCE_BUNDLE_H_
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/texture-set.h>
#include <functional>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/buffer-definition.h>
#include <dali-scene3d/public-api/loader/environment-definition.h>
#include <dali-scene3d/public-api/loader/material-definition.h>
#include <dali-scene3d/public-api/loader/mesh-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition.h>
#include <dali-scene3d/public-api/loader/skeleton-definition.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief The types of resources that .dli may define.
 * @SINCE_2_0.7
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

/**
 * @brief The string value corresponding to the given resource @a type.
 * @SINCE_2_0.7
 * @return The string value for type.
 */
DALI_SCENE3D_API const char* GetResourceTypeName(ResourceType::Value type);

using ResourceRefCounts = std::vector<Vector<uint32_t>>;

/**
 * @brief Stores all resource definitions along with the DALi resources that
 *  could be created from them, directly indexible into with values from a dli
 *  document.
 * @SINCE_2_0.7
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

  ResourceBundle();

  ResourceBundle(const ResourceBundle&) = delete;
  ResourceBundle& operator=(const ResourceBundle&) = delete;

  ResourceBundle(ResourceBundle&&) = default;
  ResourceBundle& operator=(ResourceBundle&&) = default;

  /**
   * @brief A ResourceRefCounts object with the correct number of entries for
   *  all resource types (based on the various resource definition vectors),
   *  with all reference counts set to 0.
   * @SINCE_2_0.7
   * @return A ResourceRefCounts object.
   */
  ResourceRefCounts CreateRefCounter() const;

  /**
   * @brief Based on a ResourceRefCounts, and more specifically the reference
   *  count of materials therein, it will calculate the reference count of
   *  environment maps.
   * @SINCE_2_0.7
   */
  void CountEnvironmentReferences();

  /**
   * @brief Performs the loading of all resources based on their respective reference count in @a refCounts.
   *
   * Resources that had a non-zero ref count will be loaded unless we already have a handle to them
   * (OR the ForceReload option was specified).
   * Any handles we have to resources that come in with a zero ref count will be reset,
   * UNLESS the KeepUnused option was specified.
   *
   * @SINCE_2_0.7
   * @param[in] pathProvider path provider for resource data.
   * @param[in] options Option to load resource
   * @note This method creates DALi objects like Dali::Texture, Dali::Geometry, etc.
   */
  void LoadResources(PathProvider  pathProvider,
                     Options::Type options = Options::None);

  /**
   * @brief Loads of all resources based on their respective reference count in @a refCounts.
   *
   * Resources that had a non-zero ref count will be loaded unless we already have a handle to them
   * (OR the ForceReload option was specified).
   * Any handles we have to resources that come in with a zero ref count will be reset,
   * UNLESS the KeepUnused option was specified.
   *
   * @SINCE_2_2.9
   * @note This method don't create any of DALi objects.
   * @param[in] pathProvider path provider for resource data.
   * @param[in] options Option to load resource
   * @note This method only loads raw data from resource file, and
   * doesn't create any of DALi objects. GenerateResources() method is required to be called
   * after this method to create DALi objects.
   */
  void LoadRawResources(PathProvider  pathProvider,
                        Options::Type options = Options::None);

  /**
   * @brief Generates DALi objects from already loaded Raw Resources.
   * @SINCE_2_2.9
   * @param[in] options Option to load resource
   * @note This method generates DALi objects from raw data that is already
   * loaded by LoadRawResources method. Therefore, LoadRawResources should be called first
   * before this method is called.
   */
  void GenerateResources(Options::Type options = Options::None);

public: // DATA
  ResourceRefCounts             mReferenceCounts;
  EnvironmentDefinition::Vector mEnvironmentMaps;
  ShaderDefinition::Vector      mShaders;
  MeshDefinition::Vector        mMeshes;
  MaterialDefinition::Vector    mMaterials;

  SkeletonDefinition::Vector mSkeletons;
  BufferDefinition::Vector   mBuffers;

  bool mRawResourcesLoading;
  bool mResourcesGenerating;

  bool mRawResourcesLoaded;
  bool mResourcesGenerated;
};

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADERERERER_RESOURCE_BUNDLE_H_
