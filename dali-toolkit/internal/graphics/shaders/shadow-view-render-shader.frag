//@name shadow-view-render-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uShadowColor;
};
UNIFORM sampler2D sTexture;

void main()
{
  lowp float alpha;
  alpha = TEXTURE(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;
  gl_FragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);
}