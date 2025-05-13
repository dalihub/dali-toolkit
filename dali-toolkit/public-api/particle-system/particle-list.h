#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_LIST_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_LIST_H
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/list-wrapper.h>
#include <dali/public-api/object/base-handle.h>
#include <cinttypes>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleList;
}

namespace Dali::Toolkit::ParticleSystem
{
/**
 * The function is a wrapper needed to retrieve data type enum through template
 */
template<class T>
struct StreamDataTypeWrapper
{
  static ParticleStream::StreamDataType GetType()
  {
    return {};
  }
};

/**
 * @class ParticleList
 *
 * ParticleList defines a storage or a partial view on an existing storage of particle-related data.
 *
 * ParticleList contains streams of data (properties) laid out independently (non-interleaved).
 *
 * The layout is more optimal for:
 * - parallel processing
 * - data-oriented processing (CPU-cache friendly)
 * - adding custom streams of data (for example, physics properties)
 *
 * Some streams are being added automatically by the emitter when certain modifiers are being added.
 * If the modifier requires particular data it the emitter will update the list with proper stream.
 *
 * There are several built-in streams defined:
 * - POSITION
 * - VELOCITY
 * - COLOR
 * - OPACITY
 * - SIZE
 *
 * The New() function allows adding streams upon creation.
 *
 * Custom streams may be used by custom renderers, modifiers and sources but also may play
 * a role of temporary storage space when it comes to computing particle parameters.
 *
 * Represents all or subset of group of particles
 */
class DALI_TOOLKIT_API ParticleList : public Dali::BaseHandle
{
public:
  /**
   * @brief Constructor
   */
  ParticleList();

  /**
   * @brief Bit flags grouping built-in particle streams by type
   */
  struct ParticleStreamTypeFlags
  {
    ParticleStreamTypeFlags(const ParticleStreamTypeFlags& flags)
    {
      value = flags.value;
    }

    ParticleStreamTypeFlags(const ParticleStreamTypeFlagBit& bit)
    : value(bit)
    {
    }

    ParticleStreamTypeFlags& operator|=(ParticleStreamTypeFlagBit flagBit)
    {
      value |= flagBit;
      return *this;
    }

    ParticleStreamTypeFlags operator&(ParticleStreamTypeFlagBit flagBit)
    {
      return (value & flagBit);
    }

    explicit operator uint32_t() const
    {
      return value;
    }

    explicit operator bool() const
    {
      return value;
    }

    uint32_t value;
  };

  /**
   * @brief Creates new ParticleList
   *
   * ParticleList is a storage object that contains data for particle system
   * layouted as an array of streams.
   *
   * Stream may contain data like position, velocity, etc.
   *
   * When new ParticleList is created the default streams can be pre-allocated.
   *
   * @param[in] capacity Maximum capacity (number of particles in the system)
   * @param[in] defaultStreams Default data streams to pre-allocate
   */
  static ParticleList New(uint32_t capacity, const ParticleStreamTypeFlags& defaultStreams);

  /**
   * @brief Downcasts a handle to ParticleList handle.
   *
   * If handle points to an ParticleList object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ParticleList object or an uninitialized handle
   */
  static ParticleList DownCast(BaseHandle handle);

  /**
   * @brief Registers new data-stream with given unique identifier
   *
   * @tparam T Type of data stored in the stream
   * @param[in] defaults Default value of the stream uninitialized data
   *
   * Streams added using AddStream() function are automatically passed into
   * the shader program as input attributes.
   *
   * If there is no need for the stream to be used as a shader attribute
   * then use AddLocalStream() instead.
   *
   * @return Returns index of an allocated data stream
   */
  template<class T>
  inline uint32_t AddStream(T defaults)
  {
    return AddStream(&defaults, StreamDataTypeWrapper<T>::GetType(), sizeof(T), false);
  }

  /**
   * @brief Adds local data stream
   *
   * @tparam T Type of data stored in the stream
   * @param[in] defaults Default value of the stream
   *
   * @return Returns index of newly allocated data stream
   */
  template<class T>
  inline uint32_t AddLocalStream(T defaults)
  {
    return AddStream(&defaults, sizeof(T), StreamDataTypeWrapper<T>::GetType(), true);
  }

  template<class T>
  T* GetStream(uint32_t streamIndex)
  {
    return reinterpret_cast<T*>(GetRawStream(streamIndex));
  }

  template<class T>
  T* GetDefaultStream(ParticleStreamTypeFlagBit streamFlagBit)
  {
    return reinterpret_cast<T*>(GetDefaultStream(streamFlagBit));
  }

  /**
   * @brief Returns size of list including only active particles
   *
   * @return
   */
  uint32_t GetActiveParticleCount();

  /**
   * @brief Returns capacity of particle list
   */
  [[nodiscard]] uint32_t GetCapacity() const;

  /**
   * Creates new particle in the list with specified lifetime
   *
   * @param[in] lifetime Expected lifetime of new particle (0.0f - lives forever)
   * @return index into data streams
   */
  Particle NewParticle(float lifetime);

  /**
   * @brief Returns internal data size of streams
   *
   * @param[in] includeLocalStreams If true, the size will include local streams
   * @return Size of data structure
   */
  uint32_t GetParticleDataSize(bool includeLocalStreams);

  /**
   * @brief Returns index associated with specified default stream
   *
   * @param[in] defaultStreamBit Default stream bit
   * @return Returns a valid index or -1 on error.
   */
  int GetDefaultStreamIndex(ParticleStreamTypeFlagBit defaultStreamBit);

  /**
   * @brief Returns raw data container of the particle list
   * @return list of particles
   */
  std::list<Particle>& GetActiveParticles();

private:
  /// @cond internal
  /**
   * @brief Adds new data stream to the list
   *
   * @param[in] defaults Default values to fill the stream with
   * @param[in] dataTypeSize size of stored data type
   * @param[in] dataType Stream data type
   * @param[in] localStream Flag indicating whether stream is local (not used in shaders) or not
   * @return Index of new stream
   */
  uint32_t AddStream(void* defaults, size_t dataTypeSize, ParticleStream::StreamDataType dataType, bool localStream);
  /// @endcond

  /// @cond internal
  /**
   * @brief Returns raw data pointer to the stream at given index
   *
   * @param[in] streamIndex Stream index
   *
   * @return Valid data pointer or nullptr if index invalid
   */
  void* GetRawStream(uint32_t streamIndex);
  /// @endcond

  /// @cond internal
  /**
   * @brief Returns raw data stream of a default (built-in) stream
   * @param[in] streamBit Bit (ParticleStreamTypeFlagBit) representing the stream
   *
   * @return Returns valid pointer to the data or nullptr if stream hasn't been used
   */
  void* GetDefaultStream(ParticleStreamTypeFlagBit streamBit);
  /// @endcond

  /// @cond internal
  /**
   * @brief This constructor is used by ParticleList::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  ParticleList(Dali::Toolkit::ParticleSystem::Internal::ParticleList* impl);
  /// @endcond
};

} // namespace Dali::Toolkit::ParticleSystem

#endif
