precision highp float;
INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;
#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
OUTPUT highp vec2 vPosition;
FLAT OUTPUT highp vec2 vRectSize;
FLAT OUTPUT highp vec2 vOptRectSize;
FLAT OUTPUT highp float vAliasMargin;
#ifdef IS_REQUIRED_ROUNDED_CORNER
FLAT OUTPUT highp vec4 vCornerRadius;
#endif
#endif

#ifdef IS_REQUIRED_DEBUG_VISUAL_SHADER
#define DEBUG_EXTRA_ATTRIBUTES
#define DEBUG_EXTRA_VARYINGS

DEBUG_EXTRA_ATTRIBUTES
DEBUG_EXTRA_VARYINGS
#endif

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform highp vec4 pixelArea;

#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
// Be used when we calculate anti-alias range near 1 pixel.
uniform highp vec3 uScale;
#endif

//Visual size and offset
uniform highp vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
#ifdef IS_REQUIRED_BORDERLINE
uniform highp float borderlineWidth;
uniform highp float borderlineOffset;
#endif
#ifdef IS_REQUIRED_ROUNDED_CORNER
uniform highp vec4 cornerRadius;
uniform mediump float cornerRadiusPolicy;
#endif
#ifdef IS_REQUIRED_ALPHA_MASKING
OUTPUT  highp vec2  vMaskTexCoord;
uniform lowp  float cropToMask;
uniform highp vec2  maskTextureRatio;
#endif
uniform highp vec2 extraSize;

vec4 ComputeVertexPosition()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw) + extraSize;
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

#if defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER) || defined(IS_REQUIRED_BORDERLINE)
  vRectSize = visualSize * 0.5;
  vOptRectSize = vRectSize;

  // Set soft anti-alias range at most 10% of visual size.
  // The range should be inverse proportion with scale of view.
  // To avoid divid-by-zero, let we allow minimum scale value is 0.001 (0.1%)
  vAliasMargin = min(1.0, max(visualSize.x, visualSize.y) * 0.1) / max(0.001, max(uScale.x, uScale.y));

  highp float vertexMargin = 0.0;
#endif

#ifdef IS_REQUIRED_BORDERLINE
  // Extend size of visual by borderline.
  highp float outerBorderlineSize = (1.0 + clamp(borderlineOffset, -1.0, 1.0)) * borderlineWidth;
#endif

#ifdef IS_REQUIRED_ROUNDED_CORNER
#ifdef IS_REQUIRED_BORDERLINE
  highp float maxSize = max(visualSize.x, visualSize.y) + outerBorderlineSize;
  highp float minSize = min(visualSize.x, visualSize.y) + outerBorderlineSize;
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
  // Do not increase margin if the minRadius is small enough rather than maxSize.
  // TODO : We should change the magic parameter, 0.49
  vertexMargin = 2.0 * vAliasMargin * smoothstep(maxSize * 0.49, maxSize * 0.5, minRadius);
#endif

#ifdef IS_REQUIRED_BORDERLINE
  vPosition = aPosition * (visualSize + outerBorderlineSize + vertexMargin);
  vOptRectSize -= (borderlineWidth - outerBorderlineSize * 0.5) + 1.0;
#elif defined(IS_REQUIRED_DEBUG_VISUAL_SHADER) || defined(IS_REQUIRED_ROUNDED_CORNER)
  vPosition = aPosition * (visualSize + vertexMargin);
#else
  highp vec2 vPosition = aPosition * visualSize;
#endif

  highp vec4 finalPixelArea = pixelArea;
#ifdef IS_REQUIRED_ALPHA_MASKING
  finalPixelArea = mix(pixelArea,
                       vec4(
                            vec2(0.5) + (pixelArea.xy - vec2(0.5)) * maskTextureRatio,
                            pixelArea.zw * maskTextureRatio
                       ),
                       cropToMask);
  vMaskTexCoord = pixelArea.xy + pixelArea.zw * (vec2(0.5) + aPosition.xy
#ifdef IS_REQUIRED_BORDERLINE
                                                  * (1.0 + (outerBorderlineSize + vertexMargin) / visualSize)
#elif defined(IS_REQUIRED_ROUNDED_CORNER)
                                                  * (1.0 + vertexMargin / visualSize)
#endif
                                                );
#endif
  vTexCoord = finalPixelArea.xy + finalPixelArea.zw * (vec2(0.5) + aPosition.xy
#ifdef IS_REQUIRED_BORDERLINE
                                                        * (1.0 + (outerBorderlineSize + vertexMargin) / visualSize)
#elif defined(IS_REQUIRED_ROUNDED_CORNER)
                                                        * (1.0 + vertexMargin / visualSize)
#endif
                                                      );

  return vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0);
}

#ifdef IS_REQUIRED_DEBUG_VISUAL_SHADER
// These lines in the shader may be replaced with actual definitions by the debug-image-visual-shader-script.json.
#define DEBUG_APPLY_VARYING_CODE
#define DEBUG_EXTRA_UNIFORMS

DEBUG_EXTRA_UNIFORMS

void ApplyDebugVarying()
{
  DEBUG_APPLY_VARYING_CODE
}
#endif
void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();

#ifdef IS_REQUIRED_DEBUG_VISUAL_SHADER
  ApplyDebugVarying();
#endif
}
