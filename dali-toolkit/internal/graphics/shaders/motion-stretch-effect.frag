//@name motion-stretch-effect.frag

//@version 100

precision mediump float;

UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM vec4 uColor;

  UNIFORM vec2 uObjectFadeStart;
  UNIFORM vec2 uObjectFadeEnd;
  UNIFORM float uAlphaScale; // Global alpha scale
};

// inputs
INPUT vec2 vModelSpaceCenterToPos; // 2d, ignoring z
INPUT vec2 vScreenSpaceVelocityVector;
INPUT float vSpeed;
INPUT vec2 vTexCoord;

void main()
{
  // calculate an alpha value that will fade the object towards its extremities, we need this to avoid an unsightly hard edge between color values of
  // the stretched object and the background. Use smoothstep also to hide any hard edges (discontinuities) in rate of change of this alpha gradient
  vec2 centerToPixel = abs( vModelSpaceCenterToPos );
  vec2 fadeToEdges = smoothstep(0.0, 1.0, 1.0 - ((centerToPixel - uObjectFadeStart) / (uObjectFadeEnd - uObjectFadeStart)));
  float fadeToEdgesScale = fadeToEdges.x * fadeToEdges.y * uAlphaScale; // apply global scaler
  fadeToEdgesScale = mix(1.0, fadeToEdgesScale, vSpeed);                // fade proportional to speed, so opaque when at rest

  // standard actor texel
  vec4 colActor = TEXTURE(sTexture, vTexCoord);
  gl_FragColor = colActor;
  gl_FragColor.a *= fadeToEdgesScale; // fade actor to its edges based on speed of motion
  gl_FragColor *= uColor;
}