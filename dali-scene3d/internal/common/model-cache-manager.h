#ifndef DALI_SCENE3D_MODEL_CACHE_MANAGER_H
#define DALI_SCENE3D_MODEL_CACHE_MANAGER_H

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
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/load-result.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
class ModelCacheManager;

/**
 * A singleton class to manage the cache of 3D models so that the resources of the same model
 * are only loaded once and kept in the cache. The cached resources will be reused when the
 * same model is loaded multiple times.
 */
class ModelCacheManager : public Dali::BaseHandle
{
public:
  /**
   * @brief Creates a ModelCacheManager handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  ModelCacheManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ModelCacheManager();

  /**
   * @brief Create or retrieve the ModelCacheManager singleton.
   *
   * @return A handle to the ModelCacheManager.
   */
  static ModelCacheManager Get();

  /**
   * @brief Retrieves the load result for the model with the given URI.
   * If there is no existing load result for the given model, a new one will be created.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @return A reference to the model's load result.
   */
  Dali::Scene3D::Loader::LoadResult GetModelLoadResult(std::string modelUri);

  /**
   * @brief Retrieves the reference count of the cache for the model with the given URI.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @return The reference count of the cache.
   */
  uint32_t GetModelCacheRefCount(std::string modelUri);

  /**
   * @brief Retrieves the ConditionalWait object to synchronize the scene loading of the model
   * with the given URI between multiple threads.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @return The ConditionalWait object.
   */
  Dali::ConditionalWait& GetLoadSceneConditionalWaitInstance(std::string modelUri);

  /**
   * @brief Reference the cache of the model with the given URI.
   * This will increment the reference count of the load result by 1.
   * @param[in] modelUri The model URI.
   */
  void ReferenceModelCache(std::string modelUri);

  /**
   * @brief Unreference the cache of the model with the given URI.
   * This will decrement the reference count of the load result by 1.
   * When the reference count becomes zero, the model will be removed from the cache and all
   * its resources will be deleted.
   * @param[in] modelUri The model URI.
   */
  void UnreferenceModelCache(std::string modelUri);

  /**
   * @brief Retrieves whether the scene of the model with the given URI has been loaded.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @return whether the scene of the model has been loaded. This will be true if the scene
   * has been loaded for once.
   */
  bool IsSceneLoaded(std::string modelUri);

  /**
   * @brief Sets whether the scene of the model with the given URI has been loaded.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @param[in] isSceneLoaded Whether the scene of the model has been loaded.
   */
  void SetSceneLoaded(std::string modelUri, bool isSceneLoaded);

  /**
   * @brief Retrieves whether the scene loading of the model with the given URI is in progress.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @return whether the scene loading of the model is in progress.
   */
  bool IsSceneLoading(std::string modelUri);

  /**
   * @brief Sets whether the scene loading of the model with the given URI is in progress.
   * @param[in] modelUri The unique model URI with its absolute path.
   * @param[in] isSceneLoading Whether the scene loading of the model is in progress.
   */
  void SetSceneLoading(std::string modelUri, bool isSceneLoading);

public:
  // Default copy and move operator
  ModelCacheManager(const ModelCacheManager& rhs) = default;
  ModelCacheManager(ModelCacheManager&& rhs)      = default;
  ModelCacheManager& operator=(const ModelCacheManager& rhs) = default;
  ModelCacheManager& operator=(ModelCacheManager&& rhs) = default;

private:
  class Impl;
  explicit DALI_INTERNAL ModelCacheManager(ModelCacheManager::Impl* impl);
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_CACHE_MANAGER_H
