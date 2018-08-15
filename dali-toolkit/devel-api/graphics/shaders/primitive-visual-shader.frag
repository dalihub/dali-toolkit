#version 430

layout(location=0) in   mediump vec3  vIllumination;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
   lowp vec4  uColor;
   lowp vec3  mixColor;
};

layout( location = 0 ) out vec4 fragColor;

void main()
{
    vec4 baseColor = vec4(mixColor, 1.0) * uColor;
    fragColor = vec4( vIllumination.rgb * baseColor.rgb, baseColor.a );
}
