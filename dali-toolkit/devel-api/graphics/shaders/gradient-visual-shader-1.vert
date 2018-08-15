#version 430

layout(location=0) in mediump vec2 aPosition;
layout(location=0) out mediump vec2 vTexCoord;

layout(set = 0, binding = 0, std140) uniform VertData
{
    mediump mat4 uMvpMatrix;
    mediump vec3 uSize;
    mediump mat3 uAlignmentMatrix;

    //Visual size and offset
    mediump vec2 offset;
    mediump vec2 size;
    mediump vec4 offsetSizeMode;
    mediump vec2 origin;
    mediump vec2 anchorPoint;
};

vec4 ComputeVertexPosition()
{
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
    vertexPosition.xyz *= uSize;
    gl_Position = uMvpMatrix * ComputeVertexPosition();

    vTexCoord = (uAlignmentMatrix*vertexPosition.xyw).xy;
}
