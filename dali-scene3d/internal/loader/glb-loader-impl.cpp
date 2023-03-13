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
#include <dali-scene3d/internal/loader/glb-loader-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/gltf2-util.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace gt = gltf2;
namespace js = json;

namespace Dali::Scene3D::Loader::Internal
{
namespace
{
static constexpr uint32_t GLB_MAGIC       = 0x46546C67;
static constexpr uint32_t JSON_CHUNK_TYPE = 0x4E4F534A;
static constexpr uint32_t DATA_CHUNK_TYPE = 0x004E4942;

struct GlbHeader
{
  uint32_t magic;
  uint32_t version;
  uint32_t length;
};

struct ChunkHeader
{
  uint32_t chunkLength;
  uint32_t chunkType;
};

} // namespace

bool GlbLoaderImpl::LoadModel(const std::string& url, Dali::Scene3D::Loader::LoadResult& result)
{
  Dali::FileStream fileStream(url, FileStream::READ | FileStream::BINARY);
  auto&            stream = fileStream.GetStream();
  if(!stream.rdbuf()->in_avail())
  {
    DALI_LOG_ERROR("Load Model file is failed, url : %s\n", url.c_str());
    return false;
  }

  GlbHeader glbHeader;
  stream.clear();
  stream.seekg(0u, stream.beg);
  stream.read(reinterpret_cast<char*>(&glbHeader), sizeof(GlbHeader));

  if(glbHeader.magic != GLB_MAGIC)
  {
    DALI_LOG_ERROR("Wrong file format, url : %s\n", url.c_str());
    return false;
  }

  ChunkHeader jsonChunkHeader;
  stream.read(reinterpret_cast<char*>(&jsonChunkHeader), sizeof(ChunkHeader));

  if(jsonChunkHeader.chunkType != JSON_CHUNK_TYPE)
  {
    DALI_LOG_ERROR("Glb files first chunk is not a json chunk.\n");
    return false;
  }

  std::vector<uint8_t> jsonChunkData;
  jsonChunkData.resize(jsonChunkHeader.chunkLength);
  stream.read(reinterpret_cast<char*>(&jsonChunkData[0]), static_cast<std::streamsize>(static_cast<size_t>(jsonChunkHeader.chunkLength)));
  std::string gltfText(jsonChunkData.begin(), jsonChunkData.end());

  uint32_t             binaryChunkOffset = sizeof(GlbHeader) + sizeof(ChunkHeader) + jsonChunkHeader.chunkLength;
  std::vector<uint8_t> binaryChunkData;
  if(glbHeader.length > binaryChunkOffset)
  {
    ChunkHeader binaryChunkHeader;
    stream.read(reinterpret_cast<char*>(&binaryChunkHeader), sizeof(ChunkHeader));

    if(binaryChunkHeader.chunkType != DATA_CHUNK_TYPE)
    {
      DALI_LOG_ERROR("Glb files has wrong binary chunk data.\n");
      return false;
    }

    binaryChunkData.resize(binaryChunkHeader.chunkLength);
    stream.read(reinterpret_cast<char*>(&binaryChunkData[0]), static_cast<std::streamsize>(static_cast<size_t>(binaryChunkHeader.chunkLength)));
  }

  json::unique_ptr root(json_parse(gltfText.c_str(), gltfText.size()));
  if(!root)
  {
    DALI_LOG_ERROR("Failed to parse %s\n", url.c_str());
    return false;
  }

  gt::Document document;

  bool isMRendererModel(false);
  if(!Gltf2Util::GenerateDocument(root, document, isMRendererModel))
  {
    DALI_LOG_ERROR("Failed to parse %s\n", url.c_str());
    return false;
  }

  auto                         path = url.substr(0, url.rfind('/') + 1);
  Gltf2Util::ConversionContext context{result, path, INVALID_INDEX};

  auto& outBuffers = context.mOutput.mResources.mBuffers;
  outBuffers.reserve(document.mBuffers.size());
  if(!binaryChunkData.empty())
  {
    BufferDefinition dataBuffer(binaryChunkData);
    outBuffers.emplace_back(std::move(dataBuffer));
  }

  Gltf2Util::ConvertGltfToContext(document, context, isMRendererModel);

  return true;
}

} // namespace Dali::Scene3D::Loader::Internal
