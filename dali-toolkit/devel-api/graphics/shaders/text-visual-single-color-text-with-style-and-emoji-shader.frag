#version 430

layout( location = 0 ) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  lowp float uHasMultipleTextColors;
  lowp vec4 uTextColorAnimatable;
  lowp vec4 uColor;
  lowp vec3 mixColor;
  lowp float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;
layout( set = 0, binding = 3 ) uniform sampler2D sStyle;
layout( set = 0, binding = 4 ) uniform sampler2D sMask;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  mediump vec4 textTexture = texture( sTexture, vTexCoord );
  mediump vec4 styleTexture = texture( sStyle, vTexCoord );
  mediump float maskTexture = texture( sMask, vTexCoord ).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  mediump float vstep = step( 0.0001, textTexture.a );
  textTexture.rgb = mix( textTexture.rgb, uTextColorAnimatable.rgb, vstep * maskTexture * ( 1.0 - uHasMultipleTextColors ) ) * mix( 1.0, textTexture.a, preMultipliedAlpha );

  // Draw the text as overlay above the style
  fragColor = ( textTexture + styleTexture * ( 1.0 - textTexture.a ) ) * uColor * vec4(mixColor,1.0);
}
