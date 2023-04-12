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
#include <dali-scene3d/public-api/model-components/model-primitive.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-primitive-impl.h>

namespace Dali
{
namespace Scene3D
{
ModelPrimitive ModelPrimitive::New()
{
  Internal::ModelPrimitivePtr internal = Internal::ModelPrimitive::New();

  return ModelPrimitive(internal.Get());
}

ModelPrimitive::ModelPrimitive()
{
}

ModelPrimitive::ModelPrimitive(const ModelPrimitive& modelPrimitive) = default;

ModelPrimitive::ModelPrimitive(ModelPrimitive&& rhs) noexcept = default;

ModelPrimitive::~ModelPrimitive()
{
}

ModelPrimitive& ModelPrimitive::operator=(const ModelPrimitive& handle) = default;

ModelPrimitive& ModelPrimitive::operator=(ModelPrimitive&& rhs) noexcept = default;

ModelPrimitive ModelPrimitive::DownCast(BaseHandle handle)
{
  return ModelPrimitive(dynamic_cast<Dali::Scene3D::Internal::ModelPrimitive*>(handle.GetObjectPtr()));
}

ModelPrimitive::ModelPrimitive(Dali::Scene3D::Internal::ModelPrimitive* internal)
: BaseHandle(internal)
{
}

// Public Method
void ModelPrimitive::SetGeometry(Dali::Geometry geometry)
{
  GetImplementation(*this).SetGeometry(geometry);
}

Dali::Geometry ModelPrimitive::GetGeometry() const
{
  return GetImplementation(*this).GetGeometry();
}

void ModelPrimitive::SetMaterial(Dali::Scene3D::Material material)
{
  GetImplementation(*this).SetMaterial(material);
}

Dali::Scene3D::Material ModelPrimitive::GetMaterial() const
{
  return GetImplementation(*this).GetMaterial();
}

} // namespace Scene3D

} // namespace Dali
