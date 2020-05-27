/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cfloat>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/helpers/color-conversion.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const Dali::Vector2 DEFAULT_SCROLL_BAR_PADDING( 8.0f, 6.0f );

BaseHandle Create()
{
  return Toolkit::TextSelectionToolbar::New();
}

// Setup properties, signals and actions using the type-registry.

DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextSelectionToolbar, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionToolbar, "maxSize",  VECTOR2, MAX_SIZE )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionToolbar, "enableOvershoot",  BOOLEAN, ENABLE_OVERSHOOT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionToolbar, "enableScrollBar", BOOLEAN, ENABLE_SCROLL_BAR )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionToolbar, "scrollBarPadding", VECTOR2, SCROLL_BAR_PADDING )
DALI_PROPERTY_REGISTRATION( Toolkit, TextSelectionToolbar, "scrollView",  MAP, SCROLL_VIEW )

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
      case Toolkit::TextSelectionToolbar::Property::ENABLE_OVERSHOOT:
      {
        if( !impl.mScrollView )
        {
          impl.mScrollView  = Toolkit::ScrollView::New();
        }
        impl.mScrollView.SetOvershootEnabled( value.Get< bool >() );
        break;
      }
      case Toolkit::TextSelectionToolbar::Property::ENABLE_SCROLL_BAR:
      {
        impl.SetUpScrollBar( value.Get< bool >() );
        break;
      }
      case Toolkit::TextSelectionToolbar::Property::SCROLL_BAR_PADDING:
      {
        impl.SetScrollBarPadding( value.Get< Vector2 >() );
        break;
      }
      case Toolkit::TextSelectionToolbar::Property::SCROLL_VIEW:
      {
        // Get a Property::Map from the property if possible.
        Property::Map setPropertyMap;
        if( value.Get( setPropertyMap ) )
        {
          impl.ConfigureScrollview( setPropertyMap );
        }
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
      case Toolkit::TextSelectionToolbar::Property::ENABLE_OVERSHOOT:
      {
        value = impl.mScrollView.IsOvershootEnabled();
        break;
      }
      case Toolkit::TextSelectionToolbar::Property::ENABLE_SCROLL_BAR:
      {
        value = impl.mScrollBar ? true : false;
        break;
      }
      case Toolkit::TextSelectionToolbar::Property::SCROLL_BAR_PADDING:
      {
        value = impl.GetScrollBarPadding();
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

void TextSelectionToolbar::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  float width = std::max ( mTableOfButtons.GetNaturalSize().width, size.width );
  mRulerX->SetDomain( RulerDomain( 0.0, width, true ) );
  mScrollView.SetRulerX( mRulerX );

  if( mScrollBar )
  {
    float barWidth = std::min( mTableOfButtons.GetNaturalSize().width, size.width ) - 2.f * mScrollBarPadding.x;
    mScrollBar.SetSize( Vector2( 0.0f, barWidth ) );
  }
}

void TextSelectionToolbar::SetPopupMaxSize( const Size& maxSize )
{
  mMaxSize = maxSize;
  if (mScrollView && mToolbarLayer )
  {
    mScrollView.SetProperty( Actor::Property::MAXIMUM_SIZE, mMaxSize );
    mToolbarLayer.SetProperty( Actor::Property::MAXIMUM_SIZE, mMaxSize );
  }
}

const Dali::Vector2& TextSelectionToolbar::GetPopupMaxSize() const
{
  return mMaxSize;
}

void TextSelectionToolbar::SetUpScrollView()
{
  mScrollView.SetProperty( Dali::Actor::Property::NAME,"TextSelectionScrollView");
  mScrollView.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  mScrollView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT );
  mScrollView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT );

  mScrollView.SetScrollingDirection( PanGestureDetector::DIRECTION_HORIZONTAL, Degree( 40.0f ) );
  mScrollView.SetAxisAutoLock( true );
  mScrollView.ScrollStartedSignal().Connect( this, &TextSelectionToolbar::OnScrollStarted );
  mScrollView.ScrollCompletedSignal().Connect( this, &TextSelectionToolbar::OnScrollCompleted );
  mScrollView.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX ); // In a new layer, so clip to scroll-view's bounding box

  mRulerX = new DefaultRuler();  // IntrusivePtr which is unreferenced when ScrollView is destroyed.

  RulerPtr rulerY = new DefaultRuler();  // IntrusivePtr which is unreferenced when ScrollView is destroyed.
  rulerY->Disable();
  mScrollView.SetRulerY( rulerY );

  mScrollView.SetOvershootEnabled( true );
}

