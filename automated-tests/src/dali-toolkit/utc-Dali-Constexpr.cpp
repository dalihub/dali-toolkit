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
#include <dali-test-suite-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>

namespace
{
constexpr bool CheckValues()
{
  using namespace Dali::Toolkit;
  using namespace Dali::Toolkit::ParticleSystem;
  using namespace Dali::Toolkit::ParticleSystem::ParticleStream;
  ParticleList::ParticleStreamTypeFlags flags(POSITION_STREAM_BIT);
  ParticleList::ParticleStreamTypeFlags flagsCopy(flags);
  flags |= COLOR_STREAM_BIT;
  if(!static_cast<bool>(flagsCopy) || static_cast<uint32_t>(flags & COLOR_STREAM_BIT) != COLOR_STREAM_BIT ||
     StreamDataTypeWrapper<int>::GetType() != ParticleStream::StreamDataType{})
  {
    return false;
  }
  ItemRange first(1u, 5u);
  ItemRange copy(first);
  ItemRange other(3u, 8u);
  copy = other;
  copy = copy;
  if(first.Intersection(other).begin != 3u || first.Intersection(other).end != 5u ||
     first.Intersection(ItemRange(8u, 9u)).end != 0u ||
     !first.Within(1u) || first.Within(5u) || first.Within(0u) || copy.begin != 3u)
  {
    return false;
  }
  const RulerDomain domain(1.f, 5.f);
  const RulerDomain disabled(1.f, 5.f, false);
  ClampState        clamped = NOT_CLAMPED;
  if(disabled.Clamp(9.f, 0.f, 1.f, clamped) != 9.f || clamped != NOT_CLAMPED)
  {
    return false;
  }
  if(domain.Clamp(0.f, 0.f, 1.f, clamped) != 1.f || clamped != CLAMPED_TO_MIN)
  {
    return false;
  }
  if(domain.Clamp(9.f, 1.f, 2.f, clamped) != 9.f || clamped != NOT_CLAMPED)
  {
    return false;
  }
  if(domain.Clamp(10.f, 1.f, 2.f, clamped) != 9.f || clamped != CLAMPED_TO_MAX)
  {
    return false;
  }
  return domain.GetSize() == 4.f && domain.Clamp(3.f) == 3.f;
}
static_assert(CheckValues(), "Value operations must support C++17 constant evaluation");
} //namespace

int UtcDaliToolkitConstexprValueTypes(void)
{
  DALI_TEST_CHECK(CheckValues());
  END_TEST;
}

namespace
{
using StreamType = Dali::Toolkit::ParticleSystem::ParticleStream::StreamDataType;
using namespace Dali::Toolkit::ParticleSystem;
static_assert(StreamDataTypeWrapper<float>::GetType() == StreamType::FLOAT);
static_assert(StreamDataTypeWrapper<Dali::Vector2>::GetType() == StreamType::FLOAT2);
static_assert(StreamDataTypeWrapper<Dali::Vector3>::GetType() == StreamType::FLOAT3);
static_assert(StreamDataTypeWrapper<Dali::Vector4>::GetType() == StreamType::FLOAT4);
} //namespace

int UtcDaliParticleStreamDataTypeConstexpr(void)
{
  using GetTypeFunction = StreamType (*)();
  GetTypeFunction volatile functions[]{&StreamDataTypeWrapper<float>::GetType,
                                       &StreamDataTypeWrapper<Dali::Vector2>::GetType,
                                       &StreamDataTypeWrapper<Dali::Vector3>::GetType,
                                       &StreamDataTypeWrapper<Dali::Vector4>::GetType};
  const StreamType expected[]{StreamType::FLOAT, StreamType::FLOAT2, StreamType::FLOAT3, StreamType::FLOAT4};
  for(unsigned int index = 0; index < 4; ++index)
  {
    DALI_TEST_CHECK(functions[index]() == expected[index]);
  }
  END_TEST;
}
