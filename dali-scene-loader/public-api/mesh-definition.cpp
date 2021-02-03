/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/mesh-definition.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <fstream>
#include "dali/devel-api/adaptor-framework/pixel-buffer.h"

namespace Dali
{
namespace SceneLoader
{
namespace
{
using Uint16Vector4 = uint16_t[4];

class IndexProvider
{
public:
  IndexProvider(const uint16_t* indices)
  : mData(reinterpret_cast<uintptr_t>(indices)),
    mFunc(indices ? IncrementPointer : Increment)
  {
  }

  uint16_t operator()()
  {
    return mFunc(mData);
  }

private:
  static uint16_t Increment(uintptr_t& data)
  {
    return static_cast<uint16_t>(data++);
  }

  static uint16_t IncrementPointer(uintptr_t& data)
  {
    auto iPtr   = reinterpret_cast<const uint16_t*>(data);
    auto result = *iPtr;
    data        = reinterpret_cast<uintptr_t>(++iPtr);
    return result;
  }

  uintptr_t mData;
  uint16_t (*mFunc)(uintptr_t&);
};

const std::string QUAD("quad");

///@brief Reads a blob from the given stream @a source into @a target, which must have
/// at least @a descriptor.length bytes.
bool ReadBlob(const MeshDefinition::Blob& descriptor, std::istream& source, uint8_t* target)
{
  if(!source.seekg(descriptor.mOffset, std::istream::beg))
  {
    return false;
  }

  if(descriptor.IsConsecutive())
  {
    return !!source.read(reinterpret_cast<char*>(target), descriptor.mLength);
  }
  else
  {
    DALI_ASSERT_DEBUG(descriptor.mStride > descriptor.mElementSizeHint);
    const uint32_t diff     = descriptor.mStride - descriptor.mElementSizeHint;
    uint32_t       readSize = 0;
    while(readSize < descriptor.mLength &&
          source.read(reinterpret_cast<char*>(target), descriptor.mElementSizeHint) &&
          source.seekg(diff, std::istream::cur))
    {
      readSize += descriptor.mStride;
      target += descriptor.mElementSizeHint;
    }
    return readSize == descriptor.mLength;
  }
}

template<typename T>
void ReadValues(const std::vector<uint8_t>& valuesBuffer, const std::vector<uint8_t>& indicesBuffer, uint8_t* target, uint32_t count, uint32_t elementSizeHint)
{
  const T* const indicesPtr = reinterpret_cast<const T* const>(indicesBuffer.data());
  for(uint32_t index = 0u; index < count; ++index)
  {
    uint32_t valuesIndex = indicesPtr[index] * elementSizeHint;
    memcpy(target + valuesIndex, &valuesBuffer[index * elementSizeHint], elementSizeHint);
  }
}

bool ReadAccessor(const MeshDefinition::Accessor& accessor, std::istream& source, uint8_t* target)
{
  bool success = false;

  if(accessor.mBlob.IsDefined())
  {
    success = ReadBlob(accessor.mBlob, source, target);
    if(!success)
    {
      return false;
    }
  }

  if(accessor.mSparse)
  {
    const MeshDefinition::Blob& indices = accessor.mSparse->mIndices;
    const MeshDefinition::Blob& values  = accessor.mSparse->mValues;

    if(!indices.IsDefined() || !values.IsDefined())
    {
      return false;
    }

    const auto           indicesBufferSize = indices.GetBufferSize();
    std::vector<uint8_t> indicesBuffer(indicesBufferSize);
    success = ReadBlob(indices, source, indicesBuffer.data());
    if(!success)
    {
      return false;
    }

    const auto           valuesBufferSize = values.GetBufferSize();
    std::vector<uint8_t> valuesBuffer(valuesBufferSize);
    success = ReadBlob(values, source, valuesBuffer.data());
    if(!success)
    {
      return false;
    }

    switch(indices.mElementSizeHint)
    {
      case 1u:
      {
        ReadValues<uint8_t>(valuesBuffer, indicesBuffer, target, accessor.mSparse->mCount, values.mElementSizeHint);
        break;
      }
      case 2u:
      {
        ReadValues<uint16_t>(valuesBuffer, indicesBuffer, target, accessor.mSparse->mCount, values.mElementSizeHint);
        break;
      }
      case 4u:
      {
        ReadValues<uint32_t>(valuesBuffer, indicesBuffer, target, accessor.mSparse->mCount, values.mElementSizeHint);
        break;
      }
      default:
        DALI_ASSERT_DEBUG(!"Unsupported type for an index");
    }
  }

  return success;
}

void GenerateNormals(MeshDefinition::RawData& raw)
{
  auto& attribs = raw.mAttribs;
  DALI_ASSERT_DEBUG(attribs.size() > 0); // positions
  IndexProvider getIndex(raw.mIndices.data());

  const uint32_t numIndices = raw.mIndices.empty() ? attribs[0].mNumElements : raw.mIndices.size();

  auto* positions = reinterpret_cast<const Vector3*>(attribs[0].mData.data());

  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(Vector3));
  auto                 normals = reinterpret_cast<Vector3*>(buffer.data());

