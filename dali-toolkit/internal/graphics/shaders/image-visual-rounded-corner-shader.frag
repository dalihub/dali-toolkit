INPUT mediump vec2 vTexCoord;
INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump float vCornerRadius;

uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  mediump vec2 diff = abs( vPosition ) - vRectSize;
  mediump float dist = length( max( diff, vec2( 0.0 ) ) ) - vCornerRadius;
  mediump float opacity = 1.0;
  if( dist > 1.0 )
  {
    opacity = 0.0;
  }
  else if( dist > -1.0 )
  {
    if( min( diff.x, diff.y ) < 0.0 )
    {
      dist += min( diff.x, diff.y ) / vCornerRadius;
    }
    opacity = 1.0 - smoothstep( -1.0, 1.0, dist );
  }

  OUT_COLOR = TEXTURE( sTexture, vTexCoord ) * uColor * vec4( mixColor, 1.0 );
  OUT_COLOR.a *= opacity;
  OUT_COLOR.rgb *= mix( 1.0, opacity, preMultipliedAlpha );
}
