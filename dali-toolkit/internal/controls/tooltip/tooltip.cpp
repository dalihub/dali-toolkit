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
#include <dali-toolkit/internal/controls/tooltip/tooltip.h>

// EXTERNAL INCLUDES
#include <cmath>

#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/scripting/enum-helper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/controls/tooltip/tooltip-properties.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/controls/popup/popup-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

DALI_ENUM_TO_STRING_TABLE_BEGIN( TOOLTIP_POSITION )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Tooltip::Position, ABOVE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Tooltip::Position, BELOW )
DALI_ENUM_TO_STRING_WITH_SCOPE( Toolkit::Tooltip::Position, HOVER_POINT )
DALI_ENUM_TO_STRING_TABLE_END( TOOLTIP_POSITION )

const float MILLISECONDS_PER_SECOND = 1000.0f;

const char * const PROPERTY_CONTENT_NAME              = "content";
const char * const PROPERTY_LAYOUT_NAME               = "layout";
const char * const PROPERTY_WAIT_TIME_NAME            = "waitTime";
const char * const PROPERTY_BACKGROUND_NAME           = "background";
const char * const PROPERTY_TAIL_NAME                 = "tail";
const char * const PROPERTY_POSITION_NAME             = "position";
const char * const PROPERTY_HOVER_POINT_OFFSET_NAME   = "hoverPointOffset";
const char * const PROPERTY_MOVEMENT_THRESHOLD        = "movementThreshold";
const char * const PROPERTY_DISAPPEAR_ON_MOVEMENT     = "disappearOnMovement";

const char * const PROPERTY_BACKGROUND_VISUAL         = "visual";
const char * const PROPERTY_BACKGROUND_BORDER         = "border";

const char * const PROPERTY_TAIL_VISIBILITY           = "visibility";
const char * const PROPERTY_TAIL_ABOVE_VISUAL         = "aboveVisual";
const char * const PROPERTY_TAIL_BELOW_VISUAL         = "belowVisual";

} // unnamed namespace

TooltipPtr Tooltip::New( Toolkit::Control control )
{
  return new Tooltip( control );
}

void Tooltip::SetProperties( const Property::Value& value )
{
  Toolkit::Control control = mControl.GetHandle();
  if( control )
  {
    Property::Map* properties = value.GetMap();
    if( properties )
    {
      const Property::Map::SizeType count = properties->Count();
      for( Property::Map::SizeType position = 0; position < count; ++position )
      {
        KeyValuePair keyValue = properties->GetKeyValue( position );
        Property::Key& key = keyValue.first;
        Property::Value& value = keyValue.second;

        if( key == Toolkit::Tooltip::Property::CONTENT || key == PROPERTY_CONTENT_NAME )
        {
          SetContent( control, value );
        }
        else if( key == Toolkit::Tooltip::Property::LAYOUT || key == PROPERTY_LAYOUT_NAME )
        {
          value.Get( mLayout );
        }
        else if( key == Toolkit::Tooltip::Property::WAIT_TIME || key == PROPERTY_WAIT_TIME_NAME )
        {
          float waitTime = 0.0f;
          if( value.Get( waitTime ) )
          {
            mWaitTime = waitTime * MILLISECONDS_PER_SECOND;
          }
        }
        else if( key == Toolkit::Tooltip::Property::BACKGROUND || key == PROPERTY_BACKGROUND_NAME )
        {
          SetBackground( value );
        }
        else if( key == Toolkit::Tooltip::Property::TAIL || key == PROPERTY_TAIL_NAME )
        {
          SetTail( value );
        }
        else if( key == Toolkit::Tooltip::Property::POSITION || key == PROPERTY_POSITION_NAME )
        {
          Scripting::GetEnumerationProperty< Toolkit::Tooltip::Position::Type >( value, TOOLTIP_POSITION_TABLE, TOOLTIP_POSITION_TABLE_COUNT, mPositionType );
        }
        else if( key == Toolkit::Tooltip::Property::HOVER_POINT_OFFSET || key == PROPERTY_HOVER_POINT_OFFSET_NAME )
        {
          value.Get( mHoverPointOffset );
        }
        else if( key == Toolkit::Tooltip::Property::MOVEMENT_THRESHOLD || key == PROPERTY_MOVEMENT_THRESHOLD )
        {
          value.Get( mMovementThreshold );
        }
        else if( key == Toolkit::Tooltip::Property::DISAPPEAR_ON_MOVEMENT || key == PROPERTY_DISAPPEAR_ON_MOVEMENT )
        {
          value.Get( mDisappearOnMovement );
        }
      }
    }
    else
    {
      Property::Type type = value.GetType();
      if( ( value.GetType() == Property::STRING ) || ( type == Property::ARRAY ) )
      {
        SetContent( control, value );
      }
    }
  }
}

