precision highp float;
attribute highp vec2 aPosition;
attribute highp vec2 aTexCoord;
uniform highp vec3 uSize;
uniform highp mat4 uMvpMatrix;
varying highp vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy,0.0,1.0);
  vTexCoord = aTexCoord;
}
