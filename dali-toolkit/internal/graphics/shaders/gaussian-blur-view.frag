varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform mediump vec2 uSampleOffsets[NUM_SAMPLES];
uniform mediump float uSampleWeights[NUM_SAMPLES];

void main()
{
  mediump vec4 col = texture2D(sTexture, vTexCoord + uSampleOffsets[0]) * uSampleWeights[0];
  for (int i=1; i<NUM_SAMPLES; ++i)
  {
    col += texture2D(sTexture, vTexCoord + uSampleOffsets[i]) * uSampleWeights[i];
  }
  gl_FragColor = col;
}
