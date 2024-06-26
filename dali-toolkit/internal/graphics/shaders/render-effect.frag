precision highp float;
varying highp vec2 vFragCoord;
varying highp vec2 vTexCoord;
uniform highp vec3 uSize;
uniform sampler2D sTexture;
uniform highp vec4 uRadius;

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
                      ,0.0))
           - Radius;
}

float getCurrentRadius()
{
  if(vTexCoord.x < 0.5 && vTexCoord.y < 0.5)
  {
    return uRadius.x;
  }
  else if(vTexCoord.x < 0.5 && vTexCoord.y > 0.5)
  {
    return uRadius.y;
  }
  else if(vTexCoord.x > 0.5 && vTexCoord.y > 0.5)
  {
    return uRadius.z;
  }
  else
  {
    return uRadius.w;
  }
}

void main()
{
  gl_FragColor = texture2D(sTexture, vTexCoord);

  float edgeSoftness = 1.0;
  float distance = roundedBoxSDF(vFragCoord.xy - (uSize.xy/2.0), uSize.xy/2.0, getCurrentRadius());

  float smoothedAlpha = 1.0 - smoothstep(0.0, edgeSoftness * 2.0, distance);
  gl_FragColor.a *= smoothedAlpha;

  gl_FragColor.rgb = applyDithering(gl_FragColor.rgb);
}
