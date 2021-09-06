#ifndef IS_REQUIRED_ROUNDED_CORNER
#define IS_REQUIRED_ROUNDED_CORNER 0
#endif
#ifndef IS_REQUIRED_BORDERLINE
#define IS_REQUIRED_BORDERLINE 0
#endif
#ifndef IS_REQUIRED_BLUR
#define IS_REQUIRED_BLUR 0
#endif

#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE || IS_REQUIRED_BLUR
INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump vec2 vOptRectSize;
#if IS_REQUIRED_ROUNDED_CORNER
INPUT mediump vec4 vCornerRadius;
#endif
#endif

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
#if !IS_REQUIRED_BLUR && IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
uniform lowp vec4 borderlineColor;
uniform lowp vec4 uActorColor;
#endif
#if IS_REQUIRED_BLUR
uniform mediump float blurRadius;
#endif


#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE || IS_REQUIRED_BLUR
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
#if !IS_REQUIRED_BLUR && IS_REQUIRED_BORDERLINE
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

#if !IS_REQUIRED_BLUR && IS_REQUIRED_BORDERLINE
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

#if !IS_REQUIRED_BLUR && IS_REQUIRED_BORDERLINE
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
  }

  lowp vec3  BorderlineColorRGB   = borderlineColor.rgb * uActorColor.rgb;
  lowp float BorderlineColorAlpha = borderlineColor.a * uActorColor.a;

  //calculate inside of borderline when outilneColor.a < 1.0
  if(borderlineColor.a < 1.0)
  {
    mediump float tCornerRadius = -gCenterPosition;
    mediump float MaxTexturelinePotential = tCornerRadius + gPotentialRange;
    mediump float MinTexturelinePotential = tCornerRadius - gPotentialRange;
    if(potential > MaxTexturelinePotential)
    {
      // potential is out of texture range. use borderline color instead of texture
      textureColor = vec4(BorderlineColorRGB, 0.0);
    }
    else if(potential > MinTexturelinePotential)
    {
      // potential is in texture range
      textureColor = mix(textureColor, vec4(BorderlineColorRGB, 0.0), smoothstep(MinTexturelinePotential, MaxTexturelinePotential, potential));
    }
    // TODO : need to fix here when uColor.a = 0.0 and uActorColor.a != 0
    borderlineOpacity *= borderlineColor.a;
    return mix(textureColor, vec4(BorderlineColorRGB, 1.0), borderlineOpacity);
  }
  return mix(textureColor, vec4(BorderlineColorRGB, BorderlineColorAlpha), borderlineOpacity);
}
#endif

#if !IS_REQUIRED_BLUR && IS_REQUIRED_ROUNDED_CORNER
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

#if IS_REQUIRED_BLUR
mediump float calculateBlurOpacity()
{
// Don't use borderline!
  mediump vec2 v = gDiff;
  mediump float cy = gRadius + blurRadius;
  mediump float cr = gRadius + blurRadius;

#if IS_REQUIRED_ROUNDED_CORNER
  // This routine make perfect circle. If corner radius is not exist, we don't consider prefect circle.
  cy = min(cy, min(vRectSize.x, vRectSize.y) - gRadius);
#endif
  v = vec2(min(v.x, v.y), max(v.x, v.y));
  v = v + cy;

  mediump float potential = 0.0;
  mediump float alias = min(gRadius, 1.0);
  mediump float potentialMin = cy + gRadius - blurRadius - alias;
  mediump float potentialMax = cy + gRadius + blurRadius + alias;

  // move center of circles for reduce defact
  mediump float cyDiff = min(cy, 0.2 * blurRadius);
  cy -= cyDiff;
  cr += cyDiff;

  mediump float diffFromBaseline = cy * v.y - (cy + cr) * v.x;

  if(diffFromBaseline > 0.0)
  {
    // out of calculation bound.
    potential = v.y;

    // for anti-alias when blurRaidus = 0.0
    mediump float heuristicBaselineScale = max(1.0 , cr * (cr + cy));
    mediump float potentialDiff = min(alias, diffFromBaseline / heuristicBaselineScale);
    potentialMin += potentialDiff;
    potentialMax -= potentialDiff;
  }
  else
  {
    // get some circle centered (x, x) and radius (r = cr / cy * x)
    // s.t. point v is on that circle
    // highest point of that circle is (x, x + r) and potential is x + r

    // solve (v.x - x)^2 + (v.y - x)^2 = (cr / cy * x)^2
#if IS_REQUIRED_ROUNDED_CORNER
    // NOTE : lowspec HW cannot calculate here. need to reduce numeric error
    mediump float A = (cr * cr - 2.0 * cy * cy);
    mediump float B = cy * (v.x + v.y);
    mediump float V = dot(v,v);
    mediump float D = B * B + A * V;
    potential = V * (cr + cy) / (sqrt(D) + B);
#else
    // We can simplify this value cause cy = 0.8 * blurRadius, cr = 1.2 * blurRadius
    // potential = 5.0*(sqrt(4.0*(v.x+v.y)^2 + dot(v,v)) - 2.0*(v.x+v.y));
    //           = 10.0*(v.x+v.y) * (sqrt(1.0 + (length(v) / (2.0*(v.x+v.y)))^2) - 1.0);
    //           = 10.0*(v.x+v.y) * (sqrt(1.25 - x + x^2) - 1.0);
    //          ~= 10.0*(v.x+v.y) * (0.11803399 - 0.44721360x + 0.35777088x^2 - 0.14310x^3 + O(x^4)) (Taylor series)
    //          ~= -1.0557281 * (v.x + v.y) + 2.236068 * length(v) - ~~~ (here, x <= 0.5 * (1.0 - sqrt(0.5)) < 0.1464467)
    // Note : This simplify need cause we should use it on lowspec HW.
    mediump float x = 0.5 * (1.0 - length(v) / (v.x + v.y));
    potential = -1.0557281 * (v.x + v.y) + 2.236068 * length(v) + 10.0 * (v.x + v.y) * (0.35777088 - 0.14310 * x) * x * x;
#endif
  }

  return 1.0 - smoothstep(potentialMin, potentialMax, potential);
}
#endif

void main()
{
  lowp vec4 targetColor = vec4(mixColor, 1.0) * uColor;

#if IS_REQUIRED_BLUR || IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    OUT_COLOR = targetColor;
    return;
  }
  PreprocessPotential();
#endif

#if !IS_REQUIRED_BLUR && IS_REQUIRED_BORDERLINE
  targetColor = convertBorderlineColor(targetColor);
#endif
  OUT_COLOR = targetColor;

#if IS_REQUIRED_BLUR
  mediump float opacity = calculateBlurOpacity();
  OUT_COLOR.a *= opacity;
#elif IS_REQUIRED_ROUNDED_CORNER
  mediump float opacity = calculateCornerOpacity();
  OUT_COLOR.a *= opacity;
#endif
}