  for(uint32_t i = 0; i < numIndices; i += 3)
  {
    uint16_t indices[]{getIndex(), getIndex(), getIndex()};
    Vector3  pos[]{positions[indices[0]], positions[indices[1]], positions[indices[2]]};

    Vector3 a = pos[1] - pos[0];
    Vector3 b = pos[2] - pos[0];

    Vector3 normal(a.Cross(b));
    normals[indices[0]] += normal;
    normals[indices[1]] += normal;
    normals[indices[2]] += normal;
  }

  auto iEnd = normals + attribs[0].mNumElements;
  while(normals != iEnd)
  {
    normals->Normalize();
    ++normals;
  }

  attribs.push_back({"aNormal", Property::VECTOR3, attribs[0].mNumElements, std::move(buffer)});
}

void GenerateTangentsWithUvs(MeshDefinition::RawData& raw)
{
  auto& attribs = raw.mAttribs;
  DALI_ASSERT_DEBUG(attribs.size() > 2); // positions, normals, uvs
  IndexProvider getIndex(raw.mIndices.data());

  const uint32_t numIndices = raw.mIndices.empty() ? attribs[0].mNumElements : raw.mIndices.size();

  auto* positions = reinterpret_cast<const Vector3*>(attribs[0].mData.data());
  auto* uvs       = reinterpret_cast<const Vector2*>(attribs[2].mData.data());

  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(Vector3));
  auto                 tangents = reinterpret_cast<Vector3*>(buffer.data());

  for(uint32_t i = 0; i < numIndices; i += 3)
  {
    uint16_t indices[]{getIndex(), getIndex(), getIndex()};
    Vector3  pos[]{positions[indices[0]], positions[indices[1]], positions[indices[2]]};
    Vector2  uv[]{uvs[indices[0]], uvs[indices[1]], uvs[indices[2]]};

    float x0 = pos[1].x - pos[0].x;
    float y0 = pos[1].y - pos[0].y;
    float z0 = pos[1].z - pos[0].z;

    float x1 = pos[2].x - pos[0].x;
    float y1 = pos[2].y - pos[0].y;
    float z1 = pos[2].z - pos[0].z;

    float s0 = uv[1].x - uv[0].x;
    float t0 = uv[1].y - uv[0].y;

    float s1 = uv[2].x - uv[0].x;
    float t1 = uv[2].y - uv[0].y;

    float   r = 1.f / (s0 * t1 - t0 * s1);
    Vector3 tangent((x0 * t1 - t0 * x1) * r, (y0 * t1 - t0 * y1) * r, (z0 * t1 - t0 * z1) * r);
    tangents[indices[0]] += tangent;
    tangents[indices[1]] += tangent;
    tangents[indices[2]] += tangent;
  }

  auto* normals = reinterpret_cast<const Vector3*>(attribs[1].mData.data());
  auto  iEnd    = normals + attribs[1].mNumElements;
  while(normals != iEnd)
  {
    *tangents -= *normals * normals->Dot(*tangents);
    tangents->Normalize();

    ++tangents;
    ++normals;
  }
  attribs.push_back({"aTangent", Property::VECTOR3, attribs[0].mNumElements, std::move(buffer)});
}

void GenerateTangents(MeshDefinition::RawData& raw)
{
  auto& attribs = raw.mAttribs;
  DALI_ASSERT_DEBUG(attribs.size() > 1); // positions, normals

  auto* normals = reinterpret_cast<const Vector3*>(attribs[1].mData.data());

  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(Vector3));
  auto                 tangents = reinterpret_cast<Vector3*>(buffer.data());

