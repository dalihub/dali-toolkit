//@name motion-blur-effect.frag

//@version 100

precision mediump float;

UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM vec4 uColor;

  UNIFORM vec2 uObjectFadeStart;
  UNIFORM vec2 uObjectFadeEnd;
  UNIFORM float uAlphaScale;
  UNIFORM float uBlurTexCoordScale;
  UNIFORM float uNumSamples;
  UNIFORM float uRecipNumSamples;
  UNIFORM float uRecipNumSamplesMinusOne;
};

// inputs
INPUT vec2 vModelSpaceCenterToPos;
INPUT vec2 vScreenSpaceVelocityVector;
INPUT float vSpeed;
INPUT vec2 vTexCoord;

void main()
{
  // calculate an alpha value that will fade the object towards its extremities, we need this to avoid an unsightly hard edge between color values of
  // the blurred object and the unblurred background. Use smoothstep also to hide any hard edges (discontinuities) in rate of change of this alpha gradient
  vec2 centerToPixel = abs(vModelSpaceCenterToPos);
  vec2 fadeToEdges = smoothstep(0.0, 1.0, 1.0 - ((centerToPixel - uObjectFadeStart) / (uObjectFadeEnd - uObjectFadeStart)));
  float fadeToEdgesScale = fadeToEdges.x * fadeToEdges.y * uAlphaScale; // apply global scaler
  fadeToEdgesScale = mix(1.0, fadeToEdgesScale, vSpeed);// fade proportional to speed, so opaque when at rest

  // scale velocity vector by user requirements
  vec2 velocity = vScreenSpaceVelocityVector * uBlurTexCoordScale;

  // standard actor texel
  vec4 colActor = TEXTURE(sTexture, vTexCoord);

  // blurred actor - gather texture samples from the actor texture in the direction of motion
  vec4 col = colActor * uRecipNumSamples;
  for(float i = 1.0; i < uNumSamples; i += 1.0)
  {
    float t = i * uRecipNumSamplesMinusOne;
    col += TEXTURE(sTexture, vTexCoord + (velocity * t)) * uRecipNumSamples;
  }
  gl_FragColor = mix(colActor, col, vSpeed); // lerp blurred and non-blurred actor based on speed of motion
  gl_FragColor.a = fadeToEdgesScale;//colActor.a * fadeToEdgesScale; // fade blurred actor to its edges based on speed of motion
  gl_FragColor *= uColor;
}
