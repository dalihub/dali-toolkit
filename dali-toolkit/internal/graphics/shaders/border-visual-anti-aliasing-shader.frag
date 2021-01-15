INPUT mediump float vAlpha;

uniform lowp vec4 uColor;
uniform lowp vec4 borderColor;
uniform lowp vec3 mixColor;
uniform mediump float borderSize;

void main()
{
  OUT_COLOR = vec4(mixColor, 1.0) * borderColor * uColor;
  OUT_COLOR.a *= smoothstep(0.0, 1.5, vAlpha) * smoothstep( borderSize + 1.5, borderSize, vAlpha );
}