#ifndef DALI_SCENE3D_LOADER_USD_UTILS_H
#define DALI_SCENE3D_LOADER_USD_UTILS_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>

#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usdGeom/primvar.h>
#include <pxr/usd/usdShade/shader.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
using UsdShaderContainer = std::vector<pxr::UsdShadeShader>;
using UsdAssetBuffer     = std::vector<uint8_t>;

/**
 * @brief Converts a USD matrix (GfMatrix4d) to a Dali Matrix.
 *
 * @param gfMat The input USD matrix (GfMatrix4d) to be converted.
 * @return The equivalent Dali Matrix.
 */
Dali::Matrix ConvertUsdMatrix(const pxr::GfMatrix4d& gfMat);

/**
 * @brief Recursively traverses connected shader inputs and collects all relevant shaders.
 *
 * @param shader The starting USD shader to begin traversing from.
 * @return A vector containing all collected shaders.
 */
UsdShaderContainer TraverseShaderInputs(const pxr::UsdShadeShader& shader);

/**
 * @brief Converts a USD image path to a standard path format.
 *
 * This function takes a USD image path, which may include tokens like `[]` or other
 * USD-specific formatting, and converts it into a standard file path format that can
 * be used to load textures from the file system.
 *
 * @param input The original image path as used in USD.
 * @return The converted image path.
 */
std::string ConvertImagePath(const std::string& input);

/**
 * @brief Loads a USD asset file into a memory buffer
 *
 * @param resolvedAssetPath The resolved file path of the USD asset.
 * @return A vector of bytes containing the loaded asset data.
 */
UsdAssetBuffer LoadAssetFileAsBuffer(const std::string resolvedAssetPath);

/**
 * @brief Retrieves the value of a USD attribute.
 *
 * @tparam T The data type of the attribute (e.g., GfVec3f, float).
 * @param attribute The USD attribute to retrieve the value from.
 * @param value The output parameter where the attribute's value will be stored.
 * @return The retrieved value of the attribute.
 */
template<typename T>
T GetAttributeValue(pxr::UsdAttribute attribute, T& value)
{
  std::vector<double> times;
  attribute.GetTimeSamples(&times);
  if(times.size() > 0u)
  {
    attribute.Get<T>(&value, times[0]);
  }
  else
  {
    attribute.Get<T>(&value, pxr::UsdTimeCode::Default());
  }

  return value;
}

/**
 * @brief Retrieves the flattened value of a USD geometry primvar (e.g., color, normals).
 *
 * Flattening is necessary when the primvar's interpolation type requires computation over
 * multiple samples, like face-varyingdata, before it can be used for rendering.
 *
 * @tparam T The data type of the primvar (e.g., GfVec3f for positions, GfVec2f for UVs).
 * @param primvar The USD geometry primvar (e.g., color, normal, etc.).
 * @param value The output array where the flattened primvar values are stored.
 * @return The flattened values of the geometry primvar.
 */
template<typename T>
pxr::VtArray<T> GetFlattenedPrimvarValue(pxr::UsdGeomPrimvar primvar, pxr::VtArray<T>& value)
{
  std::vector<double> times;
  primvar.GetAttr().GetTimeSamples(&times);

  if(times.size() > 0u)
  {
    primvar.ComputeFlattened<T>(&value, times[0]);
  }
  else
  {
    primvar.ComputeFlattened<T>(&value, pxr::UsdTimeCode::Default());
  }

  return value;
}

/**
 * @brief Triangulates polygonal faces based on their vertex indices, converting them into triangles.
 *
 * USD can store mesh data in polygons with more than three sides (n-gons). When preparing for
 * rendering, these n-gons must be converted into triangles. This function takes an array of
 * vertex counts per face (e.g., quads, pentagons) and converts these faces into triangles by
 * generating new vertex indices that represent the triangulated mesh.
 *
 * The process of triangulation involves breaking down these polygons (which may have 4, 5,
 * or more vertices) into a set of triangles. Each n-sided polygon is split into n-2 triangles.
 * For example, a quad (4 vertices) is split into two triangles. Triangulation also considers
 * the coordinate system's handedness (left-handed or right-handed), which affects the winding
 * order of vertices in the triangles.
 *
 * @tparam T The data type of the indices (e.g., int, unsigned int).
 * @param countArray An array containing the number of vertices for each polygonal face (e.g., 4 for quads).
 * @param indexArray An array containing the vertex indices for each polygonal face.
 * @param isLeftHanded A boolean indicating whether the coordinate system is left-handed.
 * @return An array of triangulated vertex indices.
 */
template<typename T>
pxr::VtArray<T> GetTriangulatedAttribute(const pxr::VtArray<int>& countArray, const pxr::VtArray<T>& indexArray, bool isLeftHanded)
{
  pxr::VtArray<T> returnArray;
  int             j = 0;

  // Iterate over each polygon in the count array
  for(int count : countArray)
  {
    // Extract the indices for the current polygon
    const pxr::VtArray<T> poly(indexArray.begin() + j, indexArray.begin() + j + count);

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

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_USD_UTILS_H
