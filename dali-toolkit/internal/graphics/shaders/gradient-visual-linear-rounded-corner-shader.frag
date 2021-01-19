uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
varying mediump vec2 vTexCoord;
varying mediump vec2 vPosition;
varying mediump vec2 vRectSize;
varying mediump float vCornerRadius;

void main()
{
  mediump float dist = length( max( abs( vPosition ), vRectSize ) - vRectSize ) - vCornerRadius;
  gl_FragColor = texture2D( sTexture, vec2( vTexCoord.y, 0.5 ) ) * vec4(mixColor, 1.0) * uColor;
  gl_FragColor *= 1.0 - smoothstep( -1.0, 1.0, dist );
}
