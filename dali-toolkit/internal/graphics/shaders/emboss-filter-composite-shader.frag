precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uEffectColor;

void main()
{
  gl_FragColor = uEffectColor;
  gl_FragColor.a *= texture2D( sTexture, vTexCoord).a;
}