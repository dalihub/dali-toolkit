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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/mesh-definition.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-stream.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/compile-time-math.h>
#include <cstring>
#include <fstream>
#include <functional>
#include <type_traits>

namespace Dali::Scene3D::Loader
{
namespace
{
enum class LoadDataType
{
  UNSIGNED_BYTE = 0,
  UNSIGNED_SHORT,
  FLOAT
};

struct LoadAccessorInputs
{
  MeshDefinition::RawData&  rawData;
  MeshDefinition::Accessor& accessor;
  uint32_t                  flags;
  std::iostream*            fileStream;
  std::string&              meshPath;
  BufferDefinition::Vector& buffers;
};

struct LoadAccessorListInputs
{
  MeshDefinition::RawData&               rawData;
  std::vector<MeshDefinition::Accessor>& accessors;
  uint32_t                               flags;
  std::iostream*                         fileStream;
  std::string&                           meshPath;
  BufferDefinition::Vector&              buffers;
};

template<bool use32BitIndices>
class IndexProvider
{
public:
  using IndexType = typename std::conditional_t<use32BitIndices, uint32_t, uint16_t>;
  IndexProvider(const uint16_t* indices)
  : mData(reinterpret_cast<uintptr_t>(indices)),
    mFunc(indices ? IncrementPointer : Increment)
  {
  }

  IndexType operator()()
  {
    return mFunc(mData);
  }

private:
  static IndexType Increment(uintptr_t& data)
  {
    // mData was 'zero' at construct time. Just simply return counter start with 0.
    return static_cast<IndexType>(data++);
  }

  static IndexType IncrementPointer(uintptr_t& data)
  {
    auto iPtr   = reinterpret_cast<const IndexType*>(data);
    auto result = *iPtr;
    data        = reinterpret_cast<uintptr_t>(++iPtr);
    return result;
  }

  uintptr_t mData;
  IndexType (*mFunc)(uintptr_t&);
};

const char* QUAD("quad");

///@brief Reads a blob from the given stream @a source into @a target, which must have
/// at least @a descriptor.length bytes.
bool ReadBlob(const MeshDefinition::Blob& descriptor, std::istream& source, uint8_t* target)
{
  source.clear();
  if(!source.seekg(static_cast<std::streamoff>(static_cast<std::size_t>(descriptor.mOffset)), std::istream::beg))
  {
    return false;
  }

  if(descriptor.IsConsecutive())
  {
    return !!source.read(reinterpret_cast<char*>(target), static_cast<std::streamsize>(static_cast<size_t>(descriptor.mLength)));
  }
  else
  {
    if(descriptor.mStride > descriptor.mElementSizeHint)
    {
      const uint32_t diff      = descriptor.mStride - descriptor.mElementSizeHint;
      uint32_t       readSize  = 0;
      uint32_t       totalSize = (descriptor.mLength / descriptor.mElementSizeHint) * descriptor.mStride;
      while(readSize < totalSize &&
            source.read(reinterpret_cast<char*>(target), descriptor.mElementSizeHint))
      {
        readSize += descriptor.mStride;
        target += descriptor.mElementSizeHint;
        source.seekg(static_cast<std::streamoff>(static_cast<std::size_t>(diff)), std::istream::cur);
      }
      return readSize == totalSize;
    }
  }
  return false;
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

bool ReadAccessor(const MeshDefinition::Accessor& accessor, std::istream& source, uint8_t* target, std::vector<uint32_t>* sparseIndices)
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

    // If non-null sparse indices vector, prepare it for output
    if(sparseIndices)
    {
      sparseIndices->resize(accessor.mSparse->mCount);
    }

    switch(indices.mElementSizeHint)
    {
      case 1u:
      {
        ReadValues<uint8_t>(valuesBuffer, indicesBuffer, target, accessor.mSparse->mCount, values.mElementSizeHint);
        if(sparseIndices)
        {
          // convert 8-bit indices into 32-bit
          std::transform(indicesBuffer.begin(), indicesBuffer.end(), sparseIndices->begin(), [](const uint8_t& value) { return uint32_t(value); });
        }
        break;
      }
      case 2u:
      {
        ReadValues<uint16_t>(valuesBuffer, indicesBuffer, target, accessor.mSparse->mCount, values.mElementSizeHint);
        if(sparseIndices)
        {
          // convert 16-bit indices into 32-bit
          std::transform(reinterpret_cast<uint16_t*>(indicesBuffer.data()),
                         reinterpret_cast<uint16_t*>(indicesBuffer.data()) + accessor.mSparse->mCount,
                         sparseIndices->begin(),
                         [](const uint16_t& value) {
                           return uint32_t(value);
                         });
        }
        break;
      }
      case 4u:
      {
        ReadValues<uint32_t>(valuesBuffer, indicesBuffer, target, accessor.mSparse->mCount, values.mElementSizeHint);
        if(sparseIndices)
        {
          std::copy(indicesBuffer.begin(), indicesBuffer.end(), reinterpret_cast<uint8_t*>(sparseIndices->data()));
        }
        break;
      }
      default:
      {
        DALI_ASSERT_DEBUG(!"Unsupported type for an index");
      }
    }
  }

  return success;
}

bool ReadAccessor(const MeshDefinition::Accessor& accessor, std::istream& source, uint8_t* target)
{
  return ReadAccessor(accessor, source, target, nullptr);
}

template<typename T, bool needsNormalize>
void ReadVectorAccessor(const MeshDefinition::Accessor& accessor, std::istream& source, std::vector<uint8_t>& buffer)
{
  constexpr auto sizeofBlobUnit = sizeof(T) * 4;

  DALI_ASSERT_ALWAYS(((accessor.mBlob.mLength % sizeofBlobUnit == 0) ||
                      accessor.mBlob.mStride >= sizeofBlobUnit) &&
                     "Buffer length not a multiple of element size");
  const auto inBufferSize  = accessor.mBlob.GetBufferSize();
  const auto outBufferSize = (sizeof(Vector4) / sizeofBlobUnit) * inBufferSize;

  buffer.resize(outBufferSize);
  auto inBuffer = buffer.data() + outBufferSize - inBufferSize;
  if(!ReadAccessor(accessor, source, inBuffer))
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to read vector data from Accessor.";
  }

  if(sizeofBlobUnit != sizeof(Vector4))
  {
    auto       floats = reinterpret_cast<float*>(buffer.data());
    const auto end    = inBuffer + inBufferSize;
    while(inBuffer != end)
    {
      const auto value = *reinterpret_cast<T*>(inBuffer);
      *floats          = (needsNormalize) ? static_cast<float>(value) / static_cast<float>((1 << (sizeof(T) * 8)) - 1) : static_cast<float>(value);

      inBuffer += sizeof(T);
      ++floats;
    }
  }
}

template<bool needsNormalize>
void ReadTypedVectorAccessor(LoadDataType loadDataType, MeshDefinition::Accessor& accessor, std::iostream& stream, std::vector<uint8_t>& buffer)
{
  switch(loadDataType)
  {
    case LoadDataType::UNSIGNED_SHORT:
    {
      ReadVectorAccessor<uint16_t, needsNormalize>(accessor, stream, buffer);
      break;
    }
    case LoadDataType::UNSIGNED_BYTE:
    {
      ReadVectorAccessor<uint8_t, needsNormalize>(accessor, stream, buffer);
      break;
    }
    default:
    {
      ReadVectorAccessor<float, needsNormalize>(accessor, stream, buffer);
      break;
    }
  }
}

