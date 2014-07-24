#ifndef __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
#define __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

/**
 * @addtogroup CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * @brief ImageRegionEffect is a custom shader effect to show only a region of an Image actor.
 */
class ImageRegionEffect : public ShaderEffect
{
public:

  /**
   * @brief Create an uninitialized ImageRegionEffect; this can be initialized with ImageRegionEffect::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ImageRegionEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ImageRegionEffect();

  /**
   * @brief Create an initialized ImageRegionEffect.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ImageRegionEffect New();

  /**
   * @brief Set the top-left corner of the image region.
   *
   * The coordinates are in percentage, (0,0) being the top-left and (1,1) the bottom right of the original image.
   * @param [in] point The top-left corner of the region.
   */
  void SetTopLeft(const Vector2& point);

  /**
   * @brief Set the bottom-right corner of the image region.
   *
   * The coordinates are in percentage, (0,0) being the top-left and (1,1) the bottom right of the original image.
   * @param [in] point The bottom-right corner of the region.
   */
  void SetBottomRight(const Vector2& point);

  /**
   * @brief Get the name for the top-left point property.
   *
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetTopLeftPropertyName() const;

  /**
   * @brief Get the name for the bottom-right point property which can be used in Animation APIs.
   *
   * @return A std::string containing the property name
   */
  const std::string& GetBottomRightPropertyName() const;

private: // Not intended for application developers
  ImageRegionEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_IMAGE_REGION_EFFECT_H__
