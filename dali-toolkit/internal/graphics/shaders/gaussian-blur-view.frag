varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform highp vec2 uSampleOffsets[NUM_SAMPLES];
uniform highp float uSampleWeights[NUM_SAMPLES];

void main()
{
  highp vec4 col = vec4(0.0);
  for (int i=0; i<NUM_SAMPLES; ++i)
  {
    col += (texture2D(sTexture, vTexCoord + uSampleOffsets[i]) + texture2D(sTexture, vTexCoord - uSampleOffsets[i])) * uSampleWeights[i];
  }
  gl_FragColor = col;
}
