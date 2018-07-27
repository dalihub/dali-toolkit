#version 430

layout( location = 0 ) in vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform vertData
{
 mat4 uMvpMatrix;
 vec3 uSize;
 vec4 uTextureRect;
 vec2 uTopLeft;
 vec2 uBottomRight;
};

layout( location = 0 ) out vec2 vTexCoord;

void main()
{
  vec4 position = vec4(aPosition, 0.0, 1.0);
  position.xyz *= uSize;
  gl_Position = uMvpMatrix * position;

// The line below is doing the same as the following commented lines:
//  vec2 imageSize = uTextureRect.zw - uTextureRect.xy;
//  vec2 topLeft = uTextureRect.xy + uTopLeft * imageSize;
//  vec2 bottomRight = uTextureRect.xy + uBottomRight * imageSize;
//  vec2 texCoord = (aTexCoord - uTextureRect.xy) / imageSize;
//  vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );

  vec2 texCoord = aPosition + vec2(0.5);
  vTexCoord = uTextureRect.xy + uTopLeft * ( uTextureRect.zw - uTextureRect.xy ) + ( texCoord - uTextureRect.xy ) * ( uBottomRight - uTopLeft );
}
