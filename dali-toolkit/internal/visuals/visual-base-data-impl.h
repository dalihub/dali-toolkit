#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/rendering/visual-renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-event-observer.h>
#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace Visual
{
struct Base::Impl
{
  /**
   * Constructor
   * @param [in] fittingMode that the derived class prefers
   * @param [in] type The type of the this visual
   */
  Impl(FittingMode fittingMode, Toolkit::Visual::Type type);

  /**
   * Destructor
   */
  ~Impl();

  enum Flags
  {
    IS_ON_SCENE                     = 1,
    IS_ATLASING_APPLIED             = 1 << 1,
    IS_PREMULTIPLIED_ALPHA          = 1 << 2,
    IS_SYNCHRONOUS_RESOURCE_LOADING = 1 << 3
  };

  struct CustomShader
  {
    CustomShader(const Property::Map& map);
    void SetPropertyMap(const Property::Map& map);
    void CreatePropertyMap(Property::Map& map) const;

    std::string               mVertexShader;
    std::string               mFragmentShader;
    Dali::ImageDimensions     mGridSize;
    Dali::Shader::Hint::Value mHints; //(bitfield) values from enum Shader::Hint
  };

  struct Transform
  {
    /**
     * Default constructor ensures the visual fills the control
     */
    Transform();

    /**
     * Use the property map to set zero or more of the transform
     * attributes, and sets the remaining attributes to their default
     * values.
     */
    void SetPropertyMap(const Property::Map& map);

    /**
     * Add the transform attributes to the map (using integer keys)
     */
    void GetPropertyMap(Property::Map& map) const;

    /**
     * Update zero or more attributes from the property map.
     */
    void UpdatePropertyMap(const Property::Map& map);

    /**
     * Set the uniform properties onto the renderer
     */
    void SetUniforms(VisualRenderer renderer, Toolkit::Direction::Type direction);

    /**
     * Convert the control size and the transform attributes into the actual
     * size of the visual.
     */
    Vector2 GetVisualSize(const Vector2& controlSize);

    Vector2              mOffset;
    Vector2              mSize;
    Vector2              mExtraSize;
    Vector4              mOffsetSizeMode;
    Toolkit::Align::Type mOrigin;
    Toolkit::Align::Type mAnchorPoint;
  };

  struct DecorationData
  {
    // Default constructor
    DecorationData()
    : mBorderlineColor(Color::BLACK),
      mCornerRadius(Vector4::ZERO),
      mBorderlineWidth(0.0f),
      mBorderlineOffset(0.0f),
      mCornerRadiusPolicy(static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE))
    {
    }
    Vector4 mBorderlineColor;
    Vector4 mCornerRadius;
    float   mBorderlineWidth;
    float   mBorderlineOffset;
    int     mCornerRadiusPolicy;
  };

  DecorationData* EnsureDecorationData()
  {
    if(mDecorationData == nullptr)
    {
      mDecorationData = new DecorationData();
    }
    return mDecorationData;
  }

  /**
   * @brief Get decoration data value : borderline width
   *
   * Keep these API as inline function due to the performance.
   */
  float GetBorderlineWidth()
  {
    return mDecorationData ? mDecorationData->mBorderlineWidth : 0.0f;
  }

  /**
   * @brief Set decoration data value : borderline width
   *
   * Keep these API as inline function due to the performance.
   */
  void SetBorderlineWidth(float value)
  {
    EnsureDecorationData()->mBorderlineWidth = value;
  }

  /**
   * @brief Get decoration data value : borderline color
   *
   * Keep these API as inline function due to the performance.
   */
  Vector4 GetBorderlineColor()
  {
    return mDecorationData ? mDecorationData->mBorderlineColor : Color::BLACK;
  }

  /**
   * @brief Set decoration data value : borderline color
   *
   * Keep these API as inline function due to the performance.
   */
  void SetBorderlineColor(Vector4 value)
  {
    EnsureDecorationData()->mBorderlineColor = value;
  }

  /**
   * @brief Get decoration data value : borderline offset
   *
   * Keep these API as inline function due to the performance.
   */
  float GetBorderlineOffset()
  {
    return mDecorationData ? mDecorationData->mBorderlineOffset : 0.0f;
  }

  /**
   * @brief Set decoration data value : borderline offset
   *
   * Keep these API as inline function due to the performance.
   */
  void SetBorderlineOffset(float value)
  {
    EnsureDecorationData()->mBorderlineOffset = value;
  }

  /**
   * @brief Get decoration data value : corner radius
   *
   * Keep these API as inline function due to the performance.
   */
  Vector4 GetCornerRadius()
  {
    return mDecorationData ? mDecorationData->mCornerRadius : Vector4::ZERO;
  }

  /**
   * @brief Set decoration data value : corner radius
   *
   * Keep these API as inline function due to the performance.
   */
  void SetCornerRadius(Vector4 value)
  {
    EnsureDecorationData()->mCornerRadius = value;
  }

  /**
   * @brief Get decoration data value : corner radius policy
   *
   * Keep these API as inline function due to the performance.
   */
  int GetCornerRadiusPolicy()
  {
    return mDecorationData ? mDecorationData->mCornerRadiusPolicy : static_cast<int>(Toolkit::Visual::Transform::Policy::ABSOLUTE);
  }

  /**
   * @brief Set decoration data value : corner radius policy
   *
   * Keep these API as inline function due to the performance.
   */
  void SetCornerRadiusPolicy(int value)
  {
    EnsureDecorationData()->mCornerRadiusPolicy = value;
  }

  VisualRenderer                  mRenderer;
  CustomShader*                   mCustomShader;
  EventObserver*                  mEventObserver; ///< Allows controls to observe when the visual has events to notify
  std::string                     mName;
  Transform                       mTransform;
  Vector4                         mMixColor;
  Size                            mControlSize;
  DecorationData*                 mDecorationData;
  int                             mDepthIndex;
  FittingMode                     mFittingMode; ///< How the contents should fit the view
  int                             mFlags;
  Toolkit::Visual::ResourceStatus mResourceStatus;
  const Toolkit::Visual::Type     mType;
  bool                            mAlwaysUsingBorderline : 1;   ///< Whether we need the borderline in shader always.
  bool                            mAlwaysUsingCornerRadius : 1; ///< Whether we need the corner radius in shader always.
};

} // namespace Visual

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
