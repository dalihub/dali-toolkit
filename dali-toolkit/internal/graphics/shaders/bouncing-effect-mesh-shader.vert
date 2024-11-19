// Modify the vertex position according to the bounce coefficient
attribute mediump vec3    aPosition1;
attribute mediump vec3    aPosition2;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
uniform   mediump float   uBounceCoefficient;

void main()
{
  gl_Position = uMvpMatrix * vec4(mix( aPosition1, aPosition2, abs(uBounceCoefficient) )*uSize, 1.0);
}
