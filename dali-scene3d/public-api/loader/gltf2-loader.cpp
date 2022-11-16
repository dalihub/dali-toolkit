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
#include "dali-scene3d/public-api/loader/gltf2-loader.h"
#include <fstream>
#include "dali-scene3d/internal/loader/gltf2-asset.h"
#include "dali-scene3d/public-api/loader/load-result.h"
#include "dali-scene3d/public-api/loader/resource-bundle.h"
#include "dali-scene3d/public-api/loader/scene-definition.h"
#include "dali-scene3d/public-api/loader/shader-definition-factory.h"
#include "dali-scene3d/public-api/loader/utils.h"
#include "dali/public-api/math/quaternion.h"

#define ENUM_STRING_MAPPING(t, x) \
  {                               \
#x, t::x                      \
  }

namespace gt = gltf2;
namespace js = json;

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
const std::string POSITION_PROPERTY("position");
const std::string ORIENTATION_PROPERTY("orientation");
const std::string SCALE_PROPERTY("scale");
const std::string BLEND_SHAPE_WEIGHTS_UNIFORM("uBlendShapeWeight");
const std::string MRENDERER_MODEL_IDENTIFICATION("M-Renderer");
const std::string ROOT_NODE_NAME("RootNode");
const Vector3     SCALE_TO_ADJUST(100.0f, 100.0f, 100.0f);

const Geometry::Type GLTF2_TO_DALI_PRIMITIVES[]{
  Geometry::POINTS,
  Geometry::LINES,
  Geometry::LINE_LOOP,
  Geometry::LINE_STRIP,
  Geometry::TRIANGLES,
  Geometry::TRIANGLE_STRIP,
  Geometry::TRIANGLE_FAN}; //...because Dali swaps the last two.

struct AttributeMapping
{
  gt::Attribute::Type      mType;
  MeshDefinition::Accessor MeshDefinition::*mAccessor;
  uint16_t                                  mElementSizeRequired;
} ATTRIBUTE_MAPPINGS[]{
  {gt::Attribute::NORMAL, &MeshDefinition::mNormals, sizeof(Vector3)},
  {gt::Attribute::TANGENT, &MeshDefinition::mTangents, sizeof(Vector3)},
  {gt::Attribute::TEXCOORD_0, &MeshDefinition::mTexCoords, sizeof(Vector2)},
  {gt::Attribute::COLOR_0, &MeshDefinition::mColors, sizeof(Vector4)},
  {gt::Attribute::JOINTS_0, &MeshDefinition::mJoints0, sizeof(Vector4)},
  {gt::Attribute::WEIGHTS_0, &MeshDefinition::mWeights0, sizeof(Vector4)},
};

std::vector<gt::Animation> ReadAnimationArray(const json_value_s& j)
{
  gt::Animation proxy;
  SetRefReaderObject(proxy);

  auto results = js::Read::Array<gt::Animation, js::ObjectReader<gt::Animation>::Read>(j);

  for(auto& animation : results)
  {
    for(auto& channel : animation.mChannels)
    {
      channel.mSampler.UpdateVector(animation.mSamplers);
    }
  }

  return results;
}

void ApplyAccessorMinMax(const gt::Accessor& acc, float* values)
{
  DALI_ASSERT_ALWAYS(acc.mMax.empty() || gt::AccessorType::ElementCount(acc.mType) == acc.mMax.size());
  DALI_ASSERT_ALWAYS(acc.mMin.empty() || gt::AccessorType::ElementCount(acc.mType) == acc.mMin.size());
  MeshDefinition::Blob::ApplyMinMax(acc.mMin, acc.mMax, acc.mCount, values);
}

const auto BUFFER_READER = std::move(js::Reader<gt::Buffer>()
                                       .Register(*js::MakeProperty("byteLength", js::Read::Number<uint32_t>, &gt::Buffer::mByteLength))
                                       .Register(*js::MakeProperty("uri", js::Read::StringView, &gt::Buffer::mUri)));

const auto BUFFER_VIEW_READER = std::move(js::Reader<gt::BufferView>()
                                            .Register(*js::MakeProperty("buffer", gt::RefReader<gt::Document>::Read<gt::Buffer, &gt::Document::mBuffers>, &gt::BufferView::mBuffer))
                                            .Register(*js::MakeProperty("byteOffset", js::Read::Number<uint32_t>, &gt::BufferView::mByteOffset))
                                            .Register(*js::MakeProperty("byteLength", js::Read::Number<uint32_t>, &gt::BufferView::mByteLength))
                                            .Register(*js::MakeProperty("byteStride", js::Read::Number<uint32_t>, &gt::BufferView::mByteStride))
                                            .Register(*js::MakeProperty("target", js::Read::Number<uint32_t>, &gt::BufferView::mTarget)));

const auto BUFFER_VIEW_CLIENT_READER = std::move(js::Reader<gt::BufferViewClient>()
                                                   .Register(*js::MakeProperty("bufferView", gt::RefReader<gt::Document>::Read<gt::BufferView, &gt::Document::mBufferViews>, &gt::BufferViewClient::mBufferView))
                                                   .Register(*js::MakeProperty("byteOffset", js::Read::Number<uint32_t>, &gt::BufferViewClient::mByteOffset)));

const auto COMPONENT_TYPED_BUFFER_VIEW_CLIENT_READER = std::move(js::Reader<gt::ComponentTypedBufferViewClient>()
                                                                   .Register(*new js::Property<gt::ComponentTypedBufferViewClient, gt::Ref<gt::BufferView>>("bufferView", gt::RefReader<gt::Document>::Read<gt::BufferView, &gt::Document::mBufferViews>, &gt::ComponentTypedBufferViewClient::mBufferView))
                                                                   .Register(*new js::Property<gt::ComponentTypedBufferViewClient, uint32_t>("byteOffset", js::Read::Number<uint32_t>, &gt::ComponentTypedBufferViewClient::mByteOffset))
                                                                   .Register(*js::MakeProperty("componentType", js::Read::Enum<gt::Component::Type>, &gt::ComponentTypedBufferViewClient::mComponentType)));

const auto ACCESSOR_SPARSE_READER = std::move(js::Reader<gt::Accessor::Sparse>()
                                                .Register(*js::MakeProperty("count", js::Read::Number<uint32_t>, &gt::Accessor::Sparse::mCount))
                                                .Register(*js::MakeProperty("indices", js::ObjectReader<gt::ComponentTypedBufferViewClient>::Read, &gt::Accessor::Sparse::mIndices))
                                                .Register(*js::MakeProperty("values", js::ObjectReader<gt::BufferViewClient>::Read, &gt::Accessor::Sparse::mValues)));

const auto ACCESSOR_READER = std::move(js::Reader<gt::Accessor>()
                                         .Register(*new js::Property<gt::Accessor, gt::Ref<gt::BufferView>>("bufferView",
                                                                                                            gt::RefReader<gt::Document>::Read<gt::BufferView, &gt::Document::mBufferViews>,
                                                                                                            &gt::Accessor::mBufferView))
                                         .Register(*new js::Property<gt::Accessor, uint32_t>("byteOffset",
                                                                                             js::Read::Number<uint32_t>,
                                                                                             &gt::Accessor::mByteOffset))
                                         .Register(*new js::Property<gt::Accessor, gt::Component::Type>("componentType",
                                                                                                        js::Read::Enum<gt::Component::Type>,
                                                                                                        &gt::Accessor::mComponentType))
                                         .Register(*new js::Property<gt::Accessor, std::string_view>("name", js::Read::StringView, &gt::Accessor::mName))
                                         .Register(*js::MakeProperty("count", js::Read::Number<uint32_t>, &gt::Accessor::mCount))
                                         .Register(*js::MakeProperty("normalized", js::Read::Boolean, &gt::Accessor::mNormalized))
                                         .Register(*js::MakeProperty("type", gt::ReadStringEnum<gt::AccessorType>, &gt::Accessor::mType))
                                         .Register(*js::MakeProperty("min", js::Read::Array<float, js::Read::Number>, &gt::Accessor::mMin))
                                         .Register(*js::MakeProperty("max", js::Read::Array<float, js::Read::Number>, &gt::Accessor::mMax))
                                         .Register(*new js::Property<gt::Accessor, gt::Accessor::Sparse>("sparse", js::ObjectReader<gt::Accessor::Sparse>::Read, &gt::Accessor::SetSparse)));