  auto iEnd = normals + attribs[1].mNumElements;
  while(normals != iEnd)
  {
    Vector3 t[]{normals->Cross(Vector3::XAXIS), normals->Cross(Vector3::YAXIS)};

    *tangents = t[t[1].LengthSquared() > t[0].LengthSquared()];
    *tangents -= *normals * normals->Dot(*tangents);
    tangents->Normalize();

    ++tangents;
    ++normals;
  }
  attribs.push_back({"aTangent", Property::VECTOR3, attribs[0].mNumElements, std::move(buffer)});
}

void CalculateTextureSize(uint32_t totalTextureSize, uint32_t& textureWidth, uint32_t& textureHeight)
{
  DALI_ASSERT_DEBUG(0u != totalTextureSize && "totalTextureSize is zero.")

  // Calculate the dimensions of the texture.
  // The total size of the texture is the length of the blend shapes blob.

  textureWidth  = 0u;
  textureHeight = 0u;

  if(0u == totalTextureSize)
  {
    // nothing to do.
    return;
  }

  const uint32_t pow2      = static_cast<uint32_t>(ceil(log2(totalTextureSize)));
  const uint32_t powWidth  = pow2 >> 1u;
  const uint32_t powHeight = pow2 - powWidth;

  textureWidth  = 1u << powWidth;
  textureHeight = 1u << powHeight;
}

