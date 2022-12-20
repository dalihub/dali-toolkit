/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/helpers/property-helper.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(SHADER_HINT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Shader::Hint, NONE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Shader::Hint, OUTPUT_IS_TRANSPARENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Shader::Hint, MODIFIES_GEOMETRY)
DALI_ENUM_TO_STRING_TABLE_END(SHADER_HINT)

DALI_ENUM_TO_STRING_TABLE_BEGIN(ALIGN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, TOP_BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, TOP_CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, TOP_END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, CENTER_BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, CENTER_END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, BOTTOM_BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, BOTTOM_CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Align, BOTTOM_END)
DALI_ENUM_TO_STRING_TABLE_END(ALIGN)

DALI_ENUM_TO_STRING_TABLE_BEGIN(POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual::Transform::Policy, RELATIVE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::Visual::Transform::Policy, ABSOLUTE)
DALI_ENUM_TO_STRING_TABLE_END(POLICY)

Dali::Vector2 PointToVector2(Toolkit::Align::Type point, Toolkit::Direction::Type direction)
{
  // clang-format off
  static const float pointToVector2[] = {0.0f,0.0f,
                                         0.5f,0.0f,
                                         1.0f,0.0f,
                                         0.0f,0.5f,
                                         0.5f,0.5f,
                                         1.0f,0.5f,
                                         0.0f,1.0f,
                                         0.5f,1.0f,
                                         1.0f,1.0f};

  // clang-format on

  Vector2 result(&pointToVector2[point * 2]);
  if(direction == Direction::RIGHT_TO_LEFT)
  {
    result.x = 1.0f - result.x;
  }

  return result;
}

bool GetPolicyFromValue(const Property::Value& value, Vector2& policy)
{
  bool success = false;
  if(value.Get(policy))
  {
    success = true;
  }
  else
  {
    const Property::Array* array = value.GetArray();
    if(array && array->Size() == 2)
    {
      Toolkit::Visual::Transform::Policy::Type xPolicy = static_cast<Toolkit::Visual::Transform::Policy::Type>(-1); // Assign an invalid value so definitely changes
      Toolkit::Visual::Transform::Policy::Type yPolicy = static_cast<Toolkit::Visual::Transform::Policy::Type>(-1); // Assign an invalid value so definitely changes

      if(Scripting::GetEnumerationProperty<Toolkit::Visual::Transform::Policy::Type>(array->GetElementAt(0), POLICY_TABLE, POLICY_TABLE_COUNT, xPolicy) &&
         Scripting::GetEnumerationProperty<Toolkit::Visual::Transform::Policy::Type>(array->GetElementAt(1), POLICY_TABLE, POLICY_TABLE_COUNT, yPolicy))
      {
        policy.x = xPolicy;
        policy.y = yPolicy;
        success  = true;
      }
    }
  }
  return success;
}

} // unnamed namespace

Internal::Visual::Base::Impl::Impl(FittingMode fittingMode, Toolkit::Visual::Type type)
: mCustomShader(nullptr),
  mEventObserver(nullptr),
  mTransform(),
  mMixColor(Color::WHITE),
  mControlSize(Vector2::ZERO),
  mDecorationData(nullptr),
  mDepthIndex(0.0f),
  mFittingMode(fittingMode),
  mFlags(0),
  mResourceStatus(Toolkit::Visual::ResourceStatus::PREPARING),
  mType(type),
  mAlwaysUsingBorderline(false),
  mAlwaysUsingCornerRadius(false)
{
}

Internal::Visual::Base::Impl::~Impl()
{
  delete mCustomShader;
  if(mDecorationData)
  {
    delete mDecorationData;
  }
}

Internal::Visual::Base::Impl::CustomShader::CustomShader(const Property::Map& map)
: mGridSize(1, 1),
  mHints(Shader::Hint::NONE)
{
  SetPropertyMap(map);
}

