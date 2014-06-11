//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali-toolkit/public-api/shader-effects/page-turn-effect.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/shader-effects/page-turn-effect-impl.h>

namespace Dali
{

namespace Toolkit
{

PageTurnEffect::PageTurnEffect()
{
}

// Call the Parent copy constructor to add reference to the implementation for this object
PageTurnEffect::PageTurnEffect( ShaderEffect handle, Internal::PageTurnEffect* shaderExtension )
: ShaderEffect( handle )
{
  AttachExtension( shaderExtension );
}

PageTurnEffect::~PageTurnEffect()
{
}

PageTurnEffect PageTurnEffect::New( bool enableBlending )
{
  return Internal::PageTurnEffect::CreateShaderEffect( enableBlending );
}

void PageTurnEffect::SetPageSize(const Vector2& pageSize)
{
  GetImpl( *this ).SetPageSize( pageSize );
}

void PageTurnEffect::SetOriginalCenter(const Vector2& originalCenter)
{
  GetImpl( *this ).SetOriginalCenter( originalCenter );
}

void PageTurnEffect::SetCurrentCenter(const Vector2& currentCenter)
{
  GetImpl( *this ).SetCurrentCenter( currentCenter );
}

void PageTurnEffect::SetIsTurningBack(bool isTurningBack)
{
  GetImpl( *this ).SetIsTurningBack( isTurningBack );
}

void PageTurnEffect::SetShadowWidth(float shadowWidth)
{
  GetImpl( *this ).SetShadowWidth( shadowWidth );
}

void PageTurnEffect::SetSpineShadowParameter(const Vector2& spineShadowParameter)
{
  GetImpl( *this ).SetSpineShadowParameter( spineShadowParameter);
}

const std::string& PageTurnEffect::GetPageSizePropertyName() const
{
  return GetImpl( *this ).GetPageSizePropertyName();
}

const std::string& PageTurnEffect::GetOriginalCenterPropertyName() const
{
  return GetImpl( *this ).GetOriginalCenterPropertyName();
}

const std::string& PageTurnEffect::GetCurrentCenterPropertyName() const
{
  return GetImpl( *this ).GetCurrentCenterPropertyName();
}

} // namespace Toolkit

} // namespace Dali
