/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/shader-effects/water-effect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/shader-effects/water-effect-impl.h>

namespace Dali
{

namespace Toolkit
{

WaterEffect::WaterEffect()
{
}

// Call the Parent copy constructor to add reference to the implementation for this object
WaterEffect::WaterEffect( ShaderEffect handle, Internal::WaterEffect* shaderExtension )
: ShaderEffect( handle )
{
  AttachExtension( shaderExtension );
}

WaterEffect::~WaterEffect()
{
}

WaterEffect WaterEffect::New( unsigned int numberOfWaves )
{
  return Internal::WaterEffect::CreateShaderEffect( numberOfWaves );
}

unsigned int WaterEffect::GetNumberOfWaves() const
{
  return GetImpl(*this).GetNumberOfWaves();
}

void WaterEffect::SetAmplitude( unsigned int index, float amplitude )
{
  GetImpl(*this).SetAmplitude( index, amplitude );
}

void WaterEffect::SetCenter( unsigned int index, const Vector2& center )
{
  GetImpl(*this).SetCenter( index, center );
}

void WaterEffect::SetPropagation( unsigned int index, float radius )
{
  GetImpl(*this).SetPropagation( index, radius );
}

float WaterEffect::GetAmplitude( unsigned int index ) const
{
  return GetImpl(*this).GetAmplitude( index );
}

Vector2 WaterEffect::GetCenter( unsigned int index ) const
{
  return GetImpl(*this).GetCenter( index );
}

float WaterEffect::GetPropagation( unsigned int index ) const
{
  return GetImpl(*this).GetPropagation( index );
}

std::string WaterEffect::GetAmplitudePropertyName( unsigned int index ) const
{
  return GetImpl(*this).GetAmplitudePropertyName( index );
}

std::string WaterEffect::GetCenterPropertyName( unsigned int index ) const
{
  return GetImpl(*this).GetCenterPropertyName( index );
}

std::string WaterEffect::GetPropagationPropertyName( unsigned int index ) const
{
  return GetImpl(*this).GetPropagationPropertyName( index );
}

} // namespace Toolkit

} // namespace Dali
