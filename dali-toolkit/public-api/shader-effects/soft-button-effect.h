#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SOFT_BUTTON_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SOFT_BUTTON_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 *
 * Class for soft button shader that works on a per object basis. Using animatable parameters user can create effect of button pushing in / out. Can be applied to ImageActor only.
 *
 * Usage example:-
 *
 * // Create shader used for doing soft button\n
 * SoftButtonEffect softButtonEffect = SoftButtonEffect::New();
 *
 * // set image actor shader to the soft button one\n
 * ImageActor imageActor = ImageActor::New( ... );\n
 * imageActor.SetShaderEffect( softButtonEffect );
 *
 * // animate a button push, using e.g. AlphaFunctions::Bounce. With these values the button pushes in and pops out slightly at the end\n
 * Animation animation = Animation::New( ... );\n
 * animation.AnimateTo( Property(softButtonEffect, softButtonEffect.GetLightingIndentationAmountPropertyName()), 0.25f, AlphaFunctions::Bounce, ... );\n
 * animation.AnimateTo( Property(softButtonEffect, softButtonEffect.GetLightingIndentationAmountPropertyName()), -0.05f, AlphaFunctions::Bounce, ... );\n
 * animation.AnimateTo( Property(softButtonEffect, softButtonEffect.GetTextureDistortionAmountPropertyName()), 0.25f, AlphaFunctions::Bounce, ... );\n
 * animation.AnimateTo( Property(softButtonEffect, softButtonEffect.GetTextureDistortionAmountPropertyName()), -0.05f, AlphaFunctions::Bounce, ... );\n
 * animation.Play();\n
 *
 */
class DALI_IMPORT_API SoftButtonEffect : public ShaderEffect
{

public:

  /**
   * Create an uninitialized SoftButtonEffect; this can be initialized with SoftButtonEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  SoftButtonEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SoftButtonEffect();

  typedef enum
  {
    ELLIPTICAL = 0,   /// Button is elliptical
    RECTANGULAR,      /// Button is rectangular
    FIXED             /// Button does not indent (move). Useful for matching lighting between areas that do not indent (which can thus use a cheaper shader) and those that do indent.
  }Type;

  /**
   * Create an initialized SoftButtonEffect
   * @param type The type of the soft button, can be either ELLIPTICAL, RECTANGULAR, or FIXED.
   * @return A handle to a newly allocated Dali resource.
   */
  static SoftButtonEffect New(Type type);

  /**
   * Get the name for the lighting indentation amount property (float). Useful for animation.
   * This property changes the lighting, to make it look like the button is pushed in. User should animate this in conjunction
   * with texture distortion. Allowable values range from [-1..1], higher values give more change in lighting. Default 0.0 (no lighting change).
   * See also GetTextureDistortionAmountPropertyName().
   * @return A std::string containing the property name
   */
  const std::string& GetLightingIndentationAmountPropertyName() const;

  /**
   * Get the name for the texture distortion amount property (float). Useful for animation.
   * This property changes the distortion, to make it look like the button is pushed in. User should animate this is conjunction
   * with lighting indentation. Allowable values range from [-1..1) - note 1.0 is NOT allowed - higher values give more distortion. Default 0.0 (no distortion).
   * See also GetLightingIndentationAmountPropertyName().
   * @return A std::string containing the property name
   */
  const std::string& GetTextureDistortionAmountPropertyName() const;

  /**
   * Get the name for the ambient lighting amount property (float)
   * The ambient light is used in the lighting calculation. Care must be taken to not saturate the image by setting this value too high,
   * or the indentation will not look correct. Default 0.15.
   * @return A std::string containing the property name
   */
  const std::string& GetAmbientLightAmountPropertyName() const;

  /**
   * Get the name for the diffuse light property (Vector3).
   * The diffuse light is used in the lighting calculation. Default is (0.0, 0.7070168, 0.7070168), i.e. a light angled at the surface from in front and above. Note that
   * you need to Normalize() the Vector3 that you set with this property.
   * @return A std::string containing the property name
   */
  const std::string& GetDiffuseLightPropertyName() const;

  /**
   * Get the name for the lighting multiplier property (float).
   * The ambient and diffuse lighting is multiplied by this factor. Since a diffuse light at an angle will cause the whole image to darken, even outside the soft button
   * indentation, this property can be used to scale the image back up closer to the pixel values of the original diffuse texture. Care must be taken to not saturate the image,
   * or the indentation will not look correct. Default 1.2.
   * @return A std::string containing the property name
   */
  const std::string& GetLightingMultiplierPropertyName() const;

  /**
   * Get the name for the inside shape size scale property (float).
   * The SoftButtonEffect consists of two shapes, one inside the other. The outside shape fits exactly to the actor, touching its edges but completely contained. The inside
   * shape size is given by a multiplier of the outside shape size. For example a value of 0.5 means that the inside shape is half the size of the outside one. Allowable
   * values are in the range (0.0 - 1.0), note that 0.0 and 1.0 themselves are not allowed. Default 0.75.
   * See also GetOutsideShapeDepthPropertyName().
   * @return A std::string containing the property name
   */
  const std::string& GetInsideShapeSizeScalePropertyName() const;

  /**
   * Get the name for the outside shape depth property (float).
   * The SoftButtonEffect consists of two shapes, one inside the other. The depth of the indentation at the transition between the inside and outside shapes is controlled by
   * this property. The values lies in the range [0.0 - 1.0]. A value of 0.0 means the outside shape has no depth (and is thus invisible), value of 1.0 means the outside shape
   * has maximum depth (and the inside shape is thus invisible). Default 0.05.
   * See also GetInsideShapeSizeScalePropertyName().
   * @return A std::string containing the property name
   */
  const std::string& GetOutsideShapeDepthPropertyName() const;

  /**
   * Get the name for the effect pixel area property (Vector4).
   * The soft button effect is applied within the supplied rect region of the texture. Default values for this is (0.0, 0.0, 1.0, 1.0) which is the entire image with
   * 0,0 being the top left and 1.0, 1.0 being the bottom right. If the image texture is split between multiple ImageActors then the developer should specify the
   * pixel area of the texture the effect should be applied with. Example, If the Image is split among two ImageActors side by side, with the left one using left half of the
   * texture and right one using the right half of the texture then the pixel area value for the left ImageActor will be (0.0, 0.0, 0.5, 1.0) and the pixel area for the right
   * will be (0.5, 0.0, 1.0, 1.0).
   * @return A std::string containing the property name
   */
  const std::string& GetEffectPixelAreaPropertyName() const;

  /**
   * Get the name for the rectangle size scale property (float). Only applicable having created this SoftButtonEffect via a call to New() with RECTANGULAR as the type.
   * This property can be used to set the mix between proportion of rectangle and proportion of ellipse - the result is a rectangle with rounded corners. If the value is 0.0,
   * the shape is an ellipse. If the value is close to 1.0, the shape is close to a rectangle. The value lies in the range [0.0 - 1.0). Note that a value of 1.0 is NOT allowed.
   * Default 0.5.
   * @return A std::string containing the property name
   */
  const std::string& GetRectangleSizeScalePropertyName() const;


private:
  // Not intended for application developers
  DALI_INTERNAL SoftButtonEffect(ShaderEffect handle);
};

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SOFT_BUTTON_H__
