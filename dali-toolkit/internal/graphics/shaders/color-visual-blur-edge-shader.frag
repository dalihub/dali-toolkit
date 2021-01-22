INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump float vCornerRadius;

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform mediump float blurRadius;

void main()
{
  OUT_COLOR = vec4(mixColor, 1.0) * uColor;

  mediump vec2 v = abs(vPosition) - vRectSize;
  mediump float cy = vCornerRadius + blurRadius;
  mediump float cr = vCornerRadius + blurRadius;

  cy = min(cy, min(vRectSize.x, vRectSize.y));
  v = vec2(min(v.x, v.y), max(v.x, v.y));
  v = v + cy;

  mediump float blur = 1.0;
  mediump float potential = 0.0;
  mediump float alias = min(vCornerRadius, 1.0);
  mediump float potentialMin = cy + vCornerRadius - blurRadius - alias;
  mediump float potentialMax = cy + vCornerRadius + blurRadius + alias;

  // move center of circles for reduce defact
  mediump float cyDiff = min(cy, 0.2 * blurRadius);
  cy -= cyDiff;
  cr += cyDiff;

  mediump float diffFromBaseline = cy * v.y - (cy + cr) * v.x;

  if(diffFromBaseline > 0.0)
  {
    // out of calculation bound.
    potential = v.y;

    // for anti-alias when blurRaidus = 0.0
    mediump float heuristicBaselineScale = max(1.0 , cr * (cr + cy));
    mediump float potentialDiff = min(alias, diffFromBaseline / heuristicBaselineScale);
    potentialMin += potentialDiff;
    potentialMax -= potentialDiff;
  }
  else
  {
    // get some circle centered (x, x) and radius (r = cr / cy * x)
    // s.t. point v is on that circle
    // highest point of that circle is (x, x + r) and potential is x + r

    // solve (v.x - x)^2 + (v.y - x)^2 = (cr / cy * x)^2

    mediump float A = (cr * cr - 2.0 * cy * cy);
    mediump float B = cy * (v.x + v.y);
    mediump float V = dot(v,v);
    mediump float D = B * B + A * V;
    potential = V * (cr + cy) / (sqrt(D) + B);
  }

  blur = 1.0 - smoothstep(potentialMin, potentialMax, potential);
  OUT_COLOR.a *= blur;
}
