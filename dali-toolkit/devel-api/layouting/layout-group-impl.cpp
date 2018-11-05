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
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/layouting/layout-group-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/layouting/size-negotiation-mapper.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutGroup::LayoutGroup()
: mImpl( new LayoutGroup::Impl() ),
  mSlotDelegate(this)
{
}

LayoutGroupPtr LayoutGroup::New( Handle& owner )
{
  LayoutGroupPtr layoutPtr = new LayoutGroup();
  return layoutPtr;
}

LayoutGroup::~LayoutGroup()
{
  // An object with a unique_ptr to an opaque structure must define it's destructor in the translation unit
  // where the opaque structure is defined. It cannot use the default method in the header file.
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::Add( LayoutItem& child )
{
  LayoutParent* oldParent = child.GetParent();
  if( oldParent )
  {
    LayoutGroupPtr parentGroup( dynamic_cast< LayoutGroup* >( oldParent ) );
    if( parentGroup )
    {
      parentGroup->Remove( child );
    }
  }

  Impl::ChildLayout childLayout;
  childLayout.layoutId = mImpl->mNextLayoutId++;
  childLayout.child = &child;
  mImpl->mChildren.emplace_back( childLayout );

  child.SetParent( this );

  auto owner = child.GetOwner();

  // If the owner does not have any LayoutItem child properties, add them
  if( ! DevelHandle::DoesCustomPropertyExist( owner, Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ) )
  {
    // Set default properties for LayoutGroup and LayoutItem.
    // Deriving classes can override OnChildAdd() to add their own default properties
    GenerateDefaultChildPropertyValues( owner );
  }

  // Inform deriving classes that this child has been added
  OnChildAdd( *childLayout.child.Get() );

  // Now listen to future changes to the child properties.
  DevelHandle::PropertySetSignal(owner).Connect( this, &LayoutGroup::OnSetChildProperties );

  RequestLayout();

  return childLayout.layoutId;
}

void LayoutGroup::Remove( Toolkit::LayoutGroup::LayoutId childId )
{
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    if( iter->layoutId == childId )
    {
      RemoveChild( *iter->child.Get() );
      mImpl->mChildren.erase(iter);
      break;
    }
  }
  RequestLayout();
}

void LayoutGroup::Remove( LayoutItem& child )
{
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    if( iter->child.Get() == &child )
    {
      RemoveChild( *iter->child.Get() );
      mImpl->mChildren.erase(iter);
      break;
    }
  }
  RequestLayout();
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::Insert( LayoutItem& target, LayoutItem& child )
{
  LayoutParent* oldParent = child.GetParent();
  if( oldParent )
  {
    LayoutGroupPtr parentGroup( dynamic_cast< LayoutGroup* >( oldParent ) );
    if( parentGroup )
    {
      parentGroup->Remove( child );
    }
  }

  // Find target position
  std::vector< Impl::ChildLayout >::iterator position;
  for( auto iter = mImpl->mChildren.begin(); iter != mImpl->mChildren.end(); ++iter )
  {
    if( iter->child.Get() == &target )
    {
      position = iter;
      break;
    }
  }

  Impl::ChildLayout childLayout;
  childLayout.layoutId = mImpl->mNextLayoutId++;
  childLayout.child = &child;
  mImpl->mChildren.insert( position, childLayout );

  child.SetParent( this );

  auto owner = child.GetOwner();

  // Inform deriving classes that this child has been added
  OnChildAdd( *childLayout.child.Get() );

  // Now listen to future changes to the child properties.
  DevelHandle::PropertySetSignal(owner).Connect( this, &LayoutGroup::OnSetChildProperties );

  RequestLayout();

  return childLayout.layoutId;
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::Move( LayoutItem& target, LayoutItem& child )
{
  // Remove child from the previous position
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    if( iter->child.Get() == &child )
    {
      mImpl->mChildren.erase( iter );
      break;
    }
  }

  // Find target position
  std::vector< Impl::ChildLayout >::iterator position;
  for( auto iter = mImpl->mChildren.begin(); iter != mImpl->mChildren.end(); ++iter )
  {
    if( iter->child.Get() == &target )
    {
      position = iter;
      break;
    }
  }

  Impl::ChildLayout childLayout;
  childLayout.layoutId = mImpl->mNextLayoutId++;
  childLayout.child = &child;
  mImpl->mChildren.insert( position, childLayout );

  RequestLayout();

  return childLayout.layoutId;
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::MoveBack( LayoutItem& child )
{
  // Remove child from the previous position
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; ++iter )
  {
    if( iter->child.Get() == &child )
    {
      mImpl->mChildren.erase( iter );
      break;
    }
  }

  Impl::ChildLayout childLayout;
  childLayout.layoutId = mImpl->mNextLayoutId++;
  childLayout.child = &child;
  mImpl->mChildren.emplace_back( childLayout );

  RequestLayout();

  return childLayout.layoutId;
}

