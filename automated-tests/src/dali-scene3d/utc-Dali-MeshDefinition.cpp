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

#include <vector>

#include <dali-scene3d/public-api/loader/buffer-definition.h>
#include <dali-scene3d/public-api/loader/mesh-definition.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliMeshDefinitionBlobApplyMinMaxBothMinMaxApplied(void)
{
  using Container = std::vector<float>;

  Container buffer    = {4.0f, 6.0f, 8.0f, 10.0f, 12.0f};
  Container minValues = {5.0f};
  Container maxValues = {10.0f};
  Container result    = {5.0f, 6.0f, 8.0f, 10.0f, 10.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for(auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionBlobApplyMinMaxOnlyMin(void)
{
  using Container = std::vector<float>;

  Container buffer    = {4.0f, 6.0f, 8.0f, 10.0f, 12.0f};
  Container minValues = {5.0f};
  Container maxValues = {};
  Container result    = {5.0f, 6.0f, 8.0f, 10.0f, 12.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for(auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionBlobApplyMinMaxOnlyMax(void)
{
  using Container = std::vector<float>;

  Container buffer    = {4.0f, 6.0f, 8.0f, 10.0f, 12.0f};
  Container minValues = {};
  Container maxValues = {10.0f};
  Container result    = {4.0f, 6.0f, 8.0f, 10.0f, 10.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for(auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionBlobApplyMinMaxBothEmpty(void)
{
  using Container = std::vector<float>;

  Container buffer = {4.0f, 6.0f, 8.0f, 10.0f, 12.0f};
  Container minValues;
  Container maxValues;
  Container result = {4.0f, 6.0f, 8.0f, 10.0f, 12.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for(auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionByteSkinWeight(void)
{
  float data8[8] = {0.003922, 0.062745, 0.250980, 0.098039, 0.937255, 0.749020, 0.741176, 0.937255};

  BufferDefinition bufferDefinition;
  bufferDefinition.mUri        = "data:application/base64,ARBAGe+/ve+/vT9hc2RmYXNkZmFzZGZhc2RmYXNkZmE=";
  bufferDefinition.mByteLength = 32;
  BufferDefinition::Vector buffers;
  buffers.push_back(std::move(bufferDefinition));

  MeshDefinition meshDefinition;
  meshDefinition.mFlags = MeshDefinition::U16_JOINT_IDS | MeshDefinition::U8_WEIGHT;
  MeshDefinition::SparseBlob sparseBlob;
  meshDefinition.mPositions =
    MeshDefinition::Accessor{
      std::move(MeshDefinition::Blob{0, 12, 0, (uint16_t)12, std::vector<float>(), std::vector<float>()}), std::move(sparseBlob), 0};
  meshDefinition.mJoints.push_back(
    MeshDefinition::Accessor{
      std::move(MeshDefinition::Blob{0, 16, 0, (uint16_t)16, std::vector<float>(), std::vector<float>()}), std::move(sparseBlob), 0});
  meshDefinition.mWeights.push_back(
    MeshDefinition::Accessor{
      std::move(MeshDefinition::Blob{0, 8, 0, (uint16_t)8, std::vector<float>(), std::vector<float>()}), std::move(sparseBlob), 0});

  MeshDefinition::RawData rawData = meshDefinition.LoadRaw("", buffers);

  DALI_TEST_EQUALS(rawData.mAttribs.size(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(rawData.mAttribs[3].mName, "aWeights0", TEST_LOCATION);
  DALI_TEST_EQUALS(rawData.mAttribs[3].mNumElements, 2, TEST_LOCATION);
  float* value = reinterpret_cast<float*>(rawData.mAttribs[3].mData.data());
  for(uint32_t i = 0; i < rawData.mAttribs[3].mNumElements * 4; ++i)
  {
    DALI_TEST_EQUALS(*value, data8[i], TEST_LOCATION);
    value++;
  }
  END_TEST;
}

int UtcDaliMeshDefinitionShortSkinWeight(void)
{
  float data8[8] = {0.062516, 0.098634, 0.749752, 0.936492, 0.741207, 0.379873, 0.392386, 0.380468};

  BufferDefinition bufferDefinition;
  bufferDefinition.mUri        = "data:application/base64,ARBAGe+/ve+/vT9hc2RmYXNkZmFzZGZhc2RmYXNkZmE=";
  bufferDefinition.mByteLength = 32;
  BufferDefinition::Vector buffers;
  buffers.push_back(std::move(bufferDefinition));

  MeshDefinition meshDefinition;
  meshDefinition.mFlags = MeshDefinition::U16_JOINT_IDS | MeshDefinition::U16_WEIGHT;
  MeshDefinition::SparseBlob sparseBlob;
  meshDefinition.mPositions =
    MeshDefinition::Accessor{
      std::move(MeshDefinition::Blob{0, 12, 0, (uint16_t)12, std::vector<float>(), std::vector<float>()}), std::move(sparseBlob), 0};
  meshDefinition.mJoints.push_back(
    MeshDefinition::Accessor{
      std::move(MeshDefinition::Blob{0, 16, 0, (uint16_t)16, std::vector<float>(), std::vector<float>()}), std::move(sparseBlob), 0});
  meshDefinition.mWeights.push_back(
    MeshDefinition::Accessor{
      std::move(MeshDefinition::Blob{0, 16, 0, (uint16_t)16, std::vector<float>(), std::vector<float>()}), std::move(sparseBlob), 0});

  MeshDefinition::RawData rawData = meshDefinition.LoadRaw("", buffers);

  DALI_TEST_EQUALS(rawData.mAttribs.size(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(rawData.mAttribs[3].mName, "aWeights0", TEST_LOCATION);
  DALI_TEST_EQUALS(rawData.mAttribs[3].mNumElements, 2, TEST_LOCATION);
  float* value = reinterpret_cast<float*>(rawData.mAttribs[3].mData.data());
  for(uint32_t i = 0; i < rawData.mAttribs[3].mNumElements * 4; ++i)
  {
    DALI_TEST_EQUALS(*value, data8[i], TEST_LOCATION);
    value++;
  }
  END_TEST;
}

int UtcDaliMeshDefinitionInvalidUrl(void)
{
  MeshDefinition meshDefinition;
  meshDefinition.mUri = "invalid-uri/";
  BufferDefinition::Vector buffers;
  MeshDefinition::RawData rawData = meshDefinition.LoadRaw("invalidModelPath", buffers);
  DALI_TEST_EQUALS(rawData.mIndices.size(), 0u, TEST_LOCATION);
  END_TEST;
}