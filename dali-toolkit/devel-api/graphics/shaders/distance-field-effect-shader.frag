#version 430

layout(location=0) in vec2 vTexCoord;
layout( location = 0 ) out vec4 fragColor;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  float uGlowBoundary;
  vec2  uOutlineParams;
  vec4  uOutlineColor;
  vec4  uShadowColor;
  vec2  uShadowOffset;
  vec4  uGlowColor;
  float uDoOutline;
  float uDoShadow;
  float uDoGlow;
  vec4  uColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;

void main()
{
  // sample distance field
  float smoothing = 0.5;

  float distance = texture(sTexture, vTexCoord).a;
  float smoothWidth = fwidth(distance);
  float alphaFactor = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, distance);
  vec4  color;
  if (uDoShadow == 0.0)
  {
    float alpha = uColor.a * alphaFactor;
    vec4  rgb = uColor;

    if (uDoOutline > 0.0)
    {
      float outlineWidth = uOutlineParams[1] + smoothWidth;
      float outlineBlend = smoothstep(uOutlineParams[0] - outlineWidth, uOutlineParams[0] + outlineWidth, distance);
      alpha = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, distance);
      rgb = mix(uOutlineColor, uColor, outlineBlend);
    }

    if (uDoGlow > 0.0)
    {
      rgb = mix(uGlowColor, rgb, alphaFactor);
      alpha = smoothstep(uGlowBoundary, smoothing, distance);
    }

    // set fragment color
    color = vec4(rgb.rgb, alpha);
  }

  else // (uDoShadow > 0.0)
  {
    float shadowDistance = texture(sTexture, vTexCoord - uShadowOffset).a;
    float inText = alphaFactor;
    float inShadow = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, shadowDistance);

    // inside object, outside shadow
    if (inText == 1.0)
    {
      color = uColor;
    }
    // inside object, outside shadow
    else if ((inText != 0.0) && (inShadow == 0.0))
    {
      color = uColor;
      color.a *= inText;
    }
    // outside object, completely inside shadow
    else if ((inText == 0.0) && (inShadow == 1.0))
    {
      color = uShadowColor;
    }
    // inside object, completely inside shadow
    else if ((inText != 0.0) && (inShadow == 1.0))
    {
      color = mix(uShadowColor, uColor, inText);
      color.a = uShadowColor.a;
    }
    // inside object, inside shadow's border
    else if ((inText != 0.0) && (inShadow != 0.0))
    {
      color = mix(uShadowColor, uColor, inText);
      color.a *= max(inText, inShadow);
    }
    // inside shadow's border
    else if (inShadow != 0.0)
    {
      color = uShadowColor;
      color.a *= inShadow;
    }
    // outside shadow and object
    else
    {
      color.a = 0.0;
    }

  }

  fragColor = color;
}
