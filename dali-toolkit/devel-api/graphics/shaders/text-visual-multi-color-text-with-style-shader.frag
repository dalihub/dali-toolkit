#version 430

layout( location = 0 ) in mediump vec2 vTexCoord;

layout( set = 1, binding = 0, std140 ) uniform FragData
{
  lowp vec4 uColor;
  lowp vec3 mixColor;
  lowp float preMultipliedAlpha;
};

layout( set = 1, binding = 1 ) uniform sampler2D sTexture;
layout( set = 1, binding = 2 ) uniform sampler2D sStyle;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  mediump vec4 textTexture = texture( sTexture, vTexCoord );
  mediump vec4 styleTexture = texture( sStyle, vTexCoord );
  textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );

  // Draw the text as overlay above the style
  fragColor = ( textTexture + styleTexture * ( 1.0 - textTexture.a ) ) * uColor * vec4(mixColor,1.0);
}
