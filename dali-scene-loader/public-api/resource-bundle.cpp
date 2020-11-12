/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/resource-bundle.h"

// EXTERNAL
#include "dali/public-api/rendering/sampler.h"
#include "dali-toolkit/public-api/image-loader/sync-image-loader.h"
#include <fstream>
#include <istream>
#include <cstring>
#include <fstream>

namespace Dali
{
using namespace Toolkit;

namespace SceneLoader
{
namespace
{

const char* const RESOURCE_TYPE_NAMES[] = {
  "Environment",
  "Shader",
  "Mesh",
  "Material",
};

}  // nonamespace

const char* GetResourceTypeName(ResourceType::Value type)
{
  return RESOURCE_TYPE_NAMES[static_cast<int>(type)];
}

ResourceRefCounts ResourceBundle::CreateRefCounter() const
{
  ResourceRefCounts refCounts(4);
  refCounts[ResourceType::Environment].Resize(mEnvironmentMaps.size(), 0u);
  refCounts[ResourceType::Shader].Resize(mShaders.size(), 0u);
  refCounts[ResourceType::Mesh].Resize(mMeshes.size(), 0u);
  refCounts[ResourceType::Material].Resize(mMaterials.size(), 0u);
  return refCounts;
}

void ResourceBundle::CountEnvironmentReferences(ResourceRefCounts& refCounts) const
{
  auto& environmentRefCounts = refCounts[ResourceType::Environment];

  const auto& materialRefs = refCounts[ResourceType::Material];
  for (uint32_t i = 0, iEnd = materialRefs.Size(); i != iEnd; ++i)
  {
    if (materialRefs[i] > 0)
    {
      ++environmentRefCounts[mMaterials[i].first.mEnvironmentIdx];
    }
  }
}

void ResourceBundle::LoadResources(const ResourceRefCounts& refCounts, PathProvider pathProvider, Options::Type options)
{
  const auto kForceLoad = MaskMatch(options, Options::ForceReload);
  const auto kKeepUnused = MaskMatch(options, Options::KeepUnused);

  const auto& refCountEnvMaps = refCounts[ResourceType::Environment];
  auto environmentsPath = pathProvider(ResourceType::Environment);
  for (uint32_t i = 0, iEnd = refCountEnvMaps.Size(); i != iEnd; ++i)
  {
    auto refCount = refCountEnvMaps[i];
    auto& iEnvMap = mEnvironmentMaps[i];
    if (refCount > 0 && (kForceLoad || !iEnvMap.second.IsLoaded()))
    {
      auto raw = iEnvMap.first.LoadRaw(environmentsPath);
      iEnvMap.second = iEnvMap.first.Load(std::move(raw));
    }
    else if (!kKeepUnused && refCount == 0 && iEnvMap.second.IsLoaded())
    {
      iEnvMap.second.mDiffuse = Texture();
      iEnvMap.second.mSpecular = Texture();
    }
  }

  const auto& refCountShaders = refCounts[ResourceType::Shader];
  auto shadersPath = pathProvider(ResourceType::Shader);
  for (uint32_t i = 0, iEnd = refCountShaders.Size(); i != iEnd; ++i)
  {
    auto refCount = refCountShaders[i];
    auto& iShader = mShaders[i];
    if (refCount > 0 && (kForceLoad || !iShader.second))
    {
      auto raw = iShader.first.LoadRaw(shadersPath);
      iShader.second = iShader.first.Load(std::move(raw));
    }
    else if(!kKeepUnused && refCount == 0 && iShader.second)
    {
      iShader.second = Shader();
    }
  }

  const auto& refCountMeshes = refCounts[ResourceType::Mesh];
  auto modelsPath = pathProvider(ResourceType::Mesh);
  for (uint32_t i = 0, iEnd = refCountMeshes.Size(); i != iEnd; ++i)
  {
    auto refCount = refCountMeshes[i];
    auto& iMesh = mMeshes[i];
    if (refCount > 0 && (kForceLoad || !iMesh.second.geometry))
    {
      auto raw = iMesh.first.LoadRaw(modelsPath);
      iMesh.second = iMesh.first.Load(std::move(raw));
    }
    else if (!kKeepUnused && refCount == 0 && iMesh.second.geometry)
    {
      iMesh.second.geometry = Geometry();
    }
  }

  const auto& refCountMaterials = refCounts[ResourceType::Material];
  auto imagesPath = pathProvider(ResourceType::Material);
  for (uint32_t i = 0, iEnd = refCountMaterials.Size(); i != iEnd; ++i)
  {
    auto refCount = refCountMaterials[i];
    auto& iMaterial = mMaterials[i];
    if (refCount > 0 && (kForceLoad || !iMaterial.second))
    {
      auto raw = iMaterial.first.LoadRaw(imagesPath);
      iMaterial.second = iMaterial.first.Load(mEnvironmentMaps, std::move(raw));
    }
    else if (!kKeepUnused && refCount == 0 && iMaterial.second)
    {
      iMaterial.second = TextureSet();
    }
  }
}

}
}
