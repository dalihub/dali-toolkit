precision highp float;
// Modify the vertex position according to the bounce coefficient
attribute highp vec3    aPosition1;
attribute highp vec3    aPosition2;
uniform   highp mat4    uMvpMatrix;
uniform   highp vec3    uSize;
uniform   highp float   uBounceCoefficient;

void main()
{
  gl_Position = uMvpMatrix * vec4(mix( aPosition1, aPosition2, abs(uBounceCoefficient) )*uSize, 1.0);
}