const auto IMAGE_READER = std::move(js::Reader<gt::Image>()
                                      .Register(*new js::Property<gt::Image, std::string_view>("name", js::Read::StringView, &gt::Material::mName))
                                      .Register(*js::MakeProperty("uri", js::Read::StringView, &gt::Image::mUri))
                                      .Register(*js::MakeProperty("mimeType", js::Read::StringView, &gt::Image::mMimeType))
                                      .Register(*js::MakeProperty("bufferView", gt::RefReader<gt::Document>::Read<gt::BufferView, &gt::Document::mBufferViews>, &gt::Image::mBufferView)));

const auto SAMPLER_READER = std::move(js::Reader<gt::Sampler>()
                                        .Register(*js::MakeProperty("minFilter", js::Read::Enum<gt::Filter::Type>, &gt::Sampler::mMinFilter))
                                        .Register(*js::MakeProperty("magFilter", js::Read::Enum<gt::Filter::Type>, &gt::Sampler::mMagFilter))
                                        .Register(*js::MakeProperty("wrapS", js::Read::Enum<gt::Wrap::Type>, &gt::Sampler::mWrapS))
                                        .Register(*js::MakeProperty("wrapT", js::Read::Enum<gt::Wrap::Type>, &gt::Sampler::mWrapT)));

const auto TEXURE_READER = std::move(js::Reader<gt::Texture>()
                                       .Register(*js::MakeProperty("source", gt::RefReader<gt::Document>::Read<gt::Image, &gt::Document::mImages>, &gt::Texture::mSource))
                                       .Register(*js::MakeProperty("sampler", gt::RefReader<gt::Document>::Read<gt::Sampler, &gt::Document::mSamplers>, &gt::Texture::mSampler)));

const auto TEXURE_INFO_READER = std::move(js::Reader<gt::TextureInfo>()
                                            .Register(*js::MakeProperty("index", gt::RefReader<gt::Document>::Read<gt::Texture, &gt::Document::mTextures>, &gt::TextureInfo::mTexture))
                                            .Register(*js::MakeProperty("texCoord", js::Read::Number<uint32_t>, &gt::TextureInfo::mTexCoord))
                                            .Register(*js::MakeProperty("scale", js::Read::Number<float>, &gt::TextureInfo::mScale))
                                            .Register(*js::MakeProperty("strength", js::Read::Number<float>, &gt::TextureInfo::mStrength)));

const auto MATERIAL_PBR_READER = std::move(js::Reader<gt::Material::Pbr>()
                                             .Register(*js::MakeProperty("baseColorFactor", gt::ReadDaliVector<Vector4>, &gt::Material::Pbr::mBaseColorFactor))
                                             .Register(*js::MakeProperty("baseColorTexture", js::ObjectReader<gt::TextureInfo>::Read, &gt::Material::Pbr::mBaseColorTexture))
                                             .Register(*js::MakeProperty("metallicFactor", js::Read::Number<float>, &gt::Material::Pbr::mMetallicFactor))
                                             .Register(*js::MakeProperty("roughnessFactor", js::Read::Number<float>, &gt::Material::Pbr::mRoughnessFactor))
                                             .Register(*js::MakeProperty("metallicRoughnessTexture", js::ObjectReader<gt::TextureInfo>::Read, &gt::Material::Pbr::mMetallicRoughnessTexture)));

const auto MATERIAL_READER = std::move(js::Reader<gt::Material>()
                                         .Register(*new js::Property<gt::Material, std::string_view>("name", js::Read::StringView, &gt::Material::mName))
                                         .Register(*js::MakeProperty("pbrMetallicRoughness", js::ObjectReader<gt::Material::Pbr>::Read, &gt::Material::mPbrMetallicRoughness))
                                         .Register(*js::MakeProperty("normalTexture", js::ObjectReader<gt::TextureInfo>::Read, &gt::Material::mNormalTexture))
                                         .Register(*js::MakeProperty("occlusionTexture", js::ObjectReader<gt::TextureInfo>::Read, &gt::Material::mOcclusionTexture))
                                         .Register(*js::MakeProperty("emissiveTexture", js::ObjectReader<gt::TextureInfo>::Read, &gt::Material::mEmissiveTexture))
                                         .Register(*js::MakeProperty("emissiveFactor", gt::ReadDaliVector<Vector3>, &gt::Material::mEmissiveFactor))
                                         .Register(*js::MakeProperty("alphaMode", gt::ReadStringEnum<gt::AlphaMode>, &gt::Material::mAlphaMode))
                                         .Register(*js::MakeProperty("alphaCutoff", js::Read::Number<float>, &gt::Material::mAlphaCutoff)));

std::map<gt::Attribute::Type, gt::Ref<gt::Accessor>> ReadMeshPrimitiveAttributes(const json_value_s& j)
{
  auto&                                                jo = js::Cast<json_object_s>(j);
  std::map<gt::Attribute::Type, gt::Ref<gt::Accessor>> result;

  auto i = jo.start;
  while(i)
  {
    auto jstr                                                        = *i->name;
    result[gt::Attribute::FromString(jstr.string, jstr.string_size)] = gt::RefReader<gt::Document>::Read<gt::Accessor, &gt::Document::mAccessors>(*i->value);
    i                                                                = i->next;
  }
  return result;
}

std::vector<std::map<gt::Attribute::Type, gt::Ref<gt::Accessor>>> ReadMeshPrimitiveTargets(const json_value_s& j)
{
  auto&                                                             jo = js::Cast<json_array_s>(j);
  std::vector<std::map<gt::Attribute::Type, gt::Ref<gt::Accessor>>> result;

  result.reserve(jo.length);

  auto i = jo.start;
  while(i)
  {
    result.push_back(std::move(ReadMeshPrimitiveAttributes(*i->value)));
    i = i->next;
  }

  return result;
}

const auto MESH_PRIMITIVE_READER = std::move(js::Reader<gt::Mesh::Primitive>()
                                               .Register(*js::MakeProperty("attributes", ReadMeshPrimitiveAttributes, &gt::Mesh::Primitive::mAttributes))
                                               .Register(*js::MakeProperty("indices", gt::RefReader<gt::Document>::Read<gt::Accessor, &gt::Document::mAccessors>, &gt::Mesh::Primitive::mIndices))
                                               .Register(*js::MakeProperty("material", gt::RefReader<gt::Document>::Read<gt::Material, &gt::Document::mMaterials>, &gt::Mesh::Primitive::mMaterial))
                                               .Register(*js::MakeProperty("mode", js::Read::Enum<gt::Mesh::Primitive::Mode>, &gt::Mesh::Primitive::mMode))
                                               .Register(*js::MakeProperty("targets", ReadMeshPrimitiveTargets, &gt::Mesh::Primitive::mTargets)));

const auto MESH_READER = std::move(js::Reader<gt::Mesh>()
                                     .Register(*new js::Property<gt::Mesh, std::string_view>("name", js::Read::StringView, &gt::Mesh::mName))
                                     .Register(*js::MakeProperty("primitives",
                                                                 js::Read::Array<gt::Mesh::Primitive, js::ObjectReader<gt::Mesh::Primitive>::Read>,
                                                                 &gt::Mesh::mPrimitives))
                                     .Register(*js::MakeProperty("weights", js::Read::Array<float, js::Read::Number>, &gt::Mesh::mWeights)));