void CalculateGltf2BlendShapes(uint8_t* geometryBuffer, std::ifstream& binFile, const std::vector<MeshDefinition::BlendShape>& blendShapes, uint32_t numberOfVertices, float& blendShapeUnnormalizeFactor)
{
  uint32_t geometryBufferIndex = 0u;
  float    maxDistance         = 0.f;
  Vector3* geometryBufferV3    = reinterpret_cast<Vector3*>(geometryBuffer);
  for(const auto& blendShape : blendShapes)
  {
    if(blendShape.deltas.IsDefined())
    {
      DALI_ASSERT_ALWAYS(((blendShape.deltas.mBlob.mLength % sizeof(Vector3) == 0u) ||
                          blendShape.deltas.mBlob.mStride >= sizeof(Vector3)) &&
                         "Blend Shape position buffer length not a multiple of element size");

      const auto           bufferSize = blendShape.deltas.mBlob.GetBufferSize();
      std::vector<uint8_t> buffer(bufferSize);
      if(ReadAccessor(blendShape.deltas, binFile, buffer.data()))
      {
        blendShape.deltas.mBlob.ApplyMinMax(bufferSize / sizeof(Vector3), reinterpret_cast<float*>(buffer.data()));
        // Calculate the difference with the original mesh.
        // Find the max distance to normalize the deltas.
        const Vector3* const deltasBuffer = reinterpret_cast<const Vector3* const>(buffer.data());

        for(uint32_t index = 0u; index < numberOfVertices; ++index)
        {
          Vector3& delta = geometryBufferV3[geometryBufferIndex++];
          delta          = deltasBuffer[index];

          maxDistance = std::max(maxDistance, delta.LengthSquared());
        }
      }
    }

    if(blendShape.normals.IsDefined())
    {
      DALI_ASSERT_ALWAYS(((blendShape.normals.mBlob.mLength % sizeof(Vector3) == 0u) ||
                          blendShape.normals.mBlob.mStride >= sizeof(Vector3)) &&
                         "Blend Shape normals buffer length not a multiple of element size");

      const auto           bufferSize = blendShape.normals.mBlob.GetBufferSize();
      std::vector<uint8_t> buffer(bufferSize);
      if(ReadAccessor(blendShape.normals, binFile, buffer.data()))
      {
        blendShape.normals.mBlob.ApplyMinMax(bufferSize / sizeof(Vector3), reinterpret_cast<float*>(buffer.data()));

        // Calculate the difference with the original mesh, and translate to make all values positive.
        const Vector3* const deltasBuffer = reinterpret_cast<const Vector3* const>(buffer.data());

        for(uint32_t index = 0u; index < numberOfVertices; ++index)
        {
          Vector3& delta = geometryBufferV3[geometryBufferIndex++];
          delta          = deltasBuffer[index];

          delta.x *= 0.5f;
          delta.y *= 0.5f;
          delta.z *= 0.5f;

          delta.x += 0.5f;
          delta.y += 0.5f;
          delta.z += 0.5f;
        }
      }
    }

    if(blendShape.tangents.IsDefined())
    {
      DALI_ASSERT_ALWAYS(((blendShape.tangents.mBlob.mLength % sizeof(Vector3) == 0u) ||
                          blendShape.tangents.mBlob.mStride >= sizeof(Vector3)) &&
                         "Blend Shape tangents buffer length not a multiple of element size");

      const auto           bufferSize = blendShape.tangents.mBlob.GetBufferSize();
      std::vector<uint8_t> buffer(bufferSize);
      if(ReadAccessor(blendShape.tangents, binFile, buffer.data()))
      {
        blendShape.tangents.mBlob.ApplyMinMax(bufferSize / sizeof(Vector3), reinterpret_cast<float*>(buffer.data()));

        // Calculate the difference with the original mesh, and translate to make all values positive.
        const Vector3* const deltasBuffer = reinterpret_cast<const Vector3* const>(buffer.data());

        for(uint32_t index = 0u; index < numberOfVertices; ++index)
        {
          Vector3& delta = geometryBufferV3[geometryBufferIndex++];
          delta          = deltasBuffer[index];

          delta.x *= 0.5f;
          delta.y *= 0.5f;
          delta.z *= 0.5f;

          delta.x += 0.5f;
          delta.y += 0.5f;
          delta.z += 0.5f;
        }
      }
    }
  }

  geometryBufferIndex = 0u;
  for(const auto& blendShape : blendShapes)
  {
    // Normalize all the deltas and translate to a possitive value.
    // Deltas are going to be passed to the shader in a color texture
    // whose values that are less than zero are clamped.
    if(blendShape.deltas.IsDefined())
    {
      const float normalizeFactor = (fabsf(maxDistance) < Math::MACHINE_EPSILON_1000) ? 1.f : (0.5f / sqrtf(maxDistance));

      for(uint32_t index = 0u; index < numberOfVertices; ++index)
      {
        Vector3& delta = geometryBufferV3[geometryBufferIndex++];
        delta.x        = Clamp(((delta.x * normalizeFactor) + 0.5f), 0.f, 1.f);
        delta.y        = Clamp(((delta.y * normalizeFactor) + 0.5f), 0.f, 1.f);
        delta.z        = Clamp(((delta.z * normalizeFactor) + 0.5f), 0.f, 1.f);
      }

      // Calculate and store the unnormalize factor.
      blendShapeUnnormalizeFactor = 1.f / normalizeFactor;
    }

    if(blendShape.normals.IsDefined())
    {
      geometryBufferIndex += numberOfVertices;
    }

    if(blendShape.tangents.IsDefined())
    {
      geometryBufferIndex += numberOfVertices;
    }
  }
}

} // namespace

MeshDefinition::SparseBlob::SparseBlob(const Blob& indices, const Blob& values, uint32_t count)
: mIndices{indices},
  mValues{values},
  mCount{count}
{
}

MeshDefinition::Accessor::Accessor(const MeshDefinition::Blob&       blob,
                                   const MeshDefinition::SparseBlob& sparse)
: mBlob{blob},
  mSparse{(sparse.mIndices.IsDefined() && sparse.mValues.IsDefined()) ? new SparseBlob{sparse} : nullptr}
{
}

