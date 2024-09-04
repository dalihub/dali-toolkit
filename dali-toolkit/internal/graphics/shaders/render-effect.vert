precision highp float;

attribute highp vec2 aPosition;

varying highp vec2 vTexCoord;
varying highp vec2 vOptRectSize;
varying highp vec4 vCornerRadius; //output

uniform highp mat4 uMvpMatrix;
uniform highp vec4 uCornerRadius; //input
uniform lowp float uCornerRadiusPolicy;
uniform highp vec3 uSize;

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