const auto SKIN_READER = std::move(js::Reader<gt::Skin>()
                                     .Register(*new js::Property<gt::Skin, std::string_view>("name", js::Read::StringView, &gt::Skin::mName))
                                     .Register(*js::MakeProperty("inverseBindMatrices",
                                                                 gt::RefReader<gt::Document>::Read<gt::Accessor, &gt::Document::mAccessors>,
                                                                 &gt::Skin::mInverseBindMatrices))
                                     .Register(*js::MakeProperty("skeleton",
                                                                 gt::RefReader<gt::Document>::Read<gt::Node, &gt::Document::mNodes>,
                                                                 &gt::Skin::mSkeleton))
                                     .Register(*js::MakeProperty("joints",
                                                                 js::Read::Array<gt::Ref<gt::Node>, gt::RefReader<gt::Document>::Read<gt::Node, &gt::Document::mNodes>>,
                                                                 &gt::Skin::mJoints)));

const auto CAMERA_PERSPECTIVE_READER = std::move(js::Reader<gt::Camera::Perspective>()
                                                   .Register(*js::MakeProperty("aspectRatio", js::Read::Number<float>, &gt::Camera::Perspective::mAspectRatio))
                                                   .Register(*js::MakeProperty("yfov", js::Read::Number<float>, &gt::Camera::Perspective::mYFov))
                                                   .Register(*js::MakeProperty("zfar", js::Read::Number<float>, &gt::Camera::Perspective::mZFar))
                                                   .Register(*js::MakeProperty("znear", js::Read::Number<float>, &gt::Camera::Perspective::mZNear))); // TODO: infinite perspective projection, where znear is omitted

const auto CAMERA_ORTHOGRAPHIC_READER = std::move(js::Reader<gt::Camera::Orthographic>()
                                                    .Register(*js::MakeProperty("xmag", js::Read::Number<float>, &gt::Camera::Orthographic::mXMag))
                                                    .Register(*js::MakeProperty("ymag", js::Read::Number<float>, &gt::Camera::Orthographic::mXMag))
                                                    .Register(*js::MakeProperty("zfar", js::Read::Number<float>, &gt::Camera::Orthographic::mZFar))
                                                    .Register(*js::MakeProperty("znear", js::Read::Number<float>, &gt::Camera::Orthographic::mZNear)));

const auto CAMERA_READER = std::move(js::Reader<gt::Camera>()
                                       .Register(*new js::Property<gt::Camera, std::string_view>("name", js::Read::StringView, &gt::Camera::mName))
                                       .Register(*js::MakeProperty("type", js::Read::StringView, &gt::Camera::mType))
                                       .Register(*js::MakeProperty("perspective", js::ObjectReader<gt::Camera::Perspective>::Read, &gt::Camera::mPerspective))
                                       .Register(*js::MakeProperty("orthographic", js::ObjectReader<gt::Camera::Orthographic>::Read, &gt::Camera::mOrthographic)));

const auto NODE_READER = std::move(js::Reader<gt::Node>()
                                     .Register(*new js::Property<gt::Node, std::string_view>("name", js::Read::StringView, &gt::Node::mName))
                                     .Register(*js::MakeProperty("translation", gt::ReadDaliVector<Vector3>, &gt::Node::mTranslation))
                                     .Register(*js::MakeProperty("rotation", gt::ReadQuaternion, &gt::Node::mRotation))
                                     .Register(*js::MakeProperty("scale", gt::ReadDaliVector<Vector3>, &gt::Node::mScale))
                                     .Register(*new js::Property<gt::Node, Matrix>("matrix", gt::ReadDaliVector<Matrix>, &gt::Node::SetMatrix))
                                     .Register(*js::MakeProperty("camera", gt::RefReader<gt::Document>::Read<gt::Camera, &gt::Document::mCameras>, &gt::Node::mCamera))
                                     .Register(*js::MakeProperty("children", js::Read::Array<gt::Ref<gt::Node>, gt::RefReader<gt::Document>::Read<gt::Node, &gt::Document::mNodes>>, &gt::Node::mChildren))
                                     .Register(*js::MakeProperty("mesh", gt::RefReader<gt::Document>::Read<gt::Mesh, &gt::Document::mMeshes>, &gt::Node::mMesh))
                                     .Register(*js::MakeProperty("skin", gt::RefReader<gt::Document>::Read<gt::Skin, &gt::Document::mSkins>, &gt::Node::mSkin)));

const auto ANIMATION_SAMPLER_READER = std::move(js::Reader<gt::Animation::Sampler>()
                                                  .Register(*js::MakeProperty("input", gt::RefReader<gt::Document>::Read<gt::Accessor, &gt::Document::mAccessors>, &gt::Animation::Sampler::mInput))
                                                  .Register(*js::MakeProperty("output", gt::RefReader<gt::Document>::Read<gt::Accessor, &gt::Document::mAccessors>, &gt::Animation::Sampler::mOutput))
                                                  .Register(*js::MakeProperty("interpolation", gt::ReadStringEnum<gt::Animation::Sampler::Interpolation>, &gt::Animation::Sampler::mInterpolation)));

const auto ANIMATION_TARGET_READER = std::move(js::Reader<gt::Animation::Channel::Target>()
                                                 .Register(*js::MakeProperty("node", gt::RefReader<gt::Document>::Read<gt::Node, &gt::Document::mNodes>, &gt::Animation::Channel::Target::mNode))
                                                 .Register(*js::MakeProperty("path", gt::ReadStringEnum<gt::Animation::Channel::Target>, &gt::Animation::Channel::Target::mPath)));

const auto ANIMATION_CHANNEL_READER = std::move(js::Reader<gt::Animation::Channel>()
                                                  .Register(*js::MakeProperty("target", js::ObjectReader<gt::Animation::Channel::Target>::Read, &gt::Animation::Channel::mTarget))
                                                  .Register(*js::MakeProperty("sampler", gt::RefReader<gt::Animation>::Read<gt::Animation::Sampler, &gt::Animation::mSamplers>, &gt::Animation::Channel::mSampler)));

const auto ANIMATION_READER = std::move(js::Reader<gt::Animation>()
                                          .Register(*new js::Property<gt::Animation, std::string_view>("name", js::Read::StringView, &gt::Animation::mName))
                                          .Register(*js::MakeProperty("samplers",
                                                                      js::Read::Array<gt::Animation::Sampler, js::ObjectReader<gt::Animation::Sampler>::Read>,
                                                                      &gt::Animation::mSamplers))
                                          .Register(*js::MakeProperty("channels",
                                                                      js::Read::Array<gt::Animation::Channel, js::ObjectReader<gt::Animation::Channel>::Read>,
                                                                      &gt::Animation::mChannels)));

const auto SCENE_READER = std::move(js::Reader<gt::Scene>()
                                      .Register(*new js::Property<gt::Scene, std::string_view>("name", js::Read::StringView, &gt::Scene::mName))
                                      .Register(*js::MakeProperty("nodes",
                                                                  js::Read::Array<gt::Ref<gt::Node>, gt::RefReader<gt::Document>::Read<gt::Node, &gt::Document::mNodes>>,
                                                                  &gt::Scene::mNodes)));

