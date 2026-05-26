//@name border-visual-anti-aliasing-shader.frag

//@version 100

precision highp float;

INPUT highp float vAlpha;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 borderColor;
  UNIFORM highp float borderSize;
};

void main()
{
  // Note that border color didn't pre-multiplied alpha. Apply alpha here
  gl_FragColor = vec4(borderColor.r * borderColor.a, borderColor.g * borderColor.a, borderColor.b * borderColor.a, borderColor.a)  * uColor;
  gl_FragColor *= smoothstep(0.0, 1.5, vAlpha) * smoothstep( borderSize + 1.5, borderSize, vAlpha );
}