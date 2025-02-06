precision highp float;
INPUT highp vec2 vTexCoord;
#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
INPUT highp vec2 vPosition;
FLAT INPUT highp vec2 vRectSize;
FLAT INPUT highp vec2 vOptRectSize;
FLAT INPUT highp float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
FLAT INPUT highp vec4 vCornerRadius;
#endif
#endif
#ifdef IS_REQUIRED_DEBUG_VISUAL_SHADER
#define DEBUG_EXTRA_VARYINGS
DEBUG_EXTRA_VARYINGS
#endif

uniform sampler2D sTexture;
#if defined(IS_REQUIRED_YUV_TO_RGB) || defined(IS_REQUIRED_UNIFIED_YUV_AND_RGB)
uniform sampler2D sTextureU;
uniform sampler2D sTextureV;
#endif

#ifdef IS_REQUIRED_ALPHA_MASKING
uniform sampler2D sMaskTexture;
uniform lowp float uYFlipMaskTexture;
INPUT highp vec2 vMaskTexCoord;
#endif

#ifdef ATLAS_DEFAULT_WARP
uniform highp vec4 uAtlasRect;
#elif defined(ATLAS_CUSTOM_WARP)
// WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
uniform lowp vec2 wrapMode;
#endif


#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER)
uniform highp vec3 uScale;
#endif

uniform lowp vec4 uColor;
uniform lowp float premultipliedAlpha;
#ifdef IS_REQUIRED_BORDERLINE
uniform highp float borderlineWidth;
uniform highp float borderlineOffset;
uniform lowp vec4 borderlineColor;
uniform lowp vec4 uActorColor;
#endif

#ifdef IS_REQUIRED_SQUIRCLE_CORNER
uniform highp vec4 cornerSquareness;
#endif

#ifdef ATLAS_CUSTOM_WARP
highp float wrapCoordinate( highp vec2 range, highp float coordinate, lowp float wrap )
{
  highp float coord;
  if( wrap > 1.5 ) /* REFLECT */
    coord = 1.0 - abs(fract(coordinate*0.5)*2.0 - 1.0);
  else /* warp is 0 or 1 */
    coord = mix(coordinate, fract(coordinate), wrap);
  return clamp(mix(range.x, range.y, coord), range.x, range.y);
}
#endif

#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
// Global values both rounded corner and borderline use

// radius of rounded corner on this quadrant
highp float gRadius = 0.0;
#ifdef IS_REQUIRED_SQUIRCLE_CORNER
highp float gSquareness = 0.0;
#endif

// fragment coordinate. NOTE : vec2(0.0, 0.0) is vRectSize, the corner of visual
highp vec2 gFragmentPosition = vec2(0.0, 0.0);
// center coordinate of rounded corner circle. vec2(gCenterPosition, gCenterPosition).
highp float gCenterPosition = 0.0;
// relative coordinate of gFragmentPosition from gCenterPosition.
highp vec2 gDiff = vec2(0.0, 0.0);
// potential value what our algorithm use.
highp float gPotential = 0.0;

// threshold of potential
highp float gPotentialRange = 0.0;
highp float gMaxOutlinePotential = 0.0;
highp float gMinOutlinePotential = 0.0;
#ifdef IS_REQUIRED_BORDERLINE
highp float gMaxInlinePotential = 0.0;
highp float gMinInlinePotential = 0.0;
#endif

void calculateCornerRadius()
{
#ifdef IS_REQUIRED_ROUNDED_CORNER
  gRadius =
  mix(
    mix(vCornerRadius.x, vCornerRadius.y, sign(vPosition.x) * 0.5 + 0.5),
    mix(vCornerRadius.w, vCornerRadius.z, sign(vPosition.x) * 0.5 + 0.5),
    sign(vPosition.y) * 0.5 + 0.5
  );
#ifdef IS_REQUIRED_SQUIRCLE_CORNER
  gSquareness = clamp(
  mix(
    mix(cornerSquareness.x, cornerSquareness.y, sign(vPosition.x) * 0.5 + 0.5),
    mix(cornerSquareness.w, cornerSquareness.z, sign(vPosition.x) * 0.5 + 0.5),
    sign(vPosition.y) * 0.5 + 0.5
  ), 0.0, 1.0);
#endif
#endif
}

