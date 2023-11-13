/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.

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
 */

// CLASS HEADER
#include <dali-scene3d/public-api/loader/navigation-mesh-factory.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/algorithm/navigation-mesh-impl.h>
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <stdlib.h>
#include <memory>

namespace Dali::Scene3D::Loader
{
std::unique_ptr<Algorithm::NavigationMesh> NavigationMeshFactory::CreateFromFile(std::string filename)
{
  std::vector<uint8_t> buffer;

  Dali::FileStream fileStream(filename, Dali::FileStream::READ | Dali::FileStream::BINARY);
  auto             fin = fileStream.GetFile();

  if(DALI_UNLIKELY(!fin))
  {
    const int bufferLength = 128;
    char      buffer[bufferLength];

    // Return type of stderror_r is different between system type. We should not use return value.
    [[maybe_unused]] auto ret = strerror_r(errno, buffer, bufferLength - 1);

    DALI_LOG_ERROR("NavigationMesh: Can't open %s for reading: %s", filename.c_str(), buffer);
    return nullptr;
  }

  if(DALI_UNLIKELY(fseek(fin, 0, SEEK_END)))
  {
    DALI_LOG_ERROR("NavigationMesh: Error reading file: %s\n", filename.c_str());
    return nullptr;
  }

  auto size = ftell(fin);
  if(DALI_UNLIKELY(size < 0))
  {
    DALI_LOG_ERROR("NavigationMesh: Error reading file: %s\n", filename.c_str());
    return nullptr;
  }

  auto fileSize = size_t(size);
  if(DALI_UNLIKELY(fseek(fin, 0, SEEK_SET)))
  {
    DALI_LOG_ERROR("NavigationMesh: Error reading file: %s\n", filename.c_str());
    return nullptr;
  }

  buffer.resize(size);
  auto count = fread(buffer.data(), 1, fileSize, fin);
  if(DALI_UNLIKELY(count != fileSize))
  {
    DALI_LOG_ERROR("NavigationMesh: Error reading file: %s\n", filename.c_str());
    return nullptr;
  }
  return CreateFromBuffer(buffer);
}

std::unique_ptr<Algorithm::NavigationMesh> NavigationMeshFactory::CreateFromBuffer(const std::vector<uint8_t>& buffer)
{
  auto impl = new Scene3D::Internal::Algorithm::NavigationMesh(buffer);
  return std::make_unique<Algorithm::NavigationMesh>(impl);
}

std::unique_ptr<Algorithm::NavigationMesh> NavigationMeshFactory::CreateFromVertexFaceList(const Vector3* vertices, const Vector3* vertexNormals, uint32_t vertexCount, const uint32_t* faceIndices, uint32_t indexCount)
{
  // The function takes the data and creates a binary buffer out of it
  using namespace Dali::Scene3D::Algorithm;
  auto header = Internal::Algorithm::NavigationMeshHeader_V10();

  // create header
  header.checksum = *reinterpret_cast<const uint32_t*>("NAVM");
  header.version  = 0; // latest version

  // Copy given vertices
  std::vector<NavigationMesh::Vertex> meshVertices;
  meshVertices.reserve(vertexCount);
  for(auto i = 0u; i < vertexCount; ++i)
  {
    meshVertices.emplace_back();
    meshVertices.back().x = vertices[i].x;
    meshVertices.back().y = vertices[i].y;
    meshVertices.back().z = vertices[i].z;
  }

  // copy faces and edges
  std::vector<NavigationMesh::Face> meshFaces;
  meshFaces.resize(indexCount / 3);
  auto i              = 0u;
  bool computeNormals = (vertexNormals == nullptr);
  for(auto& f : meshFaces)
  {
    f.vertex[0] = faceIndices[i];
    f.vertex[1] = faceIndices[i + 1];
    f.vertex[2] = faceIndices[i + 2];

    // compute normals (if not supplied)
    if(computeNormals)
    {
      auto v01 = Vector3(meshVertices[f.vertex[1]].coordinates) - Vector3(meshVertices[f.vertex[0]].coordinates);
      auto v02 = Vector3(meshVertices[f.vertex[2]].coordinates) - Vector3(meshVertices[f.vertex[0]].coordinates);
      auto n   = v01.Cross(v02);
      n.Normalize();
      f.normal[0] = n.x;
      f.normal[1] = n.y;
      f.normal[2] = n.z;
    }
    else
    {
      auto& n0 = vertexNormals[faceIndices[i]];
      auto& n1 = vertexNormals[faceIndices[i + 1]];
      auto& n2 = vertexNormals[faceIndices[i + 2]];

      auto faceNormal = (n0 + n1 + n2) / 3.0f;
      faceNormal.Normalize();
      f.normal[0] = faceNormal.x;
      f.normal[1] = -faceNormal.y;
      f.normal[2] = faceNormal.z;
    }
    i += 3;
  }

  // Create edges, in this case we don't care about duplicates
  // This mesh cannot be used for navigation
  std::vector<NavigationMesh::Edge> meshEdges;
  meshEdges.reserve(meshFaces.size() * 3);
  i = 0;
  for(auto& f : meshFaces)
  {
    for(auto k = 0u; k < 3; ++k)
    {
      meshEdges.emplace_back();
      auto& edge     = meshEdges.back();
      edge.face[0]   = i;
      edge.face[1]   = NavigationMesh::NULL_FACE;
      edge.vertex[0] = f.vertex[k];
      edge.vertex[1] = f.vertex[(k + 1) % 3];
    }
    ++i;
  }

  std::vector<uint8_t> navigationMeshBinary;

  // Build navigationMeshBinary binary
  navigationMeshBinary.insert(navigationMeshBinary.end(),
              reinterpret_cast<uint8_t*>(&header),
              reinterpret_cast<uint8_t*>(&header) + sizeof(Internal::Algorithm::NavigationMeshHeader_V10));

  auto& h = *reinterpret_cast<decltype(header)*>(navigationMeshBinary.data());

  h.dataOffset       = sizeof(header);
  h.edgeCount        = meshEdges.size();
  h.polyCount        = meshFaces.size();
  h.vertexCount      = meshVertices.size();
  h.gravityVector[0] = 0.0f;
  h.gravityVector[1] = -1.0f;
  h.gravityVector[2] = 0.0f;
  h.version          = 0;
  h.vertexDataOffset = 0;
  h.edgeDataOffset   = meshVertices.size() * sizeof(NavigationMesh::Vertex);
  h.polyDataOffset   = h.edgeDataOffset + meshEdges.size() * sizeof(NavigationMesh::Edge);

  // Copy data
  navigationMeshBinary.insert(navigationMeshBinary.end(),
              reinterpret_cast<uint8_t*>(meshVertices.data()),
              reinterpret_cast<uint8_t*>(meshVertices.data()) + (meshVertices.size() * sizeof(NavigationMesh::Vertex)));
  navigationMeshBinary.insert(navigationMeshBinary.end(),
              reinterpret_cast<uint8_t*>(meshEdges.data()),
              reinterpret_cast<uint8_t*>(meshEdges.data()) + (meshEdges.size() * sizeof(NavigationMesh::Edge)));
  navigationMeshBinary.insert(navigationMeshBinary.end(),
              reinterpret_cast<uint8_t*>(meshFaces.data()),
              reinterpret_cast<uint8_t*>(meshFaces.data()) + (meshFaces.size() * sizeof(NavigationMesh::Face)));

  auto retval = std::move(NavigationMeshFactory::CreateFromBuffer(navigationMeshBinary));
  return retval;
}

std::unique_ptr<Algorithm::NavigationMesh> NavigationMeshFactory::CreateFromVertexFaceList(const std::vector<Vector3>& vertices, const std::vector<Vector3>& normals, const std::vector<uint32_t>& faceIndices)
{
  return CreateFromVertexFaceList(vertices.data(), normals.data(), vertices.size(), faceIndices.data(), faceIndices.size());
}

std::vector<uint8_t> NavigationMeshFactory::GetMeshBinary(const Dali::Scene3D::Algorithm::NavigationMesh& navigationMesh)
{
  auto& meshImpl = Internal::Algorithm::GetImplementation(navigationMesh);

  // Return as mutable copy
  return meshImpl.GetData();
}

} // namespace Dali::Scene3D::Loader