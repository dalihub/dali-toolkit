//@version 100

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColorFactor;// Color from material
  UNIFORM lowp float uMask;
  UNIFORM lowp float uAlphaThreshold;
  UNIFORM int uIsShadowCasting;
};

INPUT mediump vec2 vUV;
INPUT lowp vec4 vColor;

#ifdef THREE_TEX
#ifdef BASECOLOR_TEX
UNIFORM sampler2D sAlbedoAlpha;
#endif // BASECOLOR_TEX
#else // THREE_TEX
UNIFORM sampler2D sAlbedoMetal;
#endif

lowp vec3 linear(lowp vec3 color)
{
  return pow(color, vec3(2.2));
}

void main()
{
  if(uIsShadowCasting == 0)
  {
    discard;
  }

#ifdef THREE_TEX
  // The albedo may be defined from a base texture or a flat color
#ifdef BASECOLOR_TEX
  lowp vec4 baseColor = TEXTURE(sAlbedoAlpha, vUV);
  baseColor = vColor * vec4(linear(baseColor.rgb), baseColor.w) * uColorFactor;
#else // BASECOLOR_TEX
  lowp vec4 baseColor = vColor * uColorFactor;
#endif // BASECOLOR_TEX
#else // THREE_TEX
  lowp vec4 albedoMetal = TEXTURE(sAlbedoMetal, vUV);
  lowp vec4 baseColor = vec4(linear(albedoMetal.rgb), 1.0) * vColor * uColorFactor;
#endif // THREE_TEX

  // The value of uOpaque and uMask can be 0.0 or 1.0.
  // If uMask is 1.0, a Pixel that has bigger alpha than uAlphaThreashold becomes fully opaque,
  // and, a pixel that has smaller alpha than uAlphaThreashold becomes fully transparent.
  // If uOpaque is 1.0, alpha value of final color is 1.0;
  // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#_material_alphamode
  if(uMask > 0.5 && baseColor.a < uAlphaThreshold)
  {
    discard;
  }
}