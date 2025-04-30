#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_LIST_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_LIST_H
/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/common/list-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <algorithm>
#include <cstring>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
template<class T>
struct StreamDataTypeWrapper
{
  static ParticleStream::StreamDataType GetType()
  {
    return {};
  }
};

struct ParticleDataStream
{
  ~ParticleDataStream() = default;

  /**
   * Creates new stream of requested capacity and (optionally) fills with default data
   */
  ParticleDataStream(uint32_t capacity, uint32_t dataSize, const void* defaultValue, ParticleStream::StreamDataType dataType, const char* name, bool localStream)
  : type(dataType),
    data(capacity * dataSize),
    streamName(name ? name : ""),
    dataSize(dataSize),
    localStream(localStream)
  {
    if(defaultValue)
    {
      auto*       dstPtr          = data.data();
      const auto* defaultValuePtr = reinterpret_cast<const uint8_t*>(defaultValue);
      for(auto i = 0u; i < capacity; ++i)
      {
        memcpy(dstPtr, defaultValuePtr, dataSize);
        dstPtr += dataSize;
      }
    }
  }

  /**
   * Converts raw data into requested type (does not guarantee compatibility)
   */
  template<class T>
  T* GetAs()
  {
    return reinterpret_cast<T*>(data.data());
  }

  ParticleStream::StreamDataType type;
  std::vector<uint8_t>           data;
  std::string                    streamName;
  uint32_t                       dataSize;
  bool                           localStream{true};
};

/**
 * Particle list stores particle-specific data and manages the particles memory
 * It can return a sub-list.
 *
 * ParticleList manages the storage memory.
 *
 *
 */
class ParticleList : public Dali::BaseObject
{
public:
  ParticleList(uint32_t capacity, ParticleSystem::ParticleList::ParticleStreamTypeFlags streamFlags);

  ~ParticleList();

  /**
   * Returns raw pointer to the stream data
   */
  void* GetRawStream(uint32_t index);

  /**
   * Returns number of available data streams
   * @return
   */
  uint32_t GetStreamCount() const;

  /**
   * Returns number of particles per list
   * @return
   */
  uint32_t GetParticleCount() const;

  /**
   * Returns number of currently active particles
   * @return
   */
  uint32_t GetActiveParticleCount() const;

  /**
   * Returns stream data-type
   * @param streamIndex
   * @return
   */
  ParticleStream::StreamDataType GetStreamDataType(uint32_t streamIndex);

  /**
   * Returns stream data type size
   * @param streamIndex
   * @return
   */
  [[nodiscard]] uint32_t GetStreamDataTypeSize(uint32_t streamIndex) const;

  [[nodiscard]] const std::string& GetStreamName(uint32_t streamIndex) const;

  [[nodiscard]] bool IsStreamLocal(uint32_t streamIndex) const;

  /**
   * Allocates new particle in the streams
   * @param lifetime
   * @return
   */
  ParticleSystem::Particle NewParticle(float lifetime);

  void* GetDefaultStream(ParticleStreamTypeFlagBit streamBit);

  uint32_t GetDefaultStreamIndex(ParticleStreamTypeFlagBit streamBit);

  std::list<ParticleSystem::Particle>& GetParticles();

  void ReleaseParticles(const std::vector<uint32_t>& sortedEraseIndices);

  uint32_t GetStreamElementSize(bool includeLocalStream);

private:
  template<class T>
  uint32_t AddStream(const T& defaultValue, const char* streamName, bool localStream)
  {
    return AddStream(sizeof(T), &defaultValue, StreamDataTypeWrapper<T>::GetType(), streamName, localStream);
  }

public:
  /**
   * Adds new stream and returns index
   */
  uint32_t AddStream(uint32_t sizeOfDataType, const void* defaultValue, ParticleStream::StreamDataType dataType, const char* streamName, bool localStream);

private:
  std::vector<char> mBuffer[2];

  uint32_t mAliveParticleCount{0u};
  uint32_t mMaxParticleCount;

  // Data storage
  std::vector<std::unique_ptr<ParticleDataStream>> mDataStreams;

  std::vector<uint32_t> mFreeChain;
  int32_t               mFreeIndex{0u};

  std::map<uint32_t, uint32_t> mBuiltInStreamMap;

  std::list<ParticleSystem::Particle> mParticles;

  uint32_t mParticleStreamElementSizeWithLocal{0u};
  uint32_t mParticleStreamElementSize{0u};
};

} // namespace Dali::Toolkit::ParticleSystem::Internal
namespace Dali::Toolkit::ParticleSystem
{
inline Internal::ParticleList& GetImplementation(ParticleSystem::ParticleList& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleList handle is empty");

  BaseObject& handle = source.GetBaseObject();

  return static_cast<Internal::ParticleList&>(handle);
}

inline const Internal::ParticleList& GetImplementation(const ParticleSystem::ParticleList& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleList handle is empty");

  const BaseObject& handle = source.GetBaseObject();

  return static_cast<const Internal::ParticleList&>(handle);
}

} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_LIST_H