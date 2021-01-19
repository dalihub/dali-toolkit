precision mediump float;

uniform sampler2D sTexture;
uniform vec4 uColor;

uniform vec2 uObjectFadeStart;
uniform vec2 uObjectFadeEnd;
uniform float uAlphaScale;
uniform float uBlurTexCoordScale;
uniform float uNumSamples;
uniform float uRecipNumSamples;
uniform float uRecipNumSamplesMinusOne;

// inputs
varying vec2 vModelSpaceCenterToPos;
varying vec2 vScreenSpaceVelocityVector;
varying float vSpeed;
varying vec2 vTexCoord;

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
  vec4 colActor = texture2D(sTexture, vTexCoord);

  // blurred actor - gather texture samples from the actor texture in the direction of motion
  vec4 col = colActor * uRecipNumSamples;
  for(float i = 1.0; i < uNumSamples; i += 1.0)
  {
    float t = i * uRecipNumSamplesMinusOne;
    col += texture2D(sTexture, vTexCoord + (velocity * t)) * uRecipNumSamples;
  }
  gl_FragColor = mix(colActor, col, vSpeed); // lerp blurred and non-blurred actor based on speed of motion
  gl_FragColor.a = fadeToEdgesScale;//colActor.a * fadeToEdgesScale; // fade blurred actor to its edges based on speed of motion
  gl_FragColor *= uColor;
}
