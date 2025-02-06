precision highp float;
attribute highp vec2 aPosition;
attribute highp vec2 aTexCoord;
attribute highp vec4 aColor;
uniform   highp vec2 uOffset;
uniform   highp mat4 uMvpMatrix;
varying   highp vec2 vTexCoord;
varying   highp vec4 vColor;

void main()
{
  highp vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}
