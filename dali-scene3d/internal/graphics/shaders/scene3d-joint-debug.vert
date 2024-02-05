
precision mediump float;
uniform mat4 uMvpMatrix;
INPUT vec3 aPosition;
INPUT float aColor;
flat OUTPUT float vColor;

void main()
{
  vColor = aColor;
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);
}