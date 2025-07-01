//@name color-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
OUTPUT highp vec2 vPosition;
FLAT OUTPUT highp vec2 vRectSize;
FLAT OUTPUT highp vec2 vOptRectSize;
FLAT OUTPUT highp float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
FLAT OUTPUT highp vec4 vCornerRadius;
#endif
#endif
#if defined(IS_REQUIRED_CUTOUT)
OUTPUT highp vec2 vPositionFromCenter;
#if defined(IS_REQUIRED_ROUNDED_CORNER)
FLAT OUTPUT highp vec4 vCutoutCornerRadius;
#endif
#endif

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
  // Be used when we calculate anti-alias range near 1 pixel.
  UNIFORM highp vec3 uScale;
#endif

#ifdef IS_REQUIRED_ROUNDED_CORNER
  UNIFORM mediump float cornerRadiusPolicy;
  UNIFORM highp vec4 cornerRadius;

#ifdef IS_REQUIRED_CUTOUT
  UNIFORM highp vec4 cutoutCornerRadius;
#endif
#endif
};

UNIFORM_BLOCK VisualVertBlock
{
  //Visual size and offset
  UNIFORM highp vec2 offset;
  UNIFORM highp vec2 size;
  UNIFORM highp vec2 extraSize;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 anchorPoint;
};

UNIFORM_BLOCK SharedBlock
{
  UNIFORM highp vec3 uSize;

#ifdef IS_REQUIRED_BLUR
  UNIFORM highp float blurRadius;
#endif
#ifdef IS_REQUIRED_BORDERLINE
  UNIFORM highp float borderlineWidth;
  UNIFORM highp float borderlineOffset;
#endif
};


vec4 ComputeVertexPosition()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

#if defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE) || defined(IS_REQUIRED_BLUR)
  vRectSize = visualSize * 0.5;
  vOptRectSize = vRectSize;

  // Set soft anti-alias range at most 10% of visual size.
  // The range should be inverse proportion with scale of view.
  // To avoid divid-by-zero, let we allow minimum scale value is 0.001 (0.1%)
  vAliasMargin = min(1.0, max(visualSize.x, visualSize.y) * 0.1) / max(0.001, max(uScale.x, uScale.y));

  highp float vertexMargin = 0.0;
#endif

#ifdef IS_REQUIRED_ROUNDED_CORNER
  #ifdef IS_REQUIRED_BORDERLINE
    highp float maxSize = max(visualSize.x, visualSize.y) + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth;
    highp float minSize = min(visualSize.x, visualSize.y) + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth;
  #else
    highp float maxSize = max(visualSize.x, visualSize.y);
    highp float minSize = min(visualSize.x, visualSize.y);
  #endif
  vCornerRadius = mix(cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);
  // Optimize fragment shader. 0.2929 ~= 1.0 - sqrt(0.5)
  highp float maxRadius = max(max(vCornerRadius.x, vCornerRadius.y), max(vCornerRadius.z, vCornerRadius.w));
  highp float minRadius = min(min(vCornerRadius.x, vCornerRadius.y), min(vCornerRadius.z, vCornerRadius.w));
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
  #ifdef IS_REQUIRED_BORDERLINE
    vPosition = aPosition * (visualSize + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth + 2.0 * blurRadius + vertexMargin);
    vOptRectSize -= (1.0 - clamp(borderlineOffset, -1.0, 1.0)) * 0.5 * borderlineWidth + blurRadius + 1.0;
  #else
    vPosition = aPosition * (visualSize + 2.0 * blurRadius + vertexMargin);
    vOptRectSize -= blurRadius + 1.0;
  #endif
#elif defined(IS_REQUIRED_BORDERLINE)
  vPosition = aPosition * (visualSize + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth + vertexMargin);
  vOptRectSize -= (1.0 - clamp(borderlineOffset, -1.0, 1.0)) * 0.5 * borderlineWidth + 1.0;
#elif defined(IS_REQUIRED_ROUNDED_CORNER)
  vPosition = aPosition * (visualSize + vertexMargin);
#else
  highp vec2 vPosition = aPosition * visualSize;
#endif

#if defined(IS_REQUIRED_CUTOUT)
  vPositionFromCenter = vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy;
#if defined(IS_REQUIRED_ROUNDED_CORNER)
  vCutoutCornerRadius = mix(cutoutCornerRadius * min(uSize.x, uSize.y), cutoutCornerRadius, cornerRadiusPolicy);
  vCutoutCornerRadius = min(vCutoutCornerRadius, min(uSize.x, uSize.y) * 0.5);
#endif
  return vec4(vPositionFromCenter, 0.0, 1.0);
#else
  return vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0);
#endif
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
