precision highp float;
varying highp vec2 vTexCoord;

uniform highp float uGlowBoundary;
uniform highp vec2  uOutlineParams;
uniform lowp    vec4  uOutlineColor;
uniform lowp    vec4  uShadowColor;
uniform highp vec2  uShadowOffset;
uniform lowp    vec4  uGlowColor;
uniform lowp    float uDoOutline;
uniform lowp    float uDoShadow;
uniform lowp    float uDoGlow;

uniform sampler2D sTexture;
uniform lowp vec4 uColor;

void main()
{
  // sample distance field
  highp float smoothing = 0.5;

  highp float distance = texture2D(sTexture, vTexCoord).a;
  highp float smoothWidth = fwidth(distance);
  highp float alphaFactor = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, distance);
  lowp    vec4  color;

  if (uDoShadow == 0.0)
  {
    highp float alpha = uColor.a * alphaFactor;
    lowp    vec4  rgb = uColor;

    if (uDoOutline > 0.0)
    {
      highp float outlineWidth = uOutlineParams[1] + smoothWidth;
      highp float outlineBlend = smoothstep(uOutlineParams[0] - outlineWidth, uOutlineParams[0] + outlineWidth, distance);
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
    highp float shadowDistance = texture2D(sTexture, vTexCoord - uShadowOffset).a;
    highp float inText = alphaFactor;
    highp float inShadow = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, shadowDistance);

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

  gl_FragColor = color;
}