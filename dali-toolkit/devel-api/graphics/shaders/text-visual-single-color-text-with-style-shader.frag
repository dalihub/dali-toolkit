#version 430

layout( location = 0 ) in mediump vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  lowp vec4 uTextColorAnimatable;
  lowp vec4 uColor;
  lowp vec3 mixColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;
layout( set = 0, binding = 3 ) uniform sampler2D sStyle;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  mediump float textTexture = texture( sTexture, vTexCoord ).r;
  mediump vec4 styleTexture = texture( sStyle, vTexCoord );

  // Draw the text as overlay above the style
  fragColor = ( uTextColorAnimatable * textTexture + styleTexture * ( 1.0 - uTextColorAnimatable.a * textTexture ) ) * uColor * vec4(mixColor,1.0);
}
