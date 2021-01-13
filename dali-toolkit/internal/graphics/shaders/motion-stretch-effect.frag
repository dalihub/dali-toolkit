precision mediump float;

uniform sampler2D sTexture;
uniform vec4 uColor;

uniform vec2 uObjectFadeStart;
uniform vec2 uObjectFadeEnd;
uniform float uAlphaScale;

// inputs
varying vec2 vModelSpaceCenterToPos;
varying vec2 vScreenSpaceVelocityVector;
varying float vSpeed;
varying vec2 vTexCoord;

void main()
{
  // calculate an alpha value that will fade the object towards its extremities, we need this to avoid an unsightly hard edge between color values of
  // the stretched object and the background. Use smoothstep also to hide any hard edges (discontinuities) in rate of change of this alpha gradient
  vec2 centerToPixel = abs( vModelSpaceCenterToPos );
  vec2 fadeToEdges = smoothstep(0.0, 1.0, 1.0 - ((centerToPixel - uObjectFadeStart) / (uObjectFadeEnd - uObjectFadeStart)));
  float fadeToEdgesScale = fadeToEdges.x * fadeToEdges.y * uAlphaScale; // apply global scaler
  fadeToEdgesScale = mix(1.0, fadeToEdgesScale, vSpeed);                // fade proportional to speed, so opaque when at rest

  // standard actor texel
  vec4 colActor = texture2D(sTexture, vTexCoord);
  gl_FragColor = colActor;
  gl_FragColor.a *= fadeToEdgesScale; // fade actor to its edges based on speed of motion
  gl_FragColor *= uColor;
}