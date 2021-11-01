#ifndef IS_REQUIRED_ROUNDED_CORNER
#define IS_REQUIRED_ROUNDED_CORNER 0
#endif
#ifndef IS_REQUIRED_BORDERLINE
#define IS_REQUIRED_BORDERLINE 0
#endif
#ifndef ATLAS_DEFAULT_WARP
#define ATLAS_DEFAULT_WARP 0
#endif
#ifndef ATLAS_CUSTOM_WARP
#define ATLAS_CUSTOM_WARP 0
#endif

INPUT mediump vec2 vTexCoord;
#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump vec2 vOptRectSize;
#if IS_REQUIRED_ROUNDED_CORNER
INPUT mediump vec4 vCornerRadius;
#endif
#endif

uniform sampler2D sTexture;
#if ATLAS_DEFAULT_WARP
uniform mediump vec4 uAtlasRect;
#elif ATLAS_CUSTOM_WARP
// WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
uniform lowp vec2 wrapMode;
#endif

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;
#if IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
uniform lowp vec4 borderlineColor;
uniform lowp vec4 uActorColor;
#endif

#if ATLAS_CUSTOM_WARP
mediump float wrapCoordinate( mediump vec2 range, mediump float coordinate, lowp float wrap )
{
  mediump float coord;
  if( wrap > 1.5 ) /* REFLECT */
    coord = 1.0 - abs(fract(coordinate*0.5)*2.0 - 1.0);
  else /* warp is 0 or 1 */
    coord = mix(coordinate, fract(coordinate), wrap);
  return clamp(mix(range.x, range.y, coord), range.x, range.y);
}
#endif

#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
// Global values both rounded corner and borderline use

// radius of rounded corner on this quadrant
mediump float gRadius = 0.0;

// fragment coordinate. NOTE : vec2(0.0, 0.0) is vRectSize, the corner of visual
mediump vec2 gFragmentPosition = vec2(0.0, 0.0);
// center coordinate of rounded corner circle. vec2(gCenterPosition, gCenterPosition).
mediump float gCenterPosition = 0.0;
// relative coordinate of gFragmentPosition from gCenterPosition.
mediump vec2 gDiff = vec2(0.0, 0.0);
// potential value what our algorithm use.
mediump float gPotential = 0.0;

// threshold of potential
mediump float gPotentialRange = 0.0;
mediump float gMaxOutlinePotential = 0.0;
mediump float gMinOutlinePotential = 0.0;
mediump float gMaxInlinePotential = 0.0;
mediump float gMinInlinePotential = 0.0;

void calculateCornerRadius()
{
#if IS_REQUIRED_ROUNDED_CORNER
  gRadius =
  mix(
    mix(vCornerRadius.x, vCornerRadius.y, sign(vPosition.x) * 0.5 + 0.5),
    mix(vCornerRadius.w, vCornerRadius.z, sign(vPosition.x) * 0.5 + 0.5),
    sign(vPosition.y) * 0.5 + 0.5
  );
#endif
}

void calculatePosition()
{
  gFragmentPosition = abs(vPosition) - vRectSize;
  gCenterPosition = -gRadius;
#if IS_REQUIRED_BORDERLINE
  gCenterPosition += borderlineWidth * (clamp(borderlineOffset, -1.0, 1.0) + 1.0) * 0.5;
#endif
  gDiff = gFragmentPosition - gCenterPosition;
}

void calculatePotential()
{
  gPotential = length(max(gDiff, 0.0)) + min(0.0, max(gDiff.x, gDiff.y));
}

void setupMinMaxPotential()
{
  gPotentialRange = 1.0;

  gMaxOutlinePotential = gRadius + gPotentialRange;
  gMinOutlinePotential = gRadius - gPotentialRange;

#if IS_REQUIRED_BORDERLINE
  gMaxInlinePotential = gMaxOutlinePotential - borderlineWidth;
  gMinInlinePotential = gMinOutlinePotential - borderlineWidth;
#else
  gMaxInlinePotential = gMaxOutlinePotential;
  gMinInlinePotential = gMinOutlinePotential;
#endif

  // reduce defect near edge of rounded corner.
  gMaxOutlinePotential += clamp(-min(gDiff.x, gDiff.y)/ max(1.0, gRadius) , 0.0, 1.0);
  gMinOutlinePotential += clamp(-min(gDiff.x, gDiff.y)/ max(1.0, gRadius) , 0.0, 1.0);
}

