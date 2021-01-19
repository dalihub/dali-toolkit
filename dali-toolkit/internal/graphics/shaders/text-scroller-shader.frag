varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  if ( vTexCoord.y > 1.0 )
    discard;

  mediump vec4 textTexture = texture2D( sTexture, vTexCoord );

  gl_FragColor = textTexture * uColor * vec4( mixColor, 1.0 );
}
