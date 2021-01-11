#version 300 es

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

in vec3 aPosition;
in vec2 aTexCoord;

out vec2 vUV;

uniform vec3 uSize;

uniform mat4 uCubeMatrix;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform vec2 uTilt;

void main()
{
  vec4 vPosition = vec4( aPosition * uSize, 1.0);
  vec4 mPosition = uModelMatrix * vPosition;

  mPosition.xy += 0.3 * uTilt * mPosition.z;
  gl_Position = uProjection * uViewMatrix * mPosition;

#ifdef FLIP_V
  vUV = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
#else
  vUV = aTexCoord;
#endif
}
