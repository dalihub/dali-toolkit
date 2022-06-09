INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vPosition;

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  vPosition = aPosition* visualSize;
  return vec4( vPosition + anchorPoint*visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}