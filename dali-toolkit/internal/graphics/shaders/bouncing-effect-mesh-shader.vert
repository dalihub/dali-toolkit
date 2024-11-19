//@name bouncing-effect-mesh-shader.vert

//@version 100

// Modify the vertex position according to the bounce coefficient
INPUT mediump vec3    aPosition1;
INPUT mediump vec3    aPosition2;
UNIFORM_BLOCK VertBlock
{
  UNIFORM   mediump mat4    uMvpMatrix;
  UNIFORM   mediump vec3    uSize;
  UNIFORM   mediump float   uBounceCoefficient;
};

void main()
{
  gl_Position = uMvpMatrix * vec4(mix( aPosition1, aPosition2, abs(uBounceCoefficient) )*uSize, 1.0);
}
