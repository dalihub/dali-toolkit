//@name text-atlas-rgba-shader.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4      uColor;
  UNIFORM lowp vec4      textColorAnimatable;
};
UNIFORM         sampler2D sTexture;
INPUT highp vec2      vTexCoord;

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor * textColorAnimatable;
}
