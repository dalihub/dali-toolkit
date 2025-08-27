//@name video-view-source.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp vec3 uSize;
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uScale; // Added for vAliasMargin calculation
  UNIFORM highp vec4 cornerRadius; //input
  UNIFORM lowp float cornerRadiusPolicy; //input
};

// Outputs to fragment shader
OUTPUT highp vec2 vPosition;
FLAT OUTPUT highp vec2 vRectSize;      // Added
FLAT OUTPUT highp vec2 vOptRectSize;
FLAT OUTPUT highp vec4 vCornerRadius; //output
FLAT OUTPUT highp float vAliasMargin;  // Added
OUTPUT highp vec2 vTexCoord;

void main()
{
  // Adapted from color-visual-shader.vert's ComputeVertexPosition
  // For VideoView, visualSize is directly uSize.xy. Other parameters are effectively zero or default.
  highp vec2 visualSize = uSize.xy;

  // Corner radius calculations
  vRectSize = visualSize * 0.5;
  vOptRectSize = vRectSize; // Initial value for optimization

  // Set soft anti-alias range
  // The range should be inverse proportion with scale of view.
  // To avoid divid-by-zero, let we allow minimum scale value is 0.001 (0.1%)
  vAliasMargin = min(1.0, max(visualSize.x, visualSize.y) * 0.1) / max(0.001, max(uScale.x, uScale.y));

  highp float vertexMargin = 0.0;

  highp float minSize = min(visualSize.x, visualSize.y);
  vCornerRadius = mix(cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);

  // Optimize fragment shader. 0.2929 ~= 1.0 - sqrt(0.5)
  highp float maxRadius = max(max(vCornerRadius.x, vCornerRadius.y), max(vCornerRadius.z, vCornerRadius.w));
  highp float minRadius = min(min(vCornerRadius.x, vCornerRadius.y), min(vCornerRadius.z, vCornerRadius.w));
  vOptRectSize -= 0.2929 * maxRadius + 1.0;

  // Set vertex margin as vAliasMargin if we need to make some more fragments for alias.
  // Do not increase margin if the minRadius is small enough rather than maxSize.
  // TODO : We should change the magic parameter, 0.49
  // Using visualSize.x as a proxy for maxSize as in color-visual-shader.vert
  vertexMargin = 2.0 * vAliasMargin * smoothstep(visualSize.x * 0.49, visualSize.x * 0.5, minRadius);

  // Calculate final vertex position (assuming aPosition is -0.5 to 0.5)
  // This logic is for rounded corner only (no blur/borderline)
  // vPosition here is the non-centered position, similar to color-visual-shader.vert's vPosition
  vPosition = aPosition * (visualSize + vertexMargin);

  // Final gl_Position calculation.
  // The original video-view-source.vert did:
  // vertexPosition = vec4(aPosition, 0.0, 1.0);
  // vPosition = aPosition + vec2(0.5); // This vPosition is for texcoord
  // vertexPosition.xyz *= uSize; // This makes aPosition from -0.5..0.5 to -uSize/2..uSize/2
  // gl_Position = uMvpMatrix * vertexPosition;
  //
  // The new logic from color-visual-shader.vert for final position is:
  // vec4(vPosition + anchorPoint * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0)
  // For VideoView, anchorPoint, visualOffset, origin are effectively 0.
  // So, gl_Position = uMvpMatrix * vec4(vPosition, 0.0, 1.0);
  gl_Position = uMvpMatrix * vec4(vPosition, 0.0, 1.0);

  // Texture coordinate calculation (matches original video-view-source.vert)
  // aPosition is -0.5 to 0.5, vTexCoord should be 0.0 to 1.0
  vTexCoord = aPosition + vec2(0.5);
}
