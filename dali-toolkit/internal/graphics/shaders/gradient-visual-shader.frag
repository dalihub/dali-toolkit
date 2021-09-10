#ifndef IS_REQUIRED_ROUNDED_CORNER
#define IS_REQUIRED_ROUNDED_CORNER 0
#endif
#ifndef IS_REQUIRED_BORDERLINE
#define IS_REQUIRED_BORDERLINE 0
#endif
#ifndef RADIAL
#define RADIAL 0
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

uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
#if IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
uniform lowp vec4 borderlineColor;
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
  }

  //calculate inside of borderline when outilneColor.a < 1.0
  if(borderlineColor.a < 1.0)
  {
    mediump float tCornerRadius = -gCenterPosition;
    mediump float MaxTexturelinePotential = tCornerRadius + gPotentialRange;
    mediump float MinTexturelinePotential = tCornerRadius - gPotentialRange;
    lowp vec3 BorderlineColorRGB = borderlineColor.xyz * borderlineColor.a;
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
    borderlineOpacity *= borderlineColor.a;
    return mix(textureColor, vec4(BorderlineColorRGB, 1.0), borderlineOpacity);
  }
  return mix(textureColor, borderlineColor, borderlineOpacity);
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
#if RADIAL
  lowp vec4 textureColor = TEXTURE(sTexture, vec2(length(vTexCoord), 0.5)) * vec4(mixColor, 1.0);
#else
  lowp vec4 textureColor = TEXTURE(sTexture, vec2(vTexCoord.y, 0.5)) * vec4(mixColor, 1.0);
#endif

#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    OUT_COLOR = textureColor * uColor;
    return;
  }
  PreprocessPotential();
#endif

#if IS_REQUIRED_BORDERLINE
  textureColor = convertBorderlineColor(textureColor);
#endif
  OUT_COLOR = textureColor * uColor;

#if IS_REQUIRED_ROUNDED_CORNER
  mediump float opacity = calculateCornerOpacity();
  OUT_COLOR *= opacity;
#endif
}
