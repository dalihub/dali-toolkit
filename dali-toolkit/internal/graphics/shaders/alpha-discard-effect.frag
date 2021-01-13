varying mediump vec2 vTexCoord;

uniform sampler2D sTexture;
uniform lowp vec4 uColor;

void main()
{
  mediump vec4 color = texture2D( sTexture, vTexCoord );
  if(color.a <= 0.0001)
  {
    discard;
  }
  gl_FragColor = color * uColor;
}
