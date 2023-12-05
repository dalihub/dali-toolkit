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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/public-api/model-components/material.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_components_material_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_components_material_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Positive test case for a method
int UtcDaliMaterialNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMaterialNew");

  Scene3D::Material material = Scene3D::Material::New();
  DALI_TEST_CHECK(material);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMaterialDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliMaterialDownCast");

  Scene3D::Material material = Scene3D::Material::New();
  BaseHandle        handle(material);

  Scene3D::Material material2 = Scene3D::Material::DownCast(handle);
  DALI_TEST_CHECK(material);
  DALI_TEST_CHECK(material2);
  DALI_TEST_CHECK(material2 == material);
  END_TEST;
}

int UtcDaliMaterialTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("Material");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  Scene3D::Material mateiral = Scene3D::Material::DownCast(handle);
  DALI_TEST_CHECK(mateiral);

  END_TEST;
}

int UtcDaliMaterialCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::Material mateiral = Scene3D::Material::New();
  DALI_TEST_CHECK(mateiral);

  Scene3D::Material copy(mateiral);
  DALI_TEST_CHECK(mateiral == copy);

  Scene3D::Material assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == mateiral);

  END_TEST;
}

int UtcDaliMaterialMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::Material mateiral = Scene3D::Material::New();
  DALI_TEST_EQUALS(1, mateiral.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Scene3D::Material moved = std::move(mateiral);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!mateiral);

  END_TEST;
}

int UtcDaliMaterialMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::Material mateiral = Scene3D::Material::New();
  DALI_TEST_EQUALS(1, mateiral.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Scene3D::Material moved;
  moved = std::move(mateiral);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!mateiral);

  END_TEST;
}

