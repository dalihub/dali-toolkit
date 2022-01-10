varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sStyle;
uniform sampler2D sOverlayStyle;
uniform sampler2D sMask;
uniform lowp float uHasMultipleTextColors;
uniform lowp vec4 uTextColorAnimatable;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  mediump vec4 textTexture = texture2D( sTexture, vTexCoord );
  mediump vec4 styleTexture = texture2D( sStyle, vTexCoord );
  mediump vec4 overlayStyleTexture = texture2D( sOverlayStyle, vTexCoord );
  mediump float maskTexture = texture2D( sMask, vTexCoord ).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  mediump float vstep = step( 0.0001, textTexture.a );
  textTexture.rgb = mix( textTexture.rgb, uTextColorAnimatable.rgb, vstep * maskTexture * ( 1.0 - uHasMultipleTextColors ) );

  // Draw the text as overlay above the style
  gl_FragColor = ( overlayStyleTexture + textTexture * (1.0 - overlayStyleTexture.a) + styleTexture * ( 1.0 - textTexture.a ) * (1.0 - overlayStyleTexture.a) ) * uColor * vec4( mixColor, 1.0 );
}
