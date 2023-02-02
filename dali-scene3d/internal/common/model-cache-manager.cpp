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

// CLASS HEADER
#include <dali-scene3d/internal/common/model-cache-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/public-api/object/base-object.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>

namespace Dali::Scene3D::Internal
{
class ModelCacheManager::Impl : public Dali::BaseObject
{
public:
  /**
   * @brief Constructor
   */
  Impl()
  {
  }

  Dali::Scene3D::Loader::LoadResult GetModelLoadResult(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    return Dali::Scene3D::Loader::LoadResult{cache.resources, cache.scene, cache.metaData, cache.animationDefinitions, cache.amimationGroupDefinitions, cache.cameraParameters, cache.lights};
  }

  uint32_t GetModelCacheRefCount(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    return cache.refCount;
  }

  Dali::ConditionalWait& GetLoadSceneConditionalWaitInstance(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    return cache.loadSceneConditionalWait;
  }

  Dali::ConditionalWait& GetLoadRawResourceConditionalWaitInstance(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    return cache.loadRawResourceConditionalWait;
  }

  void ReferenceModelCache(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    cache.refCount++;
  }

  void UnreferenceModelCache(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    if(cache.refCount > 0)
    {
      cache.refCount--;
    }

    if(cache.refCount == 0)
    {
      mModelCache.erase(modelUri);
    }
  }

  bool IsSceneLoaded(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    return cache.isSceneLoaded;
  }

  void SetSceneLoaded(std::string modelUri, bool isSceneLoaded)
  {
    ModelCache& cache   = mModelCache[modelUri];
    cache.isSceneLoaded = isSceneLoaded;
  }

  bool IsSceneLoading(std::string modelUri)
  {
    ModelCache& cache = mModelCache[modelUri];
    return cache.isSceneLoading;
  }

  void SetSceneLoading(std::string modelUri, bool isSceneLoading)
  {
    ModelCache& cache    = mModelCache[modelUri];
    cache.isSceneLoading = isSceneLoading;
  }

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Impl()
  {
  }

private:
  struct ModelCache
  {
    Dali::Scene3D::Loader::ResourceBundle  resources{}; ///< The bundle to store resources in.
    Dali::Scene3D::Loader::SceneDefinition scene{};     ///< The scene definition to populate.
    Dali::Scene3D::Loader::SceneMetadata   metaData{};  ///< The metadata of the scene.

    std::vector<Dali::Scene3D::Loader::AnimationDefinition>      animationDefinitions{};      ///< The list of animation definitions, in lexicographical order of their names.
    std::vector<Dali::Scene3D::Loader::AnimationGroupDefinition> amimationGroupDefinitions{}; ///< The list of animation group definitions, in lexicographical order of their names.
    std::vector<Dali::Scene3D::Loader::CameraParameters>         cameraParameters{};          ///< The camera parameters that were loaded from the scene.
    std::vector<Dali::Scene3D::Loader::LightParameters>          lights{};                    ///< The light parameters that were loaded from the scene.

    uint32_t              refCount{0};                      ///< The reference count of this model cache.
    Dali::ConditionalWait loadSceneConditionalWait{};       ///< The conditionalWait instance used to synchronise the loading of the scene for the same model in different threads.
    Dali::ConditionalWait loadRawResourceConditionalWait{}; ///< The conditionalWait instance used to synchronise the loading of the shared raw resources for the same model in different threads.

    bool isSceneLoaded{false};  ///< Whether the scene of the model has been loaded.
    bool isSceneLoading{false}; ///< Whether the scene loading of the model is in progress.
  };

  using ModelResourceCache = std::unordered_map<std::string, ModelCache>;
  ModelResourceCache mModelCache;
};

ModelCacheManager::ModelCacheManager() = default;

ModelCacheManager::~ModelCacheManager() = default;

ModelCacheManager ModelCacheManager::Get()
{
  ModelCacheManager manager;

  // Check whether the ModelCacheManager is already created
  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(ModelCacheManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to ModelCacheManager
      manager = ModelCacheManager(dynamic_cast<ModelCacheManager::Impl*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the ModelCacheManager and register it as a singleton
      manager = ModelCacheManager(new ModelCacheManager::Impl());
      singletonService.Register(typeid(manager), manager);
    }
  }

  return manager;
}

ModelCacheManager::ModelCacheManager(ModelCacheManager::Impl* impl)
: BaseHandle(impl)
{
}

Dali::Scene3D::Loader::LoadResult ModelCacheManager::GetModelLoadResult(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  return impl.GetModelLoadResult(modelUri);
}

uint32_t ModelCacheManager::GetModelCacheRefCount(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  return impl.GetModelCacheRefCount(modelUri);
}

Dali::ConditionalWait& ModelCacheManager::GetLoadSceneConditionalWaitInstance(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  return impl.GetLoadSceneConditionalWaitInstance(modelUri);
}

Dali::ConditionalWait& ModelCacheManager::GetLoadRawResourceConditionalWaitInstance(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  return impl.GetLoadRawResourceConditionalWaitInstance(modelUri);
}

void ModelCacheManager::ReferenceModelCache(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  impl.ReferenceModelCache(modelUri);
}

void ModelCacheManager::UnreferenceModelCache(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  impl.UnreferenceModelCache(modelUri);
}

bool ModelCacheManager::IsSceneLoaded(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  return impl.IsSceneLoaded(modelUri);
}

void ModelCacheManager::SetSceneLoaded(std::string modelUri, bool isSceneLoaded)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  impl.SetSceneLoaded(modelUri, isSceneLoaded);
}

bool ModelCacheManager::IsSceneLoading(std::string modelUri)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  return impl.IsSceneLoading(modelUri);
}

void ModelCacheManager::SetSceneLoading(std::string modelUri, bool isSceneLoading)
{
  ModelCacheManager::Impl& impl = static_cast<ModelCacheManager::Impl&>(GetBaseObject());
  impl.SetSceneLoading(modelUri, isSceneLoading);
}

} // namespace Dali::Scene3D::Internal
