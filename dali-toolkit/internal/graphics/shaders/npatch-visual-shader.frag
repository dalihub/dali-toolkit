varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * vec4( mixColor, 1.0 );
}
