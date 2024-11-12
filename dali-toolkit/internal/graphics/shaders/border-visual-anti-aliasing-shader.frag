//@name border-visual-anti-aliasing-shader.frag

//@version 100

INPUT mediump float vAlpha;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 borderColor;
  UNIFORM mediump float borderSize;
};

void main()
{
  gl_FragColor = borderColor * uColor;
  gl_FragColor.a *= smoothstep(0.0, 1.5, vAlpha) * smoothstep( borderSize + 1.5, borderSize, vAlpha );
}