void PreprocessPotential()
{
  calculateCornerRadius();
  calculatePosition();
  calculatePotential();

  setupMinMaxPotential();
}
#endif

#if IS_REQUIRED_BORDERLINE
lowp vec4 convertBorderlineColor(lowp vec4 textureColor)
{
  mediump float potential = gPotential;

  // default opacity of borderline is 0.0
  mediump float borderlineOpacity = 0.0;

  // calculate borderline opacity by potential
  if(potential > gMinInlinePotential)
  {
    // potential is inside borderline range.
    borderlineOpacity = smoothstep(gMinInlinePotential, gMaxInlinePotential, potential);

    // Muliply borderlineWidth to resolve very thin borderline
    borderlineOpacity *= min(1.0, borderlineWidth);
  }

  lowp vec3  borderlineColorRGB   = borderlineColor.rgb * uActorColor.rgb;
  lowp float borderlineColorAlpha = borderlineColor.a * uActorColor.a;
  borderlineColorRGB *= mix(1.0, borderlineColorAlpha, preMultipliedAlpha);

  // Calculate inside of borderline when alpha is between (0.0  1.0). So we need to apply texture color.
  // If borderlineOpacity is exactly 0.0, we always use whole texture color. In this case, we don't need to run below code.
  // But if borderlineOpacity > 0.0 and borderlineColor.a == 0.0, we need to apply tCornerRadius.
  if(borderlineOpacity > 0.0 && borderlineColor.a * borderlineOpacity < 1.0)
  {
    mediump float tCornerRadius = -gCenterPosition;
    mediump float MaxTexturelinePotential = tCornerRadius + gPotentialRange;
    mediump float MinTexturelinePotential = tCornerRadius - gPotentialRange;
    if(potential > MaxTexturelinePotential)
    {
      // potential is out of texture range.
      textureColor = vec4(0.0);
    }
    else
    {
      // potential is in texture range.
      lowp float textureAlphaScale = mix(1.0, 0.0, smoothstep(MinTexturelinePotential, MaxTexturelinePotential, potential));
      textureColor.a *= textureAlphaScale;
      textureColor.rgb *= mix(textureColor.a, textureAlphaScale, preMultipliedAlpha);
    }

    borderlineColorAlpha *= borderlineOpacity;
    borderlineColorRGB *= mix(borderlineColorAlpha, borderlineOpacity, preMultipliedAlpha);
    // We use pre-multiplied color to reduce operations.
    // In here, textureColor and borderlineColorRGB is pre-multiplied color now.

    // Manual blend operation with premultiplied colors.
    // Final alpha = borderlineColorAlpha + (1.0 - borderlineColorAlpha) * textureColor.a.
    // (Final rgb * alpha) =  borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb
    // If preMultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
    // Else, return vec4((rgb*alpha) / alpha, alpha)

    lowp float finalAlpha = mix(textureColor.a, 1.0, borderlineColorAlpha);
    lowp vec3  finalMultipliedRGB = borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb;
    // TODO : Need to find some way without division
    return vec4(finalMultipliedRGB * mix(1.0 / finalAlpha, 1.0, preMultipliedAlpha), finalAlpha);
  }
  return mix(textureColor, vec4(borderlineColorRGB, borderlineColorAlpha), borderlineOpacity);
}
#endif

#if IS_REQUIRED_ROUNDED_CORNER
mediump float calculateCornerOpacity()
{
  mediump float potential = gPotential;

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

void main()
{
#if ATLAS_DEFAULT_WARP
  mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
#elif ATLAS_CUSTOM_WARP
  mediump vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );
#else
  mediump vec2 texCoord = vTexCoord;
#endif

  lowp vec4 textureColor = TEXTURE( sTexture, texCoord ) * vec4( mixColor, 1.0 ) * uColor;

#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    OUT_COLOR = textureColor;
    return;
  }
  PreprocessPotential();
#endif

#if IS_REQUIRED_BORDERLINE
  textureColor = convertBorderlineColor(textureColor);
#endif
  OUT_COLOR = textureColor;

#if IS_REQUIRED_ROUNDED_CORNER
  mediump float opacity = calculateCornerOpacity();
  OUT_COLOR.a *= opacity;
  OUT_COLOR.rgb *= mix(1.0, opacity, preMultipliedAlpha);
#endif
}
