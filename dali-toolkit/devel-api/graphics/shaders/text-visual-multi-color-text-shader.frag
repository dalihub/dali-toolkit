#version 430

layout( location = 0 ) in mediump vec2 vTexCoord;

// uniforms
layout( set = 0, binding = 1, std140 ) uniform FragData
{
  lowp vec4 uColor;
  lowp vec3 mixColor;
  lowp float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;

layout( location = 0 ) out vec4 fragColor;

void main()
{
  mediump vec4 textTexture = texture( sTexture, vTexCoord );
  textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );

  fragColor = textTexture * uColor * vec4(mixColor,1.0);
}
