//@name dissolve-effect.vert

//@version 100

INPUT mediump vec2 aPosition;

UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump mat4   uMvpMatrix;
  UNIFORM vec3           uSize;

  UNIFORM float          uPercentage;
  UNIFORM vec3           uSaddleParam;
  UNIFORM vec2           uTranslation;
  UNIFORM vec2           uRotation;
  UNIFORM float          uToNext;
};



OUTPUT float          vPercentage;
OUTPUT vec2           vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position = vertexPosition;

  vec2 texCoord = aPosition + vec2(0.5);
  vTexCoord = texCoord;

  //Calculate the distortion value given the dissolve central line
  vec2 value = texCoord + uTranslation;
  mat2 rotateMatrix = mat2(uRotation.s, uRotation.t, -uRotation.t, uRotation.s);
  value = rotateMatrix * value;
  if(uToNext == 1.0)
    value.s = uSaddleParam[2] + value.s;
  float delay = value.t * value.t / uSaddleParam[0] - value.s * value.s / uSaddleParam[1];
  vPercentage = clamp(uPercentage * 2.0 - 0.5 * sin(delay * 1.571) - 0.5, 0.0, 1.0);
}
