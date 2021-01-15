attribute mediump vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump mat3 uAlignmentMatrix;
varying mediump vec2 vTexCoord;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

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
