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
#include <dali-usd-loader/internal/usd-loader-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// Some TBB/USD headers produce compile warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdSkel/animation.h>
#include <pxr/usd/usdSkel/bindingAPI.h>
#include <pxr/usd/usdSkel/root.h>
#include <pxr/usd/usdSkel/skeleton.h>
#pragma GCC diagnostic pop

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/utils.h>
#include <dali-usd-loader/internal/usd-material-processor.h>
#include <dali-usd-loader/internal/utils.h>

using namespace Dali;
using namespace pxr;
using namespace Dali::Scene3D::Loader;

namespace Dali::Scene3D::Loader
{
namespace
{
const Vector3 CAMERA_DEFAULT_POSITION(0.0f, 0.0f, 3.5f);

#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_USD_LOADER");
#endif

// Utility function to print a specific number of indentation levels
void PrintLevel(int level)
{
  for(int i = 0; i < level; i++)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  ");
  }
}
} // namespace

struct UsdLoaderImpl::Impl
{
public:
  /**
   * @brief Traverses materials in the USD scene and populate the output.
   * @param[in, out] output The load result.
   */
  void TraverseMaterials(LoadResult& output);

  /**
   * @brief Traverses prims in the USD scene and populate the output.
   * @param[in, out] output The load result.
   * @param[in] prim The current USD prim being traversed.
   * @param[in] parentIndex The index of the parent node in the hierarchy.
   * @param[in] level The level of nesting in the hierarchy.
   */
  void TraversePrims(LoadResult& output, const UsdPrim& prim, Index parentIndex, int level);

private:
  /**
   * @brief Converts a mesh prim to the internal representation.
   * @param[in, out] output The load result.
   * @param[in] prim The USD prim representing the mesh.
   * @param[in, out] nodeIndex The index of the current node.
   * @param[in] parentIndex The index of the parent node.
   */
  void ConvertMesh(LoadResult& output, const UsdPrim& prim, Index& nodeIndex, Index parentIndex);

  /**
   * @brief Converts a node prim to the internal representation.
   * @param[in, out] output The load result.
   * @param[in] prim The USD prim representing the node.
   * @param[in, out] nodeIndex The index of the current node.
   * @param[in] parentIndex The index of the parent node.
   */
  void ConvertNode(LoadResult& output, const UsdPrim& prim, Index& nodeIndex, Index parentIndex);

  /**
   * @brief Converts a camera prim to the internal representation.
   * @param[in, out] output The load result.
   * @param[in] prim The USD prim representing the camera.
   */
  void ConvertCamera(LoadResult& output, const UsdPrim& prim);

  /**
   * @brief Extracts the transformation (position, rotation, scale) of a given USD primitive.
   *
   * This function retrieves the local transformation matrix of a USD prim and decomposes it into
   * position, rotation, and scale components.
   *
   * @param[in] prim The USD primitive from which to extract the transformation.
   * @param[out] position The extracted position vector.
   * @param[out] rotation The extracted rotation quaternion.
   * @param[out] scale The extracted scale vector.
   * @param[in] time The time at which to sample the transformation.
   */
  void GetXformableTransformation(const UsdPrim& prim, Vector3& position, Quaternion& rotation, Vector3& scale, const UsdTimeCode time = UsdTimeCode::Default());

  /**
   * @brief Convert transform animations for a node to the internal representation.
   * @param[in, out] output The load result.
   * @param[in] prim The USD prim representing the node.
   * @param[in] nodeIndex The index of the node.
   */
  void ConvertTransformAnimation(LoadResult& output, const UsdPrim& prim, Index& nodeIndex);

  /**
   * @brief Adds a node to the scene graph and optionally sets its transformation.
   *
   * This function creates a new node based on a USD primitive and adds it to the scene graph.
   * Optionally, it can set the transformation of the node (position, rotation, scale).
   *
   * @param[in,out] scene The scene definition to which the node will be added.
   * @param[in] nodeName The name of the node to be added.
   * @param[in] parentIndex The index of the parent node in the scene graph.
   * @param[in] position The position of the node (if setting transformation).
   * @param[in] rotation The rotation of the node (if setting transformation).
   * @param[in] scale The scale of the node (if setting transformation).
   * @param[in] setTransformation Whether to apply the transformation to the node.
   * @return A pointer to the created node definition.
   */
  NodeDefinition* AddNodeToScene(SceneDefinition& scene, const std::string nodeName, const Index parentIndex, const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool setTransformation);

  /**
   * @brief Retrieves geometric primitive variables from a USD prim.
   *
   * This function extracts texture coordinates (texcoords), vertex colors, and tangent attributes
   * from a USD primitive and categorizes them into separate vectors.
   *
   * @param[in] prim The USD primitive from which to retrieve the primvars.
   * @param[out] texcoords A vector to store the retrieved texture coordinate primvars.
   * @param[out] colors A vector to store the retrieved color primvars.
   * @param[out] tangents A vector to store the retrieved tangent primvars.
   */
  void RetrieveGeomPrimvars(const UsdPrim& prim, std::vector<UsdGeomPrimvar>& texcoords, std::vector<UsdGeomPrimvar>& colors, std::vector<UsdGeomPrimvar>& tangents);

  /**
   * @brief Processes and stores mesh indices in the mesh definition.
   *
   * This function processes the triangulated face indices of a mesh, including handling subset indices,
   * and stores them in the mesh definition.
   *
   * @param[in,out] meshDefinition The mesh definition to store the processed indices.
   * @param[in] indexMap A map of the original to triangulated indices.
   * @param[in] subsetIdcs The indices belonging to the current subset.
   * @param[in] triangulatedIndex The triangulated indices for the entire mesh.
   * @param[out] subIndexArray A vector to store the processed subset indices.
   * @param[out] flattenedSubTriangulatedIndices A vector to store the flattened triangulated indices.
   */
  void ProcessMeshIndices(MeshDefinition& meshDefinition, std::map<int, VtArray<int>>& indexMap, VtIntArray& subsetIdcs, VtArray<int>& triangulatedIndex, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices);

  /**
   * @brief Processes and stores vertex positions in the mesh definition.
   *
   * This function processes the vertex positions based on the subset indices and stores them
   * in the mesh definition.
   *
   * @param[in,out] meshDefinition The mesh definition to store the processed vertex positions.
   * @param[in] points The original vertex positions.
   * @param[out] worldPosition A vector to store the processed world positions.
   * @param[in] subIndexArray The subset indices used to extract the relevant positions.
   */
  void ProcessMeshPositions(MeshDefinition& meshDefinition, const VtArray<GfVec3f>& points, VtArray<GfVec3f>& worldPosition, std::vector<uint32_t>& subIndexArray);

  /**
   * @brief Processes and stores vertex normals in the mesh definition.
   *
   * This function processes the vertex normals, handling both face-varying and vertex-based normals,
   * and stores them in the mesh definition.
   *
   * @param[in,out] meshDefinition The mesh definition to store the processed normals.
   * @param[in] usdMesh The USD mesh primitive.
   * @param[out] normals A vector to store the processed normals.
   * @param[in] subIndexArray The subset indices used to extract the relevant normals.
   * @param[in] flattenedSubTriangulatedIndices The flattened triangulated indices for face-varying normals.
   * @param[in] faceVertexCounts The number of vertices per face.
   * @param[in] isLeftHanded A flag indicating whether the coordinate system is left-handed.
   */
  void ProcessMeshNormals(MeshDefinition& meshDefinition, UsdGeomMesh& usdMesh, VtArray<GfVec3f>& normals, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices, VtArray<int>& faceVertexCounts, bool isLeftHanded);