// Method test
int UtcDaliMaterialSetGetProperty(void)
{
  tet_infoline("UtcDaliMaterialSetGetProperty.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  std::string name = "name";
  material.SetProperty(Scene3D::Material::Property::NAME, name);
  DALI_TEST_EQUALS(name, material.GetProperty<std::string>(Scene3D::Material::Property::NAME), TEST_LOCATION);

  std::string baseColorUrl = "baseColorUrl";
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, baseColorUrl);
  DALI_TEST_EQUALS(baseColorUrl, material.GetProperty<std::string>(Scene3D::Material::Property::BASE_COLOR_URL), TEST_LOCATION);

  Vector4 baseColorFactor(0.1f, 0.2f, 0.3f, 0.4f);
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_FACTOR, baseColorFactor);
  DALI_TEST_EQUALS(baseColorFactor, material.GetProperty<Vector4>(Scene3D::Material::Property::BASE_COLOR_FACTOR), TEST_LOCATION);

  std::string metallicRoughnessUrl = "metallicRoughnessUrl";
  material.SetProperty(Scene3D::Material::Property::METALLIC_ROUGHNESS_URL, metallicRoughnessUrl);
  DALI_TEST_EQUALS(metallicRoughnessUrl, material.GetProperty<std::string>(Scene3D::Material::Property::METALLIC_ROUGHNESS_URL), TEST_LOCATION);

  float metallicFactor = 0.5f;
  material.SetProperty(Scene3D::Material::Property::METALLIC_FACTOR, metallicFactor);
  DALI_TEST_EQUALS(metallicFactor, material.GetProperty<float>(Scene3D::Material::Property::METALLIC_FACTOR), TEST_LOCATION);

  float roughnessFactor = 0.6f;
  material.SetProperty(Scene3D::Material::Property::ROUGHNESS_FACTOR, roughnessFactor);
  DALI_TEST_EQUALS(roughnessFactor, material.GetProperty<float>(Scene3D::Material::Property::ROUGHNESS_FACTOR), TEST_LOCATION);

  std::string normalUrl = "normalUrl";
  material.SetProperty(Scene3D::Material::Property::NORMAL_URL, normalUrl);
  DALI_TEST_EQUALS(normalUrl, material.GetProperty<std::string>(Scene3D::Material::Property::NORMAL_URL), TEST_LOCATION);

  float normalScale = 0.7f;
  material.SetProperty(Scene3D::Material::Property::NORMAL_SCALE, normalScale);
  DALI_TEST_EQUALS(normalScale, material.GetProperty<float>(Scene3D::Material::Property::NORMAL_SCALE), TEST_LOCATION);

  std::string occlusionUrl = "occlusionUrl";
  material.SetProperty(Scene3D::Material::Property::OCCLUSION_URL, occlusionUrl);
  DALI_TEST_EQUALS(occlusionUrl, material.GetProperty<std::string>(Scene3D::Material::Property::OCCLUSION_URL), TEST_LOCATION);

  float occlusionStrength = 0.8f;
  material.SetProperty(Scene3D::Material::Property::OCCLUSION_STRENGTH, occlusionStrength);
  DALI_TEST_EQUALS(occlusionStrength, material.GetProperty<float>(Scene3D::Material::Property::OCCLUSION_STRENGTH), TEST_LOCATION);

  std::string emissiveUrl = "emissiveUrl";
  material.SetProperty(Scene3D::Material::Property::EMISSIVE_URL, emissiveUrl);
  DALI_TEST_EQUALS(emissiveUrl, material.GetProperty<std::string>(Scene3D::Material::Property::EMISSIVE_URL), TEST_LOCATION);

  Vector3 emissiveFactor(0.9f, 0.1f, 0.2f);
  material.SetProperty(Scene3D::Material::Property::EMISSIVE_FACTOR, emissiveFactor);
  DALI_TEST_EQUALS(emissiveFactor, material.GetProperty<Vector3>(Scene3D::Material::Property::EMISSIVE_FACTOR), TEST_LOCATION);

  int alphaMode = 1;
  material.SetProperty(Scene3D::Material::Property::ALPHA_MODE, alphaMode);
  DALI_TEST_EQUALS(alphaMode, material.GetProperty<int>(Scene3D::Material ::Property ::ALPHA_MODE), TEST_LOCATION);

  float alphaCutoff = 0.9f;
  material.SetProperty(Scene3D ::Material ::Property ::ALPHA_CUTOFF, alphaCutoff);
  DALI_TEST_EQUALS(alphaCutoff, material.GetProperty<float>(Scene3D ::Material ::Property ::ALPHA_CUTOFF), TEST_LOCATION);

  bool doubleSided = true;
  material.SetProperty(Scene3D ::Material ::Property ::DOUBLE_SIDED, doubleSided);
  DALI_TEST_EQUALS(doubleSided, material.GetProperty<bool>(Scene3D ::Material ::Property ::DOUBLE_SIDED), TEST_LOCATION);

  float ior = 1.0f;
  material.SetProperty(Scene3D::Material::Property::IOR, ior);
  DALI_TEST_EQUALS(ior, material.GetProperty<float>(Scene3D::Material::Property::IOR), TEST_LOCATION);

  std::string specularUrl = "specularUrl";
  material.SetProperty(Scene3D::Material::Property::SPECULAR_URL, specularUrl);
  DALI_TEST_EQUALS(specularUrl, material.GetProperty<std::string>(Scene3D::Material::Property::SPECULAR_URL), TEST_LOCATION);

  float specularFactor = 1.0f;
  material.SetProperty(Scene3D::Material::Property::SPECULAR_FACTOR, specularFactor);
  DALI_TEST_EQUALS(specularFactor, material.GetProperty<float>(Scene3D::Material::Property::SPECULAR_FACTOR), TEST_LOCATION);

  std::string specularColorUrl = "specularColorUrl";
  material.SetProperty(Scene3D::Material::Property::SPECULAR_COLOR_URL, specularColorUrl);
  DALI_TEST_EQUALS(specularColorUrl, material.GetProperty<std::string>(Scene3D::Material::Property::SPECULAR_COLOR_URL), TEST_LOCATION);

  Vector3 specularColorFactor(1.0f, 1.0f, 1.0f);
  material.SetProperty(Scene3D::Material::Property::SPECULAR_COLOR_FACTOR, specularColorFactor);
  DALI_TEST_EQUALS(specularColorFactor, material.GetProperty<Vector3>(Scene3D::Material::Property::SPECULAR_COLOR_FACTOR), TEST_LOCATION);

  int32_t depthIndex = 50;
  material.SetProperty(Scene3D::Material::Property::DEPTH_INDEX, depthIndex);
  DALI_TEST_EQUALS(depthIndex, material.GetProperty<int32_t>(Scene3D::Material::Property::DEPTH_INDEX), TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialSetGetTexture(void)
{
  tet_infoline("UtcDaliMaterialSetGetTexture.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  Dali::Texture baseColor = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::BASE_COLOR, baseColor);
  DALI_TEST_EQUALS(baseColor, material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR), TEST_LOCATION);

  Dali::Texture metallicRoughness = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::METALLIC_ROUGHNESS, metallicRoughness);
  DALI_TEST_EQUALS(metallicRoughness, material.GetTexture(Scene3D::Material::TextureType::METALLIC_ROUGHNESS), TEST_LOCATION);

  Dali::Texture normal = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::NORMAL, normal);
  DALI_TEST_EQUALS(normal, material.GetTexture(Scene3D::Material::TextureType::NORMAL), TEST_LOCATION);

  Dali::Texture occlusion = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::OCCLUSION, occlusion);
  DALI_TEST_EQUALS(occlusion, material.GetTexture(Scene3D::Material::TextureType::OCCLUSION), TEST_LOCATION);

  Dali::Texture emissive = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::EMISSIVE, emissive);
  DALI_TEST_EQUALS(emissive, material.GetTexture(Scene3D::Material::TextureType::EMISSIVE), TEST_LOCATION);

  Dali::Texture specular = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::SPECULAR, specular);
  DALI_TEST_EQUALS(specular, material.GetTexture(Scene3D::Material::TextureType::SPECULAR), TEST_LOCATION);

  Dali::Texture specularColor = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  material.SetTexture(Scene3D::Material::TextureType::SPECULAR_COLOR, specularColor);
  DALI_TEST_EQUALS(specularColor, material.GetTexture(Scene3D::Material::TextureType::SPECULAR_COLOR), TEST_LOCATION);

  DALI_TEST_CHECK(!material.GetTexture((Scene3D::Material::TextureType)100));

  END_TEST;
}

