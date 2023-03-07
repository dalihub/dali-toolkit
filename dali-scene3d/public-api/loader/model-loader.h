#ifndef DALI_SCENE3D_LOADER_MODEL_LOADER_H
#define DALI_SCENE3D_LOADER_MODEL_LOADER_H
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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>

// EXTERNAL INCLUDES
#include <string>

namespace Dali::Scene3D::Loader
{
namespace Internal
{
class ModelLoaderImpl;
}

class DALI_SCENE3D_API ModelLoader
{
public:
  class InputParameter
  {
  };

  /**
   * @brief ModelLoader Constructor.
   * @param[in] modelUrl Url of the model file to be loaded
   * @param[in] resourceDirectoryUrl Url of directory that contains resources.
   * @param[out] loadResult Loaded result that includes scene tree and resources.
   */
  ModelLoader(const std::string& modelUrl, const std::string& resourceDirectoryUrl, Dali::Scene3D::Loader::LoadResult& loadResult);

  /**
   * @brief Request to load model from model url.
   * @param[in] pathProvider Path provider that defines resource paths.
   * @param[in] loadOnlyRawResource If true, load Raw resource only, and do not create Dali::Handle
   * If false, this loader load Raw resource and create Dali::Handle too.
   * Default value is false;
   * @return True if model loading is successfully finished.
   */
  bool LoadModel(Dali::Scene3D::Loader::ResourceBundle::PathProvider& pathProvider, bool loadOnlyRawResource = false);

  /**
   * @brief Set InputParameter.
   * Thie method store only a pointer of InputParameter.
   * The object of InputParameter should not be deleted until it is no longer used.
   * @param[in] inputParameter Input parameters that can be used in loading time.
   */
  void SetInputParameter(InputParameter& inputParameter);

  /**
   * @brief Retrieves loaded scene
   * @return SceneDefinition that is loaded from file
   */
  Dali::Scene3D::Loader::SceneDefinition& GetScene();

  /**
   * @brief Retrieves resource bundle that includes resource information
   * @return ResourceBundle for model resources
   */
  Dali::Scene3D::Loader::ResourceBundle& GetResources();

  /**
   * @brief Retrieves loaded AnimationDefinition
   * @return AnimationDefinition that is loaded from file
   */
  std::vector<Dali::Scene3D::Loader::AnimationDefinition>& GetAnimations();

  /**
   * @brief Retrieves loaded CameraParameters
   * @return CameraParameters list that is loaded from file
   */
  std::vector<Dali::Scene3D::Loader::CameraParameters>& GetCameras();

  /**
   * @brief Retrieves ResourceChoices
   * @return Choices for loaded Resources
   */
  Dali::Scene3D::Loader::Customization::Choices& GetResourceChoices();

private:
  /**
   * @brief Create model loader for each file format.
   */
  void CreateModelLoader();

  /**
   * @brief Load resource of the model.
   */
  void LoadResource(Dali::Scene3D::Loader::ResourceBundle::PathProvider& pathProvider, bool loadOnlyRawResource);

private:
  std::string mModelUrl;
  std::string mResourceDirectoryUrl;

  Dali::Scene3D::Loader::LoadResult             mLoadResult;
  Dali::Scene3D::Loader::Customization::Choices mResourceChoices;

  std::shared_ptr<Internal::ModelLoaderImpl> mImpl;
};
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_MODEL_LOADER_H