void Tooltip::CreatePropertyMap( Property::Map& map ) const
{
  if( ! mContentTextVisual.Empty() )
  {
    Property::Map content = mContentTextVisual; // Need this copy as there's no Value constructor which takes in a 'const Property::Map&'.
    map.Insert( Toolkit::Tooltip::Property::CONTENT, content );
  }
  else if( ! mContentArray.Empty() )
  {
    Property::Array content = mContentArray; // Need this copy as there's no Value constructor which takes in a 'const Property::Array&'.
    map.Insert( Toolkit::Tooltip::Property::CONTENT, content );
  }

  map.Insert( Toolkit::Tooltip::Property::LAYOUT, mLayout );
  map.Insert( Toolkit::Tooltip::Property::WAIT_TIME, static_cast<float>( mWaitTime ) / MILLISECONDS_PER_SECOND );
  map.Insert( Toolkit::Tooltip::Property::BACKGROUND,
              Property::Map().Add( Toolkit::Tooltip::Background::Property::VISUAL, mBackgroundImage )
                             .Add( Toolkit::Tooltip::Background::Property::BORDER, mBackgroundBorder ) );
  map.Insert( Toolkit::Tooltip::Property::TAIL,
              Property::Map().Add( Toolkit::Tooltip::Tail::Property::VISIBILITY, mTailVisibility )
                             .Add( Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL, mTailImages[ Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL ])
                             .Add( Toolkit::Tooltip::Tail::Property::BELOW_VISUAL, mTailImages[ Toolkit::Tooltip::Tail::Property::BELOW_VISUAL ]) );
  map.Insert( Toolkit::Tooltip::Property::POSITION, mPositionType );
  map.Insert( Toolkit::Tooltip::Property::HOVER_POINT_OFFSET, mHoverPointOffset );
  map.Insert( Toolkit::Tooltip::Property::MOVEMENT_THRESHOLD, mMovementThreshold );
  map.Insert( Toolkit::Tooltip::Property::DISAPPEAR_ON_MOVEMENT, mDisappearOnMovement );
}

Tooltip::Tooltip( Toolkit::Control control )
: mPopup(),
  mTooltipTimer(),
  mControl( control ),
  mContentTextVisual(),
  mTailImages(),
  mContentArray(),
  mBackgroundBorder( 0, 0, 0, 0 ),
  mLayout( 1, 1 ),
  mHoverPoint(),
  mHoverPointOffset( 10.0f, 10.0f ),
  mBackgroundImage(),
  mMovementThreshold( 5.0f ),
  mWaitTime( 500 ),
  mPositionType( Toolkit::Tooltip::Position::ABOVE ),
  mTailVisibility( false ),
  mDisappearOnMovement( false ),
  mSignalsConnected( false )
{
  mTailImages[ Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL ] = "";
  mTailImages[ Toolkit::Tooltip::Tail::Property::BELOW_VISUAL ] = "";
}

Tooltip::~Tooltip()
{
  if( mPopup )
  {
    mPopup.Unparent();
    mPopup.Reset();
  }
}

