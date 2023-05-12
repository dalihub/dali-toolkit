#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_TYPES_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_TYPES_H
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

// EXTERNAL_INCLUDES
#include <cinttypes>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali::Toolkit::ParticleSystem
{
using ParticleStreamTypeFlagBit = uint32_t;

namespace ParticleStream DALI_TOOLKIT_API
{
constexpr ParticleStreamTypeFlagBit POSITION_STREAM_BIT = 1 << 0; ///< 3D Position stream
constexpr ParticleStreamTypeFlagBit ROTATION_STREAM_BIT = 1 << 1; ///< 3D Rotation stream
constexpr ParticleStreamTypeFlagBit SCALE_STREAM_BIT    = 1 << 2; ///< 3D Scale stream
constexpr ParticleStreamTypeFlagBit SIZE_STREAM_BIT     = 1 << 3;
constexpr ParticleStreamTypeFlagBit COLOR_STREAM_BIT    = 1 << 4;
constexpr ParticleStreamTypeFlagBit OPACITY_STREAM_BIT  = 1 << 5;
constexpr ParticleStreamTypeFlagBit VELOCITY_STREAM_BIT = 1 << 6;
constexpr ParticleStreamTypeFlagBit LIFETIME_STREAM_BIT = 1 << 7;
constexpr ParticleStreamTypeFlagBit ALL_STREAMS         = 255;
constexpr ParticleStreamTypeFlagBit DEFAULT_STREAMS     = 255; ///< Default streams

/**
 * Stream data types
 */
enum class StreamDataType
{
  FLOAT,
  FLOAT2,
  FLOAT3,
  FLOAT4,
  INT,
  INT2,
  INT3,
  INT4
};

} // namespace ParticleStream DALI_TOOLKIT_API
} // namespace Dali::Toolkit::ParticleSystem
#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_TYPES_H