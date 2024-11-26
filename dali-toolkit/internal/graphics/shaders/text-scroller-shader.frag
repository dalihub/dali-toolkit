varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;

void main()
{
  if ( vTexCoord.y > 1.0 )
    discard;

  mediump vec4 textTexture = texture2D( sTexture, vTexCoord );

  gl_FragColor = textTexture * uColor;
}
