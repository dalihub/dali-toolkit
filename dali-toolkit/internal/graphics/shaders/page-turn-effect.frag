precision mediump float;

varying mediump vec2 vTexCoord;

uniform sampler2D sTexture;
uniform lowp vec4 uColor;
uniform vec3 uSize;
uniform vec2 uSpineShadowParameter;
varying vec3 vNormal;
varying vec4 vPosition;

void main()
{
  // need to re-normalize the interpolated normal
  vec3 normal = normalize( vNormal );
  // display page content
  vec4 texel;
  // display back image of the page, flip the texture
  if(  dot(vPosition.xyz, normal) > 0.0 ) texel = texture2D( sTexture, vec2( 1.0 - vTexCoord.x, vTexCoord.y ) );
  // display front image of the page
  else texel = texture2D( sTexture, vTexCoord );

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