const auto DOCUMENT_READER = std::move(js::Reader<gt::Document>()
                                         .Register(*js::MakeProperty("buffers",
                                                                     js::Read::Array<gt::Buffer, js::ObjectReader<gt::Buffer>::Read>,
                                                                     &gt::Document::mBuffers))
                                         .Register(*js::MakeProperty("bufferViews",
                                                                     js::Read::Array<gt::BufferView, js::ObjectReader<gt::BufferView>::Read>,
                                                                     &gt::Document::mBufferViews))
                                         .Register(*js::MakeProperty("accessors",
                                                                     js::Read::Array<gt::Accessor, js::ObjectReader<gt::Accessor>::Read>,
                                                                     &gt::Document::mAccessors))
                                         .Register(*js::MakeProperty("images",
                                                                     js::Read::Array<gt::Image, js::ObjectReader<gt::Image>::Read>,
                                                                     &gt::Document::mImages))
                                         .Register(*js::MakeProperty("samplers",
                                                                     js::Read::Array<gt::Sampler, js::ObjectReader<gt::Sampler>::Read>,
                                                                     &gt::Document::mSamplers))
                                         .Register(*js::MakeProperty("textures",
                                                                     js::Read::Array<gt::Texture, js::ObjectReader<gt::Texture>::Read>,
                                                                     &gt::Document::mTextures))
                                         .Register(*js::MakeProperty("materials",
                                                                     js::Read::Array<gt::Material, js::ObjectReader<gt::Material>::Read>,
                                                                     &gt::Document::mMaterials))
                                         .Register(*js::MakeProperty("meshes",
                                                                     js::Read::Array<gt::Mesh, js::ObjectReader<gt::Mesh>::Read>,
                                                                     &gt::Document::mMeshes))
                                         .Register(*js::MakeProperty("skins",
                                                                     js::Read::Array<gt::Skin, js::ObjectReader<gt::Skin>::Read>,
                                                                     &gt::Document::mSkins))
                                         .Register(*js::MakeProperty("cameras",
                                                                     js::Read::Array<gt::Camera, js::ObjectReader<gt::Camera>::Read>,
                                                                     &gt::Document::mCameras))
                                         .Register(*js::MakeProperty("nodes",
                                                                     js::Read::Array<gt::Node, js::ObjectReader<gt::Node>::Read>,
                                                                     &gt::Document::mNodes))
                                         .Register(*js::MakeProperty("animations",
                                                                     ReadAnimationArray,
                                                                     &gt::Document::mAnimations))
                                         .Register(*js::MakeProperty("scenes",
                                                                     js::Read::Array<gt::Scene, js::ObjectReader<gt::Scene>::Read>,
                                                                     &gt::Document::mScenes))
                                         .Register(*js::MakeProperty("scene", gt::RefReader<gt::Document>::Read<gt::Scene, &gt::Document::mScenes>, &gt::Document::mScene)));

struct NodeMapping
{
  Index gltfIdx;
  Index runtimeIdx;
};

bool operator<(const NodeMapping& mapping, Index gltfIdx)
{
  return mapping.gltfIdx < gltfIdx;
}

class NodeIndexMapper
{
public:
  NodeIndexMapper()                       = default;
  NodeIndexMapper(const NodeIndexMapper&) = delete;
  NodeIndexMapper& operator=(const NodeIndexMapper&) = delete;

  ///@brief Registers a mapping of the @a gltfIdx of a node to its @a runtimeIdx .
  ///@note If the indices are the same, the registration is omitted, in order to
  /// save growing a vector.
  void RegisterMapping(Index gltfIdx, Index runtimeIdx)
  {
    if(gltfIdx != runtimeIdx)
    {
      auto iInsert = std::lower_bound(mNodes.begin(), mNodes.end(), gltfIdx);
      DALI_ASSERT_DEBUG(iInsert == mNodes.end() || iInsert->gltfIdx != gltfIdx);
      mNodes.insert(iInsert, NodeMapping{gltfIdx, runtimeIdx});
    }
  }

  ///@brief Retrieves the runtime index of a Node, mapped to the given @a gltfIdx.
  Index GetRuntimeId(Index gltfIdx) const
  {
    auto iFind = std::lower_bound(mNodes.begin(), mNodes.end(), gltfIdx); // using custom operator<
    return (iFind != mNodes.end() && iFind->gltfIdx == gltfIdx) ? iFind->runtimeIdx : gltfIdx;
  }

private:
  std::vector<NodeMapping> mNodes;
};

struct ConversionContext
{
  LoadResult& mOutput;

  std::string mPath;
  Index       mDefaultMaterial;

  std::vector<Index> mMeshIds;
  NodeIndexMapper    mNodeIndices;
};

SamplerFlags::Type ConvertWrapMode(gt::Wrap::Type w)
{
  switch(w)
  {
    case gt::Wrap::REPEAT:
      return SamplerFlags::WRAP_REPEAT;
    case gt::Wrap::CLAMP_TO_EDGE:
      return SamplerFlags::WRAP_CLAMP;
    case gt::Wrap::MIRRORED_REPEAT:
      return SamplerFlags::WRAP_MIRROR;
    default:
      throw std::runtime_error("Invalid wrap type.");
  }
}

SamplerFlags::Type ConvertSampler(const gt::Ref<gt::Sampler>& s)
{
  if(s)
  {
    return (s->mMinFilter < gt::Filter::NEAREST_MIPMAP_NEAREST) ? (s->mMinFilter - gt::Filter::NEAREST) : ((s->mMinFilter - gt::Filter::NEAREST_MIPMAP_NEAREST) + 2) | ((s->mMagFilter - gt::Filter::NEAREST) << SamplerFlags::FILTER_MAG_SHIFT) | (ConvertWrapMode(s->mWrapS) << SamplerFlags::WRAP_S_SHIFT) | (ConvertWrapMode(s->mWrapT) << SamplerFlags::WRAP_T_SHIFT);
  }
  else
  {
    // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#texturesampler
    // "The index of the sampler used by this texture. When undefined, a sampler with repeat wrapping and auto filtering should be used."
    // "What is an auto filtering", I hear you ask. Since there's nothing else to determine mipmapping from - including glTF image
    // properties, if not in some extension -, we will simply assume linear filtering.
    return SamplerFlags::FILTER_LINEAR | (SamplerFlags::FILTER_LINEAR << SamplerFlags::FILTER_MAG_SHIFT) |
           (SamplerFlags::WRAP_REPEAT << SamplerFlags::WRAP_S_SHIFT) | (SamplerFlags::WRAP_REPEAT << SamplerFlags::WRAP_T_SHIFT);
  }
}

TextureDefinition ConvertTextureInfo(const gt::TextureInfo& mm)
{
  return TextureDefinition{std::string(mm.mTexture->mSource->mUri), ConvertSampler(mm.mTexture->mSampler)};
}

