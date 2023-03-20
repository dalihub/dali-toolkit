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

#include <toolkit-event-thread-callback.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/internal/model-components/material-impl.h>
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_components_material_impl_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_components_material_impl_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
} // namespace

// Method test
int UtcDaliMaterialImplSetGetTextureInformation(void)
{
  tet_infoline("UtcDaliMaterialImplSetGetTextureInformation.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  Scene3D::Internal::Material::TextureInformation baseColor;
  Dali::Texture                                   baseColorTexture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  Dali::Sampler                                   baseColorSampler = Dali::Sampler::New();
  baseColor.mTexture                                               = baseColorTexture;
  baseColor.mSampler                                               = baseColorSampler;
  baseColor.mUrl                                                   = "baseColor";
  GetImplementation(material).SetTextureInformation(Scene3D::Material::TextureType::BASE_COLOR, std::move(baseColor));
  DALI_TEST_EQUALS(baseColorTexture, material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS(baseColorSampler, material.GetSampler(Scene3D::Material::TextureType::BASE_COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS("baseColor", material.GetProperty<std::string>(Scene3D::Material::Property::BASE_COLOR_URL), TEST_LOCATION);

  Scene3D::Internal::Material::TextureInformation metallicRoughness;
  Dali::Texture                                   metallicRoughnessTexture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  Dali::Sampler                                   metallicRoughnessSampler = Dali::Sampler::New();
  metallicRoughness.mTexture                                               = metallicRoughnessTexture;
  metallicRoughness.mSampler                                               = metallicRoughnessSampler;
  metallicRoughness.mUrl                                                   = "metallicRoughness";
  GetImplementation(material).SetTextureInformation(Scene3D::Material::TextureType::METALLIC_ROUGHNESS, std::move(metallicRoughness));
  DALI_TEST_EQUALS(metallicRoughnessTexture, material.GetTexture(Scene3D::Material::TextureType::METALLIC_ROUGHNESS), TEST_LOCATION);
  DALI_TEST_EQUALS(metallicRoughnessSampler, material.GetSampler(Scene3D::Material::TextureType::METALLIC_ROUGHNESS), TEST_LOCATION);
  DALI_TEST_EQUALS("metallicRoughness", material.GetProperty<std::string>(Scene3D::Material::Property::METALLIC_ROUGHNESS_URL), TEST_LOCATION);

  Scene3D::Internal::Material::TextureInformation normal;
  Dali::Texture                                   normalTexture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  Dali::Sampler                                   normalSampler = Dali::Sampler::New();
  normal.mTexture                                               = normalTexture;
  normal.mSampler                                               = normalSampler;
  normal.mUrl                                                   = "normal";
  GetImplementation(material).SetTextureInformation(Scene3D::Material::TextureType::NORMAL, std::move(normal));
  DALI_TEST_EQUALS(normalTexture, material.GetTexture(Scene3D::Material::TextureType::NORMAL), TEST_LOCATION);
  DALI_TEST_EQUALS(normalSampler, material.GetSampler(Scene3D::Material::TextureType::NORMAL), TEST_LOCATION);
  DALI_TEST_EQUALS("normal", material.GetProperty<std::string>(Scene3D::Material::Property::NORMAL_URL), TEST_LOCATION);

  Dali::TextureSet textureSet = GetImplementation(material).GetTextureSet();
  DALI_TEST_EQUALS(3, textureSet.GetTextureCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(baseColorTexture, textureSet.GetTexture(0u), TEST_LOCATION);
  DALI_TEST_EQUALS(metallicRoughnessTexture, textureSet.GetTexture(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(normalTexture, textureSet.GetTexture(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(baseColorSampler, textureSet.GetSampler(0u), TEST_LOCATION);
  DALI_TEST_EQUALS(metallicRoughnessSampler, textureSet.GetSampler(1u), TEST_LOCATION);

  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  GetImplementation(material).UpdateMaterialData();

  std::vector<std::string> defines;
  defines.push_back("THREE_TEX");
  defines.push_back("GLTF_CHANNELS");
  defines.push_back("BASECOLOR_TEX");
  defines.push_back("METALLIC_ROUGHNESS_TEX");
  defines.push_back("NORMAL_TEX");

  std::string fragmentShader = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_FRAG.data();
  for(const auto& define : defines)
  {
    Scene3D::Loader::ShaderDefinition::ApplyDefine(fragmentShader, define);
  }

  DALI_TEST_EQUALS(fragmentShader, GetImplementation(material).GetFragmentShader(), TEST_LOCATION);

  Scene3D ::Internal ::Material ::TextureInformation occlusion;
  Dali ::Texture                                     occlusiontexture = Dali ::Texture ::New(TextureType ::TEXTURE_2D, Pixel ::RGBA8888, 100, 100);
  Dali ::Sampler                                     occlusionSampler = Dali ::Sampler ::New();
  occlusion.mTexture                                                  = occlusiontexture;
  occlusion.mSampler                                                  = occlusionSampler;
  occlusion.mUrl                                                      = "occlusion";
  GetImplementation(material).SetTextureInformation(Scene3D ::Material ::TextureType ::OCCLUSION, std ::move(occlusion));
  DALI_TEST_EQUALS(occlusiontexture, material.GetTexture(Scene3D ::Material ::TextureType ::OCCLUSION), TEST_LOCATION);
  DALI_TEST_EQUALS(occlusionSampler, material.GetSampler(Scene3D ::Material ::TextureType ::OCCLUSION), TEST_LOCATION);
  DALI_TEST_EQUALS("occlusion", material.GetProperty<std ::string>(Scene3D ::Material ::Property ::OCCLUSION_URL), TEST_LOCATION);

  Scene3D::Internal::Material::TextureInformation emissive;
  Dali::Texture                                   emissiveTexture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  Dali::Sampler                                   emissiveSampler = Dali::Sampler::New();
  emissive.mTexture                                               = emissiveTexture;
  emissive.mSampler                                               = emissiveSampler;
  emissive.mUrl                                                   = "emissive";
  GetImplementation(material).SetTextureInformation(Scene3D::Material::TextureType::EMISSIVE, std::move(emissive));
  DALI_TEST_EQUALS(emissiveTexture, material.GetTexture(Scene3D::Material::TextureType::EMISSIVE), TEST_LOCATION);
  DALI_TEST_EQUALS(emissiveSampler, material.GetSampler(Scene3D::Material::TextureType::EMISSIVE), TEST_LOCATION);
  DALI_TEST_EQUALS("emissive", material.GetProperty<std::string>(Scene3D::Material::Property::EMISSIVE_URL), TEST_LOCATION);

  Scene3D ::Internal ::Material ::TextureInformation specular;
  Dali ::Texture                                     specularTexture = Dali ::Texture ::New(TextureType ::TEXTURE_2D, Pixel ::RGBA8888, 100, 100);
  Dali ::Sampler                                     specularSampler = Dali ::Sampler ::New();
  specular.mTexture                                                  = specularTexture;
  specular.mSampler                                                  = specularSampler;
  specular.mUrl                                                      = "specular";
  GetImplementation(material).SetTextureInformation(Scene3D ::Material ::TextureType ::SPECULAR, std ::move(specular));
  DALI_TEST_EQUALS(specularTexture, material.GetTexture(Scene3D ::Material ::TextureType ::SPECULAR), TEST_LOCATION);
  DALI_TEST_EQUALS(specularSampler, material.GetSampler(Scene3D ::Material ::TextureType ::SPECULAR), TEST_LOCATION);
  DALI_TEST_EQUALS("specular", material.GetProperty<std ::string>(Scene3D ::Material ::Property ::SPECULAR_URL), TEST_LOCATION);

  Scene3D::Internal::Material::TextureInformation specularColor;
  Dali::Texture                                   specularColorTexture = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100, 100);
  Dali::Sampler                                   specularColorSampler = Dali::Sampler::New();
  specularColor.mTexture                                               = specularColorTexture;
  specularColor.mSampler                                               = specularColorSampler;
  specularColor.mUrl                                                   = "specularColor";
  GetImplementation(material).SetTextureInformation(Scene3D::Material::TextureType::SPECULAR_COLOR, std::move(specularColor));
  DALI_TEST_EQUALS(specularColorTexture, material.GetTexture(Scene3D::Material::TextureType::SPECULAR_COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS(specularColorSampler, material.GetSampler(Scene3D::Material::TextureType::SPECULAR_COLOR), TEST_LOCATION);
  DALI_TEST_EQUALS("specularColor", material.GetProperty<std::string>(Scene3D::Material::Property::SPECULAR_COLOR_URL), TEST_LOCATION);

  textureSet = GetImplementation(material).GetTextureSet();
  DALI_TEST_EQUALS(7, textureSet.GetTextureCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(baseColorTexture, textureSet.GetTexture(0u), TEST_LOCATION);
  DALI_TEST_EQUALS(metallicRoughnessTexture, textureSet.GetTexture(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(normalTexture, textureSet.GetTexture(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(occlusiontexture, textureSet.GetTexture(3u), TEST_LOCATION);
  DALI_TEST_EQUALS(emissiveTexture, textureSet.GetTexture(4u), TEST_LOCATION);
  DALI_TEST_EQUALS(specularTexture, textureSet.GetTexture(5u), TEST_LOCATION);
  DALI_TEST_EQUALS(specularColorTexture, textureSet.GetTexture(6u), TEST_LOCATION);

  DALI_TEST_EQUALS(baseColorSampler, textureSet.GetSampler(0u), TEST_LOCATION);
  DALI_TEST_EQUALS(metallicRoughnessSampler, textureSet.GetSampler(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(normalSampler, textureSet.GetSampler(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(occlusionSampler, textureSet.GetSampler(3u), TEST_LOCATION);
  DALI_TEST_EQUALS(emissiveSampler, textureSet.GetSampler(4u), TEST_LOCATION);
  DALI_TEST_EQUALS(specularSampler, textureSet.GetSampler(5u), TEST_LOCATION);
  DALI_TEST_EQUALS(specularColorSampler, textureSet.GetSampler(6u), TEST_LOCATION);

  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  GetImplementation(material).UpdateMaterialData();

  defines.push_back("OCCLUSION");
  defines.push_back("EMISSIVE");
  defines.push_back("MATERIAL_SPECULAR_TEXTURE");
  defines.push_back("MATERIAL_SPECULAR_COLOR_TEXTURE");

  fragmentShader = SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_FRAG.data();
  for(const auto& define : defines)
  {
    Scene3D::Loader::ShaderDefinition::ApplyDefine(fragmentShader, define);
  }

  DALI_TEST_EQUALS(fragmentShader, GetImplementation(material).GetFragmentShader(), TEST_LOCATION);

  END_TEST;
}

// Method test
int UtcDaliMaterialCheckUniform(void)
{
  tet_infoline("UtcDaliMaterialCheckUniform.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  Vector4 baseColorFactor(0.1f, 0.2f, 0.3f, 0.4f);
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_FACTOR, baseColorFactor);
  DALI_TEST_EQUALS(baseColorFactor, material.GetProperty<Vector4>(Scene3D::Material::Property::BASE_COLOR_FACTOR), TEST_LOCATION);

  float metallicFactor = 0.5f;
  material.SetProperty(Scene3D::Material::Property::METALLIC_FACTOR, metallicFactor);
  DALI_TEST_EQUALS(metallicFactor, material.GetProperty<float>(Scene3D::Material::Property::METALLIC_FACTOR), TEST_LOCATION);

  float roughnessFactor = 0.6f;
  material.SetProperty(Scene3D::Material::Property::ROUGHNESS_FACTOR, roughnessFactor);
  DALI_TEST_EQUALS(roughnessFactor, material.GetProperty<float>(Scene3D::Material::Property::ROUGHNESS_FACTOR), TEST_LOCATION);

  float normalScale = 0.7f;
  material.SetProperty(Scene3D::Material::Property::NORMAL_SCALE, normalScale);
  DALI_TEST_EQUALS(normalScale, material.GetProperty<float>(Scene3D::Material::Property::NORMAL_SCALE), TEST_LOCATION);

  float occlusionStrength = 0.8f;
  material.SetProperty(Scene3D::Material::Property::OCCLUSION_STRENGTH, occlusionStrength);
  DALI_TEST_EQUALS(occlusionStrength, material.GetProperty<float>(Scene3D::Material::Property::OCCLUSION_STRENGTH), TEST_LOCATION);

  Vector3 emissiveFactor(0.9f, 0.1f, 0.2f);
  material.SetProperty(Scene3D::Material::Property::EMISSIVE_FACTOR, emissiveFactor);
  DALI_TEST_EQUALS(emissiveFactor, material.GetProperty<Vector3>(Scene3D::Material::Property::EMISSIVE_FACTOR), TEST_LOCATION);

  Dali::Scene3D::Material::AlphaModeType alphaMode = Dali::Scene3D::Material::AlphaModeType::BLEND;
  material.SetProperty(Scene3D::Material::Property::ALPHA_MODE, alphaMode);
  DALI_TEST_EQUALS(alphaMode, material.GetProperty<Dali::Scene3D::Material::AlphaModeType>(Scene3D::Material ::Property ::ALPHA_MODE), TEST_LOCATION);

  float alphaCutoff = 0.9f;
  material.SetProperty(Scene3D ::Material ::Property ::ALPHA_CUTOFF, alphaCutoff);
  DALI_TEST_EQUALS(alphaCutoff, material.GetProperty<float>(Scene3D ::Material ::Property ::ALPHA_CUTOFF), TEST_LOCATION);

  bool doubleSided = false;
  material.SetProperty(Scene3D ::Material ::Property ::DOUBLE_SIDED, doubleSided);
  DALI_TEST_EQUALS(doubleSided, material.GetProperty<bool>(Scene3D ::Material ::Property ::DOUBLE_SIDED), TEST_LOCATION);

  float ior = 1.0f;
  material.SetProperty(Scene3D::Material::Property::IOR, ior);
  DALI_TEST_EQUALS(ior, material.GetProperty<float>(Scene3D::Material::Property::IOR), TEST_LOCATION);

  float specularFactor = 1.0f;
  material.SetProperty(Scene3D::Material::Property::SPECULAR_FACTOR, specularFactor);
  DALI_TEST_EQUALS(specularFactor, material.GetProperty<float>(Scene3D::Material::Property::SPECULAR_FACTOR), TEST_LOCATION);

  Vector3 specularColorFactor(1.0f, 1.0f, 1.0f);
  material.SetProperty(Scene3D::Material::Property::SPECULAR_COLOR_FACTOR, specularColorFactor);
  DALI_TEST_EQUALS(specularColorFactor, material.GetProperty<Vector3>(Scene3D::Material::Property::SPECULAR_COLOR_FACTOR), TEST_LOCATION);

  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  GetImplementation(material).UpdateMaterialData();

  Dali::Shader   shader   = Dali::Shader::New(SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_VERT.data(), SHADER_DEFAULT_PHYSICALLY_BASED_SHADER_FRAG.data());
  Dali::Geometry geometry = Dali::Geometry::New();
  Dali::Renderer renderer = Dali::Renderer::New(geometry, shader);

  GetImplementation(material).SetRendererUniform(renderer);

  DALI_TEST_EQUALS(baseColorFactor, renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("uColorFactor")), TEST_LOCATION);
  DALI_TEST_EQUALS(metallicFactor, renderer.GetProperty<float>(renderer.GetPropertyIndex("uMetallicFactor")), TEST_LOCATION);
  DALI_TEST_EQUALS(roughnessFactor, renderer.GetProperty<float>(renderer.GetPropertyIndex("uRoughnessFactor")), TEST_LOCATION);
  DALI_TEST_EQUALS(normalScale, renderer.GetProperty<float>(renderer.GetPropertyIndex("uNormalScale")), TEST_LOCATION);
  DALI_TEST_EQUALS(emissiveFactor, renderer.GetProperty<Vector3>(renderer.GetPropertyIndex("uEmissiveFactor")), TEST_LOCATION);
  float dielectricSpecular = (Dali::Equals(ior, -1.0)) ? 0.04f : powf((ior - 1.0f) / (ior + 1.0f), 2.0f);
  DALI_TEST_EQUALS(dielectricSpecular, renderer.GetProperty<float>(renderer.GetPropertyIndex("uDielectricSpecular")), TEST_LOCATION);
  DALI_TEST_EQUALS(specularFactor, renderer.GetProperty<float>(renderer.GetPropertyIndex("uSpecularFactor")), TEST_LOCATION);
  DALI_TEST_EQUALS(specularColorFactor, renderer.GetProperty<Vector3>(renderer.GetPropertyIndex("uSpecularColorFactor")), TEST_LOCATION);
  float opaque = alphaMode == Dali::Scene3D::Material::AlphaModeType::BLEND ? 0.0f : 1.0f;
  float mask   = alphaMode == Dali::Scene3D::Material::AlphaModeType::MASK ? 1.0f : 0.0f;
  DALI_TEST_EQUALS(opaque, renderer.GetProperty<float>(renderer.GetPropertyIndex("uOpaque")), TEST_LOCATION);
  DALI_TEST_EQUALS(mask, renderer.GetProperty<float>(renderer.GetPropertyIndex("uMask")), TEST_LOCATION);
  DALI_TEST_EQUALS(alphaCutoff, renderer.GetProperty<float>(renderer.GetPropertyIndex("uAlphaThreshold")), TEST_LOCATION);

  DALI_TEST_EQUALS(DepthTestMode::ON, renderer.GetProperty<DepthTestMode::Type>(Dali::Renderer::Property::DEPTH_TEST_MODE), TEST_LOCATION);
  DALI_TEST_EQUALS(FaceCullingMode::BACK, renderer.GetProperty<FaceCullingMode::Type>(Dali::Renderer::Property::FACE_CULLING_MODE), TEST_LOCATION);
  DALI_TEST_EQUALS(DepthTestMode::ON, renderer.GetProperty<DepthTestMode::Type>(Dali::Renderer::Property::DEPTH_TEST_MODE), TEST_LOCATION);

  END_TEST;
}

int UtcDaliMaterialImplLoadTexture_1(void)
{
  tet_infoline("UtcDaliMaterialLoadTexture.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  DALI_TEST_CHECK(!material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR));
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, TEST_IMAGE_FILE_NAME);
  DALI_TEST_EQUALS(false, GetImplementation(material).IsResourceReady(), TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  DALI_TEST_CHECK(material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR));
  Dali::Texture baseColorTexture = material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR);

  Dali::TextureSet textureSet = GetImplementation(material).GetTextureSet();
  DALI_TEST_EQUALS(1u, textureSet.GetTextureCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(baseColorTexture, textureSet.GetTexture(0u), TEST_LOCATION);
  DALI_TEST_CHECK(textureSet.GetSampler(0u));

  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, TEST_IMAGE_FILE_NAME);
  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  DALI_TEST_EQUALS(baseColorTexture, material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR), TEST_LOCATION);

  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, "");
  DALI_TEST_CHECK(!material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR));

  END_TEST;
}

int UtcDaliMaterialImplLoadTexture_2(void)
{
  tet_infoline("UtcDaliMaterialImplLoadTexture_2.");

  ToolkitTestApplication application;

  Scene3D::Material material = Scene3D::Material::New();

  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  DALI_TEST_CHECK(!material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR));
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, TEST_IMAGE_FILE_NAME);
  DALI_TEST_EQUALS(false, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  material.SetProperty(Scene3D::Material::Property::BASE_COLOR_URL, "");
  DALI_TEST_EQUALS(true, GetImplementation(material).IsResourceReady(), TEST_LOCATION);
  DALI_TEST_CHECK(!material.GetTexture(Scene3D::Material::TextureType::BASE_COLOR));

  END_TEST;
}
