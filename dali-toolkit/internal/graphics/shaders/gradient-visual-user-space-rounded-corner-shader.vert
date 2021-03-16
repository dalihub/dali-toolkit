attribute mediump vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump mat3 uAlignmentMatrix;
varying mediump vec2 vTexCoord;
varying mediump vec2 vPosition;
varying mediump vec2 vRectSize;
varying mediump vec2 vOptRectSize;
varying mediump vec4 vCornerRadius;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump vec4 cornerRadius;
uniform mediump float cornerRadiusPolicy;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw);
  vec2 visualOffset = mix(offset, offset/uSize.xy, offsetSizeMode.xy);
  mediump float minSize = min(visualSize.x, visualSize.y);
  vCornerRadius = mix(cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);
  vRectSize = visualSize * 0.5;
  // Optimze fragment shader
  mediump float maxRadius = max(max(vCornerRadius.x, vCornerRadius.y), max(vCornerRadius.z, vCornerRadius.w));
  vOptRectSize = vRectSize - 0.2929 * maxRadius - 1.0;
  vCornerRadius = max(vCornerRadius, 1.0);
  vPosition = aPosition * visualSize;
  return vec4((aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0);
}

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * ComputeVertexPosition();

  vTexCoord = (uAlignmentMatrix*vertexPosition.xyw).xy;
}
