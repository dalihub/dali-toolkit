#version 430


layout(location=0) in mediump vec2 vTexCoord;

layout (constant_id = 0) const int NUM_SAMPLES = 16;

layout( set=0, binding=1, std140) uniform BlurData
{
  vec2 uSampleOffsets[NUM_SAMPLES];
  float uSampleWeights[NUM_SAMPLES];
};


layout(set=0, binding=2) uniform sampler2D sTexture;
layout(location=0) out vec4 fragColor;

void main()
{
  vec4 color = vec4(0.0);
  for( int i = 0; i < NUM_SAMPLES; ++i )
  {
    color += texture( sTexture, vTexCoord + uSampleOffsets[i] ) * uSampleWeights[i];
  }
  fragColor = color;
}
