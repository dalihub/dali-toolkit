//@name bubble-effect.frag

//@version 100

INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sBackground;
UNIFORM sampler2D sBubbleShape;
INPUT mediump float vPercentage;
INPUT mediump vec2 vEffectTexCoord;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  // Get the emit pisition color, and Mix with the actor color
  mediump vec4 fragColor = TEXTURE(sBackground, vEffectTexCoord)*uColor;

  // Apply the shape defined by the texture contained sBubbleShape
  // And make the opacity being 0.7, and animate from 0.7 to 0 during the last 1/3 of movement
  fragColor.a*= TEXTURE(sBubbleShape, vTexCoord).a * ( 2.1 - max( vPercentage*2.1, 1.4 ) );

  gl_FragColor = fragColor;
}