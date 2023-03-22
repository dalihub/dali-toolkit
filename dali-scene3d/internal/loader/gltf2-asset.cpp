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

// EXTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <algorithm>
#include <map>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/gltf2-asset.h>

using namespace Dali;

namespace gltf2
{
namespace
{
constexpr uint32_t ACCESSOR_TYPE_ELEMENT_COUNT[]{
  1,
  2,
  3,
  4,
  4,
  9,
  16,
  static_cast<uint32_t>(-1)};

const std::map<std::string_view, AccessorType::Type>& GetAccessorTypes()
{
  static const std::map<std::string_view, AccessorType::Type> ACCESSOR_TYPES{
    ENUM_STRING_MAPPING(AccessorType, SCALAR),
    ENUM_STRING_MAPPING(AccessorType, VEC2),
    ENUM_STRING_MAPPING(AccessorType, VEC3),
    ENUM_STRING_MAPPING(AccessorType, VEC4),
    ENUM_STRING_MAPPING(AccessorType, MAT2),
    ENUM_STRING_MAPPING(AccessorType, MAT3),
    ENUM_STRING_MAPPING(AccessorType, MAT4),
  };
  return ACCESSOR_TYPES;
}

const std::map<std::string_view, AlphaMode::Type>& GetAlphaModeTypes()
{
  static const std::map<std::string_view, AlphaMode::Type> ALPHA_MODE_TYPES{
    ENUM_STRING_MAPPING(AlphaMode::Type, OPAQUE),
    ENUM_STRING_MAPPING(AlphaMode::Type, MASK),
    ENUM_STRING_MAPPING(AlphaMode::Type, BLEND),
  };
  return ALPHA_MODE_TYPES;
}

const std::map<std::string_view, Attribute::Type>& GetAttributeTypes()
{
  static const std::map<std::string_view, Attribute::Type> ATTRIBUTE_TYPES{
    ENUM_STRING_MAPPING(Attribute::Type, POSITION),
    ENUM_STRING_MAPPING(Attribute::Type, NORMAL),
    ENUM_STRING_MAPPING(Attribute::Type, TANGENT),
    ENUM_STRING_MAPPING(Attribute::Type, TEXCOORD_0),
    ENUM_STRING_MAPPING(Attribute::Type, TEXCOORD_1),
    ENUM_STRING_MAPPING(Attribute::Type, COLOR_0),
    ENUM_STRING_MAPPING(Attribute::Type, JOINTS_0),
    ENUM_STRING_MAPPING(Attribute::Type, WEIGHTS_0),
  };
  return ATTRIBUTE_TYPES;
}

const std::map<std::string_view, Animation::Sampler::Interpolation::Type>& GetAnimationSamplerInterpolation()
{
  static const std::map<std::string_view, Animation::Sampler::Interpolation::Type> ANIMATION_SAMPLER_INTERPOLATION{
    ENUM_STRING_MAPPING(Animation::Sampler::Interpolation::Type, STEP),
    ENUM_STRING_MAPPING(Animation::Sampler::Interpolation::Type, LINEAR),
    ENUM_STRING_MAPPING(Animation::Sampler::Interpolation::Type, CUBICSPLINE),
  };
  return ANIMATION_SAMPLER_INTERPOLATION;
}

const std::map<std::string_view, Animation::Channel::Target::Type>& GetAnimationChannelTargetPathTypes()
{
  static const std::map<std::string_view, Animation::Channel::Target::Type> ANIMATION_CHANNEL_TARGET_PATH_TYPES{
    ENUM_STRING_MAPPING(Animation::Channel::Target::Type, TRANSLATION),
    ENUM_STRING_MAPPING(Animation::Channel::Target::Type, ROTATION),
    ENUM_STRING_MAPPING(Animation::Channel::Target::Type, SCALE),
    ENUM_STRING_MAPPING(Animation::Channel::Target::Type, WEIGHTS),
  };
  return ANIMATION_CHANNEL_TARGET_PATH_TYPES;
}

} // namespace

ENUM_TYPE_FROM_STRING(AccessorType, GetAccessorTypes())
ENUM_TYPE_FROM_STRING(AlphaMode, GetAlphaModeTypes())
ENUM_TYPE_FROM_STRING(Attribute, GetAttributeTypes())
ENUM_TYPE_FROM_STRING(Animation::Sampler::Interpolation, GetAnimationSamplerInterpolation())
ENUM_TYPE_FROM_STRING(Animation::Channel::Target, GetAnimationChannelTargetPathTypes())

bool Component::IsUnsigned(Type t)
{
  return t == UNSIGNED_BYTE || t == UNSIGNED_SHORT || t == UNSIGNED_INT;
}

uint32_t Component::Size(Type t)
{
  switch(t)
  {
    case BYTE:
    case UNSIGNED_BYTE:
      return 1;
    case SHORT:
    case UNSIGNED_SHORT:
      return 2;
    case UNSIGNED_INT:
    case FLOAT:
      return 4;
    default:
      return -1;
  }
}

uint32_t AccessorType::ElementCount(Type t)
{
  return ACCESSOR_TYPE_ELEMENT_COUNT[t];
}

uint32_t ComponentTypedBufferViewClient::GetBytesPerComponent() const
{
  return Component::Size(mComponentType);
}

void Node::SetMatrix(const Matrix& m)
{
  m.GetTransformComponents(mTranslation, mRotation, mScale);
}

Quaternion ReadQuaternion(const json_value_s& j)
{
  return Quaternion(ReadDaliVector<Vector4>(j));
}

} // namespace gltf2
