//@name bubble-effect.vert

//@version 100

precision highp float;

INPUT highp float aIndex;
INPUT highp vec2  aPosition;
INPUT highp vec2  aTexCoord;
OUTPUT highp vec2  vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4  uMvpMatrix;
// the gravity applied to the y direction
  UNIFORM mediump float uGravity;
// xy: the emit position of the bubble; zw: the destination of the bubble.
// The bubble is moving from (xy) to (zw plus the y drop influenced by gravity).
  UNIFORM vec4 uStartEndPosition[NUMBER_OF_BUBBLE];
// The undergoing percentage of the bubble movement. 0.0: start from emit position, 1.0: reach the destination
  UNIFORM float uPercentage[NUMBER_OF_BUBBLE];
  UNIFORM vec2 uInvertedMovementArea;
// The bubble number is restricted by the available UNIFORM num.
// To increase the displayed bubble, every uStartEndPosition and uPercentage UNIFORM is applied to a small bunch of bubbles (9 here)
// The offset defines the random offset between bubbles within the bunch.
  UNIFORM vec2 uOffset[9];
// This UNIFORM is used to change the bubble size during running time
  UNIFORM float uDynamicScale;
};
OUTPUT highp float vPercentage;
OUTPUT highp vec2  vEffectTexCoord;

void main()
{
  vec4 position = vec4( aPosition, 0.0, 1.0 );
  // The Z coordinate is used to record the bubble index within current mesh actor
  int index = int(aIndex);
  //for some i between 0 ~ NUMBER_OF_BUBBLE-1: i,i+NUMBER_OF_BUBBLE, i+NUMBER_OF_BUBBLE*2, ... (up to i+NUMBER_OF_BUBBLE*8) belongs to the same bunch.
  int groupIdx = index / NUMBER_OF_BUBBLE;
  // The bubbles within the same bunch applies the same uniforms uStartEndPosition[idx] & uPercentage[idx]
  int idx = index - groupIdx*NUMBER_OF_BUBBLE;
  float percentage = uPercentage[idx];
  // early out if uPercentage is (zero || one) setting position to zero (zero sized triangles)
  if( percentage <= 0.0 || percentage >= 1.0 )
  {
    gl_Position = vec4(0.0);
    return;
  }
  vec4 startAndEnd = uStartEndPosition[idx];
  // The final position is added up different offset for bubbles
  startAndEnd.zw += uOffset[groupIdx];

  // increase the bubble size from 0% to 100% during the first 1/5 of movement & apply the dynamic scale
  // the new xy value containes both the new scale and new bubble position
  position.xy *= uDynamicScale*min(percentage*5.0, 1.0);
  position.xy += mix(startAndEnd.xy, startAndEnd.zw, percentage);
  // The gravity is g*t*t on the y direction
  position.y += uGravity * pow(percentage, 2.0);
  gl_Position = uMvpMatrix * position;

  // Add multiple bubble shapes in the effect
  vTexCoord = aTexCoord;
  vPercentage = percentage;
  // Use the emit position color for the bubble
  vEffectTexCoord = startAndEnd.xy * uInvertedMovementArea + vec2(0.5);
}