void MeshDefinition::Blob::ApplyMinMax(const std::vector<float>& min, const std::vector<float>& max, uint32_t count, float* values)
{
  DALI_ASSERT_DEBUG(max.size() == min.size() || max.size() * min.size() == 0);
  const auto numComponents = std::max(min.size(), max.size());

  using ClampFn   = void (*)(const float*, const float*, uint32_t, float&);
  ClampFn clampFn = min.empty() ? (max.empty() ? static_cast<ClampFn>(nullptr) : [](const float* min, const float* max, uint32_t i, float& value) {
    value = std::min(max[i], value);
  })
                                : (max.empty() ? [](const float* min, const float* max, uint32_t i, float& value) {
                                    value = std::max(min[i], value);
                                  }
                                               : [](const float* min, const float* max, uint32_t i, float& value) {
                                                   value = std::min(std::max(min[i], value), max[i]);
                                                 });

  auto end = values + count * numComponents;
  while(values != end)
  {
    auto     nextElement = values + numComponents;
    uint32_t i           = 0;
    while(values != nextElement)
    {
      clampFn(min.data(), max.data(), i, *values);
      ++values;
      ++i;
    }
  }
}

MeshDefinition::Blob::Blob(uint32_t offset, uint32_t length, uint16_t stride, uint16_t elementSizeHint, const std::vector<float>& min, const std::vector<float>& max)
: mOffset(offset),
  mLength(length),
  mStride(stride),
  mElementSizeHint(elementSizeHint),
  mMin(min),
  mMax(max)
{
}

uint32_t MeshDefinition::Blob::GetBufferSize() const
{
  return IsConsecutive() ? mLength : (mLength * mElementSizeHint / mStride);
}

void MeshDefinition::Blob::ApplyMinMax(uint32_t count, float* values) const
{
  ApplyMinMax(mMin, mMax, count, values);
}

void MeshDefinition::RawData::Attrib::AttachBuffer(Geometry& g) const
{
  Property::Map attribMap;
  attribMap[mName]          = mType;
  VertexBuffer attribBuffer = VertexBuffer::New(attribMap);
  attribBuffer.SetData(mData.data(), mNumElements);

  g.AddVertexBuffer(attribBuffer);
}

bool MeshDefinition::IsQuad() const
{
  return CaseInsensitiveStringCompare(QUAD, mUri);
}

bool MeshDefinition::IsSkinned() const
{
  return mJoints0.IsDefined() && mWeights0.IsDefined();
}

bool MeshDefinition::HasBlendShapes() const
{
  return !mBlendShapes.empty();
}

void MeshDefinition::RequestNormals()
{
  mNormals.mBlob.mLength = mPositions.mBlob.GetBufferSize();
}

void MeshDefinition::RequestTangents()
{
  mTangents.mBlob.mLength = mNormals.mBlob.GetBufferSize();
}