void calculateFragmentPosition()
{
  gFragmentPosition = abs(vPosition) - vRectSize;
}

void calculatePosition()
{
  gCenterPosition = -gRadius;
#ifdef IS_REQUIRED_BORDERLINE
  gCenterPosition += borderlineWidth * (clamp(borderlineOffset, -1.0, 1.0) + 1.0) * 0.5;
#endif
  gDiff = gFragmentPosition - gCenterPosition;
}

void calculatePotential()
{
#ifdef IS_REQUIRED_SQUIRCLE_CORNER
  // If gSquareness is near 1.0, it make some numeric error. Let we avoid this situation by heuristic value.
  if(gSquareness > 0.99)
  {
    gPotential = max(gDiff.x, gDiff.y);
    return;
  }

  // We need to found the r value s.t. x^2 + y^2 - s/r/r x^2y^2 = r^2
  // and check this r is inside [gRadius - vAliasMargin, gRadius + vAliasMargin]

  // If we make as A = x^2 + y^2, B = sx^2y^2
  // r^2 = (A + sqrt(A^2 - 4B)) / 2
  //     = ((x^2 + y^2) + sqrt(x^4 + (2 - 4s)x^2y^2 + y^4)) / 2

  highp vec2 positiveDiff = max(gDiff, 0.0);

  // make sqr to avoid duplicate codes.
  positiveDiff *= positiveDiff;

  // TODO : Could we remove this double-sqrt code?
  gPotential = sqrt(((positiveDiff.x + positiveDiff.y)
                     + sqrt(positiveDiff.x * positiveDiff.x
                            + positiveDiff.y * positiveDiff.y
                            + (2.0 - 4.0 * gSquareness * gSquareness) * positiveDiff.x * positiveDiff.y))
                    * 0.5)
               + min(0.0, max(gDiff.x, gDiff.y)); ///< Consider negative potential, to support borderline
#else
  gPotential = length(max(gDiff, 0.0)) + min(0.0, max(gDiff.x, gDiff.y));
#endif
}

void setupMinMaxPotential()
{
  gPotentialRange = vAliasMargin;

  gMaxOutlinePotential = gRadius + gPotentialRange;
  gMinOutlinePotential = gRadius - gPotentialRange;

#ifdef IS_REQUIRED_BORDERLINE
  gMaxInlinePotential = gMaxOutlinePotential - borderlineWidth;
  gMinInlinePotential = gMinOutlinePotential - borderlineWidth;
#endif

  // reduce defect near edge of rounded corner.
  highp float heuristicEdgeCasePotential = clamp(-min(gDiff.x, gDiff.y) / max(1.0, gRadius), 0.0, gPotentialRange);
  gMaxOutlinePotential += heuristicEdgeCasePotential;
  gMinOutlinePotential += heuristicEdgeCasePotential;
}

//void PreprocessPotential()
//{
//  calculateCornerRadius();
//  calculateFragmentPosition();
//  calculatePosition();
//  calculatePotential();
//  setupMinMaxPotential();
//}
#endif

