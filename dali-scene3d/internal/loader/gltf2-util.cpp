/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-scene3d/internal/loader/gltf2-util.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>
#include <limits> ///< for std::numeric_limits

using namespace Dali::Scene3D::Loader;

namespace Dali::Scene3D::Loader::Internal
{
namespace Gltf2Util
{
static constexpr std::string_view MRENDERER_MODEL_IDENTIFICATION = "M-Renderer";
static constexpr std::string_view POSITION_PROPERTY              = "position";
static constexpr std::string_view ORIENTATION_PROPERTY           = "orientation";
static constexpr std::string_view SCALE_PROPERTY                 = "scale";
static constexpr std::string_view BLEND_SHAPE_WEIGHTS_UNIFORM    = "uBlendShapeWeight";
static constexpr std::string_view ROOT_NODE_NAME                 = "RootNode";
static const Vector3              SCALE_TO_ADJUST(100.0f, 100.0f, 100.0f);

static const Geometry::Type GLTF2_TO_DALI_PRIMITIVES[]{
  Geometry::POINTS,
  Geometry::LINES,
  Geometry::LINE_LOOP,
  Geometry::LINE_STRIP,
  Geometry::TRIANGLES,
  Geometry::TRIANGLE_STRIP,
  Geometry::TRIANGLE_FAN}; //...because Dali swaps the last two.

static struct AttributeMapping
{
  gltf2::Attribute::Type   mType;
  MeshDefinition::Accessor MeshDefinition::*mAccessor;
  uint16_t                                  mElementSizeRequired;
} ATTRIBUTE_MAPPINGS[]{
  {gltf2::Attribute::NORMAL, &MeshDefinition::mNormals, sizeof(Vector3)},
  {gltf2::Attribute::TANGENT, &MeshDefinition::mTangents, sizeof(Vector3)},
  {gltf2::Attribute::TEXCOORD_0, &MeshDefinition::mTexCoords, sizeof(Vector2)},
  {gltf2::Attribute::COLOR_0, &MeshDefinition::mColors, sizeof(Vector4)},
  {gltf2::Attribute::JOINTS_0, &MeshDefinition::mJoints0, sizeof(Vector4)},
  {gltf2::Attribute::WEIGHTS_0, &MeshDefinition::mWeights0, sizeof(Vector4)},
};

std::vector<gltf2::Animation> ReadAnimationArray(const json_value_s& j)
{
  auto results = json::Read::Array<gltf2::Animation, json::ObjectReader<gltf2::Animation>::Read>(j);

  for(auto& animation : results)
  {
    for(auto& channel : animation.mChannels)
    {
      channel.mSampler.UpdateVector(animation.mSamplers);
    }
  }

  return results;
}

void ApplyAccessorMinMax(const gltf2::Accessor& accessor, float* values)
{
  DALI_ASSERT_ALWAYS(accessor.mMax.empty() || gltf2::AccessorType::ElementCount(accessor.mType) == accessor.mMax.size());
  DALI_ASSERT_ALWAYS(accessor.mMin.empty() || gltf2::AccessorType::ElementCount(accessor.mType) == accessor.mMin.size());
  MeshDefinition::Blob::ApplyMinMax(accessor.mMin, accessor.mMax, accessor.mCount, values);
}

const json::Reader<gltf2::Buffer>& GetBufferReader()
{
  static const auto BUFFER_READER = std::move(json::Reader<gltf2::Buffer>()
                                                .Register(*json::MakeProperty("byteLength", json::Read::Number<uint32_t>, &gltf2::Buffer::mByteLength))
                                                .Register(*json::MakeProperty("uri", json::Read::StringView, &gltf2::Buffer::mUri)));
  return BUFFER_READER;
}

const json::Reader<gltf2::BufferView>& GetBufferViewReader()
{
  static const auto BUFFER_VIEW_READER = std::move(json::Reader<gltf2::BufferView>()
                                                     .Register(*json::MakeProperty("buffer", gltf2::RefReader<gltf2::Document>::Read<gltf2::Buffer, &gltf2::Document::mBuffers>, &gltf2::BufferView::mBuffer))
                                                     .Register(*json::MakeProperty("byteOffset", json::Read::Number<uint32_t>, &gltf2::BufferView::mByteOffset))
                                                     .Register(*json::MakeProperty("byteLength", json::Read::Number<uint32_t>, &gltf2::BufferView::mByteLength))
                                                     .Register(*json::MakeProperty("byteStride", json::Read::Number<uint32_t>, &gltf2::BufferView::mByteStride))
                                                     .Register(*json::MakeProperty("target", json::Read::Number<uint32_t>, &gltf2::BufferView::mTarget)));
  return BUFFER_VIEW_READER;
}

const json::Reader<gltf2::BufferViewClient>& GetBufferViewClientReader()
{
  static const auto BUFFER_VIEW_CLIENT_READER = std::move(json::Reader<gltf2::BufferViewClient>()
                                                            .Register(*json::MakeProperty("bufferView", gltf2::RefReader<gltf2::Document>::Read<gltf2::BufferView, &gltf2::Document::mBufferViews>, &gltf2::BufferViewClient::mBufferView))
                                                            .Register(*json::MakeProperty("byteOffset", json::Read::Number<uint32_t>, &gltf2::BufferViewClient::mByteOffset)));
  return BUFFER_VIEW_CLIENT_READER;
}

const json::Reader<gltf2::ComponentTypedBufferViewClient>& GetComponentTypedBufferViewClientReader()
{
  static const auto COMPONENT_TYPED_BUFFER_VIEW_CLIENT_READER = std::move(json::Reader<gltf2::ComponentTypedBufferViewClient>()
                                                                            .Register(*new json::Property<gltf2::ComponentTypedBufferViewClient, gltf2::Ref<gltf2::BufferView>>("bufferView", gltf2::RefReader<gltf2::Document>::Read<gltf2::BufferView, &gltf2::Document::mBufferViews>, &gltf2::ComponentTypedBufferViewClient::mBufferView))
                                                                            .Register(*new json::Property<gltf2::ComponentTypedBufferViewClient, uint32_t>("byteOffset", json::Read::Number<uint32_t>, &gltf2::ComponentTypedBufferViewClient::mByteOffset))
                                                                            .Register(*json::MakeProperty("componentType", json::Read::Enum<gltf2::Component::Type>, &gltf2::ComponentTypedBufferViewClient::mComponentType)));
  return COMPONENT_TYPED_BUFFER_VIEW_CLIENT_READER;
}

const json::Reader<gltf2::Accessor::Sparse>& GetAccessorSparseReader()
{
  static const auto ACCESSOR_SPARSE_READER = std::move(json::Reader<gltf2::Accessor::Sparse>()
                                                         .Register(*json::MakeProperty("count", json::Read::Number<uint32_t>, &gltf2::Accessor::Sparse::mCount))
                                                         .Register(*json::MakeProperty("indices", json::ObjectReader<gltf2::ComponentTypedBufferViewClient>::Read, &gltf2::Accessor::Sparse::mIndices))
                                                         .Register(*json::MakeProperty("values", json::ObjectReader<gltf2::BufferViewClient>::Read, &gltf2::Accessor::Sparse::mValues)));
  return ACCESSOR_SPARSE_READER;
}

const json::Reader<gltf2::Accessor>& GetAccessorReader()
{
  static const auto ACCESSOR_READER = std::move(json::Reader<gltf2::Accessor>()
                                                  .Register(*new json::Property<gltf2::Accessor, gltf2::Ref<gltf2::BufferView>>("bufferView",
                                                                                                                                gltf2::RefReader<gltf2::Document>::Read<gltf2::BufferView, &gltf2::Document::mBufferViews>,
                                                                                                                                &gltf2::Accessor::mBufferView))
                                                  .Register(*new json::Property<gltf2::Accessor, uint32_t>("byteOffset",
                                                                                                           json::Read::Number<uint32_t>,
                                                                                                           &gltf2::Accessor::mByteOffset))
                                                  .Register(*new json::Property<gltf2::Accessor, gltf2::Component::Type>("componentType",
                                                                                                                         json::Read::Enum<gltf2::Component::Type>,
                                                                                                                         &gltf2::Accessor::mComponentType))
                                                  .Register(*new json::Property<gltf2::Accessor, std::string_view>("name", json::Read::StringView, &gltf2::Accessor::mName))
                                                  .Register(*json::MakeProperty("count", json::Read::Number<uint32_t>, &gltf2::Accessor::mCount))
                                                  .Register(*json::MakeProperty("normalized", json::Read::Boolean, &gltf2::Accessor::mNormalized))
                                                  .Register(*json::MakeProperty("type", gltf2::ReadStringEnum<gltf2::AccessorType>, &gltf2::Accessor::mType))
                                                  .Register(*json::MakeProperty("min", json::Read::Array<float, json::Read::Number>, &gltf2::Accessor::mMin))
                                                  .Register(*json::MakeProperty("max", json::Read::Array<float, json::Read::Number>, &gltf2::Accessor::mMax))
                                                  .Register(*new json::Property<gltf2::Accessor, gltf2::Accessor::Sparse>("sparse", json::ObjectReader<gltf2::Accessor::Sparse>::Read, &gltf2::Accessor::SetSparse)));
  return ACCESSOR_READER;
}

const json::Reader<gltf2::Image>& GetImageReader()
{
  static const auto IMAGE_READER = std::move(json::Reader<gltf2::Image>()
                                               .Register(*new json::Property<gltf2::Image, std::string_view>("name", json::Read::StringView, &gltf2::Material::mName))
                                               .Register(*json::MakeProperty("uri", json::Read::StringView, &gltf2::Image::mUri))
                                               .Register(*json::MakeProperty("mimeType", json::Read::StringView, &gltf2::Image::mMimeType))
                                               .Register(*json::MakeProperty("bufferView", gltf2::RefReader<gltf2::Document>::Read<gltf2::BufferView, &gltf2::Document::mBufferViews>, &gltf2::Image::mBufferView)));
  return IMAGE_READER;
}

const json::Reader<gltf2::Sampler>& GetSamplerReader()
{
  static const auto SAMPLER_READER = std::move(json::Reader<gltf2::Sampler>()
                                                 .Register(*json::MakeProperty("minFilter", json::Read::Enum<gltf2::Filter::Type>, &gltf2::Sampler::mMinFilter))
                                                 .Register(*json::MakeProperty("magFilter", json::Read::Enum<gltf2::Filter::Type>, &gltf2::Sampler::mMagFilter))
                                                 .Register(*json::MakeProperty("wrapS", json::Read::Enum<gltf2::Wrap::Type>, &gltf2::Sampler::mWrapS))
                                                 .Register(*json::MakeProperty("wrapT", json::Read::Enum<gltf2::Wrap::Type>, &gltf2::Sampler::mWrapT)));
  return SAMPLER_READER;
}

const json::Reader<gltf2::Texture>& GetTextureReader()
{
  static const auto TEXURE_READER = std::move(json::Reader<gltf2::Texture>()
                                                .Register(*json::MakeProperty("source", gltf2::RefReader<gltf2::Document>::Read<gltf2::Image, &gltf2::Document::mImages>, &gltf2::Texture::mSource))
                                                .Register(*json::MakeProperty("sampler", gltf2::RefReader<gltf2::Document>::Read<gltf2::Sampler, &gltf2::Document::mSamplers>, &gltf2::Texture::mSampler)));
  return TEXURE_READER;
}

const json::Reader<gltf2::TextureInfo>& GetTextureInfoReader()
{
  static const auto TEXURE_INFO_READER = std::move(json::Reader<gltf2::TextureInfo>()
                                                     .Register(*json::MakeProperty("index", gltf2::RefReader<gltf2::Document>::Read<gltf2::Texture, &gltf2::Document::mTextures>, &gltf2::TextureInfo::mTexture))
                                                     .Register(*json::MakeProperty("texCoord", json::Read::Number<uint32_t>, &gltf2::TextureInfo::mTexCoord))
                                                     .Register(*json::MakeProperty("scale", json::Read::Number<float>, &gltf2::TextureInfo::mScale))
                                                     .Register(*json::MakeProperty("strength", json::Read::Number<float>, &gltf2::TextureInfo::mStrength)));
  return TEXURE_INFO_READER;
}

const json::Reader<gltf2::Material::Pbr>& GetMaterialPbrReader()
{
  static const auto MATERIAL_PBR_READER = std::move(json::Reader<gltf2::Material::Pbr>()
                                                      .Register(*json::MakeProperty("baseColorFactor", gltf2::ReadDaliVector<Vector4>, &gltf2::Material::Pbr::mBaseColorFactor))
                                                      .Register(*json::MakeProperty("baseColorTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::Material::Pbr::mBaseColorTexture))
                                                      .Register(*json::MakeProperty("metallicFactor", json::Read::Number<float>, &gltf2::Material::Pbr::mMetallicFactor))
                                                      .Register(*json::MakeProperty("roughnessFactor", json::Read::Number<float>, &gltf2::Material::Pbr::mRoughnessFactor))
                                                      .Register(*json::MakeProperty("metallicRoughnessTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::Material::Pbr::mMetallicRoughnessTexture)));
  return MATERIAL_PBR_READER;
}

const json::Reader<gltf2::MaterialSpecular>& GetMaterialSpecularReader()
{
  static const auto MATERIAL_SPECULAR_READER = std::move(json::Reader<gltf2::MaterialSpecular>()
                                                           .Register(*json::MakeProperty("specularFactor", json::Read::Number<float>, &gltf2::MaterialSpecular::mSpecularFactor))
                                                           .Register(*json::MakeProperty("specularTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::MaterialSpecular::mSpecularTexture))
                                                           .Register(*json::MakeProperty("specularColorFactor", gltf2::ReadDaliVector<Vector3>, &gltf2::MaterialSpecular::mSpecularColorFactor))
                                                           .Register(*json::MakeProperty("specularColorTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::MaterialSpecular::mSpecularColorTexture)));
  return MATERIAL_SPECULAR_READER;
}

const json::Reader<gltf2::MaterialIor>& GetMaterialIorReader()
{
  static const auto MATERIAL_IOR_READER = std::move(json::Reader<gltf2::MaterialIor>()
                                                      .Register(*json::MakeProperty("ior", json::Read::Number<float>, &gltf2::MaterialIor::mIor)));
  return MATERIAL_IOR_READER;
}

const json::Reader<gltf2::MaterialExtensions>& GetMaterialExtensionsReader()
{
  static const auto MATERIAL_EXTENSION_READER = std::move(json::Reader<gltf2::MaterialExtensions>()
                                                            .Register(*json::MakeProperty("KHR_materials_ior", json::ObjectReader<gltf2::MaterialIor>::Read, &gltf2::MaterialExtensions::mMaterialIor))
                                                            .Register(*json::MakeProperty("KHR_materials_specular", json::ObjectReader<gltf2::MaterialSpecular>::Read, &gltf2::MaterialExtensions::mMaterialSpecular)));
  return MATERIAL_EXTENSION_READER;
}

const json::Reader<gltf2::Material>& GetMaterialReader()
{
  static const auto MATERIAL_READER = std::move(json::Reader<gltf2::Material>()
                                                  .Register(*new json::Property<gltf2::Material, std::string_view>("name", json::Read::StringView, &gltf2::Material::mName))
                                                  .Register(*json::MakeProperty("pbrMetallicRoughness", json::ObjectReader<gltf2::Material::Pbr>::Read, &gltf2::Material::mPbrMetallicRoughness))
                                                  .Register(*json::MakeProperty("normalTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::Material::mNormalTexture))
                                                  .Register(*json::MakeProperty("occlusionTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::Material::mOcclusionTexture))
                                                  .Register(*json::MakeProperty("emissiveTexture", json::ObjectReader<gltf2::TextureInfo>::Read, &gltf2::Material::mEmissiveTexture))
                                                  .Register(*json::MakeProperty("emissiveFactor", gltf2::ReadDaliVector<Vector3>, &gltf2::Material::mEmissiveFactor))
                                                  .Register(*json::MakeProperty("alphaMode", gltf2::ReadStringEnum<gltf2::AlphaMode>, &gltf2::Material::mAlphaMode))
                                                  .Register(*json::MakeProperty("alphaCutoff", json::Read::Number<float>, &gltf2::Material::mAlphaCutoff))
                                                  .Register(*json::MakeProperty("doubleSided", json::Read::Boolean, &gltf2::Material::mDoubleSided))
                                                  .Register(*json::MakeProperty("extensions", json::ObjectReader<gltf2::MaterialExtensions>::Read, &gltf2::Material::mMaterialExtensions)));
  return MATERIAL_READER;
}

std::map<gltf2::Attribute::Type, gltf2::Ref<gltf2::Accessor>> ReadMeshPrimitiveAttributes(const json_value_s& j)
{
  auto&                                                         jsonObject = json::Cast<json_object_s>(j);
  std::map<gltf2::Attribute::Type, gltf2::Ref<gltf2::Accessor>> result;

  auto element = jsonObject.start;
  while(element)
  {
    auto jsonString                                                                 = *element->name;
    result[gltf2::Attribute::FromString(jsonString.string, jsonString.string_size)] = gltf2::RefReader<gltf2::Document>::Read<gltf2::Accessor, &gltf2::Document::mAccessors>(*element->value);
    element                                                                         = element->next;
  }
  return result;
}

std::vector<std::map<gltf2::Attribute::Type, gltf2::Ref<gltf2::Accessor>>> ReadMeshPrimitiveTargets(const json_value_s& j)
{
  auto&                                                                      jsonObject = json::Cast<json_array_s>(j);
  std::vector<std::map<gltf2::Attribute::Type, gltf2::Ref<gltf2::Accessor>>> result;

  result.reserve(jsonObject.length);

  auto element = jsonObject.start;
  while(element)
  {
    result.push_back(std::move(ReadMeshPrimitiveAttributes(*element->value)));
    element = element->next;
  }

  return result;
}

const json::Reader<gltf2::Mesh::Primitive>& GetMeshPrimitiveReader()
{
  static const auto MESH_PRIMITIVE_READER = std::move(json::Reader<gltf2::Mesh::Primitive>()
                                                        .Register(*json::MakeProperty("attributes", ReadMeshPrimitiveAttributes, &gltf2::Mesh::Primitive::mAttributes))
                                                        .Register(*json::MakeProperty("indices", gltf2::RefReader<gltf2::Document>::Read<gltf2::Accessor, &gltf2::Document::mAccessors>, &gltf2::Mesh::Primitive::mIndices))
                                                        .Register(*json::MakeProperty("material", gltf2::RefReader<gltf2::Document>::Read<gltf2::Material, &gltf2::Document::mMaterials>, &gltf2::Mesh::Primitive::mMaterial))
                                                        .Register(*json::MakeProperty("mode", json::Read::Enum<gltf2::Mesh::Primitive::Mode>, &gltf2::Mesh::Primitive::mMode))
                                                        .Register(*json::MakeProperty("targets", ReadMeshPrimitiveTargets, &gltf2::Mesh::Primitive::mTargets)));
  return MESH_PRIMITIVE_READER;
}

const json::Reader<gltf2::Mesh::Extras>& GetMeshExtrasReader()
{
  static const auto MESH_EXTRAS_READER = std::move(json::Reader<gltf2::Mesh::Extras>()
                                                     .Register(*json::MakeProperty("targetNames", json::Read::Array<std::string_view, json::Read::StringView>, &gltf2::Mesh::Extras::mTargetNames)));
  return MESH_EXTRAS_READER;
}

std::vector<std::string_view> ReadMeshExtensionsTargetsName(const json_value_s& j)
{
  auto&                         jsonObject = json::Cast<json_object_s>(j);
  std::vector<std::string_view> result;

  auto element = jsonObject.start;
  while(element)
  {
    auto     jsonString = *element->name;
    uint32_t index      = json::Read::Number<uint32_t>(*element->value);

    if(result.size() <= index)
    {
      DALI_ASSERT_ALWAYS(index < std::numeric_limits<uint32_t>::max());
      result.resize(index + 1u);
    }

    result[index] = json::Read::StringView(jsonString);

    element = element->next;
  }
  return result;
}

const json::Reader<gltf2::Mesh::Extensions>& GetMeshExtensionsReader()
{
  static const auto MESH_EXTENSIONS_READER = std::move(json::Reader<gltf2::Mesh::Extensions>()
                                                         .Register(*json::MakeProperty("SXR_targets_names", ReadMeshExtensionsTargetsName, &gltf2::Mesh::Extensions::mSXRTargetsNames))
                                                         .Register(*json::MakeProperty("avatar_shape_names", ReadMeshExtensionsTargetsName, &gltf2::Mesh::Extensions::mAvatarShapeNames)));
  return MESH_EXTENSIONS_READER;
}

const json::Reader<gltf2::Mesh>& GetMeshReader()
{
  static const auto MESH_READER = std::move(json::Reader<gltf2::Mesh>()
                                              .Register(*new json::Property<gltf2::Mesh, std::string_view>("name", json::Read::StringView, &gltf2::Mesh::mName))
                                              .Register(*json::MakeProperty("primitives",
                                                                            json::Read::Array<gltf2::Mesh::Primitive, json::ObjectReader<gltf2::Mesh::Primitive>::Read>,
                                                                            &gltf2::Mesh::mPrimitives))
                                              .Register(*json::MakeProperty("weights", json::Read::Array<float, json::Read::Number>, &gltf2::Mesh::mWeights))
                                              .Register(*json::MakeProperty("extras", json::ObjectReader<gltf2::Mesh::Extras>::Read, &gltf2::Mesh::mExtras))
                                              .Register(*json::MakeProperty("extensions", json::ObjectReader<gltf2::Mesh::Extensions>::Read, &gltf2::Mesh::mExtensions)));
  return MESH_READER;
}

const json::Reader<gltf2::Skin>& GetSkinReader()
{
  static const auto SKIN_READER = std::move(json::Reader<gltf2::Skin>()
                                              .Register(*new json::Property<gltf2::Skin, std::string_view>("name", json::Read::StringView, &gltf2::Skin::mName))
                                              .Register(*json::MakeProperty("inverseBindMatrices",
                                                                            gltf2::RefReader<gltf2::Document>::Read<gltf2::Accessor, &gltf2::Document::mAccessors>,
                                                                            &gltf2::Skin::mInverseBindMatrices))
                                              .Register(*json::MakeProperty("skeleton",
                                                                            gltf2::RefReader<gltf2::Document>::Read<gltf2::Node, &gltf2::Document::mNodes>,
                                                                            &gltf2::Skin::mSkeleton))
                                              .Register(*json::MakeProperty("joints",
                                                                            json::Read::Array<gltf2::Ref<gltf2::Node>, gltf2::RefReader<gltf2::Document>::Read<gltf2::Node, &gltf2::Document::mNodes>>,
                                                                            &gltf2::Skin::mJoints)));
  return SKIN_READER;
}

const json::Reader<gltf2::Camera::Perspective>& GetCameraPerspectiveReader()
{
  static const auto CAMERA_PERSPECTIVE_READER = std::move(json::Reader<gltf2::Camera::Perspective>()
                                                            .Register(*json::MakeProperty("aspectRatio", json::Read::Number<float>, &gltf2::Camera::Perspective::mAspectRatio))
                                                            .Register(*json::MakeProperty("yfov", json::Read::Number<float>, &gltf2::Camera::Perspective::mYFov))
                                                            .Register(*json::MakeProperty("zfar", json::Read::Number<float>, &gltf2::Camera::Perspective::mZFar))
                                                            .Register(*json::MakeProperty("znear", json::Read::Number<float>, &gltf2::Camera::Perspective::mZNear))); // TODO: infinite perspective projection, where znear is omitted
  return CAMERA_PERSPECTIVE_READER;
}

const json::Reader<gltf2::Camera::Orthographic>& GetCameraOrthographicReader()
{
  static const auto CAMERA_ORTHOGRAPHIC_READER = std::move(json::Reader<gltf2::Camera::Orthographic>()
                                                             .Register(*json::MakeProperty("xmag", json::Read::Number<float>, &gltf2::Camera::Orthographic::mXMag))
                                                             .Register(*json::MakeProperty("ymag", json::Read::Number<float>, &gltf2::Camera::Orthographic::mYMag))
                                                             .Register(*json::MakeProperty("zfar", json::Read::Number<float>, &gltf2::Camera::Orthographic::mZFar))
                                                             .Register(*json::MakeProperty("znear", json::Read::Number<float>, &gltf2::Camera::Orthographic::mZNear)));
  return CAMERA_ORTHOGRAPHIC_READER;
}

const json::Reader<gltf2::Camera>& GetCameraReader()
{
  static const auto CAMERA_READER = std::move(json::Reader<gltf2::Camera>()
                                                .Register(*new json::Property<gltf2::Camera, std::string_view>("name", json::Read::StringView, &gltf2::Camera::mName))
                                                .Register(*json::MakeProperty("type", json::Read::StringView, &gltf2::Camera::mType))
                                                .Register(*json::MakeProperty("perspective", json::ObjectReader<gltf2::Camera::Perspective>::Read, &gltf2::Camera::mPerspective))
                                                .Register(*json::MakeProperty("orthographic", json::ObjectReader<gltf2::Camera::Orthographic>::Read, &gltf2::Camera::mOrthographic)));
  return CAMERA_READER;
}

const json::Reader<gltf2::Node>& GetNodeReader()
{
  static const auto NODE_READER = std::move(json::Reader<gltf2::Node>()
                                              .Register(*new json::Property<gltf2::Node, std::string_view>("name", json::Read::StringView, &gltf2::Node::mName))
                                              .Register(*json::MakeProperty("translation", gltf2::ReadDaliVector<Vector3>, &gltf2::Node::mTranslation))
                                              .Register(*json::MakeProperty("rotation", gltf2::ReadQuaternion, &gltf2::Node::mRotation))
                                              .Register(*json::MakeProperty("scale", gltf2::ReadDaliVector<Vector3>, &gltf2::Node::mScale))
                                              .Register(*new json::Property<gltf2::Node, Matrix>("matrix", gltf2::ReadDaliVector<Matrix>, &gltf2::Node::SetMatrix))
                                              .Register(*json::MakeProperty("camera", gltf2::RefReader<gltf2::Document>::Read<gltf2::Camera, &gltf2::Document::mCameras>, &gltf2::Node::mCamera))
                                              .Register(*json::MakeProperty("children", json::Read::Array<gltf2::Ref<gltf2::Node>, gltf2::RefReader<gltf2::Document>::Read<gltf2::Node, &gltf2::Document::mNodes>>, &gltf2::Node::mChildren))
                                              .Register(*json::MakeProperty("mesh", gltf2::RefReader<gltf2::Document>::Read<gltf2::Mesh, &gltf2::Document::mMeshes>, &gltf2::Node::mMesh))
                                              .Register(*json::MakeProperty("skin", gltf2::RefReader<gltf2::Document>::Read<gltf2::Skin, &gltf2::Document::mSkins>, &gltf2::Node::mSkin)));
  return NODE_READER;
}

const json::Reader<gltf2::Animation::Sampler>& GetAnimationSamplerReader()
{
  static const auto ANIMATION_SAMPLER_READER = std::move(json::Reader<gltf2::Animation::Sampler>()
                                                           .Register(*json::MakeProperty("input", gltf2::RefReader<gltf2::Document>::Read<gltf2::Accessor, &gltf2::Document::mAccessors>, &gltf2::Animation::Sampler::mInput))
                                                           .Register(*json::MakeProperty("output", gltf2::RefReader<gltf2::Document>::Read<gltf2::Accessor, &gltf2::Document::mAccessors>, &gltf2::Animation::Sampler::mOutput))
                                                           .Register(*json::MakeProperty("interpolation", gltf2::ReadStringEnum<gltf2::Animation::Sampler::Interpolation>, &gltf2::Animation::Sampler::mInterpolation)));
  return ANIMATION_SAMPLER_READER;
}

const json::Reader<gltf2::Animation::Channel::Target>& GetAnimationChannelTargetReader()
{
  static const auto ANIMATION_TARGET_READER = std::move(json::Reader<gltf2::Animation::Channel::Target>()
                                                          .Register(*json::MakeProperty("node", gltf2::RefReader<gltf2::Document>::Read<gltf2::Node, &gltf2::Document::mNodes>, &gltf2::Animation::Channel::Target::mNode))
                                                          .Register(*json::MakeProperty("path", gltf2::ReadStringEnum<gltf2::Animation::Channel::Target>, &gltf2::Animation::Channel::Target::mPath)));
  return ANIMATION_TARGET_READER;
}

const json::Reader<gltf2::Animation::Channel>& GetAnimationChannelReader()
{
  static const auto ANIMATION_CHANNEL_READER = std::move(json::Reader<gltf2::Animation::Channel>()
                                                           .Register(*json::MakeProperty("target", json::ObjectReader<gltf2::Animation::Channel::Target>::Read, &gltf2::Animation::Channel::mTarget))
                                                           .Register(*json::MakeProperty("sampler", gltf2::RefReader<gltf2::Animation>::Read<gltf2::Animation::Sampler, &gltf2::Animation::mSamplers>, &gltf2::Animation::Channel::mSampler)));
  return ANIMATION_CHANNEL_READER;
}

const json::Reader<gltf2::Animation>& GetAnimationReader()
{
  static const auto ANIMATION_READER = std::move(json::Reader<gltf2::Animation>()
                                                   .Register(*new json::Property<gltf2::Animation, std::string_view>("name", json::Read::StringView, &gltf2::Animation::mName))
                                                   .Register(*json::MakeProperty("samplers",
                                                                                 json::Read::Array<gltf2::Animation::Sampler, json::ObjectReader<gltf2::Animation::Sampler>::Read>,
                                                                                 &gltf2::Animation::mSamplers))
                                                   .Register(*json::MakeProperty("channels",
                                                                                 json::Read::Array<gltf2::Animation::Channel, json::ObjectReader<gltf2::Animation::Channel>::Read>,
                                                                                 &gltf2::Animation::mChannels)));
  return ANIMATION_READER;
}

const json::Reader<gltf2::Scene>& GetSceneReader()
{
  static const auto SCENE_READER = std::move(json::Reader<gltf2::Scene>()
                                               .Register(*new json::Property<gltf2::Scene, std::string_view>("name", json::Read::StringView, &gltf2::Scene::mName))
                                               .Register(*json::MakeProperty("nodes",
                                                                             json::Read::Array<gltf2::Ref<gltf2::Node>, gltf2::RefReader<gltf2::Document>::Read<gltf2::Node, &gltf2::Document::mNodes>>,
                                                                             &gltf2::Scene::mNodes)));
  return SCENE_READER;
}

const json::Reader<gltf2::Document>& GetDocumentReader()
{
  static const auto DOCUMENT_READER = std::move(json::Reader<gltf2::Document>()
                                                  .Register(*json::MakeProperty("buffers",
                                                                                json::Read::Array<gltf2::Buffer, json::ObjectReader<gltf2::Buffer>::Read>,
                                                                                &gltf2::Document::mBuffers))
                                                  .Register(*json::MakeProperty("bufferViews",
                                                                                json::Read::Array<gltf2::BufferView, json::ObjectReader<gltf2::BufferView>::Read>,
                                                                                &gltf2::Document::mBufferViews))
                                                  .Register(*json::MakeProperty("accessors",
                                                                                json::Read::Array<gltf2::Accessor, json::ObjectReader<gltf2::Accessor>::Read>,
                                                                                &gltf2::Document::mAccessors))
                                                  .Register(*json::MakeProperty("images",
                                                                                json::Read::Array<gltf2::Image, json::ObjectReader<gltf2::Image>::Read>,
                                                                                &gltf2::Document::mImages))
                                                  .Register(*json::MakeProperty("samplers",
                                                                                json::Read::Array<gltf2::Sampler, json::ObjectReader<gltf2::Sampler>::Read>,
                                                                                &gltf2::Document::mSamplers))
                                                  .Register(*json::MakeProperty("textures",
                                                                                json::Read::Array<gltf2::Texture, json::ObjectReader<gltf2::Texture>::Read>,
                                                                                &gltf2::Document::mTextures))
                                                  .Register(*json::MakeProperty("materials",
                                                                                json::Read::Array<gltf2::Material, json::ObjectReader<gltf2::Material>::Read>,
                                                                                &gltf2::Document::mMaterials))
                                                  .Register(*json::MakeProperty("meshes",
                                                                                json::Read::Array<gltf2::Mesh, json::ObjectReader<gltf2::Mesh>::Read>,
                                                                                &gltf2::Document::mMeshes))
                                                  .Register(*json::MakeProperty("skins",
                                                                                json::Read::Array<gltf2::Skin, json::ObjectReader<gltf2::Skin>::Read>,
                                                                                &gltf2::Document::mSkins))
                                                  .Register(*json::MakeProperty("cameras",
                                                                                json::Read::Array<gltf2::Camera, json::ObjectReader<gltf2::Camera>::Read>,
                                                                                &gltf2::Document::mCameras))
                                                  .Register(*json::MakeProperty("nodes",
                                                                                json::Read::Array<gltf2::Node, json::ObjectReader<gltf2::Node>::Read>,
                                                                                &gltf2::Document::mNodes))
                                                  .Register(*json::MakeProperty("animations",
                                                                                ReadAnimationArray,
                                                                                &gltf2::Document::mAnimations))
                                                  .Register(*json::MakeProperty("scenes",
                                                                                json::Read::Array<gltf2::Scene, json::ObjectReader<gltf2::Scene>::Read>,
                                                                                &gltf2::Document::mScenes))
                                                  .Register(*json::MakeProperty("scene", gltf2::RefReader<gltf2::Document>::Read<gltf2::Scene, &gltf2::Document::mScenes>, &gltf2::Document::mScene)));
  return DOCUMENT_READER;
}

void ConvertBuffer(const gltf2::Buffer& buffer, decltype(ResourceBundle::mBuffers)& outBuffers, const std::string& resourcePath)
{
  BufferDefinition bufferDefinition;

  bufferDefinition.mResourcePath = resourcePath;
  bufferDefinition.mUri          = buffer.mUri;
  bufferDefinition.mByteLength   = buffer.mByteLength;

  outBuffers.emplace_back(std::move(bufferDefinition));
}

void ConvertBuffers(const gltf2::Document& document, ConversionContext& context)
{
  auto& outBuffers = context.mOutput.mResources.mBuffers;
  outBuffers.reserve(document.mBuffers.size());

  for(auto& buffer : document.mBuffers)
  {
    if(buffer.mUri.empty())
    {
      continue;
    }
    ConvertBuffer(buffer, outBuffers, context.mPath);
  }
}

SamplerFlags::Type ConvertWrapMode(gltf2::Wrap::Type wrapMode)
{
  switch(wrapMode)
  {
    case gltf2::Wrap::REPEAT:
      return SamplerFlags::WRAP_REPEAT;
    case gltf2::Wrap::CLAMP_TO_EDGE:
      return SamplerFlags::WRAP_CLAMP;
    case gltf2::Wrap::MIRRORED_REPEAT:
      return SamplerFlags::WRAP_MIRROR;
    default:
      throw std::runtime_error("Invalid wrap type.");
  }
}

SamplerFlags::Type ConvertSampler(const gltf2::Ref<gltf2::Sampler>& sampler)
{
  if(sampler)
  {
    return ((sampler->mMinFilter < gltf2::Filter::NEAREST_MIPMAP_NEAREST) ? (sampler->mMinFilter - gltf2::Filter::NEAREST) : ((sampler->mMinFilter - gltf2::Filter::NEAREST_MIPMAP_NEAREST) + 2)) |
           ((sampler->mMagFilter - gltf2::Filter::NEAREST) << SamplerFlags::FILTER_MAG_SHIFT) |
           (ConvertWrapMode(sampler->mWrapS) << SamplerFlags::WRAP_S_SHIFT) |
           (ConvertWrapMode(sampler->mWrapT) << SamplerFlags::WRAP_T_SHIFT);
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

TextureDefinition ConvertTextureInfo(const gltf2::TextureInfo& textureInfo, ConversionContext& context, const ImageMetadata& metaData = ImageMetadata())
{
  TextureDefinition textureDefinition;
  std::string       uri = std::string(textureInfo.mTexture->mSource->mUri);
  if(uri.empty())
  {
    uint32_t bufferIndex = textureInfo.mTexture->mSource->mBufferView->mBuffer.GetIndex();
    if(bufferIndex != INVALID_INDEX && context.mOutput.mResources.mBuffers[bufferIndex].IsAvailable())
    {
      auto& stream = context.mOutput.mResources.mBuffers[bufferIndex].GetBufferStream();
      stream.clear();
      stream.seekg(textureInfo.mTexture->mSource->mBufferView->mByteOffset, stream.beg);
      std::vector<uint8_t> dataBuffer;
      dataBuffer.resize(textureInfo.mTexture->mSource->mBufferView->mByteLength);
      stream.read(reinterpret_cast<char*>(dataBuffer.data()), static_cast<std::streamsize>(static_cast<size_t>(textureInfo.mTexture->mSource->mBufferView->mByteLength)));
      return TextureDefinition{std::move(dataBuffer), ConvertSampler(textureInfo.mTexture->mSampler), metaData.mMinSize, metaData.mSamplingMode};
    }
    return TextureDefinition();
  }
  else
  {
    return TextureDefinition{uri, ConvertSampler(textureInfo.mTexture->mSampler), metaData.mMinSize, metaData.mSamplingMode};
  }
}

void AddTextureStage(uint32_t semantic, MaterialDefinition& materialDefinition, gltf2::TextureInfo textureInfo, const Dali::Scene3D::Loader::ImageMetadata& metaData, ConversionContext& context)
{
  materialDefinition.mTextureStages.push_back({semantic, ConvertTextureInfo(textureInfo, context, metaData)});
  materialDefinition.mFlags |= semantic;
}

void ConvertMaterial(const gltf2::Material& material, const std::unordered_map<std::string, ImageMetadata>& imageMetaData, decltype(ResourceBundle::mMaterials)& outMaterials, ConversionContext& context)
{
  auto getTextureMetaData = [](const std::unordered_map<std::string, ImageMetadata>& metaData, const gltf2::TextureInfo& info) {
    if(!info.mTexture->mSource->mUri.empty())
    {
      if(auto search = metaData.find(info.mTexture->mSource->mUri.data()); search != metaData.end())
      {
        return search->second;
      }
    }
    return ImageMetadata();
  };

  MaterialDefinition materialDefinition;

  materialDefinition.mFlags |= MaterialDefinition::GLTF_CHANNELS;
  materialDefinition.mShadowAvailable = true;

  auto& pbr = material.mPbrMetallicRoughness;
  if(material.mAlphaMode == gltf2::AlphaMode::BLEND)
  {
    materialDefinition.mAlphaModeType = Scene3D::Material::AlphaModeType::BLEND;
    materialDefinition.mIsOpaque      = false;
    materialDefinition.mFlags |= MaterialDefinition::TRANSPARENCY;
  }
  else if(material.mAlphaMode == gltf2::AlphaMode::MASK)
  {
    materialDefinition.mAlphaModeType = Scene3D::Material::AlphaModeType::MASK;
    materialDefinition.mIsMask        = true;
    materialDefinition.SetAlphaCutoff(std::min(1.f, std::max(0.f, material.mAlphaCutoff)));
  }

  materialDefinition.mBaseColorFactor = pbr.mBaseColorFactor;

  materialDefinition.mTextureStages.reserve(!!pbr.mBaseColorTexture + !!pbr.mMetallicRoughnessTexture + !!material.mNormalTexture + !!material.mOcclusionTexture + !!material.mEmissiveTexture);
  if(pbr.mBaseColorTexture)
  {
    AddTextureStage(MaterialDefinition::ALBEDO, materialDefinition, pbr.mBaseColorTexture, getTextureMetaData(imageMetaData, pbr.mBaseColorTexture), context);
  }
  else
  {
    materialDefinition.mNeedAlbedoTexture = false;
  }

  materialDefinition.mMetallic  = pbr.mMetallicFactor;
  materialDefinition.mRoughness = pbr.mRoughnessFactor;

  if(pbr.mMetallicRoughnessTexture)
  {
    AddTextureStage(MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS,
                    materialDefinition,
                    pbr.mMetallicRoughnessTexture,
                    getTextureMetaData(imageMetaData, pbr.mMetallicRoughnessTexture),
                    context);
  }
  else
  {
    materialDefinition.mNeedMetallicRoughnessTexture = false;
  }

  materialDefinition.mNormalScale = material.mNormalTexture.mScale;
  if(material.mNormalTexture)
  {
    AddTextureStage(MaterialDefinition::NORMAL, materialDefinition, material.mNormalTexture, getTextureMetaData(imageMetaData, material.mNormalTexture), context);
  }
  else
  {
    materialDefinition.mNeedNormalTexture = false;
  }

  if(material.mOcclusionTexture)
  {
    AddTextureStage(MaterialDefinition::OCCLUSION, materialDefinition, material.mOcclusionTexture, getTextureMetaData(imageMetaData, material.mOcclusionTexture), context);
    materialDefinition.mOcclusionStrength = material.mOcclusionTexture.mStrength;
  }

  materialDefinition.mEmissiveFactor = material.mEmissiveFactor;
  if(material.mEmissiveTexture)
  {
    AddTextureStage(MaterialDefinition::EMISSIVE, materialDefinition, material.mEmissiveTexture, getTextureMetaData(imageMetaData, material.mEmissiveTexture), context);
  }

  if(!Dali::Equals(material.mMaterialExtensions.mMaterialIor.mIor, gltf2::UNDEFINED_FLOAT_VALUE))
  {
    materialDefinition.mIor                = material.mMaterialExtensions.mMaterialIor.mIor;
    materialDefinition.mDielectricSpecular = powf((materialDefinition.mIor - 1.0f) / (materialDefinition.mIor + 1.0f), 2.0f);
  }
  materialDefinition.mSpecularFactor      = material.mMaterialExtensions.mMaterialSpecular.mSpecularFactor;
  materialDefinition.mSpecularColorFactor = material.mMaterialExtensions.mMaterialSpecular.mSpecularColorFactor;

  if(material.mMaterialExtensions.mMaterialSpecular.mSpecularTexture)
  {
    AddTextureStage(MaterialDefinition::SPECULAR, materialDefinition, material.mMaterialExtensions.mMaterialSpecular.mSpecularTexture, getTextureMetaData(imageMetaData, material.mMaterialExtensions.mMaterialSpecular.mSpecularTexture), context);
  }

  if(material.mMaterialExtensions.mMaterialSpecular.mSpecularColorTexture)
  {
    AddTextureStage(MaterialDefinition::SPECULAR_COLOR, materialDefinition, material.mMaterialExtensions.mMaterialSpecular.mSpecularColorTexture, getTextureMetaData(imageMetaData, material.mMaterialExtensions.mMaterialSpecular.mSpecularColorTexture), context);
  }

  materialDefinition.mDoubleSided = material.mDoubleSided;

  outMaterials.emplace_back(std::move(materialDefinition), TextureSet());
}

void ConvertMaterials(const gltf2::Document& document, ConversionContext& context)
{
  auto& imageMetaData = context.mOutput.mSceneMetadata.mImageMetadata;

  auto& outMaterials = context.mOutput.mResources.mMaterials;
  outMaterials.reserve(document.mMaterials.size());

  for(auto& material : document.mMaterials)
  {
    ConvertMaterial(material, imageMetaData, outMaterials, context);
  }
}

MeshDefinition::Accessor ConvertMeshPrimitiveAccessor(const gltf2::Accessor& accessor)
{
  DALI_ASSERT_ALWAYS((accessor.mBufferView &&
                      (accessor.mBufferView->mByteStride < std::numeric_limits<uint16_t>::max())) ||
                     (accessor.mSparse && !accessor.mBufferView));

  DALI_ASSERT_ALWAYS(!accessor.mSparse ||
                     ((accessor.mSparse->mIndices.mBufferView && (accessor.mSparse->mIndices.mBufferView->mByteStride < std::numeric_limits<uint16_t>::max())) &&
                      (accessor.mSparse->mValues.mBufferView && (accessor.mSparse->mValues.mBufferView->mByteStride < std::numeric_limits<uint16_t>::max()))));

  MeshDefinition::SparseBlob sparseBlob;
  if(accessor.mSparse)
  {
    const gltf2::Accessor::Sparse&               sparse  = *accessor.mSparse;
    const gltf2::ComponentTypedBufferViewClient& indices = sparse.mIndices;
    const gltf2::BufferViewClient&               values  = sparse.mValues;

    MeshDefinition::Blob indicesBlob(
      indices.mBufferView->mByteOffset + indices.mByteOffset,
      sparse.mCount * indices.GetBytesPerComponent(),
      static_cast<uint16_t>(indices.mBufferView->mByteStride),
      static_cast<uint16_t>(indices.GetBytesPerComponent()),
      {},
      {});
    MeshDefinition::Blob valuesBlob(
      values.mBufferView->mByteOffset + values.mByteOffset,
      sparse.mCount * accessor.GetElementSizeBytes(),
      static_cast<uint16_t>(values.mBufferView->mByteStride),
      static_cast<uint16_t>(accessor.GetElementSizeBytes()),
      {},
      {});

    sparseBlob = std::move(MeshDefinition::SparseBlob(std::move(indicesBlob), std::move(valuesBlob), accessor.mSparse->mCount));
  }

  uint32_t bufferViewOffset = 0u;
  uint32_t bufferViewStride = 0u;
  if(accessor.mBufferView)
  {
    bufferViewOffset = accessor.mBufferView->mByteOffset;
    bufferViewStride = accessor.mBufferView->mByteStride;
  }

  return MeshDefinition::Accessor{
    std::move(MeshDefinition::Blob{bufferViewOffset + accessor.mByteOffset,
                                   accessor.GetBytesLength(),
                                   static_cast<uint16_t>(bufferViewStride),
                                   static_cast<uint16_t>(accessor.GetElementSizeBytes()),
                                   accessor.mMin,
                                   accessor.mMax}),
    std::move(sparseBlob),
    accessor.mBufferView ? accessor.mBufferView->mBuffer.GetIndex() : 0};
}

void ConvertMeshes(const gltf2::Document& document, ConversionContext& context)
{
  uint32_t meshCount = 0;
  context.mMeshIds.reserve(document.mMeshes.size());
  for(auto& mesh : document.mMeshes)
  {
    context.mMeshIds.push_back(meshCount);
    meshCount += mesh.mPrimitives.size();
  }

  auto& outMeshes = context.mOutput.mResources.mMeshes;
  outMeshes.reserve(meshCount);
  for(auto& mesh : document.mMeshes)
  {
    for(auto& primitive : mesh.mPrimitives)
    {
      MeshDefinition meshDefinition;

      auto& attribs                 = primitive.mAttributes;
      meshDefinition.mPrimitiveType = GLTF2_TO_DALI_PRIMITIVES[primitive.mMode];

      auto positionIter = attribs.find(gltf2::Attribute::POSITION);

      if(positionIter == attribs.end())
      {
        DALI_LOG_ERROR("Primitive mesh dosn't have POSITION atrributes!");
        continue;
      }

      auto& accPositions        = *positionIter->second;
      meshDefinition.mPositions = ConvertMeshPrimitiveAccessor(accPositions);
      // glTF2 support vector4 tangent for mesh.
      // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#meshes-overview
      meshDefinition.mTangentType = Property::VECTOR4;

      const bool needNormalsTangents = accPositions.mType == gltf2::AccessorType::VEC3;
      for(auto& attributeMapping : ATTRIBUTE_MAPPINGS)
      {
        auto iFind = attribs.find(attributeMapping.mType);
        if(iFind != attribs.end())
        {
          auto& accessor = meshDefinition.*(attributeMapping.mAccessor);
          accessor       = ConvertMeshPrimitiveAccessor(*iFind->second);

          if(iFind->first == gltf2::Attribute::JOINTS_0)
          {
            meshDefinition.mFlags |= (iFind->second->mComponentType == gltf2::Component::UNSIGNED_SHORT) * MeshDefinition::U16_JOINT_IDS;
            meshDefinition.mFlags |= (iFind->second->mComponentType == gltf2::Component::UNSIGNED_BYTE) * MeshDefinition::U8_JOINT_IDS;
            DALI_ASSERT_DEBUG(MaskMatch(meshDefinition.mFlags, MeshDefinition::U16_JOINT_IDS) || MaskMatch(meshDefinition.mFlags, MeshDefinition::U8_JOINT_IDS) || iFind->second->mComponentType == gltf2::Component::FLOAT);
          }
          if(iFind->first == gltf2::Attribute::WEIGHTS_0)
          {
            meshDefinition.mFlags |= (iFind->second->mComponentType == gltf2::Component::UNSIGNED_SHORT) * MeshDefinition::U16_WEIGHT;
            meshDefinition.mFlags |= (iFind->second->mComponentType == gltf2::Component::UNSIGNED_BYTE) * MeshDefinition::U8_WEIGHT;
            DALI_ASSERT_DEBUG(MaskMatch(meshDefinition.mFlags, MeshDefinition::U16_WEIGHT) || MaskMatch(meshDefinition.mFlags, MeshDefinition::U8_WEIGHT) || iFind->second->mComponentType == gltf2::Component::FLOAT);
          }
        }
        else if(needNormalsTangents)
        {
          switch(attributeMapping.mType)
          {
            case gltf2::Attribute::NORMAL:
              meshDefinition.RequestNormals();
              break;

            case gltf2::Attribute::TANGENT:
              meshDefinition.RequestTangents();
              break;

            default:
              break;
          }
        }
      }

      if(primitive.mIndices)
      {
        meshDefinition.mIndices = ConvertMeshPrimitiveAccessor(*primitive.mIndices);
        meshDefinition.mFlags |= (primitive.mIndices->mComponentType == gltf2::Component::UNSIGNED_INT) * MeshDefinition::U32_INDICES;
        meshDefinition.mFlags |= (primitive.mIndices->mComponentType == gltf2::Component::UNSIGNED_BYTE) * MeshDefinition::U8_INDICES;
        DALI_ASSERT_DEBUG(MaskMatch(meshDefinition.mFlags, MeshDefinition::U32_INDICES) || MaskMatch(meshDefinition.mFlags, MeshDefinition::U8_INDICES) || primitive.mIndices->mComponentType == gltf2::Component::UNSIGNED_SHORT);
      }

      if(!primitive.mTargets.empty())
      {
        meshDefinition.mBlendShapes.reserve(primitive.mTargets.size());
        meshDefinition.mBlendShapeVersion = BlendShapes::Version::VERSION_2_0;
        uint32_t blendShapeIndex          = 0u;
        for(const auto& target : primitive.mTargets)
        {
          MeshDefinition::BlendShape blendShape;

          auto endIt = target.end();
          auto it    = target.find(gltf2::Attribute::POSITION);
          if(it != endIt)
          {
            blendShape.deltas = ConvertMeshPrimitiveAccessor(*it->second);
          }
          it = target.find(gltf2::Attribute::NORMAL);
          if(it != endIt)
          {
            blendShape.normals = ConvertMeshPrimitiveAccessor(*it->second);
          }
          it = target.find(gltf2::Attribute::TANGENT);
          if(it != endIt)
          {
            blendShape.tangents = ConvertMeshPrimitiveAccessor(*it->second);
          }

          if(!mesh.mWeights.empty())
          {
            blendShape.weight = mesh.mWeights[meshDefinition.mBlendShapes.size()];
          }

          // Get blendshape name from extras / SXR_targets_names / avatar_shape_names.
          if(blendShapeIndex < mesh.mExtras.mTargetNames.size())
          {
            blendShape.name = mesh.mExtras.mTargetNames[blendShapeIndex];
          }
          else if(blendShapeIndex < mesh.mExtensions.mSXRTargetsNames.size())
          {
            blendShape.name = mesh.mExtensions.mSXRTargetsNames[blendShapeIndex];
          }
          else if(blendShapeIndex < mesh.mExtensions.mAvatarShapeNames.size())
          {
            blendShape.name = mesh.mExtensions.mAvatarShapeNames[blendShapeIndex];
          }

          meshDefinition.mBlendShapes.push_back(std::move(blendShape));
          ++blendShapeIndex;
        }
      }

      outMeshes.push_back({std::move(meshDefinition), MeshGeometry{}});
    }
  }
}

ModelRenderable* MakeModelRenderable(const gltf2::Mesh::Primitive& primitive, ConversionContext& context)
{
  auto modelRenderable = new ModelRenderable();

  auto materialIdx = primitive.mMaterial.GetIndex();
  if(INVALID_INDEX == materialIdx)
  {
    // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#default-material
    if(INVALID_INDEX == context.mDefaultMaterial)
    {
      auto& outMaterials       = context.mOutput.mResources.mMaterials;
      context.mDefaultMaterial = outMaterials.size();

      ConvertMaterial(gltf2::Material{}, context.mOutput.mSceneMetadata.mImageMetadata, outMaterials, context);
    }

    materialIdx = context.mDefaultMaterial;
  }

  modelRenderable->mMaterialIdx = materialIdx;

  return modelRenderable;
}

void ConvertCamera(const gltf2::Camera& camera, CameraParameters& cameraParameters)
{
  cameraParameters.isPerspective = camera.mType.compare("perspective") == 0;
  if(cameraParameters.isPerspective)
  {
    auto& perspective = camera.mPerspective;
    if(!Dali::Equals(perspective.mYFov, gltf2::UNDEFINED_FLOAT_VALUE))
    {
      cameraParameters.yFovDegree = Degree(Radian(perspective.mYFov));
    }
    else
    {
      cameraParameters.yFovDegree = Degree(gltf2::UNDEFINED_FLOAT_VALUE);
    }
    cameraParameters.zNear = perspective.mZNear;
    cameraParameters.zFar  = perspective.mZFar;
    // TODO: yes, we seem to ignore aspectRatio in CameraParameters.
  }
  else
  {
    auto& ortho = camera.mOrthographic;
    if(!Dali::Equals(ortho.mYMag, gltf2::UNDEFINED_FLOAT_VALUE) && !Dali::Equals(ortho.mXMag, gltf2::UNDEFINED_FLOAT_VALUE))
    {
      cameraParameters.orthographicSize = ortho.mYMag * .5f;
      cameraParameters.aspectRatio      = ortho.mXMag / ortho.mYMag;
    }
    else
    {
      cameraParameters.orthographicSize = gltf2::UNDEFINED_FLOAT_VALUE;
      cameraParameters.aspectRatio      = gltf2::UNDEFINED_FLOAT_VALUE;
    }
    cameraParameters.zNear = ortho.mZNear;
    cameraParameters.zFar  = ortho.mZFar;
  }
}

void ConvertNode(gltf2::Node const& node, const Index gltfIndex, Index parentIndex, ConversionContext& context, bool isMRendererModel)
{
  auto& output    = context.mOutput;
  auto& scene     = output.mScene;
  auto& resources = output.mResources;

  const auto index    = scene.GetNodeCount();
  auto       weakNode = scene.AddNode([&]() {
    std::unique_ptr<NodeDefinition> nodeDefinition{new NodeDefinition()};

    nodeDefinition->mParentIdx = parentIndex;
    nodeDefinition->mName      = node.mName;
    if(nodeDefinition->mName.empty())
    {
      // TODO: Production quality generation of unique names.
      nodeDefinition->mName = std::to_string(reinterpret_cast<uintptr_t>(nodeDefinition.get()));
    }

    if(!node.mSkin) // Nodes with skinned meshes are not supposed to have local transforms.
    {
      nodeDefinition->mPosition    = node.mTranslation;
      nodeDefinition->mOrientation = node.mRotation;
      nodeDefinition->mScale       = node.mScale;

      if(isMRendererModel && node.mName == ROOT_NODE_NAME && node.mScale == SCALE_TO_ADJUST)
      {
        nodeDefinition->mScale *= 0.01f;
      }
    }

    return nodeDefinition; }());
  if(!weakNode)
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Node name '" << node.mName << "' is not unique; scene is invalid.";
  }

  context.mNodeIndices.RegisterMapping(gltfIndex, index);

  Index skeletonIdx = node.mSkin ? node.mSkin.GetIndex() : INVALID_INDEX;
  if(node.mMesh)
  {
    auto&    mesh           = *node.mMesh;
    uint32_t primitiveCount = mesh.mPrimitives.size();
    auto     meshIndex      = context.mMeshIds[node.mMesh.GetIndex()];
    weakNode->mRenderables.reserve(primitiveCount);
    for(uint32_t i = 0; i < primitiveCount; ++i)
    {
      std::unique_ptr<NodeDefinition::Renderable> renderable;
      auto                                        modelRenderable = MakeModelRenderable(mesh.mPrimitives[i], context);
      modelRenderable->mMeshIdx                                   = meshIndex + i;

      DALI_ASSERT_DEBUG(resources.mMeshes[modelRenderable->mMeshIdx].first.mSkeletonIdx == INVALID_INDEX ||
                        resources.mMeshes[modelRenderable->mMeshIdx].first.mSkeletonIdx == skeletonIdx);
      resources.mMeshes[modelRenderable->mMeshIdx].first.mSkeletonIdx = skeletonIdx;

      renderable.reset(modelRenderable);
      weakNode->mRenderables.push_back(std::move(renderable));
    }
  }

  if(node.mCamera)
  {
    CameraParameters cameraParameters;
    ConvertCamera(*node.mCamera, cameraParameters);

    cameraParameters.matrix.SetTransformComponents(node.mScale, node.mRotation, node.mTranslation);
    output.mCameraParameters.push_back(cameraParameters);
  }

  for(auto& child : node.mChildren)
  {
    ConvertNode(*child, child.GetIndex(), index, context, isMRendererModel);
  }
}

void ConvertSceneNodes(const gltf2::Scene& scene, ConversionContext& context, bool isMRendererModel)
{
  auto& outScene  = context.mOutput.mScene;
  Index rootIndex = outScene.GetNodeCount();
  switch(scene.mNodes.size())
  {
    case 0:
      break;

    case 1:
      ConvertNode(*scene.mNodes[0], scene.mNodes[0].GetIndex(), INVALID_INDEX, context, isMRendererModel);
      outScene.AddRootNode(rootIndex);
      break;

    default:
    {
      std::unique_ptr<NodeDefinition> sceneRoot{new NodeDefinition()};
      sceneRoot->mName = "GLTF_LOADER_SCENE_ROOT_" + std::to_string(outScene.GetRoots().size());

      outScene.AddNode(std::move(sceneRoot));
      outScene.AddRootNode(rootIndex);

      for(auto& node : scene.mNodes)
      {
        ConvertNode(*node, node.GetIndex(), rootIndex, context, isMRendererModel);
      }
      break;
    }
  }
}

void ConvertNodes(const gltf2::Document& document, ConversionContext& context, bool isMRendererModel)
{
  if(!document.mScenes.empty())
  {
    uint32_t rootSceneIndex = 0u;
    if(document.mScene)
    {
      rootSceneIndex = document.mScene.GetIndex();
    }
    ConvertSceneNodes(document.mScenes[rootSceneIndex], context, isMRendererModel);

    for(uint32_t i = 0; i < rootSceneIndex; ++i)
    {
      ConvertSceneNodes(document.mScenes[i], context, isMRendererModel);
    }

    for(uint32_t i = rootSceneIndex + 1; i < document.mScenes.size(); ++i)
    {
      ConvertSceneNodes(document.mScenes[i], context, isMRendererModel);
    }
  }
}

template<typename T>
void LoadDataFromAccessor(ConversionContext& context, uint32_t bufferIndex, Vector<T>& dataBuffer, uint32_t offset, uint32_t size)
{
  if(bufferIndex >= context.mOutput.mResources.mBuffers.size())
  {
    DALI_LOG_ERROR("Invailid buffer index\n");
    return;
  }

  auto& buffer = context.mOutput.mResources.mBuffers[bufferIndex];
  if(!buffer.IsAvailable())
  {
    DALI_LOG_ERROR("Failed to load from buffer stream.\n");
  }
  auto& stream = buffer.GetBufferStream();
  stream.clear();
  stream.seekg(offset, stream.beg);
  stream.read(reinterpret_cast<char*>(dataBuffer.Begin()), static_cast<std::streamsize>(static_cast<size_t>(size)));
}

template<typename T>
float LoadDataFromAccessors(ConversionContext& context, const gltf2::Accessor& input, const gltf2::Accessor& output, Vector<float>& inputDataBuffer, Vector<T>& outputDataBuffer)
{
  inputDataBuffer.Resize(input.mCount);
  outputDataBuffer.Resize(output.mCount);

  const uint32_t inputDataBufferSize  = input.GetBytesLength();
  const uint32_t outputDataBufferSize = output.GetBytesLength();

  LoadDataFromAccessor<float>(context, output.mBufferView->mBuffer.GetIndex(), inputDataBuffer, input.mBufferView->mByteOffset + input.mByteOffset, inputDataBufferSize);
  LoadDataFromAccessor<T>(context, output.mBufferView->mBuffer.GetIndex(), outputDataBuffer, output.mBufferView->mByteOffset + output.mByteOffset, outputDataBufferSize);
  ApplyAccessorMinMax(input, reinterpret_cast<float*>(inputDataBuffer.begin()));
  ApplyAccessorMinMax(output, reinterpret_cast<float*>(outputDataBuffer.begin()));

  return inputDataBuffer[input.mCount - 1u];
}

bool IsFirstFrameValueEmpty(const uint32_t inputCount, const Vector<float>& inputBuffer)
{
  return (inputCount > 0 && !Dali::EqualsZero(inputBuffer[0]));
}

template<typename T>
float LoadKeyFrames(ConversionContext& context, const gltf2::Animation::Channel& channel, KeyFrames& keyFrames, gltf2::Animation::Channel::Target::Type type)
{
  const gltf2::Accessor& input  = *channel.mSampler->mInput;
  const gltf2::Accessor& output = *channel.mSampler->mOutput;

  Vector<float> inputDataBuffer;
  Vector<T>     outputDataBuffer;

  const float duration = std::max(LoadDataFromAccessors<T>(context, input, output, inputDataBuffer, outputDataBuffer), AnimationDefinition::MIN_DURATION_SECONDS);

  if(IsFirstFrameValueEmpty(input.mCount, inputDataBuffer))
  {
    keyFrames.Add(0.0f, outputDataBuffer[0]);
  }

  for(uint32_t i = 0; i < input.mCount; ++i)
  {
    keyFrames.Add(inputDataBuffer[i] / duration, outputDataBuffer[i]);
  }

  return duration;
}

float LoadBlendShapeKeyFrames(ConversionContext& context, const gltf2::Animation::Channel& channel, Index nodeIndex, uint32_t& propertyIndex, AnimationDefinition& animationDefinition)
{
  const gltf2::Accessor& input  = *channel.mSampler->mInput;
  const gltf2::Accessor& output = *channel.mSampler->mOutput;

  Vector<float> inputDataBuffer;
  Vector<float> outputDataBuffer;

  const float duration = std::max(LoadDataFromAccessors<float>(context, input, output, inputDataBuffer, outputDataBuffer), AnimationDefinition::MIN_DURATION_SECONDS);

  char        weightNameBuffer[32];
  auto        prefixSize    = snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s[", BLEND_SHAPE_WEIGHTS_UNIFORM.data());
  char* const pWeightName   = weightNameBuffer + prefixSize;
  const auto  remainingSize = sizeof(weightNameBuffer) - prefixSize;
  for(uint32_t weightIndex = 0u, endWeightIndex = channel.mSampler->mOutput->mCount / channel.mSampler->mInput->mCount; weightIndex < endWeightIndex; ++weightIndex)
  {
    AnimatedProperty animatedProperty;

    animatedProperty.mNodeIndex = nodeIndex;
    snprintf(pWeightName, remainingSize, "%d]", weightIndex);
    animatedProperty.mPropertyName = std::string(weightNameBuffer);

    animatedProperty.mKeyFrames = KeyFrames::New();

    if(IsFirstFrameValueEmpty(input.mCount, inputDataBuffer))
    {
      animatedProperty.mKeyFrames.Add(0.0f, outputDataBuffer[weightIndex]);
    }

    for(uint32_t i = 0; i < input.mCount; ++i)
    {
      animatedProperty.mKeyFrames.Add(inputDataBuffer[i] / duration, outputDataBuffer[i * endWeightIndex + weightIndex]);
    }

    animatedProperty.mTimePeriod = {0.f, duration};

    animationDefinition.SetProperty(propertyIndex++, std::move(animatedProperty));
  }

  return duration;
}

template<typename T>
float LoadAnimation(AnimationDefinition& animationDefinition, Index nodeIndex, Index propertyIndex, const std::string& propertyName, const gltf2::Animation::Channel& channel, ConversionContext& context)
{
  AnimatedProperty animatedProperty;
  animatedProperty.mNodeIndex    = nodeIndex;
  animatedProperty.mPropertyName = propertyName;

  animatedProperty.mKeyFrames  = KeyFrames::New();
  float duration               = LoadKeyFrames<T>(context, channel, animatedProperty.mKeyFrames, channel.mTarget.mPath);
  animatedProperty.mTimePeriod = {0.f, duration};

  animationDefinition.SetProperty(propertyIndex, std::move(animatedProperty));
  return duration;
}

void ConvertAnimations(const gltf2::Document& document, ConversionContext& context)
{
  auto& output = context.mOutput;

  output.mAnimationDefinitions.reserve(output.mAnimationDefinitions.size() + document.mAnimations.size());

  for(const auto& animation : document.mAnimations)
  {
    AnimationDefinition animationDefinition;

    if(!animation.mName.empty())
    {
      animationDefinition.SetName(animation.mName.data());
    }

    uint32_t numberOfProperties = 0u;
    for(const auto& channel : animation.mChannels)
    {
      if(channel.mTarget.mPath == gltf2::Animation::Channel::Target::WEIGHTS)
      {
        numberOfProperties += channel.mSampler->mOutput->mCount / channel.mSampler->mInput->mCount;
      }
      else
      {
        numberOfProperties++;
      }
    }
    animationDefinition.ReserveSize(numberOfProperties);

    Index propertyIndex = 0u;
    for(const auto& channel : animation.mChannels)
    {
      Index nodeIndex = context.mNodeIndices.GetRuntimeId(channel.mTarget.mNode.GetIndex());
      float duration  = 0.f;

      switch(channel.mTarget.mPath)
      {
        case gltf2::Animation::Channel::Target::TRANSLATION:
        {
          duration = LoadAnimation<Vector3>(animationDefinition, nodeIndex, propertyIndex, POSITION_PROPERTY.data(), channel, context);
          break;
        }
        case gltf2::Animation::Channel::Target::ROTATION:
        {
          duration = LoadAnimation<Quaternion>(animationDefinition, nodeIndex, propertyIndex, ORIENTATION_PROPERTY.data(), channel, context);
          break;
        }
        case gltf2::Animation::Channel::Target::SCALE:
        {
          duration = LoadAnimation<Vector3>(animationDefinition, nodeIndex, propertyIndex, SCALE_PROPERTY.data(), channel, context);
          break;
        }
        case gltf2::Animation::Channel::Target::WEIGHTS:
        {
          duration = LoadBlendShapeKeyFrames(context, channel, nodeIndex, propertyIndex, animationDefinition);

          break;
        }
        default:
        {
          // nothing to animate.
          break;
        }
      }

      animationDefinition.SetDuration(std::max(duration, animationDefinition.GetDuration()));

      ++propertyIndex;
    }

    output.mAnimationDefinitions.push_back(std::move(animationDefinition));
  }
}

void ProcessSkins(const gltf2::Document& document, ConversionContext& context)
{
  // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#skininversebindmatrices
  // If an inverseBindMatrices accessor was provided, we'll load the joint data from the buffer,
  // otherwise we'll set identity matrices for inverse bind pose.
  struct IInverseBindMatrixProvider
  {
    virtual ~IInverseBindMatrixProvider()
    {
    }
    virtual void Provide(Matrix& inverseBindMatrix) = 0;
  };

  struct InverseBindMatrixAccessor : public IInverseBindMatrixProvider
  {
    std::istream&  mStream;
    const uint32_t mElementSizeBytes;

    InverseBindMatrixAccessor(const gltf2::Accessor& accessor, ConversionContext& context)
    : mStream(context.mOutput.mResources.mBuffers[accessor.mBufferView->mBuffer.GetIndex()].GetBufferStream()),
      mElementSizeBytes(accessor.GetElementSizeBytes())
    {
      DALI_ASSERT_DEBUG(accessor.mType == gltf2::AccessorType::MAT4 && accessor.mComponentType == gltf2::Component::FLOAT);

      if(!mStream.rdbuf()->in_avail())
      {
        DALI_LOG_ERROR("Failed to load from stream\n");
      }
      mStream.clear();
      mStream.seekg(accessor.mBufferView->mByteOffset + accessor.mByteOffset, mStream.beg);
    }

    virtual void Provide(Matrix& inverseBindMatrix) override
    {
      DALI_ASSERT_ALWAYS(mStream.read(reinterpret_cast<char*>(inverseBindMatrix.AsFloat()), static_cast<std::streamsize>(static_cast<size_t>(mElementSizeBytes))));
    }
  };

  struct DefaultInverseBindMatrixProvider : public IInverseBindMatrixProvider
  {
    virtual void Provide(Matrix& inverseBindMatrix) override
    {
      inverseBindMatrix = Matrix::IDENTITY;
    }
  };

  auto& resources = context.mOutput.mResources;
  resources.mSkeletons.reserve(document.mSkins.size());

  for(auto& skin : document.mSkins)
  {
    std::unique_ptr<IInverseBindMatrixProvider> inverseBindMatrixProvider;
    if(skin.mInverseBindMatrices)
    {
      inverseBindMatrixProvider.reset(new InverseBindMatrixAccessor(*skin.mInverseBindMatrices, context));
    }
    else
    {
      inverseBindMatrixProvider.reset(new DefaultInverseBindMatrixProvider());
    }

    SkeletonDefinition skeleton;
    if(skin.mSkeleton.GetIndex() != INVALID_INDEX)
    {
      skeleton.mRootNodeIdx = context.mNodeIndices.GetRuntimeId(skin.mSkeleton.GetIndex());
    }

    skeleton.mJoints.resize(skin.mJoints.size());
    auto iJoint = skeleton.mJoints.begin();
    for(auto& joint : skin.mJoints)
    {
      iJoint->mNodeIdx = context.mNodeIndices.GetRuntimeId(joint.GetIndex());

      inverseBindMatrixProvider->Provide(iJoint->mInverseBindMatrix);

      ++iJoint;
    }

    resources.mSkeletons.push_back(std::move(skeleton));
  }
}

void SetObjectReaders()
{
  json::SetObjectReader(GetBufferReader());
  json::SetObjectReader(GetBufferViewReader());
  json::SetObjectReader(GetBufferViewClientReader());
  json::SetObjectReader(GetComponentTypedBufferViewClientReader());
  json::SetObjectReader(GetAccessorSparseReader());
  json::SetObjectReader(GetAccessorReader());
  json::SetObjectReader(GetImageReader());
  json::SetObjectReader(GetSamplerReader());
  json::SetObjectReader(GetTextureReader());
  json::SetObjectReader(GetTextureInfoReader());
  json::SetObjectReader(GetMaterialPbrReader());
  json::SetObjectReader(GetMaterialSpecularReader());
  json::SetObjectReader(GetMaterialIorReader());
  json::SetObjectReader(GetMaterialExtensionsReader());
  json::SetObjectReader(GetMaterialReader());
  json::SetObjectReader(GetMeshPrimitiveReader());
  json::SetObjectReader(GetMeshExtrasReader());
  json::SetObjectReader(GetMeshExtensionsReader());
  json::SetObjectReader(GetMeshReader());
  json::SetObjectReader(GetSkinReader());
  json::SetObjectReader(GetCameraPerspectiveReader());
  json::SetObjectReader(GetCameraOrthographicReader());
  json::SetObjectReader(GetCameraReader());
  json::SetObjectReader(GetNodeReader());
  json::SetObjectReader(GetAnimationSamplerReader());
  json::SetObjectReader(GetAnimationChannelTargetReader());
  json::SetObjectReader(GetAnimationChannelReader());
  json::SetObjectReader(GetAnimationReader());
  json::SetObjectReader(GetSceneReader());
}

void SetDefaultEnvironmentMap(const gltf2::Document& document, ConversionContext& context)
{
  EnvironmentDefinition environmentDefinition;
  environmentDefinition.mUseBrdfTexture = true;
  environmentDefinition.mIblIntensity   = Scene3D::Loader::EnvironmentDefinition::GetDefaultIntensity();
  context.mOutput.mResources.mEnvironmentMaps.push_back({std::move(environmentDefinition), EnvironmentDefinition::Textures()});
}

void InitializeGltfLoader()
{
  static Dali::Mutex initializeMutex;
  // Set ObjectReader only once (for all gltf loading).
  static bool setObjectReadersRequired = true;
  {
    Mutex::ScopedLock lock(initializeMutex);
    if(setObjectReadersRequired)
    {
      // NOTE: only referencing own, anonymous namespace, const objects; the pointers will never need to change.
      SetObjectReaders();
      setObjectReadersRequired = false;
    }
  }
}

const std::string_view GetRendererModelIdentification()
{
  return MRENDERER_MODEL_IDENTIFICATION;
}

void ReadDocument(const json_object_s& jsonObject, gltf2::Document& document)
{
  GetDocumentReader().Read(jsonObject, document);
}

void ReadDocumentFromParsedData(const json_object_s& jsonObject, gltf2::Document& document)
{
  static Dali::Mutex readMutex;
  Mutex::ScopedLock  lock(readMutex);
  gt::SetRefReaderObject(document);
  Gltf2Util::ReadDocument(jsonObject, document);
}

bool GenerateDocument(json::unique_ptr& root, gt::Document& document, bool& isMRendererModel)
{
  auto& rootObject = js::Cast<json_object_s>(*root);
  auto  jsonAsset  = js::FindObjectChild("asset", rootObject);

  auto jsAssetVersion = js::FindObjectChild("version", js::Cast<json_object_s>(*jsonAsset));
  if(jsAssetVersion)
  {
    document.mAsset.mVersion = js::Read::StringView(*jsAssetVersion);
  }

  auto jsAssetGenerator = js::FindObjectChild("generator", js::Cast<json_object_s>(*jsonAsset));
  if(jsAssetGenerator)
  {
    document.mAsset.mGenerator = js::Read::StringView(*jsAssetGenerator);
    isMRendererModel           = (document.mAsset.mGenerator.find(Gltf2Util::GetRendererModelIdentification().data()) != std::string_view::npos);
  }

  Gltf2Util::InitializeGltfLoader();
  Gltf2Util::ReadDocumentFromParsedData(rootObject, document);

  return true;
}

void ConvertGltfToContext(gt::Document& document, Gltf2Util::ConversionContext& context, bool isMRendererModel)
{
  Gltf2Util::ConvertBuffers(document, context);
  Gltf2Util::ConvertMaterials(document, context);
  Gltf2Util::ConvertMeshes(document, context);
  Gltf2Util::ConvertNodes(document, context, isMRendererModel);
  Gltf2Util::ConvertAnimations(document, context);
  Gltf2Util::ProcessSkins(document, context);

  // Set Default Environment map
  Gltf2Util::SetDefaultEnvironmentMap(document, context);
}

} // namespace Gltf2Util

} // namespace Dali::Scene3D::Loader::Internal