void TextSelectionToolbar::SetUp()
{
  Actor self = Self();

  self.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  // Create Layer to house the toolbar.
  mToolbarLayer = Layer::New();
  mToolbarLayer.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
  mToolbarLayer.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
  mToolbarLayer.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  if( !mScrollView )
  {
    mScrollView = Toolkit::ScrollView::New();
  }
  SetUpScrollView();

  // Toolbar must start with at least one option, adding further options with increase it's size
  mTableOfButtons = Dali::Toolkit::TableView::New( 1, 1 );
  mTableOfButtons.SetFitHeight( 0 );
  mTableOfButtons.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER_LEFT );
  mTableOfButtons.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER_LEFT );

  mScrollView.Add( mTableOfButtons );
  mToolbarLayer.Add( mScrollView );

  self.Add( mToolbarLayer );
}

void TextSelectionToolbar::SetUpScrollBar( bool enable )
{
  if( enable )
  {
    if( ! mScrollBar )
    {
      Toolkit::ImageView indicator = Toolkit::ImageView::New();
      indicator.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      indicator.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      indicator.SetStyleName( "TextSelectionScrollIndicator" );

      mScrollBar = Toolkit::ScrollBar::New( Toolkit::ScrollBar::Horizontal );
      mScrollBar.SetProperty( Dali::Actor::Property::NAME, "Text popup scroll bar" );
      mScrollBar.SetStyleName( "TextSelectionScrollBar" );
      mScrollBar.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_LEFT );
      mScrollBar.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      mScrollBar.SetPosition( mScrollBarPadding.x, -mScrollBarPadding.y );
      mScrollBar.SetResizePolicy( Dali::ResizePolicy::FIT_TO_CHILDREN, Dali::Dimension::WIDTH );
      mScrollBar.SetProperty( Actor::Property::ORIENTATION, Quaternion( Quaternion( Radian( 1.5f * Math::PI ), Vector3::ZAXIS ) ) );
      mScrollBar.SetScrollIndicator( indicator );
      mScrollBar.GetPanGestureDetector().DetachAll();
      mScrollView.Add( mScrollBar );
    }
  }
  else
  {
    UnparentAndReset( mScrollBar );
  }
}

void TextSelectionToolbar::OnScrollStarted( const Vector2& position )
{
  if( mFirstScrollEnd )
  {
    mScrollView.SetOvershootEnabled( true );
  }
  mTableOfButtons.SetProperty( Actor::Property::SENSITIVE, false );
}

void TextSelectionToolbar::OnScrollCompleted( const Vector2& position )
{
  mFirstScrollEnd = true;
  mTableOfButtons.SetProperty( Actor::Property::SENSITIVE, true );
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
  mDividerIndexes.PushBack( mIndexInTable - 1u );
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

void TextSelectionToolbar::RaiseAbove( Layer target )
{
  mToolbarLayer.RaiseAbove( target );
}

void TextSelectionToolbar::SetScrollBarPadding( const Vector2& padding )
{
  mScrollBarPadding = padding;
  if( mScrollBar )
  {
    mScrollBar.SetPosition( mScrollBarPadding.x, -mScrollBarPadding.y );
  }

  RelayoutRequest();
}

void TextSelectionToolbar::ScrollTo( const Vector2& position )
{
  mFirstScrollEnd = false;
  mScrollView.SetOvershootEnabled( false );
  mScrollView.ScrollTo( position, 0.f );
}

void TextSelectionToolbar::ConfigureScrollview( const Property::Map& properties )
{
  // Set any properties specified for the label by iterating through all property key-value pairs.
  for( unsigned int i = 0, mapCount = properties.Count(); i < mapCount; ++i )
  {
    const StringValuePair& propertyPair( properties.GetPair( i ) );

    // Convert the property string to a property index.
    Property::Index setPropertyIndex = mScrollView.GetPropertyIndex( propertyPair.first );
    if( setPropertyIndex != Property::INVALID_INDEX )
    {
      // Convert the string representation of a color into a Vector4
      if( setPropertyIndex == Toolkit::Scrollable::Property::OVERSHOOT_EFFECT_COLOR )
      {
        Vector4 color;
        if( ConvertPropertyToColor( propertyPair.second, color ) )
        {
          mScrollView.SetOvershootEffectColor( color );
        }
      }
      else
      {
        // If the conversion worked, we have a valid property index,
        // Set the property to the new value.
        mScrollView.SetProperty( setPropertyIndex, propertyPair.second );
      }
    }
  }

  RelayoutRequest();
}

const Vector2& TextSelectionToolbar::GetScrollBarPadding() const
{
  return mScrollBarPadding;
}

TextSelectionToolbar::TextSelectionToolbar()
: Control( ControlBehaviour( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ) ),
  mMaxSize (),
  mScrollBarPadding( DEFAULT_SCROLL_BAR_PADDING ),
  mIndexInTable( 0 ),
  mDividerIndexes(),
  mFirstScrollEnd( false )
{
}

TextSelectionToolbar::~TextSelectionToolbar()
{
  mRulerX.Reset();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
