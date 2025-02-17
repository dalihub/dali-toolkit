//@name text-decorator-shader.vert

//@version 100

precision highp float;

INPUT highp vec2    aPosition;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4      uMvpMatrix;
};
void main()
{
  highp vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
}
