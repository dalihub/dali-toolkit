//@name primitive-visual-shader.frag

//@version 100

//Very simple fragment shader that merely applies the vertex shading to the color at each fragment.

precision mediump float;
INPUT mediump vec3 vIllumination;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );
}
