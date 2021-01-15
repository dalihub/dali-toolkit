INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump float vCornerRadius;

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  mediump float dist = length( max( abs( vPosition ), vRectSize ) - vRectSize ) - vCornerRadius;
  OUT_COLOR = vec4(mixColor, 1.0) * uColor;
  OUT_COLOR.a *= 1.0 - smoothstep( -1.0, 1.0, dist );
}