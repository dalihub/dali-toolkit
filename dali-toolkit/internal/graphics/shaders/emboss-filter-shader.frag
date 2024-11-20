//@name emboss-filter-shader.frag

//@version 100

precision highp float;
INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM vec2 uTexScale;
  UNIFORM vec3 uCoefficient;
};

void main()
{
  vec4 color  = uCoefficient.x * TEXTURE( sTexture, vTexCoord + vec2(0.0, -uTexScale.y) );
  color += uCoefficient.y * TEXTURE( sTexture, vTexCoord );
  color += uCoefficient.z * TEXTURE( sTexture, vTexCoord + vec2(0.0, uTexScale.y) );
  gl_FragColor = color;
}