#ifdef IS_REQUIRED_BORDERLINE
lowp vec4 convertBorderlineColor(lowp vec4 textureColor)
{
  highp float potential = gPotential;

  // default opacity of borderline is 0.0
  mediump float borderlineOpacity = 0.0;

  // calculate borderline opacity by potential
  if(potential > gMinInlinePotential)
  {
    // potential is inside borderline range.
    borderlineOpacity = smoothstep(gMinInlinePotential, gMaxInlinePotential, potential);

    // Muliply borderlineWidth to resolve very thin borderline
    borderlineOpacity *= min(1.0, borderlineWidth / gPotentialRange);
  }

  highp vec3  borderlineColorRGB   = borderlineColor.rgb * uActorColor.rgb;
  highp float borderlineColorAlpha = borderlineColor.a * uActorColor.a;
  borderlineColorRGB *= mix(1.0, borderlineColorAlpha, premultipliedAlpha);

  // Calculate inside of borderline when alpha is between (0.0  1.0). So we need to apply texture color.
  // If borderlineOpacity is exactly 0.0, we always use whole texture color. In this case, we don't need to run below code.
  // But if borderlineOpacity > 0.0 and borderlineColor.a == 0.0, we need to apply tCornerRadius.
  if(borderlineOpacity > 0.0 && borderlineColor.a * borderlineOpacity < 1.0)
  {
    highp float tCornerRadius = -gCenterPosition + gPotentialRange;
    highp float MaxTexturelinePotential = tCornerRadius + gPotentialRange;
    highp float MinTexturelinePotential = tCornerRadius - gPotentialRange;
    if(potential > MaxTexturelinePotential)
    {
      // potential is out of texture range.
      textureColor = vec4(0.0);
    }
    else
    {
      // potential is in texture range.
      highp float textureAlphaScale = mix(1.0, 0.0, smoothstep(MinTexturelinePotential, MaxTexturelinePotential, potential));
      textureColor.a *= textureAlphaScale;
      textureColor.rgb *= mix(textureColor.a, textureAlphaScale, premultipliedAlpha);
    }

    borderlineColorAlpha *= borderlineOpacity;
    borderlineColorRGB *= mix(borderlineColorAlpha, borderlineOpacity, premultipliedAlpha);
    // We use pre-multiplied color to reduce operations.
    // In here, textureColor and borderlineColorRGB is pre-multiplied color now.

    // Manual blend operation with premultiplied colors.
    // Final alpha = borderlineColorAlpha + (1.0 - borderlineColorAlpha) * textureColor.a.
    // (Final rgb * alpha) =  borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb
    // If premultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
    // Else, return vec4((rgb*alpha) / alpha, alpha)

    highp float finalAlpha = mix(textureColor.a, 1.0, borderlineColorAlpha);
    highp vec3  finalMultipliedRGB = borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb;
    // TODO : Need to find some way without division
    return vec4(finalMultipliedRGB * mix(1.0 / finalAlpha, 1.0, premultipliedAlpha), finalAlpha);
  }
  return mix(textureColor, vec4(borderlineColorRGB, borderlineColorAlpha), borderlineOpacity);
}
#endif

#ifdef IS_REQUIRED_ROUNDED_CORNER
mediump float calculateCornerOpacity()
{
  highp float potential = gPotential;

  // default opacity is 1.0
  mediump float opacity = 1.0;

  // calculate borderline opacity by potential
  if(potential > gMaxOutlinePotential)
  {
    // potential is out of borderline range. just discard here
    discard;
  }
  else if(potential > gMinOutlinePotential)
  {
    opacity = 1.0 - smoothstep(gMinOutlinePotential, gMaxOutlinePotential, potential);
  }
  return opacity;
}
#endif

#if defined(IS_REQUIRED_YUV_TO_RGB) || defined(IS_REQUIRED_UNIFIED_YUV_AND_RGB)
lowp vec4 ConvertYuvToRgba(highp vec2 texCoord)
{
#ifdef IS_REQUIRED_UNIFIED_YUV_AND_RGB
  // Special case when shader use YUV but actual textures are not YUV format.
  // In this case, just resturn sTexture.
  if(textureSize(sTextureU, 0) != textureSize(sTextureV, 0))
  {
    return texture(sTexture, texCoord);
  }
#endif

  highp float y = TEXTURE(sTexture, texCoord).r;
  highp float u = TEXTURE(sTextureU, texCoord).r - 0.5;
  highp float v = TEXTURE(sTextureV, texCoord).r - 0.5;
  highp vec4 rgba;
  rgba.r = y + (1.403 * v);
  rgba.g = y - (0.344 * u) - (0.714 * v);
  rgba.b = y + (1.770 * u);
  rgba.a = 1.0;
  return rgba;
}
#endif

