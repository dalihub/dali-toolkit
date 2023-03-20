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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/gltf2-asset.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition-factory.h>

namespace gt = gltf2;
namespace js = json;

namespace Dali::Scene3D::Loader::Internal
{
namespace Gltf2Util
{
struct NodeMapping
{
  Index gltfIndex;
  Index runtimeIndex;

  bool operator<(Index gltfIndex) const
  {
    return this->gltfIndex < gltfIndex;
  }
};

class NodeIndexMapper
{
public:
  NodeIndexMapper()                       = default;
  NodeIndexMapper(const NodeIndexMapper&) = delete;
  NodeIndexMapper& operator=(const NodeIndexMapper&) = delete;

  ///@brief Registers a mapping of the @a gltfIndex of a node to its @a runtimeIndex .
  ///@note If the indices are the same, the registration is omitted, in order to
  /// save growing a vector.
  void RegisterMapping(Index gltfIndex, Index runtimeIndex)
  {
    if(gltfIndex != runtimeIndex)
    {
      auto iInsert = std::lower_bound(mNodes.begin(), mNodes.end(), gltfIndex);
      DALI_ASSERT_DEBUG(iInsert == mNodes.end() || iInsert->gltfIndex != gltfIndex);
      mNodes.insert(iInsert, NodeMapping{gltfIndex, runtimeIndex});
    }
  }

  ///@brief Retrieves the runtime index of a Node, mapped to the given @a gltfIndex.
  Index GetRuntimeId(Index gltfIndex) const
  {
    auto iFind = std::lower_bound(mNodes.begin(), mNodes.end(), gltfIndex); // using custom operator<
    return (iFind != mNodes.end() && iFind->gltfIndex == gltfIndex) ? iFind->runtimeIndex : gltfIndex;
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

void ConvertBuffers(const gt::Document& document, ConversionContext& context);

void ConvertMaterials(const gt::Document& document, ConversionContext& context);

void ConvertMeshes(const gt::Document& document, ConversionContext& context);

void ConvertCamera(const gt::Camera& camera, CameraParameters& cameraParameters);

void ConvertNodes(const gt::Document& document, ConversionContext& context, bool isMRendererModel);

void ConvertAnimations(const gt::Document& document, ConversionContext& context);

void ProcessSkins(const gt::Document& document, ConversionContext& context);

void ProduceShaders(ShaderDefinitionFactory& shaderFactory, SceneDefinition& scene);

void SetDefaultEnvironmentMap(const gt::Document& document, ConversionContext& context);

const std::string_view GetRendererModelIdentification();

void ReadDocument(const json_object_s& jsonObject, gt::Document& document);

void InitializeGltfLoader();

void ReadDocumentFromParsedData(const json_object_s& jsonObject, gltf2::Document& document);

bool GenerateDocument(json::unique_ptr& root, gt::Document& document, bool& isMRendererModel);

void ConvertGltfToContext(gt::Document& document, Gltf2Util::ConversionContext& context, bool isMRendererModel);

} // namespace Gltf2Util

} // namespace Dali::Scene3D::Loader::Internal

#endif // DALI_SCENE3D_LOADER_GLTF2_UTIL_H
