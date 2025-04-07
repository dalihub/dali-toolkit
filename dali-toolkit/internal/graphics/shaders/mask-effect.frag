//@name mask-effect.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;

UNIFORM sampler2D uMaskTexture;
UNIFORM sampler2D uTargetTexture;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM highp float uMaskMode;
  UNIFORM highp vec2 uMaskPosition;
  UNIFORM highp vec2 uMaskScale;
};

void main()
{
  highp vec2 transformedUV = vTexCoord * uMaskScale + uMaskPosition;

  highp vec4 baseColor = TEXTURE(uTargetTexture, vTexCoord);
  highp vec4 maskColor = TEXTURE(uMaskTexture, transformedUV);
  highp float maskAlpha = 0.0f;

  highp float alphaMask = maskColor.a;
  highp float luminanceMask = dot(maskColor.rgb, vec3(0.299, 0.587, 0.114));

  highp float uInBounds = step(0.0, transformedUV.x) * step(transformedUV.x, 1.0) * step(0.0, transformedUV.y) * step(transformedUV.y, 1.0);

  maskAlpha = mix(alphaMask, luminanceMask, clamp(uMaskMode, 0.0f, 1.0f));
  maskAlpha *= uInBounds;

  gl_FragColor = baseColor * uColor * maskAlpha;
}
