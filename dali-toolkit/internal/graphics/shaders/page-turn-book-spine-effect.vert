precision mediump float;
attribute mediump vec2 aPosition;
uniform mediump mat4 uMvpMatrix;
uniform vec3 uSize;
uniform float uTextureWidth;
varying vec2 vTexCoord;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition*uSize.xy, 0.0, 1.0);
  gl_Position = uMvpMatrix * vertexPosition;
  vTexCoord = aPosition + vec2(0.5);
  vTexCoord.x /= uTextureWidth;
}
