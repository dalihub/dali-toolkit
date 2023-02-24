#ifndef DALI_SCENE3D_LOADER_GLTF2_UTIL_H
#define DALI_SCENE3D_LOADER_GLTF2_UTIL_H
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
#include <dali-scene3d/internal/loader/gltf2-asset.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition-factory.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>

namespace gt = gltf2;
namespace js = json;

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace Internal
{
namespace Gltf2Util
{

struct NodeMapping
{
  Index gltfIdx;
  Index runtimeIdx;

  bool operator<(Index gltfIdx) const
  {
    return this->gltfIdx < gltfIdx;
  }
};

class NodeIndexMapper
{
public:
  NodeIndexMapper()                                  = default;
  NodeIndexMapper(const NodeIndexMapper&)            = delete;
  NodeIndexMapper& operator=(const NodeIndexMapper&) = delete;

  ///@brief Registers a mapping of the @a gltfIdx of a node to its @a runtimeIdx .
  ///@note If the indices are the same, the registration is omitted, in order to
  /// save growing a vector.
  void RegisterMapping(Index gltfIdx, Index runtimeIdx)
  {
    if(gltfIdx != runtimeIdx)
    {
      auto iInsert = std::lower_bound(mNodes.begin(), mNodes.end(), gltfIdx);
      DALI_ASSERT_DEBUG(iInsert == mNodes.end() || iInsert->gltfIdx != gltfIdx);
      mNodes.insert(iInsert, NodeMapping{gltfIdx, runtimeIdx});
    }
  }

  ///@brief Retrieves the runtime index of a Node, mapped to the given @a gltfIdx.
  Index GetRuntimeId(Index gltfIdx) const
  {
    auto iFind = std::lower_bound(mNodes.begin(), mNodes.end(), gltfIdx); // using custom operator<
    return (iFind != mNodes.end() && iFind->gltfIdx == gltfIdx) ? iFind->runtimeIdx : gltfIdx;
  }

private:
  std::vector<NodeMapping> mNodes;
};

struct ConversionContext
{
  LoadResult& mOutput;

  std::string mPath;
  Index       mDefaultMaterial;

  std::vector<Index> mMeshIds;
  NodeIndexMapper    mNodeIndices;
};

void ConvertBuffers(const gt::Document& doc, ConversionContext& context);

void ConvertMaterials(const gt::Document& doc, ConversionContext& context);

void ConvertMeshes(const gt::Document& doc, ConversionContext& context);

void ConvertCamera(const gt::Camera& camera, CameraParameters& camParams);

void ConvertNodes(const gt::Document& doc, ConversionContext& context, bool isMRendererModel);

void ConvertAnimations(const gt::Document& doc, ConversionContext& context);

void ProcessSkins(const gt::Document& doc, ConversionContext& context);

void ProduceShaders(ShaderDefinitionFactory& shaderFactory, SceneDefinition& scene);

void SetDefaultEnvironmentMap(const gt::Document& doc, ConversionContext& context);

const std::string_view GetRendererModelIdentification();

void ReadDocument(const json_object_s& jsonObject, gt::Document& document);

void InitializeGltfLoader();

void ReadDocumentFromParsedData(const json_object_s& jsonObject, gltf2::Document& document);

bool GenerateDocument(json::unique_ptr& root, gt::Document& document, bool& isMRendererModel);

void ConvertGltfToContext(gt::Document& document, Gltf2Util::ConversionContext& context, bool isMRendererModel);

} // namespace Gltf2Util

} // namespace Internal
} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_GLTF2_UTIL_H
