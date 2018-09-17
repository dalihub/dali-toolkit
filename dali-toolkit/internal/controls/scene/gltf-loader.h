#ifndef DALI_TOOLKIT_INTERNAL_GLTF_LOADER_H
#define DALI_TOOLKIT_INTERNAL_GLTF_LOADER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali/dali.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/internal/controls/scene/scene-impl.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace GLTF
{

enum ShaderType
{
  NO_TEXTURE_SHADER,
  BASECOLOR_SHADER,
  METALLICROUGHNESS_SHADER,
  BASECOLOR_METALLICROUGHNESS_SHADER,
  NORMAL_SHADER,
  BASECOLOR_NORMAL_SHADER,
  METALLICROUGHNESS_NORMAL_SHADER,
  BASECOLOR_METALLICROUGHNESS_NORMAL_SHADER,
  OCCLUSION_SHADER,
  BASECOLOR_OCCLUSION_SHADER,
  METALLICROUGHNESS_OCCLUSION_SHADER,
  BASECOLOR_METALLICROUGHNESS_OCCLUSION_SHADER,
  NORMAL_OCCLUSION_SHADER,
  BASECOLOR_NORMAL_OCCLUSION_SHADER,
  METALLICROUGHNESS_NORMAL_OCCLUSION_SHADER,
  BASECOLOR_METALLICROUGHNESS_NORMAL_OCCLUSION_SHADER,
  EMIT_SHADER,
  BASECOLOR_EMIT_SHADER,
  METALLICROUGHNESS_EMIT_SHADER,
  BASECOLOR_METALLICROUGHNESS_EMIT_SHADER,
  NORMAL_EMIT_SHADER,
  BASECOLOR_NORMAL_EMIT_SHADER,
  METALLICROUGHNESS_NORMAL_EMIT_SHADER,
  BASECOLOR_METALLICROUGHNESS_NORMAL_EMIT_SHADER,
  OCCLUSION_EMIT_SHADER,
  BASECOLOR_OCCLUSION_EMIT_SHADER,
  METALLICROUGHNESS_OCCLUSION_EMIT_SHADER,
  BASECOLOR_METALLICROUGHNESS_OCCLUSION_EMIT_SHADER,
  NORMAL_OCCLUSION_EMIT_SHADER,
  BASECOLOR_NORMAL_OCCLUSION_EMIT_SHADER,
  METALLICROUGHNESS_NORMAL_OCCLUSION_EMIT_SHADER,
  BASECOLOR_METALLICROUGHNESS_NORMAL_OCCLUSION_EMIT_SHADER,
  IBL_SHADER,
  IBL_BASECOLOR_SHADER,
  IBL_METALLICROUGHNESS_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_SHADER,
  IBL_NORMAL_SHADER,
  IBL_BASECOLOR_NORMAL_SHADER,
  IBL_METALLICROUGHNESS_NORMAL_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_NORMAL_SHADER,
  IBL_OCCLUSION_SHADER,
  IBL_BASECOLOR_OCCLUSION_SHADER,
  IBL_METALLICROUGHNESS_OCCLUSION_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_OCCLUSION_SHADER,
  IBL_NORMAL_OCCLUSION_SHADER,
  IBL_BASECOLOR_NORMAL_OCCLUSION_SHADER,
  IBL_METALLICROUGHNESS_NORMAL_OCCLUSION_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_NORMAL_OCCLUSION_SHADER,
  IBL_EMIT_SHADER,
  IBL_BASECOLOR_EMIT_SHADER,
  IBL_METALLICROUGHNESS_EMIT_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_EMIT_SHADER,
  IBL_NORMAL_EMIT_SHADER,
  IBL_BASECOLOR_NORMAL_EMIT_SHADER,
  IBL_METALLICROUGHNESS_NORMAL_EMIT_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_NORMAL_EMIT_SHADER,
  IBL_OCCLUSION_EMIT_SHADER,
  IBL_BASECOLOR_OCCLUSION_EMIT_SHADER,
  IBL_METALLICROUGHNESS_OCCLUSION_EMIT_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_OCCLUSION_EMIT_SHADER,
  IBL_NORMAL_OCCLUSION_EMIT_SHADER,
  IBL_BASECOLOR_NORMAL_OCCLUSION_EMIT_SHADER,
  IBL_METALLICROUGHNESS_NORMAL_OCCLUSION_EMIT_SHADER,
  IBL_BASECOLOR_METALLICROUGHNESS_NORMAL_OCCLUSION_EMIT_SHADER,
  SHADER_TYPE_MAX = IBL_BASECOLOR_METALLICROUGHNESS_NORMAL_OCCLUSION_EMIT_SHADER
};

struct BufferInfo
{
  BufferInfo()
    : byteLength( -1 ),
    uri( "" ),
    name( "" )
  {
  }

  ~BufferInfo()
  {
  }

  int byteLength;
  std::string uri;
  std::string name;
};

struct BufferViewInfo
{
  BufferViewInfo()
    : buffer( -1 ),
    byteOffset( 0 ),
    byteLength( 0 ),
    byteStride( 0 ),
    target( 0 ),
    name( "" )
  {
  }

  ~BufferViewInfo()
  {
  }

  int buffer;
  int byteOffset;
  int byteLength;
  int byteStride;
  int target;
  std::string name;
};

struct TextureInfo
{
  TextureInfo()
    : sourceIdx( -1 ),
    samplerIdx( -1 )
  {
  }

  ~TextureInfo()
  {
  }
  int sourceIdx;
  int samplerIdx;
};

struct PbrTextureInfo
{
  PbrTextureInfo()
    : index( -1 ),
    texCoord( 0 ),
    value( 0.0 )
  {
  }
  ~PbrTextureInfo()
  {
  }

  int index;
  int texCoord;
  float value;
};

struct MaterialInfo
{
  MaterialInfo()
    : baseColorFactor( 1, 1, 1, 1 ),
    metallicFactor( 1.0 ),
    roughnessFactor( 1.0 ),
    emissiveFactor( 0.0, 0.0, 0.0 ),
    alphaMode( "OPAQUE" ),
    alphaCutoff( 0.5 ),
    doubleSided( false ),
    name( "" )
  {
  }

  ~MaterialInfo()
  {
  }

  Vector4 baseColorFactor;
  float metallicFactor;
  float roughnessFactor;
  Vector3 emissiveFactor;
  std::string alphaMode;
  float alphaCutoff;
  bool doubleSided;

  PbrTextureInfo baseColorTexture;
  PbrTextureInfo metallicRoughnessTexture;
  PbrTextureInfo normalTexture;
  PbrTextureInfo occlusionTexture;
  PbrTextureInfo emissiveTexture;

  std::string name;
  //need to add max, min
};

struct AccessorInfo
{
  AccessorInfo()
    : bufferView( -1 ),
    byteOffset( 0 ),
    componentType( -1 ),
    normalized( false ),
    count( 0 ),
    type( "" ),
    max( 0 ),
    min( 0 ),
    name( "" )
  {
  }

  ~AccessorInfo()
  {
  }

  int bufferView;
  int byteOffset;
  int componentType;
  bool normalized;
  int count;
  std::string type;
  int max;
  int min;
  std::string name;
  //need to add max, min
};

struct Attribute
{
  Attribute()
    : POSITION( -1 ),
    NORMAL( -1 ),
    TANGENT( -1 )
  {
  }

  ~Attribute()
  {
  }

  int POSITION;
  int NORMAL;
  int TANGENT;

  std::vector<int> TEXCOORD;
  std::vector<int> COLOR;
};

struct MeshInfo
{
  MeshInfo()
    : indicesIdx( -1 ),
    materialsIdx( -1 ),
    mode( 4 )
  {
  }

  ~MeshInfo()
  {
  }
  Geometry geometry;
  std::string name;

  int indicesIdx;
  int materialsIdx;
  int mode;

  Vector3 size;
  Vector3 pivot;

  Attribute attribute;
  //need to add max, min
};

struct AnimationChannelInfo
{
  AnimationChannelInfo()
    : sampler( -1 ),
    targetNode( -1 ),
    path( "" )
  {
  }

  ~AnimationChannelInfo()
  {
  }

  int sampler;
  int targetNode;
  std::string path;

};

struct AnimationSamplerInfo
{
  AnimationSamplerInfo()
    : input( -1 ),
    output( -1 ),
    interpolation( "" )
  {
  }

  ~AnimationSamplerInfo()
  {
  }

  int input;
  int output;
  std::string interpolation;
};

struct AnimationInfo
{
  AnimationInfo()
    : name( "" )
  {
  }

  ~AnimationInfo()
  {
  }

  std::string name;
  std::vector<AnimationChannelInfo> channelArray;
  std::vector<AnimationSamplerInfo> samplerArray;
};

struct OrthographicInfo
{
  OrthographicInfo()
    : xmag( 0.0f ),
    ymag( 0.0f ),
    zfar( 0.0f ),
    znear( 0.0f )
  {
  }

  ~OrthographicInfo()
  {
  }

  float xmag;
  float ymag;
  float zfar;
  float znear;
};

struct PerspectiveInfo
{
  PerspectiveInfo()
    : aspectRatio( 0.0f ),
    yfov( 0.0f ),
    zfar( 0.0f ),
    znear( 0.0f )
  {
  }

  ~PerspectiveInfo()
  {
  }

  float aspectRatio;
  float yfov;
  float zfar;
  float znear;
};

struct CameraInfo
{
  CameraInfo()
    : name( "" ),
    type( "" )
  {
  }

  ~CameraInfo()
  {
  }

  std::string name;
  std::string type;
  OrthographicInfo orthographic;
  PerspectiveInfo perspective;
};

} // namespace GLTF

