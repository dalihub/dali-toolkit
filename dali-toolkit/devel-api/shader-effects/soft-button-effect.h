#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SOFT_BUTTON_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SOFT_BUTTON_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 * @brief Creates a new soft button shader effect
 *
 * Soft button shader effect works on a per object basis. Using animatable parameters user can create
 * effect of button pushing in / out. Can be applied to ImageActor only.
 *
 * Usage example:-
 *
 * // Create shader used for doing soft button\n
 * ShaderEffect softButtonEffect = CreateSoftButtonEffect();
 *
 * // set image actor shader to the soft button one\n
 * ImageActor imageActor = ImageActor::New( ... );\n
 * imageActor.SetShaderEffect( softButtonEffect );
 *
 * // animate a button push, using e.g. AlphaFunction::BOUNCE. With these values the button pushes in and pops out slightly at the end\n
 * Animation animation = Animation::New( ... );\n
 * animation.AnimateTo( Property(softButtonEffect, "uLightingIndentationAmount), 0.25f, AlphaFunction::BOUNCE, ... );\n
 * animation.AnimateTo( Property(softButtonEffect, "uLightingIndentationAmount"), -0.05f, AlphaFunction::BOUNCE, ... );\n
 * animation.AnimateTo( Property(softButtonEffect, "uTextureDistortAmount"), 0.25f, AlphaFunction::BOUNCE, ... );\n
 * animation.AnimateTo( Property(softButtonEffect, "uTextureDistortAmount"), -0.05f, AlphaFunction::BOUNCE, ... );\n
 * animation.Play();\n
 *
 * Animatable/Constrainable uniforms:
 *
 *  "uLightingIndentationAmount"  - This property changes the lighting, to make it look like the button is pushed in. User should animate
 *                                  this in conjunction with texture distortion. Allowable values range from [-1..1], higher values give
 *                                  more change in lighting. Default 0.0 (no lighting change).
 *  "uTextureDistortAmount"       - This property changes the distortion, to make it look like the button is pushed in. User should animate
 *                                  this in conjunction with lighting indentation. Allowable values range from [-1..1) - note 1.0 is NOT
 *                                  allowed - higher values give more distortion. Default 0.0 (no distortion).
 *  "uAmbientLight"               - The ambient light is used in the lighting calculation. Care must be taken to not saturate the image by
 *                                  setting this value too high, or the indentation will not look correct. Default 0.15
 *  "uDiffuseLight"               - The diffuse light is used in the lighting calculation. Default is (0.0, 0.7070168, 0.7070168),
 *                                  i.e. a light angled at the surface from in front and above. Note that you need to Normalize()
 *                                  the Vector3 that you set with this property
 *  "uLightMultiplier"            - The ambient and diffuse lighting is multiplied by this factor. Since a diffuse light at an angle will
 *                                  cause the whole image to darken, even outside the soft button indentation, this property can be used
 *                                  to scale the image back up closer to the pixel values of the original diffuse texture. Care must be
 *                                  taken to not saturate the image,or the indentation will not look correct. Default 1.2.
 *  "uInsideCircleSizeScale"      - The SoftButtonEffect consists of two shapes, one inside the other. The outside shape fits exactly to
 *                                  the actor, touching its edges but completely contained. The inside shape size is given by a multiplier
 *                                  of the outside shape size. For example a value of 0.5 means that the inside shape is half the size of
 *                                  the outside one. Allowable values are in the range (0.0 - 1.0), note that 0.0 and 1.0 themselves are
 *                                  not allowed. Default 0.75.
 *  "uOutsideCircleDepth"         - The SoftButtonEffect consists of two shapes, one inside the other. The depth of the indentation at the
 *                                  transition between the inside and outside shapes is controlled by this property. The values lies in the
 *                                  range [0.0 - 1.0]. A value of 0.0 means the outside shape has no depth (and is thus invisible), value of
 *                                  1.0 means the outside shape has maximum depth (and the inside shape is thus invisible). Default 0.05
 *  "uEffectRegion"               - The soft button effect is applied within the supplied rect region of the texture. Default values for this
 *                                  is (0.0, 0.0, 1.0, 1.0) which is the entire image with 0,0 being the top left and 1.0, 1.0 being the bottom
 *                                  right. If the image texture is split between multiple ImageActors then the developer should specify the pixel
 *                                  area of the texture the effect should be applied with. Example, If the Image is split among two ImageActors
 *                                  side by side, with the left one using left half of the texture and right one using the right half of the
 *                                  texture then the pixel area value for the left ImageActor will be (0.0, 0.0, 0.5, 1.0) and the pixel area for
 *                                  the right will be (0.5,0.0,1.0,1.0).
 * "uRectangleSizeScale"          - This property can be used to set the mix between proportion of rectangle and proportion of ellipse - the
 *                                  result is a rectangle with rounded corners. If the value is 0.0, the shape is an ellipse. If the value is
 *                                  close to 1.0, the shape is close to a rectangle. The value lies in the range [0.0 - 1.0). Note that a value
 *                                  of 1.0 is NOT allowed.Default 0.5.
 *
 * @param type The type of the soft button, can be either ELLIPTICAL, RECTANGULAR, or FIXED.
 * @return A handle to a newly allocated ShaderEffect
 */
typedef enum
{
  SOFT_BUTTON_ELLIPTICAL = 0,   /// Button is elliptical
  SOFT_BUTTON_RECTANGULAR,      /// Button is rectangular
  SOFT_BUTTON_FIXED             /// Button does not indent (move). Useful for matching lighting between areas that do not indent (which can thus use a cheaper shader) and those that do indent.
}SoftButtonEffectType;

static void InverseConstraint( float& current, const PropertyInputContainer& inputs )
{
  current = 1.0f / inputs[0]->GetFloat();
}

inline ShaderEffect CreateSoftButtonEffect(SoftButtonEffectType type)
{
  std::string vertexSource;
  vertexSource =  "precision mediump float;\n"
      "uniform vec3 uDiffuseLight;\n"
      "uniform float uAmbientLight;\n"
      "uniform float uLightMultiplier;\n"
      "uniform vec4 uEffectRegion;\n"
      "varying vec2 vCentredCoord;\n"

      "const vec3 norm = vec3(0.0, 0.0, 1.0);\n"

      "void main()\n"
      "{\n"
      "  vTexCoord = aTexCoord;\n"
      // Get the rect coords of the effect region in -1..1 range, i.e. circle centred around the center of the rect
      // Done in the vertex shader itself to make use of gl interpolation for varying.
      "  vCentredCoord = vec2( ( (vTexCoord.x - uEffectRegion.x)/(uEffectRegion.z - uEffectRegion.x) * 2.0 - 1.0 ), ( (vTexCoord.y - uEffectRegion.y)/(uEffectRegion.w - uEffectRegion.y) * 2.0 - 1.0  ) );\n"
      "  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);\n"
      "}\n";

  std::string fragmentSourceFixed;
  fragmentSourceFixed =  "precision mediump float;\n"

      "uniform vec3 uDiffuseLight;\n"
      "uniform float uAmbientLight;\n"
      "uniform float uLightMultiplier;\n"
      "varying vec2 vCentredCoord;\n"

      "const vec3 norm = vec3(0.0, 0.0, 1.0);\n"

      "void main()\n"
      "{\n"
      "   vec4 col = texture2D(sTexture, vTexCoord);\n"
      // calc lighting
      "   float lighting = (dot(uDiffuseLight, norm) + uAmbientLight) * uLightMultiplier;\n"
      // output col = image * light
      // use the lighting value for colors only
      "   gl_FragColor = vec4(col.rgb * uColor.rgb * lighting, col.a * uColor.a);\n"
      "}\n";

  std::string fragmentSourceElliptical;
  fragmentSourceElliptical =  "precision mediump float;\n"

      "uniform float uLightingIndentationAmount;\n"
      "uniform float uTextureDistortAmount;\n"
      "uniform vec3 uDiffuseLight;\n"
      "uniform float uAmbientLight;\n"
      "uniform float uLightMultiplier;\n"
      "uniform float uInsideCircleSizeScale;\n"
      "uniform float uRecipInsideCircleSizeScale;\n"
      "uniform float uOutsideCircleDepth;\n"
      "uniform vec4 uEffectRegion;\n"
      "varying vec2 vCentredCoord;\n"

      "const float PI = 3.1415927;\n"

      "void main()\n"
      "{\n"
      // Apply distortion only if the pixel is within the rect specified
      "if( (vTexCoord.x > uEffectRegion.x) && (vTexCoord.x < uEffectRegion.z) && (vTexCoord.y > uEffectRegion.y) && (vTexCoord.y < uEffectRegion.w) )\n"
      "{\n"
      "   vec2 coord = vCentredCoord;\n"

      // find a coordinate representing distance from circle centre, such that we split into inside / outside circles that can have different gradients / normals
      "   float realDistFromCentre = length(coord);\n"
      "   realDistFromCentre = min(1.0, realDistFromCentre);\n" // clamp corners of square to vertical normal
      "   float distFromCentre;\n"
      "   if(realDistFromCentre <= uInsideCircleSizeScale)\n"
      "   {\n"
      "     distFromCentre = realDistFromCentre * uRecipInsideCircleSizeScale * (1.0 - uOutsideCircleDepth);\n" // inside circle indent, up to outline depth
      "   }\n"
      "   else                                                                                                                      \n"
      "   {\n"
      "     distFromCentre = mix(1.0 - uOutsideCircleDepth, 1.0, (realDistFromCentre - ( uInsideCircleSizeScale)) / (1.0 - uInsideCircleSizeScale));\n" // outside circle
      "   }\n"

      // get coords in -PI..PI range, i.e. scale the circle for use by trig functions
      "  coord *= PI;\n"

      // get a z value for the distorted surface in 0..1 range, using cos for a smooth curve (note, we ignore inside / outside circles since the difference isn't noticeable visually)
      "   vec2 cosThetaCoord = (cos(coord) * 0.5) + 0.5;\n"
      "   float z = cosThetaCoord.x * cosThetaCoord.y;\n"

      // get the normal for the distorted surface, using the fact that the derivative of cos is -sin, finding tangent vector from slope and then normal by cross product...
      "   float sinThetaCoord = sin(distFromCentre*PI) * uLightingIndentationAmount;\n" // slope, so tangent vec is (1.0, -sin)
      // ...2D normal vector along distFromCentre vec is (sin, 1.0), convert to components in 3D.
      "   vec3 norm = normalize(vec3(coord.x * sinThetaCoord, coord.y * sinThetaCoord, 1.0));\n"

      // form surface z and project texture onto it.
      "   float indentAmount = 1.0 / (1.0 - (z * uTextureDistortAmount));\n"
      "   vec2 distortedCoord = vCentredCoord * indentAmount;\n"

      // Convert the rect coordinates in -1 to 1 range back to the original coordinates
      "   vec2 texCoord = vec2( ( (distortedCoord.x + 1.0)*(0.5) * (uEffectRegion.z - uEffectRegion.x) + uEffectRegion.x ), ( (distortedCoord.y + 1.0)*(0.5) * (uEffectRegion.w - uEffectRegion.y) + uEffectRegion.y  ) );  \n"
      "   vec4 col = texture2D(sTexture, texCoord);\n"

      // calc lighting
      "   float lighting = (dot(uDiffuseLight, norm) + uAmbientLight) * uLightMultiplier;\n"
      "  gl_FragColor = vec4(col.rgb * uColor.rgb * lighting, col.a * uColor.a);\n"
      "}\n"
      "else\n"
      "{\n"
      "  vec4 col = texture2D(sTexture, vTexCoord);\n"
      "  float lighting = (dot(uDiffuseLight, vec3(0.0, 0.0, 1.0)) + uAmbientLight) * uLightMultiplier;\n"
      "  gl_FragColor = vec4(col.rgb * uColor.rgb * lighting, col.a * uColor.a);\n"
      "}\n"
      "}\n";

  std::string fragmentSourceRectangular;
  fragmentSourceRectangular =  "precision mediump float;\n"

      "uniform float uLightingIndentationAmount;\n"
      "uniform float uTextureDistortAmount;\n"
      "uniform vec3 uDiffuseLight;\n"
      "uniform float uAmbientLight;\n"
      "uniform float uLightMultiplier;\n"
      "uniform float uInsideCircleSizeScale;\n"
      "uniform float uRecipInsideCircleSizeScale;\n"
      "uniform float uOutsideCircleDepth;\n"
      "uniform float uRectangleSizeScale;\n"
      "uniform vec4 uEffectRegion;\n"
      "varying vec2 vCentredCoord;\n"

      "const float PI = 3.1415927;\n"

      "void main()\n"
      "{\n"
      // Apply distortion only if the pixel is within the rect specified
      "if( (vTexCoord.x > uEffectRegion.x) && (vTexCoord.x < uEffectRegion.z) && (vTexCoord.y > uEffectRegion.y) && (vTexCoord.y < uEffectRegion.w) )\n"
      "{ \n"
      // get the rect coords to -1..1 range, i.e. circle centred around the center of the rect
      "   vec2 centredCoord = vCentredCoord;\n"
      // clamp coords such that the circle is split into 4 pieces that lie in the corners of the actor. uRectangleScale is the distance along each axis from the centre
      // of the actor, e.g. 0.5 is half way along an axis from centre to actor edge.
      "   vec2 clampedCoord;\n"
      "   if(centredCoord.x > 0.0)\n"
      "   {\n"
      "     if(centredCoord.x < uRectangleSizeScale)\n"
      "     {\n"
      // we are in a rectangular region along this axis, clamp coord to be same as centre pixel
      "       clampedCoord.x = 0.0;\n"
      "     }\n"
      "     else\n"
      "     {\n"
      // we are outside rectangular region along this axis, so we want curvature.
      "       clampedCoord.x = smoothstep(0.0, 1.0, (centredCoord.x - uRectangleSizeScale) / (1.0 - uRectangleSizeScale));\n"
      "     }\n"
      "   }\n"
      "   else\n"
      "   {\n"
      "     if(centredCoord.x > -uRectangleSizeScale)\n"
      "     {\n"
      // we are in a rectangular region along this axis, clamp coord to be same as centre pixel
      "       clampedCoord.x = 0.0;\n"
      "     }\n"
      "     else\n"
      "     {\n"
      // we are outside rectangular region along this axis, so we want curvature.
      "       clampedCoord.x = -smoothstep(0.0, 1.0, (centredCoord.x + uRectangleSizeScale) / (uRectangleSizeScale - 1.0));\n"
      "     }\n"
      "   }\n"
      "   if(centredCoord.y > 0.0)\n"
      "   {\n"
      "     if(centredCoord.y < uRectangleSizeScale)\n"
      "     {\n"
      // we are in a rectangular region along this axis, clamp coord to be same as centre pixel
      "       clampedCoord.y = 0.0;\n"
      "     }\n"
      "     else\n"
      "     {\n"
      // we are outside rectangular region along this axis, so we want curvature.
      "       clampedCoord.y = smoothstep(0.0, 1.0, (centredCoord.y - uRectangleSizeScale) / (1.0 - uRectangleSizeScale));\n"
      "     }\n"
      "   }\n"
      "   else\n"
      "   {\n"
      "     if(centredCoord.y > -uRectangleSizeScale)\n"
      "     {\n"
      // we are in a rectangular region along this axis, clamp coord to be same as centre pixel
      "       clampedCoord.y = 0.0;\n"
      "     }\n"
      "     else\n"
      "     {\n"
      // we are outside rectangular region along this axis, so we want curvature.
      "       clampedCoord.y = -smoothstep(0.0, 1.0, (centredCoord.y + uRectangleSizeScale) / (uRectangleSizeScale - 1.0));\n"
      "     }\n"
      "   }\n"
      // get coords in -PI..PI range, i.e. scale above circle for use by trig functions
      "   vec2 thetaCoord = clampedCoord * PI;\n"
      // get a z value for the distorted surface in 0..1 range, using cos for a smooth curve (note, we ignore inside / outside circles since the difference isn't noticeable visually)
      "   vec2 cosThetaCoord = (cos(thetaCoord) * 0.5) + 0.5;\n"
      "   float z = cosThetaCoord.x * cosThetaCoord.y;\n"
      // find a coordinate representing distance from circle centre, such that we split into inside / outside circles that can have different gradients / normals
      "   float realDistFromCentre = length(thetaCoord);\n"
      "   realDistFromCentre = min(PI, realDistFromCentre);\n" // clamp corners of square to vertical normal
      "   float distFromCentre;\n"
      "   if(realDistFromCentre <= PI * uInsideCircleSizeScale)\n"
      "   {\n"
      "     distFromCentre = realDistFromCentre * uRecipInsideCircleSizeScale * (PI - (uOutsideCircleDepth * PI)) / PI;\n" // inside circle indent, up to outline depth
      "   }\n"
      "   else\n"
      "   {\n"
      "     distFromCentre = mix(PI - (uOutsideCircleDepth * PI), PI, (realDistFromCentre - ( PI * uInsideCircleSizeScale)) / (PI - (PI * uInsideCircleSizeScale)));\n" // outside circle
      "   }\n"
      // get the normal for the distorted surface, using the fact that the derivative of cos is -sin, finding tangent vector from slope and then normal by cross product...
      "   float sinThetaCoord = sin(distFromCentre) * uLightingIndentationAmount;\n" // slope, so tangent vec is (1.0, -sin)
      // ...2D normal vector along distFromCentre vec is (sin, 1.0), convert to components in 3D.
      "   vec3 norm = normalize(vec3(thetaCoord.x * sinThetaCoord, thetaCoord.y * sinThetaCoord, 1.0));\n"
      // form surface z and project texture onto it.
      "   float indentAmount = 1.0 / (1.0 - (z * uTextureDistortAmount));\n"
      "   vec2 distortedCoord = centredCoord * indentAmount;\n"
      // Convert the rect coordinates in -1 to 1 range back to the original coordinates
      "   vec2 texCoord = vec2( ( (distortedCoord.x + 1.0)/(2.0) * (uEffectRegion.z - uEffectRegion.x) + uEffectRegion.x ), ( (distortedCoord.y + 1.0)/(2.0) * (uEffectRegion.w - uEffectRegion.y) + uEffectRegion.y  ) );\n"
      "   vec4 col = texture2D(sTexture, texCoord);\n"
      // calc lighting
      "   float lighting = (dot(uDiffuseLight, norm) + uAmbientLight) * uLightMultiplier;\n"
      // output col = image * light
      // use the lighting value for colors only
      "   gl_FragColor = vec4(col.rgb * uColor.rgb * lighting, col.a * uColor.a);\n"

      "}\n"
      "else\n"
      "{\n"
      "   vec4 col = texture2D(sTexture, vTexCoord);\n"
      "   float lighting = (dot(uDiffuseLight, vec3(0.0, 0.0, 1.0)) + uAmbientLight) * uLightMultiplier;\n"
      "   gl_FragColor = vec4(col.rgb * uColor.rgb * lighting, col.a * uColor.a);\n"
      "} \n"
      "}\n";


  //////////////////////////////////////
  // Create shader effectCreateSoftButtonEffect
  //
  //

  ShaderEffect shader;
  switch(type)
  {
    case SOFT_BUTTON_RECTANGULAR:
      shader = ShaderEffect::New( vertexSource, fragmentSourceRectangular, GeometryType( GEOMETRY_TYPE_IMAGE ), ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ));
      break;

    case SOFT_BUTTON_ELLIPTICAL:
      shader = ShaderEffect::New( vertexSource, fragmentSourceElliptical, GeometryType( GEOMETRY_TYPE_IMAGE ), ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ));
      break;

    case SOFT_BUTTON_FIXED:
    default:
      shader = ShaderEffect::New( vertexSource, fragmentSourceFixed, GeometryType( GEOMETRY_TYPE_IMAGE ), ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ));
      break;
  }

  //////////////////////////////////////
  // Register uniform properties
  //
  //
  static const float SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_DEFAULT = 0.0f;
  static const float SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_DEFAULT = 0.0f;
  static const float SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_DEFAULT = 0.15f;
  static const Vector3 SOFT_BUTTON_DIFFUSE_LIGHT_DEFAULT = Vector3(0.0f, 0.7070168f, 0.7071068f);
  static const float SOFT_BUTTON_LIGHTING_MULTIPLIER_DEFAULT = 1.2f;
  static const float SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_DEFAULT = 0.75f;
  static const float SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_DEFAULT = Math::PI * 0.05f;
  static const Vector4 SOFT_BUTTON_EFFECT_PIXEL_AREA_DEFAULT = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  static const float SOFT_BUTTON_RECTANGLE_SIZE_SCALE_DEFAULT = 0.5f;

  // factors that scale the look, defaults
  shader.SetUniform("uAmbientLight", SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_DEFAULT);
  shader.SetUniform("uDiffuseLight", SOFT_BUTTON_DIFFUSE_LIGHT_DEFAULT);
  shader.SetUniform("uLightMultiplier", SOFT_BUTTON_LIGHTING_MULTIPLIER_DEFAULT);
  if(SOFT_BUTTON_FIXED != type)
  {
    shader.SetUniform("uLightingIndentationAmount", SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_DEFAULT);
    shader.SetUniform("uTextureDistortAmount", SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_DEFAULT);
    shader.SetUniform("uInsideCircleSizeScale", SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_DEFAULT);
    shader.SetUniform("uRecipInsideCircleSizeScale", 1.0f / SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_DEFAULT);
    shader.SetUniform("uOutsideCircleDepth", SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_DEFAULT);
    shader.SetUniform("uEffectRegion", SOFT_BUTTON_EFFECT_PIXEL_AREA_DEFAULT);
    if(SOFT_BUTTON_RECTANGULAR == type)
    {
      shader.SetUniform("uRectangleSizeScale", SOFT_BUTTON_RECTANGLE_SIZE_SCALE_DEFAULT);
    }

    // precalc 1.0 / uInsideCircleSizeScale on CPU to save shader insns, using constraint to tie to the normal property
    Dali::Property::Index insideCircleSizeScalePropertyIndex = shader.GetPropertyIndex("uInsideCircleSizeScale");
    Dali::Property::Index recipInsideCircleSizeScalePropertyIndex = shader.GetPropertyIndex("uRecipInsideCircleSizeScale");
    Constraint constraint = Constraint::New<float>( shader, recipInsideCircleSizeScalePropertyIndex, InverseConstraint );
    constraint.AddSource( LocalSource(insideCircleSizeScalePropertyIndex) );
    constraint.Apply();
  }

  return shader;
}

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SOFT_BUTTON_H__
