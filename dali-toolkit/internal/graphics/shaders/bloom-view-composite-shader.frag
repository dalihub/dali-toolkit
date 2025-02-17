//@name bloom-view-composite-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM sampler2D sEffect;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM float uBloomIntensity;
  UNIFORM float uImageIntensity;
  UNIFORM float uBloomSaturation;
  UNIFORM float uImageSaturation;
};

vec4 ChangeSaturation(vec4 col, float sat)
{
  float grey = dot(col.rgb, vec3(0.3, 0.6, 0.1));
  return mix(vec4(grey, grey, grey, 1.0), col, sat);
}

void main()
{
  highp vec4 image;
  highp vec4 bloom;
  image = TEXTURE(sTexture, vTexCoord);
  bloom = TEXTURE(sEffect, vTexCoord);
  image = ChangeSaturation(image, uImageSaturation) * uImageIntensity;
  bloom = ChangeSaturation(bloom, uBloomSaturation) * uBloomIntensity;
  image *= 1.0 - clamp(bloom, 0.0, 1.0); // darken base where bloom is strong, to prevent excessive burn-out of result
  gl_FragColor = image + bloom;
}