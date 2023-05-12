#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_H
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
#include <dali-toolkit/public-api/particle-system/particle-types.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali::Toolkit::ParticleSystem
{
namespace Internal
{
class Particle;
}

/**
 * @class Particle
 *
 * Particle represents a single instance of a particle in the particle system.
 *
 * Particle provides a simple interface that allows R/W access to selected data stream and
 * allows ignoring a placement of the particle data within the stream (Particle serves as a
 * view on particular data within a stream).
 */
class DALI_TOOLKIT_API Particle : public BaseHandle
{
public:
  /**
   * @brief Constructor
   */
  Particle() = default;

  /**
   * @brief Downcasts a handle to Particle handle.
   *
   * If handle points to an Particle object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a Particle object or an uninitialized handle
   */
  static Particle DownCast(BaseHandle handle);

  /**
   * @brief Returns writeable reference to the data for specified stream
   *
   * The ParticleStreamTypeFlagBit allows accessing only pre-defined streams
   * defined by the ParticleSystem. For custom streams GetByIndex() should be used.
   *
   * @tparam T type of data
   * @param[in] streamBit Stream to access data from
   * @return Reference to the data value
   */
  template<class T>
  T& Get(ParticleStreamTypeFlagBit streamBit)
  {
    return *reinterpret_cast<T*>(Get(streamBit));
  }

  /**
   * @brief Returns writeable reference to the data for a stream specified by stream index.
   *
   * This function allows accessing builtin streams as well as custom ones. The index of a custom
   * stream should be stored upon creation.
   *
   * @tparam T type of data
   * @param[in] streamIndex Index of stream in the emitter
   * @return Reference to the data value
   */
  template<class T>
  T& GetByIndex(uint32_t streamIndex)
  {
    return *reinterpret_cast<T*>(GetByIndex(streamIndex));
  }

  /**
   * @brief Returns an index of particle within emitter data streams.
   *
   * @return Index of particle
   */
  [[nodiscard]] uint32_t GetIndex() const;

public:
  /// @cond internal
  /**
   * @brief This constructor is used by Particle::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  Particle(Dali::Toolkit::ParticleSystem::Internal::Particle* impl);
  /// @endcond

private:
  /// @cond internal
  /**
   * @brief Returns pointer to the value
   *
   *
   * The ParticleStreamTypeFlagBit allows accessing only pre-defined streams
   * defined by the ParticleSystem. For custom streams GetByIndex() should be used.
   *
   * @param[in] streamBit Stream to access data from
   *
   * @return void* to the memory within stream that stores the data
   */
  void* Get(ParticleStreamTypeFlagBit streamBit);
  /// @endcond

  /// @cond internal
  /**
   * @brief Returns writeable reference to the data for a stream specified by stream index.
   *
   * This function allows accessing builtin streams as well as custom ones. The index of a custom
   * stream should be stored upon creation.
   *
   * @param[in] streamIndex Index of stream in the emitter
   *
   * @return void* to the memory within stream that stores the data
   */
  void* GetByIndex(uint32_t streamIndex);
  /// @endcond
};
} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_H