/**
 *
 * GltfLoader is a class to parse glTf, to load data from file, and to generate Scene.
 * This glTF loader supports glTF 2.0 features.
 *
 * @remarks glTF loader didn't support such features.
 *  - Sparse accessor
 *  - Morphing
 *  - Skeletal Animation
 * These features will be supported soon.
 *
 */
class GltfLoader
{
public:

  /**
   * @brief Create an uninitialized GltfLoader.
   */
  GltfLoader();

  /**
   * @brief Destructor
   */
  ~GltfLoader();

  /**
   * @brief Load Scene from scene format file(e.g., glTF).
   * @param[in] filePath Path of scene format file.
   * @param[in] scene Scene data loaded from file.
   * @return true if scene is successfully loaded
   */
  bool LoadScene( const std::string& filePath, Internal::Scene& scene );

private:
  bool ParseGltf( const std::string& filePath );
  bool LoadAssets();

  bool LoadBinaryData( const TreeNode* root );
  bool LoadBuffer( const TreeNode& buffer );
  bool LoadBufferView( const TreeNode& buffer );
  bool LoadAccessor( const TreeNode& buffer );

  bool LoadTextureArray( const TreeNode* root );
  Texture LoadTexture( const char* imageUrl, bool generateMipmaps );
  Sampler LoadSampler( const TreeNode& samplerNode );
  FilterMode::Type GetFilterMode( int mode );
  WrapMode::Type GetWrapMode( int mode );

