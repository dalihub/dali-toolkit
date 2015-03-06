#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISPLACEMENT_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_DISPLACEMENT_H__

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
 * Class for two state displacement effect shader that works on a per object basis. By passing a height-normal map as an effect image, the user can create
 * various styles of buttons on an image actor. The shader requires two height-normal maps in one image, one for each state.
 *
 *    The normals and height information for the two states of the button should be strictly specified in this format:
 *     ______________
 *    |   State 0    |
 *    |              |
 *    |              | --> Unpressed button normals in rgb and height in a
 *    |     Map      |
 *    |______________|
 *    |   State 1    |
 *    |              |
 *    |              | --> Pressed button normals in rgb and height in a
 *    |     Map      |
 *    |______________|
 *
 *    The RGB values should contain the surface normals and the alpha should contian the height map. For a better effect keep the highest point (alpha value) in
 *    the combined map as 1.0 and the lowest posint as 0.0 and 0.5 for any region which doesn't need displacement.
 *
 *    For the supplied Normal map the Y-Axis should be down, Meaning (0,0) is in the top left. As the shader inverts the Y axis for lighting calculation.
 *
 *    Limitations: Can be applied to ImageActor only, And doesn't provide support for specular color.
 *
 * Usage example:-
 *
 * // Create shader used for doing soft button\n
 * DisplacementEffect buttonEffect = DisplacementEffect::New();
 * buttonEffect.SetEffectImage(Image::New( FANCY_BUTTON_HEIGHT_MAP_IMAGE_PATH ););
 *
 * // set shader to the soft button\n
 * ImageActor fancyButton = ImageActor::New( ... );\n
 * fancyButton.SetShaderEffect( buttonEffect );
 *
 * // animate a button push, using e.g. AlphaFunctions::Bounce. With these values the button pushes in and out (animates to and fro between the two states)
 *
 *
 * Animation animation = Animation::New( ... );\n
 * animation.AnimateTo( Property(buttonEffect, buttonEffect.GetStatePropertyName()), 1.0f, AlphaFunctions::Bounce, ... );\n
 * animation.Play();\n
 *
 */
class DALI_IMPORT_API DisplacementEffect : public ShaderEffect
{

public:

  typedef enum
  {
    DISPLACED = 0,    /// Image gets displaced
    FIXED             /// Image does not displace. Useful for matching lighting between areas that do not displace and those that do, e.g for backgrounds which are visible between buttons.
  }Type;

  /**
   * Create an uninitialized DisplacementEffect; this can be initialized with DisplacementEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  DisplacementEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DisplacementEffect();

  /**
   * Create an initialized DisplacementEffect
   * @param type The type of the effect, can be either DISPLACED, or FIXED.
   * @return A handle to a newly allocated Dali resource.
   */
  static DisplacementEffect New(Type type);

  /**
   * Get the name for the light direction property (Vector3)
   * The light direction is used in the lighting calculation. The angle of incidence directly affects the amount of light reflected.
   * Default (0.0f, 0.7070168f, 0.7071068f), i.e angled at the surface from in front and above.
   * @return A std::string containing the property name
   */
  const std::string& GetLightDirectionPropertyName() const;

  /**
   * Get the name for the ambient lighting color property (Vector3)
   * The ambient light is used in the lighting calculation. Care must be taken to not saturate the image by setting this value too high,
   * or the indentation will not look correct. Default 0.15.
   * @return A std::string containing the property name
   */
  const std::string& GetAmbientLightColorPropertyName() const;

  /**
   * Get the name for the diffuse light color property (Vector3).
   * The diffuse light is used in the lighting calculation. Default is (1.0f, 1.0f, 1.0f).
   * @return A std::string containing the property name
   */
  const std::string& GetDiffuseLightColorPropertyName() const;

  /**
   * Get the name for the lighting multiplier property (float).
   * The ambient and diffuse lighting is multiplied by this factor. Since a diffuse light at an angle will cause the whole image to darken,
   * this property can be used to scale the image back up closer to the pixel values of the original diffuse texture. Care must be taken to not saturate the image,
   * or the indentation will not look correct. Default 1.0
   * @return A std::string containing the property name
   */
  const std::string& GetLightingMultiplierPropertyName() const;

  /**
   * Get the name for the state property (float).
   * The shader can have a maximum of two end states 0 or 1, Animate between these two values to do the transitions between states.
   * Default 0.0
   * @return A std::string containing the property name.
   */
  const std::string& GetStatePropertyName() const;

  /**
   * Get the name for the height scale property (float).
   * The height displacement is multiplied by this factor. Tweak this to get the required level of depth.
   * Default 0.1
   * @return A std::string containing the property name.
   */
  const std::string& GetHeightScalePropertyName() const;

  /**
   * Get the name for the fixed normal property (Vector3).
   * Only applicable for the FIXED type shader and not for DISPLACEMENT type.
   * The Fixed normal will be used for the light calculation. Tweak this to get the required level of light.
   * Default (0.0f, 0.0f, 1.0f)
   * @return A std::string containing the property name.
   */
  const std::string& GetFixedNormalPropertyName() const;

  /**
   * Set the light direction property
   * The light direction is used in the lighting calculation. The angle of incidence directly affects the amount of light reflected.
   * Default (0.0f, 0.7070168f, 0.7071068f), i.e angled at the surface from in front and above.
   * @param [in] lightDirection The new light direction.
   */
  void SetLightDirection(Vector3 lightDirection);

  /**
   * Set the ambient light color property
   * The ambient light is used in the lighting calculation. Care must be taken to not saturate the image by setting this value too high,
   * or the indentation will not look correct. Default (0.15f, 0.15f, 0.15f).
   * @param [in] ambientLight The new ambient light value.
   */
  void SetAmbientLightColorProperty(Vector3 ambientLight);

  /**
   * Set the diffuse light color property.
   * The diffuse light is used in the lighting calculation. Default is (1.0f, 1.0f, 1.0f), i.e. a white light so the natural image color is shown.
   * @param [in] diffuseLight The new diffuse light value.
   */
  void SetDiffuseLightColorProperty(Vector3 diffuseLight);

  /**
   * Get the name for the lighting multiplier property.
   * The ambient and diffuse lighting is multiplied by this factor. Since a diffuse light at an angle will cause the whole image to darken,
   * this property can be used to scale the image back up closer to the pixel values of the original diffuse texture. Care must be taken to not saturate the image,
   * or the indentation will not look correct. Default 1.0
   * @param [in] lightMultiplier The new light multiplier value.
   */
  void SetLightingMultiplierProperty(float lightMultiplier);

  /**
   * Get the name for the state property.
   * The shader can only be in or in between two states 0 or 1, Animate between these two values to do the transitions between states.
   * @param [in] state The new state value.
   */
  void SetStateProperty(float state);

  /**
   * Set the name for the height scale property.
   * The height displacement is multiplied by this factor. Tweak this to get the required level of depth. Default 0.1
   * @param [in] heightScale The new height scale.
   */
  void SetHeightScaleProperty(float heightScale);

  /**
   * Set the name for fixed normal property, Only applicable for the FIXED type shader and not for DISPLACEMENT type.
   * The Fixed normal will be used for the light calculation. Tweak this to get the required level of light.
   * @param [in] fixedNormal The new normal for the fixed type shader effect.
   */
  void SetFixedNormalProperty(Vector3 fixedNormal);

private:
  // Not intended for application developers
  DALI_INTERNAL DisplacementEffect(ShaderEffect handle);
};

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISPLACEMENT_H__
