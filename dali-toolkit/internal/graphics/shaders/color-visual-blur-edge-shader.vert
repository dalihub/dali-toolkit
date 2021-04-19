INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vPosition;
OUTPUT mediump vec2 vRectSize;

uniform highp   mat4 uMvpMatrix;
uniform highp vec3 uSize;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec2 extraSize;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump float blurRadius;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize + blurRadius * 2.0;
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
  vRectSize = visualSize / 2.0;
  vPosition = aPosition* visualSize;
  return vec4( vPosition + anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}