  bool LoadMaterialSetArray( const TreeNode* root );
  bool LoadPbrMetallicRoughness( const TreeNode& material, GLTF::MaterialInfo& materialInfo );

  bool LoadMeshArray( const TreeNode* root );
  bool LoadPrimitive( const TreeNode& mesh, GLTF::MeshInfo& meshInfo );
  bool LoadAttribute( const TreeNode* attribute, GLTF::MeshInfo& meshInfo );
  bool SetGeometry( GLTF::MeshInfo& meshInfo );
  void SetMeshInfoAndCanonize( GLTF::MeshInfo& meshInfo, Dali::Vector<Dali::Vector3> &vertexBufferData );

  bool CreateScene( Internal::Scene& scene );

  void LoadCamera( Scene& scene );
  bool LoadOrthoGraphic( const TreeNode& camera, GLTF::CameraInfo& cameraInfo );
  bool LoadPerspective( const TreeNode& camera, GLTF::CameraInfo& cameraInfo );

  bool LoadSceneNodes( Scene& scene );
  Actor AddNode( Scene& scene, int index );
  void SetActorCache( Actor& actor, const int index );
  bool SetTextureAndSampler( TextureSet& textureSet, int textureIdx, std::string& toShader, std::string shader, int& addIdx );

  bool LoadAnimation( Scene& scene );
  bool LoadAnimationChannels( const TreeNode& animation, GLTF::AnimationInfo& animationInfo );
  bool LoadAnimationSamplers( const TreeNode& animation, GLTF::AnimationInfo& animationInfo );

