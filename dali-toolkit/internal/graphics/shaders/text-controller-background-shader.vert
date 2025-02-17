//@name text-controller-background-shader.vert

//@version 100

precision highp float;

INPUT highp vec2    aPosition;
INPUT highp vec4    aColor;
OUTPUT   highp vec4    vColor;
UNIFORM_BLOCK VertBlock
{
  UNIFORM   highp mat4      uMvpMatrix;
};

void main()
{
  highp vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vColor = aColor;
}
