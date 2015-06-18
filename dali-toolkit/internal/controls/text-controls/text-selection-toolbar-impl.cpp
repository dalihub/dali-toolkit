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
#include <dali-toolkit/internal/controls/text-controls/text-selection-toolbar-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <iostream>
#include <libintl.h>
#include <cfloat>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const Dali::Vector2 DEFAULT_MAX_SIZE( 400.0f, 65.0f ); ///< The maximum size of the Toolbar.

BaseHandle Create()
{
  return Toolkit::TextSelectionToolbar::New();
}

// Setup properties, signals and actions using the type-registry.

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextSelectionToolbar, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionToolbar, "max-size", VECTOR2, MAX_SIZE )

DALI_TYPE_REGISTRATION_END()

} // namespace

Dali::Toolkit::TextSelectionToolbar TextSelectionToolbar::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextSelectionToolbar > impl = new TextSelectionToolbar();

  // Pass ownership to CustomActor handle
  Dali::Toolkit::TextSelectionToolbar handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextSelectionToolbar::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextSelectionToolbar selectionPopup = Toolkit::TextSelectionToolbar::DownCast( Dali::BaseHandle( object ) );

  if( selectionPopup )
  {
    TextSelectionToolbar& impl( GetImpl( selectionPopup ) );

    switch( index )
    {
      case Toolkit::TextSelectionToolbar::Property::MAX_SIZE:
      {
       impl.SetPopupMaxSize( value.Get< Vector2 >() );
       break;
      }

    } // switch
  } // TextSelectionToolbar
}

Property::Value TextSelectionToolbar::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextSelectionToolbar selectionPopup = Toolkit::TextSelectionToolbar::DownCast( Dali::BaseHandle( object ) );

  if( selectionPopup )
  {
    TextSelectionToolbar& impl( GetImpl( selectionPopup ) );

    switch( index )
    {
      case Toolkit::TextSelectionToolbar::Property::MAX_SIZE:
      {
        value = impl.GetPopupMaxSize();
        break;
      }
    } // switch
  }
  return value;
}

void TextSelectionToolbar::OnInitialize()
{
  SetUp();
}

void TextSelectionToolbar::SetPopupMaxSize( const Size& maxSize )
{
  mMaxSize = maxSize;
}

const Dali::Vector2& TextSelectionToolbar::GetPopupMaxSize() const
{
  return mMaxSize;
}

void TextSelectionToolbar::SetUp()
{
  Actor self = Self();
  self.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  // Create Layer and Stencil.
  mStencilLayer = Layer::New();
  mStencilLayer.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  mStencilLayer.SetParentOrigin( ParentOrigin::CENTER );
  mStencilLayer.SetMaximumSize( mMaxSize );

  ImageActor stencil = CreateSolidColorActor( Color::RED );
  stencil.SetDrawMode( DrawMode::STENCIL );
  stencil.SetVisible( true );
  stencil.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  stencil.SetParentOrigin( ParentOrigin::CENTER );

  Actor scrollview = Actor::New(); //todo make a scrollview
  scrollview.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  scrollview.SetParentOrigin( ParentOrigin::CENTER );

  // Toolbar needs at least one option, adding further options with increase it's size
  mTableOfButtons = Dali::Toolkit::TableView::New( 1, 1 );
  mTableOfButtons.SetFitHeight( 0 );
  mTableOfButtons.SetParentOrigin( ParentOrigin::CENTER );

  mStencilLayer.Add( stencil );
  mStencilLayer.Add( scrollview );
  scrollview.Add( mTableOfButtons );
  self.Add( mStencilLayer );

  mStencilLayer.RaiseToTop();
}

void TextSelectionToolbar::AddOption( Actor& option )
{
  mTableOfButtons.AddChild( option, Toolkit::TableView::CellPosition( 0, mIndexInTable )  );
  mTableOfButtons.SetFitWidth( mIndexInTable );
  mIndexInTable++;
}

void TextSelectionToolbar::AddDivider( Actor& divider )
{
  AddOption( divider );
  mDividerIndexes.PushBack( mIndexInTable );
}

void TextSelectionToolbar::ResizeDividers( Size& size )
{
  for( unsigned int i = 0; i < mDividerIndexes.Count(); ++i )
  {
    Actor divider = mTableOfButtons.GetChildAt( Toolkit::TableView::CellPosition( 0, mDividerIndexes[ i ] ) );
    divider.SetSize( size );
  }
  RelayoutRequest();
}

TextSelectionToolbar::TextSelectionToolbar()
: Control( ControlBehaviour( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ) ),
  mMaxSize ( DEFAULT_MAX_SIZE ),
  mIndexInTable( 0 ),
  mDividerIndexes()
{
}

TextSelectionToolbar::~TextSelectionToolbar()
{
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
