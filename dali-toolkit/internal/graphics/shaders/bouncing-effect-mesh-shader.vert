//@name bouncing-effect-mesh-shader.vert

//@version 100

precision highp float;

// Modify the vertex position according to the bounce coefficient
INPUT highp vec3    aPosition1;
INPUT highp vec3    aPosition2;
UNIFORM_BLOCK VertBlock
{
  UNIFORM   highp mat4    uMvpMatrix;
  UNIFORM   highp vec3    uSize;
  UNIFORM   highp float   uBounceCoefficient;
};

void main()
{
  gl_Position = uMvpMatrix * vec4(mix( aPosition1, aPosition2, abs(uBounceCoefficient) )*uSize, 1.0);
}
