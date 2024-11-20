//@name render-effect.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;

OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec2 vOptRectSize;
OUTPUT highp vec4 vCornerRadius; //output

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec4 uCornerRadius;//input
  UNIFORM lowp float uCornerRadiusPolicy;
};

UNIFORM_BLOCK SharedBlock
{
UNIFORM highp vec3 uSize;
};

void main()
{
  highp vec4 vertexPosition = vec4(aPosition * uSize.xy, 0.0, 1.0);
  vTexCoord = aPosition + vec2(0.5);
  gl_Position = uMvpMatrix * vertexPosition;

  highp float minSize = min(uSize.x, uSize.y);
  vCornerRadius = mix(uCornerRadius * minSize, uCornerRadius, uCornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);

  vOptRectSize = uSize.xy * 0.5;

  // Optimize fragment shader. 0.2929 ~= 1.0 - sqrt(0.5)
  highp float maxRadius = max(max(vCornerRadius.x, vCornerRadius.y), max(vCornerRadius.z, vCornerRadius.w));
  vOptRectSize -= 0.2929 * maxRadius + min(1.0, maxRadius);
}
