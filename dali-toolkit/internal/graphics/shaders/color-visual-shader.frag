//@name color-visual-shader.frag

//@version 100

precision highp float;

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
INPUT highp vec2 vPosition;
FLAT INPUT highp vec2 vRectSize;
FLAT INPUT highp vec2 vOptRectSize;
FLAT INPUT highp float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
FLAT INPUT highp vec4 vCornerRadius;
#endif
#endif
#if defined(IS_REQUIRED_CUTOUT)
INPUT highp vec2 vPositionFromCenter;
#if defined(IS_REQUIRED_ROUNDED_CORNER)
FLAT INPUT highp vec4 vCutoutCornerRadius;
#endif
#endif

UNIFORM_BLOCK FragBlock
{
    UNIFORM lowp vec4 uColor;
#if defined(IS_REQUIRED_BORDERLINE)
    UNIFORM lowp vec4 uActorColor;
#endif

#if defined(IS_REQUIRED_CUTOUT)
    UNIFORM lowp int uCutoutWithCornerRadius;
    UNIFORM lowp int uCutoutOutside;
#endif

#ifdef IS_REQUIRED_BORDERLINE
    UNIFORM lowp vec4 borderlineColor;
#endif

#ifdef IS_REQUIRED_SQUIRCLE_CORNER
    UNIFORM highp vec4 cornerSquareness;
#endif
};

UNIFORM_BLOCK SharedBlock
{
    UNIFORM highp vec3 uSize;

#ifdef IS_REQUIRED_BLUR
    UNIFORM highp float blurRadius;
#endif
#ifdef IS_REQUIRED_BORDERLINE
    UNIFORM highp float borderlineWidth;
    UNIFORM highp float borderlineOffset;
#endif
};


#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
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