void Internal::Visual::Base::Impl::CustomShader::SetPropertyMap(const Property::Map& shaderMap)
{
  mVertexShader.clear();
  mFragmentShader.clear();
  mGridSize = ImageDimensions(1, 1);
  mHints    = Shader::Hint::NONE;

  Property::Value* vertexShaderValue = shaderMap.Find(Toolkit::Visual::Shader::Property::VERTEX_SHADER, CUSTOM_VERTEX_SHADER);
  if(vertexShaderValue)
  {
    if(!GetStringFromProperty(*vertexShaderValue, mVertexShader))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a string\n", CUSTOM_VERTEX_SHADER);
    }
  }

  Property::Value* fragmentShaderValue = shaderMap.Find(Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, CUSTOM_FRAGMENT_SHADER);
  if(fragmentShaderValue)
  {
    if(!GetStringFromProperty(*fragmentShaderValue, mFragmentShader))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a string\n", CUSTOM_FRAGMENT_SHADER);
    }
  }

  Property::Value* subdivideXValue = shaderMap.Find(Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X, CUSTOM_SUBDIVIDE_GRID_X);
  if(subdivideXValue)
  {
    int subdivideX;
    if(!subdivideXValue->Get(subdivideX) || subdivideX < 1)
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a value greater than 1\n", CUSTOM_SUBDIVIDE_GRID_X);
    }
    else
    {
      mGridSize = ImageDimensions(subdivideX, mGridSize.GetY());
    }
  }

  Property::Value* subdivideYValue = shaderMap.Find(Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y, CUSTOM_SUBDIVIDE_GRID_Y);
  if(subdivideYValue)
  {
    int subdivideY;
    if(!subdivideYValue->Get(subdivideY) || subdivideY < 1)
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a value greater than 1\n", CUSTOM_SUBDIVIDE_GRID_Y);
    }
    else
    {
      mGridSize = ImageDimensions(mGridSize.GetX(), subdivideY);
    }
  }

  Property::Value* hintsValue = shaderMap.Find(Toolkit::Visual::Shader::Property::HINTS, CUSTOM_SHADER_HINTS);
  if(hintsValue)
  {
    if(!Scripting::GetBitmaskEnumerationProperty(*hintsValue, SHADER_HINT_TABLE, SHADER_HINT_TABLE_COUNT, mHints))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a hint or an array of hint strings\n", CUSTOM_SHADER_HINTS);
    }
  }
}

void Internal::Visual::Base::Impl::CustomShader::CreatePropertyMap(Property::Map& map) const
{
  if(!mVertexShader.empty() || !mFragmentShader.empty())
  {
    Property::Map customShader;
    if(!mVertexShader.empty())
    {
      customShader.Insert(Toolkit::Visual::Shader::Property::VERTEX_SHADER, mVertexShader);
    }
    if(!mFragmentShader.empty())
    {
      customShader.Insert(Toolkit::Visual::Shader::Property::FRAGMENT_SHADER, mFragmentShader);
    }

    if(mGridSize.GetWidth() != 1)
    {
      customShader.Insert(Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X, mGridSize.GetWidth());
    }
    if(mGridSize.GetHeight() != 1)
    {
      customShader.Insert(Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y, mGridSize.GetHeight());
    }

    if(mHints != Dali::Shader::Hint::NONE)
    {
      customShader.Insert(Toolkit::Visual::Shader::Property::HINTS, static_cast<int>(mHints));
    }

    map.Insert(Toolkit::Visual::Property::SHADER, customShader);
  }
}

Internal::Visual::Base::Impl::Transform::Transform()
: mOffset(0.0f, 0.0f),
  mSize(1.0f, 1.0f),
  mExtraSize(0.0f, 0.0f),
  mOffsetSizeMode(0.0f, 0.0f, 0.0f, 0.0f),
  mOrigin(Toolkit::Align::TOP_BEGIN),
  mAnchorPoint(Toolkit::Align::TOP_BEGIN)
{
}

void Internal::Visual::Base::Impl::Transform::SetPropertyMap(const Property::Map& map)
{
  // Set default values
  mOffset         = Vector2(0.0f, 0.0f);
  mSize           = Vector2(1.0f, 1.0f);
  mExtraSize      = Vector2(0.0f, 0.0f);
  mOffsetSizeMode = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  mOrigin         = Toolkit::Align::TOP_BEGIN;
  mAnchorPoint    = Toolkit::Align::TOP_BEGIN;

  UpdatePropertyMap(map);
}