  /**
   * @brief Generates normals for a mesh if none are provided.
   *
   * This function generates normals for a mesh by computing the cross product of adjacent
   * edges for each face. The generated normals are then stored in the mesh definition.
   *
   * @param[in,out] meshDefinition The mesh definition where the generated normals will be stored.
   */
  void GenerateNormal(MeshDefinition& meshDefinition);

  /**
   * @brief Processes and stores texture coordinates (UVs) in the mesh definition.
   *
   * This function processes the texture coordinates, handling both face-varying and vertex-based UVs,
   * and stores them in the mesh definition.
   *
   * @param[in,out] meshDefinition The mesh definition to store the processed texture coordinates.
   * @param[in] texcoords A vector of texture coordinate primvars to process.
   * @param[in] subIndexArray The subset indices used to extract the relevant UVs.
   * @param[in] flattenedSubTriangulatedIndices The flattened triangulated indices for face-varying UVs.
   * @param[in] faceVertexCounts The number of vertices per face.
   * @param[in] isLeftHanded A flag indicating whether the coordinate system is left-handed.
   */
  void ProcessMeshTexcoords(MeshDefinition& meshDefinition, std::vector<UsdGeomPrimvar>& texcoords, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices, VtArray<int>& faceVertexCounts, bool isLeftHanded);

  /**
   * @brief Generates tangent vectors for a mesh.
   *
   * This function generates tangent vectors for a mesh based on its texture coordinates (UVs)
   * and stores them in the mesh definition.
   *
   * @param[in,out] meshDefinition The mesh definition where the generated tangents will be stored.
   * @param[in] texcoords A vector of texture coordinate primvars to assist in tangent generation.
   */
  void GenerateTangents(MeshDefinition& meshDefinition, std::vector<UsdGeomPrimvar>& texcoords);

  /**
   * @brief Processes and stores vertex colors in the mesh definition.
   *
   * This function processes the vertex colors, handling different interpolation types (constant, vertex, face-varying),
   * and stores them in the mesh definition. If no colors are provided, a default white color is assigned.
   *
   * @param[in,out] meshDefinition The mesh definition to store the processed vertex colors.
   * @param[in] colors A vector of color primvars to process.
   * @param[in] worldPosition The vertex positions to match with colors.
   * @param[in] subIndexArray The subset indices used to extract the relevant colors.
   * @param[in] flattenedSubTriangulatedIndices The flattened triangulated indices for face-varying colors.
   * @param[in] faceVertexCounts The number of vertices per face.
   * @param[in] isLeftHanded A flag indicating whether the coordinate system is left-handed.
   */
  void ProcessMeshColors(MeshDefinition& meshDefinition, std::vector<UsdGeomPrimvar>& colors, VtArray<GfVec3f>& worldPosition, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices, VtArray<int>& faceVertexCounts, bool isLeftHanded);

  /**
   * @brief Processes and binds materials to a mesh subset within a USD prim.
   *
   * This function retrieves and assigns the appropriate material to a specific subset of a mesh
   * within the USD primitive. It updates the material ID used by the mesh subset in the output data.
   *
   * @param[in,out] output The load result where the material binding will be stored.
   * @param[in] prim The USD primitive containing the mesh and its subsets.
   * @param[in] subsets A vector of geometric subsets (parts of the mesh) within the USD primitive.
   * @param[in] subIndex The index of the subset within the mesh for which the material is being processed.
   * @param[out] meshSubMaterialId The material ID that is associated with the subset. It is updated with the correct ID after processing.
   */
  void ProcessMaterialBinding(LoadResult& output, const UsdPrim& prim, std::vector<UsdGeomSubset>& subsets, size_t subIndex, int& meshSubMaterialId);

public:
  UsdStageRefPtr mUsdStage; ///< Pointer to the USD stage.

  UsdMaterialProcessor::MaterialMap mMaterialMap; ///< Maps prim paths to material IDs.

  Index mNodeIndex; ///< Index of the current node being processed.
  int   mMeshCount; ///< Count of mesh objects encountered during traversal.

  Index mDefaultMaterial; ///< Index of the default material.
};

UsdLoaderImpl::UsdLoaderImpl()
: mImpl{new Impl}
{
}

UsdLoaderImpl::~UsdLoaderImpl() = default;

bool UsdLoaderImpl::LoadModel(const std::string& url, Dali::Scene3D::Loader::LoadResult& result)
{
  // Open the stage of the USD scene from the specified URL
  mImpl->mUsdStage = UsdStage::Open(url);
  if(!mImpl->mUsdStage)
  {
    DALI_LOG_ERROR("Failed to open %s\n", url.c_str());
    return false;
  }

  mImpl->mMeshCount       = 0;
  mImpl->mNodeIndex       = INVALID_INDEX;
  mImpl->mDefaultMaterial = INVALID_INDEX;

  // Traverse materials in the USD scene and populate the result
  mImpl->TraverseMaterials(result);

  // Get the index of the root node in the result scene
  Index rootIndex = result.mScene.GetNodeCount();

  // Create a node definition for the scene root
  std::unique_ptr<NodeDefinition> sceneRoot{new NodeDefinition()};
  sceneRoot->mName = "USD_SCENE_ROOT_NODE";

  // Add the scene root node to the result scene
  result.mScene.AddNode(std::move(sceneRoot));
  result.mScene.AddRootNode(rootIndex);

  // Traverse prims in the USD scene and populate the result
  UsdPrim rootPrim = mImpl->mUsdStage->GetPseudoRoot();
  mImpl->TraversePrims(result, rootPrim, rootIndex, 0);

  // Set default environment map
  EnvironmentDefinition environmentDefinition;
  environmentDefinition.mUseBrdfTexture = true;
  environmentDefinition.mIblIntensity   = Scene3D::Loader::EnvironmentDefinition::GetDefaultIntensity();
  result.mResources.mEnvironmentMaps.push_back({std::move(environmentDefinition), EnvironmentDefinition::Textures()});

  return true;
}

void UsdLoaderImpl::Impl::TraverseMaterials(LoadResult& output)
{
  auto& imageMetaData = output.mSceneMetadata.mImageMetadata;

  UsdMaterialProcessor materialProcessor(mUsdStage, &mMaterialMap, imageMetaData);
  materialProcessor.TraverseMaterials(output);
}

void UsdLoaderImpl::Impl::TraversePrims(LoadResult& output, const UsdPrim& prim, Index parentIndex, int level)
{
  PrintLevel(level);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "%s\n", prim.GetName().GetText());

  auto& scene = output.mScene;

  Index nodeIndex = scene.GetNodeCount() - 1;

  if(prim.IsA<UsdGeomMesh>())
  {
    ConvertMesh(output, prim, nodeIndex, parentIndex);
  }
  else if(prim.IsA<UsdGeomXformable>())
  {
    ConvertNode(output, prim, nodeIndex, parentIndex);
  }
  else if(prim.IsA<UsdSkelRoot>())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdSkelRoot\n");
  }
  else if(prim.IsA<UsdSkelSkeleton>())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdSkelSkeleton\n");
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "\n");
  }

  level++;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraversePrims: nodeIndex: %d, ", nodeIndex);

  // Recursively traverse child prims
  for(const UsdPrim& child : prim.GetChildren())
  {
    TraversePrims(output, child, nodeIndex, level);
  }

  level--;
}