void calculateCornerRadius(highp vec4 cornerRadius, highp vec2 position)
{
#ifdef IS_REQUIRED_ROUNDED_CORNER
  gRadius =
  mix(
    mix(cornerRadius.x, cornerRadius.y, sign(position.x) * 0.5 + 0.5),
    mix(cornerRadius.w, cornerRadius.z, sign(position.x) * 0.5 + 0.5),
    sign(position.y) * 0.5 + 0.5
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
void calculateFragmentPosition(highp vec2 position, highp vec2 halfSizeOfRect)
{
  gFragmentPosition = abs(position) - halfSizeOfRect;
}

void calculatePosition(highp float currentBorderlineWidth)
{
  gCenterPosition = -gRadius;
#ifdef IS_REQUIRED_BORDERLINE
  gCenterPosition += currentBorderlineWidth * (clamp(borderlineOffset, -1.0, 1.0) + 1.0) * 0.5;
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

void setupMinMaxPotential(highp float currentBorderlineWidth, float heuristicFactor)
{
  gPotentialRange = vAliasMargin;

  gMaxOutlinePotential = gRadius + gPotentialRange;
  gMinOutlinePotential = gRadius - gPotentialRange;

#ifdef IS_REQUIRED_BORDERLINE
  gMaxInlinePotential = gMaxOutlinePotential - currentBorderlineWidth;
  gMinInlinePotential = gMinOutlinePotential - currentBorderlineWidth;
#endif

  // reduce defect near edge of rounded corner.
  highp float heuristicEdgeCasePotential = clamp(-min(gDiff.x, gDiff.y) / max(1.0, gRadius), 0.0, gPotentialRange * heuristicFactor);
  gMaxOutlinePotential += heuristicEdgeCasePotential;
  gMinOutlinePotential += heuristicEdgeCasePotential;
}

void PreprocessPotential(highp vec4 cornerRadius, highp vec2 position, highp vec2 halfSizeOfRect, highp float currentBorderlineWidth, float heuristicFactor)
{
  calculateCornerRadius(cornerRadius, position);
  calculateFragmentPosition(position, halfSizeOfRect);
  calculatePosition(currentBorderlineWidth);
  calculatePotential();
  setupMinMaxPotential(currentBorderlineWidth, heuristicFactor);
}
#endif

#ifdef IS_REQUIRED_BORDERLINE
#ifdef IS_REQUIRED_BLUR
// TODO : Current logic is only for PoC! We should make clean up!
lowp vec4 convertBorderlineColorWithBlur(lowp vec4 textureColor,highp float currentBorderlineWidth, highp float blurRadius)
{
  highp float potential = gPotential;

  blurRadius = max(blurRadius, 0.0) + vAliasMargin;

  highp vec3  borderlineColorRGB   = borderlineColor.rgb * uActorColor.rgb;
  highp float borderlineColorAlpha = borderlineColor.a * uActorColor.a;
  // NOTE : color-visual is always preMultiplied.
  borderlineColorRGB *= borderlineColorAlpha;

  highp float borderlineOpacity = 0.0;
  highp float textureOpacity = 0.0;

  highp float outsideThreshold = gRadius;
  highp float insideThreshold  = gRadius - currentBorderlineWidth;
  highp float textureOutlineThreshold = -gCenterPosition;

  // borderlineOpacity should be 0~1 whenever potential over insideThreshold
  // and 1~0 whenever potential outsideThreshold.
  // To combine this 2 information, we can use smoothstep and multiply function.

  borderlineOpacity = smoothstep(insideThreshold - blurRadius, insideThreshold + blurRadius, potential) *
                      (1.0 - smoothstep(outsideThreshold - blurRadius, outsideThreshold + blurRadius, potential));

  textureOpacity = 1.0 - smoothstep(textureOutlineThreshold - blurRadius, textureOutlineThreshold + blurRadius, potential);

  // NOTE : color-visual is always preMultiplied.
  borderlineColorRGB *= borderlineOpacity;
  borderlineColorAlpha *= borderlineOpacity;
  textureColor *= textureOpacity;

  // Manual blend operation with premultiplied colors.
  // Final alpha = borderlineColorAlpha + (1.0 - borderlineColorAlpha) * textureColor.a.
  // (Final rgb * alpha) =  borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb
  // If premultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
  // Else, return vec4((rgb*alpha) / alpha, alpha)

  highp float finalAlpha = mix(textureColor.a, 1.0, borderlineColorAlpha);
  highp vec3  finalMultipliedRGB = borderlineColorRGB + (1.0 - borderlineColorAlpha) * textureColor.rgb;
  return vec4(finalMultipliedRGB, finalAlpha);
}
#else
lowp vec4 convertBorderlineColor(lowp vec4 textureColor)
{
  highp float potential = gPotential;

  // default opacity of borderline is 0.0
  highp float borderlineOpacity = 0.0;

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
  // NOTE : color-visual is always preMultiplied.
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

    // NOTE : color-visual is always preMultiplied.
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
#endif

#ifdef IS_REQUIRED_BLUR
#elif defined(IS_REQUIRED_ROUNDED_CORNER)
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

#ifdef IS_REQUIRED_BLUR
#if defined(SL_VERSION_LOW) || defined(IS_REQUIRED_SQUIRCLE_CORNER) || defined(IS_REQUIRED_BORDERLINE)
// Legacy code for low version glsl
mediump float calculateBlurOpacity()
{
  highp float potential = gPotential;

  highp float alias = min(gRadius, vAliasMargin);
  highp float potentialMin = gMinOutlinePotential - blurRadius - alias;
  highp float potentialMax = gMaxOutlinePotential + blurRadius + alias;

  return 1.0 - smoothstep(potentialMin, potentialMax, potential);
}
#else
mediump float calculateBlurOpacity()
{
  // TODO : Need to consider squareness.

  // Don't use borderline!
  highp vec2 v = gDiff;
  highp float cy = gRadius + blurRadius;
  highp float cr = gRadius + blurRadius;

#ifdef IS_REQUIRED_ROUNDED_CORNER
  // This routine make perfect circle. If corner radius is not exist, we don't consider prefect circle.
  cy = min(cy, min(vRectSize.x, vRectSize.y) - gRadius);
#endif
  v = vec2(min(v.x, v.y), max(v.x, v.y));
  v = v + cy;

  highp float potential = 0.0;
  highp float alias = min(gRadius, vAliasMargin);
  highp float potentialMin = cy + gRadius - blurRadius - alias;
  highp float potentialMax = cy + gRadius + blurRadius + alias;

  // move center of circles for reduce defact
  highp float cyDiff = min(cy, 0.2 * blurRadius);
  cy -= cyDiff;
  cr += cyDiff;

  highp float diffFromBaseline = cy * v.y - (cy + cr) * v.x;

  if(diffFromBaseline > 0.0)
  {
    // out of calculation bound.
    potential = v.y;

    // for anti-alias when blurRaidus = 0.0
    highp float heuristicBaselineScale = max(vAliasMargin , cr * (cr + cy));
    highp float potentialDiff = min(alias, diffFromBaseline / heuristicBaselineScale);
    potentialMin += potentialDiff;
    potentialMax -= potentialDiff;
  }
  else
  {
    // get some circle centered (x, x) and radius (r = cr / cy * x)
    // s.t. point v is on that circle
    // highest point of that circle is (x, x + r) and potential is x + r

    // solve (v.x - x)^2 + (v.y - x)^2 = (cr / cy * x)^2
#ifdef IS_REQUIRED_ROUNDED_CORNER
    // Note : lowspec HW cannot calculate here. need to reduce numeric error
    highp float A = (cr * cr - 2.0 * cy * cy);
    highp float B = cy * (v.x + v.y);
    highp float V = dot(v,v);
    highp float D = B * B + A * V;
    potential = V * (cr + cy) / (sqrt(D) + B);
#else
    // We can simplify this value cause cy = 0.8 * blurRadius, cr = 1.2 * blurRadius
    // potential = 5.0*(sqrt(4.0*(v.x+v.y)^2 + dot(v,v)) - 2.0*(v.x+v.y));
    //           = 10.0*(v.x+v.y) * (sqrt(1.0 + (length(v) / (2.0*(v.x+v.y)))^2) - 1.0);
    //           = 10.0*(v.x+v.y) * (sqrt(1.25 - x + x^2) - 1.0);
    //          ~= 10.0*(v.x+v.y) * (0.11803399 - 0.44721360x + 0.35777088x^2 - 0.14310x^3 + O(x^5)) (Taylor series)
    //          ~= -1.0557281 * (v.x + v.y) + 2.236068 * length(v) - ~~~ (here, x <= 0.5 * (1.0 - sqrt(0.5)) < 0.1464467)
    // Note : This simplify need cause we should use it on lowspec HW.
    highp float x = 0.5 * (1.0 - length(v) / (v.x + v.y));
    potential = -1.0557281 * (v.x + v.y) + 2.236068 * length(v) + 10.0 * (v.x + v.y) * (0.35777088 - 0.14310 * x) * x * x;
#endif
  }

  return 1.0 - smoothstep(potentialMin, potentialMax, potential);
}
#endif
#endif

void main()
{
  lowp vec4 targetColor = uColor;

#ifdef IS_REQUIRED_CUTOUT
  mediump float discardOpacity = 1.0;

  mediump float cutoutVertexMargin = 0.0;
#if defined(IS_REQUIRED_ROUNDED_CORNER)
  if(uCutoutWithCornerRadius == 1)
  {
    cutoutVertexMargin += vAliasMargin * float(uCutoutOutside) * 1.5;
  }
#endif

  if(abs(vPositionFromCenter.x) <= uSize.x * 0.5 + cutoutVertexMargin && abs(vPositionFromCenter.y) <= uSize.y * 0.5 + cutoutVertexMargin)
  {
#if defined(IS_REQUIRED_ROUNDED_CORNER)
    if(uCutoutWithCornerRadius == 1)
    {
      // Ignore borderline width
      PreprocessPotential(vCutoutCornerRadius, vPositionFromCenter, uSize.xy * 0.5, 0.0, 0.0);

      // Decrease potential range, to avoid alias make some hole.
      // For cutout case, move hole insde 1 pixels
      // For cutout outside case, move hole outside 0.5 pixels
      gMinOutlinePotential += cutoutVertexMargin - vAliasMargin;
      gMaxOutlinePotential += cutoutVertexMargin - vAliasMargin;

      discardOpacity = smoothstep(gMinOutlinePotential, gMaxOutlinePotential, gPotential);
    }
    else
    {
      discardOpacity = 0.0;
    }
#else
    discardOpacity = 0.0;
#endif
  }

  // discardOpacity = 1.0 mean it is outside of the view.
  // discardOpacity = 0.0 mean it is inside of the view, where we want to cutout
  if(uCutoutOutside == 1)
  {
    // Revert cutout area if uCutoutOutside is 1
    discardOpacity = 1.0 - discardOpacity;
  }

  if(discardOpacity < 0.001)
  {
    discard;
  }
#endif

#if defined(IS_REQUIRED_BLUR) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    gl_FragColor = targetColor;
  }
  else
  {
    highp vec4 tempCornerRadius = vec4(0.0);
    highp float tempBorderlineWidth = 0.0;
#ifdef IS_REQUIRED_ROUNDED_CORNER
    tempCornerRadius = vCornerRadius;
#endif
    calculateCornerRadius(tempCornerRadius, vPosition);
    calculateFragmentPosition(vPosition, vRectSize);
#endif

#ifdef IS_REQUIRED_BLUR
#ifdef IS_REQUIRED_BORDERLINE
    tempBorderlineWidth = borderlineWidth;
#endif
    calculatePosition(tempBorderlineWidth);
    calculatePotential();

#ifdef IS_REQUIRED_BORDERLINE
    gl_FragColor = convertBorderlineColorWithBlur(targetColor, tempBorderlineWidth, blurRadius);
#else
    setupMinMaxPotential(tempBorderlineWidth, 1.0);

    gl_FragColor = targetColor;

    mediump float opacity = calculateBlurOpacity();
    gl_FragColor *= opacity;
#endif
#else
#if defined(IS_REQUIRED_ROUNDED_CORNER) && !defined(IS_REQUIRED_BORDERLINE)
    // skip rounded corner calculate for performance
    if(gFragmentPosition.x + gFragmentPosition.y < -(gRadius + vAliasMargin) * 2.0)
    {
      // Do nothing.
      gl_FragColor = targetColor;
    }
    else
#endif
    {
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
#ifdef IS_REQUIRED_BORDERLINE
      tempBorderlineWidth = borderlineWidth;
#endif
      calculatePosition(tempBorderlineWidth);
      calculatePotential();
      setupMinMaxPotential(tempBorderlineWidth, 1.0);

#ifdef IS_REQUIRED_BORDERLINE
      targetColor = convertBorderlineColor(targetColor);
#endif
#endif

      gl_FragColor = targetColor;

#ifdef IS_REQUIRED_ROUNDED_CORNER
      mediump float opacity = calculateCornerOpacity();
      gl_FragColor *= opacity;
#endif
    }
#endif

#if defined(IS_REQUIRED_BLUR) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  }
#endif

#ifdef IS_REQUIRED_CUTOUT
  gl_FragColor *= discardOpacity;
#endif
}
