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
#include "tool-bar-impl.h"

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>

// EXTERNAL INCLUDES

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
  return Toolkit::ToolBar::New();
}

TypeRegistration mType( typeid(Toolkit::ToolBar), typeid(Toolkit::Control), Create );

const float DEFAULT_RELATIVE_SIZE( 0.1f );
const Toolkit::Alignment::Type DEFAULT_ALIGNMENT( Toolkit::Alignment::HorizontalLeft );
} // namespace

Toolkit::ToolBar ToolBar::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< ToolBar > internalToolBar = new ToolBar();

  // Pass ownership to Toolkit::View
  Toolkit::ToolBar toolBar( *internalToolBar );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalToolBar->Initialize();

  return toolBar;
}

void ToolBar::SetBackground( Actor background )
{
  Lock lock( mInitializing );

  // ToolBar image
  background.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
  background.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  background.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

  RenderableActor renderableActor = RenderableActor::DownCast( background );
  if ( renderableActor )
  {
    renderableActor.SetSortModifier( 1.f );
  }

  Self().Add( background );
}

void ToolBar::AddControl( Actor control, float relativeSize, Toolkit::Alignment::Type alignment, const Toolkit::Alignment::Padding& padding )
{
  // Work out index and update bases and offsets for further insertions.
  unsigned int index = 0;
  switch( alignment )
  {
    case Toolkit::Alignment::HorizontalLeft:
    {
      index = mLeftOffset;
      ++mLeftOffset;
      ++mCenterBase;
      ++mRightBase;
      break;
    }
    case Toolkit::Alignment::HorizontalCenter:
    {
      index = mCenterBase + mCenterOffset;
      ++mCenterOffset;
      ++mRightBase;
      break;
    }
    case Toolkit::Alignment::HorizontalRight:
    {
      index = mRightBase - mRightOffset;
      ++mRightBase;
      ++mRightOffset;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( false );
    }
  }

  // Create a new column for the new control.
  mLayout.InsertColumn( index );

  // Create an alignment container where to place the control.
  Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( alignment );
  alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
  alignmentContainer.SetPadding( padding );
  alignmentContainer.Add( control );

  // Insert the control in the table view.
  mLayout.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 0, index ) );
  mLayout.SetRelativeWidth( index, relativeSize );

  // Relate control and alignmentContainer in order to allow removing controls.
  mControls[control] = alignmentContainer;

  // Update accumulated relative space.
  mAccumulatedRelativeSpace += relativeSize;

  // Update spaces between left, center and right groups of controls.
  switch( alignment )
  {
    case Toolkit::Alignment::HorizontalLeft:
    {
      mLeftRelativeSpace -= relativeSize;
      if ( mLeftRelativeSpace < 0.f )
      {
        mLeftRelativeSpace = 0.f;
      }
      break;
    }
    case Toolkit::Alignment::HorizontalCenter:
    {
      mLeftRelativeSpace -= 0.5f * relativeSize;
      if ( mLeftRelativeSpace < 0.f )
      {
        mLeftRelativeSpace = 0.f;
      }
      mRightRelativeSpace -= 0.5f * relativeSize;
      if ( mRightRelativeSpace < 0.f )
      {
        mRightRelativeSpace = 0.f;
      }
      break;
    }
    case Toolkit::Alignment::HorizontalRight:
    {
      mRightRelativeSpace -= relativeSize;
      if ( mRightRelativeSpace < 0.f )
      {
        mRightRelativeSpace = 0.f;
      }
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( false );
    }
  }

  mLayout.SetRelativeWidth( mLeftOffset, mLeftRelativeSpace );
  mLayout.SetRelativeWidth( mCenterBase + mCenterOffset, mRightRelativeSpace );
}

