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

#include <dali-toolkit/internal/particle-system/particle-list-impl.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>

namespace Dali::Toolkit::ParticleSystem
{
template<>
ParticleStream::StreamDataType StreamDataTypeWrapper<Vector3>::GetType()
{
  return ParticleStream::StreamDataType::FLOAT3;
}

template<>
ParticleStream::StreamDataType StreamDataTypeWrapper<Vector4>::GetType()
{
  return ParticleStream::StreamDataType::FLOAT4;
}

template<>
ParticleStream::StreamDataType StreamDataTypeWrapper<Vector2>::GetType()
{
  return ParticleStream::StreamDataType::FLOAT2;
}

template<>
ParticleStream::StreamDataType StreamDataTypeWrapper<float>::GetType()
{
  return ParticleStream::StreamDataType::FLOAT;
}

ParticleList ParticleList::New(uint32_t capacity, const ParticleStreamTypeFlags& defaultStreams)
{
  return {new ParticleSystem::Internal::ParticleList(capacity, defaultStreams)};
}

ParticleList ParticleList::DownCast(BaseHandle handle)
{
  return {dynamic_cast<Internal::ParticleList*>(handle.GetObjectPtr())};
}

uint32_t ParticleList::AddStream(void* defaults, size_t dataTypeSize, ParticleStream::StreamDataType dataType, bool localStream)
{
  return GetImplementation(*this).AddStream(dataTypeSize, defaults, dataType, "", localStream);
}

void* ParticleList::GetRawStream(uint32_t streamIndex)
{
  return GetImplementation(*this).GetRawStream(streamIndex);
}

void* ParticleList::GetDefaultStream(ParticleStreamTypeFlagBit streamBit)
{
  return GetImplementation(*this).GetDefaultStream(streamBit);
}

uint32_t ParticleList::GetActiveParticleCount()
{
  return GetImplementation(*this).GetActiveParticleCount();
}

ParticleList::ParticleList(Dali::Toolkit::ParticleSystem::Internal::ParticleList* impl)
: Dali::BaseHandle(impl)
{
}

uint32_t ParticleList::GetCapacity() const
{
  return GetImplementation(*this).GetParticleCount();
}

Particle ParticleList::NewParticle(float lifetime)
{
  return GetImplementation(*this).NewParticle(lifetime);
}

uint32_t ParticleList::GetParticleDataSize(bool includeLocalStreams)
{
  return GetImplementation(*this).GetStreamElementSize(includeLocalStreams);
}

int ParticleList::GetDefaultStreamIndex(ParticleStreamTypeFlagBit defaultStreamBit)
{
  return GetImplementation(*this).GetDefaultStreamIndex(defaultStreamBit);
}

std::list<Particle>& ParticleList::GetActiveParticles()
{
  return GetImplementation(*this).GetParticles();
}

ParticleList::ParticleList() = default;

} // namespace Dali::Toolkit::ParticleSystem