void LayoutGroup::RemoveAll()
{
  for( auto iter = mImpl->mChildren.begin() ; iter != mImpl->mChildren.end() ; )
  {
    RemoveChild( *iter->child.Get() );
    iter = mImpl->mChildren.erase(iter);
  }
}

unsigned int LayoutGroup::GetChildCount() const
{
  return mImpl->mChildren.size();
}

LayoutItemPtr LayoutGroup::GetChildAt( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( index < mImpl->mChildren.size() );
  return mImpl->mChildren[ index ].child;
}

LayoutItemPtr LayoutGroup::GetChild( Toolkit::LayoutGroup::LayoutId childId ) const
{
  for( auto&& childLayout : mImpl->mChildren )
  {
    if( childLayout.layoutId == childId )
    {
      return childLayout.child;
    }
  }
  return NULL;
}

Toolkit::LayoutGroup::LayoutId LayoutGroup::GetChildId( LayoutItem& child ) const
{
  for( auto&& childLayout : mImpl->mChildren )
  {
    if( childLayout.child.Get() == &child )
    {
      return childLayout.layoutId;
    }
  }
  return Toolkit::LayoutGroup::UNKNOWN_ID;
}

void LayoutGroup::OnChildAdd( LayoutItem& child )
{
}

void LayoutGroup::OnChildRemove( LayoutItem& child )
{
}

void LayoutGroup::DoInitialize()
{
}

void LayoutGroup::DoRegisterChildProperties( const std::string& containerType )
{
}

void LayoutGroup::OnSetChildProperties( Handle& handle, Property::Index index, Property::Value value )
{
  DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LayoutGroup::OnSetChildProperties property(" << handle.GetPropertyName(index) << ")\n" );

  if ( ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) &&
         ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX ) )
       ||
       ( index == Toolkit::Control::Property::MARGIN || index == Toolkit::Control::Property::PADDING ) )
  {
    // If any child properties are set, must perform relayout
    for( auto&& child : mImpl->mChildren )
    {
      if( child.child->GetOwner() == handle )
      {
        child.child->RequestLayout();
        break;
      }
    }
  }
}

void LayoutGroup::GenerateDefaultChildPropertyValues( Handle child )
{
  child.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION,
                     Toolkit::ChildLayoutData::WRAP_CONTENT );
  child.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION,
                     Toolkit::ChildLayoutData::WRAP_CONTENT );
}

void LayoutGroup::MeasureChildren( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec)
{
  for( auto&& child : mImpl->mChildren )
  {
    //if( (child.mViewFlags & Impl::VISIBILITY_MASK) != Impl::GONE ) // Use owner visibility/enabled/ready
    {
      MeasureChild( child.child, widthMeasureSpec, heightMeasureSpec );
    }
  }
}

void LayoutGroup::MeasureChild( LayoutItemPtr child,
                                MeasureSpec parentWidthMeasureSpec,
                                MeasureSpec parentHeightMeasureSpec )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  auto childOwner = child->GetOwner();

  auto control = Toolkit::Control::DownCast( childOwner );

#if defined( DEBUG_ENABLED )
  if ( control )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChild(%s) natural size(%f, %f)\n",
                   control.GetName().c_str(), control.GetNaturalSize().width, control.GetNaturalSize().height );
  }