void Tooltip::SetContent( Toolkit::Control& control, const Property::Value& value )
{
  // Delete popup & timer

  if( mTooltipTimer )
  {
    mTooltipTimer.Stop();
    mTooltipTimer.Reset();
  }

  if( mPopup )
  {
    mPopup.Unparent();
    mPopup.Reset();
  }

  bool connectSignals = false;

  Property::Type type = value.GetType();
  if( type == Property::MAP )
  {
    Property::Map* map = value.GetMap();
    if( map )
    {
      mContentTextVisual.Merge( *map );

      Property::Value* typeValue = map->Find( Toolkit::Visual::Property::TYPE, VISUAL_TYPE );
      if( typeValue )
      {
        // Set to an invalid value so it definitely changes if set in Scripting::GetEnumerationProperty
        Toolkit::DevelVisual::Type visualType = static_cast< Toolkit::DevelVisual::Type >( -1 );

        if( Scripting::GetEnumerationProperty( *typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType ) )
        {
          if( visualType == Toolkit::DevelVisual::TEXT )
          {
            // Visual Type is text, ensure we have a the TEXT property set before we connect to the signals.

            if( map->Find( Toolkit::TextVisual::Property::TEXT, TEXT_PROPERTY ) )
            {
              mContentArray.Clear();
              connectSignals = true;
            }
          }
          else
          {
            // Visual Type is not text, so connect to the signals as we're displaying a non text visual.

            mContentArray.Clear();
            connectSignals = true;
          }
        }
      }
    }
  }
  else if( type == Property::ARRAY )
  {
    if( value.Get( mContentArray ) )
    {
      mContentTextVisual.Clear();
      connectSignals = true;
    }
  }
  else if( type == Property::STRING )
  {
    std::string text;
    if( value.Get( text ) )
    {
      mContentTextVisual[ Toolkit::TextVisual::Property::TEXT ] = text;
      mContentTextVisual[ Toolkit::Visual::Property::TYPE ] = DevelVisual::TEXT;
      mContentArray.Clear();
      connectSignals = true;
    }
  }

  if( connectSignals && ! mSignalsConnected )
  {
    control.HoveredSignal().Connect( this, &Tooltip::OnHovered );
    control.SetLeaveRequired( true );
    mSignalsConnected = true;
  }
}

void Tooltip::SetBackground( const Property::Value& value )
{
  Property::Type type = value.GetType();

  if( type == Property::STRING )
  {
    value.Get( mBackgroundImage );
    mBackgroundBorder.Set( 0, 0, 0, 0 );
  }
  else if( type == Property::MAP )
  {
    Property::Map* map = value.GetMap();
    if( map )
    {
      const Property::Map::SizeType count = map->Count();
      for( Property::Map::SizeType position = 0; position < count; ++position )
      {
        KeyValuePair keyValue = map->GetKeyValue( position );
        Property::Key& key = keyValue.first;
        Property::Value& value = keyValue.second;

        if( key == Toolkit::Tooltip::Background::Property::VISUAL || key == PROPERTY_BACKGROUND_VISUAL )
        {
          value.Get( mBackgroundImage );
        }
        else if( key == Toolkit::Tooltip::Background::Property::BORDER || key == PROPERTY_BACKGROUND_BORDER )
        {
          if( ! value.Get( mBackgroundBorder ) )
          {
            // If not a Property::RECTANGLE, then check if it's a Vector4 and set it accordingly
            Vector4 valueVector4;
            if( value.Get( valueVector4 ) )
            {
              mBackgroundBorder.left   = valueVector4.x;
              mBackgroundBorder.right  = valueVector4.y;
              mBackgroundBorder.bottom = valueVector4.z;
              mBackgroundBorder.top    = valueVector4.w;
            }
          }
        }
      }
    }
  }
}

void Tooltip::SetTail( const Property::Value& value )
{
  Property::Type type = value.GetType();

  if( type == Property::BOOLEAN )
  {
    value.Get( mTailVisibility );
  }
  else if( type == Property::MAP )
  {
    Property::Map map;
    if( value.Get( map ) )
    {
      const Property::Map::SizeType count = map.Count();
      for( Property::Map::SizeType position = 0; position < count; ++position )
      {
        KeyValuePair keyValue = map.GetKeyValue( position );
        Property::Key& key = keyValue.first;
        Property::Value& value = keyValue.second;

        // Set the values manually rather than merging so that we only have to deal with Property indices when creating the actual tooltip.

        if( key == Toolkit::Tooltip::Tail::Property::VISIBILITY || key == PROPERTY_TAIL_VISIBILITY )
        {
          value.Get( mTailVisibility );
        }
        else if( key == Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL || key == PROPERTY_TAIL_ABOVE_VISUAL )
        {
          std::string path;
          if( value.Get( path ) )
          {
            mTailImages[ Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL ] = path;
          }
        }
        else if( key == Toolkit::Tooltip::Tail::Property::BELOW_VISUAL || key == PROPERTY_TAIL_BELOW_VISUAL )
        {
          std::string path;
          if( value.Get( path ) )
          {
            mTailImages[ Toolkit::Tooltip::Tail::Property::BELOW_VISUAL ] = path;
          }
        }
      }
    }
  }
}

