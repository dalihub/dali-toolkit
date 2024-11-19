attribute mediump vec2 aPosition;
attribute mediump vec2 aTexCoord;
uniform mediump vec3 uSize;
uniform mediump mat4 uMvpMatrix;
varying mediump vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy,0.0,1.0);
  vTexCoord = aTexCoord;
}
