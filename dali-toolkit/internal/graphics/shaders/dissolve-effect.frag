//@name dissolve-effect.frag

//@version 100

precision highp float;

INPUT highp float vPercentage;
INPUT highp vec2  vTexCoord;

UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

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
  gl_FragColor = TEXTURE(sTexture, lookupCoord) * uColor;
  gl_FragColor.a *= 1.0 - vPercentage;
}