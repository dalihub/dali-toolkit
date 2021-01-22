INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vPosition;
OUTPUT mediump vec2 vRectSize;
OUTPUT mediump float vCornerRadius;

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec2 extraSize;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump float cornerRadius;
uniform mediump float cornerRadiusPolicy;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
  mediump float minSize = min( visualSize.x, visualSize.y );
  vCornerRadius = mix( cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min( vCornerRadius, minSize * 0.5 );
  vRectSize = visualSize / 2.0 - vCornerRadius;
  vCornerRadius = max( vCornerRadius, 1.0 );
  vPosition = aPosition* visualSize;
  return vec4( vPosition + anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
