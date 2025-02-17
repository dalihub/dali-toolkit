//@name scene3d-joint-debug.vert

//@version 100

precision highp float;

UNIFORM_BLOCK VertBlock
{
  UNIFORM mat4 uMvpMatrix;
};
INPUT vec3 aPosition;
INPUT float aColor;
FLAT OUTPUT float vColor;

void main()
{
  vColor = aColor;
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);
}