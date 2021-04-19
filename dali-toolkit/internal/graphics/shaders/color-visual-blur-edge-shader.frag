INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform mediump float blurRadius;

void main()
{
  mediump vec2 blur = 1.0 - smoothstep( vRectSize - blurRadius * 2.0, vRectSize, abs( vPosition ) );
  OUT_COLOR = vec4(mixColor, 1.0) * uColor;
  OUT_COLOR.a *= blur.x * blur.y;
}