bool Tooltip::OnHovered( Actor /* actor */, const HoverEvent& hover )
{
  const TouchPoint::State state = hover.points[0].state;
  switch( state )
  {
    case TouchPoint::Started:
    case TouchPoint::Motion:
    {
      if( ! mPopup )
      {
        if( ! mTooltipTimer )
        {
          mHoverPoint = hover.points[ 0 ].screen;
          mTooltipTimer = Timer::New( mWaitTime );
          mTooltipTimer.TickSignal().Connect( this, &Tooltip::OnTimeout );
          mTooltipTimer.Start();
        }
        else
        {
          Vector2 movement = mHoverPoint - hover.points[ 0 ].screen;
          if( std::abs( movement.Length() ) > mMovementThreshold )
          {
            mTooltipTimer.Stop();
            mTooltipTimer.Reset();

            mHoverPoint = hover.points[ 0 ].screen;
            mTooltipTimer = Timer::New( mWaitTime );
            mTooltipTimer.TickSignal().Connect( this, &Tooltip::OnTimeout );
            mTooltipTimer.Start();
          }
        }
      }
      else if( mDisappearOnMovement )
      {
        // Popup is showing, and we're set to disappear on excessive movement so make sure we're still within the threshold.

        Vector2 movement = mHoverPoint - hover.points[ 0 ].screen;
        if( std::abs( movement.Length() ) > mMovementThreshold )
        {
          // Exceeding the threshold, hide the popup.

          if( mTooltipTimer )
          {
            mTooltipTimer.Stop();
            mTooltipTimer.Reset();
          }
          if( mPopup )
          {
            mPopup.Unparent();
            mPopup.Reset();
          }
        }
      }
      break;
    }
    case TouchPoint::Finished:
    case TouchPoint::Leave:
    case TouchPoint::Interrupted:
    {
      if( mTooltipTimer )
      {
        mTooltipTimer.Stop();
        mTooltipTimer.Reset();
      }
      if( mPopup )
      {
        mPopup.Unparent();
        mPopup.Reset();
      }
      break;
    }

    case TouchPoint::Stationary:
    case TouchPoint::Last:
    {
      break;
    }
  }

  return true;
}

bool Tooltip::OnTimeout()
{
  Toolkit::Control control = mControl.GetHandle();
  if( ! mPopup && control )
  {
    mPopup = Toolkit::Popup::New();

    // General set up of popup
    mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );
    mPopup.SetProperty( Toolkit::Popup::Property::CONTEXTUAL_MODE, "NON_CONTEXTUAL" );
    mPopup.SetProperty( Toolkit::Popup::Property::ANIMATION_MODE, "NONE" );
    mPopup.SetProperty( Toolkit::Popup::Property::BACKING_ENABLED, false ); // Disable the dimmed backing.
    mPopup.SetProperty( Toolkit::Popup::Property::TOUCH_TRANSPARENT, true ); // Let events pass through the popup
    mPopup.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mPopup.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    // Background
    mPopup.SetProperty( Toolkit::Popup::Property::POPUP_BACKGROUND_IMAGE, mBackgroundImage );
    mPopup.SetProperty( Toolkit::Popup::Property::POPUP_BACKGROUND_BORDER, mBackgroundBorder );

    // Tail
    mPopup.SetProperty( Toolkit::Popup::Property::TAIL_VISIBILITY, mTailVisibility );
    mPopup.SetProperty( Toolkit::Popup::Property::TAIL_UP_IMAGE,   mTailImages[ Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL ] );
    mPopup.SetProperty( Toolkit::Popup::Property::TAIL_DOWN_IMAGE, mTailImages[ Toolkit::Tooltip::Tail::Property::BELOW_VISUAL ] );

    Vector3 tailPosition;
    switch( mPositionType )
    {
      case Toolkit::Tooltip::Position::HOVER_POINT:
      case Toolkit::Tooltip::Position::BELOW:
      {
        tailPosition = Vector3( 0.5f, 0.0f, 0.0 );
        break;
      }

      case Toolkit::Tooltip::Position::ABOVE:
      {
        tailPosition = Vector3( 0.5f, 1.0f, 0.0 );
        break;
      }
    }
    mPopup.SetProperty( Toolkit::Popup::Property::TAIL_POSITION, tailPosition );

    // Content
    Actor content;
    if( ! mContentTextVisual.Empty() )
    {
      content = Toolkit::Control::New();
      content.SetProperty( Toolkit::Control::Property::BACKGROUND, mContentTextVisual );
    }
    else if( ! mContentArray.Empty() )
    {
      const unsigned int visuals = mContentArray.Size();
      unsigned int rows = mLayout.x;
      unsigned int columns = mLayout.y;
      if( Equals( mLayout.x, 1.0f, Math::MACHINE_EPSILON_1 ) &&
          Equals( mLayout.y, 1.0f, Math::MACHINE_EPSILON_1 ) &&
          visuals > 1 )
      {
        rows = mLayout.x;
        columns = visuals;
      }

      Toolkit::TableView tableView = Toolkit::TableView::New( rows, columns );
      tableView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

      for( unsigned int currentContent = 0, currentRow = 0; currentRow < rows && currentContent < visuals; ++currentRow )
      {
        tableView.SetFitHeight( currentRow );
        for( unsigned int currentColumn = 0; currentColumn < columns && currentContent < visuals; ++currentColumn )
        {
          Actor child = Toolkit::Control::New();
          child.SetProperty( Toolkit::Control::Property::BACKGROUND, mContentArray[ currentContent ] );

          Toolkit::TableView::CellPosition cellPosition( currentRow, currentColumn );
          tableView.AddChild( child, cellPosition );
          tableView.SetCellAlignment( cellPosition, HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
          tableView.SetFitWidth( currentColumn );

          ++currentContent;
        }
      }
      content = tableView;
    }
    mPopup.SetContent( content );

    // Connect to the relayout signal of the background of the popup as at that point we have the full size
    Actor popupBackground = GetImpl( mPopup ).GetPopupBackgroundImage();
    if( popupBackground )
    {
      popupBackground.OnRelayoutSignal().Connect( this, &Tooltip::OnRelayout );
    }

    mPopup.SetDisplayState( Toolkit::Popup::SHOWN );

    Stage::GetCurrent().Add( mPopup );
  }

  return false;
}

