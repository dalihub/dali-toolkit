uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
varying mediump vec2 vTexCoord;

void main()
{
  gl_FragColor = texture2D( sTexture, vec2( vTexCoord.y, 0.5 ) ) * vec4(mixColor, 1.0) * uColor;
}
