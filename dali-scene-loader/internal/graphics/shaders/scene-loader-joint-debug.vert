#version 300 es

precision mediump float;
uniform mat4 uMvpMatrix;
in vec3 aPosition;
in float aColor;
flat out float vColor;

void main()
{
  vColor = aColor;
  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);
}