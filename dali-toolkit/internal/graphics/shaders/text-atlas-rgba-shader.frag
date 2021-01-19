uniform lowp    vec4      uColor;
uniform lowp    vec4      textColorAnimatable;
uniform         sampler2D sTexture;
varying mediump vec2      vTexCoord;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * textColorAnimatable;
}