void UsdLoaderImpl::Impl::GetXformableTransformation(const UsdPrim& prim, Vector3& position, Quaternion& rotation, Vector3& scale, const UsdTimeCode time)
{
  // Retrieve the local transformation matrix of the xformable prim
  auto       xformable = UsdGeomXformable(prim);
  GfMatrix4d result(1);
  bool       resetsXformStack;
  xformable.GetLocalTransformation(&result, &resetsXformStack, time);

  // Decompose the matrix into position, rotation, and scale components
  Matrix transformMatrix = ConvertUsdMatrix(result);
  transformMatrix.GetTransformComponents(position, rotation, scale);

  if(transformMatrix == Dali::Matrix::IDENTITY)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "IDENTITY, ");
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Position: %.7f, %.7f, %.7f, ", position.x, position.y, position.z);

    if(rotation == Quaternion::IDENTITY)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Rotation: IDENTITY, ");
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Rotation: %.7f, %.7f, %.7f, %.7f, ", rotation.AsVector().x, rotation.AsVector().y, rotation.AsVector().z, rotation.AsVector().w);
    }

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Scale: %.7f, %.7f, %.7f, ", scale.x, scale.y, scale.z);
  }
}

NodeDefinition* UsdLoaderImpl::Impl::AddNodeToScene(SceneDefinition& scene, const std::string nodeName, const Index parentIndex, const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool setTransformation)
{
  // Add the node to the scene graph
  auto weakNode = scene.AddNode([&]() {
        std::unique_ptr<NodeDefinition> nodeDefinition{new NodeDefinition()};

        nodeDefinition->mParentIdx = parentIndex;
        nodeDefinition->mName      = nodeName;
        if(nodeDefinition->mName.empty())
        {
          nodeDefinition->mName = std::to_string(reinterpret_cast<uintptr_t>(nodeDefinition.get()));
        }

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "scene.AddNode (ConvertNode): %s, parentIndex: %d\n", nodeDefinition->mName.c_str(), parentIndex);

        if (setTransformation)
        {
          nodeDefinition->mPosition    = position;
          nodeDefinition->mOrientation = rotation;
          nodeDefinition->mScale       = scale;
        }

        return nodeDefinition; }());

  if(!weakNode)
  {
    DALI_LOG_ERROR("Failed to create Node %s!\n", nodeName.c_str());
  }

  return weakNode;
}

void UsdLoaderImpl::Impl::RetrieveGeomPrimvars(const UsdPrim& prim, std::vector<UsdGeomPrimvar>& texcoords, std::vector<UsdGeomPrimvar>& colors, std::vector<UsdGeomPrimvar>& tangents)
{
  UsdGeomPrimvarsAPI          pvAPI(prim);
  std::vector<UsdGeomPrimvar> primvars = pvAPI.GetPrimvars();

  for(auto p : primvars)
  {
    if(p.HasAuthoredValue())
    {
      // Collect texture coordinates (UVs), assuming all UVs are stored in one of these primvar types
      if(p.GetTypeName() == SdfValueTypeNames->TexCoord2hArray || p.GetTypeName() == SdfValueTypeNames->TexCoord2fArray || p.GetTypeName() == SdfValueTypeNames->TexCoord2dArray || (p.GetPrimvarName() == "st" && p.GetTypeName() == SdfValueTypeNames->Float2Array))
      {
        texcoords.push_back(p);
      }
      else if(p.GetTypeName().GetRole() == SdfValueRoleNames->Color)
      {
        // Collect color attributes
        std::string colorName;
        size_t      pos = p.GetName().GetString().find(":");
        if(pos != std::string::npos)
        {
          colorName = p.GetName().GetString().substr(pos + 1);
        }

        if(colorName == "displayColor")
        {
          // Add "displayColor" at the front
          colors.insert(colors.begin(), p);
        }
        else
        {
          colors.push_back(p);
        }
      }

      // Collect tangent attributes
      if(p.GetName().GetString().find("tangents") != std::string::npos)
      {
        tangents.push_back(p);
      }
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "texcoords: %lu, colors: %lu, tangents: %lu, ", texcoords.size(), colors.size(), tangents.size());
}

void UsdLoaderImpl::Impl::ProcessMeshIndices(MeshDefinition& meshDefinition, std::map<int, VtArray<int>>& indexMap, VtIntArray& subsetIdcs, VtArray<int>& triangulatedIndex, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices)
{
  std::vector<VtArray<int>> subTriangulatedIndices;

  // Get indices for each subset
  for(int index : subsetIdcs)
  {
    subTriangulatedIndices.push_back(indexMap[index]);
  }

  // Flatten and store the triangulated indices for the current subset
  for(auto sublist : subTriangulatedIndices)
  {
    for(auto item : sublist)
    {
      flattenedSubTriangulatedIndices.push_back(item);
    }
  }

  for(int index : flattenedSubTriangulatedIndices)
  {
    subIndexArray.push_back(triangulatedIndex[index * 3]);
    subIndexArray.push_back(triangulatedIndex[index * 3 + 1]);
    subIndexArray.push_back(triangulatedIndex[index * 3 + 2]);
  }

  std::vector<uint32_t> indexArrayTriangulated;
  for(size_t k = 0; k < subIndexArray.size(); ++k)
  {
    indexArrayTriangulated.push_back(k);
  }

  // To store the final triangulated indices, we need space for uint32_t.
  meshDefinition.mRawData->mIndices.resize(indexArrayTriangulated.size() * 2);

  auto indicesData = reinterpret_cast<uint32_t*>(meshDefinition.mRawData->mIndices.data());
  for(size_t i = 0; i < indexArrayTriangulated.size(); i++)
  {
    indicesData[i] = indexArrayTriangulated[i];
  }
}

void UsdLoaderImpl::Impl::ProcessMeshPositions(MeshDefinition& meshDefinition, const VtArray<GfVec3f>& points, VtArray<GfVec3f>& worldPosition, std::vector<uint32_t>& subIndexArray)
{
  // Process vertex positions
  for(uint32_t index : subIndexArray)
  {
    worldPosition.push_back(points[index]);
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "subIndexArray: %lu, worldPosition: %lu, ", subIndexArray.size(), worldPosition.size());

  // Add vertex positions into the mesh definition
  std::vector<uint8_t> bufferPositions(worldPosition.size() * sizeof(GfVec3f));

  std::copy(reinterpret_cast<const uint8_t*>(worldPosition.data()),
            reinterpret_cast<const uint8_t*>(worldPosition.data() + worldPosition.size()),
            bufferPositions.begin());

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "bufferPositions.size: %lu, ", bufferPositions.size());

  meshDefinition.mRawData->mAttribs.push_back({"aPosition", Property::VECTOR3, static_cast<uint32_t>(worldPosition.size()), std::move(bufferPositions)});
}

