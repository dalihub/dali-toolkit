varying mediump vec2 vTexCoord;
uniform sampler2D sTexture;
uniform lowp vec4 uTextColorAnimatable;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  mediump float textTexture = texture2D( sTexture, vTexCoord ).r;

  // Set the color of the text to what it is animated to.
  gl_FragColor = uTextColorAnimatable * textTexture * uColor * vec4( mixColor, 1.0 );
}
