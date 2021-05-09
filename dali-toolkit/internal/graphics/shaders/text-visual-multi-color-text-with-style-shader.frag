varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sStyle;
uniform sampler2D sOverlayStyle;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  mediump vec4 textTexture = texture2D( sTexture, vTexCoord );
  mediump vec4 styleTexture = texture2D( sStyle, vTexCoord );
  mediump vec4 overlayStyleTexture = texture2D( sOverlayStyle, vTexCoord );

  // Draw the text as overlay above the style
  gl_FragColor = ( overlayStyleTexture + textTexture * (1.0 - overlayStyleTexture.a) + styleTexture * ( 1.0 - textTexture.a ) * (1.0 - overlayStyleTexture.a) ) * uColor * vec4( mixColor, 1.0 );
}
