INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump vec2 vOptRectSize;
INPUT mediump vec4 vCornerRadius;

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  OUT_COLOR = vec4(mixColor, 1.0) * uColor;
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    return;
  }
  mediump float radius =
  mix(
    mix(vCornerRadius.x, vCornerRadius.y, sign(vPosition.x) * 0.5 + 0.5),
    mix(vCornerRadius.w, vCornerRadius.z, sign(vPosition.x) * 0.5 + 0.5),
    sign(vPosition.y) * 0.5 + 0.5
  );

  mediump vec2 diff = abs(vPosition) - vRectSize + radius;
  mediump float dist = length(max(diff, vec2(0.0))) - radius;
  if(dist > 1.0)
  {
    OUT_COLOR.a = 0.0;
  }
  else if(dist > -1.0)
  {
    if(min(diff.x, diff.y) < 0.0)
    {
      dist += min(diff.x, diff.y) / max(radius, 1.0);
    }
    OUT_COLOR.a *= 1.0 - smoothstep(-1.0, 1.0, dist);
  }
}