void ConvertMaterial(const gt::Material& m, decltype(ResourceBundle::mMaterials)& outMaterials)
{
  MaterialDefinition matDef;

  auto& pbr = m.mPbrMetallicRoughness;
  if(m.mAlphaMode != gt::AlphaMode::OPAQUE || pbr.mBaseColorFactor.a < 1.f)
  {
    matDef.mFlags |= MaterialDefinition::TRANSPARENCY;
  }

  if(m.mAlphaMode == gt::AlphaMode::MASK)
  {
    matDef.SetAlphaCutoff(std::min(1.f, std::max(0.f, m.mAlphaCutoff)));
  }

  matDef.mBaseColorFactor = pbr.mBaseColorFactor;

  matDef.mTextureStages.reserve(!!pbr.mBaseColorTexture + !!pbr.mMetallicRoughnessTexture + !!m.mNormalTexture + !!m.mOcclusionTexture + !!m.mEmissiveTexture);
  if(pbr.mBaseColorTexture)
  {
    const auto semantic = MaterialDefinition::ALBEDO;
    matDef.mTextureStages.push_back({semantic, ConvertTextureInfo(pbr.mBaseColorTexture)});
    // TODO: and there had better be one
    matDef.mFlags |= semantic;
  }
  else
  {
    matDef.mNeedAlbedoTexture = false;
  }

  matDef.mMetallic  = pbr.mMetallicFactor;
  matDef.mRoughness = pbr.mRoughnessFactor;

  if(pbr.mMetallicRoughnessTexture)
  {
    const auto semantic = MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS |
                          MaterialDefinition::GLTF_CHANNELS;
    matDef.mTextureStages.push_back({semantic, ConvertTextureInfo(pbr.mMetallicRoughnessTexture)});
    // TODO: and there had better be one
    matDef.mFlags |= semantic;
  }
  else
  {
    matDef.mNeedMetallicRoughnessTexture = false;
  }

  matDef.mNormalScale = m.mNormalTexture.mScale;
  if(m.mNormalTexture)
  {
    const auto semantic = MaterialDefinition::NORMAL;
    matDef.mTextureStages.push_back({semantic, ConvertTextureInfo(m.mNormalTexture)});
    // TODO: and there had better be one
    matDef.mFlags |= semantic;
  }
  else
  {
    matDef.mNeedNormalTexture = false;
  }

  // TODO: handle doubleSided
  if(m.mOcclusionTexture)
  {
    const auto semantic = MaterialDefinition::OCCLUSION;
    matDef.mTextureStages.push_back({semantic, ConvertTextureInfo(m.mOcclusionTexture)});
    // TODO: and there had better be one
    matDef.mFlags |= semantic;
    matDef.mOcclusionStrength = m.mOcclusionTexture.mStrength;
  }

  if(m.mEmissiveTexture)
  {
    const auto semantic = MaterialDefinition::EMISSIVE;
    matDef.mTextureStages.push_back({semantic, ConvertTextureInfo(m.mEmissiveTexture)});
    // TODO: and there had better be one
    matDef.mFlags |= semantic;
    matDef.mEmissiveFactor = m.mEmissiveFactor;
  }

  outMaterials.emplace_back(std::move(matDef), TextureSet());
}

void ConvertMaterials(const gt::Document& doc, ConversionContext& cctx)
{
  auto& outMaterials = cctx.mOutput.mResources.mMaterials;
  outMaterials.reserve(doc.mMaterials.size());

  for(auto& m : doc.mMaterials)
  {
    ConvertMaterial(m, outMaterials);
  }
}

MeshDefinition::Accessor ConvertMeshPrimitiveAccessor(const gt::Accessor& acc)
{
  DALI_ASSERT_ALWAYS((acc.mBufferView &&
                      (acc.mBufferView->mByteStride < std::numeric_limits<uint16_t>::max())) ||
                     (acc.mSparse && !acc.mBufferView));

  DALI_ASSERT_ALWAYS(!acc.mSparse ||
                     ((acc.mSparse->mIndices.mBufferView && (acc.mSparse->mIndices.mBufferView->mByteStride < std::numeric_limits<uint16_t>::max())) &&
                      (acc.mSparse->mValues.mBufferView && (acc.mSparse->mValues.mBufferView->mByteStride < std::numeric_limits<uint16_t>::max()))));

  MeshDefinition::SparseBlob sparseBlob;
  if(acc.mSparse)
  {
    const gt::Accessor::Sparse&               sparse  = *acc.mSparse;
    const gt::ComponentTypedBufferViewClient& indices = sparse.mIndices;
    const gt::BufferViewClient&               values  = sparse.mValues;

    MeshDefinition::Blob indicesBlob(
      indices.mBufferView->mByteOffset + indices.mByteOffset,
      sparse.mCount * indices.GetBytesPerComponent(),
      static_cast<uint16_t>(indices.mBufferView->mByteStride),
      static_cast<uint16_t>(indices.GetBytesPerComponent()),
      {},
      {});
    MeshDefinition::Blob valuesBlob(
      values.mBufferView->mByteOffset + values.mByteOffset,
      sparse.mCount * acc.GetElementSizeBytes(),
      static_cast<uint16_t>(values.mBufferView->mByteStride),
      static_cast<uint16_t>(acc.GetElementSizeBytes()),
      {},
      {});

    sparseBlob = std::move(MeshDefinition::SparseBlob(std::move(indicesBlob), std::move(valuesBlob), acc.mSparse->mCount));
  }

  uint32_t bufferViewOffset = 0u;
  uint32_t bufferViewStride = 0u;
  if(acc.mBufferView)
  {
    bufferViewOffset = acc.mBufferView->mByteOffset;
    bufferViewStride = acc.mBufferView->mByteStride;
  }

  return MeshDefinition::Accessor{
    std::move(MeshDefinition::Blob{bufferViewOffset + acc.mByteOffset,
                                   acc.GetBytesLength(),
                                   static_cast<uint16_t>(bufferViewStride),
                                   static_cast<uint16_t>(acc.GetElementSizeBytes()),
                                   acc.mMin,
                                   acc.mMax}),
    std::move(sparseBlob)};
}

void ConvertMeshes(const gt::Document& doc, ConversionContext& cctx)
{
  uint32_t meshCount = 0;
  cctx.mMeshIds.reserve(doc.mMeshes.size());
  for(auto& m : doc.mMeshes)
  {
    cctx.mMeshIds.push_back(meshCount);
    meshCount += m.mPrimitives.size();
  }

  auto& outMeshes = cctx.mOutput.mResources.mMeshes;
  outMeshes.reserve(meshCount);
  for(auto& m : doc.mMeshes)
  {
    for(auto& p : m.mPrimitives)
    {
      MeshDefinition meshDef;

      auto& attribs          = p.mAttributes;
      meshDef.mUri           = attribs.begin()->second->mBufferView->mBuffer->mUri;
      meshDef.mPrimitiveType = GLTF2_TO_DALI_PRIMITIVES[p.mMode];

      auto& accPositions = *attribs.find(gt::Attribute::POSITION)->second;
      meshDef.mPositions = ConvertMeshPrimitiveAccessor(accPositions);
      // glTF2 support vector4 tangent for mesh.
      // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#meshes-overview
      meshDef.mTangentType = Property::VECTOR4;

      const bool needNormalsTangents = accPositions.mType == gt::AccessorType::VEC3;
      for(auto& am : ATTRIBUTE_MAPPINGS)
      {
        auto iFind = attribs.find(am.mType);
        if(iFind != attribs.end())
        {
          DALI_ASSERT_DEBUG(iFind->second->mBufferView->mBuffer->mUri.compare(meshDef.mUri) == 0);
          auto& accessor = meshDef.*(am.mAccessor);
          accessor       = ConvertMeshPrimitiveAccessor(*iFind->second);

          if(iFind->first == gt::Attribute::JOINTS_0)
          {
            meshDef.mFlags |= (iFind->second->mComponentType == gt::Component::UNSIGNED_SHORT) * MeshDefinition::U16_JOINT_IDS;
            DALI_ASSERT_DEBUG(MaskMatch(meshDef.mFlags, MeshDefinition::U16_JOINT_IDS) || iFind->second->mComponentType == gt::Component::FLOAT);
          }
        }
        else if(needNormalsTangents)
        {
          switch(am.mType)
          {
            case gt::Attribute::NORMAL:
              meshDef.RequestNormals();
              break;

            case gt::Attribute::TANGENT:
              meshDef.RequestTangents();
              break;

            default:
              break;
          }
        }
      }

      if(p.mIndices)
      {
        meshDef.mIndices = ConvertMeshPrimitiveAccessor(*p.mIndices);
        meshDef.mFlags |= (p.mIndices->mComponentType == gt::Component::UNSIGNED_INT) * MeshDefinition::U32_INDICES;
        DALI_ASSERT_DEBUG(MaskMatch(meshDef.mFlags, MeshDefinition::U32_INDICES) || p.mIndices->mComponentType == gt::Component::UNSIGNED_SHORT);
      }

      if(!p.mTargets.empty())
      {
        meshDef.mBlendShapes.reserve(p.mTargets.size());
        meshDef.mBlendShapeVersion = BlendShapes::Version::VERSION_2_0;
        for(const auto& target : p.mTargets)
        {
          MeshDefinition::BlendShape blendShape;

          auto endIt = target.end();
          auto it    = target.find(gt::Attribute::POSITION);
          if(it != endIt)
          {
            blendShape.deltas = ConvertMeshPrimitiveAccessor(*it->second);
          }
          it = target.find(gt::Attribute::NORMAL);
          if(it != endIt)
          {
            blendShape.normals = ConvertMeshPrimitiveAccessor(*it->second);
          }
          it = target.find(gt::Attribute::TANGENT);
          if(it != endIt)
          {
            blendShape.tangents = ConvertMeshPrimitiveAccessor(*it->second);
          }

          if(!m.mWeights.empty())
          {
            blendShape.weight = m.mWeights[meshDef.mBlendShapes.size()];
          }

          meshDef.mBlendShapes.push_back(std::move(blendShape));
        }
      }

      outMeshes.push_back({std::move(meshDef), MeshGeometry{}});
    }
  }
}