#endif


  // Get last stored width and height specifications for the child
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChild desiredWidth(%d) desiredHeight(%d)\n", desiredWidth, desiredHeight );

  auto padding = GetPadding(); // Padding of this layout's owner, not of the child being measured.

  const MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( parentWidthMeasureSpec,
                                                                 padding.start + padding.end,
                                                                 desiredWidth);
  const MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( parentHeightMeasureSpec,
                                                                  padding.top + padding.bottom,
                                                                  desiredHeight);

  child->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
}

void LayoutGroup::MeasureChildWithMargins( LayoutItemPtr child,
                                           MeasureSpec parentWidthMeasureSpec, LayoutLength widthUsed,
                                           MeasureSpec parentHeightMeasureSpec, LayoutLength heightUsed)
{
  auto childOwner = child->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

  auto padding = GetPadding(); // Padding of this layout's owner, not of the child being measured.

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChildWithMargins desiredWidth(%d)\n",  desiredWidth );

  MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( parentWidthMeasureSpec,
                                                           LayoutLength( padding.start + padding.end ) +
                                                           widthUsed, desiredWidth );

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::MeasureChildWithMargins desiredHeight(%d)\n",  desiredHeight );

  MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( parentHeightMeasureSpec,
                                                            LayoutLength( padding.top + padding.bottom )+
                                                            heightUsed, desiredHeight );

  child->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
}


MeasureSpec LayoutGroup::GetChildMeasureSpec(
  MeasureSpec  measureSpec,
  LayoutLength padding,
  LayoutLength childDimension )
{
  auto specMode = measureSpec.GetMode();
  LayoutLength specSize = measureSpec.GetSize();

  LayoutLength size = std::max( LayoutLength(0), specSize - padding ); // reduce available size by the owners padding

  LayoutLength resultSize = 0;
  MeasureSpec::Mode resultMode = MeasureSpec::Mode::UNSPECIFIED;

  switch( specMode )
  {
    // Parent has imposed an exact size on us
    case MeasureSpec::Mode::EXACTLY:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec MeasureSpec::Mode::EXACTLY\n");
      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec childDimension MATCH_PARENT\n");

        // Child wants to be our size. So be it.
        resultSize = size;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec childDimension WRAP_CONTENT\n");

        // Child wants to determine its own size. It can't be
        // bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::Mode::AT_MOST;
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec childDimension UNSPECIFIED\n");
        resultSize = childDimension;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }

      break;
    }

      // Parent has imposed a maximum size on us
    case MeasureSpec::Mode::AT_MOST:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec MeasureSpec::Mode::AT_MOST\n");
      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size, but our size is not fixed.
        // Constrain child to not be bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::Mode::AT_MOST;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size. It can't be
        // bigger than us.
        resultSize = size;
        resultMode = MeasureSpec::Mode::AT_MOST;
      }
      else
      {
        // Child wants a specific size... so be it
        resultSize = childDimension + padding;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }

      break;
    }

      // Parent asked to see how big we want to be
    case MeasureSpec::Mode::UNSPECIFIED:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec MeasureSpec::Mode::UNSPECIFIED\n");

      if (childDimension == Toolkit::ChildLayoutData::MATCH_PARENT)
      {
        // Child wants to be our size... find out how big it should be
        resultSize = LayoutItem::Impl::sUseZeroUnspecifiedMeasureSpec ? LayoutLength(0) : size;
        resultMode = MeasureSpec::Mode::UNSPECIFIED;
      }
      else if (childDimension == Toolkit::ChildLayoutData::WRAP_CONTENT)
      {
        // Child wants to determine its own size.... find out how big
        // it should be
        resultSize = LayoutItem::Impl::sUseZeroUnspecifiedMeasureSpec ? LayoutLength(0) : size;
        resultMode = MeasureSpec::Mode::UNSPECIFIED;
      }
      else
      {
        // Child wants a specific size... let him have it
        resultSize = childDimension + padding;
        resultMode = MeasureSpec::Mode::EXACTLY;
      }
      break;
    }
  }

  DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LayoutGroup::GetChildMeasureSpec resultSize(" << resultSize << ")\n" );

  //noinspection ResourceType
  return MeasureSpec( resultSize, resultMode );
}


