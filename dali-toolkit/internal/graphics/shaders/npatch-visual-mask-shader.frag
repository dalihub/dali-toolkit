varying mediump vec2 vTexCoord;
varying mediump vec2 vMaskTexCoord;
uniform sampler2D sTexture;
uniform sampler2D sMask;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;
uniform mediump float auxiliaryImageAlpha;

void main()
{
  // Where mask image is transparent, all of background image must show through.
  // where mask image is opaque, only mask should be shown
  // where mask is translucent, less of background should be shown.
  // auxiliaryImageAlpha controls how much of mask is visible

  mediump vec4 color = texture2D( sTexture, vTexCoord );
  mediump vec4 mask= texture2D( sMask, vMaskTexCoord );

  mediump vec3 mixedColor = color.rgb * mix( 1.0-mask.a, 1.0, 1.0-auxiliaryImageAlpha)
                            + mask.rgb*mask.a * auxiliaryImageAlpha;
  gl_FragColor = vec4(mixedColor,1.0) * uColor * vec4( mixColor, 1.0 );
}
