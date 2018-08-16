#version 430

layout(location=0) in vec2 aPosition;
layout(location=0) out vec2 vTexCoord;

layout(set = 0, binding = 0, std140) uniform VertData
{
    vec3 uSize;
    float uDelta;
    vec2 uTextureSize;
    float uGap;
    float uHorizontalAlign;
    float uVerticalAlign;

    mat4 uModelMatrix;
    mat4 uViewMatrix;
    mat4 uProjection;

    //Visual size and offset
    vec2 offset;
    vec2 size;
    vec4 offsetSizeMode;
    vec2 origin;
    vec2 anchorPoint;
};

void main()
{
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy );
    vec2 visualSize = mix( uSize.xy * size, size, offsetSizeMode.zw );

    vTexCoord.x = ( uDelta + uHorizontalAlign * ( uTextureSize.x - visualSize.x - uGap ) + floor( aPosition.x * visualSize.x ) + 0.5 - uGap * 0.5 ) / uTextureSize.x + 0.5;
    vTexCoord.y = ( uVerticalAlign * ( uTextureSize.y - visualSize.y ) + floor( aPosition.y * visualSize.y ) + 0.5 ) / ( uTextureSize.y ) + 0.5;

    vec4 vertexPosition = vec4( floor( ( aPosition + anchorPoint ) * visualSize + ( visualOffset + origin ) * uSize.xy ), 0.0, 1.0 );

    gl_Position = uProjection * uViewMatrix * uModelMatrix * vertexPosition;
}
