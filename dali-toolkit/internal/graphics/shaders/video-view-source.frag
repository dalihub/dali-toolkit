//@name video-view-source.frag

//@version 100

precision highp float;

// Inputs from vertex shader
INPUT highp vec2 vPosition;
FLAT INPUT highp vec2 vRectSize;      // Added
FLAT INPUT highp vec2 vOptRectSize;
FLAT INPUT highp vec4 vCornerRadius;
FLAT INPUT highp float vAliasMargin;  // Added
INPUT highp vec2 vTexCoord;

// Textures
UNIFORM sampler2D sPreviousTexture;
UNIFORM sampler2D sCurrentTexture;

// Uniform blocks
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM highp vec4 cornerSquareness; // input for squircle effect
  UNIFORM highp float uInterpolationFactor; // 0.0 for previous frame, 1.0 for current frame
};

// Global values for rounded corner (from color-visual-shader.frag)
highp float gRadius = 0.0;
highp float gSquareness = 0.0; // For squircle effect

highp vec2 gFragmentPosition = vec2(0.0, 0.0);
highp float gCenterPosition = 0.0;
highp vec2 gDiff = vec2(0.0, 0.0);
highp float gPotential = 0.0;

highp float gPotentialRange = 0.0;
highp float gMaxOutlinePotential = 0.0;
highp float gMinOutlinePotential = 0.0;

// Helper functions (from color-visual-shader.frag)
void calculateCornerRadius(highp vec4 cornerRadius, highp vec2 position)
{
  gRadius =
  mix(
    mix(cornerRadius.x, cornerRadius.y, sign(position.x) * 0.5 + 0.5),
    mix(cornerRadius.w, cornerRadius.z, sign(position.x) * 0.5 + 0.5),
    sign(position.y) * 0.5 + 0.5
  );
  gSquareness = clamp(
  mix(
    mix(cornerSquareness.x, cornerSquareness.y, sign(vPosition.x) * 0.5 + 0.5),
    mix(cornerSquareness.w, cornerSquareness.z, sign(vPosition.x) * 0.5 + 0.5),
    sign(vPosition.y) * 0.5 + 0.5
  ), 0.0, 1.0);
}

void calculateFragmentPosition(highp vec2 position, highp vec2 halfSizeOfRect)
{
  gFragmentPosition = abs(position) - halfSizeOfRect;
}

void calculatePosition()
{
  gCenterPosition = -gRadius;
  gDiff = gFragmentPosition - gCenterPosition;
}

void calculatePotential()
{
  // If gSquareness is near 1.0, it make some numeric error. Let we avoid this situation by heuristic value.
  if(gSquareness > 0.99)
  {
    gPotential = max(gDiff.x, gDiff.y);
    return;
  }

  highp vec2 positiveDiff = max(gDiff, 0.0);
  positiveDiff *= positiveDiff;

  gPotential = sqrt(((positiveDiff.x + positiveDiff.y)
                     + sqrt(positiveDiff.x * positiveDiff.x
                            + positiveDiff.y * positiveDiff.y
                            + (2.0 - 4.0 * gSquareness * gSquareness) * positiveDiff.x * positiveDiff.y))
                    * 0.5)
               + min(0.0, max(gDiff.x, gDiff.y));
}

void setupMinMaxPotential(float heuristicFactor)
{
  gPotentialRange = vAliasMargin;
  gMaxOutlinePotential = gRadius + gPotentialRange;
  gMinOutlinePotential = gRadius - gPotentialRange;

  // reduce defect near edge of rounded corner.
  highp float heuristicEdgeCasePotential = clamp(-min(gDiff.x, gDiff.y) / max(1.0, gRadius), 0.0, gPotentialRange * heuristicFactor);
  gMaxOutlinePotential += heuristicEdgeCasePotential;
  gMinOutlinePotential += heuristicEdgeCasePotential;
}

void PreprocessPotential(highp vec4 cornerRadius, highp vec2 position, highp vec2 halfSizeOfRect, float heuristicFactor)
{
  calculateCornerRadius(cornerRadius, position);
  calculateFragmentPosition(position, halfSizeOfRect);
  calculatePosition();
  calculatePotential();
  setupMinMaxPotential(heuristicFactor);
}

mediump float calculateCornerOpacity()
{
  highp float potential = gPotential;
  mediump float opacity = 1.0;

  if(potential > gMaxOutlinePotential)
  {
    discard;
  }
  else if(potential > gMinOutlinePotential)
  {
    opacity = 1.0 - smoothstep(gMinOutlinePotential, gMaxOutlinePotential, potential);
  }
  return opacity;
}

void main()
{
  // Texture interpolation logic (from original video-view-source.frag)
  vec4 currentColor = TEXTURE(sCurrentTexture, vTexCoord);
  vec4 previousColor = TEXTURE(sPreviousTexture, vTexCoord);
  vec4 interpolatedColor = mix(previousColor, currentColor, uInterpolationFactor);
  lowp vec4 targetColor = vec4(interpolatedColor.rgb * uColor.rgb, interpolatedColor.a); // Apply uColor modulation

  // Corner radius logic (from color-visual-shader.frag)
  // skip most potential calculate for performance
  if(abs(vPosition.x) < vOptRectSize.x && abs(vPosition.y) < vOptRectSize.y)
  {
    gl_FragColor = targetColor;
  }
  else
  {
    calculateCornerRadius(vCornerRadius, vPosition);
    calculateFragmentPosition(vPosition, vRectSize);

    // skip rounded corner calculate for performance if fragment is far inside the corner
    if(gFragmentPosition.x + gFragmentPosition.y < -(gRadius + vAliasMargin) * 2.0)
    {
      gl_FragColor = targetColor;
    }
    else
    {
      calculatePosition();
      calculatePotential();
      setupMinMaxPotential(1.0); // Using a default heuristicFactor

      gl_FragColor = targetColor;

      mediump float opacity = calculateCornerOpacity();
      gl_FragColor.a *= opacity; // Apply opacity to the alpha channel
    }
  }
}
