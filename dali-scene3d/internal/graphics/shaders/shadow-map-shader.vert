#version 300 es

precision highp float;

in vec3 aPosition;
in vec2 aTexCoord;
in vec4 aVertexColor;

out mediump vec2 vUV;
out lowp vec4 vColor;

uniform highp mat4 uViewMatrix;
uniform highp mat4 uModelMatrix;
uniform highp mat4 uProjection;

#ifdef SKINNING
  in vec4 aJoints;
  in vec4 aWeights;
  #define MAX_BONES 64
  uniform mat4 uBone[MAX_BONES];
  uniform mediump vec3 uYDirection;
#endif

#ifdef MORPH
#define MAX_BLEND_SHAPE_NUMBER 128
uniform int uNumberOfBlendShapes;                                         ///< Total number of blend shapes loaded.
uniform highp float uBlendShapeWeight[MAX_BLEND_SHAPE_NUMBER];            ///< The weight of each blend shape.
#ifdef MORPH_VERSION_2_0
uniform highp float uBlendShapeUnnormalizeFactor;                         ///< Factor used to unnormalize the geometry of the blend shape.
#else
uniform highp float uBlendShapeUnnormalizeFactor[MAX_BLEND_SHAPE_NUMBER]; ///< Factor used to unnormalize the geometry of the blend shape.
#endif
uniform highp int uBlendShapeComponentSize;                               ///< The size in the texture of either the vertices, normals or tangents. Used to calculate the offset to address them.
#endif

uniform highp mat4 uShadowLightViewProjectionMatrix;

void main()
{
  highp vec4 position = vec4(aPosition, 1.0);

#ifdef MORPH
  int width = textureSize( sBlendShapeGeometry, 0 ).x;

  highp int blendShapeBufferOffset = 0;

  for( int index = 0; index < uNumberOfBlendShapes; ++index )
  {
    highp vec3 diff = vec3(0.0);
    highp int vertexId = 0;
    highp int x = 0;
    highp int y = 0;

#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape geometry from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
#ifdef MORPH_VERSION_2_0
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor;
#else
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor[index];
#endif

      diff = uBlendShapeWeight[index] * unnormalizeFactor * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    position.xyz += diff;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_NORMAL
    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_TANGENT
    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif
  }

#endif

#ifdef SKINNING
  highp mat4 bone = uBone[int(aJoints.x)] * aWeights.x +
    uBone[int(aJoints.y)] * aWeights.y +
    uBone[int(aJoints.z)] * aWeights.z +
    uBone[int(aJoints.w)] * aWeights.w;
  position = bone * position;

  highp vec4 positionW = position;
#else
  highp vec4 positionW = uModelMatrix * position;
#endif

  // To synchronize View-Projection matrix with pbr shader
  gl_Position = uShadowLightViewProjectionMatrix * positionW;

#ifdef FLIP_V
  vUV = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
#else
  vUV = aTexCoord;
#endif

  vColor = aVertexColor;
}
