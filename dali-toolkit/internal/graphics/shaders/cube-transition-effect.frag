varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform lowp vec4 uSamplerRect;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
}
