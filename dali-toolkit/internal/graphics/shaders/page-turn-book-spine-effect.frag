precision mediump float;
varying mediump vec2 vTexCoord;
uniform vec3 uSize;
uniform vec2 uSpineShadowParameter;
uniform sampler2D sTexture;
uniform lowp vec4 uColor;

void main()
{
  if( gl_FrontFacing ) // display front side
  {
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
  }
  else // display back side, flip the image horizontally by changing the x component of the texture coordinate
  {
    gl_FragColor = texture2D( sTexture, vec2( 1.0 - vTexCoord.x, vTexCoord.y ) ) * uColor;
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