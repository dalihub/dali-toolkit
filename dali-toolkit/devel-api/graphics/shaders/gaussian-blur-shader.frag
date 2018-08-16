#version 430
#define NUM_SAMPLES 5

layout(location=0) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140) uniform FragData
{
  vec4 uColor;
  vec2 uSampleOffsets[NUM_SAMPLES];
  float uSampleWeights[NUM_SAMPLES];
};

layout( set = 0, binding = 2) uniform sampler2D sTexture;
layout( location = 0 ) out vec4 fragColor;


void main()
{
   vec4 col = texture(sTexture, vTexCoord + uSampleOffsets[0]) * uSampleWeights[0];
   for (int i=1; i<NUM_SAMPLES; ++i)
   {
     col += texture(sTexture, vTexCoord + uSampleOffsets[i]) * uSampleWeights[i];
   }
   fragColor = col;
}
