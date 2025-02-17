//@name page-turn-effect.vert

//@version 100

/*
 * The common parameters for all the vertices, calculate in CPU then pass into the shader as uniforms
 *
 * first part of the page, (outside the the line passing through original center and vertical to curve direction)
 * no Z change, only 2D rotation and translation
 * ([0][0],[0][1],[1][0],[1][1]) mat2 rotateMatrix
 * ([2][0],[2][1]) vec2 translationVector
 *
 * ([0][2],[0][3]) vec2 originalCenter: Typically the press down position of the Pan Gesture
 * ([1][2],[1][3]) vec2 currentCenter: Typically the current position of the Pan Gesture
 * ([3][0],[3][1]) vec2 curveDirection: The normalized vector pointing from original center to current center
 * ([2][2]) float vanishingPointY: The Y coordinate of the intersection of the spine
 *   and the line which goes through the original center and is vertical to the curveDirection
 * ([2][3]) float curveEndY: The Y coordinate of intersection of the spine and the line through both original and current center
 * ([3][2]) float curveHeight: The height of the interpolated hermite curve.
 * ([3][3]) float currentLength: The length from the current center to the curveEnd.
 */

precision highp float;

INPUT highp vec2 aPosition;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp mat3 uNormalMatrix;
  UNIFORM highp mat4 uModelView;

  UNIFORM highp mat4 uCommonParameters;

  UNIFORM float uIsTurningBack;
  UNIFORM highp float uTextureWidth;
};

UNIFORM_BLOCK SharedBlock
{
  UNIFORM highp vec3 uSize;
};

OUTPUT highp vec3 vNormal;
OUTPUT highp vec4 vPosition;
OUTPUT highp vec2 vTexCoord;

