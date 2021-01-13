INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;
OUTPUT mediump vec2 vPosition;
OUTPUT mediump vec2 vRectSize;
OUTPUT mediump float vCornerRadius;

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump vec4 pixelArea;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump float cornerRadius;
uniform mediump float cornerRadiusPolicy;
uniform mediump vec2 extraSize;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
  mediump float minSize = min( visualSize.x, visualSize.y );
  vCornerRadius = mix( cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min( vCornerRadius, minSize * 0.5 );
  vRectSize = visualSize * 0.5 - vCornerRadius;
  vPosition = aPosition* visualSize;
  return vec4( vPosition + anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
  vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );
}