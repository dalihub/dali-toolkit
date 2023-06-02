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
#include <dali-toolkit/internal/particle-system/particle-list-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/particle-system/particle-impl.h>

namespace Dali::Toolkit::ParticleSystem::Internal
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

ParticleList::ParticleList(uint32_t capacity, ParticleSystem::ParticleList::ParticleStreamTypeFlags streamFlags)
{
  // capacity makes for max immutable particle count
  mMaxParticleCount = capacity;

  // initialize built-in streams and build map (to optimize later)
  if(streamFlags & ParticleStream::POSITION_STREAM_BIT)
  {
    AddStream(Vector3::ZERO, "aStreamPosition", false);
    mBuiltInStreamMap[uint32_t(ParticleStream::POSITION_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::ROTATION_STREAM_BIT)
  {
    AddStream(Vector4::ZERO, "aStreamRotation", false);
    mBuiltInStreamMap[uint32_t(ParticleStream::ROTATION_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::SCALE_STREAM_BIT)
  {
    AddStream(Vector3::ONE, "aStreamScale", false);
    mBuiltInStreamMap[uint32_t(ParticleStream::SCALE_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::VELOCITY_STREAM_BIT)
  {
    AddStream(Vector3::ZERO, "aStreamVelocity", false);
    mBuiltInStreamMap[uint32_t(ParticleStream::VELOCITY_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::COLOR_STREAM_BIT)
  {
    AddStream(Color::YELLOW, "aStreamColor", false);
    mBuiltInStreamMap[uint32_t(ParticleStream::COLOR_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::OPACITY_STREAM_BIT)
  {
    AddStream(0.0f, "aStreamOpacity", false);
    mBuiltInStreamMap[uint32_t(ParticleStream::OPACITY_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::LIFETIME_STREAM_BIT)
  {
    AddStream(0.0f, "aStreamLifetime", true);
    mBuiltInStreamMap[uint32_t(ParticleStream::LIFETIME_STREAM_BIT)] = mDataStreams.size() - 1;
  }
  if(streamFlags & ParticleStream::LIFETIME_STREAM_BIT)
  {
    AddStream(0.0f, "aStreamLifetimeBase", true);
    mBuiltInStreamMap[uint32_t(ParticleStream::LIFETIME_BASE_STREAM_BIT)] = mDataStreams.size() - 1;
  }

  // create free chain
  mFreeChain.resize(capacity);
  for(auto i = 0u; i < mFreeChain.size(); ++i)
  {
    mFreeChain[i] = i + 1;
  }
  mFreeChain[mFreeChain.size() - 1] = 0;
  mFreeIndex                        = 0;
}

ParticleList::~ParticleList() = default;

uint32_t ParticleList::AddStream(uint32_t sizeOfDataType, const void* defaultValue, ParticleStream::StreamDataType dataType, const char* streamName, bool localStream)
{
  mDataStreams.emplace_back(new ParticleDataStream(mMaxParticleCount, sizeOfDataType, defaultValue, dataType));
  if(streamName)
  {
    mDataStreams.back()->SetStreamName(streamName);
  }

  mDataStreams.back()->SetStreamLocal(localStream);

  // Update element size
  mParticleStreamElementSize          = 0;
  mParticleStreamElementSizeWithLocal = 0;
  for(auto& ds : mDataStreams)
  {
    if(!ds->localStream)
    {
      mParticleStreamElementSize += ds->dataSize;
    }
    mParticleStreamElementSizeWithLocal += ds->dataSize;
  }

  return mDataStreams.size() - 1;
}

void* ParticleList::GetRawStream(uint32_t index)
{
  if(index < mDataStreams.size() && mDataStreams[index])
  {
    return mDataStreams[index]->data.data();
  }
  return nullptr;
}

uint32_t ParticleList::GetStreamCount() const
{
  return mDataStreams.size();
}

uint32_t ParticleList::GetParticleCount() const
{
  return mMaxParticleCount;
}

uint32_t ParticleList::GetActiveParticleCount() const
{
  return mParticles.size();
}

ParticleStream::StreamDataType ParticleList::GetStreamDataType(uint32_t streamIndex)
{
  return mDataStreams[streamIndex]->type;
}

const std::string& ParticleList::GetStreamName(uint32_t streamIndex) const
{
  return mDataStreams[streamIndex]->streamName;
}

bool ParticleList::IsStreamLocal(uint32_t streamIndex) const
{
  return mDataStreams[streamIndex]->localStream;
}

uint32_t ParticleList::GetStreamDataTypeSize(uint32_t streamIndex) const
{
  return mDataStreams[streamIndex]->dataSize;
}

ParticleSystem::Particle ParticleList::NewParticle(float lifetime)
{
  if(mParticles.size() < mMaxParticleCount)
  {
    auto newIndex = int32_t(mFreeIndex);
    mFreeIndex    = int32_t(mFreeChain[mFreeIndex]);
    mAliveParticleCount++;

    // Add particle
    mParticles.emplace_back(new Internal::Particle(*this, newIndex));

    // Set particle lifetime
    auto& particle = mParticles.back();

    particle.Get<float>(ParticleStream::LIFETIME_STREAM_BIT) = lifetime;

    // Store initial lifetime
    particle.Get<float>(ParticleStream::LIFETIME_BASE_STREAM_BIT) = lifetime;

    return mParticles.back();
  }
  return {nullptr};
}

uint32_t ParticleList::GetStreamElementSize(bool includeLocalStream)
{
  if(includeLocalStream)
  {
    return mParticleStreamElementSizeWithLocal;
  }
  else
  {
    return mParticleStreamElementSize;
  }
}

void ParticleList::ReleaseParticle(uint32_t particleIndex)
{
  auto it = mParticles.begin();
  std::advance(it, particleIndex);

  // Point at this slot of memory as next free slot
  auto& p = *it;
  if(mFreeIndex > -1)
  {
    mFreeChain[p.GetIndex()] = mFreeIndex;
  }
  mFreeIndex = p.GetIndex();

  // Remove particle from the list
  mParticles.erase(it);
  mAliveParticleCount--;
}

void* ParticleList::GetDefaultStream(ParticleStreamTypeFlagBit streamBit)
{
  return GetRawStream(mBuiltInStreamMap[streamBit]);
}

uint32_t ParticleList::GetDefaultStreamIndex(ParticleStreamTypeFlagBit streamBit)
{
  return mBuiltInStreamMap[uint32_t(streamBit)];
}

std::list<ParticleSystem::Particle>& ParticleList::GetParticles()
{
  return mParticles;
}

} // namespace Dali::Toolkit::ParticleSystem::Internal
