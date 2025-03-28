//@name distance-field-effect.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;

UNIFORM_BLOCK FragBlock
{
  UNIFORM mediump float uGlowBoundary;
  UNIFORM mediump vec2  uOutlineParams;
  UNIFORM lowp    vec4  uOutlineColor;
  UNIFORM lowp    vec4  uShadowColor;
  UNIFORM mediump vec2  uShadowOffset;
  UNIFORM lowp    vec4  uGlowColor;
  UNIFORM lowp    float uDoOutline;
  UNIFORM lowp    float uDoShadow;
  UNIFORM lowp    float uDoGlow;
  UNIFORM lowp    vec4 uColor;
};

UNIFORM sampler2D sTexture;

void main()
{
  // sample distance field
  mediump float smoothing = 0.5;

  mediump float distance = TEXTURE(sTexture, vTexCoord).a;
  mediump float smoothWidth = fwidth(distance);
  mediump float alphaFactor = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, distance);
  lowp    vec4  color;

  if (uDoShadow == 0.0)
  {
    mediump float alpha = uColor.a * alphaFactor;
    lowp    vec4  rgb = uColor;

    if (uDoOutline > 0.0)
    {
      mediump float outlineWidth = uOutlineParams[1] + smoothWidth;
      mediump float outlineBlend = smoothstep(uOutlineParams[0] - outlineWidth, uOutlineParams[0] + outlineWidth, distance);
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
    mediump float shadowDistance = TEXTURE(sTexture, vTexCoord - uShadowOffset).a;
    mediump float inText = alphaFactor;
    mediump float inShadow = smoothstep(smoothing - smoothWidth, smoothing + smoothWidth, shadowDistance);

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