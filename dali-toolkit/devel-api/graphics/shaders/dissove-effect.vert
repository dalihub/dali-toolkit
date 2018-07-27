#version 430

layout( location = 0 ) in vec2 aPosition;

layout( location = 0 ) out float vPercentage;
layout( location = 1 ) out vec2 vTexCoord;

layout( set = 0, binding = 0, std140 ) uniform vertData
{
   mat4 uMvpMatrix;
   vec3 uSize;
   vec4 uTextureRect;

   float uPercentage;
   vec3 uSaddleParam;
   vec2 uTranslation;
   vec2 uRotation;
   float uToNext;
};

void main()
{
  vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;
  gl_Position = vertexPosition;

  vec2 texCoord = aPosition + vec2(0.5);
  vTexCoord = texCoord;

  //Calculate the distortion value given the dissolve central line
  vec2 value = texCoord + uTranslation;
  mat2 rotateMatrix = mat2( uRotation.s, uRotation.t, -uRotation.t, uRotation.s );
  value = rotateMatrix * value;

  if(uToNext == 1.0)
    value.s = uSaddleParam[2] + value.s;

  float delay = value.t*value.t / uSaddleParam[0] - value.s*value.s/uSaddleParam[1];
  vPercentage = clamp( uPercentage*2.0 - 0.5*sin(delay*1.571) - 0.5, 0.0, 1.0 );
}
