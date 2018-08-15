#version 430

layout( location = 0 ) in mediump vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  lowp vec4 uTextColorAnimatable;
  lowp vec4 uColor;
  lowp vec3 mixColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;
layout( set = 0, binding = 3 ) uniform sampler2D sMask;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  mediump vec4 theTextTexture = texture( sTexture, vTexCoord );
  mediump float theMaskTexture = texture( sMask, vTexCoord ).r;

  // Set the color of non-transparent pixel in text to what it is animated to.
  // Markup text with multiple text colors are not animated (but can be supported later on if required).
  // Emoji color are not animated.
  mediump float vstep = step( 0.0001, theTextTexture.a );
  theTextTexture.rgb = mix( theTextTexture.rgb, uTextColorAnimatable.rgb, vstep * theMaskTexture );

  // Draw the text as overlay above the style
  fragColor = theTextTexture * uColor * vec4(mixColor,1.0);
}
