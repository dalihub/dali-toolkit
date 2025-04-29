#ifndef DALI_TOOLKIT_CONTROL_DECORATION_DATA_H
#define DALI_TOOLKIT_CONTROL_DECORATION_DATA_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali::Toolkit::Internal
{
struct DecorationData
{
  // Default constructor
  DecorationData()
  : mBorderlineColor(Color::BLACK),
    mCornerRadius(Vector4::ZERO),
    mCornerSquareness(Vector4::ZERO),
    mBorderlineWidth(0.0f),
    mBorderlineOffset(0.0f),
    mCornerRadiusPolicy(static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE))
  {
  }
  Vector4 mBorderlineColor;
  Vector4 mCornerRadius;
  Vector4 mCornerSquareness;
  float   mBorderlineWidth;
  float   mBorderlineOffset;
  int     mCornerRadiusPolicy;

  /**
   * @brief Helper API to reduce duplicated codes.
   *
   * Keep these API as inline function due to the performance.
   */
  static DecorationData* EnsureDecorationData(DecorationData*& data)
  {
    if(data == nullptr)
    {
      data = new DecorationData();
    }
    return data;
  }

  static float GetBorderlineWidth(const DecorationData* const data)
  {
    return data ? data->mBorderlineWidth : 0.0f;
  }
  static void SetBorderlineWidth(DecorationData*& data, float value)
  {
    EnsureDecorationData(data)->mBorderlineWidth = value;
  }

  static Vector4 GetBorderlineColor(const DecorationData* const data)
  {
    return data ? data->mBorderlineColor : Color::BLACK;
  }
  static void SetBorderlineColor(DecorationData*& data, Vector4 value)
  {
    EnsureDecorationData(data)->mBorderlineColor = value;
  }

  static float GetBorderlineOffset(const DecorationData* const data)
  {
    return data ? data->mBorderlineOffset : 0.0f;
  }
  static void SetBorderlineOffset(DecorationData*& data, float value)
  {
    EnsureDecorationData(data)->mBorderlineOffset = value;
  }

  static Vector4 GetCornerRadius(const DecorationData* const data)
  {
    return data ? data->mCornerRadius : Vector4::ZERO;
  }
  static void SetCornerRadius(DecorationData*& data, Vector4 value)
  {
    EnsureDecorationData(data)->mCornerRadius = value;
  }

  static int GetCornerRadiusPolicy(const DecorationData* const data)
  {
    return data ? data->mCornerRadiusPolicy : static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE);
  }
  static void SetCornerRadiusPolicy(DecorationData*& data, int value)
  {
    EnsureDecorationData(data)->mCornerRadiusPolicy = value;
  }

  static Vector4 GetCornerSquareness(const DecorationData* const data)
  {
    return data ? data->mCornerSquareness : Vector4::ZERO;
  }
  static void SetCornerSquareness(DecorationData*& data, Vector4 value)
  {
    EnsureDecorationData(data)->mCornerSquareness = value;
  }
};

} // namespace Dali::Toolkit::Internal

#endif // DALI_TOOLKIT_CONTROL_DECORATION_DATA_H
