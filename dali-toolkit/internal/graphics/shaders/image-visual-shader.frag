INPUT mediump vec2 vTexCoord;
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump vec2 vOptRectSize;
INPUT mediump float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
INPUT mediump vec4 vCornerRadius;
#endif
#endif

uniform sampler2D sTexture;
#if defined(IS_REQUIRED_YUV_TO_RGB) || defined(IS_REQUIRED_UNIFIED_YUV_AND_RGB)
uniform sampler2D sTextureU;
uniform sampler2D sTextureV;
#endif

#ifdef IS_REQUIRED_ALPHA_MASKING
uniform sampler2D sMaskTexture;
INPUT mediump vec2 vMaskTexCoord;
#endif

#ifdef ATLAS_DEFAULT_WARP
uniform mediump vec4 uAtlasRect;
#elif defined(ATLAS_CUSTOM_WARP)
// WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
uniform lowp vec2 wrapMode;
#endif

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;
#ifdef IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
uniform lowp vec4 borderlineColor;
uniform lowp vec4 uActorColor;
#endif

#ifdef ATLAS_CUSTOM_WARP
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

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
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
#ifdef IS_REQUIRED_ROUNDED_CORNER
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
#ifdef IS_REQUIRED_BORDERLINE
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
  gPotentialRange = vAliasMargin;

  gMaxOutlinePotential = gRadius + gPotentialRange;
  gMinOutlinePotential = gRadius - gPotentialRange;

#ifdef IS_REQUIRED_BORDERLINE
  gMaxInlinePotential = gMaxOutlinePotential - borderlineWidth;
  gMinInlinePotential = gMinOutlinePotential - borderlineWidth;
#else
  gMaxInlinePotential = gMaxOutlinePotential;
  gMinInlinePotential = gMinOutlinePotential;
#endif

  // reduce defect near edge of rounded corner.
  gMaxOutlinePotential += clamp(-min(gDiff.x, gDiff.y) / max(1.0, gRadius), 0.0, 1.0);
  gMinOutlinePotential += clamp(-min(gDiff.x, gDiff.y) / max(1.0, gRadius), 0.0, 1.0);
}

void PreprocessPotential()
{
  calculateCornerRadius();
  calculatePosition();
  calculatePotential();

  setupMinMaxPotential();
}
#endif

#ifdef IS_REQUIRED_BORDERLINE
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
    borderlineOpacity *= min(1.0, borderlineWidth / gPotentialRange);
  }

  lowp vec3  borderlineColorRGB   = borderlineColor.rgb * uActorColor.rgb;
  lowp float borderlineColorAlpha = borderlineColor.a * uActorColor.a;
  borderlineColorRGB *= mix(1.0, borderlineColorAlpha, preMultipliedAlpha);

  // Calculate inside of borderline when alpha is between (0.0  1.0). So we need to apply texture color.
  // If borderlineOpacity is exactly 0.0, we always use whole texture color. In this case, we don't need to run below code.
  // But if borderlineOpacity > 0.0 and borderlineColor.a == 0.0, we need to apply tCornerRadius.
  if(borderlineOpacity > 0.0 && borderlineColor.a * borderlineOpacity < 1.0)
  {
    mediump float tCornerRadius = -gCenterPosition + gPotentialRange;
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

#ifdef IS_REQUIRED_ROUNDED_CORNER
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

#if defined(IS_REQUIRED_YUV_TO_RGB) || defined(IS_REQUIRED_UNIFIED_YUV_AND_RGB)
lowp vec4 ConvertYuvToRgba(mediump vec2 texCoord)
{
#ifdef IS_REQUIRED_UNIFIED_YUV_AND_RGB
  // Special case when shader use YUV but actual textures are not YUV format.
  // In this case, just resturn sTexture.
  if(textureSize(sTextureU, 0) != textureSize(sTextureV, 0))
  {
    return texture(sTexture, texCoord);
  }
#endif

  lowp float y = texture(sTexture, texCoord).r;
  lowp float u = texture(sTextureU, texCoord).r - 0.5;
  lowp float v = texture(sTextureV, texCoord).r - 0.5;
  lowp vec4 rgba;
  rgba.r = y + (1.403 * v);
  rgba.g = y - (0.344 * u) - (0.714 * v);
  rgba.b = y + (1.770 * u);
  rgba.a = 1.0;
  return rgba;
}
#endif

void main()
{
#ifdef ATLAS_DEFAULT_WARP
  mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
#elif defined(ATLAS_CUSTOM_WARP)
  mediump vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );
#else
  mediump vec2 texCoord = vTexCoord;
#endif

#if defined(IS_REQUIRED_YUV_TO_RGB) || defined(IS_REQUIRED_UNIFIED_YUV_AND_RGB)
  lowp vec4 textureColor = ConvertYuvToRgba(texCoord) * vec4( mixColor, 1.0 ) * uColor;
#else
  lowp vec4 textureColor = TEXTURE( sTexture, texCoord ) * vec4( mixColor, 1.0 ) * uColor;
#endif

#ifdef IS_REQUIRED_ALPHA_MASKING
  mediump float maskAlpha = TEXTURE(sMaskTexture, vMaskTexCoord).a;
  textureColor.a *= maskAlpha;
  textureColor.rgb *= mix(1.0, maskAlpha, preMultipliedAlpha);
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    OUT_COLOR = textureColor;
  }
  else
  {
    PreprocessPotential();
#endif

#ifdef IS_REQUIRED_BORDERLINE
    textureColor = convertBorderlineColor(textureColor);
#endif
    OUT_COLOR = textureColor;

#ifdef IS_REQUIRED_ROUNDED_CORNER
    mediump float opacity = calculateCornerOpacity();
    OUT_COLOR.a *= opacity;
    OUT_COLOR.rgb *= mix(1.0, opacity, preMultipliedAlpha);
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  }
#endif
}
