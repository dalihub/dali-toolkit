/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/control-wrapper-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/object/handle-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/styling/style-manager.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  // empty handle as we cannot create control wrapper
  return BaseHandle();
}

// Setup type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::ControlWrapper, Toolkit::Control, Create )
DALI_TYPE_REGISTRATION_END()

}

/*
 * Implementation.
 */

Dali::Toolkit::ControlWrapper ControlWrapper::New( const std::string& typeName, ControlWrapper* controlWrapper )
{
  ControlWrapperPtr wrapper( controlWrapper );

  // Pass ownership to CustomActor via derived handle.
  Dali::Toolkit::ControlWrapper handle( *wrapper );

  // Second-phase initialisation of the implementation.
  // This can only be done after the CustomActor connection has been made.
  wrapper->Initialize();

  // Different types of C# custom view registered themselves using type registry,
  // but their type names are registered per type not per instance, so they still
  // have the same wrong type name in native side when type registry queries the
  // unique type name of each instance using typeid() because of the binding.
  // Therefore, we have to link each instance with its correct type info if already
  // pre-registered.

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeName );
  if(typeInfo)
  {
    Dali::DevelHandle::SetTypeInfo( handle, typeInfo );
  }

  return handle;
}

ControlWrapper::ControlWrapper( CustomControlBehaviour behaviourFlags )
: Control( static_cast< ControlBehaviour >( behaviourFlags ) )
{
}

ControlWrapper::~ControlWrapper()
{
}

void ControlWrapper::RelayoutRequest()
{
  CustomActorImpl::RelayoutRequest();
}

float ControlWrapper::GetHeightForWidthBase( float width )
{
  return CustomActorImpl::GetHeightForWidthBase( width );
}

float ControlWrapper::GetWidthForHeightBase( float height )
{
  return CustomActorImpl::GetWidthForHeightBase( height );
}

float ControlWrapper::CalculateChildSizeBase( const Dali::Actor& child, Dimension::Type dimension )
{
  return CustomActorImpl::CalculateChildSizeBase( child, dimension );
}

bool ControlWrapper::RelayoutDependentOnChildrenBase( Dimension::Type dimension )
{
  return CustomActorImpl::RelayoutDependentOnChildrenBase( dimension );
}

void ControlWrapper::RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual )
{
  Control::RegisterVisual( index, visual );
}

void ControlWrapper::RegisterVisual( Property::Index index, Toolkit::Visual::Base& visual, bool enabled )
{
  Control::RegisterVisual( index, visual, enabled );
}

void ControlWrapper::UnregisterVisual( Property::Index index )
{
  Control::UnregisterVisual( index );
}

Toolkit::Visual::Base ControlWrapper::GetVisual( Property::Index index ) const
{
  return Control::GetVisual( index );
}

void ControlWrapper::EnableVisual( Property::Index index, bool enable )
{
  Control::EnableVisual( index, enable );
}

bool ControlWrapper::IsVisualEnabled( Property::Index index ) const
{
  return Control::IsVisualEnabled( index );
}

Dali::Animation ControlWrapper::CreateTransition( const Toolkit::TransitionData& handle )
{
  return Control::CreateTransition( handle );
}

void ControlWrapper::ApplyThemeStyle()
{
  Toolkit::StyleManager styleManager = StyleManager::Get();

  // if style manager is available
  if( styleManager )
  {
    StyleManager& styleManagerImpl = GetImpl( styleManager );

    // Apply the current style
    styleManagerImpl.ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