void ToolBar::RemoveControl( Actor control )
{
  Toolkit::TableView::CellPosition position;

  // Find the alignment where the control is placed.
  std::map<Actor,Toolkit::Alignment>::iterator it = mControls.find( control );

  if( ( it != mControls.end() ) && ( mLayout.FindChildPosition( it->second, position ) ) )
  {
    // Update accumulated relative space.
    mAccumulatedRelativeSpace -= mLayout.GetRelativeWidth( position.columnIndex );

    // Update spaces between left, center and right groups of controls.
    if( 1.0 > mAccumulatedRelativeSpace )
    {
      Toolkit::Alignment::Type alignment = Toolkit::Alignment::HorizontalLeft;
      if( position.columnIndex < mLeftOffset )
      {
        alignment = Toolkit::Alignment::HorizontalLeft;
      }
      else if( ( position.columnIndex > mLeftOffset ) && ( position.columnIndex < mCenterBase + mCenterOffset ) )
      {
        alignment = Toolkit::Alignment::HorizontalCenter;
      }
      else if( position.columnIndex > mCenterBase + mCenterOffset )
      {
        alignment = Toolkit::Alignment::HorizontalRight;
      }
      else
      {
        DALI_ASSERT_ALWAYS( false );
      }

      float relativeSize = mLayout.GetRelativeWidth( position.columnIndex );

      switch( alignment )
      {
        case Toolkit::Alignment::HorizontalLeft:
        {
          mLeftRelativeSpace += relativeSize;
          if ( mLeftRelativeSpace < 0.f )
          {
            mLeftRelativeSpace = 0.f;
          }
          break;
        }
        case Toolkit::Alignment::HorizontalCenter:
        {
          mLeftRelativeSpace += 0.5f * relativeSize;
          if ( mLeftRelativeSpace < 0.f )
          {
            mLeftRelativeSpace = 0.f;
          }
          mRightRelativeSpace += 0.5f * relativeSize;
          if ( mRightRelativeSpace < 0.f )
          {
            mRightRelativeSpace = 0.f;
          }
          break;
        }
        case Toolkit::Alignment::HorizontalRight:
        {
          mRightRelativeSpace += relativeSize;
          if ( mRightRelativeSpace < 0.f )
          {
            mRightRelativeSpace = 0.f;
          }
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( false );
        }
      }
      mLayout.SetRelativeWidth( mLeftOffset, mLeftRelativeSpace );
      mLayout.SetRelativeWidth( mCenterBase + mCenterOffset, mRightRelativeSpace );
    }

    // Remove alignment as parent of control.
    it->second.Remove( control );

    // Remove the relationship between control and alignment.
    mControls.erase( it );

    // Remove column from tableview.
    mLayout.DeleteColumn( position.columnIndex );

    // Update bases and offsets.
    if( position.columnIndex < mCenterBase )
    {
      // Control is on the left side. Decrease left offset and center and right bases.
      --mLeftOffset;
      --mCenterBase;
      --mRightBase;
    }
    else if( position.columnIndex < mCenterBase + mCenterOffset )
    {
      // Control is on the center side. Decrease center offset and right base.
      --mCenterOffset;
      --mRightBase;
    }
    else
    {
      // Control is on the right side. Decrease right base and right offset.
      --mRightBase;
      --mRightOffset;
    }
  }
}

ToolBar::ToolBar()
: Control( CONTROL_BEHAVIOUR_NONE ),
  mLayout(),
  mLeftOffset( 0 ),
  mCenterBase( 1 ),
  mCenterOffset( 0 ),
  mRightBase( 2 ),
  mRightOffset( 0 ),
  mLeftRelativeSpace( 0.5f ),
  mRightRelativeSpace( 0.5f ),
  mAccumulatedRelativeSpace( 0.f ),
  mInitializing( false ),
  mControls()
{
}

ToolBar::~ToolBar()
{
}

void ToolBar::OnInitialize()
{
  Lock lock( mInitializing );

  // Layout
  mLayout = Toolkit::TableView::New( 1, 1 );
  mLayout.SetPositionInheritanceMode( Dali::USE_PARENT_POSITION );

  Self().Add( mLayout );

  // Add two default actors to create spaces between controls grouped on the left, center and right.
  Actor leftSpace = Actor::New();
  Actor rightSpace = Actor::New();
  mLayout.AddChild( leftSpace, Toolkit::TableView::CellPosition( 0, 0 ) );
  mLayout.AddChild( rightSpace, Toolkit::TableView::CellPosition( 0, 1 ) );
  mLayout.SetRelativeWidth( 0, mLeftRelativeSpace );
  mLayout.SetRelativeWidth( 1, mRightRelativeSpace );
}

void ToolBar::OnControlChildAdd(Actor& child)
{
  if( !mInitializing )
  {
    // An actor is being added through the Actor's API.

    // Remove child from tool bar actor and insert it in table view with some 'default' values
    if ( child && child.GetParent() )
    {
      child.GetParent().Remove( child );
    }

    AddControl( child, DEFAULT_RELATIVE_SIZE, DEFAULT_ALIGNMENT, Toolkit::ToolBar::DEFAULT_PADDING );
  }

  // No OnControlChildRemove method required because Actors are added to the mLayout table view, so if an
  // actor is removed using the Actor::RemoveChild method it will not remove anything because the
  // actor is in mLayout not in Self().
}

void ToolBar::OnRelaidOut( Vector2 size, ActorSizeContainer& container )
{
  Relayout( mLayout, size, container );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