ModelNode* MakeModelNode(const gt::Mesh::Primitive& prim, ConversionContext& cctx)
{
  auto modelNode = new ModelNode();

  modelNode->mShaderIdx = 0; // TODO: further thought

  auto materialIdx = prim.mMaterial.GetIndex();
  if(INVALID_INDEX == materialIdx)
  {
    // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#default-material
    if(INVALID_INDEX == cctx.mDefaultMaterial)
    {
      auto& outMaterials    = cctx.mOutput.mResources.mMaterials;
      cctx.mDefaultMaterial = outMaterials.size();

      ConvertMaterial(gt::Material{}, outMaterials);
    }

    materialIdx = cctx.mDefaultMaterial;
  }

  modelNode->mMaterialIdx = materialIdx;

  return modelNode;
}

void ConvertCamera(const gt::Camera& camera, CameraParameters& camParams)
{
  camParams.isPerspective = camera.mType.compare("perspective") == 0;
  if(camParams.isPerspective)
  {
    auto& perspective = camera.mPerspective;
    camParams.yFov    = Degree(Radian(perspective.mYFov)).degree;
    camParams.zNear   = perspective.mZNear;
    camParams.zFar    = perspective.mZFar;
    // TODO: yes, we seem to ignore aspectRatio in CameraParameters.
  }
  else
  {
    auto& ortho                = camera.mOrthographic;
    camParams.orthographicSize = ortho.mYMag * .5f;
    camParams.aspectRatio      = ortho.mXMag / ortho.mYMag;
    camParams.zNear            = ortho.mZNear;
    camParams.zFar             = ortho.mZFar;
  }
}

void ConvertNode(gt::Node const& node, const Index gltfIdx, Index parentIdx, ConversionContext& cctx, bool isMRendererModel)
{
  auto& output    = cctx.mOutput;
  auto& scene     = output.mScene;
  auto& resources = output.mResources;

  const auto idx      = scene.GetNodeCount();
  auto       weakNode = scene.AddNode([&]() {
    std::unique_ptr<NodeDefinition> nodeDef{new NodeDefinition()};

    nodeDef->mParentIdx = parentIdx;
    nodeDef->mName      = node.mName;
    if(nodeDef->mName.empty())
    {
      // TODO: Production quality generation of unique names.
      nodeDef->mName = std::to_string(reinterpret_cast<uintptr_t>(nodeDef.get()));
    }

    if(!node.mSkin) // Nodes with skinned meshes are not supposed to have local transforms.
    {
      nodeDef->mPosition    = node.mTranslation;
      nodeDef->mOrientation = node.mRotation;
      nodeDef->mScale       = node.mScale;

      if(isMRendererModel && node.mName == ROOT_NODE_NAME && node.mScale == SCALE_TO_ADJUST)
      {
        nodeDef->mScale *= 0.01f;
      }
    }

    return nodeDef;
  }());
  if(!weakNode)
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Node name '" << node.mName << "' is not unique; scene is invalid.";
  }

  cctx.mNodeIndices.RegisterMapping(gltfIdx, idx);

  Index skeletonIdx = node.mSkin ? node.mSkin.GetIndex() : INVALID_INDEX;
  if(node.mMesh && !node.mMesh->mPrimitives.empty())
  {
    auto& mesh = *node.mMesh;

    auto iPrim          = mesh.mPrimitives.begin();
    auto modelNode      = MakeModelNode(*iPrim, cctx);
    auto meshIdx        = cctx.mMeshIds[node.mMesh.GetIndex()];
    modelNode->mMeshIdx = meshIdx;

    weakNode->mRenderable.reset(modelNode);

    DALI_ASSERT_DEBUG(resources.mMeshes[meshIdx].first.mSkeletonIdx == INVALID_INDEX ||
                      resources.mMeshes[meshIdx].first.mSkeletonIdx == skeletonIdx);
    resources.mMeshes[meshIdx].first.mSkeletonIdx = skeletonIdx;

    // As does model-exporter, we'll create anonymous child nodes for additional mesh( primitiv)es.
    while(++iPrim != mesh.mPrimitives.end())
    {
      std::unique_ptr<NodeDefinition> child{new NodeDefinition};
      child->mParentIdx = idx;

      auto childModel = MakeModelNode(*iPrim, cctx);

      ++meshIdx;
      childModel->mMeshIdx = meshIdx;

      child->mRenderable.reset(childModel);

      scene.AddNode(std::move(child));

      DALI_ASSERT_DEBUG(resources.mMeshes[meshIdx].first.mSkeletonIdx == INVALID_INDEX ||
                        resources.mMeshes[meshIdx].first.mSkeletonIdx == skeletonIdx);
      resources.mMeshes[meshIdx].first.mSkeletonIdx = skeletonIdx;
    }
  }

  if(node.mCamera)
  {
    CameraParameters camParams;
    ConvertCamera(*node.mCamera, camParams);

    camParams.matrix.SetTransformComponents(node.mScale, node.mRotation, node.mTranslation);
    output.mCameraParameters.push_back(camParams);
  }

  for(auto& n : node.mChildren)
  {
    ConvertNode(*n, n.GetIndex(), idx, cctx, isMRendererModel);
  }
}

void ConvertSceneNodes(const gt::Scene& scene, ConversionContext& cctx, bool isMRendererModel)
{
  auto& outScene = cctx.mOutput.mScene;
  Index rootIdx  = outScene.GetNodeCount();
  switch(scene.mNodes.size())
  {
    case 0:
      break;

    case 1:
      ConvertNode(*scene.mNodes[0], scene.mNodes[0].GetIndex(), INVALID_INDEX, cctx, isMRendererModel);
      outScene.AddRootNode(rootIdx);
      break;

    default:
    {
      std::unique_ptr<NodeDefinition> sceneRoot{new NodeDefinition()};
      sceneRoot->mName = "GLTF_LOADER_SCENE_ROOT_" + std::to_string(outScene.GetRoots().size());

      outScene.AddNode(std::move(sceneRoot));
      outScene.AddRootNode(rootIdx);

      for(auto& n : scene.mNodes)
      {
        ConvertNode(*n, n.GetIndex(), rootIdx, cctx, isMRendererModel);
      }
      break;
    }
  }
}

