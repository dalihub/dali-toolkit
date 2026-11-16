#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_SOURCE_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_SOURCE_H
/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle-source.h>

namespace DALI_NAMESPACE::Toolkit::ParticleSystem::Internal
{
class ParticleList;
class ParticleEmitter;
class ParticleSource : public Dali::BaseObject
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL ParticleSource* NewForBaseHandle(UniquePtr<ParticleSourceInterface>&& sourceUpdater)
  {
    return new ParticleSource(std::move(sourceUpdater));
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static Dali::IntrusivePtr<ParticleSource> New(UniquePtr<ParticleSourceInterface>&& sourceUpdater)
  {
    return Dali::IntrusivePtr<ParticleSource>(new ParticleSource(std::move(sourceUpdater)));
  }

  void Update(ParticleSystem::ParticleList& list, uint32_t count);

  [[nodiscard]] ParticleSourceInterface& GetUpdater();

protected:
  ~ParticleSource() override = default;

  explicit ParticleSource(UniquePtr<ParticleSourceInterface>&& sourceUpdater);

private:
  UniquePtr<ParticleSourceInterface> mUpdater;
};

} //namespace DALI_NAMESPACE::Toolkit::ParticleSystem::Internal

namespace DALI_NAMESPACE::Toolkit::ParticleSystem
{
inline Internal::ParticleSource& GetImplementation(ParticleSystem::ParticleSource& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleSource handle is empty");

  BaseObject& handle = source.GetBaseObject();

  return static_cast<Internal::ParticleSource&>(handle);
}

inline const Internal::ParticleSource& GetImplementation(const ParticleSystem::ParticleSource& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleSource handle is empty");

  const BaseObject& handle = source.GetBaseObject();

  return static_cast<const Internal::ParticleSource&>(handle);
}

} //namespace DALI_NAMESPACE::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_SOURCE_H