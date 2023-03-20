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
#include <dali-scene3d/public-api/model-components/material.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/sampler.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/material-impl.h>

namespace Dali
{
namespace Scene3D
{
Material Material::New()
{
  Internal::MaterialPtr internal = Internal::Material::New();

  return Material(internal.Get());
}

Material::Material()
{
}

Material::Material(const Material& mateiral) = default;

Material::Material(Material&& rhs) = default;

Material::~Material()
{
}

Material& Material::operator=(const Material& handle) = default;

Material& Material::operator=(Material&& rhs) = default;

Material Material::DownCast(BaseHandle handle)
{
  return Material(dynamic_cast<Dali::Scene3D::Internal::Material*>(handle.GetObjectPtr()));
}

Material::Material(Dali::Scene3D::Internal::Material* internal)
: BaseHandle(internal)
{
}

// Public Method

void Material::SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue)
{
  GetImplementation(*this).SetProperty(index, propertyValue);
}

Dali::Property::Value Material::GetProperty(Dali::Property::Index index) const
{
  return GetImplementation(*this).GetProperty(index);
}

void Material::SetTexture(Scene3D::Material::TextureType index, Dali::Texture texture)
{
  GetImplementation(*this).SetTexture(index, texture);
}

Dali::Texture Material::GetTexture(Scene3D::Material::TextureType index)
{
  return GetImplementation(*this).GetTexture(index);
}

void Material::SetSampler(Scene3D::Material::TextureType index, Dali::Sampler sampler)
{
  GetImplementation(*this).SetSampler(index, sampler);
}

Dali::Sampler Material::GetSampler(Scene3D::Material::TextureType index)
{
  return GetImplementation(*this).GetSampler(index);
}

} // namespace Scene3D

} // namespace Dali