void Tooltip::OnRelayout( Actor actor )
{
  if( mPopup && actor )
  {
    float popupWidth = actor.GetRelayoutSize( Dimension::WIDTH );
    float popupHeight = actor.GetRelayoutSize( Dimension::HEIGHT );
    float tailHeight = 0.0f;
    Actor tail;

    if( mTailVisibility )
    {
      // Popup's background has the tail, we want to know the tail size as well.
      if( actor.GetChildCount() )
      {
        tail = actor.GetChildAt( 0 );
        if( tail )
        {
          tailHeight = tail.GetRelayoutSize( Dimension::HEIGHT );
        }
      }
    }

    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Vector3 position;

    switch( mPositionType )
    {
      case Toolkit::Tooltip::Position::HOVER_POINT:
      {
        position = mHoverPoint + mHoverPointOffset;
        position.y += tailHeight;
        break;
      }

      case Toolkit::Tooltip::Position::ABOVE:
      {
        Toolkit::Control control = mControl.GetHandle();
        if( control )
        {
          Vector3 worldPos = control.GetCurrentWorldPosition();
          float height = control.GetRelayoutSize( Dimension::HEIGHT );

          position.x = stageSize.width * 0.5f + worldPos.x - popupWidth * 0.5f;
          position.y = stageSize.height * 0.5f + worldPos.y - height * 0.5f - popupHeight * 1.0f - tailHeight;
        }
        break;
      }

      case Toolkit::Tooltip::Position::BELOW:
      {
        Toolkit::Control control = mControl.GetHandle();
        if( control )
        {
          Vector3 worldPos = control.GetCurrentWorldPosition();
          float height = control.GetRelayoutSize( Dimension::HEIGHT );

          position.x = stageSize.width * 0.5f + worldPos.x - popupWidth * 0.5f;
          position.y = stageSize.height * 0.5f + worldPos.y + height * 0.5f + tailHeight;
        }
        break;
      }
    }

    // Ensure the Popup is still on the screen

    if( position.x < 0.0f )
    {
      position.x = 0.0f;
    }
    else if( ( position.x + popupWidth ) > stageSize.width )
    {
      position.x -= position.x + popupWidth - stageSize.width;
    }

    bool yPosChanged = false;
    if( position.y < 0.0f )
    {
      yPosChanged = true;
      position.y = 0.0f;
    }
    else if( ( position.y + popupHeight ) > stageSize.height )
    {
      yPosChanged = true;
      position.y -= position.y + popupHeight - stageSize.height;
    }

    if( yPosChanged && tail )
    {
      // If we change the y position, then the tail may be shown pointing to the wrong control so just hide it.
      tail.SetVisible( false );
    }

    mPopup.SetPosition( position );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
