precision highp float;
INPUT highp vec2 vTexCoord;
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
INPUT highp vec2 vPosition;
FLAT INPUT highp vec2 vRectSize;
FLAT INPUT highp vec2 vOptRectSize;
FLAT INPUT highp float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
FLAT INPUT highp vec4 vCornerRadius;
#endif
#endif

// scale factor to fit start and end position of gradient.
uniform highp float uTextureCoordinateScaleFactor;

uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
#ifdef IS_REQUIRED_BORDERLINE
uniform highp float borderlineWidth;
uniform highp float borderlineOffset;
uniform lowp vec4 borderlineColor;
uniform lowp vec4 uActorColor;
#endif

#ifdef IS_REQUIRED_SQUIRCLE_CORNER
uniform highp vec4 cornerSquareness;
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
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
  // NOTE : gradient-visual is always preMultiplied.
  borderlineColorRGB *= borderlineColorAlpha;

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
    // If premultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
    // Else, return vec4((rgb*alpha) / alpha, alpha)

    highp float finalAlpha = mix(textureColor.a, 1.0, borderlineColorAlpha);
    highp vec3  finalMultipliedRGB = borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb;
    return vec4(finalMultipliedRGB, finalAlpha);
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

void main()
{
#ifdef RADIAL
  highp float radialTexCoord = ((length(vTexCoord) - 0.5) * uTextureCoordinateScaleFactor) + 0.5;
  lowp vec4 textureColor = TEXTURE(sTexture, vec2(radialTexCoord, 0.5)) * uColor;
#else
  lowp vec4 textureColor = TEXTURE(sTexture, vec2(vTexCoord.y, 0.5)) * uColor;
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    OUT_COLOR = textureColor;
  }
  else
  {
    calculateCornerRadius();
    calculateFragmentPosition();
#endif

#if defined(IS_REQUIRED_ROUNDED_CORNER) && !defined(IS_REQUIRED_BORDERLINE)
    // skip length and etc potential calculation for performance
    if(gFragmentPosition.x + gFragmentPosition.y < -(gRadius + vAliasMargin) * 2.0)
    {
      // Do nothing.
      OUT_COLOR = textureColor;
    }
    else
#endif
    {
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
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
      OUT_COLOR *= opacity;
#endif
    }

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  }
#endif
}
