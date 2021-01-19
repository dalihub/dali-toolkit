uniform lowp vec4 uColor;
varying mediump vec2 vTexCoord;
uniform samplerExternalOES sTexture;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
}
