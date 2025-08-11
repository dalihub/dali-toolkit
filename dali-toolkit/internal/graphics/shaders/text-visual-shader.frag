//@name text-visual-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
#ifdef IS_REQUIRED_STYLE
UNIFORM sampler2D sStyle;
#endif
#ifdef IS_REQUIRED_OVERLAY
UNIFORM sampler2D sOverlayStyle;
#endif

#ifdef IS_REQUIRED_MULTI_COLOR
#elif defined(IS_REQUIRED_EMOJI)
// Single color with emoji.
UNIFORM sampler2D sMask;
#endif

UNIFORM_BLOCK FragBlock
{
  #ifdef IS_REQUIRED_MULTI_COLOR
  #elif defined(IS_REQUIRED_EMOJI)
  // Single color with emoji.
  UNIFORM lowp float uHasMultipleTextColors;
  #endif
  UNIFORM lowp vec4 uTextColorAnimatable;
  UNIFORM lowp vec4 uColor;

  #ifdef IS_REQUIRED_EMBOSS
  UNIFORM lowp vec2 uEmbossSize;
  UNIFORM lowp vec2 uEmbossDirection;
  UNIFORM lowp float uEmbossStrength;
  UNIFORM lowp vec4 uEmbossLightColor;
  UNIFORM lowp vec4 uEmbossShadowColor;
  #endif
};
void main()
{
#ifdef IS_REQUIRED_STYLE
  mediump vec4 styleTexture = TEXTURE( sStyle, vTexCoord );
#endif

#ifdef IS_REQUIRED_OVERLAY
  mediump vec4 overlayStyleTexture = TEXTURE( sOverlayStyle, vTexCoord );
#endif

mediump vec4 textColor;
#if defined(IS_REQUIRED_MULTI_COLOR) || defined(IS_REQUIRED_EMOJI)
  // Multiple color or use emoji.
  textColor = TEXTURE(sTexture, vTexCoord);
#ifdef IS_REQUIRED_EMBOSS
  // Multiple color or use emoji, with emboss
  mediump float textAlpha = textColor.a;
  mediump vec2 offset = normalize(uEmbossDirection) * uEmbossSize * uEmbossStrength;
  float light = TEXTURE(sTexture, clamp(vTexCoord - offset, 0.0, 1.0)).a;
  float shadow = TEXTURE(sTexture, clamp(vTexCoord + offset, 0.0, 1.0)).a;

  vec4 lightColor = vec4(uEmbossLightColor.rgb * light, light);
  vec4 shadowColor = vec4(uEmbossShadowColor.rgb * shadow, shadow);
  vec4 embossColor = lightColor + shadowColor;

  vec4 baseColor = vec4(uTextColorAnimatable.rgb * textAlpha, textAlpha);
  textColor = embossColor * (1.0 - textAlpha) + baseColor;
  textColor.a = max(textAlpha, embossColor.a);
#endif
#endif

#ifdef IS_REQUIRED_MULTI_COLOR
#elif defined(IS_REQUIRED_EMOJI)
  // Single color with emoji.
  mediump float maskTexture = TEXTURE(sMask, vTexCoord).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  mediump float vstep = step( 0.0001, textColor.a );
  textColor.rgb = mix(textColor.rgb, uTextColorAnimatable.rgb, vstep * maskTexture * (1.0 - uHasMultipleTextColors));
#elif defined(IS_REQUIRED_EMBOSS)
// Single color with emboss, without emoji.
  mediump float textAlpha = TEXTURE(sTexture, vTexCoord).r;
  mediump vec2 offset = normalize(uEmbossDirection) * uEmbossSize * uEmbossStrength;
  float light = TEXTURE(sTexture, clamp(vTexCoord - offset, 0.0, 1.0)).r;
  float shadow = TEXTURE(sTexture, clamp(vTexCoord + offset, 0.0, 1.0)).r;

  vec4 lightColor = vec4(uEmbossLightColor.rgb * light, light);
  vec4 shadowColor = vec4(uEmbossShadowColor.rgb * shadow, shadow);
  vec4 embossColor = lightColor + shadowColor;

  vec4 baseColor = vec4(uTextColorAnimatable.rgb * textAlpha, textAlpha);
  textColor = embossColor * (1.0 - textAlpha) + baseColor;
  textColor.a = max(textAlpha, embossColor.a);

#else
  // Single color without emoji.
  mediump float textTexture = TEXTURE(sTexture, vTexCoord).r;
  textColor = uTextColorAnimatable * textTexture;
#endif

  // Draw the text as overlay above the style
  gl_FragColor = uColor * (
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
