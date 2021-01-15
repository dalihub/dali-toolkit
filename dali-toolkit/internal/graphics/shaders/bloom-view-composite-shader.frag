precision mediump float;
varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sEffect;
uniform lowp vec4 uColor;
uniform float uBloomIntensity;
uniform float uImageIntensity;
uniform float uBloomSaturation;
uniform float uImageSaturation;

vec4 ChangeSaturation(vec4 col, float sat)
{
  float grey = dot(col.rgb, vec3(0.3, 0.6, 0.1));
  return mix(vec4(grey, grey, grey, 1.0), col, sat);
}

void main()
{
  mediump vec4 image;
  mediump vec4 bloom;
  image = texture2D(sTexture, vTexCoord);
  bloom = texture2D(sEffect, vTexCoord);
  image = ChangeSaturation(image, uImageSaturation) * uImageIntensity;
  bloom = ChangeSaturation(bloom, uBloomSaturation) * uBloomIntensity;
  image *= 1.0 - clamp(bloom, 0.0, 1.0); // darken base where bloom is strong, to prevent excessive burn-out of result
  gl_FragColor = image + bloom;
}