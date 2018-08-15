#version 430

layout ( set = 0, binding = 1, std140 ) uniform FragData
{
  lowp vec4 uColor;
};

layout( location = 0 ) out vec4 fragColor;

void main()
{
  fragColor = vec4(0.0);
}