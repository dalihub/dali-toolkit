varying mediump vec2 vTexCoord;
uniform lowp vec4 uColor;
uniform sampler2D sBackground;
uniform sampler2D sBubbleShape;
varying mediump float vPercentage;
varying mediump vec2 vEffectTexCoord;

void main()
{
  // Get the emit pisition color, and Mix with the actor color
  mediump vec4 fragColor = texture2D(sBackground, vEffectTexCoord)*uColor;

  // Apply the shape defined by the texture contained sBubbleShape
  // And make the opacity being 0.7, and animate from 0.7 to 0 during the last 1/3 of movement
  fragColor.a*= texture2D(sBubbleShape, vTexCoord).a * ( 2.1 - max( vPercentage*2.1, 1.4 ) );

  gl_FragColor = fragColor;
}