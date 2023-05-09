INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
OUTPUT mediump vec2 vPosition;
OUTPUT mediump vec2 vRectSize;
OUTPUT mediump vec2 vOptRectSize;
#ifdef IS_REQUIRED_ROUNDED_CORNER
OUTPUT mediump vec4 vCornerRadius;
#endif
#endif

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump mat3 uAlignmentMatrix;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
#ifdef IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
#endif
#ifdef IS_REQUIRED_ROUNDED_CORNER
uniform highp vec4 cornerRadius;
uniform mediump float cornerRadiusPolicy;
#endif

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  vRectSize = visualSize * 0.5;
  vOptRectSize = vRectSize;
#endif

#ifdef IS_REQUIRED_ROUNDED_CORNER
#ifdef IS_REQUIRED_BORDERLINE
  mediump float minSize = min(visualSize.x, visualSize.y) + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth;
#else
  mediump float minSize = min(visualSize.x, visualSize.y);
#endif
  vCornerRadius = mix(cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);
  // Optimize fragment shader. 0.2929 ~= 1.0 - sqrt(0.5)
  mediump float maxRadius = max(max(vCornerRadius.x, vCornerRadius.y), max(vCornerRadius.z, vCornerRadius.w));
  vOptRectSize -= 0.2929 * maxRadius + 1.0;
#endif

  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
#ifdef IS_REQUIRED_BORDERLINE
  vPosition = aPosition * (visualSize + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth);
  vertexPosition.xy *= (1.0 + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth / visualSize);
  vOptRectSize -= (1.0 - clamp(borderlineOffset, -1.0, 1.0)) * 0.5 * borderlineWidth + 1.0;
#elif defined(IS_REQUIRED_ROUNDED_CORNER)
  vPosition = aPosition * visualSize;
#else
  mediump vec2 vPosition = aPosition * visualSize;
#endif

#ifdef USER_SPACE
  vertexPosition.xyz *= uSize;
#endif

  vTexCoord = (uAlignmentMatrix*vertexPosition.xyw).xy;

  return vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0);
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
