//@name blur-effect.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM highp float uAnimationRatio;
  UNIFORM highp float uOpacity;
  UNIFORM highp vec2 uSampleOffsets[NUM_SAMPLES];
  UNIFORM highp float uSampleWeights[NUM_SAMPLES];
};

void main()
{
  highp vec4 col = vec4(0.0);
  for (int i=0; i<NUM_SAMPLES; ++i)
  {
    col += (TEXTURE(sTexture, vTexCoord + uSampleOffsets[i] * uAnimationRatio) + TEXTURE(sTexture, vTexCoord - uSampleOffsets[i] * uAnimationRatio)) * uSampleWeights[i];
  }
  col.w *= uOpacity;
  gl_FragColor = col;
}