void ConvertNodes(const gt::Document& doc, ConversionContext& cctx, bool isMRendererModel)
{
  ConvertSceneNodes(*doc.mScene, cctx, isMRendererModel);

  for(uint32_t i = 0, i1 = doc.mScene.GetIndex(); i < i1; ++i)
  {
    ConvertSceneNodes(doc.mScenes[i], cctx, isMRendererModel);
  }

  for(uint32_t i = doc.mScene.GetIndex() + 1; i < doc.mScenes.size(); ++i)
  {
    ConvertSceneNodes(doc.mScenes[i], cctx, isMRendererModel);
  }
}

template<typename T>
void LoadDataFromAccessor(const std::string& path, Vector<T>& dataBuffer, uint32_t offset, uint32_t size)
{
  std::ifstream animationBinaryFile(path, std::ifstream::binary);

  if(!animationBinaryFile.is_open())
  {
    throw std::runtime_error("Failed to load " + path);
  }

  animationBinaryFile.seekg(offset);
  animationBinaryFile.read(reinterpret_cast<char*>(dataBuffer.Begin()), size);
  animationBinaryFile.close();
}

template<typename T>
float LoadDataFromAccessors(const std::string& path, const gltf2::Accessor& input, const gltf2::Accessor& output, Vector<float>& inputDataBuffer, Vector<T>& outputDataBuffer)
{
  inputDataBuffer.Resize(input.mCount);
  outputDataBuffer.Resize(output.mCount);

  const uint32_t inputDataBufferSize  = input.GetBytesLength();
  const uint32_t outputDataBufferSize = output.GetBytesLength();

  LoadDataFromAccessor<float>(path + std::string(input.mBufferView->mBuffer->mUri), inputDataBuffer, input.mBufferView->mByteOffset + input.mByteOffset, inputDataBufferSize);
  LoadDataFromAccessor<T>(path + std::string(output.mBufferView->mBuffer->mUri), outputDataBuffer, output.mBufferView->mByteOffset + output.mByteOffset, outputDataBufferSize);
  ApplyAccessorMinMax(output, reinterpret_cast<float*>(outputDataBuffer.begin()));

  return inputDataBuffer[input.mCount - 1u];
}

template<typename T>
float LoadKeyFrames(const std::string& path, const gt::Animation::Channel& channel, KeyFrames& keyFrames, gt::Animation::Channel::Target::Type type)
{
  const gltf2::Accessor& input  = *channel.mSampler->mInput;
  const gltf2::Accessor& output = *channel.mSampler->mOutput;

  Vector<float> inputDataBuffer;
  Vector<T>     outputDataBuffer;

  const float duration = LoadDataFromAccessors<T>(path, input, output, inputDataBuffer, outputDataBuffer);

  for(uint32_t i = 0; i < input.mCount; ++i)
  {
    keyFrames.Add(inputDataBuffer[i] / duration, outputDataBuffer[i]);
  }

  return duration;
}

