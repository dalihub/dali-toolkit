#ifndef IS_REQUIRED_ROUNDED_CORNER
#define IS_REQUIRED_ROUNDED_CORNER 0
#endif
#ifndef IS_REQUIRED_BORDERLINE
#define IS_REQUIRED_BORDERLINE 0
#endif
#ifndef IS_REQUIRED_ALPHA_MASKING
#define IS_REQUIRED_ALPHA_MASKING 0
#endif

INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;
#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
OUTPUT mediump vec2 vPosition;
OUTPUT mediump vec2 vRectSize;
OUTPUT mediump vec2 vOptRectSize;
#if IS_REQUIRED_ROUNDED_CORNER
OUTPUT mediump vec4 vCornerRadius;
#endif
#endif

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump vec4 pixelArea;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
#if IS_REQUIRED_BORDERLINE
uniform mediump float borderlineWidth;
uniform mediump float borderlineOffset;
#endif
#if IS_REQUIRED_ROUNDED_CORNER
uniform mediump vec4 cornerRadius;
uniform mediump float cornerRadiusPolicy;
#endif
#if IS_REQUIRED_ALPHA_MASKING
OUTPUT  mediump vec2  vMaskTexCoord;
uniform lowp    float cropToMask;
uniform mediump vec2  maskTextureRatio;
#endif
uniform mediump vec2 extraSize;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy * size, size, offsetSizeMode.zw) + extraSize;
  vec2 visualOffset = mix(offset, offset/uSize.xy, offsetSizeMode.xy);

#if IS_REQUIRED_ROUNDED_CORNER || IS_REQUIRED_BORDERLINE
  vRectSize = visualSize * 0.5;
  vOptRectSize = vRectSize;
#endif

#if IS_REQUIRED_ROUNDED_CORNER
#if IS_REQUIRED_BORDERLINE
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

#if IS_REQUIRED_BORDERLINE
  vPosition = aPosition * (visualSize + (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth);
  vOptRectSize -= (1.0 - clamp(borderlineOffset, -1.0, 1.0)) * 0.5 * borderlineWidth + 1.0;
#elif IS_REQUIRED_ROUNDED_CORNER
  vPosition = aPosition * visualSize;
#else
  mediump vec2 vPosition = aPosition * visualSize;
#endif

  vec4 finalPixelArea = pixelArea;
#if IS_REQUIRED_ALPHA_MASKING
  finalPixelArea = mix(pixelArea,
                       vec4(
                            vec2(0.5) + (pixelArea.xy - vec2(0.5)) * maskTextureRatio,
                            pixelArea.zw * maskTextureRatio
                       ),
                       cropToMask);
  vMaskTexCoord = pixelArea.xy + pixelArea.zw * (vPosition.xy / max(vec2(1.0), visualSize) + vec2(0.5));
#endif
  vTexCoord = finalPixelArea.xy + finalPixelArea.zw * (vPosition.xy / max(vec2(1.0), visualSize) + vec2(0.5));

  return vec4(vPosition + anchorPoint * visualSize + (visualOffset + origin) * uSize.xy, 0.0, 1.0);
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
