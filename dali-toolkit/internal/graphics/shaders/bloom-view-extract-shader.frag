precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform highp float uBloomThreshold;
uniform highp float uRecipOneMinusBloomThreshold;

void main()
{
  highp vec4 col;
  col = texture2D(sTexture, vTexCoord);
  col = (col - uBloomThreshold) * uRecipOneMinusBloomThreshold; // remove intensities lower than the thresold and remap intensities above the threshold to [0..1]
  gl_FragColor = clamp(col, 0.0, 1.0);
}