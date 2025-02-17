//@name shadow-map-shader.vert

//@version 100

#define MORPH defined(MORPH_POSITION) || defined(MORPH_NORMAL) || defined(MORPH_TANGENT)

#define ADD_EXTRA_SKINNING_ATTRIBUTES
#define ADD_EXTRA_WEIGHTS

precision highp float;

INPUT vec3 aPosition;
INPUT vec2 aTexCoord;
INPUT vec4 aVertexColor;

#ifdef SL_VERSION_LOW
INPUT float aVertexID;
#endif

#ifdef SKINNING
INPUT vec4 aJoints0;
INPUT vec4 aWeights0;
ADD_EXTRA_SKINNING_ATTRIBUTES;
#endif

#ifdef MORPH
UNIFORM highp sampler2D sBlendShapeGeometry;
#endif

#ifdef SKINNING
#ifndef SL_VERSION_LOW
#define MAX_BONES 256
UNIFORM_BLOCK Bones
{
  UNIFORM mat4 uBone[MAX_BONES];
};
#endif
#endif

OUTPUT highp vec2 vUV;
OUTPUT highp vec4 vColor;

UNIFORM_BLOCK VertBlock
{
#ifdef MORPH
#ifdef SL_VERSION_LOW
  UNIFORM highp int uBlendShapeGeometryWidth;
  UNIFORM highp int uBlendShapeGeometryHeight;
#endif
#endif

  UNIFORM highp mat4 uViewMatrix;
  UNIFORM highp mat4 uModelMatrix;
  UNIFORM highp mat4 uProjection;

#ifdef SKINNING
#ifdef SL_VERSION_LOW
#define MAX_BONES 80
  UNIFORM mat4 uBone[MAX_BONES];
#endif
  UNIFORM mediump vec3 uYDirection;
#endif

#ifdef MORPH
#define MAX_BLEND_SHAPE_NUMBER 256
  UNIFORM highp int uNumberOfBlendShapes;///< Total number of blend shapes loaded.
  UNIFORM highp float uBlendShapeWeight[MAX_BLEND_SHAPE_NUMBER];///< The weight of each blend shape.
#ifdef MORPH_VERSION_2_0
  UNIFORM highp float uBlendShapeUnnormalizeFactor;///< Factor used to unnormalize the geometry of the blend shape.
#else
  UNIFORM highp float uBlendShapeUnnormalizeFactor[MAX_BLEND_SHAPE_NUMBER];///< Factor used to unnormalize the geometry of the blend shape.
#endif
  UNIFORM highp int uBlendShapeComponentSize;///< The size in the texture of either the vertices, normals or tangents. Used to calculate the offset to address them.
#endif

  UNIFORM highp mat4 uShadowLightViewProjectionMatrix;
};

void main()
{
  highp vec4 position = vec4(aPosition, 1.0);

#ifdef MORPH

#ifdef SL_VERSION_LOW
  highp int width = uBlendShapeGeometryWidth;
#else
  highp int width = textureSize( sBlendShapeGeometry, 0 ).x;
#endif

  highp int blendShapeBufferOffset = 0;

#ifdef SL_VERSION_LOW
  highp float blendShapeWidth = float(uBlendShapeGeometryWidth);
  highp float blendShapeHeight = float(uBlendShapeGeometryHeight);
  highp float invertBlendShapeWidth = 1.0 / blendShapeWidth;
  highp float invertBlendShapeHeight = 1.0 / blendShapeHeight;
#endif

  for( highp int index = 0; index < uNumberOfBlendShapes; ++index )
  {
    highp vec3 diff = vec3(0.0);
    highp int vertexId = 0;
    highp int x = 0;
    highp int y = 0;
    highp float weight = clamp(uBlendShapeWeight[index], 0.0, 1.0);

#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
#ifdef SL_VERSION_LOW
    vertexId = int(floor(aVertexID + 0.5)) + blendShapeBufferOffset;
    y = vertexId / width;
    x = vertexId - y * width;
#else
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;
#endif

    // Retrieves the blend shape geometry from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
#ifdef MORPH_VERSION_2_0
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor;
#else
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor[index];
#endif

#ifdef SL_VERSION_LOW
      highp float floatX = float(x) + 0.5;
      highp float floatY = float(y) + 0.5;
      diff = weight * unnormalizeFactor * ( texture2D( sBlendShapeGeometry, vec2(floatX * invertBlendShapeWidth, floatY * invertBlendShapeHeight) ).xyz - 0.5 );
#else
      diff = weight * unnormalizeFactor * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
#endif
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
  highp mat4 bone = uBone[int(aJoints0.x)] * aWeights0.x +
    uBone[int(aJoints0.y)] * aWeights0.y +
    uBone[int(aJoints0.z)] * aWeights0.z +
    uBone[int(aJoints0.w)] * aWeights0.w;

  ADD_EXTRA_WEIGHTS;

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
