precision highp float;
varying highp vec2 vFragCoord;
varying highp vec2 vTexCoord;
varying highp vec4 vCornerRadius;
uniform highp vec3 uSize;
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
  inColor.rgb += rnd / 255.0;
  return inColor;
}

// from https://iquilezles.org/articles/distfunctions
float roundedBoxSDF(vec2 PixelPositionFromCenter, vec2 RectangleEdgePositionFromCenter, float Radius) {
    return length(max(abs(PixelPositionFromCenter)
                      - RectangleEdgePositionFromCenter
                      + Radius
                      , 0.0))
           - Radius;
}

void main()
{
  gl_FragColor = texture2D(sTexture, vTexCoord);

  highp vec2 location = vTexCoord.xy - vec2(0.5);
  float radius =
    mix(
      mix(vCornerRadius.x, vCornerRadius.y, sign(location.x)*0.5 + 0.5),
      mix(vCornerRadius.w, vCornerRadius.z, sign(location.x)*0.5 + 0.5),
      sign(location.y) * 0.5 + 0.5
     );

  float edgeSoftness = min(1.0, radius);
  float distance = roundedBoxSDF(vFragCoord.xy - (uSize.xy/2.0), uSize.xy/2.0, radius);

  float smoothedAlpha = 1.0 - smoothstep(-edgeSoftness, edgeSoftness, distance);
  gl_FragColor.a *= smoothedAlpha;

  gl_FragColor.rgb = applyDithering(gl_FragColor.rgb);
}
