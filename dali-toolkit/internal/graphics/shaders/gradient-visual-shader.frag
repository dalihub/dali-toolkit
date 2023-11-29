INPUT mediump vec2 vTexCoord;
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump vec2 vOptRectSize;
#ifdef IS_REQUIRED_ROUNDED_CORNER
INPUT mediump vec4 vCornerRadius;
#endif
#endif

// scale factor to fit start and end position of gradient.
uniform mediump float uTextureCoordinateScaleFactor;

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
// Be used when we calculate anti-alias range near 1 pixel.
uniform highp vec3 uScale;
#endif

uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
#ifdef IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
uniform lowp vec4 borderlineColor;
uniform lowp vec4 uActorColor;
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
  // Set soft anti-alias range at most 10% of visual size.
  // The range should be inverse proportion with scale of view.
  // To avoid divid-by-zero, let we allow minimum scale value is 0.001 (0.1%)
  gPotentialRange = min(1.0, max(vRectSize.x, vRectSize.y) * 0.2) / max(0.001, max(uScale.x, uScale.y));

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
  // NOTE : gradient-visual is always preMultiplied.
  borderlineColorRGB *= borderlineColorAlpha;

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
      textureColor.rgb *= textureAlphaScale;
    }

    // NOTE : gradient-visual is always preMultiplied.
    borderlineColorAlpha *= borderlineOpacity;
    borderlineColorRGB *= borderlineOpacity;
    // We use pre-multiplied color to reduce operations.
    // In here, textureColor and borderlineColorRGB is pre-multiplied color now.

    // Manual blend operation with premultiplied colors.
    // Final alpha = borderlineColorAlpha + (1.0 - borderlineColorAlpha) * textureColor.a.
    // (Final rgb * alpha) =  borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb
    // If preMultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
    // Else, return vec4((rgb*alpha) / alpha, alpha)

    lowp float finalAlpha = mix(textureColor.a, 1.0, borderlineColorAlpha);
    lowp vec3  finalMultipliedRGB = borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb;
    return vec4(finalMultipliedRGB, finalAlpha);
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

void main()
{
#ifdef RADIAL
  mediump float radialTexCoord = ((length(vTexCoord) - 0.5) * uTextureCoordinateScaleFactor) + 0.5;
  lowp vec4 textureColor = TEXTURE(sTexture, vec2(radialTexCoord, 0.5)) * vec4(mixColor, 1.0) * uColor;
#else
  lowp vec4 textureColor = TEXTURE(sTexture, vec2(vTexCoord.y, 0.5)) * vec4(mixColor, 1.0) * uColor;
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
    OUT_COLOR *= opacity;
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  }
#endif
}
