precision highp float;
varying highp vec2 vTexCoord;
uniform lowp vec4 uShadowColor;
uniform sampler2D sTexture;

void main()
{
  lowp float alpha;
  alpha = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;
  gl_FragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);
}