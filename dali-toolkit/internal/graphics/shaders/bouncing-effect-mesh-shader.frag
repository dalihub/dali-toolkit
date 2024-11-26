//@name bouncing-effect-mesh-shader.frag

//@version 100

// Use the actor color to paint every layer
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  gl_FragColor = uColor;
}