#ifdef IS_REQUIRED_DEBUG_VISUAL_SHADER

// Predefined values whether some macro defined or not.
// Since we make debug codes replace by macro,
// sharp if keyword cannot be used.
// Instead, let we use bool values so we can use define checked in script
#ifdef IS_REQUIRED_ROUNDED_CORNER
const bool IS_REQUIRED_ROUNDED_CORNER_BOOL = true;
#else
const bool IS_REQUIRED_ROUNDED_CORNER_BOOL = false;
#endif
#ifdef IS_REQUIRED_SQUIRCLE_CORNER
const bool IS_REQUIRED_SQUIRCLE_CORNER_BOOL = true;
#else
const bool IS_REQUIRED_SQUIRCLE_CORNER_BOOL = false;
#endif
#ifdef IS_REQUIRED_BORDERLINE
const bool IS_REQUIRED_BORDERLINE_BOOL = true;
#else
const bool IS_REQUIRED_BORDERLINE_BOOL = false;
#endif
#ifdef IS_REQUIRED_YUV_TO_RGB
const bool IS_REQUIRED_YUV_TO_RGB_BOOL = true;
#else
const bool IS_REQUIRED_YUV_TO_RGB_BOOL = false;
#endif
#ifdef IS_REQUIRED_UNIFIED_YUV_AND_RGB
const bool IS_REQUIRED_UNIFIED_YUV_AND_RGB_BOOL = true;
#else
const bool IS_REQUIRED_UNIFIED_YUV_AND_RGB_BOOL = false;
#endif
#ifdef IS_REQUIRED_ALPHA_MASKING
const bool IS_REQUIRED_ALPHA_MASKING_BOOL = true;
#else
const bool IS_REQUIRED_ALPHA_MASKING_BOOL = false;
#endif
#ifdef ATLAS_DEFAULT_WARP
const bool ATLAS_DEFAULT_WARP_BOOL = true;
#else
const bool ATLAS_DEFAULT_WARP_BOOL = false;
#endif
#ifdef ATLAS_CUSTOM_WARP
const bool ATLAS_CUSTOM_WARP_BOOL = true;
#else
const bool ATLAS_CUSTOM_WARP_BOOL = false;
#endif

// These lines in the shader may be replaced with actual definitions by the debug-image-visual-shader-script.json.
// DEBUG_TRIGGER_CODE return bool type, and DEBUG_RATIO_CODE return float value which will be clamped between 0.0 and 1.0
// If DEBUG_TRIGGER_CODE return true, it mean we will change final color's channel value.
// If ratio is 0.0, debug color rate become MINIMUM_DEBUG_COLOR_RATE, and 1.0 than MAXIMUM_DEBUG_COLOR_RATE.
#define MINIMUM_DEBUG_COLOR_RATE
#define MAXIMUM_DEBUG_COLOR_RATE
#define DEBUG_TRIGGER_RED_CODE
#define DEBUG_TRIGGER_GREEN_CODE
#define DEBUG_TRIGGER_BLUE_CODE
#define DEBUG_RATIO_RED_CODE
#define DEBUG_RATIO_GREEN_CODE
#define DEBUG_RATIO_BLUE_CODE
#define DEBUG_EXTRA_UNIFORMS

DEBUG_EXTRA_UNIFORMS

const mediump float gMinDebugColorRate = MINIMUM_DEBUG_COLOR_RATE;
const mediump float gMaxDebugColorRate = MAXIMUM_DEBUG_COLOR_RATE;

bool DebugTriggerRed(mediump vec4 originColor)
{
  DEBUG_TRIGGER_RED_CODE
}

bool DebugTriggerGreen(mediump vec4 originColor)
{
  DEBUG_TRIGGER_GREEN_CODE
}

bool DebugTriggerBlue(mediump vec4 originColor)
{
  DEBUG_TRIGGER_BLUE_CODE
}

mediump float DebugRatioRed(mediump vec4 originColor)
{
  DEBUG_RATIO_RED_CODE
}