void Internal::Visual::Base::Impl::Transform::UpdatePropertyMap(const Property::Map& map)
{
  for(Property::Map::SizeType i(0); i < map.Count(); ++i)
  {
    KeyValuePair keyValue = map.GetKeyValue(i);
    switch(Visual::Base::GetIntKey(keyValue.first))
    {
      case Toolkit::Visual::Transform::Property::OFFSET:
      {
        keyValue.second.Get(mOffset);
        break;
      }
      case Toolkit::Visual::Transform::Property::SIZE:
      {
        keyValue.second.Get(mSize);
        break;
      }
      case Toolkit::Visual::Transform::Property::ORIGIN:
      {
        Scripting::GetEnumerationProperty<Toolkit::Align::Type>(keyValue.second, ALIGN_TABLE, ALIGN_TABLE_COUNT, mOrigin);
        break;
      }
      case Toolkit::Visual::Transform::Property::ANCHOR_POINT:
      {
        Scripting::GetEnumerationProperty<Toolkit::Align::Type>(keyValue.second, ALIGN_TABLE, ALIGN_TABLE_COUNT, mAnchorPoint);
        break;
      }
      case Toolkit::Visual::Transform::Property::OFFSET_POLICY:
      {
        Vector2 policy;
        if(GetPolicyFromValue(keyValue.second, policy))
        {
          mOffsetSizeMode.x = policy.x;
          mOffsetSizeMode.y = policy.y;
        }
        break;
      }
      case Toolkit::Visual::Transform::Property::SIZE_POLICY:
      {
        Vector2 policy;
        if(GetPolicyFromValue(keyValue.second, policy))
        {
          mOffsetSizeMode.z = policy.x;
          mOffsetSizeMode.w = policy.y;
        }
        break;
      }
      case Toolkit::DevelVisual::Transform::Property::EXTRA_SIZE:
      {
        keyValue.second.Get(mExtraSize);
        break;
      }
    }
  }
}

void Internal::Visual::Base::Impl::Transform::GetPropertyMap(Property::Map& map) const
{
  map.Clear();
  map
    .Add(Toolkit::Visual::Transform::Property::OFFSET, mOffset)
    .Add(Toolkit::Visual::Transform::Property::SIZE, mSize)
    .Add(Toolkit::Visual::Transform::Property::ORIGIN, mOrigin)
    .Add(Toolkit::Visual::Transform::Property::ANCHOR_POINT, mAnchorPoint)
    .Add(Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2(mOffsetSizeMode.x, mOffsetSizeMode.y))
    .Add(Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2(mOffsetSizeMode.z, mOffsetSizeMode.w))
    .Add(Toolkit::DevelVisual::Transform::Property::EXTRA_SIZE, mExtraSize);
}

void Internal::Visual::Base::Impl::Transform::SetUniforms(Dali::VisualRenderer renderer, Toolkit::Direction::Type direction)
{
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, mSize);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET,
                       direction == Toolkit::Direction::LEFT_TO_RIGHT ? mOffset : mOffset * Vector2(-1.0f, 1.0f));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, mOffsetSizeMode);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ORIGIN, PointToVector2(mOrigin, direction) - Vector2(0.5, 0.5));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ANCHOR_POINT,
                       Vector2(0.5, 0.5) - PointToVector2(mAnchorPoint, direction));
  renderer.SetProperty(VisualRenderer::Property::EXTRA_SIZE, mExtraSize);
}

Vector2 Internal::Visual::Base::Impl::Transform::GetVisualSize(const Vector2& controlSize)
{
  return Vector2(Lerp(mOffsetSizeMode.z, mSize.x * controlSize.x, mSize.x),
                 Lerp(mOffsetSizeMode.w, mSize.y * controlSize.y, mSize.y)) +
         mExtraSize;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
