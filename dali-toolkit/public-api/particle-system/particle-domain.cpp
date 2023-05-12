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

// CLASS HEADER
#include <dali-toolkit/public-api/particle-system/particle-domain.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/particle-system/particle-domain-impl.h>

namespace Dali::Toolkit::ParticleSystem
{
ParticleDomain ParticleDomain::New()
{
  return {new Internal::ParticleDomain()};
}

ParticleDomain::ParticleDomain(Internal::ParticleDomain* impl)
: Dali::BaseHandle(impl)
{
}

ParticleDomain ParticleDomain::DownCast(BaseHandle handle)
{
  return {dynamic_cast<Internal::ParticleDomain*>(handle.GetObjectPtr())};
}

} // namespace Dali::Toolkit::ParticleSystem