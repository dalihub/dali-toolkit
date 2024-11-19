//@name page-turn-effect.frag

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

INPUT vec3 vNormal;
INPUT vec4 vPosition;

void main()
{
  // need to re-normalize the interpolated normal
  vec3 normal = normalize( vNormal );
  // display page content
  vec4 texel;
  // display back image of the page, flip the texture
  if(  dot(vPosition.xyz, normal) > 0.0 ) texel = TEXTURE( sTexture, vec2( 1.0 - vTexCoord.x, vTexCoord.y ) );
  // display front image of the page
  else texel = TEXTURE( sTexture, vTexCoord );

  // display book spine, a stripe of shadowed texture
  float pixelPos = vTexCoord.x * uSize.x;
  float spineShadowCoef = 1.0;
  if( pixelPos < uSpineShadowParameter.x )
  {
    float x = pixelPos - uSpineShadowParameter.x;
    float y = sqrt( uSpineShadowParameter.x*uSpineShadowParameter.x - x*x );
    spineShadowCoef = normalize( vec2( uSpineShadowParameter.y*x/uSpineShadowParameter.x, y ) ).y;
  }
  // calculate the lighting
  // set the ambient color as vec3(0.4);
  float lightColor = abs( normal.z ) * 0.6 + 0.4;
  gl_FragColor = vec4( ( spineShadowCoef * lightColor ) * texel.rgb , texel.a ) * uColor;
}
