precision highp float;
uniform lowp  vec4      uColor;
uniform lowp  vec4      textColorAnimatable;
uniform       sampler2D sTexture;
varying highp vec2      vTexCoord;
varying highp vec4      vColor;

void main()
{
  mediump vec4 color = texture2D( sTexture, vTexCoord );
  gl_FragColor = vec4( vColor.rgb * uColor.rgb * textColorAnimatable.rgb, uColor.a * vColor.a * textColorAnimatable.a * color.r );
}