  // template functions
  template <typename T>
  bool ReadBinFile( Vector<T> &dataBuffer, std::string url, int offset, int count );
  template <typename T>
  float IntToFloat( T element, bool normalize );
  template <typename Td, typename Ts>
  void FitBuffer( Dali::Vector<Td>& bufferDestiny, Dali::Vector<Ts>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize );
  template <typename Ts>
  void FitBuffer( Dali::Vector<Vector2>& bufferDestiny, Dali::Vector<Ts>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize );
  template <typename Ts>
  void FitBuffer( Dali::Vector<Vector3>& bufferDestiny, Dali::Vector<Ts>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize );
  template <typename Ts>
  void FitBuffer( Dali::Vector<Vector4>& bufferDestiny, Dali::Vector<Ts>& bufferSource, int bufferSize, int elementNumOfByteStride, bool normalize );
  template <typename T>
  void LoadDataFromAccessor( int accessorIdx, Dali::Vector<T>& bufferData );
  template <typename T>
  PropertyBuffer CreatePropertyBuffer( Vector<T> bufferData, std::string map, int type );
  template <typename T>
  void SetAttributeBufferData( GLTF::MeshInfo& meshInfo, int accessorIdx, std::string map, int type );
  template<typename T>
  float LoadKeyFrames( const GLTF::AnimationSamplerInfo& currentSampler, const Property::Index propIndex, KeyFrames& keyframes );

  void SetIndexBuffersData( GLTF::MeshInfo& meshInfo, int indexIdx );
  void SetVertexBufferData( GLTF::MeshInfo& meshInfo, int accessorIdx, std::string map, int type );

  // utility functions
  const TreeNode* Tidx( const TreeNode *node, int index );
  bool ReadBool( const TreeNode* node, bool& num );
  bool ReadInt( const TreeNode* node, int& num );
  bool ReadFloat( const TreeNode* node, float& num );
  bool ReadVector( const TreeNode* node, float* num, unsigned int size );
  bool ReadString( const TreeNode* node, std::string& strValue );

public:
  void InitGltf();

private:
  Dali::Toolkit::JsonParser mParser;
  const TreeNode* mNodes;
  const TreeNode* mRoot;

  std::string mPath;

  std::vector<Actor> mActorCache;
  Shader mShaderCache[GLTF::ShaderType::SHADER_TYPE_MAX + 1];

  std::vector<GLTF::BufferInfo> mBufferArray;
  std::vector<GLTF::BufferViewInfo> mBufferViewArray;
  std::vector<GLTF::AccessorInfo> mAccessorArray;

  std::vector<GLTF::MeshInfo> mMeshArray;
  std::vector<GLTF::MaterialInfo> mMaterialArray;
  std::vector<GLTF::TextureInfo> mTextureArray;

  std::vector<Texture> mSourceArray;
  std::vector<Sampler> mSamplerArray;
};

}//namespace Internal

}//namespace Toolkit

}//namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GLTF_LOADER_H
