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

// FILE HEADER
#include <dali-scene3d/public-api/loader/model-loader.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <filesystem>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/dli-loader-impl.h>
#include <dali-scene3d/internal/loader/gltf2-loader-impl.h>
#include <dali-scene3d/internal/loader/model-loader-impl.h>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
static constexpr std::string_view OBJ_EXTENSION      = ".obj";
static constexpr std::string_view GLTF_EXTENSION     = ".gltf";
static constexpr std::string_view DLI_EXTENSION      = ".dli";
static constexpr std::string_view METADATA_EXTENSION = "metadata";
} // namespace

ModelLoader::ModelLoader(const std::string& modelUrl, const std::string& resourceDirectoryUrl, Dali::Scene3D::Loader::LoadResult& loadResult)
: mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mLoadResult(loadResult)
{
  CreateModelLoader();
}

bool ModelLoader::LoadModel(Dali::Scene3D::Loader::ResourceBundle::PathProvider& pathProvider, bool loadOnlyRawResource)
{
  if(!mImpl)
  {
    return false;
  }

  bool loadSucceeded = false;

  mLoadResult.mAnimationDefinitions.clear();
  std::filesystem::path metaDataUrl(mModelUrl);
  metaDataUrl.replace_extension(METADATA_EXTENSION.data());

  Dali::Scene3D::Loader::LoadSceneMetadata(metaDataUrl.c_str(), mLoadResult.mSceneMetadata);
  loadSucceeded = mImpl->LoadModel(mModelUrl, mLoadResult);
  LoadResource(pathProvider, loadOnlyRawResource);

  return loadSucceeded;
}

void ModelLoader::SetInputParameter(InputParameter& inputParameter)
{
  mImpl->SetInputParameter(inputParameter);
}

Dali::Scene3D::Loader::SceneDefinition& ModelLoader::GetScene()
{
  return mLoadResult.mScene;
}

Dali::Scene3D::Loader::ResourceBundle& ModelLoader::GetResources()
{
  return mLoadResult.mResources;
}

std::vector<Dali::Scene3D::Loader::AnimationDefinition>& ModelLoader::GetAnimations()
{
  return mLoadResult.mAnimationDefinitions;
}

std::vector<Dali::Scene3D::Loader::CameraParameters>& ModelLoader::GetCameras()
{
  return mLoadResult.mCameraParameters;
}

Dali::Scene3D::Loader::Customization::Choices& ModelLoader::GetResourceChoices()
{
  return mResourceChoices;
}

void ModelLoader::CreateModelLoader()
{
  std::filesystem::path modelPath(mModelUrl);
  if(mResourceDirectoryUrl.empty())
  {
    mResourceDirectoryUrl = std::string(modelPath.parent_path()) + "/";
  }
  std::string extension = modelPath.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  if(extension == DLI_EXTENSION)
  {
    mImpl = std::make_shared<Dali::Scene3D::Loader::Internal::DliLoaderImpl>();
  }
  else if(extension == GLTF_EXTENSION)
  {
    mImpl = std::make_shared<Dali::Scene3D::Loader::Internal::Gltf2LoaderImpl>();
  }
  else
  {
    DALI_LOG_ERROR("Not supported model format : %s\n", extension.c_str());
  }
}

void ModelLoader::LoadResource(Dali::Scene3D::Loader::ResourceBundle::PathProvider& pathProvider, bool loadOnlyRawResource)
{
  if(GetResources().mRawResourcesLoaded && loadOnlyRawResource)
  {
    return;
  }

  Dali::Scene3D::Loader::ResourceRefCounts resourceRefCount = std::move(mLoadResult.mResources.CreateRefCounter());
  for(auto iRoot : GetScene().GetRoots())
  {
    GetScene().CountResourceRefs(iRoot, mResourceChoices, resourceRefCount);
  }

  GetResources().mReferenceCounts = std::move(resourceRefCount);
  GetResources().CountEnvironmentReferences();

  if(loadOnlyRawResource)
  {
    GetResources().LoadRawResources(pathProvider);
  }
  else
  {
    GetResources().LoadResources(pathProvider);
  }
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