void UsdLoaderImpl::Impl::ProcessMeshNormals(MeshDefinition& meshDefinition, UsdGeomMesh& usdMesh, VtArray<GfVec3f>& normals, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices, VtArray<int>& faceVertexCounts, bool isLeftHanded)
{
  UsdAttribute normalsAttr = usdMesh.GetNormalsAttr();
  if(normalsAttr.HasValue())
  {
    VtVec3fArray rawNormals;
    GetAttributeValue<VtVec3fArray>(normalsAttr, rawNormals);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "rawNormals: %lu, ", rawNormals.size());

    if(usdMesh.GetNormalsInterpolation().GetString() == "faceVarying")
    {
      // Handle face-varying normals (one normal per face vertex)
      VtArray<GfVec3f> triangulatedNormal = GetTriangulatedAttribute<GfVec3f>(faceVertexCounts, rawNormals, isLeftHanded);

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normals: faceVarying, triangulatedNormal: %lu, flattenedSubTriangulatedIndices: %lu, ", triangulatedNormal.size(), flattenedSubTriangulatedIndices.size());

      for(int index : flattenedSubTriangulatedIndices)
      {
        normals.push_back(static_cast<GfVec3f>(triangulatedNormal[index * 3]));
        normals.push_back(static_cast<GfVec3f>(triangulatedNormal[index * 3 + 1]));
        normals.push_back(static_cast<GfVec3f>(triangulatedNormal[index * 3 + 2]));
      }
    }
    else if(usdMesh.GetNormalsInterpolation().GetString() == "vertex")
    {
      // Handle vertex-based normals (one normal per vertex)
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normals: vertex, subIndexArray: %lu, ", subIndexArray.size());
      for(auto x : subIndexArray)
      {
        normals.push_back(static_cast<GfVec3f>(rawNormals[x]));
      }
    }

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normals: size = %lu, value: ", normals.size());

    if(normals.size() > 0)
    {
      std::vector<uint8_t> bufferNormals(normals.size() * sizeof(GfVec3f));

      std::copy(reinterpret_cast<const uint8_t*>(normals.data()),
                reinterpret_cast<const uint8_t*>(normals.data() + normals.size()),
                bufferNormals.begin());

      // Add normal attribute to the mesh definition
      meshDefinition.mRawData->mAttribs.push_back({"aNormal", Property::VECTOR3, static_cast<uint32_t>(normals.size()), std::move(bufferNormals)});
    }
  }
}

void UsdLoaderImpl::Impl::GenerateNormal(MeshDefinition& meshDefinition)
{
  auto& attribs = meshDefinition.mRawData->mAttribs;

  // Determine the number of indices. If indices are not defined, use the number of vertices in the position attribute.
  const uint32_t numIndices = meshDefinition.mRawData->mIndices.empty() ? attribs[0].mNumElements : static_cast<uint32_t>(meshDefinition.mRawData->mIndices.size() / 2);

  // Pointer to the vertex positions
  auto* positions = reinterpret_cast<const Vector3*>(attribs[0].mData.data());

  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(Vector3));
  auto                 normals = reinterpret_cast<Vector3*>(buffer.data());

  // Pointer to the index data
  auto indicesData = reinterpret_cast<uint32_t*>(meshDefinition.mRawData->mIndices.data());

  // Loop through each triangle (3 indices at a time)
  for(uint32_t i = 0; i < numIndices; i += 3)
  {
    // Get the positions of the three vertices of the triangle
    Vector3 pos[]{positions[indicesData[i]], positions[indicesData[i + 1]], positions[indicesData[i + 2]]};

    // Compute the edge vectors of the triangle
    Vector3 a = pos[1] - pos[0]; // Edge from vertex 0 to vertex 1
    Vector3 b = pos[2] - pos[0]; // Edge from vertex 0 to vertex 2

    // Compute the normal using the cross product of the two edge vectors
    Vector3 normal(a.Cross(b));

    // Accumulate the normal for each vertex of the triangle
    normals[indicesData[i]] += normal;
    normals[indicesData[i + 1]] += normal;
    normals[indicesData[i + 2]] += normal;
  }

  // Normalize the accumulated normals to ensure they are unit vectors
  auto iEnd = normals + attribs[0].mNumElements;
  while(normals != iEnd)
  {
    normals->Normalize();
    ++normals;
  }

  // Add generated normals to the mesh definition
  attribs.push_back({"aNormal", Property::VECTOR3, attribs[0].mNumElements, std::move(buffer)});
}

void UsdLoaderImpl::Impl::ProcessMeshTexcoords(MeshDefinition& meshDefinition, std::vector<UsdGeomPrimvar>& texcoords, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices, VtArray<int>& faceVertexCounts, bool isLeftHanded)
{
  if(texcoords.size() > 0 && texcoords.size() <= 2)
  {
    // Support up to two texture coordinate sets
    for(size_t i = 0; i < texcoords.size(); i++)
    {
      UsdGeomPrimvar stCoords            = texcoords[i];
      std::string    stCoordsPrimvarName = stCoords.GetName().GetString();
      std::string    txName              = stCoordsPrimvarName.substr(stCoordsPrimvarName.find(":") + 1, std::string::npos);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "texcoords[%lu]: %s, %s, ", i, stCoordsPrimvarName.c_str(), txName.c_str());

      if(stCoords.IsDefined())
      {
        VtVec2fArray rawUVs;
        GetFlattenedPrimvarValue<GfVec2f>(stCoords, rawUVs);
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "rawUVs: %lu, value: ", rawUVs.size());

        VtVec2fArray UVs;
        TfToken      interpolation = stCoords.GetInterpolation();
        if(interpolation.GetString() == "faceVarying")
        {
          // Handle face-varying UVs
          VtVec2fArray triangulatedUV = GetTriangulatedAttribute<GfVec2f>(faceVertexCounts, rawUVs, isLeftHanded);

          for(int index : flattenedSubTriangulatedIndices)
          {
            UVs.push_back(static_cast<GfVec2f>(triangulatedUV[index * 3]));
            UVs.push_back(static_cast<GfVec2f>(triangulatedUV[index * 3 + 1]));
            UVs.push_back(static_cast<GfVec2f>(triangulatedUV[index * 3 + 2]));
          }
        }
        else if(interpolation.GetString() == "vertex")
        {
          // Handle vertex-based UVs
          for(auto x : subIndexArray)
          {
            UVs.push_back(static_cast<GfVec2f>(rawUVs[x]));
          }
        }
        else
        {
          DALI_LOG_ERROR("Unexpected interpolation type %s for UV, ", interpolation.GetString().c_str());
          continue;
        }

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "UVs: size = %lu, value: ", UVs.size());

        // Flip UVs vertically to match the texture coordinate system in DALi
        VtVec2fArray flipyUVs;
        for(const auto& uv : UVs)
        {
          flipyUVs.push_back(GfVec2f(uv[0], 1.0f - uv[1]));
        }

        std::vector<uint8_t> bufferTexCoords(flipyUVs.size() * sizeof(GfVec2f));

        std::copy(reinterpret_cast<const uint8_t*>(flipyUVs.data()),
                  reinterpret_cast<const uint8_t*>(flipyUVs.data() + flipyUVs.size()),
                  bufferTexCoords.begin());

        // Add texcoord attribute to the mesh definition
        meshDefinition.mRawData->mAttribs.push_back({"aTexCoord", Property::VECTOR2, static_cast<uint32_t>(flipyUVs.size()), std::move(bufferTexCoords)});
      }
    }
  }
}

