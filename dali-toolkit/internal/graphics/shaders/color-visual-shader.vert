INPUT mediump vec2 aPosition;
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
OUTPUT mediump vec2 vPosition;
OUTPUT mediump vec2 vRectSize;
OUTPUT mediump vec2 vOptRectSize;
OUTPUT mediump float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
OUTPUT mediump vec4 vCornerRadius;
#endif
#endif

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
// Be used when we calculate anti-alias range near 1 pixel.
uniform highp vec3 uScale;
#endif

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
#ifdef IS_REQUIRED_BLUR
uniform mediump float blurRadius;
#elif defined(IS_REQUIRED_BORDERLINE)
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
#endif
#ifdef IS_REQUIRED_ROUNDED_CORNER
uniform highp vec4 cornerRadius;
uniform mediump float cornerRadiusPolicy;
#endif
uniform mediump vec2 extraSize;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
  vRectSize = visualSize * 0.5;
  vOptRectSize = vRectSize;

  // Set soft anti-alias range at most 10% of visual size.
  // The range should be inverse proportion with scale of view.
  // To avoid divid-by-zero, let we allow minimum scale value is 0.001 (0.1%)
  vAliasMargin = min(1.0, max(visualSize.x, visualSize.y) * 0.1) / max(0.001, max(uScale.x, uScale.y));

  mediump float vertexMargin = 0.0;
#endif

#ifdef IS_REQUIRED_ROUNDED_CORNER
#ifdef IS_REQUIRED_BLUR
  mediump float maxSize = max(visualSize.x, visualSize.y);
  mediump float minSize = min(visualSize.x, visualSize.y);
#elif defined(IS_REQUIRED_BORDERLINE)
  mediump float maxSize = max(visualSize.x, visualSize.y) + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth;
  mediump float minSize = min(visualSize.x, visualSize.y) + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth;
#else
  mediump float maxSize = max(visualSize.x, visualSize.y);
  mediump float minSize = min(visualSize.x, visualSize.y);
#endif
  vCornerRadius = mix(cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);
  // Optimize fragment shader. 0.2929 ~= 1.0 - sqrt(0.5)
  mediump float maxRadius = max(max(vCornerRadius.x, vCornerRadius.y), max(vCornerRadius.z, vCornerRadius.w));
  mediump float minRadius = min(min(vCornerRadius.x, vCornerRadius.y), min(vCornerRadius.z, vCornerRadius.w));
  vOptRectSize -= 0.2929 * maxRadius + 1.0;

  // Set vertex margin as vAliasMargin if we need to make some more fragments for alias.
#ifdef IS_REQUIRED_BLUR
  // Let we always increase alias margin for blur case
  vertexMargin = 2.0 * vAliasMargin;
#else
  // Do not increase margin if the minRadius is small enough rather than maxSize.
  // TODO : We should change the magic parameter, 0.49
  vertexMargin = 2.0 * vAliasMargin * smoothstep(maxSize * 0.49, maxSize * 0.5, minRadius);
#endif
#endif

#ifdef IS_REQUIRED_BLUR
  vPosition = aPosition * (visualSize + 2.0 * blurRadius + vertexMargin);
  vOptRectSize -= blurRadius + 1.0;
#elif defined(IS_REQUIRED_BORDERLINE)
  vPosition = aPosition * (visualSize + (1.0 + clamp(borderlineOffset, -1.0, 1.0))* borderlineWidth + vertexMargin);
  vOptRectSize -= (1.0 - clamp(borderlineOffset, -1.0, 1.0)) * 0.5 * borderlineWidth + 1.0;
#elif defined(IS_REQUIRED_ROUNDED_CORNER)
  vPosition = aPosition * (visualSize + vertexMargin);
#else
  mediump vec2 vPosition = aPosition * visualSize;
#endif
  return vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0);
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