void LayoutGroup::OnInitialize()
{
  auto control = Toolkit::Control::DownCast( GetOwner() );

  if( control )
  {
    // Take ownership of existing children
    for( unsigned int childIndex = 0 ; childIndex < control.GetChildCount(); ++childIndex )
    {
      ChildAddedToOwnerImpl( control.GetChildAt( childIndex ) );
    }

    DevelActor::ChildAddedSignal( control ).Connect( mSlotDelegate, &LayoutGroup::ChildAddedToOwner );
    DevelActor::ChildRemovedSignal( control ).Connect( mSlotDelegate, &LayoutGroup::ChildRemovedFromOwner );
    DevelActor::ChildOrderChangedSignal( control ).Connect( mSlotDelegate, &LayoutGroup::ChildOrderChanged );
    DevelHandle::PropertySetSignal( control ).Connect( mSlotDelegate, &LayoutGroup::OnOwnerPropertySet );

    if( control.GetParent() )
    {
      auto parent = Toolkit::Control::DownCast( control.GetParent() );
      if( parent )
      {
        auto parentLayout = Toolkit::LayoutGroup::DownCast( DevelControl::GetLayout( parent ) );
        if( parentLayout )
        {
          Internal::LayoutGroup& parentLayoutImpl = GetImplementation( parentLayout );

          unsigned int count = parent.GetChildCount();
          unsigned int index = static_cast< unsigned int >( control.GetProperty< int >( DevelActor::Property::SIBLING_ORDER ) );

          // Find insertion position
          while( ++index < count )
          {
            auto sibling = Toolkit::Control::DownCast( parent.GetChildAt( index ) );
            if( sibling )
            {
              auto siblingLayout = DevelControl::GetLayout( sibling );
              if( siblingLayout )
              {
                Internal::LayoutItem& siblingLayoutImpl = GetImplementation( siblingLayout );
                parentLayoutImpl.Insert( siblingLayoutImpl, *this );
                break;
              }
            }
          }

          if( index >= count )
          {
            parentLayoutImpl.Add( *this );
          }
        }
      }
    }

    RequestLayout( Dali::Toolkit::LayoutTransitionData::LayoutTransitionType::ON_OWNER_SET );
  }
}

void LayoutGroup::OnRegisterChildProperties( const std::string& containerType )
{
  DoRegisterChildProperties( containerType );
}

void LayoutGroup::OnUnparent()
{
  // Remove children
  RemoveAll();

  auto control = Toolkit::Control::DownCast( GetOwner() );
  if( control )
  {
    DevelActor::ChildAddedSignal( control ).Disconnect( mSlotDelegate, &LayoutGroup::ChildAddedToOwner );
    DevelActor::ChildRemovedSignal( control ).Disconnect( mSlotDelegate, &LayoutGroup::ChildRemovedFromOwner );
    DevelActor::ChildOrderChangedSignal( control ).Disconnect( mSlotDelegate, &LayoutGroup::ChildOrderChanged );
    DevelHandle::PropertySetSignal( control ).Disconnect( mSlotDelegate, &LayoutGroup::OnOwnerPropertySet );
  }
}

void LayoutGroup::RemoveChild( LayoutItem& item )
{
  item.SetParent( nullptr );
  OnChildRemove( item );
}

void LayoutGroup::ChildAddedToOwner( Actor child )
{
  ChildAddedToOwnerImpl( child );
  RequestLayout( Dali::Toolkit::LayoutTransitionData::LayoutTransitionType::ON_CHILD_ADD );
}

void LayoutGroup::ChildAddedToOwnerImpl( Actor child )
{
  LayoutItemPtr childLayout;
  Toolkit::Control control = Toolkit::Control::DownCast( child );

#if defined(DEBUG_ENABLED)
  auto parent = Toolkit::Control::DownCast( GetOwner() );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner child control(%s) owner control(%s)\n",
                                              control?control.GetName().c_str():"Invalid",
                                              parent?parent.GetName().c_str():"Invalid" );
