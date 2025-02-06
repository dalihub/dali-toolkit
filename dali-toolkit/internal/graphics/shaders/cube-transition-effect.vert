precision highp float;
attribute highp vec2 aPosition;
varying highp vec2 vTexCoord;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform highp vec4 uTextureRect;

void main()
{
  highp vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = aPosition + vec2(0.5);
  vTexCoord = mix(uTextureRect.xy, uTextureRect.zw, vTexCoord);

  gl_Position = vertexPosition;
}