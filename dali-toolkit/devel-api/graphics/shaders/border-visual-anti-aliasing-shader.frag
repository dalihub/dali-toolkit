#version 430

layout( location = 0 ) in mediump float vAlpha;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
  lowp vec4 uColor;
  lowp vec4 borderColor;
  lowp vec3 mixColor;
  mediump float borderSize;
};

layout( location = 0 ) out vec4 fragColor;

void main()
{
  fragColor = vec4(mixColor, 1.0)*borderColor*uColor;
  fragColor.a *= smoothstep(0.0, 1.5, vAlpha)*smoothstep( borderSize+1.5, borderSize, vAlpha );
}
