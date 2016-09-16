#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISPLACEMENT_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_DISPLACEMENT_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Creates a new displacement effect
 *
 * Two state displacement effect shader that works on a per object basis. By passing a height-normal map as an effect image, the user can create
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
 * ShaderEffect buttonEffect = CreateDisplacementEffect();
 * buttonEffect.SetEffectImage(Image::New( FANCY_BUTTON_HEIGHT_MAP_IMAGE_PATH ););
 *
 * // set shader to the soft button\n
 * ImageActor fancyButton = ImageActor::New( ... );\n
 * fancyButton.SetShaderEffect( buttonEffect );
 *
 * // animate a button push, using e.g. AlphaFunction::BOUNCE. With these values the button pushes in and out (animates to and fro between the two states)
 *
 *
 * Animation animation = Animation::New( ... );\n
 * animation.AnimateTo( Property(buttonEffect, "uState"), 1.0f, AlphaFunction::BOUNCE, ... );\n
 * animation.Play();\n
 *
 * Animatable/Constrainable uniforms:
 *   "uLightDirection"      - The light direction is used in the lighting calculation. The angle of incidence directly affects the amount of light reflected.
 *                            Default (0.0f, 0.7070168f, 0.7071068f), i.e angled at the surface from in front and above.
 *   "uAmbientLightColor"   - The ambient light is used in the lighting calculation. Care must be taken to not saturate the image by setting this value too high,
 *                            or the indentation will not look correct. Default 0.15.
 *   "uDiffuseLightColor"   - The diffuse light is used in the lighting calculation. Default is (1.0f, 1.0f, 1.0f).
 *   "uLightMultiplier"     - The ambient and diffuse lighting is multiplied by this factor. Since a diffuse light at an angle will cause the whole image to darken,
 *                            this property can be used to scale the image back up closer to the pixel values of the original diffuse texture. Care must be taken
 *                            to not saturate the image,or the indentation will not look correct. Default 1.0
 *   "uState"               - The shader can have a maximum of two end states 0 or 1, Animate between these two values to do the transitions
 *                            between states. Default 0.0
 *   "uHightScale"          - The height displacement is multiplied by this factor. Tweak this to get the required level of depth. Default 0.1
 *   "uFixedNormal"         - The Fixed normal will be used for the light calculation. Tweak this to get the required level of light.
 *                            Only applicable for the FIXED type shader and not for DISPLACED type
 *
 * @param type The type of the effect, can be either DISPLACED, or FIXED.
 * @return A handle to a newly allocated ShaderEffect
 *
 */

typedef enum
{
  DISPLACEMENT_EFFECT_DISPLACED = 0,    /// Image gets displaced
  DISPLACEMENT_EFFECT_FIXED             /// Image does not displace. Useful for matching lighting between areas that do not displace and those that do, e.g for backgrounds which are visible between buttons.
}DisplacementEffectType;

inline ShaderEffect CreateDisplacementEffect(DisplacementEffectType type)
{
  std::string fragmentSourceFixed;
  fragmentSourceFixed =  "precision mediump float;\n"
      "uniform vec3 uLightDirection;\n"
      "uniform vec3 uAmbientLightColor;\n"
      "uniform vec3 uDiffuseLightColor;\n"
      "uniform float uLightMultiplier;\n"
      "uniform float uState;\n"
      "uniform float uHightScale;\n"
      "uniform vec3 uFixedNormal;\n"

      "void main()\n"
      "{\n"
      "  vec4 col = texture2D(sTexture, vTexCoord);\n"
      // calc lighting
      "  float intensity = dot(uLightDirection, uFixedNormal);"
      "  vec3 lighting = (intensity * uDiffuseLightColor) + uAmbientLightColor;\n"
      "  lighting *= uLightMultiplier;\n"
      // output col = image * light
      "  gl_FragColor = vec4(col.rgb * lighting * uColor.rgb, col.a * uColor.a);\n"
      "}\n";



  std::string fragmentSourceDisplaced(
      "precision mediump float;\n"
      "uniform vec3 uLightDirection;\n"
      "uniform vec3 uAmbientLightColor;\n"
      "uniform vec3 uDiffuseLightColor;\n"
      "uniform float uLightMultiplier;\n"
      "uniform float uState;\n"
      "uniform float uHightScale;\n"
      "void main()\n"
      "{\n"
      "  highp vec4 displacementMap1 = texture2D(sEffect, vec2(vTexCoord.s, vTexCoord.t/2.0));\n"
      "  highp vec4 displacementMap2 = texture2D(sEffect, vec2(vTexCoord.s, 0.5+vTexCoord.t/2.0));\n"
      "  highp vec4 displacementMap = mix(displacementMap1, displacementMap2, uState);\n"

      "  vec3 normalAdjusted = normalize(displacementMap.rgb*2.0-1.0);\n"
      "  float height = uHightScale * (displacementMap.a*2.0 - 1.0);\n"
      "  vec2 displacement = vec2(0.0);\n"
      "  displacement += (vec2(0.5)-vTexCoord.st)*height;\n"
      "  vec2 newCoord = vTexCoord.st + displacement.xy;\n"

      "  vec4 col = texture2D(sTexture, newCoord);\n"
      // Y-Axis for the normal map is taken as in Y-Down format, So inverting it for GL
      "  float intensity = dot(uLightDirection, vec3(1.0,-1.0, 1.0) * normalAdjusted);"
      "  vec3 lighting = (intensity * uDiffuseLightColor) + uAmbientLightColor;\n"
      "  lighting *= uLightMultiplier;\n"
      "  vec3 color = col.rgb * lighting * uColor.rgb;\n"
      "  gl_FragColor = vec4(color, col.a * uColor.a);\n"
      "}\n");

  //////////////////////////////////////
  // Create shader effect
  //
  //

  ShaderEffect shaderEffect;
  switch(type)
  {
    case DISPLACEMENT_EFFECT_DISPLACED:
      shaderEffect = ShaderEffect::New( "", fragmentSourceDisplaced);
      break;

    case DISPLACEMENT_EFFECT_FIXED:
    default:
      shaderEffect = ShaderEffect::New( "", fragmentSourceFixed);
      break;
  }


  //////////////////////////////////////
  // Register uniform properties
  //
  //
  // factors that scale the look, defaults
  shaderEffect.SetUniform("uLightDirection",Vector3(0.0, 0.7070168f, 0.7071068f));
  shaderEffect.SetUniform("uAmbientLightColor",Vector3(0.15f, 0.15f, 0.15f));
  shaderEffect.SetUniform("uDiffuseLightColor",Vector3(1.0f, 1.0f, 1.0f));
  shaderEffect.SetUniform("uLightMultiplier",1.0f);
  shaderEffect.SetUniform("uState",0.0f);
  shaderEffect.SetUniform("uHightScale",0.1f);

  if(type == DISPLACEMENT_EFFECT_FIXED)
  {
    shaderEffect.SetUniform("uFixedNormal",Vector3(0.0f, 0.0f, 1.0f) );
  }

  return shaderEffect;
}

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISPLACEMENT_H__