void main()
{
  vec4 position = vec4( aPosition*uSize.xy, 0.0, 1.0);
  vec2 currentCenter = vec2( uCommonParameters[1][2], uCommonParameters[1][3]);
  vec2 originalCenter = vec2( uCommonParameters[0][2], uCommonParameters[0][3]);
  vec3 normal = vec3(0.0,0.0,1.0);

  if(currentCenter.x < originalCenter.x)
  {
    // change the coordinate origin from the center of the page to its top-left
    position.xy += uSize.xy * 0.5;
    vec2 curveDirection = vec2( uCommonParameters[3]);
    vec3 vanishingPoint = vec3(0.0, uCommonParameters[2][2], 0.0);
    // first part of the page, (outside the the line passing through original center and vertical to curve direction)
    //no Z change, only 2D rotation and translation
    if( dot(curveDirection, position.xy - originalCenter) < 0.0 )
    {
      position.y -= vanishingPoint.y;
      position.xy = mat2(uCommonParameters)*position.xy + vec2( uCommonParameters[2]);
    }
    // second part of the page, bent as a ruled surface
    else
    {
      // calculate on the flat plane, between
      // the first line passing through current vertex and vanishing point
      // the second line passing through original center and current center
      vec2 curveEnd = vec2( 0.0, uCommonParameters[2][3] );
      vec2 curFlatDirection = vec2(0.0,1.0);
      float lengthFromCurve = position.y - originalCenter.y;
      float lengthOnCurve = position.x;
      if(currentCenter.y != originalCenter.y)
      {
        curFlatDirection = normalize(position.xy - vanishingPoint.xy);
        lengthFromCurve = (curveEnd.x*curveDirection.y-curveEnd.y*curveDirection.x-position.x*curveDirection.y+position.y*curveDirection.x)
        / (curFlatDirection.x*curveDirection.y-curFlatDirection.y*curveDirection.x);
        lengthOnCurve = length(position.xy+lengthFromCurve*curFlatDirection-curveEnd);
      }

      // define the control points of hermite curve, composed with two segments
      // calculation is carried out on the 2D plane which is passing through both current and original center and vertical to the image plane
      float currentLength = uCommonParameters[3][3];
      float originalLength =  abs(originalCenter.x/curveDirection.x);
      float height = uCommonParameters[3][2];
      float percentage = currentLength/originalLength;
      //vec2 SegmentOneControlPoint0 = vec2(0.0, 0.0);
      vec2 SegmentOneControlPoint1 = vec2((0.65*percentage - 0.15)*originalLength, (0.8 + 0.2 * percentage)*height);
      vec2 SegmentTwoControlPoint0 = SegmentOneControlPoint1;
      vec2 SegmentTwoControlPoint1 = vec2(currentLength, 0.0);
      vec2 SegmentOneTangentVector0 = SegmentOneControlPoint1;
      vec2 SegmentOneTangentVector1 = vec2(0.5*originalLength,0.0);
      vec2 SegmentTwoTangentVector0 = SegmentOneTangentVector1;
      vec2 SegmentTwoTangentVector1 = SegmentOneTangentVector1;

      // calculate the corresponding curve point position and its tangent vector
      // it is a linear mapping onto nonlinear curves, might cause some unwanted deformation
      // but as there are no analytical method to calculate the curve length on arbitrary segment
      // no efficient way to solve this nonlinear mapping, Numerical approximation would cost too much computation in shader
      vec2 curvePoint2D;
      vec2 tangent;
      float t0 = lengthOnCurve / originalLength;
      if(t0<=0.5)
      {
        float t = 2.0*t0;
        float t_2 = t*t;
        float t_3 = t*t_2;
        curvePoint2D = (-2.0*t_3+3.0*t_2)*SegmentOneControlPoint1
        + (t_3-2.0*t_2+t)*SegmentOneTangentVector0 + (t_3-t_2)*SegmentOneTangentVector1;
        tangent = (-6.0*t_2+6.0*t)*SegmentOneControlPoint1
        + (3.0*t_2-4.0*t+1.0)*SegmentOneTangentVector0 + (3.0*t_2-2.0*t)*SegmentOneTangentVector1;
      }
      else
      {
        float t = 2.0*t0-1.0;
        float t_2 = t*t;
        float t_3 = t*t_2;
        curvePoint2D = (2.0*t_3-3.0*t_2+1.0)*SegmentTwoControlPoint0 + (-2.0*t_3+3.0*t_2)*SegmentTwoControlPoint1
        + (t_3-2.0*t_2+t)*SegmentTwoTangentVector0 + (t_3-t_2)*SegmentTwoTangentVector1;
        tangent = (6.0*t_2-6.0*t)*SegmentTwoControlPoint0 + (-6.0*t_2+6.0*t)*SegmentTwoControlPoint1
        + (3.0*t_2-4.0*t+1.0)*SegmentTwoTangentVector0 + (3.0*t_2-2.0*t)*SegmentTwoTangentVector1;
        // a trick to eliminate some optical illusion caused by the gradient matter of normal in per-fragment shading
        // which is caused by linear interpolation of normal vs. nonlinear lighting
        // will notice some artifact in the areas with dramatically normal changes, so compress the normal differences here
        tangent.y *=  min(1.0, length(position.xyz - vanishingPoint) / uSize.y );
      }
      vec3 curvePoint = vec3(curveEnd - curvePoint2D.x*curveDirection,max(0.0,curvePoint2D.y));
      vec3 tangentVector = vec3(-tangent.x*curveDirection,tangent.y);

      // locate the new vertex position on the line passing through both vanishing point and the calculated curve point position
      vec3 curLiftDirection = vec3(0.0,-1.0,0.0);
      if(currentCenter.y != originalCenter.y)
      {
        curLiftDirection = normalize(curvePoint - vanishingPoint);
        tangentVector *= (curveDirection.y > 0.0) ? -1.0 : 1.0;
        // an heuristic adjustment here, to compensate the linear parameter mapping onto the nonlinear curve
        float Y0 = position.y - curveDirection.y * (position.x/curveDirection.x);
        float proportion;
        float refLength;
        if(abs(Y0-vanishingPoint.y) > abs(curveEnd.y-vanishingPoint.y))
        {
          proportion = abs(curveEnd.y - Y0) / (abs(curveEnd.y-Y0)+abs(curveEnd.y - vanishingPoint.y));
          refLength = proportion*length(originalCenter-vanishingPoint.xy) / (proportion-1.0);
        }
        else
        {
          proportion = abs(curveEnd.y - Y0) / abs(curveEnd.y - vanishingPoint.y);
          refLength = proportion*length(originalCenter-vanishingPoint.xy);
        }
        float Y1 = currentCenter.y - (normalize(currentCenter-vanishingPoint.xy)).y * refLength;
        position.y = mix(Y0, Y1, t0);
      }
      position.xz = curvePoint.xz - lengthFromCurve*curLiftDirection.xz;
      // calculate the normal vector, will be used for lighting
      normal = cross(curLiftDirection, normalize(tangentVector));
      // the signature of Z is decided by the page turning direction:
      // from left to right(negative); from right to left (positive)
      position.z *= -uIsTurningBack;
      normal.xy *= -uIsTurningBack;
    }
    // change the coordinate origin from the top-left of the page to its center
    position.xy -= uSize.xy * 0.5;
  }
  vNormal =  uNormalMatrix * normal;
  gl_Position = uMvpMatrix * position;

  // out parameters for fragment shader
  vTexCoord = aPosition + vec2(0.5);
  vTexCoord.x /= uTextureWidth;
  vPosition = uModelView * position;
}
