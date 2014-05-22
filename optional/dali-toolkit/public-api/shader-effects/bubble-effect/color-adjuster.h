#ifndef __DALI_TOOLKIT_SHADER_COLOR_ADJUSTER_H__
#define __DALI_TOOLKIT_SHADER_COLOR_ADJUSTER_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * ColorAdjuster is a custom shader effect to adjust the image color in HSV space.
 */
class ColorAdjuster : public ShaderEffect
{
public:

  /**
   * Create an empty ColorAdjuster handle.
   */
  ColorAdjuster();

  /**
   * Virtual destructor.
   */
  virtual ~ColorAdjuster();

  /**
   * Create an initialized ColorAdjuster.
   * @param[in] hsvDelta The color difference to apply to the HSV channel.
   * @param[in] ignoreAlpha If true, the result color will be opaque even though source has alpha value
   * @return A handle to a newly allocated Dali resource.
   */
  static ColorAdjuster New( const Vector3& hsvDelta, bool ignoreAlpha = false );

   /**
   * Get the name of the uHSVDelta uniform so that it can be animated
   */
  std::string GetHsvDeltaPropertyName() const;

private: // Not intended for application developers

ColorAdjuster( ShaderEffect handle );

};

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_COLOR_ADJUSTER_EFFECT_H__ */
