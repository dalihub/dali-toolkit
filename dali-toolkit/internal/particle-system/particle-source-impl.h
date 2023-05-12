#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_SOURCE_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_SOURCE_H
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

#include <dali-toolkit/public-api/particle-system/particle-source.h>
#include <dali/public-api/object/base-object.h>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleList;
class ParticleEmitter;
class ParticleSource : public Dali::BaseObject
{
public:
  ~ParticleSource() override = default;

  explicit ParticleSource(std::unique_ptr<ParticleSourceInterface>&& sourceUpdater);

  void Update(ParticleSystem::ParticleList& list, uint32_t count);

  [[nodiscard]] ParticleSourceInterface& GetUpdater() const;

private:
  std::unique_ptr<ParticleSourceInterface> mUpdater;
};

} // namespace Dali::Toolkit::ParticleSystem::Internal

namespace Dali::Toolkit::ParticleSystem
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

} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_SOURCE_H