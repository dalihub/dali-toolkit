precision highp float;
varying float        vPercentage;
varying highp vec2 vTexCoord;

uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform vec4 uTextureRect;

float rand(vec2 co)
{
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
  //Calculate the randomness
  float  offsetS = rand(vTexCoord * vPercentage) - vTexCoord.s;
  float offsetT = rand(vec2(vTexCoord.t * vPercentage, vTexCoord.s * vPercentage)) - vTexCoord.t;
  vec2 lookupCoord = vTexCoord + vec2(offsetS, offsetT) * vPercentage;
  gl_FragColor = texture2D(sTexture, lookupCoord) * uColor;
  gl_FragColor.a *= 1.0 - vPercentage;
}