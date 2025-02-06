precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 effectColor;

void main()
{
  gl_FragColor = effectColor;
  gl_FragColor.a *= texture2D( sTexture, vTexCoord).a;
}
