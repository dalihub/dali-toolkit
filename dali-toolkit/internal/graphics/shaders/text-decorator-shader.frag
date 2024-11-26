//@name text-decorator-shader.frag

//@version 100

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};
void main()
{
  gl_FragColor = uColor;
}
