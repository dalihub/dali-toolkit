//@name border-visual-shader.frag

//@version 100

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 borderColor;
};

void main()
{
  gl_FragColor = borderColor * uColor;
}