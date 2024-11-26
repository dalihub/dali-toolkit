//@name video-view.vert

//@version 100

INPUT mediump vec2 aPosition;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
};

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