#endif

  if( control ) // Can only support adding Controls, not Actors to layout
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    childLayout = childControlDataImpl.GetLayout();

    if( ! childLayout )
    {
      // If the child doesn't already have a layout, then create a LayoutItem or LayoutGroup for it.
      // If control behaviour flag set to Layout then set a LayoutGroup.
      if( DevelControl::IsLayoutingRequired( control ) )
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner Creating default LayoutGroup for control:%s\n",
                                                    control?control.GetName().c_str():"Invalid" );
        childLayout = LayoutGroup::New( control );
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner Creating default LayoutItem for control:%s\n",
                                                    control?control.GetName().c_str():"Invalid" );
        childLayout = LayoutItem::New( control );
        childLayout->SetAnimateLayout( IsLayoutAnimated() ); // forces animation inheritance.
      }

      DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner child control:" <<  control.GetName() <<
                       " desiredWidth: " <<  control.GetNaturalSize().width <<
                       " desiredHeight:"  << control.GetNaturalSize().height );

      childControlDataImpl.SetLayout( *childLayout.Get() );

      Vector3 size = child.GetTargetSize();
      // If the size of the control is set explicitly make sure that the control size
      // stays the same after the layout except it is over written with match parent specs.
      if ( size.x != 0 )
      {
        childLayout->SetMinimumWidth( size.x );
      }

      if ( size.y != 0 )
      {
        childLayout->SetMinimumHeight( size.y );
      }
      // Default layout data will be generated by Add().
    }
    else
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::ChildAddedToOwner child(%s) already has a Layout\n", control.GetName().c_str() );
      LayoutGroupPtr layoutGroup( dynamic_cast< LayoutGroup* >( childLayout.Get() ) );
      if( !layoutGroup )
      {
        // Set only in case of leaf children
        childLayout->SetAnimateLayout( IsLayoutAnimated() );
      }
    }

    Add( *childLayout.Get() );
  }
}

void LayoutGroup::ChildRemovedFromOwner( Actor child )
{
  Toolkit::Control control = Toolkit::Control::DownCast( child );
  if( control )
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    auto childLayout = childControlDataImpl.GetLayout();
    if( childLayout )
    {
      Remove( *childLayout.Get() );
      RequestLayout( Dali::Toolkit::LayoutTransitionData::LayoutTransitionType::ON_CHILD_REMOVE );
    }
  }
}

void LayoutGroup::ChildOrderChanged( Actor child )
{
  Toolkit::Control childControl = Toolkit::Control::DownCast( child );
  if( childControl )
  {
    Internal::Control& childControlImpl = GetImplementation( childControl );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );

    auto childLayout = childControlDataImpl.GetLayout();
    if( childLayout )
    {
      Toolkit::Control control = Toolkit::Control::DownCast( GetOwner() );
      unsigned int count = control.GetChildCount();
      unsigned int index = static_cast< unsigned int >( childControl.GetProperty< int >( DevelActor::Property::SIBLING_ORDER ) );

      // Find insertion position
      while( ++index < count )
      {
        auto sibling = Toolkit::Control::DownCast( control.GetChildAt( index ) );
        if( sibling )
        {
          auto siblingLayout = DevelControl::GetLayout( sibling );
          if( siblingLayout )
          {
            Internal::LayoutItem& siblingLayoutImpl = GetImplementation( siblingLayout );
            Move( siblingLayoutImpl, *childLayout );
            return;
          }
        }
      }

      MoveBack( *childLayout );
    }
  }
}

void LayoutGroup::OnOwnerPropertySet( Handle& handle, Property::Index index, Property::Value value )
{
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutGroup::OnOwnerPropertySet\n");
  auto actor = Actor::DownCast( handle );
  if( actor &&
      (
        index == Actor::Property::LAYOUT_DIRECTION  ||
        index == Toolkit::Control::Property::PADDING  ||
        index == Toolkit::Control::Property::MARGIN
      )
    )
  {
    RequestLayout();
  }
}

