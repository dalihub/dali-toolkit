#version 430

layout( location=0 ) in vec2 vTexCoord;
layout( location = 0 ) out vec4 fragColor;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  vec4 uColor;
};

layout( set=1, binding=1 ) uniform sampler2D sTexture;

void main()
{
  vec4 color = texture( sTexture, vTexCoord );
  if(color.a <= 0.0001)
  {
    discard;
  }
  fragColor = color * uColor;
}
