//@name effects-view.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp vec3 effectOffset;
};

void main()
{
  highp vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition.xyz += effectOffset;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = aPosition + vec2(0.5);
  gl_Position = vertexPosition;
}