void UsdLoaderImpl::Impl::GenerateTangents(MeshDefinition& meshDefinition, std::vector<UsdGeomPrimvar>& texcoords)
{
  auto& attribs = meshDefinition.mRawData->mAttribs;

  // Required positions, normals, uvs (if we have them).
  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(Vector3));
  auto                 tangentsData = reinterpret_cast<Vector3*>(buffer.data());

  // Check if UVs are present. Tangents require UV coordinates for calculation.
  bool hasUVs = texcoords.size() > 0 && attribs.size() == 3;

  if(hasUVs)
  {
    // Number of indices (each triangle face has 3 indices).
    const uint32_t numIndices = meshDefinition.mRawData->mIndices.empty() ? attribs[0].mNumElements : static_cast<uint32_t>(meshDefinition.mRawData->mIndices.size() / 2);

    // Pointers to the vertex positions and UV coordinates.
    auto* positions = reinterpret_cast<const Vector3*>(attribs[0].mData.data());
    auto* uvs       = reinterpret_cast<const Vector2*>(attribs[2].mData.data());

    // Pointer to the index data.
    auto indicesData = reinterpret_cast<uint32_t*>(meshDefinition.mRawData->mIndices.data());

    // Loop over each triangle (three indices at a time).
    for(uint32_t i = 0; i < numIndices; i += 3)
    {
      // Get the positions of the triangle vertices.
      Vector3 pos[]{positions[indicesData[i]], positions[indicesData[i + 1]], positions[indicesData[i + 2]]};

      // Get the UV coordinates of the triangle vertices.
      Vector2 uv[]{uvs[indicesData[i]], uvs[indicesData[i + 1]], uvs[indicesData[i + 2]]};

      // Calculate the edge vectors in 3D space.
      float x0 = pos[1].x - pos[0].x;
      float y0 = pos[1].y - pos[0].y;
      float z0 = pos[1].z - pos[0].z;

      float x1 = pos[2].x - pos[0].x;
      float y1 = pos[2].y - pos[0].y;
      float z1 = pos[2].z - pos[0].z;

      // Calculate the edge vectors in UV space.
      float s0 = uv[1].x - uv[0].x;
      float t0 = uv[1].y - uv[0].y;

      float s1 = uv[2].x - uv[0].x;
      float t1 = uv[2].y - uv[0].y;

      // Calculate the determinant of the matrix formed by the UV edges.
      float det = (s0 * t1 - t0 * s1);

      // To avoid division by zero, check the determinant against a small epsilon value.
      float r = 1.f / ((std::abs(det) < Dali::Epsilon<1000>::value) ? (Dali::Epsilon<1000>::value * (det > 0.0f ? 1.f : -1.f)) : det);

      // Compute the tangent vector using the positions and UVs.
      Vector3 tangent((x0 * t1 - t0 * x1) * r, (y0 * t1 - t0 * y1) * r, (z0 * t1 - t0 * z1) * r);

      // Accumulate the tangent for each vertex of the triangle.
      tangentsData[indicesData[i]] += Vector3(tangent);
      tangentsData[indicesData[i + 1]] += Vector3(tangent);
      tangentsData[indicesData[i + 2]] += Vector3(tangent);
    }
  }

  // Normalize the accumulated tangents.
  auto* normalsData = reinterpret_cast<const Vector3*>(attribs[1].mData.data());
  auto  iEnd        = normalsData + attribs[1].mNumElements;
  while(normalsData != iEnd)
  {
    Vector3 tangentVec3;
    if(hasUVs)
    {
      // Tangent is calculated from the accumulated data.
      tangentVec3 = Vector3((*tangentsData).x, (*tangentsData).y, (*tangentsData).z);
    }
    else
    {
      // Fallback: generate tangent using the cross product of the normal with the X or Y axis.
      Vector3 t[]{normalsData->Cross(Vector3::XAXIS), normalsData->Cross(Vector3::YAXIS)};
      tangentVec3 = t[t[1].LengthSquared() > t[0].LengthSquared()];
    }

    // Orthogonalize the tangent by subtracting the component in the direction of the normal.
    tangentVec3 -= *normalsData * normalsData->Dot(tangentVec3);
    tangentVec3.Normalize();

    // Store the calculated tangent.
    if(hasUVs)
    {
      *tangentsData = tangentVec3;
    }
    else
    {
      *tangentsData = Vector4(tangentVec3.x, tangentVec3.y, tangentVec3.z, 1.0f);
    }

    ++tangentsData;
    ++normalsData;
  }

  // Add tangent attribute to the mesh definition
  attribs.push_back({"aTangent", Property::VECTOR3, attribs[0].mNumElements, std::move(buffer)});
}

void UsdLoaderImpl::Impl::ProcessMeshColors(MeshDefinition& meshDefinition, std::vector<UsdGeomPrimvar>& colors, VtArray<GfVec3f>& worldPosition, std::vector<uint32_t>& subIndexArray, std::vector<uint32_t>& flattenedSubTriangulatedIndices, VtArray<int>& faceVertexCounts, bool isLeftHanded)
{
  // Extract color data from the USD mesh primvar and map that data onto the mesh’s triangles.
  if(colors.size() > 0)
  {
    // We only support up to one color attribute
    UsdGeomPrimvar displayColor = colors[0];

    std::string colorPrimvarName = displayColor.GetName().GetString();
    std::string colorName        = colorPrimvarName.substr(colorPrimvarName.find(":") + 1, std::string::npos);
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "displayColor: %s, %s, ", colorPrimvarName.c_str(), colorName.c_str());

    if(displayColor.IsDefined() && displayColor.HasAuthoredValue())
    {
      VtVec3fArray rawColors;
      GetAttributeValue<VtVec3fArray>(displayColor.GetAttr(), rawColors);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "rawColors: %lu, ", rawColors.size());

      VtArray<GfVec3f> convertedColors;

      // USD allows different types of interpolation for primvars (colors, normals, etc.)
      TfToken interpolation = displayColor.GetInterpolation();
      if(interpolation.GetString() == "constant")
      {
        // Handle constant color: same color for all vertices
        convertedColors = VtArray<GfVec3f>(subIndexArray.size(), rawColors[0]);
      }
      else if(interpolation.GetString() == "faceVarying")
      {
        // Handle face-varying colors: one value per face–vertex, i.e. each corner of a face gets its own color
        VtVec3fArray triangulatedColors = GetTriangulatedAttribute<GfVec3f>(faceVertexCounts, rawColors, isLeftHanded);

        for(int index : flattenedSubTriangulatedIndices)
        {
          convertedColors.push_back(static_cast<GfVec3f>(triangulatedColors[index * 3]));
          convertedColors.push_back(static_cast<GfVec3f>(triangulatedColors[index * 3 + 1]));
          convertedColors.push_back(static_cast<GfVec3f>(triangulatedColors[index * 3 + 2]));
        }
      }
      else if(interpolation.GetString() == "vertex")
      {
        // Handle vertex colors: one value per vertex
        for(auto x : subIndexArray)
        {
          convertedColors.push_back(static_cast<GfVec3f>(rawColors[x]));
        }
      }
      else if(interpolation.GetString() == "uniform")
      {
        // Handle uniform colors: one color per face, i.e. all vertices of a face get the same color
        GetFlattenedPrimvarValue<GfVec3f>(displayColor, rawColors);
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "rawColors (uniform): %lu", rawColors.size());

        // Clear any previous color data.
        convertedColors.clear();

        // Check that we have one color per face.
        if(rawColors.size() == faceVertexCounts.size())
        {
          // For each face in the original mesh, replicate its uniform color for
          // each triangle generated from that face.
          for(size_t faceIndex = 0; faceIndex < faceVertexCounts.size(); ++faceIndex)
          {
            int vertexCount = faceVertexCounts[faceIndex];
            if(vertexCount < 3)
            {
              DALI_LOG_ERROR("Face %lu has fewer than 3 vertices. Skipping this face.", faceIndex);
              continue;
            }

            // A polygon with 'vertexCount' vertices produces (vertexCount - 2) triangles.
            int triangleCount = vertexCount - 2;
            for(int t = 0; t < triangleCount; ++t)
            {
              // Each triangle has 3 vertices. Replicate the same face color for each vertex.
              convertedColors.push_back(rawColors[faceIndex]);
              convertedColors.push_back(rawColors[faceIndex]);
              convertedColors.push_back(rawColors[faceIndex]);
            }
          }
        }
        else
        {
          DALI_LOG_ERROR("Mesh data integrity issue: raw colors (%lu) does not equal to face count (%lu).  Skipping this mesh.", rawColors.size(), faceVertexCounts.size());
        }
      }

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "convertedColors: size = %lu, value: ", convertedColors.size());

      // COLOR_0

      std::vector<uint8_t> bufferColors(convertedColors.size() * sizeof(GfVec3f));

      std::copy(reinterpret_cast<const uint8_t*>(convertedColors.data()),
                reinterpret_cast<const uint8_t*>(convertedColors.data() + convertedColors.size()),
                bufferColors.begin());

      // Add color attribute to the mesh definition
      meshDefinition.mRawData->mAttribs.push_back({"aVertexColor", Property::VECTOR3, static_cast<uint32_t>(convertedColors.size()), std::move(bufferColors)});
    }
  }
  else if(worldPosition.size() > 0)
  {
    // If no colors are defined, use white color (Vector4::ONE)
    std::vector<uint8_t> buffer(worldPosition.size() * sizeof(Vector4));
    auto                 bufferColors = reinterpret_cast<Vector4*>(buffer.data());

    for(uint32_t i = 0; i < worldPosition.size(); i++)
    {
      bufferColors[i] = Vector4::ONE;
    }

    // Add default white color attribute
    meshDefinition.mRawData->mAttribs.push_back({"aVertexColor", Property::VECTOR4, static_cast<uint32_t>(worldPosition.size()), std::move(buffer)});
  }
}

