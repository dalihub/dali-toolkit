//@name animated-gradient-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp vec2 start_point;
  UNIFORM highp vec2 end_point;
  UNIFORM highp vec2 rotate_center;
  UNIFORM highp float rotate_angle;
};

OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec2 vStart;
OUTPUT highp vec2 vEnd;

vec2 rotate(vec2 x, vec2 c, float a)
{
  vec2 d = x - c;
  vec2 r = vec2(d.x * cos(a) - d.y * sin(a), d.x * sin(a) + d.y * cos(a));

#ifdef UNIT_TYPE_BOUNDING_BOX
  return r + c;
#endif

  /* UnitType::OBJECT_BOUNDING_BOX */
#ifdef UNIT_TYPE_USER
  return (r + c) / uSize.x;
#endif
  /* UnitType::USER_SPACE*/
}

UNIFORM_BLOCK VisualVertBlock
{
  //Visual size and offset
  UNIFORM highp vec2 offset;
  UNIFORM highp vec2 size;
  UNIFORM highp vec2 extraSize;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 anchorPoint;
};

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy );
  return vec4( (aPosition + anchorPoint)*visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );
}

void main()
{
  vStart = rotate( start_point, rotate_center, rotate_angle );
  vEnd = rotate( end_point, rotate_center, rotate_angle );
  gl_Position = uMvpMatrix * ComputeVertexPosition();

#ifdef UNIT_TYPE_BOUNDING_BOX
  vTexCoord = vec2(aPosition.x, -aPosition.y);
#endif
/* UnitType::OBJECT_BOUNDING_BOX */

#ifdef UNIT_TYPE_USER
  vTexCoord = vec2(aPosition.x, -aPosition.y * uSize.y / uSize.x);
#endif
/* UnitType::USER_SPACE*/
}
