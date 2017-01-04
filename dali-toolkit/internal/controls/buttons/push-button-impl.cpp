/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "push-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

#if defined(DEBUG_ENABLED)
  extern Debug::Filter* gLogButtonFilter;
#endif

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
  return Toolkit::PushButton::New();
}

// Properties

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::PushButton, Toolkit::Button, Create )

DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "unselectedIcon",  STRING, UNSELECTED_ICON )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "selectedIcon",  STRING, SELECTED_ICON )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "iconAlignment",  STRING, ICON_ALIGNMENT )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "labelPadding",  STRING, LABEL_PADDING )
DALI_PROPERTY_REGISTRATION( Toolkit, PushButton, "iconPadding",  STRING, ICON_PADDING )

DALI_TYPE_REGISTRATION_END()

/*
 * Table to define Text-to-enum conversions for IconAlignment.
 */
const Dali::Scripting::StringEnum IconAlignmentTable[] = {
  { "LEFT",   Toolkit::Internal::PushButton::LEFT },
  { "RIGHT",  Toolkit::Internal::PushButton::RIGHT },
  { "TOP",    Toolkit::Internal::PushButton::TOP },
  { "BOTTOM", Toolkit::Internal::PushButton::BOTTOM },
}; const unsigned int IconAlignmentTableCount = sizeof( IconAlignmentTable ) / sizeof( IconAlignmentTable[0] );

} // unnamed namespace

namespace
{

} // unnamed namespace

Dali::Toolkit::PushButton PushButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< PushButton > internalPushButton = new PushButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::PushButton pushButton( *internalPushButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPushButton->Initialize();

  return pushButton;
}

PushButton::PushButton()
: Button(),
  mIconAlignment( RIGHT )
{
}

PushButton::~PushButton()
{
}

void PushButton::OnInitialize()
{
  Button::OnInitialize();

  // Push button requires the Leave event.
  Actor self = Self();
  self.SetLeaveRequired( true );
}

void PushButton::SetIconAlignment( const PushButton::IconAlignment iconAlignment )
{
  mIconAlignment = iconAlignment;
  Button::Align labelAlignment;
  switch ( iconAlignment )
  {
  case RIGHT:
  {
    labelAlignment = Button::BEGIN;
    break;
  }
  case TOP:
  {
    labelAlignment = Button::BOTTOM;
    break;
  }
  case BOTTOM:
  {
    labelAlignment = Button::TOP;
    break;
  }
  case LEFT:
  default:
    labelAlignment = Button::END;
    break;
  }

  Button::SetLabelAlignment( labelAlignment );
}

const PushButton::IconAlignment PushButton::GetIconAlignment() const
{
  return mIconAlignment;
}

void PushButton::SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( Dali::BaseHandle( object ) );

  if ( pushButton )
  {
    PushButton& pushButtonImpl( GetImplementation( pushButton ) );

    // Properties remain here for Tizen 3.0 legacy requirements. Are now in Button base class

    switch ( propertyIndex )
    {
      case Toolkit::PushButton::Property::UNSELECTED_ICON:
      {
        pushButtonImpl.CreateVisualsForComponent( Toolkit::DevelButton::Property::UNSELECTED_VISUAL, value, DepthIndex::CONTENT );
        break;
      }
      case Toolkit::PushButton::Property::SELECTED_ICON:
      {
        pushButtonImpl.CreateVisualsForComponent( Toolkit::DevelButton::Property::SELECTED_VISUAL, value, DepthIndex::CONTENT );
        break;
      }
      case Toolkit::PushButton::Property::ICON_ALIGNMENT:
      {
        IconAlignment iconAlignment;
        if( Scripting::GetEnumeration< IconAlignment >( value.Get< std::string >().c_str(), IconAlignmentTable, IconAlignmentTableCount, iconAlignment ) )
        {
          pushButtonImpl.SetIconAlignment( iconAlignment );
        }
        break;
      }
      case Toolkit::PushButton::Property::LABEL_PADDING:
      {
        Vector4 padding ( value.Get< Vector4 >() );
        pushButtonImpl.Button::SetLabelPadding( Padding( padding.x, padding.y, padding.z, padding.w ) );
        break;
      }
      case Toolkit::PushButton::Property::ICON_PADDING:
      {
        Vector4 padding ( value.Get< Vector4 >() );
        pushButtonImpl.Button::SetForegroundPadding( Padding( padding.x, padding.y, padding.z, padding.w ) );
        break;
      }
    }
  }
}

Property::Value PushButton::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( Dali::BaseHandle( object ) );

  if ( pushButton )
  {
    PushButton& pushButtonImpl( GetImplementation( pushButton ) );

    switch ( propertyIndex )
    {
      case Toolkit::PushButton::Property::UNSELECTED_ICON:
      {
        //value = pushButtonImpl.GetIcon( UNSELECTED_DECORATION );
        break;
      }
      case Toolkit::PushButton::Property::SELECTED_ICON:
      {
        //value = pushButtonImpl.GetIcon( UNSELECTED_DECORATION );
        break;
      }
      case Toolkit::PushButton::Property::ICON_ALIGNMENT:
      {
        value = Scripting::GetLinearEnumerationName< IconAlignment >( pushButtonImpl.GetIconAlignment(), IconAlignmentTable, IconAlignmentTableCount );
        break;
      }
      case Toolkit::PushButton::Property::LABEL_PADDING:
      {
        Padding padding = pushButtonImpl.Button::GetLabelPadding();
        value = Vector4( padding.x, padding.y, padding.top, padding.bottom);
        break;
      }
      case Toolkit::PushButton::Property::ICON_PADDING:
      {
        Padding padding = pushButtonImpl.Button::GetForegroundPadding();
        value = Vector4( padding.x, padding.y, padding.top, padding.bottom);
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