void UsdLoaderImpl::Impl::ProcessMaterialBinding(LoadResult& output, const UsdPrim& prim, std::vector<UsdGeomSubset>& subsets, size_t subIndex, int& meshSubMaterialId)
{
  auto& outMaterials = output.mResources.mMaterials;

  int meshMaterialId = INVALID_INDEX;

  UsdShadeMaterialBindingAPI materialAPI  = UsdShadeMaterialBindingAPI(prim);
  std::string                materialPath = materialAPI.ComputeBoundMaterial().GetPrim().GetPath().GetString();

  if(auto material = mMaterialMap.find(materialPath); material != mMaterialMap.end())
  {
    meshMaterialId = material->second;
  }

  bool doubleSided(false);
  if(prim.HasAttribute(TfToken("doubleSided")) && meshMaterialId >= 0)
  {
    // Handle double sidedness
    UsdAttribute doubleSidedAttr = UsdGeomMesh(prim).GetDoubleSidedAttr();
    GetAttributeValue<bool>(doubleSidedAttr, doubleSided);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "doubleSided: %d, ", doubleSided);

    outMaterials[meshMaterialId].first.mDoubleSided = doubleSided;
  }

  // Set default mesh material if no material is bound
  if(meshMaterialId >= 0)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "meshMaterialId: %d, materialPath: %s, ", meshMaterialId, materialPath.c_str());
  }
  else
  {
    // The default material is used when a mesh does not specify a material
    if(mDefaultMaterial == INVALID_INDEX)
    {
      mDefaultMaterial = outMaterials.size();

      MaterialDefinition materialDefinition;
      materialDefinition.mFlags |= MaterialDefinition::GLTF_CHANNELS;
      materialDefinition.mShadowAvailable              = true;
      materialDefinition.mNeedAlbedoTexture            = false;
      materialDefinition.mNeedMetallicRoughnessTexture = false;
      materialDefinition.mNeedNormalTexture            = false;

      outMaterials.emplace_back(std::move(materialDefinition), TextureSet());
    }

    meshMaterialId = mDefaultMaterial;
  }

  meshSubMaterialId = meshMaterialId;

  std::string subsetMaterialPath;

  // Set material for the subset of the mesh
  auto                       subset            = subsets[subIndex];
  UsdShadeMaterialBindingAPI subsetMaterialAPI = UsdShadeMaterialBindingAPI(subset.GetPrim());
  subsetMaterialPath                           = subsetMaterialAPI.ComputeBoundMaterial().GetPath().GetString();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "subsetMaterialPath: %s, ", subsetMaterialPath.c_str());

  if(auto subMaterial = mMaterialMap.find(subsetMaterialPath); subMaterial != mMaterialMap.end())
  {
    meshSubMaterialId = subMaterial->second;
  }

  if(meshSubMaterialId >= 0)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "meshSubMaterialId: %d, subsetMaterialPath: %s, ", meshSubMaterialId, subsetMaterialPath.c_str());

    // Set double-sided property if applicable
    outMaterials[meshSubMaterialId].first.mDoubleSided = doubleSided;
  }
  else
  {
    meshSubMaterialId = mDefaultMaterial;
  }
}

