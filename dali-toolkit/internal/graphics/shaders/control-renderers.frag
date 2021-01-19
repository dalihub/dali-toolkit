precision mediump float;
varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform vec4 uColor;

void main()
{
  gl_FragColor = texture2D(sTexture, vTexCoord);
  gl_FragColor *= uColor;
}
