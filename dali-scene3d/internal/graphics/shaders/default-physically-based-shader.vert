
// Original Code
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/glTF-WebGL-PBR/shaders/pbr-vert.glsl
// Commit dc84b5e374fb3d23153d2248a338ef88173f9eb6

#define MORPH defined(MORPH_POSITION) || defined(MORPH_NORMAL) || defined(MORPH_TANGENT)

// These lines in the shader may be replaced with actual definitions by the model loader,
// if they are needed. Note, some shader compilers have problems with spurious ";", so
// the macro invocations don't have a trailing ";". The replacement strings in the model
// loader will provide it instead.
#define ADD_EXTRA_SKINNING_ATTRIBUTES
#define ADD_EXTRA_WEIGHTS

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

INPUT vec3 aPosition;
INPUT vec2 aTexCoord;
INPUT vec3 aNormal;

#ifdef VEC4_TANGENT
INPUT vec4 aTangent;
#else
INPUT vec3 aTangent;
#endif

INPUT vec4 aVertexColor;

#ifdef GLSL_VERSION_1_0
INPUT float aVertexID;
#endif

#ifdef SKINNING
INPUT vec4 aJoints0;
INPUT vec4 aWeights0;
ADD_EXTRA_SKINNING_ATTRIBUTES
#endif

#ifdef MORPH
uniform highp sampler2D sBlendShapeGeometry;
#ifdef GLSL_VERSION_1_0
uniform int uBlendShapeGeometryWidth;
uniform int uBlendShapeGeometryHeight;
#endif
#endif

OUTPUT mediump vec2 vUV;
OUTPUT lowp mat3 vTBN;
OUTPUT lowp vec4 vColor;
OUTPUT highp vec3 vPositionToCamera;

uniform highp mat4 uViewMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uProjection;

#ifdef SKINNING

#ifdef GLSL_VERSION_1_0
#define MAX_BONES 80
uniform mat4 uBone[MAX_BONES];
#else
#define MAX_BONES 256
layout(std140) uniform Bones
{
  mat4 uBone[MAX_BONES];
};
#endif

uniform mediump vec3 uYDirection;
#endif

#ifdef MORPH
#define MAX_BLEND_SHAPE_NUMBER 256
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
OUTPUT highp vec3 positionFromLightView;

void main()
{
  highp vec4 position = vec4(aPosition, 1.0);
  highp vec3 normal = aNormal;
  highp vec3 tangent = aTangent.xyz;

#ifdef MORPH
#ifdef GLSL_VERSION_1_0
  int width = uBlendShapeGeometryWidth;
#else
  int width = textureSize( sBlendShapeGeometry, 0 ).x;
#endif

  highp int blendShapeBufferOffset = 0;

#ifdef GLSL_VERSION_1_0
  highp float blendShapeWidth = float(uBlendShapeGeometryWidth);
  highp float blendShapeHeight = float(uBlendShapeGeometryHeight);
  highp float invertBlendShapeWidth = 1.0 / blendShapeWidth;
  highp float invertBlendShapeHeight = 1.0 / blendShapeHeight;
#endif

  for( int index = 0; index < uNumberOfBlendShapes; ++index )
  {
    highp vec3 diff = vec3(0.0);
    highp int vertexId = 0;
    highp int x = 0;
    highp int y = 0;
    highp float weight = clamp(uBlendShapeWeight[index], 0.0, 1.0);

#ifdef MORPH_POSITION
    // Calculate the index to retrieve the geometry from the texture.
#ifdef GLSL_VERSION_1_0
    vertexId = int(floor(aVertexID + 0.5)) + blendShapeBufferOffset;
    y = vertexId / width;
    x = vertexId - y * width;
#else
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;
#endif

    // Retrieves the blend shape geometry from the texture, unnormalizes it and multiply by the weight.
    if(0.0 != weight)
    {
#ifdef MORPH_VERSION_2_0
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor;
#else
       highp float unnormalizeFactor = uBlendShapeUnnormalizeFactor[index];
#endif

#ifdef GLSL_VERSION_1_0
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
    // Calculate the index to retrieve the normal from the texture.
#ifdef GLSL_VERSION_1_0
    vertexId = int(floor(aVertexID + 0.5)) + blendShapeBufferOffset;
    y = vertexId / width;
    x = vertexId - y * width;
#else
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;
#endif

    // Retrieves the blend shape normal from the texture, unnormalizes it and multiply by the weight.
    if(0.0 != weight)
    {
#ifdef GLSL_VERSION_1_0
      highp float floatX = float(x) + 0.5;
      highp float floatY = float(y) + 0.5;
      diff = weight * 2.0 * ( texture2D( sBlendShapeGeometry, vec2(floatX * invertBlendShapeWidth, floatY * invertBlendShapeHeight) ).xyz - 0.5 );
#else
      diff = weight * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
#endif
    }

    normal += diff.xyz;

    blendShapeBufferOffset += uBlendShapeComponentSize;
#endif

#ifdef MORPH_TANGENT
    // Calculate the index to retrieve the tangent from the texture.
#ifdef GLSL_VERSION_1_0
    vertexId = int(floor(aVertexID + 0.5)) + blendShapeBufferOffset;
    y = vertexId / width;
    x = vertexId - y * width;
#else
    vertexId = gl_VertexID + blendShapeBufferOffset;
    x = vertexId % width;
    y = vertexId / width;
#endif

    // Retrieves the blend shape tangent from the texture, unnormalizes it and multiply by the weight.
    if(0.0 != weight)
    {
#ifdef GLSL_VERSION_1_0
      highp float floatX = float(x) + 0.5;
      highp float floatY = float(y) + 0.5;
      diff = weight * 2.0 * ( texture2D( sBlendShapeGeometry, vec2(floatX * invertBlendShapeWidth, floatY * invertBlendShapeHeight) ).xyz - 0.5 );
#else
      diff = weight * 2.0 * ( texelFetch( sBlendShapeGeometry, ivec2(x, y), 0 ).xyz - 0.5 );
#endif
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

  ADD_EXTRA_WEIGHTS

  position = bone * position;
  normal = uYDirection * (bone * vec4(normal, 0.0)).xyz;
  tangent = uYDirection * (bone * vec4(tangent, 0.0)).xyz;

  highp vec4 positionW = position;
#else
  highp vec4 positionW = uModelMatrix * position;
#endif

  highp vec4 positionV = uViewMatrix * positionW;

#ifdef GLSL_VERSION_1_0
  highp vec3 i0 = uViewMatrix[0].xyz;
  highp vec3 i1 = uViewMatrix[1].xyz;
  highp vec3 i2 = uViewMatrix[2].xyz;

  vPositionToCamera = mat3(vec3(i0.x, i1.x, i2.x), vec3(i0.y, i1.y, i2.y), vec3(i0.z, i1.z, i2.z)) * -vec3(positionV.xyz / positionV.w);
#else
  vPositionToCamera = transpose(mat3(uViewMatrix)) * -vec3(positionV.xyz / positionV.w);
#endif

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
