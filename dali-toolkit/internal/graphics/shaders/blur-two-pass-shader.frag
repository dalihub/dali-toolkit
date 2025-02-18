//@name blur-two-pass-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM vec2 uSampleOffsets[NUM_SAMPLES];
  UNIFORM float uSampleWeights[NUM_SAMPLES];
};

void main()
{
  vec4 color = vec4(0.0);
  for( int i = 0; i < NUM_SAMPLES; ++i )
  {
    color += TEXTURE( sTexture, vTexCoord + uSampleOffsets[i] ) * uSampleWeights[i];
  }
  gl_FragColor = color;
}