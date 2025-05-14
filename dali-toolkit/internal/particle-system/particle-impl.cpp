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
#include <dali-toolkit/internal/particle-system/particle-impl.h>

namespace Dali::Toolkit::ParticleSystem::Internal
{

Particle::Particle(Internal::ParticleList& ownerList, uint32_t index)
: mOwnerList(ownerList),
  mIndex(index)
{
}

void* Particle::Get(ParticleStreamTypeFlagBit streamBit)
{
  const auto streamIndex = mOwnerList.GetDefaultStreamIndex(streamBit);
  const auto dataSize    = mOwnerList.GetStreamDataTypeSize(streamIndex);
  return reinterpret_cast<uint8_t*>(mOwnerList.GetDefaultStream(streamBit)) + (mIndex * dataSize);
}

void* Particle::GetByIndex(uint32_t streamIndex)
{
  const auto dataSize = mOwnerList.GetStreamDataTypeSize(streamIndex);
  auto*      ptr      = reinterpret_cast<uint8_t*>(mOwnerList.GetRawStream(streamIndex));
  return reinterpret_cast<uint8_t*>(ptr + (mIndex * dataSize));
}

uint32_t Particle::GetIndex() const
{
  return mIndex;
}

} // namespace Dali::Toolkit::ParticleSystem::Internal