precision highp float;
varying highp vec2 vTexCoord;
varying highp vec2 vOptRectSize;
varying highp vec4 vCornerRadius;

uniform lowp vec4 uColor;
uniform highp vec3 uSize;
uniform highp vec4 uCornerSquareness;
uniform sampler2D sTexture;

highp float nrand(const in vec2 uv)
{
  const highp float a = 12.9898, b = 78.233, c = 43758.5453, pi = 3.141592653589793;
  highp float dt = dot(uv, vec2(a, b)), sn = mod(dt, pi);
  return fract(sin(sn) * c);
}

vec3 applyDithering( vec3 inColor )
{
  float rnd = nrand(vTexCoord) - 0.5;
  inColor.rgb += rnd * 0.0039215686;
  return inColor;
}

// from https://iquilezles.org/articles/distfunctions
float roundedBoxSDF(vec2 PixelPositionFromCenter, vec2 RectangleEdgePositionFromCenter, float Radius, float Squareness) {
  highp vec2 positiveDiff = max(PixelPositionFromCenter
                                - RectangleEdgePositionFromCenter
                                + Radius
                                , 0.0);

  highp float squrenessFactor = Squareness / max(1.0, Radius * Radius);

  // make sqr to avoid duplicate codes.
  positiveDiff *= positiveDiff;

  return sqrt(positiveDiff.x + positiveDiff.y - squrenessFactor * positiveDiff.x * positiveDiff.y)
         - Radius;
}

void main()
{
  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;
  gl_FragColor.rgb = applyDithering(gl_FragColor.rgb);

  highp vec2 location = (vTexCoord.xy - vec2(0.5)) * uSize.xy;
  // skip most potential calculate for performance
  if(abs(location.x) < vOptRectSize.x && abs(location.y) < vOptRectSize.y)
  {
    // Do nothing.
  }
  else
  {
    float radius =
      mix(
        mix(vCornerRadius.x, vCornerRadius.y, sign(location.x) * 0.5 + 0.5),
        mix(vCornerRadius.w, vCornerRadius.z, sign(location.x) * 0.5 + 0.5),
        sign(location.y) * 0.5 + 0.5
      );
    float squareness =
      mix(
        mix(uCornerSquareness.x, uCornerSquareness.y, sign(location.x) * 0.5 + 0.5),
        mix(uCornerSquareness.w, uCornerSquareness.z, sign(location.x) * 0.5 + 0.5),
        sign(location.y) * 0.5 + 0.5
      );

    float edgeSoftness = min(1.0, radius);

    highp vec2 halfSize = uSize.xy * 0.5;
    location = abs(location);

    // For test, let we just linear function.
    if(location.x + location.y < halfSize.x + halfSize.y - radius - 2.0 * edgeSoftness)
    {
      // Do nothing
    }
    else
    {
      float distance = roundedBoxSDF(location, halfSize, radius, squareness);

      float smoothedAlpha = 1.0 - smoothstep(-edgeSoftness, edgeSoftness, distance);

      // Premultiply alpha feature used.
      gl_FragColor *= smoothedAlpha;
    }
  }
}
