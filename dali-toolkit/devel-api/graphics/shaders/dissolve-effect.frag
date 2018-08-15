#version 430

layout( location=0 ) in float vPercentage;
layout( location=1 ) in mediump vec2 vTexCoord;

layout( location = 0 ) out vec4 fragColor;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
   lowp vec4 uColor;
   vec4 uTextureRect;
};

layout( set=1, binding=1 ) uniform sampler2D sTexture;


float rand(vec2 co)
{
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
  // Calculate the randomness
  float offsetS = rand( vTexCoord * vPercentage ) - vTexCoord.s;
  float offsetT = rand( vec2(vTexCoord.t*vPercentage, vTexCoord.s * vPercentage) ) - vTexCoord.t;
  vec2 lookupCoord = vTexCoord + vec2(offsetS, offsetT) * vPercentage;
  fragColor = texture( sTexture, lookupCoord ) * uColor;
  fragColor.a *= 1.0 - vPercentage;

  fragColor = texture( sTexture, vTexCoord ) * uColor;
}
