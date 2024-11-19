//@name image-region-effect.vert

//@version 100

INPUT mediump vec2 aPosition;

OUTPUT vec2 vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump vec2 uTopLeft;
  UNIFORM mediump vec2 uBottomRight;
  UNIFORM mediump mat4 uMvpMatrix;
  UNIFORM vec3 uSize;
  UNIFORM vec4 uTextureRect;
};

void main()
{
  mediump vec4 position = vec4(aPosition, 0.0, 1.0);
  position.xyz *= uSize;
  gl_Position = uMvpMatrix * position;

    // The line below is doing the same as the following commented lines:
    //
    // vec2 imageSize = uTextureRect.zw - uTextureRect.xy;
    // vec2 topLeft = uTextureRect.xy + uTopLeft * imageSize;
    // vec2 bottomRight = uTextureRect.xy + uBottomRight * imageSize;
    // vec2 texCoord = (aTexCoord - uTextureRect.xy) / imageSize;
    // vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );

  vec2 texCoord = aPosition + vec2(0.5);
  vTexCoord = uTextureRect.xy + uTopLeft * ( uTextureRect.zw - uTextureRect.xy ) + ( texCoord - uTextureRect.xy ) * ( uBottomRight - uTopLeft );
}