//@name text-controller-background-shader.vert

//@version 100

INPUT mediump vec2    aPosition;
INPUT mediump vec4    aColor;
OUTPUT   mediump vec4    vColor;
UNIFORM_BLOCK VertBlock
{
  UNIFORM   highp mat4      uMvpMatrix;
};

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vColor = aColor;
}
