#version 430

precision mediump float;

layout(location=0) in mediump vec2 vTexCoord;
layout(location=1) in mediump vec3 vIllumination;
layout(location=2) in mediump float vSpecular;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
    lowp vec4 uColor;
    lowp vec3 mixColor;
};

layout( set = 0, binding = 2 ) uniform sampler2D sDiffuse;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    vec4 tex = texture( sDiffuse, vTexCoord );
    vec4 visualMixColor = vec4(mixColor,1.0);
    fragColor = vec4( vIllumination.rgb * tex.rgb * uColor.rgb * visualMixColor.rgb + vSpecular * 0.3, tex.a * uColor.a * visualMixColor.a );
}
