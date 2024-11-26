INPUT mediump vec2 vTexCoord;
uniform sampler2D sTexture;
#ifdef IS_REQUIRED_STYLE
uniform sampler2D sStyle;
#endif
#ifdef IS_REQUIRED_OVERLAY
uniform sampler2D sOverlayStyle;
#endif
#ifdef IS_REQUIRED_MULTI_COLOR
#elif defined(IS_REQUIRED_EMOJI)
// Single color with emoji.
uniform sampler2D sMask;
uniform lowp float uHasMultipleTextColors;
#endif
uniform lowp vec4 uTextColorAnimatable;
uniform lowp vec4 uColor;

void main()
{
#ifdef IS_REQUIRED_STYLE
  mediump vec4 styleTexture = TEXTURE( sStyle, vTexCoord );
#endif

#ifdef IS_REQUIRED_OVERLAY
  mediump vec4 overlayStyleTexture = TEXTURE( sOverlayStyle, vTexCoord );
#endif

#if defined(IS_REQUIRED_MULTI_COLOR) || defined(IS_REQUIRED_EMOJI)
  // Multiple color or use emoji.
  mediump vec4 textColor = TEXTURE(sTexture, vTexCoord);

#ifdef IS_REQUIRED_MULTI_COLOR
#elif defined(IS_REQUIRED_EMOJI)
  // Single color with emoji.
  mediump float maskTexture = TEXTURE(sMask, vTexCoord).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  mediump float vstep = step( 0.0001, textColor.a );
  textColor.rgb = mix(textColor.rgb, uTextColorAnimatable.rgb, vstep * maskTexture * (1.0 - uHasMultipleTextColors));
#endif
#else
  // Single color without emoji.
  mediump float textTexture = TEXTURE(sTexture, vTexCoord).r;
  mediump vec4 textColor = uTextColorAnimatable * textTexture;
#endif

  // Draw the text as overlay above the style
  OUT_COLOR = uColor * (
#ifdef IS_REQUIRED_OVERLAY
                   (
#endif
                     textColor
#ifdef IS_REQUIRED_STYLE
                     + styleTexture * (1.0 - textColor.a)
#endif
#ifdef IS_REQUIRED_OVERLAY
                   ) * (1.0 - overlayStyleTexture.a) + overlayStyleTexture
#endif
                 );
}