template<bool use32BitsIndices, typename IndexProviderType = IndexProvider<use32BitsIndices>>
bool GenerateNormals(MeshDefinition::RawData& raw)
{
  using IndexType = typename IndexProviderType::IndexType;

  // mIndicies size must be even if we use 32bit indices.
  if(DALI_UNLIKELY(use32BitsIndices && !raw.mIndices.empty() && !(raw.mIndices.size() % (sizeof(IndexType) / sizeof(uint16_t)) == 0)))
  {
    return false;
  }

  auto& attribs = raw.mAttribs;
  DALI_ASSERT_DEBUG(attribs.size() > 0); // positions

  IndexProviderType getIndex(raw.mIndices.data());

  const uint32_t numIndices = raw.mIndices.empty() ? attribs[0].mNumElements : static_cast<uint32_t>(raw.mIndices.size() / (sizeof(IndexType) / sizeof(uint16_t)));

  auto* positions = reinterpret_cast<const Vector3*>(attribs[0].mData.data());

  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(Vector3));
  auto                 normals = reinterpret_cast<Vector3*>(buffer.data());

  for(uint32_t i = 0; i < numIndices; i += 3)
  {
    IndexType indices[]{getIndex(), getIndex(), getIndex()};
    Vector3   pos[]{positions[indices[0]], positions[indices[1]], positions[indices[2]]};

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

  return true;
}

