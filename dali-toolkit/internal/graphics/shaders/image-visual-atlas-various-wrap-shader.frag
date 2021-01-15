INPUT mediump vec2 vTexCoord;

uniform sampler2D sTexture;
uniform mediump vec4 uAtlasRect;

// WrapMode -- 0: CLAMP; 1: REPEAT; 2: REFLECT;
uniform lowp vec2 wrapMode;

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;
mediump float wrapCoordinate( mediump vec2 range, mediump float coordinate, lowp float wrap )

{
  mediump float coord;
  if( wrap > 1.5 )\n // REFLECT
    coord = 1.0-abs(fract(coordinate*0.5)*2.0 - 1.0);
  else \n// warp == 0 or 1
    coord = mix(coordinate, fract( coordinate ), wrap);
  return clamp( mix(range.x, range.y, coord), range.x, range.y );
}

void main()
{
  mediump vec2 texCoord = vec2( wrapCoordinate( uAtlasRect.xz, vTexCoord.x, wrapMode.x ),
                                wrapCoordinate( uAtlasRect.yw, vTexCoord.y, wrapMode.y ) );
  OUT_COLOR = TEXTURE( sTexture, texCoord ) * uColor * vec4( mixColor, 1.0 );
}