MeshDefinition::RawData
MeshDefinition::LoadRaw(const std::string& modelsPath) const
{
  RawData raw;
  if(IsQuad())
  {
    return raw;
  }

  const std::string meshPath = modelsPath + mUri;
  std::ifstream     binFile(meshPath, std::ios::binary);
  if(!binFile)
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to read geometry data from '" << meshPath << "'";
  }

  if(mIndices.IsDefined())
  {
    if(MaskMatch(mFlags, U32_INDICES))
    {
      DALI_ASSERT_ALWAYS(((mIndices.mBlob.mLength % sizeof(uint32_t) == 0) ||
                          mIndices.mBlob.mStride >= sizeof(uint32_t)) &&
                         "Index buffer length not a multiple of element size");
      const auto indexCount = mIndices.mBlob.GetBufferSize() / sizeof(uint32_t);
      raw.mIndices.resize(indexCount * 2); // NOTE: we need space for uint32_ts initially.
      if(!ReadAccessor(mIndices, binFile, reinterpret_cast<uint8_t*>(raw.mIndices.data())))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read indices from '" << meshPath << "'.";
      }

      auto u16s = raw.mIndices.data();
      auto u32s = reinterpret_cast<uint32_t*>(raw.mIndices.data());
      auto end  = u32s + indexCount;
      while(u32s != end)
      {
        *u16s = static_cast<uint16_t>(*u32s);
        ++u16s;
        ++u32s;
      }

      raw.mIndices.resize(indexCount);
    }
    else
    {
      DALI_ASSERT_ALWAYS(((mIndices.mBlob.mLength % sizeof(unsigned short) == 0) ||
                          mIndices.mBlob.mStride >= sizeof(unsigned short)) &&
                         "Index buffer length not a multiple of element size");
      raw.mIndices.resize(mIndices.mBlob.mLength / sizeof(unsigned short));
      if(!ReadAccessor(mIndices, binFile, reinterpret_cast<uint8_t*>(raw.mIndices.data())))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read indices from '" << meshPath << "'.";
      }
    }
  }

  std::vector<Vector3> positions;
  if(mPositions.IsDefined())
  {
    DALI_ASSERT_ALWAYS(((mPositions.mBlob.mLength % sizeof(Vector3) == 0) ||
                        mPositions.mBlob.mStride >= sizeof(Vector3)) &&
                       "Position buffer length not a multiple of element size");
    const auto           bufferSize = mPositions.mBlob.GetBufferSize();
    std::vector<uint8_t> buffer(bufferSize);
    if(!ReadAccessor(mPositions, binFile, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read positions from '" << meshPath << "'.";
    }

    uint32_t numVector3 = bufferSize / sizeof(Vector3);
    mPositions.mBlob.ApplyMinMax(numVector3, reinterpret_cast<float*>(buffer.data()));

    if(HasBlendShapes())
    {
      positions.resize(numVector3);
      std::copy(buffer.data(), buffer.data() + buffer.size(), reinterpret_cast<uint8_t*>(positions.data()));
    }

    raw.mAttribs.push_back({"aPosition", Property::VECTOR3, numVector3, std::move(buffer)});
  }

  const auto isTriangles = mPrimitiveType == Geometry::TRIANGLES;
  auto       hasNormals  = mNormals.IsDefined();
  if(hasNormals)
  {
    DALI_ASSERT_ALWAYS(((mNormals.mBlob.mLength % sizeof(Vector3) == 0) ||
                        mNormals.mBlob.mStride >= sizeof(Vector3)) &&
                       "Normal buffer length not a multiple of element size");
    const auto           bufferSize = mNormals.mBlob.GetBufferSize();
    std::vector<uint8_t> buffer(bufferSize);
    if(!ReadAccessor(mNormals, binFile, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read normals from '" << meshPath << "'.";
    }

    mNormals.mBlob.ApplyMinMax(bufferSize / sizeof(Vector3), reinterpret_cast<float*>(buffer.data()));

    raw.mAttribs.push_back({"aNormal", Property::VECTOR3, static_cast<uint32_t>(bufferSize / sizeof(Vector3)), std::move(buffer)});
  }
  else if(mNormals.mBlob.mLength != 0 && isTriangles)
  {
    DALI_ASSERT_DEBUG(mNormals.mBlob.mLength == mPositions.mBlob.GetBufferSize());
    GenerateNormals(raw);
    hasNormals = true;
  }

  const auto hasUvs = mTexCoords.IsDefined();
  if(hasUvs)
  {
    DALI_ASSERT_ALWAYS(((mTexCoords.mBlob.mLength % sizeof(Vector2) == 0) ||
                        mTexCoords.mBlob.mStride >= sizeof(Vector2)) &&
                       "Normal buffer length not a multiple of element size");
    const auto           bufferSize = mTexCoords.mBlob.GetBufferSize();
    std::vector<uint8_t> buffer(bufferSize);
    if(!ReadAccessor(mTexCoords, binFile, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read uv-s from '" << meshPath << "'.";
    }

    const auto uvCount = bufferSize / sizeof(Vector2);
    if(MaskMatch(mFlags, FLIP_UVS_VERTICAL))
    {
      auto uv    = reinterpret_cast<Vector2*>(buffer.data());
      auto uvEnd = uv + uvCount;
      while(uv != uvEnd)
      {
        uv->y = 1.0f - uv->y;
        ++uv;
      }
    }

    mTexCoords.mBlob.ApplyMinMax(bufferSize / sizeof(Vector2), reinterpret_cast<float*>(buffer.data()));

    raw.mAttribs.push_back({"aTexCoord", Property::VECTOR2, static_cast<uint32_t>(uvCount), std::move(buffer)});
  }

  if(mTangents.IsDefined())
  {
    DALI_ASSERT_ALWAYS(((mTangents.mBlob.mLength % sizeof(Vector3) == 0) ||
                        mTangents.mBlob.mStride >= sizeof(Vector3)) &&
                       "Tangents buffer length not a multiple of element size");
    const auto           bufferSize = mTangents.mBlob.GetBufferSize();
    std::vector<uint8_t> buffer(bufferSize);
    if(!ReadAccessor(mTangents, binFile, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read tangents from '" << meshPath << "'.";
    }

    mTangents.mBlob.ApplyMinMax(bufferSize / sizeof(Vector3), reinterpret_cast<float*>(buffer.data()));

    raw.mAttribs.push_back({"aTangent", Property::VECTOR3, static_cast<uint32_t>(bufferSize / sizeof(Vector3)), std::move(buffer)});
  }
  else if(mTangents.mBlob.mLength != 0 && hasNormals && isTriangles)
  {
    DALI_ASSERT_DEBUG(mTangents.mBlob.mLength == mNormals.mBlob.GetBufferSize());
    hasUvs ? GenerateTangentsWithUvs(raw) : GenerateTangents(raw);
  }

  if(IsSkinned())
  {
    if(MaskMatch(mFlags, U16_JOINT_IDS))
    {
      DALI_ASSERT_ALWAYS(((mJoints0.mBlob.mLength % sizeof(Uint16Vector4) == 0) ||
                          mJoints0.mBlob.mStride >= sizeof(Uint16Vector4)) &&
                         "Joints buffer length not a multiple of element size");
      const auto           inBufferSize = mJoints0.mBlob.GetBufferSize();
      std::vector<uint8_t> buffer(inBufferSize * 2);
      auto                 u16s = buffer.data() + inBufferSize;
      if(!ReadAccessor(mJoints0, binFile, u16s))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read joints from '" << meshPath << "'.";
      }

      auto floats = reinterpret_cast<float*>(buffer.data());
      auto end    = u16s + inBufferSize;
      while(u16s != end)
      {
        auto value = *reinterpret_cast<uint16_t*>(u16s);
        *floats    = static_cast<float>(value);

        u16s += sizeof(uint16_t);
        ++floats;
      }
      raw.mAttribs.push_back({"aJoints", Property::VECTOR4, static_cast<uint32_t>(buffer.size() / sizeof(Vector4)), std::move(buffer)});
    }
    else
    {
      DALI_ASSERT_ALWAYS(((mJoints0.mBlob.mLength % sizeof(Vector4) == 0) ||
                          mJoints0.mBlob.mStride >= sizeof(Vector4)) &&
                         "Joints buffer length not a multiple of element size");
      const auto           bufferSize = mJoints0.mBlob.GetBufferSize();
      std::vector<uint8_t> buffer(bufferSize);
      if(!ReadAccessor(mJoints0, binFile, buffer.data()))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read joints from '" << meshPath << "'.";
      }

      raw.mAttribs.push_back({"aJoints", Property::VECTOR4, static_cast<uint32_t>(bufferSize / sizeof(Vector4)), std::move(buffer)});
    }

    DALI_ASSERT_ALWAYS(((mWeights0.mBlob.mLength % sizeof(Vector4) == 0) ||
                        mWeights0.mBlob.mStride >= sizeof(Vector4)) &&
                       "Weights buffer length not a multiple of element size");
    const auto           bufferSize = mWeights0.mBlob.GetBufferSize();
    std::vector<uint8_t> buffer(bufferSize);
    if(!ReadAccessor(mWeights0, binFile, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read weights from '" << meshPath << "'.";
    }

    raw.mAttribs.push_back({"aWeights", Property::VECTOR4, static_cast<uint32_t>(bufferSize / sizeof(Vector4)), std::move(buffer)});
  }

  // Calculate the Blob for the blend shapes.
  Blob blendShapesBlob;
  blendShapesBlob.mOffset = std::numeric_limits<unsigned int>::max();
  blendShapesBlob.mLength = 0u;

  for(const auto& blendShape : mBlendShapes)
  {
    for(auto i : {&blendShape.deltas, &blendShape.normals, &blendShape.tangents})
    {
      if(i->IsDefined())
      {
        blendShapesBlob.mOffset = std::min(blendShapesBlob.mOffset, i->mBlob.mOffset);
        blendShapesBlob.mLength += i->mBlob.mLength;
      }
    }
  }

  if(HasBlendShapes())
  {
    const uint32_t numberOfVertices = mPositions.mBlob.mLength / sizeof(Vector3);

    // Calculate the size of one buffer inside the texture.
    raw.mBlendShapeBufferOffset = numberOfVertices;

    bool     calculateGltf2BlendShapes = false;
    uint32_t textureWidth              = 0u;
    uint32_t textureHeight             = 0u;

    if(!mBlendShapeHeader.IsDefined())
    {
      CalculateTextureSize(blendShapesBlob.mLength / sizeof(Vector3), textureWidth, textureHeight);
      calculateGltf2BlendShapes = true;
    }
    else
    {
      uint16_t header[2u];
      ReadBlob(mBlendShapeHeader, binFile, reinterpret_cast<uint8_t*>(header));
      textureWidth  = header[0u];
      textureHeight = header[1u];
    }

    const uint32_t numberOfBlendShapes = mBlendShapes.size();
    raw.mBlendShapeUnnormalizeFactor.Resize(numberOfBlendShapes);

    Devel::PixelBuffer geometryPixelBuffer = Devel::PixelBuffer::New(textureWidth, textureHeight, Pixel::RGB32F);
    uint8_t*           geometryBuffer      = geometryPixelBuffer.GetBuffer();

    if(calculateGltf2BlendShapes)
    {
      CalculateGltf2BlendShapes(geometryBuffer, binFile, mBlendShapes, numberOfVertices, raw.mBlendShapeUnnormalizeFactor[0u]);
    }
    else
    {
      Blob unnormalizeFactorBlob;
      unnormalizeFactorBlob.mLength = sizeof(float) * ((BlendShapes::Version::VERSION_2_0 == mBlendShapeVersion) ? 1u : numberOfBlendShapes);

      if(blendShapesBlob.IsDefined())
      {
        if(ReadBlob(blendShapesBlob, binFile, geometryBuffer))
        {
          unnormalizeFactorBlob.mOffset = blendShapesBlob.mOffset + blendShapesBlob.mLength;
        }
      }

      // Read the unnormalize factors.
      if(unnormalizeFactorBlob.IsDefined())
      {
        ReadBlob(unnormalizeFactorBlob, binFile, reinterpret_cast<uint8_t*>(&raw.mBlendShapeUnnormalizeFactor[0u]));
      }
    }
    raw.mBlendShapeData = Devel::PixelBuffer::Convert(geometryPixelBuffer);
  }

  return raw;
}

MeshGeometry MeshDefinition::Load(RawData&& raw) const
{
  MeshGeometry meshGeometry;
  meshGeometry.geometry = Geometry::New();
  meshGeometry.geometry.SetType(mPrimitiveType);

  if(IsQuad()) // TODO: do this in raw data; provide MakeTexturedQuadGeometry() that only creates buffers.
  {
    auto options          = MaskMatch(mFlags, FLIP_UVS_VERTICAL) ? TexturedQuadOptions::FLIP_VERTICAL : 0;
    meshGeometry.geometry = MakeTexturedQuadGeometry(options);
  }
  else
  {
    if(!raw.mIndices.empty())
    {
      meshGeometry.geometry.SetIndexBuffer(raw.mIndices.data(), raw.mIndices.size());
    }

    for(auto& a : raw.mAttribs)
    {
      a.AttachBuffer(meshGeometry.geometry);
    }

    if(HasBlendShapes())
    {
      meshGeometry.blendShapeBufferOffset      = raw.mBlendShapeBufferOffset;
      meshGeometry.blendShapeUnnormalizeFactor = std::move(raw.mBlendShapeUnnormalizeFactor);

      meshGeometry.blendShapeGeometry = Texture::New(TextureType::TEXTURE_2D,
                                                     raw.mBlendShapeData.GetPixelFormat(),
                                                     raw.mBlendShapeData.GetWidth(),
                                                     raw.mBlendShapeData.GetHeight());
      meshGeometry.blendShapeGeometry.Upload(raw.mBlendShapeData);
    }
  }

  return meshGeometry;
}

} // namespace SceneLoader
} // namespace Dali
