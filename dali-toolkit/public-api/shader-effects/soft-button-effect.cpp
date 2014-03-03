//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/shader-effects/soft-button-effect.h>
namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_PROPERTY_NAME( "uLightingIndentationAmount" );
const std::string SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_PROPERTY_NAME( "uTextureDistortAmount" );
const std::string SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_PROPERTY_NAME( "uAmbientLight" );
const std::string SOFT_BUTTON_DIFFUSE_LIGHT_PROPERTY_NAME( "uDiffuseLight" );
const std::string SOFT_BUTTON_LIGHTING_MULTIPLIER_PROPERTY_NAME( "uLightMultiplier" );
const std::string SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME( "uInsideCircleSizeScale" );
const std::string SOFT_BUTTON_RECIP_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME( "uRecipInsideCircleSizeScale" );
const std::string SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_PROPERTY_NAME( "uOutsideCircleDepth" );
const std::string SOFT_BUTTON_EFFECT_PIXEL_AREA_PROPERTY_NAME( "uEffectRegion" );
const std::string SOFT_BUTTON_RECTANGLE_SIZE_SCALE_PROPERTY_NAME( "uRectangleSizeScale" );


// factors that scale the look, defaults
const float SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_DEFAULT = 0.0f;
const float SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_DEFAULT = 0.0f;
const float SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_DEFAULT = 0.15f;
const Vector3 SOFT_BUTTON_DIFFUSE_LIGHT_DEFAULT = Vector3(0.0f, 0.7070168f, 0.7071068f);
const float SOFT_BUTTON_LIGHTING_MULTIPLIER_DEFAULT = 1.2f;
const float SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_DEFAULT = 0.75f;
const float SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_DEFAULT = Math::PI * 0.05f;
const Vector4 SOFT_BUTTON_EFFECT_PIXEL_AREA_DEFAULT = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
const float SOFT_BUTTON_RECTANGLE_SIZE_SCALE_DEFAULT = 0.5f;

} // namespace

/**
 * ReciprocalConstraint
 *
 * f(current, property) = 1.0 / property
 */
struct ReciprocalConstraint
{
  ReciprocalConstraint(){}

  float operator()(const float current, const PropertyInput& property)
  {
    return 1.0f / property.GetFloat();
  }
};


////////////////////////////////////////////////////
//
// Soft button shader / actor tweaking parameters
//


SoftButtonEffect::SoftButtonEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
SoftButtonEffect::SoftButtonEffect(ShaderEffect handle)
:ShaderEffect(handle)
{
}

SoftButtonEffect::~SoftButtonEffect()
{
}

SoftButtonEffect SoftButtonEffect::New(Type type)
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
  // Create shader effect
  //
  //

  ShaderEffect shader;
  switch(type)
  {
    case RECTANGULAR:
      shader = ShaderEffect::New( vertexSource, fragmentSourceRectangular, GeometryType( GEOMETRY_TYPE_IMAGE ), ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ));
      break;

    case ELLIPTICAL:
      shader = ShaderEffect::New( vertexSource, fragmentSourceElliptical, GeometryType( GEOMETRY_TYPE_IMAGE ), ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ));
      break;

    case FIXED:
    default:
      shader = ShaderEffect::New( vertexSource, fragmentSourceFixed, GeometryType( GEOMETRY_TYPE_IMAGE ), ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ));
      break;
  }
  SoftButtonEffect handle( shader );


  //////////////////////////////////////
  // Register uniform properties
  //
  //

  // factors that scale the look, defaults
  handle.SetUniform(SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_PROPERTY_NAME, SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_DEFAULT);
  handle.SetUniform(SOFT_BUTTON_DIFFUSE_LIGHT_PROPERTY_NAME, SOFT_BUTTON_DIFFUSE_LIGHT_DEFAULT);
  handle.SetUniform(SOFT_BUTTON_LIGHTING_MULTIPLIER_PROPERTY_NAME, SOFT_BUTTON_LIGHTING_MULTIPLIER_DEFAULT);
  if(FIXED != type)
  {
    handle.SetUniform(SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_PROPERTY_NAME, SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_DEFAULT);
    handle.SetUniform(SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_PROPERTY_NAME, SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_DEFAULT);
    handle.SetUniform(SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME, SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_DEFAULT);
    handle.SetUniform(SOFT_BUTTON_RECIP_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME, 1.0f / SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_DEFAULT);
    handle.SetUniform(SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_PROPERTY_NAME, SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_DEFAULT);
    handle.SetUniform(SOFT_BUTTON_EFFECT_PIXEL_AREA_PROPERTY_NAME, SOFT_BUTTON_EFFECT_PIXEL_AREA_DEFAULT);
    if(RECTANGULAR == type)
    {
      handle.SetUniform(SOFT_BUTTON_RECTANGLE_SIZE_SCALE_PROPERTY_NAME, SOFT_BUTTON_RECTANGLE_SIZE_SCALE_DEFAULT);
    }

    // precalc 1.0 / uInsideCircleSizeScale on CPU to save shader insns, using constraint to tie to the normal property
    Property::Index insideCircleSizeScalePropertyIndex = handle.GetPropertyIndex(SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME);
    Property::Index recipInsideCircleSizeScalePropertyIndex = handle.GetPropertyIndex(SOFT_BUTTON_RECIP_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME);
    Constraint constraint = Constraint::New<float>( recipInsideCircleSizeScalePropertyIndex, LocalSource(insideCircleSizeScalePropertyIndex), ReciprocalConstraint());
    handle.ApplyConstraint(constraint);
  }

  return handle;
}

const std::string& SoftButtonEffect::GetLightingIndentationAmountPropertyName() const
{
  return SOFT_BUTTON_LIGHTING_INDENTATION_AMOUNT_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetTextureDistortionAmountPropertyName() const
{
  return SOFT_BUTTON_TEXTURE_DISTORTION_AMOUNT_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetAmbientLightAmountPropertyName() const
{
  return SOFT_BUTTON_AMBIENT_LIGHT_AMOUNT_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetDiffuseLightPropertyName() const
{
  return SOFT_BUTTON_DIFFUSE_LIGHT_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetLightingMultiplierPropertyName() const
{
  return SOFT_BUTTON_LIGHTING_MULTIPLIER_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetInsideShapeSizeScalePropertyName() const
{
  return SOFT_BUTTON_INSIDE_SHAPE_SIZE_SCALE_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetOutsideShapeDepthPropertyName() const
{
  return SOFT_BUTTON_OUTSIDE_SHAPE_DEPTH_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetEffectPixelAreaPropertyName() const
{
  return SOFT_BUTTON_EFFECT_PIXEL_AREA_PROPERTY_NAME;
}

const std::string& SoftButtonEffect::GetRectangleSizeScalePropertyName() const
{
  return SOFT_BUTTON_RECTANGLE_SIZE_SCALE_PROPERTY_NAME;
}

}

}

