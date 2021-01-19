INPUT mediump vec2 vTexCoord;

uniform sampler2D sTexture;
uniform mediump vec4 uAtlasRect;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
  OUT_COLOR = TEXTURE( sTexture, texCoord ) * uColor * vec4( mixColor, 1.0 );
}