precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform vec2 uTexScale;
uniform vec3 uCoefficient;

void main()
{
  vec4 color  = uCoefficient.x * texture2D( sTexture, vTexCoord + vec2(0.0, -uTexScale.y) );
  color += uCoefficient.y * texture2D( sTexture, vTexCoord );
  color += uCoefficient.z * texture2D( sTexture, vTexCoord + vec2(0.0, uTexScale.y) );
  gl_FragColor = color;
}