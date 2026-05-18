//@name border-visual-shader.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 borderColor;
};

void main()
{
  // Note that border color didn't pre-multiplied alpha. Apply alpha here
  gl_FragColor = vec4(borderColor.r * borderColor.a, borderColor.g * borderColor.a, borderColor.b * borderColor.a, borderColor.a)  * uColor;
}