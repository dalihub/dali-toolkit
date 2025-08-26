//@name video-view-source.frag

//@version 100

precision highp float;
INPUT highp vec2 vPosition;
INPUT highp vec2 vOptRectSize;
INPUT highp vec4 vCornerRadius;
INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sPreviousTexture;
UNIFORM sampler2D sCurrentTexture;

UNIFORM_BLOCK SharedBlock
{
  UNIFORM highp vec3 uSize;
};

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM highp vec4 cornerSquareness; // input
  UNIFORM highp float uInterpolationFactor; // 0.0 for previous frame, 1.0 for current frame
};

// from https://iquilezles.org/articles/distfunctions
float roundedBoxSDF(vec2 PixelPositionFromCenter, vec2 RectangleEdgePositionFromCenter, float radius, float squareness)
{
  highp vec2 diff = PixelPositionFromCenter
                    - RectangleEdgePositionFromCenter
                    + radius;

  // If squareness is near 1.0 or 0.0, it make some numeric error. Let we avoid this situation by heuristic value.
  if(squareness < 0.01)
  {
    return length(max(diff, 0.0)) + min(0.0, max(diff.x, diff.y)) - radius;
  }
  if(squareness > 0.99)
  {
    return max(diff.x, diff.y) - radius;
  }

  highp vec2 positiveDiff = max(diff, 0.0);

  // make sqr to avoid duplicate codes.
  positiveDiff *= positiveDiff;

  // TODO : Could we remove this double-sqrt code?
  return sqrt(((positiveDiff.x + positiveDiff.y)
               + sqrt(positiveDiff.x * positiveDiff.x
                      + positiveDiff.y * positiveDiff.y
                      + (2.0 - 4.0 * squareness * squareness) * positiveDiff.x * positiveDiff.y))
              * 0.5)
         + min(0.0, max(diff.x, diff.y)) ///< Consider negative potential, to avoid visual defect when radius is zero
         - radius;
}

mediump float calculateCornerOpacity()
{
  // highp vec2 location = abs(vPosition);
  highp vec2 location = (vPosition.xy - vec2(0.5)) * uSize.xy;
  float radius =
      mix(
        mix(vCornerRadius.x, vCornerRadius.y, sign(location.x) * 0.5 + 0.5),
        mix(vCornerRadius.w, vCornerRadius.z, sign(location.x) * 0.5 + 0.5),
        sign(location.y) * 0.5 + 0.5
      );

  float edgeSoftness = min(1.0, radius);
  location = abs(location);
  float squareness =
    mix(
      mix(cornerSquareness.x, cornerSquareness.y, sign(location.x) * 0.5 + 0.5),
      mix(cornerSquareness.w, cornerSquareness.z, sign(location.x) * 0.5 + 0.5),
      sign(location.y) * 0.5 + 0.5
    );

  float distance = roundedBoxSDF(location, vOptRectSize, radius, squareness);
  float smoothedAlpha = 1.0 - smoothstep(-edgeSoftness, edgeSoftness, distance);
  return smoothedAlpha;
}

void main()
{
  vec4 currentColor = TEXTURE( sCurrentTexture, vTexCoord );
  vec4 previousColor = TEXTURE( sPreviousTexture, vTexCoord );
  vec4 interpolatedColor = mix(previousColor, currentColor, uInterpolationFactor);
  gl_FragColor = vec4(interpolatedColor.rgb * uColor.rgb, calculateCornerOpacity());
}
