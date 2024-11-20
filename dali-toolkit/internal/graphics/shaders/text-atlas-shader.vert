//@name text-atlas-shader.vert

//@version 100

INPUT   highp vec2    aPosition;
INPUT mediump vec2    aTexCoord;
INPUT mediump vec4    aColor;

UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump vec2    uOffset;
  UNIFORM highp mat4    uMvpMatrix;
};
OUTPUT   mediump vec2    vTexCoord;
OUTPUT   mediump vec4    vColor;

void main()
{
  highp vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}
