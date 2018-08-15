#version 430

layout( location = 0 ) in mediump vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform vertData
{
    mediump mat4 uModelMatrix;  // 0
    mediump mat4 uViewMatrix;   // 64
    mediump mat4 uProjection;   // 128
    mediump vec3 uSize;         // 192
    mediump vec4 pixelArea;     // 204
    lowp float uPixelAligned; // 220

    //Visual size and offset
    mediump vec2 offset; // 224
    mediump vec2 size; // 232
    mediump vec4 offsetSizeMode; // 240
    mediump vec2 origin; // 256
    mediump vec2 anchorPoint; // 264
    // 272
};

layout( location = 0 ) out mediump vec2 vTexCoord;

vec4 ComputeVertexPosition()
{
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
    mediump vec4 vertexPosition = uViewMatrix * uModelMatrix * ComputeVertexPosition();
    vec4 alignedVertexPosition = vertexPosition;
    alignedVertexPosition.xy = floor ( vertexPosition.xy ); // Pixel alignment
    vertexPosition = uProjection * mix( vertexPosition, alignedVertexPosition, uPixelAligned );
    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );
    gl_Position = vertexPosition;
}