template<bool use32BitsIndices, bool useVec3, bool hasUvs, typename T = std::conditional_t<useVec3, Vector3, Vector4>, typename = std::enable_if_t<(std::is_same<T, Vector3>::value || std::is_same<T, Vector4>::value)>, typename IndexProviderType = IndexProvider<use32BitsIndices>>
bool GenerateTangents(MeshDefinition::RawData& raw)
{
  using IndexType = typename IndexProviderType::IndexType;

  // mIndicies size must be even if we use 32bit indices.
  if(DALI_UNLIKELY(use32BitsIndices && !raw.mIndices.empty() && !(raw.mIndices.size() % (sizeof(IndexType) / sizeof(uint16_t)) == 0)))
  {
    return false;
  }

  auto& attribs = raw.mAttribs;
  // Required positions, normals, uvs (if we have). If not, skip generation
  if(DALI_UNLIKELY(attribs.size() < (2 + static_cast<size_t>(hasUvs))))
  {
    return false;
  }

  std::vector<uint8_t> buffer(attribs[0].mNumElements * sizeof(T));
  auto                 tangents = reinterpret_cast<T*>(buffer.data());

  if constexpr(hasUvs)
  {
    IndexProviderType getIndex(raw.mIndices.data());

    const uint32_t numIndices = raw.mIndices.empty() ? attribs[0].mNumElements : static_cast<uint32_t>(raw.mIndices.size() / (sizeof(IndexType) / sizeof(uint16_t)));

    auto* positions = reinterpret_cast<const Vector3*>(attribs[0].mData.data());
    auto* uvs       = reinterpret_cast<const Vector2*>(attribs[2].mData.data());

    for(uint32_t i = 0; i < numIndices; i += 3)
    {
      IndexType indices[]{getIndex(), getIndex(), getIndex()};
      Vector3   pos[]{positions[indices[0]], positions[indices[1]], positions[indices[2]]};
      Vector2   uv[]{uvs[indices[0]], uvs[indices[1]], uvs[indices[2]]};

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

      float   det = (s0 * t1 - t0 * s1);
      float   r   = 1.f / ((std::abs(det) < Dali::Epsilon<1000>::value) ? (Dali::Epsilon<1000>::value * (det > 0.0f ? 1.f : -1.f)) : det);
      Vector3 tangent((x0 * t1 - t0 * x1) * r, (y0 * t1 - t0 * y1) * r, (z0 * t1 - t0 * z1) * r);
      tangents[indices[0]] += T(tangent);
      tangents[indices[1]] += T(tangent);
      tangents[indices[2]] += T(tangent);
    }
  }

  auto* normals = reinterpret_cast<const Vector3*>(attribs[1].mData.data());
  auto  iEnd    = normals + attribs[1].mNumElements;
  while(normals != iEnd)
  {
    Vector3 tangentVec3;
    if constexpr(hasUvs)
    {
      // Calculated by indexs
      tangentVec3 = Vector3((*tangents).x, (*tangents).y, (*tangents).z);
    }
    else
    {
      // Only choiced by normal vector. by indexs
      Vector3 t[]{normals->Cross(Vector3::XAXIS), normals->Cross(Vector3::YAXIS)};
      tangentVec3 = t[t[1].LengthSquared() > t[0].LengthSquared()];
    }

    tangentVec3 -= *normals * normals->Dot(tangentVec3);
    tangentVec3.Normalize();
    if constexpr(useVec3)
    {
      *tangents = tangentVec3;
    }
    else
    {
      *tangents = Vector4(tangentVec3.x, tangentVec3.y, tangentVec3.z, 1.0f);
    }

    ++tangents;
    ++normals;
  }
  attribs.push_back({"aTangent", useVec3 ? Property::VECTOR3 : Property::VECTOR4, attribs[0].mNumElements, std::move(buffer)});

  return true;
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

template<typename T>
float GetNormalizedScale()
{
  return 1.0f / (std::numeric_limits<T>::max());
}

template<typename T>
void DequantizeData(std::vector<uint8_t>& buffer, float* dequantizedValues, uint32_t numValues, bool normalized)
{
  // see https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_mesh_quantization#encoding-quantized-data

  T* values = reinterpret_cast<T*>(buffer.data());

  for(uint32_t i = 0; i < numValues; ++i)
  {
    *dequantizedValues = normalized ? std::max((*values) * GetNormalizedScale<T>(), -1.0f) : *values;

    values++;
    dequantizedValues++;
  }
}

void GetDequantizedData(std::vector<uint8_t>& buffer, uint32_t numComponents, uint32_t count, uint32_t flags, bool normalized)
{
  bool dequantized = false;

  std::vector<uint8_t> dequantizedBuffer(count * numComponents * sizeof(float));
  float*               dequantizedValues = reinterpret_cast<float*>(dequantizedBuffer.data());

  if(MaskMatch(flags, MeshDefinition::Flags::S8_POSITION) || MaskMatch(flags, MeshDefinition::Flags::S8_NORMAL) || MaskMatch(flags, MeshDefinition::Flags::S8_TANGENT) || MaskMatch(flags, MeshDefinition::Flags::S8_TEXCOORD))
  {
    DequantizeData<int8_t>(buffer, dequantizedValues, numComponents * count, normalized);
    dequantized = true;
  }
  else if(MaskMatch(flags, MeshDefinition::Flags::U8_POSITION) || MaskMatch(flags, MeshDefinition::Flags::U8_TEXCOORD))
  {
    DequantizeData<uint8_t>(buffer, dequantizedValues, numComponents * count, normalized);
    dequantized = true;
  }
  else if(MaskMatch(flags, MeshDefinition::Flags::S16_POSITION) || MaskMatch(flags, MeshDefinition::Flags::S16_NORMAL) || MaskMatch(flags, MeshDefinition::Flags::S16_TANGENT) || MaskMatch(flags, MeshDefinition::Flags::S16_TEXCOORD))
  {
    DequantizeData<int16_t>(buffer, dequantizedValues, numComponents * count, normalized);
    dequantized = true;
  }
  else if(MaskMatch(flags, MeshDefinition::Flags::U16_POSITION) || MaskMatch(flags, MeshDefinition::Flags::U16_TEXCOORD))
  {
    DequantizeData<uint16_t>(buffer, dequantizedValues, numComponents * count, normalized);
    dequantized = true;
  }

  if(dequantized)
  {
    buffer = std::move(dequantizedBuffer);
  }
}

void GetDequantizedMinMax(std::vector<float>& min, std::vector<float>& max, uint32_t flags)
{
  float scale = 1.0f;

  if(MaskMatch(flags, MeshDefinition::Flags::S8_POSITION) || MaskMatch(flags, MeshDefinition::Flags::S8_NORMAL) || MaskMatch(flags, MeshDefinition::Flags::S8_TANGENT) || MaskMatch(flags, MeshDefinition::Flags::S8_TEXCOORD))
  {
    scale = GetNormalizedScale<int8_t>();
  }
  else if(MaskMatch(flags, MeshDefinition::Flags::U8_POSITION) || MaskMatch(flags, MeshDefinition::Flags::U8_TEXCOORD))
  {
    scale = GetNormalizedScale<uint8_t>();
  }
  else if(MaskMatch(flags, MeshDefinition::Flags::S16_POSITION) || MaskMatch(flags, MeshDefinition::Flags::S16_NORMAL) || MaskMatch(flags, MeshDefinition::Flags::S16_TANGENT) || MaskMatch(flags, MeshDefinition::Flags::S16_TEXCOORD))
  {
    scale = GetNormalizedScale<int16_t>();
  }
  else if(MaskMatch(flags, MeshDefinition::Flags::U16_POSITION) || MaskMatch(flags, MeshDefinition::Flags::U16_TEXCOORD))
  {
    scale = GetNormalizedScale<uint16_t>();
  }

  if(scale != 1.0f)
  {
    for(float& value : min)
    {
      value = std::max(value * scale, -1.0f);
    }

    for(float& value : max)
    {
      value = std::min(value * scale, 1.0f);
    }
  }
}

void CalculateGltf2BlendShapes(uint8_t* geometryBuffer, std::vector<MeshDefinition::BlendShape>& blendShapes, uint32_t numberOfVertices, float& blendShapeUnnormalizeFactor, BufferDefinition::Vector& buffers)
{
  uint32_t geometryBufferIndex = 0u;
  float    maxDistanceSquared  = 0.f;
  Vector3* geometryBufferV3    = reinterpret_cast<Vector3*>(geometryBuffer);
  for(auto& blendShape : blendShapes)
  {
    if(blendShape.deltas.IsDefined())
    {
      const auto bufferSize = blendShape.deltas.mBlob.GetBufferSize();
      uint32_t   numVector3;

      if(MaskMatch(blendShape.mFlags, MeshDefinition::S8_POSITION))
      {
        DALI_ASSERT_ALWAYS(((blendShape.deltas.mBlob.mLength % (sizeof(uint8_t) * 3) == 0) ||
                            blendShape.deltas.mBlob.mStride >= (sizeof(uint8_t) * 3)) &&
                           "Blend Shape position buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(uint8_t) * 3));
      }
      else if(MaskMatch(blendShape.mFlags, MeshDefinition::S16_POSITION))
      {
        DALI_ASSERT_ALWAYS(((blendShape.deltas.mBlob.mLength % (sizeof(uint16_t) * 3) == 0) ||
                            blendShape.deltas.mBlob.mStride >= (sizeof(uint16_t) * 3)) &&
                           "Blend Shape position buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(uint16_t) * 3));
      }
      else
      {
        DALI_ASSERT_ALWAYS(((blendShape.deltas.mBlob.mLength % sizeof(Vector3) == 0) ||
                            blendShape.deltas.mBlob.mStride >= sizeof(Vector3)) &&
                           "Blend Shape position buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / sizeof(Vector3));
      }

      std::vector<uint8_t>  buffer(bufferSize);
      std::vector<uint32_t> sparseIndices{};

      if(ReadAccessor(blendShape.deltas, buffers[blendShape.deltas.mBufferIdx].GetBufferStream(), buffer.data(), &sparseIndices))
      {
        GetDequantizedData(buffer, 3u, numVector3, blendShape.mFlags & MeshDefinition::POSITIONS_MASK, blendShape.deltas.mNormalized);

        if(blendShape.deltas.mNormalized)
        {
          GetDequantizedMinMax(blendShape.deltas.mBlob.mMin, blendShape.deltas.mBlob.mMax, blendShape.mFlags & MeshDefinition::POSITIONS_MASK);
        }

        blendShape.deltas.mBlob.ApplyMinMax(numVector3, reinterpret_cast<float*>(buffer.data()), &sparseIndices);

        // Calculate the difference with the original mesh.
        // Find the max distance to normalize the deltas.
        const auto* const deltasBuffer = reinterpret_cast<const Vector3* const>(buffer.data());

        auto ProcessVertex = [&geometryBufferV3, &deltasBuffer, &maxDistanceSquared](uint32_t geometryBufferIndex, uint32_t deltaIndex) {
          Vector3& delta = geometryBufferV3[geometryBufferIndex] = deltasBuffer[deltaIndex];
          delta                                                  = deltasBuffer[deltaIndex];
          return std::max(maxDistanceSquared, delta.LengthSquared());
        };

        if(sparseIndices.empty())
        {
          for(uint32_t index = 0u; index < numberOfVertices; ++index)
          {
            maxDistanceSquared = ProcessVertex(geometryBufferIndex++, index);
          }
        }
        else
        {
          // initialize blendshape texture
          // TODO: there may be a case when sparse accessor uses a base buffer view for initial values.
          std::fill(geometryBufferV3 + geometryBufferIndex, geometryBufferV3 + geometryBufferIndex + numberOfVertices, Vector3::ZERO);
          for(auto index : sparseIndices)
          {
            maxDistanceSquared = ProcessVertex(geometryBufferIndex + index, index);
          }
          geometryBufferIndex += numberOfVertices;
        }
      }
    }

    if(blendShape.normals.IsDefined())
    {
      const auto bufferSize = blendShape.normals.mBlob.GetBufferSize();
      uint32_t   numVector3;

      if(MaskMatch(blendShape.mFlags, MeshDefinition::S8_NORMAL))
      {
        DALI_ASSERT_ALWAYS(((blendShape.normals.mBlob.mLength % (sizeof(int8_t) * 3) == 0) ||
                            blendShape.normals.mBlob.mStride >= (sizeof(int8_t) * 3)) &&
                           "Blend Shape normals buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(int8_t) * 3));
      }
      else if(MaskMatch(blendShape.mFlags, MeshDefinition::S16_NORMAL))
      {
        DALI_ASSERT_ALWAYS(((blendShape.normals.mBlob.mLength % (sizeof(int16_t) * 3) == 0) ||
                            blendShape.normals.mBlob.mStride >= (sizeof(int16_t) * 3)) &&
                           "Blend Shape normals buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(int16_t) * 3));
      }
      else
      {
        DALI_ASSERT_ALWAYS(((blendShape.normals.mBlob.mLength % sizeof(Vector3) == 0) ||
                            blendShape.normals.mBlob.mStride >= sizeof(Vector3)) &&
                           "Blend Shape normals buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / sizeof(Vector3));
      }

      std::vector<uint8_t>  buffer(bufferSize);
      std::vector<uint32_t> sparseIndices;

      if(ReadAccessor(blendShape.normals, buffers[blendShape.normals.mBufferIdx].GetBufferStream(), buffer.data(), &sparseIndices))
      {
        GetDequantizedData(buffer, 3u, numVector3, blendShape.mFlags & MeshDefinition::NORMALS_MASK, blendShape.normals.mNormalized);

        if(blendShape.normals.mNormalized)
        {
          GetDequantizedMinMax(blendShape.normals.mBlob.mMin, blendShape.normals.mBlob.mMax, blendShape.mFlags & MeshDefinition::NORMALS_MASK);
        }

        blendShape.normals.mBlob.ApplyMinMax(numVector3, reinterpret_cast<float*>(buffer.data()), &sparseIndices);

        // Calculate the difference with the original mesh, and translate to make all values positive.
        const Vector3* const deltasBuffer  = reinterpret_cast<const Vector3* const>(buffer.data());
        auto                 ProcessVertex = [&geometryBufferV3, &deltasBuffer, &maxDistanceSquared](uint32_t geometryBufferIndex, uint32_t deltaIndex) {
          Vector3& delta = geometryBufferV3[geometryBufferIndex] = deltasBuffer[deltaIndex];
          delta.x *= 0.5f;
          delta.y *= 0.5f;
          delta.z *= 0.5f;

          delta.x += 0.5f;
          delta.y += 0.5f;
          delta.z += 0.5f;
        };

        if(sparseIndices.empty())
        {
          for(uint32_t index = 0u; index < numberOfVertices; ++index)
          {
            ProcessVertex(geometryBufferIndex++, index);
          }
        }
        else
        {
          std::fill(geometryBufferV3 + geometryBufferIndex, geometryBufferV3 + geometryBufferIndex + numberOfVertices, Vector3(0.5, 0.5, 0.5));
          for(auto index : sparseIndices)
          {
            ProcessVertex(geometryBufferIndex + index, index);
          }
          geometryBufferIndex += numberOfVertices;
        }
      }
    }

    if(blendShape.tangents.IsDefined())
    {
      const auto bufferSize = blendShape.tangents.mBlob.GetBufferSize();

      uint32_t numVector3;

      if(MaskMatch(blendShape.mFlags, MeshDefinition::S8_TANGENT))
      {
        DALI_ASSERT_ALWAYS(((blendShape.tangents.mBlob.mLength % (sizeof(int8_t) * 3) == 0) ||
                            blendShape.tangents.mBlob.mStride >= (sizeof(int8_t) * 3)) &&
                           "Blend Shape tangents buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(int8_t) * 3));
      }
      else if(MaskMatch(blendShape.mFlags, MeshDefinition::S16_TANGENT))
      {
        DALI_ASSERT_ALWAYS(((blendShape.tangents.mBlob.mLength % (sizeof(int16_t) * 3) == 0) ||
                            blendShape.tangents.mBlob.mStride >= (sizeof(int16_t) * 3)) &&
                           "Blend Shape tangents buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(int16_t) * 3));
      }
      else
      {
        DALI_ASSERT_ALWAYS(((blendShape.tangents.mBlob.mLength % sizeof(Vector3) == 0) ||
                            blendShape.tangents.mBlob.mStride >= sizeof(Vector3)) &&
                           "Blend Shape tangents buffer length not a multiple of element size");
        numVector3 = static_cast<uint32_t>(bufferSize / sizeof(Vector3));
      }

      std::vector<uint8_t>  buffer(bufferSize);
      std::vector<uint32_t> sparseIndices;

      if(ReadAccessor(blendShape.tangents, buffers[blendShape.tangents.mBufferIdx].GetBufferStream(), buffer.data(), &sparseIndices))
      {
        GetDequantizedData(buffer, 3u, numVector3, blendShape.mFlags & MeshDefinition::TANGENTS_MASK, blendShape.tangents.mNormalized);

        if(blendShape.tangents.mNormalized)
        {
          GetDequantizedMinMax(blendShape.tangents.mBlob.mMin, blendShape.tangents.mBlob.mMax, blendShape.mFlags & MeshDefinition::TANGENTS_MASK);
        }

        blendShape.tangents.mBlob.ApplyMinMax(numVector3, reinterpret_cast<float*>(buffer.data()), &sparseIndices);

        // Calculate the difference with the original mesh, and translate to make all values positive.
        const Vector3* const deltasBuffer  = reinterpret_cast<const Vector3* const>(buffer.data());
        auto                 ProcessVertex = [&geometryBufferV3, &deltasBuffer, &maxDistanceSquared](uint32_t geometryBufferIndex, uint32_t deltaIndex) {
          Vector3& delta = geometryBufferV3[geometryBufferIndex] = deltasBuffer[deltaIndex];
          delta.x *= 0.5f;
          delta.y *= 0.5f;
          delta.z *= 0.5f;

          delta.x += 0.5f;
          delta.y += 0.5f;
          delta.z += 0.5f;
        };

        if(sparseIndices.empty())
        {
          for(uint32_t index = 0u; index < numberOfVertices; ++index)
          {
            ProcessVertex(geometryBufferIndex++, index);
          }
        }
        else
        {
          std::fill(geometryBufferV3 + geometryBufferIndex, geometryBufferV3 + geometryBufferIndex + numberOfVertices, Vector3(0.5, 0.5, 0.5));
          for(auto index : sparseIndices)
          {
            ProcessVertex(geometryBufferIndex + index, index);
          }
          geometryBufferIndex += numberOfVertices;
        }
      }
    }
  }

  geometryBufferIndex = 0u;

  const float maxDistance = sqrtf(maxDistanceSquared);

  const float normalizeFactor = (maxDistanceSquared < Math::MACHINE_EPSILON_100) ? 1.f : (0.5f / maxDistance);

  // Calculate and store the unnormalize factor.
  blendShapeUnnormalizeFactor = maxDistance * 2.0f;

  for(const auto& blendShape : blendShapes)
  {
    // Normalize all the deltas and translate to a possitive value.
    // Deltas are going to be passed to the shader in a color texture
    // whose values that are less than zero are clamped.
    if(blendShape.deltas.IsDefined())
    {
      for(uint32_t index = 0u; index < numberOfVertices; ++index)
      {
        Vector3& delta = geometryBufferV3[geometryBufferIndex++];
        delta.x        = Clamp(((delta.x * normalizeFactor) + 0.5f), 0.f, 1.f);
        delta.y        = Clamp(((delta.y * normalizeFactor) + 0.5f), 0.f, 1.f);
        delta.z        = Clamp(((delta.z * normalizeFactor) + 0.5f), 0.f, 1.f);
      }
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

std::iostream& GetAvailableData(std::iostream* meshStream, const std::string& meshPath, BufferDefinition& buffer, std::string& availablePath)
{
  auto& stream  = meshStream ? *meshStream : buffer.GetBufferStream();
  availablePath = meshStream ? meshPath : buffer.GetUri();
  return stream;
}

template<bool needsNormalize>
void ReadTypedVectorAccessors(LoadAccessorListInputs loadAccessorListInputs, LoadDataType loadDataType, std::string attributeName)
{
  int setIndex = 0;
  for(auto& accessor : loadAccessorListInputs.accessors)
  {
    std::string        pathJoint;
    auto&              dataStream = GetAvailableData(loadAccessorListInputs.fileStream, loadAccessorListInputs.meshPath, loadAccessorListInputs.buffers[accessor.mBufferIdx], pathJoint);
    std::ostringstream name;
    name << attributeName << setIndex++;
    std::vector<uint8_t> buffer;
    ReadTypedVectorAccessor<needsNormalize>(loadDataType, accessor, dataStream, buffer);
    loadAccessorListInputs.rawData.mAttribs.push_back({name.str(), Property::VECTOR4, static_cast<uint32_t>(buffer.size() / sizeof(Vector4)), std::move(buffer)});
  }
}

void LoadIndices(LoadAccessorInputs indicesInput)
{
  if(indicesInput.accessor.IsDefined())
  {
    if(MaskMatch(indicesInput.flags, MeshDefinition::Flags::U32_INDICES))
    {
      DALI_ASSERT_ALWAYS(((indicesInput.accessor.mBlob.mLength % sizeof(uint32_t) == 0) ||
                          indicesInput.accessor.mBlob.mStride >= sizeof(uint32_t)) &&
                         "Index buffer length not a multiple of element size");
      const auto indexCount = indicesInput.accessor.mBlob.GetBufferSize() / sizeof(uint32_t);
      indicesInput.rawData.mIndices.resize(indexCount * 2); // NOTE: we need space for uint32_ts initially.

      std::string path;
      auto&       stream = GetAvailableData(indicesInput.fileStream, indicesInput.meshPath, indicesInput.buffers[indicesInput.accessor.mBufferIdx], path);
      if(!ReadAccessor(indicesInput.accessor, stream, reinterpret_cast<uint8_t*>(indicesInput.rawData.mIndices.data())))
      {
        DALI_LOG_ERROR("Failed to read indices from %s\n", path.c_str());
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read indices from '" << path << "'.";
      }
    }
    else if(MaskMatch(indicesInput.flags, MeshDefinition::Flags::U8_INDICES))
    {
      DALI_ASSERT_ALWAYS(((indicesInput.accessor.mBlob.mLength % sizeof(uint8_t) == 0) ||
                          indicesInput.accessor.mBlob.mStride >= sizeof(uint8_t)) &&
                         "Index buffer length not a multiple of element size");
      const auto indexCount = indicesInput.accessor.mBlob.GetBufferSize() / sizeof(uint8_t);
      indicesInput.rawData.mIndices.resize(indexCount); // NOTE: we need space for uint16_ts initially.

      std::string path;
      auto        u8s    = reinterpret_cast<uint8_t*>(indicesInput.rawData.mIndices.data()) + indexCount;
      auto&       stream = GetAvailableData(indicesInput.fileStream, indicesInput.meshPath, indicesInput.buffers[indicesInput.accessor.mBufferIdx], path);
      if(!ReadAccessor(indicesInput.accessor, stream, u8s))
      {
        DALI_LOG_ERROR("Failed to read indices from %s\n", path.c_str());
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read indices from '" << path << "'.";
      }

      auto u16s = indicesInput.rawData.mIndices.data();
      auto end  = u8s + indexCount;
      while(u8s != end)
      {
        *u16s = static_cast<uint16_t>(*u8s);
        ++u16s;
        ++u8s;
      }
    }
    else
    {
      DALI_ASSERT_ALWAYS(((indicesInput.accessor.mBlob.mLength % sizeof(unsigned short) == 0) ||
                          indicesInput.accessor.mBlob.mStride >= sizeof(unsigned short)) &&
                         "Index buffer length not a multiple of element size");
      indicesInput.rawData.mIndices.resize(indicesInput.accessor.mBlob.mLength / sizeof(unsigned short));

      std::string path;
      auto&       stream = GetAvailableData(indicesInput.fileStream, indicesInput.meshPath, indicesInput.buffers[indicesInput.accessor.mBufferIdx], path);
      if(!ReadAccessor(indicesInput.accessor, stream, reinterpret_cast<uint8_t*>(indicesInput.rawData.mIndices.data())))
      {
        DALI_LOG_ERROR("Failed to read indices from %s\n", path.c_str());
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read indicesInput.accessor from '" << path << "'.";
      }
    }
  }
}

uint32_t LoadPositions(LoadAccessorInputs positionsInput, bool hasBlendShape)
{
  uint32_t             numVector3 = 0u;
  std::vector<Vector3> positions;
  if(positionsInput.accessor.IsDefined())
  {
    const auto bufferSize = positionsInput.accessor.mBlob.GetBufferSize();

    if(MaskMatch(positionsInput.flags, MeshDefinition::Flags::S8_POSITION) || MaskMatch(positionsInput.flags, MeshDefinition::Flags::U8_POSITION))
    {
      DALI_ASSERT_ALWAYS(((positionsInput.accessor.mBlob.mLength % (sizeof(uint8_t) * 3) == 0) ||
                          positionsInput.accessor.mBlob.mStride >= (sizeof(uint8_t) * 3)) &&
                         "Position buffer length not a multiple of element size");
      numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(uint8_t) * 3));
    }
    else if(MaskMatch(positionsInput.flags, MeshDefinition::Flags::S16_POSITION) || MaskMatch(positionsInput.flags, MeshDefinition::Flags::U16_POSITION))
    {
      DALI_ASSERT_ALWAYS(((positionsInput.accessor.mBlob.mLength % (sizeof(uint16_t) * 3) == 0) ||
                          positionsInput.accessor.mBlob.mStride >= (sizeof(uint16_t) * 3)) &&
                         "Position buffer length not a multiple of element size");
      numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(uint16_t) * 3));
    }
    else
    {
      DALI_ASSERT_ALWAYS(((positionsInput.accessor.mBlob.mLength % sizeof(Vector3) == 0) ||
                          positionsInput.accessor.mBlob.mStride >= sizeof(Vector3)) &&
                         "Position buffer length not a multiple of element size");
      numVector3 = static_cast<uint32_t>(bufferSize / sizeof(Vector3));
    }

    std::vector<uint8_t> buffer(bufferSize);

    std::string path;
    auto&       stream = GetAvailableData(positionsInput.fileStream, positionsInput.meshPath, positionsInput.buffers[positionsInput.accessor.mBufferIdx], path);
    if(!ReadAccessor(positionsInput.accessor, stream, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read positions from '" << path << "'.";
    }

    GetDequantizedData(buffer, 3u, numVector3, positionsInput.flags & MeshDefinition::FlagMasks::POSITIONS_MASK, positionsInput.accessor.mNormalized);

    if(positionsInput.accessor.mNormalized)
    {
      GetDequantizedMinMax(positionsInput.accessor.mBlob.mMin, positionsInput.accessor.mBlob.mMax, positionsInput.flags & MeshDefinition::FlagMasks::POSITIONS_MASK);
    }

    if(positionsInput.accessor.mBlob.mMin.size() != 3u || positionsInput.accessor.mBlob.mMax.size() != 3u)
    {
      positionsInput.accessor.mBlob.ComputeMinMax(3u, numVector3, reinterpret_cast<float*>(buffer.data()));
    }
    else
    {
      positionsInput.accessor.mBlob.ApplyMinMax(numVector3, reinterpret_cast<float*>(buffer.data()));
    }

    if(hasBlendShape)
    {
      positions.resize(numVector3);
      std::copy(buffer.data(), buffer.data() + buffer.size(), reinterpret_cast<uint8_t*>(positions.data()));
    }

    positionsInput.rawData.mAttribs.push_back({"aPosition", Property::VECTOR3, numVector3, std::move(buffer)});
  }
  return numVector3;
}

bool LoadNormals(LoadAccessorInputs normalsInput, bool isTriangles, uint32_t positionBufferSize)
{
  auto hasNormals = normalsInput.accessor.IsDefined();
  if(hasNormals)
  {
    const auto bufferSize = normalsInput.accessor.mBlob.GetBufferSize();
    uint32_t   numVector3;

    if(MaskMatch(normalsInput.flags, MeshDefinition::Flags::S8_NORMAL))
    {
      DALI_ASSERT_ALWAYS(((normalsInput.accessor.mBlob.mLength % (sizeof(int8_t) * 3) == 0) ||
                          normalsInput.accessor.mBlob.mStride >= (sizeof(int8_t) * 3)) &&
                         "Normal buffer length not a multiple of element size");
      numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(int8_t) * 3));
    }
    else if(MaskMatch(normalsInput.flags, MeshDefinition::Flags::S16_NORMAL))
    {
      DALI_ASSERT_ALWAYS(((normalsInput.accessor.mBlob.mLength % (sizeof(int16_t) * 3) == 0) ||
                          normalsInput.accessor.mBlob.mStride >= (sizeof(int16_t) * 3)) &&
                         "Normal buffer length not a multiple of element size");
      numVector3 = static_cast<uint32_t>(bufferSize / (sizeof(int16_t) * 3));
    }
    else
    {
      DALI_ASSERT_ALWAYS(((normalsInput.accessor.mBlob.mLength % sizeof(Vector3) == 0) ||
                          normalsInput.accessor.mBlob.mStride >= sizeof(Vector3)) &&
                         "Normal buffer length not a multiple of element size");
      numVector3 = static_cast<uint32_t>(bufferSize / sizeof(Vector3));
    }

    std::vector<uint8_t> buffer(bufferSize);

    std::string path;
    auto&       stream = GetAvailableData(normalsInput.fileStream, normalsInput.meshPath, normalsInput.buffers[normalsInput.accessor.mBufferIdx], path);
    if(!ReadAccessor(normalsInput.accessor, stream, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read normals from '" << path << "'.";
    }

    GetDequantizedData(buffer, 3u, numVector3, normalsInput.flags & MeshDefinition::FlagMasks::NORMALS_MASK, normalsInput.accessor.mNormalized);

    if(normalsInput.accessor.mNormalized)
    {
      GetDequantizedMinMax(normalsInput.accessor.mBlob.mMin, normalsInput.accessor.mBlob.mMax, normalsInput.flags & MeshDefinition::FlagMasks::NORMALS_MASK);
    }

    normalsInput.accessor.mBlob.ApplyMinMax(numVector3, reinterpret_cast<float*>(buffer.data()));

    normalsInput.rawData.mAttribs.push_back({"aNormal", Property::VECTOR3, numVector3, std::move(buffer)});
  }
  else if(normalsInput.accessor.mBlob.mLength != 0 && isTriangles)
  {
    DALI_ASSERT_DEBUG(normalsInput.accessor.mBlob.mLength == positionBufferSize);
    static const std::function<bool(MeshDefinition::RawData&)> GenerateNormalsFunction[2] =
      {
        GenerateNormals<false>,
        GenerateNormals<true>,
      };
    const bool generateSuccessed = GenerateNormalsFunction[MaskMatch(normalsInput.flags, MeshDefinition::Flags::U32_INDICES)](normalsInput.rawData);
    if(!generateSuccessed)
    {
      DALI_LOG_ERROR("Failed to generate normal\n");
    }
    else
    {
      hasNormals = true;
    }
  }
  return hasNormals;
}

void LoadTextureCoordinates(LoadAccessorListInputs textureCoordinatesInput)
{
  if(!textureCoordinatesInput.accessors.empty() && textureCoordinatesInput.accessors[0].IsDefined())
  {
    auto&      texCoords  = textureCoordinatesInput.accessors[0];
    const auto bufferSize = texCoords.mBlob.GetBufferSize();
    uint32_t   uvCount;

    if(MaskMatch(textureCoordinatesInput.flags, MeshDefinition::Flags::S8_TEXCOORD) || MaskMatch(textureCoordinatesInput.flags, MeshDefinition::Flags::U8_TEXCOORD))
    {
      DALI_ASSERT_ALWAYS(((texCoords.mBlob.mLength % (sizeof(uint8_t) * 2) == 0) ||
                          texCoords.mBlob.mStride >= (sizeof(uint8_t) * 2)) &&
                         "TexCoords buffer length not a multiple of element size");
      uvCount = static_cast<uint32_t>(bufferSize / (sizeof(uint8_t) * 2));
    }
    else if(MaskMatch(textureCoordinatesInput.flags, MeshDefinition::Flags::S16_TEXCOORD) || MaskMatch(textureCoordinatesInput.flags, MeshDefinition::Flags::U16_TEXCOORD))
    {
      DALI_ASSERT_ALWAYS(((texCoords.mBlob.mLength % (sizeof(uint16_t) * 2) == 0) ||
                          texCoords.mBlob.mStride >= (sizeof(uint16_t) * 2)) &&
                         "TexCoords buffer length not a multiple of element size");
      uvCount = static_cast<uint32_t>(bufferSize / (sizeof(uint16_t) * 2));
    }
    else
    {
      DALI_ASSERT_ALWAYS(((texCoords.mBlob.mLength % sizeof(Vector2) == 0) ||
                          texCoords.mBlob.mStride >= sizeof(Vector2)) &&
                         "TexCoords buffer length not a multiple of element size");
      uvCount = static_cast<uint32_t>(bufferSize / sizeof(Vector2));
    }

    std::vector<uint8_t> buffer(bufferSize);

    std::string path;
    auto&       stream = GetAvailableData(textureCoordinatesInput.fileStream, textureCoordinatesInput.meshPath, textureCoordinatesInput.buffers[texCoords.mBufferIdx], path);
    if(!ReadAccessor(texCoords, stream, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read uv-s from '" << path << "'.";
    }

    GetDequantizedData(buffer, 2u, uvCount, textureCoordinatesInput.flags & MeshDefinition::FlagMasks::TEXCOORDS_MASK, texCoords.mNormalized);

    if(MaskMatch(textureCoordinatesInput.flags, MeshDefinition::Flags::FLIP_UVS_VERTICAL))
    {
      auto uv    = reinterpret_cast<Vector2*>(buffer.data());
      auto uvEnd = uv + uvCount;
      while(uv != uvEnd)
      {
        uv->y = 1.0f - uv->y;
        ++uv;
      }
    }

    if(texCoords.mNormalized)
    {
      GetDequantizedMinMax(texCoords.mBlob.mMin, texCoords.mBlob.mMax, textureCoordinatesInput.flags & MeshDefinition::FlagMasks::TEXCOORDS_MASK);
    }

    texCoords.mBlob.ApplyMinMax(static_cast<uint32_t>(uvCount), reinterpret_cast<float*>(buffer.data()));
    textureCoordinatesInput.rawData.mAttribs.push_back({"aTexCoord", Property::VECTOR2, static_cast<uint32_t>(uvCount), std::move(buffer)});
  }
}

void LoadTangents(LoadAccessorInputs tangentsInput, bool hasNormals, bool hasUvs, bool isTriangles, Property::Type tangentType, uint32_t normalBufferSize)
{
  if(tangentsInput.accessor.IsDefined())
  {
    const auto bufferSize = tangentsInput.accessor.mBlob.GetBufferSize();

    uint32_t propertySize   = static_cast<uint32_t>((tangentType == Property::VECTOR4) ? sizeof(Vector4) : sizeof(Vector3));
    uint32_t componentCount = static_cast<uint32_t>(propertySize / sizeof(float));

    uint32_t numTangents;

    if(MaskMatch(tangentsInput.flags, MeshDefinition::Flags::S8_TANGENT))
    {
      DALI_ASSERT_ALWAYS(((tangentsInput.accessor.mBlob.mLength % (sizeof(int8_t) * componentCount) == 0) ||
                          tangentsInput.accessor.mBlob.mStride >= (sizeof(int8_t) * componentCount)) &&
                         "Tangents buffer length not a multiple of element size");
      numTangents = static_cast<uint32_t>(bufferSize / (sizeof(int8_t) * componentCount));
    }
    else if(MaskMatch(tangentsInput.flags, MeshDefinition::Flags::S16_TANGENT))
    {
      DALI_ASSERT_ALWAYS(((tangentsInput.accessor.mBlob.mLength % (sizeof(int16_t) * componentCount) == 0) ||
                          tangentsInput.accessor.mBlob.mStride >= (sizeof(int16_t) * componentCount)) &&
                         "Tangents buffer length not a multiple of element size");
      numTangents = static_cast<uint32_t>(bufferSize / (sizeof(int16_t) * componentCount));
    }
    else
    {
      DALI_ASSERT_ALWAYS(((tangentsInput.accessor.mBlob.mLength % propertySize == 0) ||
                          tangentsInput.accessor.mBlob.mStride >= propertySize) &&
                         "Tangents buffer length not a multiple of element size");
      numTangents = static_cast<uint32_t>(bufferSize / propertySize);
    }

    std::vector<uint8_t> buffer(bufferSize);

    std::string path;
    auto&       stream = GetAvailableData(tangentsInput.fileStream, tangentsInput.meshPath, tangentsInput.buffers[tangentsInput.accessor.mBufferIdx], path);
    if(!ReadAccessor(tangentsInput.accessor, stream, buffer.data()))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read tangents from '" << path << "'.";
    }

    GetDequantizedData(buffer, componentCount, numTangents, tangentsInput.flags & MeshDefinition::FlagMasks::TANGENTS_MASK, tangentsInput.accessor.mNormalized);

    if(tangentsInput.accessor.mNormalized)
    {
      GetDequantizedMinMax(tangentsInput.accessor.mBlob.mMin, tangentsInput.accessor.mBlob.mMax, tangentsInput.flags & MeshDefinition::FlagMasks::TANGENTS_MASK);
    }

    tangentsInput.accessor.mBlob.ApplyMinMax(numTangents, reinterpret_cast<float*>(buffer.data()));

    tangentsInput.rawData.mAttribs.push_back({"aTangent", tangentType, static_cast<uint32_t>(numTangents), std::move(buffer)});
  }
  else if(tangentsInput.accessor.mBlob.mLength != 0 && hasNormals && isTriangles)
  {
    DALI_ASSERT_DEBUG(tangentsInput.accessor.mBlob.mLength == normalBufferSize);
    static const std::function<bool(MeshDefinition::RawData&)> GenerateTangentsFunction[2][2][2] =
      {
        {
          {
            GenerateTangents<false, false, false>,
            GenerateTangents<false, false, true>,
          },
          {
            GenerateTangents<false, true, false>,
            GenerateTangents<false, true, true>,
          },
        },
        {
          {
            GenerateTangents<true, false, false>,
            GenerateTangents<true, false, true>,
          },
          {
            GenerateTangents<true, true, false>,
            GenerateTangents<true, true, true>,
          },
        }};
    const bool generateSuccessed = GenerateTangentsFunction[MaskMatch(tangentsInput.flags, MeshDefinition::Flags::U32_INDICES)][tangentType == Property::VECTOR3][hasUvs](tangentsInput.rawData);
    if(!generateSuccessed)
    {
      DALI_LOG_ERROR("Failed to generate tangents\n");
    }
  }
}

void LoadColors(LoadAccessorListInputs colorsInput)
{
  // Only support 1 vertex color
  if(!colorsInput.accessors.empty() && colorsInput.accessors[0].IsDefined())
  {
    uint32_t       propertySize = colorsInput.accessors[0].mBlob.mElementSizeHint;
    Property::Type propertyType = (propertySize == sizeof(Vector4)) ? Property::VECTOR4 : ((propertySize == sizeof(Vector3)) ? Property::VECTOR3 : Property::NONE);
    if(propertyType != Property::NONE)
    {
      DALI_ASSERT_ALWAYS(((colorsInput.accessors[0].mBlob.mLength % propertySize == 0) ||
                          colorsInput.accessors[0].mBlob.mStride >= propertySize) &&
                         "Colors buffer length not a multiple of element size");
      const auto           bufferSize = colorsInput.accessors[0].mBlob.GetBufferSize();
      std::vector<uint8_t> buffer(bufferSize);

      std::string path;
      auto&       stream = GetAvailableData(colorsInput.fileStream, colorsInput.meshPath, colorsInput.buffers[colorsInput.accessors[0].mBufferIdx], path);
      if(!ReadAccessor(colorsInput.accessors[0], stream, buffer.data()))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read colors from '" << path << "'.";
      }
      colorsInput.accessors[0].mBlob.ApplyMinMax(bufferSize / propertySize, reinterpret_cast<float*>(buffer.data()));

      colorsInput.rawData.mAttribs.push_back({"aVertexColor", propertyType, static_cast<uint32_t>(bufferSize / propertySize), std::move(buffer)});
    }
  }
  else if(!colorsInput.rawData.mAttribs.empty())
  {
    std::vector<uint8_t> buffer(colorsInput.rawData.mAttribs[0].mNumElements * sizeof(Vector4));
    auto                 colors = reinterpret_cast<Vector4*>(buffer.data());

    for(uint32_t i = 0; i < colorsInput.rawData.mAttribs[0].mNumElements; i++)
    {
      colors[i] = Vector4::ONE;
    }

    colorsInput.rawData.mAttribs.push_back({"aVertexColor", Property::VECTOR4, colorsInput.rawData.mAttribs[0].mNumElements, std::move(buffer)});
  }
}

void LoadBlendShapes(MeshDefinition::RawData& rawData, std::vector<MeshDefinition::BlendShape>& blendShapes, MeshDefinition::Blob& blendShapeHeader, BlendShapes::Version blendShapeVersion, uint32_t numberOfVertices, std::istream* fileStream, BufferDefinition::Vector& buffers)
{
  // Calculate the Blob for the blend shapes.
  MeshDefinition::Blob blendShapesBlob;
  blendShapesBlob.mOffset = std::numeric_limits<unsigned int>::max();
  blendShapesBlob.mLength = 0u;

  uint32_t totalTextureSize(0u);

  auto processAccessor = [&](const MeshDefinition::Accessor& accessor, uint32_t vector3Size) {
    if(accessor.IsDefined())
    {
      blendShapesBlob.mOffset = std::min(blendShapesBlob.mOffset, accessor.mBlob.mOffset);
      blendShapesBlob.mLength += accessor.mBlob.mLength;

      totalTextureSize += accessor.mBlob.mLength / vector3Size;
    }
  };

  for(const auto& blendShape : blendShapes)
  {
    const auto positionMask = blendShape.mFlags & MeshDefinition::FlagMasks::POSITIONS_MASK;
    const auto normalMask   = blendShape.mFlags & MeshDefinition::FlagMasks::NORMALS_MASK;
    const auto tangentMask  = blendShape.mFlags & MeshDefinition::FlagMasks::TANGENTS_MASK;

    processAccessor(blendShape.deltas, MaskMatch(positionMask, MeshDefinition::S8_POSITION) ? sizeof(uint8_t) * 3 : (MaskMatch(positionMask, MeshDefinition::S16_POSITION) ? sizeof(uint16_t) * 3 : sizeof(Vector3)));
    processAccessor(blendShape.normals, MaskMatch(normalMask, MeshDefinition::S8_NORMAL) ? sizeof(uint8_t) * 3 : (MaskMatch(normalMask, MeshDefinition::S16_NORMAL) ? sizeof(uint16_t) * 3 : sizeof(Vector3)));
    processAccessor(blendShape.tangents, MaskMatch(tangentMask, MeshDefinition::S8_TANGENT) ? sizeof(uint8_t) * 3 : (MaskMatch(tangentMask, MeshDefinition::S16_TANGENT) ? sizeof(uint16_t) * 3 : sizeof(Vector3)));
  }

  if(!blendShapes.empty())
  {
    // Calculate the size of one buffer inside the texture.
    rawData.mBlendShapeBufferOffset = numberOfVertices;

    bool     calculateGltf2BlendShapes = false;
    uint32_t textureWidth              = 0u;
    uint32_t textureHeight             = 0u;

    if(!blendShapeHeader.IsDefined())
    {
      CalculateTextureSize(totalTextureSize, textureWidth, textureHeight);
      calculateGltf2BlendShapes = true;
    }
    else if(fileStream)
    {
      uint16_t header[2u];
      ReadBlob(blendShapeHeader, *fileStream, reinterpret_cast<uint8_t*>(header));
      textureWidth  = header[0u];
      textureHeight = header[1u];
    }

    const uint32_t numberOfBlendShapes = blendShapes.size();
    rawData.mBlendShapeUnnormalizeFactor.Resize(numberOfBlendShapes);

    Devel::PixelBuffer geometryPixelBuffer = Devel::PixelBuffer::New(textureWidth, textureHeight, Pixel::RGB32F);
    uint8_t*           geometryBuffer      = geometryPixelBuffer.GetBuffer();

    if(calculateGltf2BlendShapes)
    {
      CalculateGltf2BlendShapes(geometryBuffer, blendShapes, numberOfVertices, rawData.mBlendShapeUnnormalizeFactor[0u], buffers);
    }
    else
    {
      MeshDefinition::Blob unnormalizeFactorBlob;
      unnormalizeFactorBlob.mLength = static_cast<uint32_t>(sizeof(float) * ((BlendShapes::Version::VERSION_2_0 == blendShapeVersion) ? 1u : numberOfBlendShapes));

      if(blendShapesBlob.IsDefined())
      {
        if(fileStream && ReadBlob(blendShapesBlob, *fileStream, geometryBuffer))
        {
          unnormalizeFactorBlob.mOffset = blendShapesBlob.mOffset + blendShapesBlob.mLength;
        }
      }

      // Read the unnormalize factors.
      if(unnormalizeFactorBlob.IsDefined() && fileStream)
      {
        ReadBlob(unnormalizeFactorBlob, *fileStream, reinterpret_cast<uint8_t*>(&rawData.mBlendShapeUnnormalizeFactor[0u]));
      }
    }
    rawData.mBlendShapeData = Devel::PixelBuffer::Convert(geometryPixelBuffer);
  }
}

constexpr uint32_t MINIMUM_SHADER_VERSION_SUPPORT_VERTEX_ID = 300;

} // namespace

MeshDefinition::SparseBlob::SparseBlob(const Blob& indices, const Blob& values, uint32_t count)
: mIndices{indices},
  mValues{values},
  mCount{count}
{
}

MeshDefinition::SparseBlob::SparseBlob(Blob&& indices, Blob&& values, uint32_t count)
: mIndices(std::move(indices)),
  mValues(std::move(values)),
  mCount{count}
{
}

MeshDefinition::Accessor::Accessor(const MeshDefinition::Blob&       blob,
                                   const MeshDefinition::SparseBlob& sparse,
                                   Index                             bufferIndex,
                                   bool                              normalized)
: mBlob{blob},
  mSparse{(sparse.mIndices.IsDefined() && sparse.mValues.IsDefined()) ? new SparseBlob{sparse} : nullptr},
  mBufferIdx(bufferIndex),
  mNormalized(normalized)
{
}

MeshDefinition::Accessor::Accessor(MeshDefinition::Blob&&       blob,
                                   MeshDefinition::SparseBlob&& sparse,
                                   Index                        bufferIndex,
                                   bool                         normalized)
: mBlob{std::move(blob)},
  mSparse{(sparse.mIndices.IsDefined() && sparse.mValues.IsDefined()) ? new SparseBlob{std::move(sparse)} : nullptr},
  mBufferIdx(bufferIndex),
  mNormalized(normalized)
{
}

void MeshDefinition::Blob::ComputeMinMax(std::vector<float>& min, std::vector<float>& max, uint32_t numComponents, uint32_t count, const float* values)
{
  min.assign(numComponents, MAXFLOAT);
  max.assign(numComponents, -MAXFLOAT);
  for(uint32_t i = 0; i < count; ++i)
  {
    for(uint32_t j = 0; j < numComponents; ++j)
    {
      min[j] = std::min(min[j], *values);
      max[j] = std::max(max[j], *values);
      values++;
    }
  }
}

void MeshDefinition::Blob::ApplyMinMax(const std::vector<float>& min, const std::vector<float>& max, uint32_t count, float* values, std::vector<uint32_t>* sparseIndices)
{
  DALI_ASSERT_DEBUG(max.size() == min.size() || max.size() * min.size() == 0);
  const auto numComponents = std::max(min.size(), max.size());

  using ClampFn   = void (*)(const float*, const float*, uint32_t, float&);
  ClampFn clampFn = min.empty() ? (max.empty() ? static_cast<ClampFn>(nullptr) : [](const float* min, const float* max, uint32_t i, float& value) { value = std::min(max[i], value); })
                                : (max.empty() ? [](const float* min, const float* max, uint32_t i, float& value) { value = std::max(min[i], value); }
                                               : static_cast<ClampFn>([](const float* min, const float* max, uint32_t i, float& value) { value = std::min(std::max(min[i], value), max[i]); }));

  if(!clampFn)
  {
    return;
  }

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
  return mLength;
}

void MeshDefinition::Blob::ComputeMinMax(uint32_t numComponents, uint32_t count, float* values)
{
  ComputeMinMax(mMin, mMax, numComponents, count, values);
}

void MeshDefinition::Blob::ApplyMinMax(uint32_t count, float* values, std::vector<uint32_t>* sparseIndices) const
{
  ApplyMinMax(mMin, mMax, count, values, sparseIndices);
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
  return !mJoints.empty() && !mWeights.empty();
}

bool MeshDefinition::HasVertexColor() const
{
  return !mColors.empty();
}

uint32_t MeshDefinition::GetNumberOfJointSets() const
{
  uint32_t number = static_cast<uint32_t>(mJoints.size());
  if(number > MeshDefinition::MAX_NUMBER_OF_JOINT_SETS)
  {
    number = MeshDefinition::MAX_NUMBER_OF_JOINT_SETS;
  }
  return number;
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
MeshDefinition::LoadRaw(const std::string& modelsPath, BufferDefinition::Vector& buffers)
{
  RawData raw;
  if (IsQuad())
  {
    return raw;
  }

  std::string meshPath;
  meshPath = modelsPath + mUri;

  std::unique_ptr<Dali::FileStream> daliFileStream(nullptr);
  std::iostream* fileStream = nullptr;
  if (!mUri.empty())
  {
    daliFileStream.reset(new Dali::FileStream(meshPath, FileStream::READ | FileStream::BINARY));
    fileStream = &daliFileStream->GetStream();
    if(!fileStream->good() || !fileStream->rdbuf()->in_avail())
    {
      DALI_LOG_ERROR("Fail to open buffer from %s.\n", meshPath.c_str());
      fileStream = nullptr;
    }
  }

  LoadAccessorInputs indicesInput = {raw, mIndices, mFlags, fileStream, meshPath, buffers};
  LoadIndices(indicesInput);

  LoadAccessorInputs positionsInput   = {raw, mPositions, mFlags, fileStream, meshPath, buffers};
  uint32_t           numberOfVertices = LoadPositions(positionsInput, HasBlendShapes());

  const auto         isTriangles  = mPrimitiveType == Geometry::TRIANGLES;
  LoadAccessorInputs normalsInput = {raw, mNormals, mFlags, fileStream, meshPath, buffers};
  auto               hasNormals   = LoadNormals(normalsInput, isTriangles, mPositions.mBlob.GetBufferSize());

  LoadAccessorListInputs textureCoordinatesInput = {raw, mTexCoords, mFlags, fileStream, meshPath, buffers};
  LoadTextureCoordinates(textureCoordinatesInput);

  const bool         hasUvs        = !mTexCoords.empty() && mTexCoords[0].IsDefined();
  LoadAccessorInputs tangentsInput = {raw, mTangents, mFlags, fileStream, meshPath, buffers};
  LoadTangents(tangentsInput, hasNormals, hasUvs, isTriangles, mTangentType, mNormals.mBlob.GetBufferSize());

  LoadAccessorListInputs colorsInput = {raw, mColors, mFlags, fileStream, meshPath, buffers};
  LoadColors(colorsInput);

  if(IsSkinned())
  {
    LoadDataType           loadDataType = (MaskMatch(mFlags, MeshDefinition::U16_JOINT_IDS)) ? LoadDataType::UNSIGNED_SHORT : (MaskMatch(mFlags, MeshDefinition::U8_JOINT_IDS) ? LoadDataType::UNSIGNED_BYTE : LoadDataType::FLOAT);
    LoadAccessorListInputs jointsInput  = {raw, mJoints, mFlags, fileStream, meshPath, buffers};
    ReadTypedVectorAccessors<false>(jointsInput, loadDataType, "aJoints");

    loadDataType                        = (MaskMatch(mFlags, MeshDefinition::U16_WEIGHT)) ? LoadDataType::UNSIGNED_SHORT : (MaskMatch(mFlags, MeshDefinition::U8_WEIGHT) ? LoadDataType::UNSIGNED_BYTE : LoadDataType::FLOAT);
    LoadAccessorListInputs weightsInput = {raw, mWeights, mFlags, fileStream, meshPath, buffers};
    ReadTypedVectorAccessors<true>(weightsInput, loadDataType, "aWeights");
  }

  LoadBlendShapes(raw, mBlendShapes, mBlendShapeHeader, mBlendShapeVersion, numberOfVertices, fileStream, buffers);
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
      if(MaskMatch(mFlags, U32_INDICES))
      {
        // TODO : We can only store indeces as uint16_type. Send Dali::Geometry that we use it as uint32_t actual.
        meshGeometry.geometry.SetIndexBuffer(reinterpret_cast<const uint32_t*>(raw.mIndices.data()), raw.mIndices.size() / 2);
      }
      else
      {
        meshGeometry.geometry.SetIndexBuffer(raw.mIndices.data(), raw.mIndices.size());
      }
    }

    if(DALI_UNLIKELY(Dali::Shader::GetShaderLanguageVersion() < MINIMUM_SHADER_VERSION_SUPPORT_VERTEX_ID && !raw.mAttribs.empty()))
    {
      auto numElements = raw.mAttribs[0].mNumElements;

      // gl_VertexID not support. We should add buffer hard.
      Property::Map attribMap;
      attribMap["aVertexID"] = Property::FLOAT;

      VertexBuffer attribBuffer = VertexBuffer::New(attribMap);

      std::vector<uint8_t> buffer(numElements * sizeof(float));
      auto                 ids = reinterpret_cast<float*>(buffer.data());

      for(uint32_t i = 0; i < numElements; i++)
      {
        ids[i] = static_cast<float>(i);
      }

      attribBuffer.SetData(buffer.data(), numElements);

      meshGeometry.geometry.AddVertexBuffer(attribBuffer);
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

void MeshDefinition::RetrieveBlendShapeComponents(bool& hasPositions, bool& hasNormals, bool& hasTangents) const
{
  for(const auto& blendShape : mBlendShapes)
  {
    hasPositions = hasPositions || blendShape.deltas.IsDefined();
    hasNormals   = hasNormals || blendShape.normals.IsDefined();
    hasTangents  = hasTangents || blendShape.tangents.IsDefined();
  }
}

} // namespace Dali::Scene3D::Loader
