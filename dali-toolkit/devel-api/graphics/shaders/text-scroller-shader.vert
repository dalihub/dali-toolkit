#version 430

layout(location=0) in mediump vec2 aPosition;
layout(location=0) out vec2 vTexCoord;

layout(set = 0, binding = 0, std140) uniform VertData
{
    mediump vec3 uSize;
    mediump float uDelta;
    mediump vec2 uTextureSize;
    mediump float uGap;
    mediump float uHorizontalAlign;
    mediump float uVerticalAlign;

    mediump mat4 uModelMatrix;
    mediump mat4 uViewMatrix;
    mediump mat4 uProjection;

    //Visual size and offset
    mediump vec2 offset;
    mediump vec2 size;
    mediump vec4 offsetSizeMode;
    mediump vec2 origin;
    mediump vec2 anchorPoint;
};

void main()
{
    mediump vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy );
    mediump vec2 visualSize = mix( uSize.xy * size, size, offsetSizeMode.zw );

    vTexCoord.x = ( uDelta + uHorizontalAlign * ( uTextureSize.x - visualSize.x - uGap ) + floor( aPosition.x * visualSize.x ) + 0.5 - uGap * 0.5 ) / uTextureSize.x + 0.5;
    vTexCoord.y = ( uVerticalAlign * ( uTextureSize.y - visualSize.y ) + floor( aPosition.y * visualSize.y ) + 0.5 ) / ( uTextureSize.y ) + 0.5;

    mediump vec4 vertexPosition = vec4( floor( ( aPosition + anchorPoint ) * visualSize + ( visualOffset + origin ) * uSize.xy ), 0.0, 1.0 );

    gl_Position = uProjection * uViewMatrix * uModelMatrix * vertexPosition;
}
