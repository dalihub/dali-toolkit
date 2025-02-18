//@name text-atlas-shader.vert

//@version 100

precision highp float;

INPUT   highp vec2    aPosition;
INPUT highp vec2    aTexCoord;
INPUT highp vec4    aColor;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp vec2    uOffset;
  UNIFORM highp mat4    uMvpMatrix;
};
OUTPUT   highp vec2    vTexCoord;
OUTPUT   highp vec4    vColor;

void main()
{
  highp vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}
