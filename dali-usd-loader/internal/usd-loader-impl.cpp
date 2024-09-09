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

#include <pxr/usd/ar/asset.h>
#include <pxr/usd/ar/resolvedPath.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdSkel/animation.h>
#include <pxr/usd/usdSkel/bindingAPI.h>
#include <pxr/usd/usdSkel/root.h>
#include <pxr/usd/usdSkel/skeleton.h>
#include <pxr/usd/usdSkel/utils.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/utils.h>

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

// Convert a USD matrix to a DALi Matrix
Matrix ConvertUsdMatrix(const GfMatrix4d& gfMat)
{
  std::vector<float> matData(gfMat.data(), gfMat.data() + 16);
  return Matrix(matData.data());
}

// Template function to retrieve the value of a USD attribute, handling time samples if available
template<typename T>
T GetAttributeValue(UsdAttribute attribute, T& value)
{
  std::vector<double> times;
  attribute.GetTimeSamples(&times);
  if(times.size() > 0u)
  {
    attribute.Get<T>(&value, times[0]);
  }
  else
  {
    attribute.Get<T>(&value, UsdTimeCode::Default());
  }

  return value;
}

// Template function to retrieve the flattened value of a USD geometry primvar (e.g., color, normals)
template<typename T>
VtArray<T> GetFlattenedPrimvarValue(UsdGeomPrimvar primvar, VtArray<T>& value)
{
  std::vector<double> times;
  primvar.GetAttr().GetTimeSamples(&times);

  if(times.size() > 0u)
  {
    primvar.ComputeFlattened<T>(&value, times[0]);
  }
  else
  {
    primvar.ComputeFlattened<T>(&value, UsdTimeCode::Default());
  }

  return value;
}

// Recursively traverses connected shader inputs to collect all relevant shaders
std::vector<UsdShadeShader> TraverseShaderInputs(const UsdShadeShader& shader)
{
  std::vector<UsdShadeShader> matches;

  for(const auto& i : shader.GetInputs())
  {
    if(i.HasConnectedSource())
    {
      for(const auto& s : i.GetConnectedSources())
      {
        if(s)
        {
          matches.push_back(s.source);
          auto nestedMatches = TraverseShaderInputs(s.source);
          matches.insert(matches.end(), nestedMatches.begin(), nestedMatches.end());
        }
      }
    }
  }

  return matches;
}

// Triangulates polygonal faces based on their vertex indices, converting them into triangles.
//
// USD can store mesh data in polygons with more than three sides (n-gons). When preparing for
// rendering, these n-gons must be converted into triangles. This function takes an array of
// vertex counts per face (e.g., quads, pentagons) and converts these faces into triangles by
// generating new vertex indices that represent the triangulated mesh.
//
// The process of triangulation involves breaking down these polygons (which may have 4, 5,
// or more vertices) into a set of triangles. Each n-sided polygon is split into n-2 triangles.
// For example, a quad (4 vertices) is split into two triangles. Triangulation also considers
// the coordinate system's handedness (left-handed or right-handed), which affects the winding
// order of vertices in the triangles.
template<typename T>
VtArray<T> GetTriangulatedAttribute(const VtArray<int>& countArray, const VtArray<T>& indexArray, bool isLeftHanded)
{
  VtArray<T> returnArray;
  int        j = 0;

  // Iterate over each polygon in the count array
  for(int count : countArray)
  {
    // Extract the indices for the current polygon
    const VtArray<T> poly(indexArray.begin() + j, indexArray.begin() + j + count);

    // Triangulate the polygon (assumes convex polygons)
    for(int i = 0; i < count - 2; ++i)
    {
      // Append triangulated indices to the return array
      if(isLeftHanded)
      {
        // Left-handed winding order
        returnArray.push_back(poly[0]);
        returnArray.push_back(poly[i + 2]);
        returnArray.push_back(poly[i + 1]);
      }
      else
      {
        // Right-handed winding order
        returnArray.push_back(poly[0]);
        returnArray.push_back(poly[i + 1]);
        returnArray.push_back(poly[i + 2]);
      }
    }

    // Move to the next polygon
    j += count;
  }

  return returnArray;
}

