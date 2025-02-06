precision highp float;
attribute highp vec2 aPosition;

uniform highp mat4 uMvpMatrix;
uniform vec3 uSize;
uniform vec4 uTextureRect;

varying vec2 vTexCoord;

uniform highp vec2 uTopLeft;
uniform highp vec2 uBottomRight;

void main()
{
  highp vec4 position = vec4(aPosition, 0.0, 1.0);
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