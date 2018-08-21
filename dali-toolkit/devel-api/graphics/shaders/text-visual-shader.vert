#version 430

// in variables
layout( location = 0 ) in mediump vec2 aPosition;

// uniforms
layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mediump mat4 uMvpMatrix;
  mediump vec3 uSize;
  mediump vec4 pixelArea;

  mediump mat4 uModelMatrix;
  mediump mat4 uViewMatrix;
  mediump mat4 uProjection;

  // Visual size and offset
  mediump vec2 offset;
  mediump vec2 size;
  mediump vec4 offsetSizeMode;
  mediump vec2 origin;
  mediump vec2 anchorPoint;
};

// out variables
layout( location = 0 ) out vec2 vTexCoord;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
  return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
  mediump vec4 nonAlignedVertex = uViewMatrix*uModelMatrix*ComputeVertexPosition();
  mediump vec4 pixelAlignedVertex = vec4 ( floor(nonAlignedVertex.xyz), 1.0 );
  mediump vec4 vertexPosition = uProjection*pixelAlignedVertex;

  vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );
  gl_Position = vertexPosition;
}
