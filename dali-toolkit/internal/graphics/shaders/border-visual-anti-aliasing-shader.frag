INPUT mediump float vAlpha;

uniform lowp vec4 uColor;
uniform lowp vec4 borderColor;
uniform mediump float borderSize;

void main()
{
  OUT_COLOR = borderColor * uColor;
  OUT_COLOR.a *= smoothstep(0.0, 1.5, vAlpha) * smoothstep( borderSize + 1.5, borderSize, vAlpha );
}