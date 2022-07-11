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

  mediump vec4 color = texture2D(sTexture, vTexCoord);
  mediump vec4 mask = texture2D(sMask, vMaskTexCoord);

  mediump float maskAlpha = mask.a * auxiliaryImageAlpha;

  lowp vec3 preMultipliedMaskRGB = mask.rgb * mix(mask.a, 1.0, preMultipliedAlpha) * auxiliaryImageAlpha;
  lowp vec3 preMultipliedTextureRGB = color.rgb * mix(color.a, 1.0, preMultipliedAlpha);

  // Manual blend operation with premultiplied colors.
  // Final alpha = maskAlpha + (1.0 - maskAlpha) * color.a.
  // (Final rgb * alpha) =  preMultipliedMaskRGB + (1.0 - maskAlpha) * preMultipliedTextureRGB
  // If preMultipliedAlpha == 1.0, just return vec4(rgb*alpha, alpha)
  // Else, return vec4((rgb*alpha) / alpha, alpha)

  lowp float finalAlpha = mix(color.a, 1.0, maskAlpha);
  lowp vec3  finalMultipliedRGB = preMultipliedMaskRGB + (1.0 - maskAlpha) * preMultipliedTextureRGB;

  // TODO : Need to find some way without division
  lowp vec4 finalColor = vec4(finalMultipliedRGB * mix(1.0 / finalAlpha, 1.0, preMultipliedAlpha), finalAlpha);
  gl_FragColor = finalColor * uColor * vec4(mixColor, 1.0);
}
