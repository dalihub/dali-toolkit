varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sStyle;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  mediump vec4 textTexture = texture2D( sTexture, vTexCoord );
  mediump vec4 styleTexture = texture2D( sStyle, vTexCoord );

  // Draw the text as overlay above the style
  gl_FragColor = ( textTexture + styleTexture * ( 1.0 - textTexture.a ) ) * uColor * vec4( mixColor, 1.0 );
}