float LoadBlendShapeKeyFrames(const std::string& path, const gt::Animation::Channel& channel, const std::string& nodeName, uint32_t& propertyIndex, std::vector<Dali::Scene3D::Loader::AnimatedProperty>& properties)
{
  const gltf2::Accessor& input  = *channel.mSampler->mInput;
  const gltf2::Accessor& output = *channel.mSampler->mOutput;

  Vector<float> inputDataBuffer;
  Vector<float> outputDataBuffer;

  const float duration = LoadDataFromAccessors<float>(path, input, output, inputDataBuffer, outputDataBuffer);

  char        weightNameBuffer[32];
  auto        prefixSize    = snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s[", BLEND_SHAPE_WEIGHTS_UNIFORM.c_str());
  char* const pWeightName   = weightNameBuffer + prefixSize;
  const auto  remainingSize = sizeof(weightNameBuffer) - prefixSize;
  for(uint32_t weightIndex = 0u, endWeightIndex = channel.mSampler->mOutput->mCount / channel.mSampler->mInput->mCount; weightIndex < endWeightIndex; ++weightIndex)
  {
    AnimatedProperty& animatedProperty = properties[propertyIndex++];

    animatedProperty.mNodeName = nodeName;
    snprintf(pWeightName, remainingSize, "%d]", weightIndex);
    animatedProperty.mPropertyName = std::string(weightNameBuffer);

    animatedProperty.mKeyFrames = KeyFrames::New();
    for(uint32_t i = 0; i < input.mCount; ++i)
    {
      animatedProperty.mKeyFrames.Add(inputDataBuffer[i] / duration, outputDataBuffer[i * endWeightIndex + weightIndex]);
    }

    animatedProperty.mTimePeriod = {0.f, duration};
  }

  return duration;
}

void ConvertAnimations(const gt::Document& doc, ConversionContext& cctx)
{
  auto& output = cctx.mOutput;

  output.mAnimationDefinitions.reserve(output.mAnimationDefinitions.size() + doc.mAnimations.size());

  for(const auto& animation : doc.mAnimations)
  {
    AnimationDefinition animationDef;

    if(!animation.mName.empty())
    {
      animationDef.mName = animation.mName;
    }

    uint32_t numberOfProperties = 0u;

    for(const auto& channel : animation.mChannels)
    {
      numberOfProperties += channel.mSampler->mOutput->mCount;
    }
    animationDef.mProperties.resize(numberOfProperties);

    Index propertyIndex = 0u;
    for(const auto& channel : animation.mChannels)
    {
      std::string nodeName;
      if(!channel.mTarget.mNode->mName.empty())
      {
        nodeName = channel.mTarget.mNode->mName;
      }
      else
      {
        Index index = cctx.mNodeIndices.GetRuntimeId(channel.mTarget.mNode.GetIndex());
        nodeName    = cctx.mOutput.mScene.GetNode(index)->mName;
      }

      float duration = 0.f;

      switch(channel.mTarget.mPath)
      {
        case gt::Animation::Channel::Target::TRANSLATION:
        {
          AnimatedProperty& animatedProperty = animationDef.mProperties[propertyIndex];

          animatedProperty.mNodeName     = nodeName;
          animatedProperty.mPropertyName = POSITION_PROPERTY;

          animatedProperty.mKeyFrames = KeyFrames::New();
          duration                    = LoadKeyFrames<Vector3>(cctx.mPath, channel, animatedProperty.mKeyFrames, channel.mTarget.mPath);

          animatedProperty.mTimePeriod = {0.f, duration};
          break;
        }
        case gt::Animation::Channel::Target::ROTATION:
        {
          AnimatedProperty& animatedProperty = animationDef.mProperties[propertyIndex];

          animatedProperty.mNodeName     = nodeName;
          animatedProperty.mPropertyName = ORIENTATION_PROPERTY;

          animatedProperty.mKeyFrames = KeyFrames::New();
          duration                    = LoadKeyFrames<Quaternion>(cctx.mPath, channel, animatedProperty.mKeyFrames, channel.mTarget.mPath);

          animatedProperty.mTimePeriod = {0.f, duration};
          break;
        }
        case gt::Animation::Channel::Target::SCALE:
        {
          AnimatedProperty& animatedProperty = animationDef.mProperties[propertyIndex];

          animatedProperty.mNodeName     = nodeName;
          animatedProperty.mPropertyName = SCALE_PROPERTY;

          animatedProperty.mKeyFrames = KeyFrames::New();
          duration                    = LoadKeyFrames<Vector3>(cctx.mPath, channel, animatedProperty.mKeyFrames, channel.mTarget.mPath);

          animatedProperty.mTimePeriod = {0.f, duration};
          break;
        }
        case gt::Animation::Channel::Target::WEIGHTS:
        {
          duration = LoadBlendShapeKeyFrames(cctx.mPath, channel, nodeName, propertyIndex, animationDef.mProperties);

          break;
        }
        default:
        {
          // nothing to animate.
          break;
        }
      }

      animationDef.mDuration = std::max(duration, animationDef.mDuration);

      ++propertyIndex;
    }

    output.mAnimationDefinitions.push_back(std::move(animationDef));
  }
}

void ProcessSkins(const gt::Document& doc, ConversionContext& cctx)
{
  // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#skininversebindmatrices
  // If an inverseBindMatrices accessor was provided, we'll load the joint data from the buffer,
  // otherwise we'll set identity matrices for inverse bind pose.
  struct IInverseBindMatrixProvider
  {
    virtual ~IInverseBindMatrixProvider()
    {
    }
    virtual void Provide(Matrix& ibm) = 0;
  };

  struct InverseBindMatrixAccessor : public IInverseBindMatrixProvider
  {
    std::ifstream  mStream;
    const uint32_t mElementSizeBytes;

    InverseBindMatrixAccessor(const gt::Accessor& accessor, const std::string& path)
    : mStream(path + std::string(accessor.mBufferView->mBuffer->mUri), std::ios::binary),
      mElementSizeBytes(accessor.GetElementSizeBytes())
    {
      DALI_ASSERT_ALWAYS(mStream);
      DALI_ASSERT_DEBUG(accessor.mType == gt::AccessorType::MAT4 && accessor.mComponentType == gt::Component::FLOAT);

      mStream.seekg(accessor.mBufferView->mByteOffset + accessor.mByteOffset);
    }

    virtual void Provide(Matrix& ibm) override
    {
      DALI_ASSERT_ALWAYS(mStream.read(reinterpret_cast<char*>(ibm.AsFloat()), mElementSizeBytes));
    }
  };

  struct DefaultInverseBindMatrixProvider : public IInverseBindMatrixProvider
  {
    virtual void Provide(Matrix& ibm) override
    {
      ibm = Matrix::IDENTITY;
    }
  };

  auto& resources = cctx.mOutput.mResources;
  resources.mSkeletons.reserve(doc.mSkins.size());

  for(auto& s : doc.mSkins)
  {
    std::unique_ptr<IInverseBindMatrixProvider> ibmProvider;
    if(s.mInverseBindMatrices)
    {
      ibmProvider.reset(new InverseBindMatrixAccessor(*s.mInverseBindMatrices, cctx.mPath));
    }
    else
    {
      ibmProvider.reset(new DefaultInverseBindMatrixProvider());
    }

    SkeletonDefinition skeleton;
    if(s.mSkeleton.GetIndex() != INVALID_INDEX)
    {
      skeleton.mRootNodeIdx = cctx.mNodeIndices.GetRuntimeId(s.mSkeleton.GetIndex());
    }

    skeleton.mJoints.resize(s.mJoints.size());
    auto iJoint = skeleton.mJoints.begin();
    for(auto& j : s.mJoints)
    {
      iJoint->mNodeIdx = cctx.mNodeIndices.GetRuntimeId(j.GetIndex());

      ibmProvider->Provide(iJoint->mInverseBindMatrix);

      ++iJoint;
    }

    resources.mSkeletons.push_back(std::move(skeleton));
  }
}

void ProduceShaders(ShaderDefinitionFactory& shaderFactory, SceneDefinition& scene)
{
  for(size_t i0 = 0, i1 = scene.GetNodeCount(); i0 != i1; ++i0)
  {
    auto nodeDef = scene.GetNode(i0);
    if(auto renderable = nodeDef->mRenderable.get())
    {
      renderable->mShaderIdx = shaderFactory.ProduceShader(*nodeDef);
    }
  }
}

void SetObjectReaders()
{
  js::SetObjectReader(BUFFER_READER);
  js::SetObjectReader(BUFFER_VIEW_READER);
  js::SetObjectReader(BUFFER_VIEW_CLIENT_READER);
  js::SetObjectReader(COMPONENT_TYPED_BUFFER_VIEW_CLIENT_READER);
  js::SetObjectReader(ACCESSOR_SPARSE_READER);
  js::SetObjectReader(ACCESSOR_READER);
  js::SetObjectReader(IMAGE_READER);
  js::SetObjectReader(SAMPLER_READER);
  js::SetObjectReader(TEXURE_READER);
  js::SetObjectReader(TEXURE_INFO_READER);
  js::SetObjectReader(MATERIAL_PBR_READER);
  js::SetObjectReader(MATERIAL_READER);
  js::SetObjectReader(MESH_PRIMITIVE_READER);
  js::SetObjectReader(MESH_READER);
  js::SetObjectReader(SKIN_READER);
  js::SetObjectReader(CAMERA_PERSPECTIVE_READER);
  js::SetObjectReader(CAMERA_ORTHOGRAPHIC_READER);
  js::SetObjectReader(CAMERA_READER);
  js::SetObjectReader(NODE_READER);
  js::SetObjectReader(ANIMATION_SAMPLER_READER);
  js::SetObjectReader(ANIMATION_TARGET_READER);
  js::SetObjectReader(ANIMATION_CHANNEL_READER);
  js::SetObjectReader(ANIMATION_READER);
  js::SetObjectReader(SCENE_READER);
}

void SetDefaultEnvironmentMap(const gt::Document& doc, ConversionContext& cctx)
{
  EnvironmentDefinition envDef;
  envDef.mUseBrdfTexture = true;
  envDef.mIblIntensity   = Scene3D::Loader::EnvironmentDefinition::GetDefaultIntensity();
  cctx.mOutput.mResources.mEnvironmentMaps.push_back({std::move(envDef), EnvironmentDefinition::Textures()});
}

} // namespace

void LoadGltfScene(const std::string& url, ShaderDefinitionFactory& shaderFactory, LoadResult& params)
{
  bool failed = false;
  auto js     = LoadTextFile(url.c_str(), &failed);
  if(failed)
  {
    throw std::runtime_error("Failed to load " + url);
  }

  json::unique_ptr root(json_parse(js.c_str(), js.size()));
  if(!root)
  {
    throw std::runtime_error("Failed to parse " + url);
  }

  static bool setObjectReaders = true;
  if(setObjectReaders)
  {
    // NOTE: only referencing own, anonymous namespace, const objects; the pointers will never need to change.
    SetObjectReaders();
    setObjectReaders = false;
  }

  gt::Document doc;

  auto& rootObj = js::Cast<json_object_s>(*root);
  auto  jsAsset = js::FindObjectChild("asset", rootObj);

  auto jsAssetVersion = js::FindObjectChild("version", js::Cast<json_object_s>(*jsAsset));
  if(jsAssetVersion)
  {
    doc.mAsset.mVersion = js::Read::StringView(*jsAssetVersion);
  }

  bool isMRendererModel(false);
  auto jsAssetGenerator = js::FindObjectChild("generator", js::Cast<json_object_s>(*jsAsset));
  if(jsAssetGenerator)
  {
    doc.mAsset.mGenerator = js::Read::StringView(*jsAssetGenerator);
    isMRendererModel      = (doc.mAsset.mGenerator.find(MRENDERER_MODEL_IDENTIFICATION) != std::string_view::npos);
  }

  gt::SetRefReaderObject(doc);
  DOCUMENT_READER.Read(rootObj, doc);

  auto              path = url.substr(0, url.rfind('/') + 1);
  ConversionContext cctx{params, path, INVALID_INDEX};

  ConvertMaterials(doc, cctx);
  ConvertMeshes(doc, cctx);
  ConvertNodes(doc, cctx, isMRendererModel);
  ConvertAnimations(doc, cctx);
  ProcessSkins(doc, cctx);
  ProduceShaders(shaderFactory, params.mScene);
  params.mScene.EnsureUniqueSkinningShaderInstances(params.mResources);

  // Set Default Environment map
  SetDefaultEnvironmentMap(doc, cctx);
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
