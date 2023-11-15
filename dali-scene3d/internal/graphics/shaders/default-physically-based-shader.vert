#version 300 es

// Original Code
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/glTF-WebGL-PBR/shaders/pbr-vert.glsl
// Commit dc84b5e374fb3d23153d2248a338ef88173f9eb6

#define MORPH defined(MORPH_POSITION) || defined(MORPH_NORMAL) || defined(MORPH_TANGENT)

// If needed, define these strings in code, insert after each.
#define ADD_EXTRA_SKINNING_ATTRIBUTES
#define ADD_EXTRA_WEIGHTS

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

#ifdef SKINNING
in vec4 aJoints0;
in vec4 aWeights0;
ADD_EXTRA_SKINNING_ATTRIBUTES;
#endif

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

#define MAX_BONES 256
layout(std140) uniform Bones
{
  mat4 uBone[MAX_BONES];
};

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

// Shadow
uniform lowp int uIsShadowEnabled;
uniform highp mat4 uShadowLightViewProjectionMatrix;
out highp vec3 positionFromLightView;

void main()
{
  highp vec4 position = vec4(aPosition, 1.0);
  highp vec3 normal = aNormal;
  highp vec3 tangent = aTangent.xyz;

#ifdef MORPH
  int width = textureSize( sBlendShapeGeometry, 0 ).x;

  highp int blendShapeBufferOffset = 0;

  for( int index = 0; index < uNumberOfBlendShapes; ++index )
  {
    highp vec3 diff = vec3(0.0);
    highp int vertexId = 0;
    highp int x = 0;
    highp int y = 0;
    highp float weight = clamp(uBlendShapeWeight[index], 0.0, 1.0);

#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;

    // Retrieves the blend shape geometry from the texture, unnormalizes it and multiply by the weight.
    if(0.0 != weight)
    {
#ifdef MORPH_VERSION_2_0
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor;
#else
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor[index];
#endif

      diff = weight * unnormalizeFactor * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
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
    if(0.0 != weight)
    {
      diff = weight * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
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
    if(0.0 != weight)
    {
      diff = weight * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
    }

    tangent += diff.xyz;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif
  }

#endif




#ifdef SKINNING
  highp mat4 bone =
    uBone[int(aJoints0.x)] * aWeights0.x +
    uBone[int(aJoints0.y)] * aWeights0.y +
    uBone[int(aJoints0.z)] * aWeights0.z +
    uBone[int(aJoints0.w)] * aWeights0.w;

  ADD_EXTRA_WEIGHTS;

  position = bone * position;
  normal = uYDirection * (bone * vec4(normal, 0.0)).xyz;
  tangent = uYDirection * (bone * vec4(tangent, 0.0)).xyz;

  highp vec4 positionW = position;
#else
  highp vec4 positionW = uModelMatrix * position;
#endif

  highp vec4 positionV = uViewMatrix * positionW;

  vPositionToCamera = transpose(mat3(uViewMatrix)) * -vec3(positionV.xyz / positionV.w);

  normal = normalize(normal);
  tangent = normalize(tangent);
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

  positionFromLightView = vec3(1.0);
  if(uIsShadowEnabled > 0)
  {
    highp vec4 positionInLightView = uShadowLightViewProjectionMatrix * positionW;
    positionFromLightView = ((positionInLightView.xyz / positionInLightView.w) * 0.5) + vec3(0.5);
  }

  gl_Position = uProjection * positionV;
}
