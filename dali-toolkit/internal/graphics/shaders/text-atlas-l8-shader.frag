//@name text-atlas-l8-shader.frag

//@version 100

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp    vec4      uColor;
  UNIFORM lowp    vec4      textColorAnimatable;
};
UNIFORM         sampler2D sTexture;
INPUT mediump vec2      vTexCoord;
INPUT mediump vec4      vColor;

void main()
{
  mediump vec4 color = TEXTURE( sTexture, vTexCoord );
  gl_FragColor = vec4( vColor.rgb * uColor.rgb * textColorAnimatable.rgb, uColor.a * vColor.a * textColorAnimatable.a * color.r );
}
