precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform vec2 uSampleOffsets[NUM_SAMPLES];
uniform float uSampleWeights[NUM_SAMPLES];

void main()
{
  vec4 color = vec4(0.0);
  for( int i = 0; i < NUM_SAMPLES; ++i )
  {
    color += texture2D( sTexture, vTexCoord + uSampleOffsets[i] ) * uSampleWeights[i];
  }
  gl_FragColor = color;
}