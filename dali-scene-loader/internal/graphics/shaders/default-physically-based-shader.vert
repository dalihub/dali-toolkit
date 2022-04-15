#version 300 es

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;
in vec3 aTangent;

#ifdef MORPH
  uniform sampler2D sBlendShapeGeometry;
#endif

out vec2 vUV;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vViewVec;

uniform highp mat4 uMvpMatrix;
uniform highp mat4 uViewMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelView;
uniform mat4 uProjection;

#ifdef SKINNING
  in vec4 aJoints;
  in vec4 aWeights;
  #define MAX_BONES 64
  uniform mat4 uBone[MAX_BONES];
#endif

#ifdef MORPH
#define MAX_BLEND_SHAPE_NUMBER 128
uniform int uNumberOfBlendShapes;                                   ///< Total number of blend shapes loaded.
uniform float uBlendShapeWeight[MAX_BLEND_SHAPE_NUMBER];            ///< The weight of each blend shape.
#ifdef MORPH_VERSION_2_0
uniform float uBlendShapeUnnormalizeFactor;                         ///< Factor used to unnormalize the geometry of the blend shape.
#else
uniform float uBlendShapeUnnormalizeFactor[MAX_BLEND_SHAPE_NUMBER]; ///< Factor used to unnormalize the geometry of the blend shape.
#endif
uniform int uBlendShapeComponentSize;                               ///< The size in the texture of either the vertices, normals or tangents. Used to calculate the offset to address them.
#endif

void main()
{
  vec4 position = vec4(aPosition, 1.0);
  vec3 normal = aNormal;
  vec3 tangent = aTangent;

#ifdef MORPH
  int width = textureSize( sBlendShapeGeometry, 0 ).x;

  int blendShapeBufferOffset = 0;
  for( int index = 0; index < uNumberOfBlendShapes; ++index )
  {
#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
    int vertexId = gl_VertexID + blendShapeBufferOffset;
    int x = vertexId % width;
    int y = vertexId / width;

    vec3 diff = vec3(0.0);
    // Retrieves the blend shape geometry from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
#ifdef MORPH_VERSION_2_0
       float unnormalizeFactor = uBlendShapeUnnormalizeFactor;
#else
       float unnormalizeFactor = uBlendShapeUnnormalizeFactor[index];
#endif

      diff = uBlendShapeWeight[index] * unnormalizeFactor * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    position.xyz += diff;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_NORMAL
    // Calculate the index to retrieve the normal from the texture.
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape normal from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
      diff = uBlendShapeWeight[index] * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    normal += diff.xyz;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_TANGENT
    // Calculate the index to retrieve the tangent from the texture.
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape tangent from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
      diff = uBlendShapeWeight[index] * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    tangent += diff.xyz;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif
  }

#endif

#ifdef SKINNING
  mat4 bone = uBone[int(aJoints.x)] * aWeights.x +
    uBone[int(aJoints.y)] * aWeights.y +
    uBone[int(aJoints.z)] * aWeights.z +
    uBone[int(aJoints.w)] * aWeights.w;
  position = bone * position;
  normal = (bone * vec4(normal, 0.0)).xyz;
  tangent = (bone * vec4(tangent, 0.0)).xyz;
#endif

  vec4 vPosition = uModelMatrix * position;

  vNormal = normalize(uNormalMatrix * normal);

  vTangent = normalize(uNormalMatrix * tangent);


  vec4 viewPosition = uViewMatrix * vPosition;
  gl_Position = uProjection * viewPosition;

#ifdef FLIP_V
  vUV = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
#else
  vUV = aTexCoord;
#endif

  vViewVec = viewPosition.xyz;
}