mediump float DebugRatioGreen(mediump vec4 originColor)
{
  DEBUG_RATIO_GREEN_CODE
}

mediump float DebugRatioBlue(mediump vec4 originColor)
{
  DEBUG_RATIO_BLUE_CODE
}

mediump vec3 ApplyDebugMixColor(mediump vec4 originColor)
{
  mediump float debugColorRateRed = 0.0;
  mediump float debugColorRateGreen = 0.0;
  mediump float debugColorRateBlue = 0.0;

  if(DebugTriggerRed(originColor))
  {
    debugColorRateRed = mix(gMinDebugColorRate, gMaxDebugColorRate, smoothstep(0.0, 1.0, DebugRatioRed(originColor)));
  }
  if(DebugTriggerGreen(originColor))
  {
    debugColorRateGreen = mix(gMinDebugColorRate, gMaxDebugColorRate, smoothstep(0.0, 1.0, DebugRatioGreen(originColor)));
  }
  if(DebugTriggerBlue(originColor))
  {
    debugColorRateBlue = mix(gMinDebugColorRate, gMaxDebugColorRate, smoothstep(0.0, 1.0, DebugRatioBlue(originColor)));
  }

  mediump float colorRate = max(debugColorRateRed, max(debugColorRateGreen, debugColorRateBlue));
  mediump vec3 debugColor = vec3(debugColorRateRed, debugColorRateGreen, debugColorRateBlue);

  debugColor *= mix(1.0, originColor.a, premultipliedAlpha);

  return originColor.rgb * (1.0 - colorRate) + debugColor;
}
#endif

void main()
{
#ifdef ATLAS_DEFAULT_WARP
  highp vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
#elif defined(ATLAS_CUSTOM_WARP)
  highp vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                              wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );
#else
  highp vec2 texCoord = vTexCoord;
#endif

#if defined(IS_REQUIRED_YUV_TO_RGB) || defined(IS_REQUIRED_UNIFIED_YUV_AND_RGB)
  lowp vec4 textureColor = ConvertYuvToRgba(texCoord) * uColor;
#else
  lowp vec4 textureColor = TEXTURE( sTexture, texCoord ) * uColor;
#endif

#ifdef IS_REQUIRED_ALPHA_MASKING
  highp vec2 maskTexCoord = vMaskTexCoord;
  maskTexCoord.y = mix(maskTexCoord.y, 1.0-maskTexCoord.y, uYFlipMaskTexture);
  mediump float maskAlpha = TEXTURE(sMaskTexture, maskTexCoord).a;
  textureColor.a *= maskAlpha;
  textureColor.rgb *= mix(1.0, maskAlpha, premultipliedAlpha);
#endif

#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
#ifndef IS_REQUIRED_DEBUG_VISUAL_SHADER
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    OUT_COLOR = textureColor;
  }
  else
#endif
  {
    calculateCornerRadius();
    calculateFragmentPosition();
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) && !defined(IS_REQUIRED_BORDERLINE) && !defined(IS_REQUIRED_DEBUG_VISUAL_SHADER)
    // skip rounded corner calculate for performance
    if(gFragmentPosition.x + gFragmentPosition.y < -(gRadius + vAliasMargin) * 2.0)
    {
      // Do nothing.
      OUT_COLOR = textureColor;
    }
    else
#endif
    {
#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
      calculatePosition();
      calculatePotential();
      setupMinMaxPotential();
#endif

#ifdef IS_REQUIRED_BORDERLINE
      textureColor = convertBorderlineColor(textureColor);
#endif
      OUT_COLOR = textureColor;

#ifdef IS_REQUIRED_ROUNDED_CORNER
      mediump float opacity = calculateCornerOpacity();
      OUT_COLOR.a *= opacity;
      OUT_COLOR.rgb *= mix(1.0, opacity, premultipliedAlpha);
#endif
    }

#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  }
#endif

#ifdef IS_REQUIRED_DEBUG_VISUAL_SHADER
  OUT_COLOR.rgb = ApplyDebugMixColor(OUT_COLOR);
#endif
}