void UsdLoaderImpl::Impl::ConvertMesh(LoadResult& output, const UsdPrim& prim, Index& nodeIndex, Index parentIndex)
{
  auto& scene = output.mScene;

  nodeIndex = scene.GetNodeCount();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdGeomMesh %d, nodeIndex: %d, parentIndex: %d, ", mMeshCount, nodeIndex, parentIndex);
  mMeshCount++;

  Vector3    position;
  Quaternion rotation;
  Vector3    scale;
  Matrix     transformMatrix;

  // Handle transformation for non-skeleton mesh nodes
  bool isNonSkeletonMeshNode = prim.IsA<UsdGeomXformable>() && !prim.IsA<UsdSkelSkeleton>();
  if(isNonSkeletonMeshNode)
  {
    GetXformableTransformation(prim, position, rotation, scale);
  }

  // Create a new node for the mesh in the scene graph
  nodeIndex                = scene.GetNodeCount();
  NodeDefinition* weakNode = AddNodeToScene(scene, prim.GetName().GetString(), parentIndex, position, rotation, scale, isNonSkeletonMeshNode);

  // Handle xform animation
  ConvertTransformAnimation(output, prim, nodeIndex);

  // Start processing the mesh geometry
  UsdGeomMesh usdMesh = UsdGeomMesh(prim);

  // Retrieve the mesh's vertices (points)
  UsdAttribute     pointsAttr = usdMesh.GetPointsAttr();
  VtArray<GfVec3f> points;
  GetAttributeValue<VtArray<GfVec3f>>(pointsAttr, points);

  if(points.empty())
  {
    DALI_LOG_ERROR("No points in mesh, ");
    mMeshCount--;
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "PointsCount: %lu, ", points.size());

    auto& outMeshes = output.mResources.mMeshes;

    // Get Face Vertex Counts (number of vertices per face)
    VtArray<int> faceVertexCounts;
    UsdAttribute facesAttr = usdMesh.GetFaceVertexCountsAttr();
    GetAttributeValue<VtArray<int>>(facesAttr, faceVertexCounts);

    // Get Face Vertex Indices (index of vertices for each face)
    VtArray<int> faceVertexIndices;
    UsdAttribute indicesAttr = usdMesh.GetFaceVertexIndicesAttr();
    GetAttributeValue<VtArray<int>>(indicesAttr, faceVertexIndices);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "FaceVertexCounts: %lu, FaceVertexIndices: %lu, ", faceVertexCounts.size(), faceVertexIndices.size());

    std::vector<UsdGeomPrimvar> texcoords;
    std::vector<UsdGeomPrimvar> colors;
    std::vector<UsdGeomPrimvar> tangents;

    // Check for UV, color, and tangent attributes
    RetrieveGeomPrimvars(prim, texcoords, colors, tangents);

    // Determine if the mesh uses left-handed or right-handed coordinates
    TfToken      orientation;
    UsdAttribute orientationAttr = usdMesh.GetOrientationAttr();
    GetAttributeValue<TfToken>(orientationAttr, orientation);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "orientation: %s, ", orientation.GetText());

    bool isLeftHanded = orientation.GetString() != "rightHanded";

    // Maps triangulated indices
    std::map<int, VtArray<int>> indexMap;
    int                         j = 0;
    for(size_t i = 0; i < faceVertexCounts.size(); ++i)
    {
      VtArray<int> tmp;
      for(int k = 0; k < faceVertexCounts[i] - 2; ++k)
      {
        tmp.push_back(j);
        j++;
      }
      indexMap[i] = tmp;
    }

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "indexMap: %lu, ", indexMap.size());

    // Triangulate face indices
    VtArray<int> triangulatedIndex = GetTriangulatedAttribute<int>(faceVertexCounts, faceVertexIndices, isLeftHanded);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "triangulatedIndex: %lu, ", triangulatedIndex.size());

    // Get mesh subsets (i.e. the group of faces sharing the same material)
    std::vector<UsdGeomSubset> subsets          = UsdGeomSubset::GetAllGeomSubsets(usdMesh);
    VtIntArray                 remainingIndices = UsdGeomSubset::GetUnassignedIndices(subsets, faceVertexCounts.size());
    if(remainingIndices.size() > 0)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "extra subset: remainingIndices: %lu, ", remainingIndices.size());

      // Handle the case where a prim is an instance and therefore cannot be modified
      UsdPrim p = prim;
      while(p.IsInstance())
      {
        p = p.GetParent();
      }

      // Create a subset for unassigned faces
      subsets.emplace_back(UsdGeomSubset::CreateGeomSubset(UsdGeomImageable(p), UsdGeomTokens->partition, UsdGeomTokens->face, remainingIndices));
    }

    size_t numSubsets = subsets.size();

    // Reserve space for the mesh renderables
    weakNode->mRenderables.reserve(numSubsets);

    // Prepare subset indices
    std::vector<VtIntArray> subsetIndices(numSubsets, VtIntArray());
    for(size_t i = 0; i < numSubsets; ++i)
    {
      VtIntArray indices;
      GetAttributeValue<VtIntArray>(subsets[i].GetIndicesAttr(), indices);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "indices[%lu]: %lu, ", i, indices.size());

      if(indices == remainingIndices)
      {
        subsetIndices[i] = remainingIndices;
      }
      else
      {
        subsetIndices[i] = indices;
      }
    }

    // Process each subset of the mesh
    for(size_t subIndex = 0; subIndex < numSubsets; ++subIndex)
    {
      // Initialize a mesh definition for each subset
      MeshDefinition meshDefinition;
      meshDefinition.mRawData = std::make_shared<MeshDefinition::RawData>();
      meshDefinition.mFlags |= MeshDefinition::U32_INDICES;
      meshDefinition.mSkeletonIdx = INVALID_INDEX;

      // Process indices
      auto&                 subsetIdcs = subsetIndices[subIndex];
      std::vector<uint32_t> subIndexArray;
      std::vector<uint32_t> flattenedSubTriangulatedIndices;

      ProcessMeshIndices(meshDefinition, indexMap, subsetIdcs, triangulatedIndex, subIndexArray, flattenedSubTriangulatedIndices);

      // Process vertex positions
      VtArray<GfVec3f> worldPosition;
      ProcessMeshPositions(meshDefinition, points, worldPosition, subIndexArray);

      // Process normals
      VtArray<GfVec3f> normals;
      ProcessMeshNormals(meshDefinition, usdMesh, normals, subIndexArray, flattenedSubTriangulatedIndices, faceVertexCounts, isLeftHanded);

      // Generate normals if not provided
      if(normals.size() == 0 && meshDefinition.mRawData->mAttribs.size() > 0) // Check if normals are missing but positions are available
      {
        GenerateNormal(meshDefinition);
      }

      // Process texture coordinates (texcoords)
      ProcessMeshTexcoords(meshDefinition, texcoords, subIndexArray, flattenedSubTriangulatedIndices, faceVertexCounts, isLeftHanded);

      // Generate Tangents
      GenerateTangents(meshDefinition, texcoords);

      // Process vertex colors
      ProcessMeshColors(meshDefinition, colors, worldPosition, subIndexArray, flattenedSubTriangulatedIndices, faceVertexCounts, isLeftHanded);

      // Add the processed meshes to the output meshes list
      outMeshes.emplace_back(std::move(meshDefinition), MeshGeometry{});

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "outMeshes: mIndices: %lu, mAttribs: %lu, ", outMeshes.back().first.mRawData->mIndices.size(), outMeshes.back().first.mRawData->mAttribs.size());

      // Process material binding
      int meshSubMaterialId;
      ProcessMaterialBinding(output, prim, subsets, subIndex, meshSubMaterialId);

      // Create a renderable object for the model and associate the mesh and material with the renderable
      std::unique_ptr<NodeDefinition::Renderable> renderable;

      auto modelRenderable      = new ModelRenderable();
      modelRenderable->mMeshIdx = mMeshCount - 1 + subIndex;

      modelRenderable->mMaterialIdx = meshSubMaterialId;

      renderable.reset(modelRenderable);
      weakNode->mRenderables.push_back(std::move(renderable));

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "weakNode %s->mRenderables.push_back, ", weakNode->mName.c_str());
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "\n");
}

void UsdLoaderImpl::Impl::ConvertNode(LoadResult& output, const UsdPrim& prim, Index& nodeIndex, Index parentIndex)
{
  auto& scene = output.mScene;

  nodeIndex = scene.GetNodeCount();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdGeomXformable %d: parentIndex: %d, ", nodeIndex, parentIndex);

  // Retrieve the local transformation matrix for the node
  Vector3    position;
  Quaternion rotation;
  Vector3    scale;
  GetXformableTransformation(prim, position, rotation, scale);

  // Create a new node for the prim in the scene graph
  nodeIndex = scene.GetNodeCount();
  AddNodeToScene(scene, prim.GetName().GetString(), parentIndex, position, rotation, scale, true);

  // Handle xform animation
  ConvertTransformAnimation(output, prim, nodeIndex);

  // Check whether the prim is a transformable camera
  if(prim.IsA<UsdGeomCamera>())
  {
    ConvertCamera(output, prim);
  }
}

