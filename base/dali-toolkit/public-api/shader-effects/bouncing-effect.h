#ifndef __DALI_TOOLKIT_SHADER_EFFECT_BOUNCING_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_BOUNCING_H__

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
 * @brief BouncingEffect is a custom overscroll effect with two waves appearing at two sides then moving towards center and overlapping.
 *
 * Usage Example:
 *
 *  // Create the an imageActor, set shader effect, and add it to the stage
 *  ImageActor imageActor = ImageActor::New( BitmapImage::New( 1, 1 ) );
 *  imageActor.SetSize(720.f,58.f);
 *  Toolkit::BouncingEffect bouncingEffect = Toolkit::BouncingEffect::New( Vector4(0.f,1.f,1.f,0.5f) );
 *  imageActor.SetShaderEffect( bouncingEffect );
 *  imageActor.SetParentOrigin( ParentOrigin::CENTER );
 *  Stage::GetCurrent().Add( imageActor );
 *
 *   // Start the animation
 *   Animation animation = Animation::New(1.f);
 *   animation.AnimateTo( Property( bouncingEffect, bouncingEffect.GetProgressRatePropertyName() ),
 *                        1.f, AlphaFunctions::Bounce );
 *   animation.Play();
 */
class BouncingEffect : public ShaderEffect
{
public:

  /**
   * @brief Creates an empty BouncingEffect handle
   */
  BouncingEffect();

  /**
   * @brief Virtual destructor
   */
  virtual ~BouncingEffect();

  /**
   * @brief Create a BouncingEffect object
   *
   * @param[in] color The color used on the bouncing stripe
   * @return A handle to a newly allocated Dali resource.
   */
  static BouncingEffect New( const Vector4& color );

  /**
   * @brief Set the progress rate to the effect.
   *
   * The whole progress ( with progress rate from 0.0 to 1.0 ):
   *      two waves appear at two sides; move towards center and overlap.
   * @param[in] progressRate The progress rate value.
   */
  void SetProgressRate( float progressRate );

  /**
   * @brief Get the name for the progress rate property.
   *
   * @return A std::string containing the property name.
   */
  const std::string& GetProgressRatePropertyName() const;


private: // Not intended for application developers
  BouncingEffect( ShaderEffect handle );

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_BOUNCING_H__
