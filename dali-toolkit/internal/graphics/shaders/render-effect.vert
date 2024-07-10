precision highp float;
attribute highp vec2 aPosition;
varying highp vec2 vFragCoord;
varying highp vec2 vTexCoord;
varying highp vec4 vCornerRadius; //output
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform highp vec4 uCornerRadius; //input
uniform highp float uCornerRadiusPolicy;

void main()
{
  highp vec4 vertexPosition = vec4(aPosition * uSize.xy, 0.0, 1.0);
  vFragCoord = vertexPosition.xy + uSize.xy/2.0;
  vTexCoord = aPosition + vec2(0.5);
  gl_Position = uMvpMatrix * vertexPosition;

  highp float minSize = min(uSize.x, uSize.y);
  vCornerRadius = mix(uCornerRadius * minSize, uCornerRadius, uCornerRadiusPolicy);
  vCornerRadius = min(vCornerRadius, minSize * 0.5);
}
