INPUT mediump vec2 vTexCoord;

uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  OUT_COLOR = TEXTURE( sTexture, vTexCoord ) * uColor * vec4( mixColor, 1.0 );
}