void UsdLoaderImpl::Impl::ConvertCamera(LoadResult& output, const UsdPrim& prim)
{
  auto& cameraParameters = output.mCameraParameters;

  // Initialize camera parameters with default values if not present
  if(cameraParameters.empty())
  {
    cameraParameters.push_back(CameraParameters());
    cameraParameters[0].matrix.SetTranslation(CAMERA_DEFAULT_POSITION);
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdGeomCamera: ");

  // Convert camera properties from USD to internal representation
  UsdGeomCamera camera         = UsdGeomCamera(prim);
  UsdAttribute  projectionAttr = camera.GetProjectionAttr();
  if(projectionAttr.HasValue())
  {
    TfToken projection("");
    GetAttributeValue<TfToken>(projectionAttr, projection);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "projection: %s, ", projection.GetText());

    if(projection.GetString() == "perspective")
    {
      cameraParameters[0].isPerspective = true;
    }
    else if(projection.GetString() == "orthographic")
    {
      cameraParameters[0].isPerspective = false;
    }
  }

  GfCamera     gfCamera          = camera.GetCamera(UsdTimeCode::Default());
  UsdAttribute clippingRangeAttr = camera.GetClippingRangeAttr();
  if(clippingRangeAttr.HasValue())
  {
    GfVec2f clippingRange(0.0f, 0.0f);
    GetAttributeValue<GfVec2f>(clippingRangeAttr, clippingRange);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "zNear: %.7f, zFar: %.7f, ", clippingRange[0], clippingRange[1]);

    cameraParameters[0].zNear = clippingRange[0];
    cameraParameters[0].zFar  = clippingRange[1];
  }

  Radian yFOV = Radian(gfCamera.GetFieldOfView(GfCamera::FOVVertical));
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "yFOV: %f, ", yFOV.radian);
  cameraParameters[0].yFovDegree = Degree(gfCamera.GetFieldOfView(GfCamera::FOVVertical));

  float aspectRatio = gfCamera.GetAspectRatio();
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "aspectRatio: %.7f, ", aspectRatio);
  cameraParameters[0].aspectRatio = aspectRatio;

  float apertureX = gfCamera.GetHorizontalAperture() / 10.0f;
  float apertureY = gfCamera.GetVerticalAperture() / 10.0f;
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "apertureX: %.7f, apertureY: %.7f\n", apertureX, apertureY);

  // Apply the camera's transform matrix to the camera parameters
  GfMatrix4d matrix          = gfCamera.GetTransform();
  cameraParameters[0].matrix = ConvertUsdMatrix(matrix);
}

void UsdLoaderImpl::Impl::ConvertTransformAnimation(LoadResult& output, const UsdPrim& prim, Index& nodeIndex)
{
  auto xformable = UsdGeomXformable(prim);

  std::vector<double> timeSamples;
  xformable.GetTimeSamples(&timeSamples);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "timeSamples: %lu, ", timeSamples.size());

  float FPS = mUsdStage->GetFramesPerSecond();

  // USD represents animation as time-sampled attribute values.
  // https://openusd.org/release/tut_xforms.html
  // https://openusd.org/release/glossary.html#usdglossary-timecode
  // For any given composed scene, defined by its root layer, the TimeCode ordinates of the TimeSamples
  // contained in the scene are scaled to seconds by the root layer’s timeCodesPerSecond metadata.
  // USD’s default FPS is 24 frames per second, and time code from GetTimeSamples() is the frame number.
  // e.g. if there are totally 192 frames, the duration of the animation is 8 seconds.

  if(timeSamples.size() > 0)
  {
    AnimationDefinition animationDefinition;

    // Times
    // Each xform may have its own amount of time samples
    float minTime = timeSamples[0] / FPS;
    float maxTime = timeSamples[0] / FPS;

    for(auto sample : timeSamples)
    {
      float gltfTime = sample / FPS;

      minTime = std::min(minTime, gltfTime);
      maxTime = std::max(maxTime, gltfTime);
    }

    float duration = maxTime - minTime;
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "minTime: %f, maxTime: %f, animation duration: %f, ", minTime, maxTime, duration);

    animationDefinition.ReserveSize(3);

    AnimatedProperty positionProperty;
    positionProperty.mNodeIndex    = nodeIndex;
    positionProperty.mPropertyName = "position";
    positionProperty.mTimePeriod   = {0.f, duration};
    positionProperty.mKeyFrames    = KeyFrames::New();

    AnimatedProperty orientationProperty;
    orientationProperty.mNodeIndex    = nodeIndex;
    orientationProperty.mPropertyName = "orientation";
    orientationProperty.mTimePeriod   = {0.f, duration};
    orientationProperty.mKeyFrames    = KeyFrames::New();

    AnimatedProperty scaleProperty;
    scaleProperty.mNodeIndex    = nodeIndex;
    scaleProperty.mPropertyName = "scale";
    scaleProperty.mTimePeriod   = {0.f, duration};
    scaleProperty.mKeyFrames    = KeyFrames::New();

    std::vector<Vector3>    translations;
    std::vector<Quaternion> rotations;
    std::vector<Vector3>    scales;

    // Iterate over each time sample to get the transform at each key frame
    for(auto time : timeSamples)
    {
      // Get the local transformation matrix at this time
      Vector3    position;
      Quaternion rotation;
      Vector3    scale;
      GetXformableTransformation(prim, position, rotation, scale, time);

      translations.push_back(position);
      rotations.push_back(rotation);
      scales.push_back(scale);

      float progress = time / FPS / duration;

      positionProperty.mKeyFrames.Add(progress, position);
      orientationProperty.mKeyFrames.Add(progress, rotation);
      scaleProperty.mKeyFrames.Add(progress, scale);
    }

    animationDefinition.SetProperty(0, std::move(positionProperty));
    animationDefinition.SetProperty(1, std::move(orientationProperty));
    animationDefinition.SetProperty(2, std::move(scaleProperty));

    animationDefinition.SetDuration(std::max(duration, AnimationDefinition::MIN_DURATION_SECONDS));

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "translations: %lu, rotations: %lu, scales: %lu, ", translations.size(), rotations.size(), scales.size());

    for(size_t i = 0; i < translations.size(); i++)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "KeyFrame[%lu]: progress: %.7f, translations[%lu]: %.7f, %.7f, %.7f, scales[%lu]: %.7f, %.7f, %.7f, rotations[%lu]: %.7f, %.7f, %.7f, %.7f, ", i, timeSamples[i] / FPS / duration, i, translations[i].x, translations[i].y, translations[i].z, i, scales[i].x, scales[i].y, scales[i].z, i, rotations[i].AsVector().x, rotations[i].AsVector().y, rotations[i].AsVector().z, rotations[i].AsVector().w);
    }

    std::ostringstream oss;
    oss << prim.GetName() << "_xform_anim";
    std::string animationName = oss.str();

    animationDefinition.SetName(animationName);

    output.mAnimationDefinitions.push_back(std::move(animationDefinition));
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "\n");
}

} // namespace Dali::Scene3D::Loader
