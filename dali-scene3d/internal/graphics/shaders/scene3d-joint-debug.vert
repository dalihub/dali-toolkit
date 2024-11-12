//@version 100

precision mediump float;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mat4 uMvpMatrix;
};
INPUT vec3 aPosition;
INPUT float aColor;
OUTPUT flat float vColor;

void main()
{
  vColor = aColor;
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);
}