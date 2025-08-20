#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H

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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/rendering/visual-renderer.h>
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/controls/control/control-decoration-data.h>
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
    IS_SYNCHRONOUS_RESOURCE_LOADING = 1 << 3,
  };

  struct CustomShader
  {
    CustomShader(const Property::Map& map);
    void SetPropertyMap(const Property::Map& map);
    Property::Map CreatePropertyMap() const;

    std::string               mVertexShader;
    std::string               mFragmentShader;
    Dali::ImageDimensions     mGridSize;
    Dali::Shader::Hint::Value mHints; //(bitfield) values from enum Shader::Hint
    int32_t                   mRenderPassTag;
    std::string               mName;
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

    /**
     * Get property maps for the default transform.
     */
    static const Property::Map& GetDefaultTransformMap();

    Vector2              mOffset;
    Vector2              mSize;
    Vector2              mExtraSize;
    Vector4              mOffsetSizeMode;
    Toolkit::Align::Type mOrigin;
    Toolkit::Align::Type mAnchorPoint;
  };

  /**
   * @brief Adds new Custom Shader for the visual.
   */
  void AddCustomShader(const Property::Map& shaderMap)
  {
    mCustomShaders.emplace_back(std::make_unique<CustomShader>(shaderMap));
  }

  /**
   * @brief Clears existed Custom Shaders
   */
  void ClearCustomShader()
  {
    mCustomShaders.clear();
  }

  /**
   * @brief Gets Custom Shader Count
   */
  uint32_t GetCustomShaderCount() const
  {
    return mCustomShaders.size();
  }

  /**
   * @brief Retrieves Custom Shadet at the index.
   */
  const Visual::Base::Impl::CustomShader* GetCustomShaderAt(uint32_t index) const
  {
    if(index < mCustomShaders.size())
    {
      return mCustomShaders[index].get();
    }
    return nullptr;
  }

  /**
   * @brief Creates and inserts PropertyMap of the Custom Shaders.
   * @param[in, out] map property map to insert custom shader properties.
   */
  void CreateCustomShaderPropertyMap(Property::Map& map)
  {
    if(mCustomShaders.empty())
    {
      return;
    }

    if(mCustomShaders.size() == 1)
    {
      map.Insert(Toolkit::Visual::Property::SHADER, mCustomShaders[0]->CreatePropertyMap());
    }
    else
    {
      Property::Array shaderArray;
      for(auto && customShader : mCustomShaders)
      {
        shaderArray.PushBack(customShader->CreatePropertyMap());
      }
      map.Insert(Toolkit::Visual::Property::SHADER, shaderArray);
    }
  }

  /**
   * @brief Ensure to create and get the transform data for the visual.
   */
  Transform& GetOrCreateTransform()
  {
    if(DALI_UNLIKELY(!mTransform))
    {
      mTransform.reset(new Transform());
    }
    return *(mTransform.get());
  }

  /**
   * @brief Set the uniform properties onto the renderer.
   * And Register visual transform uniforms if neccessary.
   */
  void SetTransformUniforms(VisualRenderer renderer, Toolkit::Direction::Type direction)
  {
    if(!mTransformMapUsingDefault || direction != Toolkit::Direction::LEFT_TO_RIGHT)
    {
      renderer.RegisterVisualTransformUniform();
      GetOrCreateTransform().SetUniforms(renderer, direction);
    }
  }

  /**
   * Convert the control size and the transform attributes into the actual
   * size of the visual.
   */
  Vector2 GetTransformVisualSize(const Vector2& controlSize)
  {
    if(!mTransformMapUsingDefault && mTransform)
    {
      return mTransform->GetVisualSize(controlSize);
    }
    return controlSize;
  }

  /**
   * @brief Get decoration data value : borderline width
   *
   * Keep these API as inline function due to the performance.
   */
  float GetBorderlineWidth() const
  {
    return DecorationData::GetBorderlineWidth(mDecorationData);
  }

  /**
   * @brief Set decoration data value : borderline width
   *
   * Keep these API as inline function due to the performance.
   */
  void SetBorderlineWidth(float value)
  {
    DecorationData::SetBorderlineWidth(mDecorationData, value);
  }

  /**
   * @brief Get decoration data value : borderline color
   *
   * Keep these API as inline function due to the performance.
   */
  Vector4 GetBorderlineColor() const
  {
    return DecorationData::GetBorderlineColor(mDecorationData);
  }

  /**
   * @brief Set decoration data value : borderline color
   *
   * Keep these API as inline function due to the performance.
   */
  void SetBorderlineColor(Vector4 value)
  {
    DecorationData::SetBorderlineColor(mDecorationData, value);
  }

  /**
   * @brief Get decoration data value : borderline offset
   *
   * Keep these API as inline function due to the performance.
   */
  float GetBorderlineOffset() const
  {
    return DecorationData::GetBorderlineOffset(mDecorationData);
  }

  /**
   * @brief Set decoration data value : borderline offset
   *
   * Keep these API as inline function due to the performance.
   */
  void SetBorderlineOffset(float value)
  {
    DecorationData::SetBorderlineOffset(mDecorationData, value);
  }

  /**
   * @brief Get decoration data value : corner radius
   *
   * Keep these API as inline function due to the performance.
   */
  Vector4 GetCornerRadius() const
  {
    return DecorationData::GetCornerRadius(mDecorationData);
  }

  /**
   * @brief Set decoration data value : corner radius
   *
   * Keep these API as inline function due to the performance.
   */
  void SetCornerRadius(Vector4 value)
  {
    DecorationData::SetCornerRadius(mDecorationData, value);
  }

  /**
   * @brief Get decoration data value : corner radius policy
   *
   * Keep these API as inline function due to the performance.
   */
  int GetCornerRadiusPolicy() const
  {
    return DecorationData::GetCornerRadiusPolicy(mDecorationData);
  }

  /**
   * @brief Set decoration data value : corner radius policy
   *
   * Keep these API as inline function due to the performance.
   */
  void SetCornerRadiusPolicy(int value)
  {
    DecorationData::SetCornerRadiusPolicy(mDecorationData, value);
  }

  /**
   * @brief Get decoration data value : corner squareness
   *
   * Keep these API as inline function due to the performance.
   */
  Vector4 GetCornerSquareness() const
  {
    return DecorationData::GetCornerSquareness(mDecorationData);
  }

  /**
   * @brief Set decoration data value : corner squareness
   *
   * Keep these API as inline function due to the performance.
   */
  void SetCornerSquareness(Vector4 value)
  {
    DecorationData::SetCornerSquareness(mDecorationData, value);
  }

  VisualRenderer                             mRenderer;
  std::vector<std::unique_ptr<CustomShader>> mCustomShaders;
  EventObserver*                             mEventObserver; ///< Allows controls to observe when the visual has events to notify
  std::string                                mName;
  std::unique_ptr<Transform>                 mTransform;
  Vector4                                    mMixColor;
  Size                                       mControlSize;
  DecorationData*                            mDecorationData;
  int                                        mDepthIndex;
  FittingMode                                mFittingMode; ///< How the contents should fit the view
  int                                        mFlags;
  Toolkit::Visual::ResourceStatus            mResourceStatus;
  const Toolkit::Visual::Type                mType;

  bool mAlwaysUsingBorderline : 1;         ///< Whether we need the borderline in shader always.
  bool mAlwaysUsingCornerRadius : 1;       ///< Whether we need the corner radius in shader always.
  bool mAlwaysUsingCornerSquareness : 1;   ///< Whether we need the corner squareness in shader always.
  bool mIgnoreFittingMode : 1;             ///< Whether we need to ignore fitting mode.
  bool mPixelAreaSetByFittingMode : 1;     ///< Whether the pixel area is set for fitting mode.
  bool mTransformMapSetForFittingMode : 1; ///< Whether the transformMap is set for fitting mode.
  bool mTransformMapUsingDefault : 1;      ///< Whether we are using the default transformMap not. We'll be false after SetTransform called, or animated.
                                           ///< Note : If it change to false, never be true again.
  bool mTransformMapChanged : 1;           ///< Whether the transformMap is changed or not. We'll be false after SetTransform called.
};

} // namespace Visual

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
