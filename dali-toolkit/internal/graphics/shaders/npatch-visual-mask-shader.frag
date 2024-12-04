//@name npatch-visual-mask-shader.frag

//@version 100

INPUT mediump vec2 vTexCoord;
INPUT mediump vec2 vMaskTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM sampler2D sMask;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp float premultipliedAlpha;
  UNIFORM mediump float auxiliaryImageAlpha;
};

void main()
{
  // Where mask image is transparent, all of background image must show through.
  // where mask image is opaque, only mask should be shown
  // where mask is translucent, less of background should be shown.
  // auxiliaryImageAlpha controls how much of mask is visible

  mediump vec4 color = TEXTURE(sTexture, vTexCoord);
  mediump vec4 mask = TEXTURE(sMask, vMaskTexCoord);

  mediump float maskAlpha = mask.a * auxiliaryImageAlpha;

  lowp vec3 preMultipliedMaskRGB = mask.rgb * mix(mask.a, 1.0, premultipliedAlpha) * auxiliaryImageAlpha;
  lowp vec3 preMultipliedTextureRGB = color.rgb * mix(color.a, 1.0, premultipliedAlpha);

  // Manual blend operation with premultiplied colors.
  // Final alpha = maskAlpha + (1.0 - maskAlpha) * color.a.
  // (Final rgb * alpha) =  preMultipliedMaskRGB + (1.0 - maskAlpha) * preMultipliedTextureRGB
  // If premultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
  // Else, return vec4((rgb*alpha) / alpha, alpha)

  lowp float finalAlpha = mix(color.a, 1.0, maskAlpha);
  lowp vec3  finalMultipliedRGB = preMultipliedMaskRGB + (1.0 - maskAlpha) * preMultipliedTextureRGB;

  // TODO : Need to find some way without division
  lowp vec4 finalColor = vec4(finalMultipliedRGB * mix(1.0 / finalAlpha, 1.0, premultipliedAlpha), finalAlpha);
  gl_FragColor = finalColor * uColor;
}
