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
  return std::unique_ptr<Algorithm::NavigationMesh>(new Algorithm::NavigationMesh(impl));
}

} // namespace Dali::Scene3D::Loader