// Converts a USD image path to a standard path format
std::string ConvertImagePath(const std::string& input)
{
  std::string result = input;

  // Find the position of '[' and ']'
  size_t startPos = result.find('[');
  size_t endPos   = result.find(']');

  if(startPos != std::string::npos && endPos != std::string::npos)
  {
    // Extract the substring between '[' and ']'
    std::string extracted = result.substr(startPos + 1, endPos - startPos - 1);

    // Find the last '/' in the extracted string between '[' and ']'
    size_t lastSlashPosInExtracted = extracted.rfind('/', endPos);
    if(lastSlashPosInExtracted != std::string::npos)
    {
      extracted.erase(0, lastSlashPosInExtracted + 1);
    }

    // Find the last '/' before '[' in the original path
    size_t lastSlashPos = result.rfind('/', startPos);
    if(lastSlashPos != std::string::npos)
    {
      result.erase(lastSlashPos + 1, endPos - lastSlashPos + 1);
      result.insert(lastSlashPos + 1, extracted);
    }
  }

  return result;
}

// Loads a USD asset file as a memory buffer (vector of uint8_t)
std::vector<uint8_t> LoadAssetFileAsBuffer(const std::string resolvedAssetPath)
{
  std::shared_ptr<ArAsset> const asset = ArGetResolver().OpenAsset(ArResolvedPath(resolvedAssetPath));

  if(asset)
  {
    std::shared_ptr<const char> const buffer = asset->GetBuffer();
    if(buffer)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "LoadAssetFileAsBuffer: %s, size: %lu", ArResolvedPath(resolvedAssetPath).GetPathString().c_str(), asset->GetSize());

      // Convert the buffer to a vector of uint8_t
      return std::vector<uint8_t>(buffer.get(), buffer.get() + asset->GetSize());
    }
  }

  // Return an empty vector if loading fails
  return std::vector<uint8_t>();
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
   * @brief Converts a texture associated with a material to the internal representation.
   * @param[in] usdMaterial The USD material.
   * @param[in] usdUvTexture The USD UV texture.
   * @param[in, out] materialDefinition The material definition.
   * @param[in] semantic The semantic information of the texture.
   * @return True if conversion successful, false otherwise.
   */
  bool ConvertTexture(const UsdShadeMaterial& usdMaterial, const UsdShadeShader& usdUvTexture, MaterialDefinition& materialDefinition, uint32_t semantic = 0u);

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

  std::map<std::string, int> mMaterialMap; ///< Maps prim paths to material IDs.

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
  auto& outMaterials = output.mResources.mMaterials;

  int materialId = 0; // Initialize material ID counter

  // Traverse all prims (nodes) in the USD stage
  UsdPrimRange prims = mUsdStage->Traverse();
  for(auto prim : prims)
  {
    // Check if the current prim is a material
    if(prim.IsA<UsdShadeMaterial>())
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, " => UsdShadeMaterial: %d: %s\n        ", materialId, prim.GetPrimPath().GetText());

      UsdShadeMaterial material = UsdShadeMaterial(prim);
      UsdShadeOutput   surf     = material.GetSurfaceOutput();

      // If no valid connected sources are found, skip this material
      if(surf.GetConnectedSources().size() == 0)
      {
        DALI_LOG_ERROR("No valid connected sources, ");
        continue;
      }

      UsdShadeConnectableAPI     previewSurface = surf.GetConnectedSources()[0].source;
      std::vector<UsdShadeInput> inputs         = previewSurface.GetInputs();

      // Initialize the material definition with default values
      MaterialDefinition materialDefinition;

      materialDefinition.mFlags |= MaterialDefinition::GLTF_CHANNELS;
      materialDefinition.mShadowAvailable = true;

      materialDefinition.mBaseColorFactor     = Vector4::ONE;
      materialDefinition.mEmissiveFactor      = Vector3::ZERO;
      materialDefinition.mSpecularFactor      = 1.0f;
      materialDefinition.mSpecularColorFactor = Dali::Vector3::ONE;

      materialDefinition.mMetallic    = 1.0f;
      materialDefinition.mRoughness   = 1.0f;
      materialDefinition.mNormalScale = 1.0f;

      materialDefinition.mShadowAvailable = true;
      materialDefinition.mDoubleSided     = false;

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mMaterialMap[%s] = %d, ", prim.GetPrimPath().GetAsString().c_str(), materialId);

      // Map the material path to the material ID
      mMaterialMap[prim.GetPrimPath().GetAsString()] = materialId;
      materialId++;

      // Flags to track different material properties
      bool hasAlpha                     = false;
      bool hasThreshold                 = false;
      bool needMetallicRoughnessTexture = false;
      bool needMetallicTexture          = false;
      bool needRoughnessTexture         = false;
      bool needNormalTexture            = false;
      bool needAlbedoTexture            = false;

      float opacityThreshold(0.0f);

      std::map<uint32_t, UsdShadeInput> shaderInputMap; // Map of texture semantic and shader input

      // Loop through all inputs of the surface shader and sort them to
      // match with the order of texture loading in MaterialDefinition.
      for(auto input : inputs)
      {
        std::string baseName = input.GetBaseName().GetString();

        // Handle opacity input
        if(baseName == "opacity")
        {
          UsdAttribute opacityAttr = input.GetAttr();
          float        opacity(1.0f);
          if(opacityAttr.HasAuthoredValue())
          {
            GetAttributeValue<float>(opacityAttr, opacity);
          }

          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "opacity: %f, ", opacity);

          // Set the alpha value in the base color factor
          materialDefinition.mBaseColorFactor.a = opacity;

          // Check if the material has transparency
          if(opacity < 1.0f || input.HasConnectedSource())
          {
            hasAlpha = true;
          }

          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "hasAlpha: %d, ", hasAlpha);
        }
        else if(baseName == "opacityThreshold")
        {
          // Handle opacity threshold input (for alpha masking)
          UsdAttribute opacityThresholdAttr = input.GetAttr();
          if(opacityThresholdAttr.HasAuthoredValue())
          {
            GetAttributeValue<float>(opacityThresholdAttr, opacityThreshold);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "opacityThreshold: %.7f, ", opacityThreshold);
          }

          if(opacityThreshold > 0.0f)
          {
            hasThreshold = true;
          }
        }
        else if(baseName == "ior")
        {
          // Handle index of refraction (ior)
          UsdAttribute iorAttr = input.GetAttr();
          float        ior(1.5f);
          if(iorAttr.HasAuthoredValue())
          {
            GetAttributeValue<float>(iorAttr, ior);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ior: %.7f, ", ior);

            materialDefinition.mIor                = ior;
            materialDefinition.mDielectricSpecular = powf((materialDefinition.mIor - 1.0f) / (materialDefinition.mIor + 1.0f), 2.0f);
          }
        }
        else if(baseName == "diffuseColor")
        {
          shaderInputMap[MaterialDefinition::ALBEDO] = input;
        }
        else if(baseName == "metallic")
        {
          shaderInputMap[MaterialDefinition::METALLIC] = input;
        }
        else if(baseName == "roughness")
        {
          shaderInputMap[MaterialDefinition::ROUGHNESS] = input;
        }
        else if(baseName == "normal")
        {
          shaderInputMap[MaterialDefinition::NORMAL] = input;
        }
        else if(baseName == "occlusion")
        {
          shaderInputMap[MaterialDefinition::OCCLUSION] = input;
        }
        else if(baseName == "emissiveColor")
        {
          shaderInputMap[MaterialDefinition::EMISSIVE] = input;
        }
        else if(baseName == "specularColor")
        {
          shaderInputMap[MaterialDefinition::SPECULAR_COLOR] = input;
        }
        else if(baseName == "useSpecularWorkflow")
        {
          UsdAttribute useSpecularWorkflowAttr = input.GetAttr();
          int          useSpecularWorkflow(0);
          if(useSpecularWorkflowAttr.HasAuthoredValue())
          {
            GetAttributeValue<int>(useSpecularWorkflowAttr, useSpecularWorkflow);
          }
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "useSpecularWorkflow: %d, ", useSpecularWorkflow);
        }
      }

      // Process each mapped shader input
      for(auto iter : shaderInputMap)
      {
        UsdShadeInput input = iter.second;

        // Check if the input has a connected texture source
        UsdShadeShader uvTexture;
        if(input.HasConnectedSource())
        {
          uvTexture = UsdShadeShader(input.GetConnectedSources()[0].source);
        }

        std::string baseName = input.GetBaseName().GetString();

        if(baseName == "diffuseColor")
        {
          // Process diffuse color (albedo)
          UsdAttribute diffuseAttr = input.GetAttr();
          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "diffuseColorTexture: ");
              needAlbedoTexture = ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::ALBEDO);
              if(needAlbedoTexture)
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::ALBEDO, ");
              }
            }
          }
          else
          {
            GfVec3f diffuseColor(0.18f, 0.18f, 0.18f);
            if(diffuseAttr.HasAuthoredValue())
            {
              GetAttributeValue<GfVec3f>(diffuseAttr, diffuseColor);
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "diffuseColor: %.7f, %.7f, %.7f, ", diffuseColor[0], diffuseColor[1], diffuseColor[2]);
            }

            // Set the base color factor of the material
            materialDefinition.mBaseColorFactor.r = diffuseColor[0];
            materialDefinition.mBaseColorFactor.g = diffuseColor[1];
            materialDefinition.mBaseColorFactor.b = diffuseColor[2];
          }
        }
        else if(baseName == "metallic")
        {
          // Process metallic input
          UsdAttribute metallicAttr = input.GetAttr();

          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "metallicTexture: ");
              needMetallicTexture = ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::METALLIC);
              if(needMetallicTexture)
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::METALLIC, ");
              }
            }
          }
          else if(metallicAttr.HasAuthoredValue())
          {
            float metallicFactor(0.0f);
            GetAttributeValue<float>(metallicAttr, metallicFactor);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "metallicFactor: %.7f， ", metallicFactor);

            // Set the metallic factor of the material
            materialDefinition.mMetallic = metallicFactor;
          }
        }
        else if(baseName == "roughness")
        {
          // Process roughness input
          UsdAttribute roughnessAttr = input.GetAttr();
          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "roughnessTexture: ");
              needRoughnessTexture = ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::ROUGHNESS);
              if(needRoughnessTexture)
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::MaterialDefinition::ROUGHNESS, ");
              }
            }
          }
          else if(roughnessAttr.HasAuthoredValue())
          {
            float roughnessFactor(0.5f);
            GetAttributeValue<float>(roughnessAttr, roughnessFactor);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "roughnessFactor: %.7f， ", roughnessFactor);

            // Set the roughness factor of the material
            materialDefinition.mRoughness = roughnessFactor;
          }
        }
        else if(baseName == "normal")
        {
          // Process normal map input
          UsdAttribute normalAttr = input.GetAttr();
          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normalTexture: ");
              needNormalTexture = ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::NORMAL);
              if(needNormalTexture)
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::NORMAL, ");
              }
            }
          }
          else if(normalAttr.HasAuthoredValue())
          {
            GfVec3f normal;
            GetAttributeValue<GfVec3f>(normalAttr, normal);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "normal: %.7f, %.7f, %.7f， ", normal[0], normal[1], normal[2]);
          }
        }
        else if(baseName == "occlusion")
        {
          // Process occlusion map input
          UsdAttribute occlusionAttr = input.GetAttr();
          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "occlusionTexture: ");
              if(ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::OCCLUSION))
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::OCCLUSION, ");
              }
            }
          }
          else if(occlusionAttr.HasAuthoredValue())
          {
            float occlusion(1.0f);
            GetAttributeValue<float>(occlusionAttr, occlusion);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "occlusion: %.7f， ", occlusion);
          }
        }
        else if(baseName == "emissiveColor")
        {
          // Process emissive color input
          UsdAttribute emissiveAttr = input.GetAttr();
          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "emissiveColorTexture: ");
              if(ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::EMISSIVE))
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::EMISSIVE, ");
                materialDefinition.mEmissiveFactor = Vector3::ONE;
              }

              // Handle emissive color scale
              UsdShadeInput scaleInput = uvTexture.GetInput(TfToken("scale"));
              if(scaleInput)
              {
                GfVec4d scale;
                scaleInput.Get<GfVec4d>(&scale);
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "emissiveColorScale: %.7f, %.7f, %.7f, %.7f, ", scale[0], scale[1], scale[2], scale[3]);
              }
            }
          }

          if(emissiveAttr.HasAuthoredValue())
          {
            GfVec3f emissiveFactor;
            GetAttributeValue<GfVec3f>(emissiveAttr, emissiveFactor);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "emissiveFactor: %.7f, %.7f, %.7f， ", emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);

            // Set the emissive factor of the material
            materialDefinition.mEmissiveFactor = Vector3(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);
          }
        }
        else if(baseName == "specularColor")
        {
          // Process specular color input
          UsdAttribute specularAttr = input.GetAttr();
          if(input.HasConnectedSource())
          {
            TfToken id;
            if(uvTexture.GetShaderId(&id) && id.GetString() == "UsdUVTexture")
            {
              // Convert the texture and associate it with the material
              DALI_LOG_INFO(gLogFilter, Debug::Verbose, "specularColorTexture: ");
              if(ConvertTexture(material, uvTexture, materialDefinition, MaterialDefinition::SPECULAR_COLOR))
              {
                DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: MaterialDefinition::SPECULAR_COLOR, ");
              }
            }
          }
          else if(specularAttr.HasAuthoredValue())
          {
            GfVec3f specularColor;
            GetAttributeValue<GfVec3f>(specularAttr, specularColor);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "specularColor: %.7f, %.7f, %.7f， ", specularColor[0], specularColor[1], specularColor[2]);

            // Set the specular color factor of the material
            materialDefinition.mSpecularColorFactor = Vector3(specularColor[0], specularColor[1], specularColor[2]);
          }
        }
      }

      // Set alpha mode based on transparency and threshold values
      if(hasAlpha)
      {
        if(hasThreshold)
        {
          materialDefinition.mAlphaModeType = Scene3D::Material::AlphaModeType::MASK;
          materialDefinition.mIsMask        = true;
          materialDefinition.SetAlphaCutoff(std::min(1.f, std::max(0.f, opacityThreshold)));
        }
        else
        {
          materialDefinition.mAlphaModeType = Scene3D::Material::AlphaModeType::BLEND;
          materialDefinition.mIsOpaque      = false;
          materialDefinition.mFlags |= MaterialDefinition::TRANSPARENCY;
        }
      }

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TraverseMaterials: materialDefinition.mFlags: %u. needAlbedoTexture: %d, needMetallicRoughnessTexture: %d, needNormalTexture: %d\n", materialDefinition.mFlags, needAlbedoTexture, needMetallicRoughnessTexture, needNormalTexture);

      // Set texture needs in the material definition
      materialDefinition.mNeedAlbedoTexture            = needAlbedoTexture;
      materialDefinition.mNeedMetallicRoughnessTexture = needMetallicRoughnessTexture;
      materialDefinition.mNeedMetallicTexture          = needMetallicTexture;
      materialDefinition.mNeedRoughnessTexture         = needRoughnessTexture;
      materialDefinition.mNeedNormalTexture            = needNormalTexture;

      // Add the processed material to the output materials list
      outMaterials.emplace_back(std::move(materialDefinition), TextureSet());
    }
  }
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
  else if(prim.IsA<UsdGeomCamera>())
  {
    ConvertCamera(output, prim);
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

bool UsdLoaderImpl::Impl::ConvertTexture(const UsdShadeMaterial& usdMaterial, const UsdShadeShader& usdUvTexture, MaterialDefinition& materialDefinition, uint32_t semantic)
{
  bool    transformOffsetAuthored = false;
  GfVec2f uvTransformOffset(0.0f, 0.0f);
  float   uvTransformRotation = 0.0f;
  GfVec2f uvTransformScale(0.0f, 0.0f);

  // Get all inputs (primvar reader, transform, etc)
  std::vector<UsdShadeShader> deps = TraverseShaderInputs(usdUvTexture);
  deps.push_back(UsdShadeShader(usdUvTexture.GetPrim()));

  for(const auto& d : deps)
  {
    TfToken tokenId;
    d.GetShaderId(&tokenId);
    std::string depsId = tokenId.GetString();

    // Check if it is a primvar reader (for UV channels)
    if(depsId == "UsdPrimvarReader_float2")
    {
      TfToken tokenVarName("varname");
      // Handle UV channel
      UsdShadeInput shaderInput = UsdShadeShader(d).GetInput(tokenVarName);
      if(shaderInput)
      {
        std::string uvMapName;
        shaderInput.Get<std::string>(&uvMapName);
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "uvMapName: %s, ", uvMapName.c_str());
      }
    }
    else if(depsId == "UsdTransform2d") // Check if it is a 2D transform
    {
      // Extract transformation attributes (translation, scale, rotation)
      for(auto input : UsdShadeShader(d).GetInputs())
      {
        transformOffsetAuthored = true;

        std::string baseName = input.GetBaseName().GetString();
        if(baseName == "translation")
        {
          TfToken       tokenTranslation(baseName);
          UsdShadeInput translationInput = UsdShadeShader(d).GetInput(tokenTranslation);
          if(translationInput)
          {
            translationInput.Get<GfVec2f>(&uvTransformOffset);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "uvTransformOffset: %.7f, %.7f, ", uvTransformOffset[0], uvTransformOffset[1]);
          }
        }
        else if(baseName == "scale")
        {
          TfToken       tokenScale(baseName);
          UsdShadeInput scaleInput = UsdShadeShader(d).GetInput(tokenScale);
          if(scaleInput)
          {
            scaleInput.Get<GfVec2f>(&uvTransformScale);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "uvTransformScale: %.7f, %.7f, ", uvTransformScale[0], uvTransformScale[1]);
          }
        }
        else if(baseName == "rotation")
        {
          TfToken       tokenRotation(baseName);
          UsdShadeInput rotationInput = UsdShadeShader(d).GetInput(tokenRotation);
          if(rotationInput)
          {
            rotationInput.Get<float>(&uvTransformRotation); // in Degree, need to convert it to Radian
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "uvTransformRotation: %.7f, ", uvTransformRotation);
          }
        }
      }
    }
    else if(depsId == "UsdUVTexture")
    {
      // Handle UV texture

      std::string          imagePath;
      std::vector<uint8_t> imageBuffer;

      // Extract various texture attributes (file, wrapS, wrapT, scale, bias, st, fallback)
      std::vector<UsdShadeInput> inputs = usdUvTexture.GetInputs();
      for(auto input : inputs)
      {
        std::string baseName = input.GetBaseName().GetString();
        if(baseName == "file")
        {
          // Get the asset path of the texture
          SdfAssetPath fileInput;
          input.Get<SdfAssetPath>(&fileInput);

          std::string resolvedAssetPath = fileInput.GetResolvedPath();
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "File: %s, ", resolvedAssetPath.c_str());

          imagePath = ConvertImagePath(resolvedAssetPath);
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Converted File Path: %s, ", imagePath.c_str());

          // Load the texture image data as a buffer
          imageBuffer = LoadAssetFileAsBuffer(resolvedAssetPath);
        }
        else if(baseName == "wrapS")
        {
          // Handle texture wrapping in S direction
          TfToken       tokenWrapS(baseName);
          UsdShadeInput wrapSInput = UsdShadeShader(d).GetInput(tokenWrapS);
          if(wrapSInput)
          {
            TfToken wrapS;
            wrapSInput.Get<TfToken>(&wrapS);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "wrapS: %s, ", wrapS.GetText());
          }
        }
        else if(baseName == "wrapT")
        {
          // Handle texture wrapping in T direction
          TfToken       tokenWrapT(baseName);
          UsdShadeInput wrapTInput = UsdShadeShader(d).GetInput(tokenWrapT);
          if(wrapTInput)
          {
            TfToken wrapT;
            wrapTInput.Get<TfToken>(&wrapT);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "wrapT: %s, ", wrapT.GetText());
          }
        }
        else if(baseName == "scale")
        {
          // Handle texture scale
          TfToken       tokenScale(baseName);
          UsdShadeInput scaleInput = UsdShadeShader(d).GetInput(tokenScale);
          if(scaleInput)
          {
            GfVec4f scale;
            scaleInput.Get<GfVec4f>(&scale);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "scale: %.7f, %.7f, %.7f, %.7f, ", scale[0], scale[1], scale[2], scale[3]);
          }
        }
        else if(baseName == "bias")
        {
          // Handle texture bias
          TfToken       tokenBias(baseName);
          UsdShadeInput biasInput = UsdShadeShader(d).GetInput(tokenBias);
          if(biasInput)
          {
            GfVec4f bias;
            biasInput.Get<GfVec4f>(&bias);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "bias: %.7f, %.7f, %.7f, %.7f, ", bias[0], bias[1], bias[2], bias[3]);
          }
        }
        else if(baseName == "st")
        {
          // Handle texture ST (UV) coordinates
          TfToken       tokenSt(baseName);
          UsdShadeInput stInput = UsdShadeShader(d).GetInput(tokenSt);
          if(stInput)
          {
            GfVec2f st;
            stInput.Get<GfVec2f>(&st);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "st: %.7f, %.7f, ", st[0], st[1]);
          }
        }
        else if(baseName == "fallback")
        {
          // Handle fallback color for the texture
          TfToken       tokenFallback(baseName);
          UsdShadeInput fallbackInput = UsdShadeShader(d).GetInput(tokenFallback);
          if(fallbackInput)
          {
            GfVec4f fallback;
            fallbackInput.Get<GfVec4f>(&fallback);
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "fallback: %.7f, %.7f, %.7f, %.7f, ", fallback[0], fallback[1], fallback[2], fallback[3]);
          }
        }
      }

      if(imageBuffer.size() > 0)
      {
        // If the texture is loaded as an image buffer, add the buffer to the material definition
        materialDefinition.mTextureStages.push_back({semantic, TextureDefinition{std::move(imageBuffer)}});
        materialDefinition.mFlags |= semantic;
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mTextureStages.push_back: semantic: %u, mFlags: %u, imageBuffer: %ld, ", semantic, materialDefinition.mFlags, imageBuffer.size());

        return true;
      }
      else if(!imagePath.empty())
      {
        // Otherwise, add the image file path to the material definition
        materialDefinition.mTextureStages.push_back({semantic, TextureDefinition{std::move(imagePath)}});
        materialDefinition.mFlags |= semantic;
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mTextureStages.push_back: semantic: %u, mFlags: %u, imagePath: %s, ", semantic, materialDefinition.mFlags, imagePath.c_str());

        return true;
      }
    }
  }

  if(transformOffsetAuthored)
  {
    // @TODO: Process texture transform (similar to KHR_texture_transform) (future work)
  }

  return false; // Return false if texture conversion fails
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

      TfToken interpolation = displayColor.GetInterpolation();
      if(interpolation.GetString() == "constant")
      {
        // Handle constant color (same color for all vertices)
        convertedColors = VtArray<GfVec3f>(subIndexArray.size(), rawColors[0]);
      }
      else if(interpolation.GetString() == "faceVarying")
      {
        // Handle face-varying colors
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
        // Handle vertex colors
        for(auto x : subIndexArray)
        {
          convertedColors.push_back(static_cast<GfVec3f>(rawColors[x]));
        }
      }
      else if(interpolation.GetString() == "uniform")
      {
        // Handle uniform colors
        GetFlattenedPrimvarValue<GfVec3f>(displayColor, rawColors);
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "rawColors (uniform): %lu, value: ", rawColors.size());

        for(auto x : subIndexArray)
        {
          convertedColors.push_back(static_cast<GfVec3f>(rawColors[x]));
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

  // @TODO: Handle xform animation (future work)

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

  // @TODO: Handle xform animation (future work)
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

} // namespace Dali::Scene3D::Loader
