/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/model-loader-impl.h>
#include <dali-scene3d/public-api/loader/model-loader.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dlfcn.h>
#include <filesystem>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/dli-loader-impl.h>
#include <dali-scene3d/internal/loader/glb-loader-impl.h>
#include <dali-scene3d/internal/loader/gltf2-loader-impl.h>

namespace Dali::Scene3D::Loader
{
namespace
{
constexpr std::string_view OBJ_EXTENSION      = ".obj";
constexpr std::string_view GLTF_EXTENSION     = ".gltf";
constexpr std::string_view GLB_EXTENSION      = ".glb";
constexpr std::string_view DLI_EXTENSION      = ".dli";
constexpr std::string_view USD_EXTENSION      = ".usd";
constexpr std::string_view USDZ_EXTENSION     = ".usdz";
constexpr std::string_view USDA_EXTENSION     = ".usda";
constexpr std::string_view USDC_EXTENSION     = ".usdc";
constexpr std::string_view METADATA_EXTENSION = "metadata";

const char* USD_LOADER_SO("libdali2-usd-loader.so");
const char* CREATE_USD_LOADER_SYMBOL("CreateUsdLoader");

// Custom deleter for dlopen handles
void DlcloseDeleter(void* handle)
{
  if(handle)
  {
    dlclose(handle);
  }
}

// Static shared pointer to store a pointer to the dlopen handle
std::shared_ptr<void*> gUsdLoaderHandle(nullptr, DlcloseDeleter);

using CreateUsdLoaderFunc = ModelLoaderImpl* (*)();
CreateUsdLoaderFunc gCreateUsdLoaderFunc(nullptr);

// Poxy function for `dlopen` to allow easy overriding in test environments.
extern "C" void* DlopenProxy(const char* filename, int flag)
{
  // Calls the real dlopen
  return dlopen(filename, flag);
}

// Poxy function for `dlsym` to allow easy overriding in test environments.
extern "C" void* DlsymProxy(void* handle, const char* name)
{
  // Calls the real dlsym
  return dlsym(handle, name);
}

} // namespace

ModelLoader::ModelLoader(const std::string& modelUrl, const std::string& resourceDirectoryUrl, Dali::Scene3D::Loader::LoadResult& loadResult)
: mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mLoadResult(loadResult)
{
  CreateModelLoader();
}

ModelLoader::~ModelLoader()
{
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
    mImpl = std::make_unique<Dali::Scene3D::Loader::Internal::DliLoaderImpl>();
  }
  else if(extension == GLTF_EXTENSION)
  {
    mImpl = std::make_unique<Dali::Scene3D::Loader::Internal::Gltf2LoaderImpl>();
  }
  else if(extension == GLB_EXTENSION)
  {
    mImpl = std::make_unique<Dali::Scene3D::Loader::Internal::GlbLoaderImpl>();
  }
  else if(extension == USD_EXTENSION || extension == USDZ_EXTENSION || extension == USDA_EXTENSION || extension == USDC_EXTENSION)
  {
    // Attempt to load the USD loader library dynamically
    // Once loaded we will keep it open so that any subsequent loading of USD models
    // doesn't require loading the same library repeatedly.
    if(!gUsdLoaderHandle || !(*gUsdLoaderHandle))
    {
      void* handle = DlopenProxy(USD_LOADER_SO, RTLD_LAZY);

      if(!handle)
      {
        // The shared library failed to load
        DALI_LOG_ERROR("ModelLoader::CreateModelLoader, dlopen error: %s\n", dlerror());
        return;
      }

      // Store the handle in the shared_ptr (passing the handle and the custom deleter)
      gUsdLoaderHandle = std::shared_ptr<void*>(new void*(handle), [](void* ptr) {
        if(ptr)
        {
          DlcloseDeleter(*(static_cast<void**>(ptr))); // Call custom deleter
          delete static_cast<void**>(ptr);             // Clean up dynamically allocated memory
        }
      });

      // Dynamically link to the CreateUsdLoader function in the shared library at runtime.
      // Cache the function pointer only if it hasn't been loaded yet
      if(!gCreateUsdLoaderFunc)
      {
        gCreateUsdLoaderFunc = reinterpret_cast<CreateUsdLoaderFunc>(DlsymProxy(*gUsdLoaderHandle.get(), CREATE_USD_LOADER_SYMBOL));
        if(!gCreateUsdLoaderFunc)
        {
          // If the symbol couldn't be found, reset the shared_ptr to invoke the custom deleter
          gUsdLoaderHandle.reset(); // This will automatically call dlclose via the custom deleter

          DALI_LOG_ERROR("Cannot find CreateUsdLoader function: %s\n", dlerror());

          return;
        }
      }
    }

    // Create an instance of USD loader
    mImpl = ModelLoaderImplUniquePtr(gCreateUsdLoaderFunc());
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

  Dali::Scene3D::Loader::ResourceRefCounts resourceRefCount = mLoadResult.mResources.CreateRefCounter();
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
} // namespace Dali::Scene3D::Loader