int UtcDaliMaterialSetGetSampler(void)
{
  tet_infoline("UtcDaliMaterialSetGetSampler.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  Dali::Sampler baseColor = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::BASE_COLOR, baseColor);
  DALI_TEST_EQUALS(baseColor, material.GetSampler(Scene3D::Material::TextureType::BASE_COLOR), TEST_LOCATION);

  Dali::Sampler metallicRoughness = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::METALLIC_ROUGHNESS, metallicRoughness);
  DALI_TEST_EQUALS(metallicRoughness, material.GetSampler(Scene3D::Material::TextureType::METALLIC_ROUGHNESS), TEST_LOCATION);

  Dali::Sampler normal = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::NORMAL, normal);
  DALI_TEST_EQUALS(normal, material.GetSampler(Scene3D::Material::TextureType::NORMAL), TEST_LOCATION);

  Dali::Sampler occlusion = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::OCCLUSION, occlusion);
  DALI_TEST_EQUALS(occlusion, material.GetSampler(Scene3D::Material::TextureType::OCCLUSION), TEST_LOCATION);

  Dali::Sampler emissive = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::EMISSIVE, emissive);
  DALI_TEST_EQUALS(emissive, material.GetSampler(Scene3D::Material::TextureType::EMISSIVE), TEST_LOCATION);

  Dali::Sampler specular = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::SPECULAR, specular);
  DALI_TEST_EQUALS(specular, material.GetSampler(Scene3D::Material::TextureType::SPECULAR), TEST_LOCATION);

  Dali::Sampler specularColor = Dali::Sampler::New();
  material.SetSampler(Scene3D::Material::TextureType::SPECULAR_COLOR, specularColor);
  DALI_TEST_EQUALS(specularColor, material.GetSampler(Scene3D::Material::TextureType::SPECULAR_COLOR), TEST_LOCATION);

  DALI_TEST_CHECK(!material.GetSampler((Scene3D::Material::TextureType)100));

  END_TEST;
}
