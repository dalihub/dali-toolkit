attribute mediump vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

uniform mediump vec2 start_point;
uniform mediump vec2 end_point;
uniform mediump vec2 rotate_center;
uniform mediump float rotate_angle;

varying mediump vec2 vTexCoord;
varying mediump vec2 vStart;
varying mediump vec2 vEnd;

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

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
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
