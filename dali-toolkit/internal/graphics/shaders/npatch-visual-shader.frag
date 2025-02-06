precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
}
