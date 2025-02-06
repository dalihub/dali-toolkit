precision highp float;
INPUT highp vec2 aPosition;
INPUT highp vec2 aDrift;
OUTPUT highp float vAlpha;

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform highp float borderSize;

void main()
{
  highp vec2 position = aPosition*(uSize.xy+vec2(0.75)) + aDrift*(borderSize+1.5);
  gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);
  vAlpha = min( abs(aDrift.x), abs(aDrift.y) )*(borderSize+1.5);
}