//@name bloom-view-extract-shader.frag

//@version 100

INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM mediump float uBloomThreshold;
  UNIFORM mediump float uRecipOneMinusBloomThreshold;
};

void main()
{
  mediump vec4 col;
  col = TEXTURE(sTexture, vTexCoord);
  col = (col - uBloomThreshold) * uRecipOneMinusBloomThreshold; // remove intensities lower than the thresold and remap intensities above the threshold to [0..1]
  gl_FragColor = clamp(col, 0.0, 1.0);
}