#version 300 es

// Original Code
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/glTF-WebGL-PBR/shaders/pbr-vert.glsl
// Commit dc84b5e374fb3d23153d2248a338ef88173f9eb6

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

#ifdef VEC4_TANGENT
in vec4 aTangent;
#else
in vec3 aTangent;
#endif

in vec4 aVertexColor;

#ifdef MORPH
  uniform highp sampler2D sBlendShapeGeometry;
#endif

out mediump vec2 vUV;
out lowp mat3 vTBN;
out lowp vec4 vColor;
out highp vec3 vPositionToCamera;

uniform highp mat4 uViewMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uProjection;

#ifdef SKINNING
  in vec4 aJoints;
  in vec4 aWeights;
  #define MAX_BONES 64
  uniform mat4 uBone[MAX_BONES];
#endif

#ifdef MORPH
#define MAX_BLEND_SHAPE_NUMBER 128
uniform float uNumberOfBlendShapes;                                 ///< Total number of blend shapes loaded.
uniform highp float uBlendShapeWeight[MAX_BLEND_SHAPE_NUMBER];            ///< The weight of each blend shape.
#ifdef MORPH_VERSION_2_0
uniform float uBlendShapeUnnormalizeFactor;                         ///< Factor used to unnormalize the geometry of the blend shape.
#else
uniform float uBlendShapeUnnormalizeFactor[MAX_BLEND_SHAPE_NUMBER]; ///< Factor used to unnormalize the geometry of the blend shape.
#endif
uniform float uBlendShapeComponentSize;                             ///< The size in the texture of either the vertices, normals or tangents. Used to calculate the offset to address them.
#endif

void main()
{
  highp vec4 position = vec4(aPosition, 1.0);
  highp vec3 normal = aNormal;
  highp vec3 tangent = aTangent.xyz;

#ifdef MORPH
  int width = textureSize( sBlendShapeGeometry, 0 ).x;

  int blendShapeBufferOffset = 0;
  int blendShapeComponentSize = int(uBlendShapeComponentSize);
  int numberOfBlendShapes = int(uNumberOfBlendShapes);

  for( int index = 0; index < numberOfBlendShapes; ++index )
  {
    highp vec3 diff = vec3(0.0);

#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
    int vertexId = gl_VertexID + blendShapeBufferOffset;
    int x = vertexId % width;
    int y = vertexId / width;

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

    blendShapeBufferOffset += blendShapeComponentSize;
#endif

#ifdef MORPH_NORMAL
    // Calculate the index to retrieve the normal from the texture.
    vertexId = gl_VertexID + int(blendShapeBufferOffset);
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape normal from the texture, unnormalizes it and multiply by the weight.
    if( 0.0 != uBlendShapeWeight[index] )
    {
      diff = uBlendShapeWeight[index] * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    normal += diff.xyz;

    blendShapeBufferOffset += blendShapeComponentSize;
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

    blendShapeBufferOffset += blendShapeComponentSize;
#endif
  }

#endif

#ifdef SKINNING
  highp mat4 bone = uBone[int(aJoints.x)] * aWeights.x +
    uBone[int(aJoints.y)] * aWeights.y +
    uBone[int(aJoints.z)] * aWeights.z +
    uBone[int(aJoints.w)] * aWeights.w;
  position = bone * position;
  normal = (bone * vec4(normal, 0.0)).xyz;
  tangent = (bone * vec4(tangent, 0.0)).xyz;
#endif

  highp vec4 positionW = uModelMatrix * position;
  highp vec4 positionV = uViewMatrix * positionW;

  vPositionToCamera = transpose(mat3(uViewMatrix)) * -vec3(positionV.xyz / positionV.w);

  vec3 bitangent = cross(normal, tangent);
#ifdef VEC4_TANGENT
  bitangent *= aTangent.w;
#endif
  vTBN = mat3(uModelMatrix) * mat3(tangent, bitangent, normal);

#ifdef FLIP_V
  vUV = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
#else
  vUV = aTexCoord;
#endif

  vColor = aVertexColor;

  gl_Position = uProjection * positionV;
}
