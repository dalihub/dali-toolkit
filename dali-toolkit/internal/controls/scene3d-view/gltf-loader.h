#ifndef DALI_TOOLKIT_INTERNAL_GLTF_LOADER_H
#define DALI_TOOLKIT_INTERNAL_GLTF_LOADER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/json-parser.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class Scene3dView;

namespace Gltf
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
  : byteLength(-1),
    uri(""),
    name("")
  {
  }

  ~BufferInfo()
  {
  }

  int32_t     byteLength;
  std::string uri;
  std::string name;
};

struct BufferViewInfo
{
  BufferViewInfo()
  : buffer(-1),
    byteOffset(0),
    byteLength(0),
    byteStride(0),
    target(0),
    name("")
  {
  }

  ~BufferViewInfo()
  {
  }

  int32_t     buffer;
  int32_t     byteOffset;
  int32_t     byteLength;
  int32_t     byteStride;
  int32_t     target;
  std::string name;
};

struct TextureInfo
{
  TextureInfo()
  : sourceIdx(-1),
    samplerIdx(-1)
  {
  }

  ~TextureInfo()
  {
  }
  int32_t sourceIdx;
  int32_t samplerIdx;
};

struct PbrTextureInfo
{
  PbrTextureInfo()
  : index(-1),
    texCoord(0),
    value(0.0)
  {
  }
  ~PbrTextureInfo()
  {
  }

  int32_t index;
  int32_t texCoord;
  float   value;
};

struct MaterialInfo
{
  MaterialInfo()
  : baseColorFactor(1, 1, 1, 1),
    metallicFactor(1.0),
    roughnessFactor(1.0),
    emissiveFactor(0.0, 0.0, 0.0),
    alphaMode("OPAQUE"),
    alphaCutoff(0.5),
    doubleSided(false),
    name("")
  {
  }

  ~MaterialInfo()
  {
  }

  Vector4     baseColorFactor;
  float       metallicFactor;
  float       roughnessFactor;
  Vector3     emissiveFactor;
  std::string alphaMode;
  float       alphaCutoff;
  bool        doubleSided;

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
  : bufferView(-1),
    byteOffset(0),
    componentType(-1),
    normalized(false),
    count(0),
    type(""),
    max(0),
    min(0),
    name("")
  {
  }

  ~AccessorInfo()
  {
  }

  int32_t     bufferView;
  int32_t     byteOffset;
  int32_t     componentType;
  bool        normalized;
  int32_t     count;
  std::string type;
  int32_t     max;
  int32_t     min;
  std::string name;
  //need to add max, min
};

struct Attribute
{
  Attribute()
  : POSITION(-1),
    NORMAL(-1),
    TANGENT(-1)
  {
  }

  ~Attribute()
  {
  }

  int32_t POSITION;
  int32_t NORMAL;
  int32_t TANGENT;

  std::vector<int32_t> TEXCOORD;
  std::vector<int32_t> COLOR;
};

struct MeshInfo
{
  MeshInfo()
  : indicesIdx(-1),
    materialsIdx(-1),
    mode(4)
  {
  }

  ~MeshInfo()
  {
  }
  Geometry    geometry;
  std::string name;

  int32_t indicesIdx;
  int32_t materialsIdx;
  int32_t mode;

  Vector3 size;
  Vector3 pivot;

  Attribute attribute;
  //need to add max, min
};

struct AnimationChannelInfo
{
  AnimationChannelInfo()
  : sampler(-1),
    targetNode(-1),
    path("")
  {
  }

  ~AnimationChannelInfo()
  {
  }

  int32_t     sampler;
  int32_t     targetNode;
  std::string path;
};

struct AnimationSamplerInfo
{
  AnimationSamplerInfo()
  : input(-1),
    output(-1),
    interpolation("")
  {
  }

  ~AnimationSamplerInfo()
  {
  }

  int32_t     input;
  int32_t     output;
  std::string interpolation;
};

struct AnimationInfo
{
  AnimationInfo()
  : name("")
  {
  }

  ~AnimationInfo()
  {
  }

  std::string                       name;
  std::vector<AnimationChannelInfo> channelArray;
  std::vector<AnimationSamplerInfo> samplerArray;
};

struct OrthographicInfo
{
  OrthographicInfo()
  : xmag(0.0f),
    ymag(0.0f),
    zfar(0.0f),
    znear(0.0f)
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
  : aspectRatio(0.0f),
    yfov(0.0f),
    zfar(0.0f),
    znear(0.0f)
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
  : name(""),
    type("")
  {
  }

  ~CameraInfo()
  {
  }

  std::string      name;
  std::string      type;
  OrthographicInfo orthographic;
  PerspectiveInfo  perspective;
};

/**
 *
 * Loader is a class to parse glTf, to load data from file, and to generate Scene.
 * This glTF loader supports glTF 2.0 features.
 *
 * @remarks glTF loader didn't support such features.
 *  - Sparse accessor
 *  - Morphing
 *  - Skeletal Animation
 * These features will be supported soon.
 *
 */
class Loader
{
public:
  /**
   * @brief Create an uninitialized Loader.
   */
  Loader();

  /**
   * @brief Destructor
   */
  ~Loader();

  /**
   * @brief Load Scene3dView from scene format file(e.g., glTF).
   * @param[in] filePath Path of scene format file.
   * @param[in] scene3dView Scene3dView data loaded from file.
   * @return true if scene is successfully loaded
   */
  bool LoadScene(const std::string& filePath, Internal::Scene3dView& scene3dView);

private:
  bool ParseGltf(const std::string& filePath);
  bool LoadAssets();

  bool CreateScene(Internal::Scene3dView& scene3dView);

  void LoadCamera(Scene3dView& scene3dView);
  bool LoadOrthoGraphic(const TreeNode& camera, CameraInfo& cameraInfo);
  bool LoadPerspective(const TreeNode& camera, CameraInfo& cameraInfo);

  bool  LoadSceneNodes(Scene3dView& scene3dView);
  Actor AddNode(Scene3dView& scene3dView, uint32_t index);
  void  SetActorCache(Actor& actor, uint32_t index);
  bool  SetTextureAndSampler(TextureSet& textureSet, int32_t textureIdx, std::string& toShader, std::string shader, int32_t& addIdx);

  bool LoadAnimation(Scene3dView& scene3dView);
  bool LoadAnimationChannels(const TreeNode& animation, AnimationInfo& animationInfo);
  bool LoadAnimationSamplers(const TreeNode& animation, AnimationInfo& animationInfo);

private:
  Dali::Toolkit::JsonParser mParser;
  const TreeNode*           mNodes;
  const TreeNode*           mRoot;

  std::string mPath;

  std::vector<Actor> mActorCache;
  Shader             mShaderCache[ShaderType::SHADER_TYPE_MAX + 1];

  std::vector<BufferInfo>     mBufferArray;
  std::vector<BufferViewInfo> mBufferViewArray;
  std::vector<AccessorInfo>   mAccessorArray;

  std::vector<MeshInfo>     mMeshArray;
  std::vector<MaterialInfo> mMaterialArray;
  std::vector<TextureInfo>  mTextureArray;

  std::vector<Texture> mSourceArray;
  std::vector<Sampler> mSamplerArray;
};

} //namespace Gltf

} //namespace Internal

} //namespace Toolkit

} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GLTF_LOADER_H
