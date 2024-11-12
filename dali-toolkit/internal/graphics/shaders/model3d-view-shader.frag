//@name model3d-view-shader.frag

//@version 100

precision mediump float;
INPUT mediump vec2 vTexCoord;
INPUT mediump vec3 vIllumination;
INPUT mediump float vSpecular;
UNIFORM sampler2D sDiffuse;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  vec4 texture = TEXTURE( sDiffuse, vTexCoord );
  gl_FragColor = vec4( vIllumination.rgb * texture.rgb * uColor.rgb + vSpecular * 0.3, texture.a * uColor.a);
}
