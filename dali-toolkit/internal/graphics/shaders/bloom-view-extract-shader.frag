//@name bloom-view-extract-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM highp float uBloomThreshold;
  UNIFORM highp float uRecipOneMinusBloomThreshold;
};

void main()
{
  highp vec4 col;
  col = TEXTURE(sTexture, vTexCoord);
  col = (col - uBloomThreshold) * uRecipOneMinusBloomThreshold; // remove intensities lower than the thresold and remap intensities above the threshold to [0..1]
  gl_FragColor = clamp(col, 0.0, 1.0);
}