void LayoutGroup::OnAnimationStateChanged( bool animateLayout )
{
  // Change children's animation state
  for( auto&& child : mImpl->mChildren )
  {
    LayoutGroupPtr parentGroup( dynamic_cast< LayoutGroup* >( child.child.Get() ) );
    if( ! parentGroup )
    {
      // Change state only in case of leaf children
      child.child->SetAnimateLayout( animateLayout );
    }
  }
}

void LayoutGroup::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  auto childCount = GetChildCount();

  DALI_LOG_STREAM( gLogFilter, Debug::Verbose,
                  "LayoutGroup::OnMeasure Actor Id:" <<  Actor::DownCast(GetOwner()).GetId() <<
                  " Owner:" <<  Actor::DownCast(GetOwner()).GetName() <<
                  " Child Count:" << childCount <<
                  " MeasureSpecs( width:"<<widthMeasureSpec<<", height:"<<heightMeasureSpec );

  auto widthMode = widthMeasureSpec.GetMode();
  auto heightMode = heightMeasureSpec.GetMode();
  LayoutLength widthSpecSize = widthMeasureSpec.GetSize();
  LayoutLength heightSpecSize = heightMeasureSpec.GetSize();

  bool exactWidth ( false );
  bool exactHeight ( false );

  // Layouting behaviour
  // EXACT, width and height as provided.
  // MATCH_PARENT, width and height that of parent
  // WRAP_CONTENT, take width of widest child and height size of longest child (within given limit)
  // UNSPECIFIED, take width of widest child and height size of longest child.

  LayoutLength layoutWidth( 0 );
  LayoutLength layoutHeight( 0 );

  // If LayoutGroup has children then measure children to get max dimensions
  if ( childCount > 0 )
  {
    for( unsigned int i=0; i<childCount; ++i )
    {
      auto childLayout = GetChildAt( i );
      if( childLayout )
      {
        auto childControl = Toolkit::Control::DownCast(childLayout->GetOwner());

        // If child control has children check if a ResizePolicy is set on it.  A LayoutItem could be a legacy container.
        // A legacy container would need it's ResizePolicy to be applied as a MeasureSpec.

        // Check below will be true for legacy containers and controls with layout required set.
        // Other layouts will have their own OnMeasure (a checked requirement) hence not execute LayoutGroup::OnMeasure.
        // Controls which have set layout required will not be legacy controls hence should not have a ResizePolicy set.
        if( childControl.GetChildCount() > 0 )
        {
          // First pass, Static mappings that are not dependant on parent
          SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( childControl, childLayout, Dimension::WIDTH );
          SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( childControl, childLayout, Dimension::HEIGHT );
        }

        // Second pass, if any mappings were not possible due to parent size dependancies then calculate an exact desired size for child
        if( true == childLayout->IsResizePolicyRequired() ) // No need to test child count as this flag would only be set if control had children.
        {
          // Get last stored width and height specifications for the child
          LayoutLength desiredWidth = childControl.GetProperty<float>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
          LayoutLength desiredHeight = childControl.GetProperty<float>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

          DALI_LOG_INFO( gLogFilter, Debug::General, "LayoutGroup::MeasureChild Initial desired size pre ResizePolicy(%f,%f)\n", desiredWidth.AsInteger(), desiredHeight.AsInteger() );

          childLayout->SetResizePolicyRequired( false ); // clear flag incase in case of changes before next Measure
          SizeNegotiationMapper::GetSizeofChildForParentDependentResizePolicy( childControl, widthMeasureSpec, heightMeasureSpec, desiredWidth, desiredHeight );

          // Parent dependant ResizePolicies become exact sizes so are now set on the child before it's measured.
          childControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, desiredWidth.AsInteger() );
          childControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, desiredHeight.AsInteger()  );

          DALI_LOG_INFO( gLogFilter, Debug::General, " LayoutGroup::OnMeasure ResizePolicy Required resulting size(%f,%f)\n",  desiredWidth.AsInteger(), desiredHeight.AsInteger() );
        }

        // Get size of child
        MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
        LayoutLength childWidth = childLayout->GetMeasuredWidth();
        LayoutLength childHeight = childLayout->GetMeasuredHeight();

        Extents childMargin = childLayout->GetMargin();
        DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LayoutGroup::OnMeasure child " << childControl.GetName().c_str() << " width[" << childWidth << "] height[" << childHeight << "]\n" );

        layoutWidth = std::max( layoutWidth, childWidth + childMargin.start + childMargin.end );
        layoutHeight = std::max( layoutHeight, childHeight + childMargin.top + childMargin.bottom );
        DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LayoutGroup::OnMeasure calculated child width[" << layoutWidth << "] height[" << layoutHeight << "]\n" );
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::OnMeasure Not a layout\n" );
      }
    }

    Extents padding = GetPadding();
    layoutWidth += padding.start + padding.end;
    layoutHeight += padding.top + padding.bottom;
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::OnMeasure Getting default size as a leaf\n" );
    // LayoutGroup does not contain any children so must be a leaf
    layoutWidth = GetDefaultSize( GetSuggestedMinimumWidth(), widthMeasureSpec );
    layoutHeight = GetDefaultSize( GetSuggestedMinimumHeight(), heightMeasureSpec );
  }

  // Can't exceed specified width
  if( widthMode == MeasureSpec::Mode::EXACTLY )
  {
    exactWidth = true;
  }
  else if ( widthMode == MeasureSpec::Mode::AT_MOST )
  {
    layoutWidth = std::min( layoutWidth, widthSpecSize );
  }

  // Can't exceed specified height
  if( heightMode == MeasureSpec::Mode::EXACTLY )
  {
    exactHeight = true;
  }
  else if ( heightMode == MeasureSpec::Mode::AT_MOST )
  {
    layoutHeight = std::min( layoutHeight, heightSpecSize );
  }

  layoutWidth = std::max( layoutWidth, GetSuggestedMinimumWidth() );
  layoutHeight = std::max( layoutHeight, GetSuggestedMinimumHeight() );

  if( exactWidth )
  {
    layoutWidth = widthSpecSize;
  }

  if( exactHeight )
  {
    layoutHeight = heightSpecSize;
  }

  DALI_LOG_STREAM( gLogFilter, Debug::General, "LayoutGroup::OnMeasure Measured size(" << layoutWidth << "," << layoutHeight << ") for : " << Actor::DownCast(GetOwner()).GetName() << " \n" );
  SetMeasuredDimensions( MeasuredSize( layoutWidth ), MeasuredSize( layoutHeight ) );
}

