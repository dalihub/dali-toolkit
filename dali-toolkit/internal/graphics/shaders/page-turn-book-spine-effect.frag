//@name page-turn-book-spine-effect.frag

//@version 100

precision mediump float;
INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM vec2 uSpineShadowParameter;
};

UNIFORM_BLOCK SharedBlock
{
UNIFORM vec3 uSize;
};

void main()
{
  if( gl_FrontFacing ) // display front side
  {
    gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor;
  }
  else // display back side, flip the image horizontally by changing the x component of the texture coordinate
  {
    gl_FragColor = TEXTURE( sTexture, vec2( 1.0 - vTexCoord.x, vTexCoord.y ) ) * uColor;
  }
  // display book spine, a stripe of shadowed texture
  float pixelPos = vTexCoord.x * uSize.x;
  if( pixelPos < uSpineShadowParameter.x )
  {
    float x = pixelPos - uSpineShadowParameter.x;
    float y = sqrt( uSpineShadowParameter.x*uSpineShadowParameter.x - x*x );
    vec2 spineNormal = normalize(vec2(uSpineShadowParameter.y*x/uSpineShadowParameter.x, y));
    gl_FragColor.rgb *= spineNormal.y;
  }
}