uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
varying mediump vec2 vTexCoord;
varying mediump vec2 vPosition;
varying mediump vec2 vRectSize;
varying mediump vec2 vOptRectSize;
varying mediump vec4 vCornerRadius;

void main()
{
  gl_FragColor = texture2D( sTexture, vec2( length(vTexCoord), 0.5 ) ) * vec4(mixColor, 1.0) * uColor;
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
    gl_FragColor = vec4(0.0);
  }
  else if(dist > -1.0)
  {
    if(min(diff.x, diff.y) < 0.0)
    {
      dist += min(diff.x, diff.y) / max(radius, 1.0);
    }
    gl_FragColor *= 1.0 - smoothstep(-1.0, 1.0, dist);
  }
}