void LayoutGroup::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  auto count = GetChildCount();

  DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LayoutGroup OnLayout owner:" << ( ( Toolkit::Control::DownCast(GetOwner())) ? Toolkit::Control::DownCast(GetOwner()).GetName() : "invalid" )  << " childCount:" << count );

  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {

      auto childOwner = childLayout->GetOwner();
      LayoutLength childWidth = childLayout->GetMeasuredWidth();
      LayoutLength childHeight = childLayout->GetMeasuredHeight();
      Extents childMargin = childLayout->GetMargin();
      auto control = Toolkit::Control::DownCast( childOwner );
      Extents padding = GetPadding();

      auto childPosition = control.GetProperty< Vector3 >( Actor::Property::POSITION );
      auto anchorPoint = control.GetProperty< Vector3 >( Actor::Property::ANCHOR_POINT );

      DALI_LOG_STREAM( gLogFilter, Debug::General, "LayoutGroup::OnLayout child[" << control.GetName() <<
                       "] position(" << childPosition << ") child width[" << childWidth << "] height[" << childHeight << "]\n" );

      // Margin and Padding only supported when child anchor point is TOP_LEFT.
      int paddingAndMarginOffsetX = ( AnchorPoint::TOP_LEFT == anchorPoint ) ? ( padding.top + childMargin.top ) : 0;
      int paddingAndMarginOffsetY = ( AnchorPoint::TOP_LEFT == anchorPoint ) ? ( padding.start + childMargin.start ) : 0;
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutGroup::OnLayout paddingMargin offset(%d,%d)\n", paddingAndMarginOffsetX, paddingAndMarginOffsetY );

      LayoutLength childLeft = childPosition.x + paddingAndMarginOffsetX;
      LayoutLength childTop = childPosition.y + paddingAndMarginOffsetY;

      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
    }
  }
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
